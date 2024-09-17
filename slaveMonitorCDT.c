// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

//slaveMonitorCDT.c

#include "slaveMonitorADT.h"


struct slaveMonitorCDT  {

	int * pipe_fd_write;	//File descriptors from pipes from each slave. Its seen from the perspective
	int * pipe_fd_read;     // of the master (where master writes and reads)

	int total_slaves;
	int slaves_started;

	char ** files;
	int total_files;
	int file_idx;
	int start_files;

};


static void writeSlaveOutput(char * str, sharedADT shm)   {
	
	int len = strlen(str) + 1;

	writeShm(shm, str, len);

	int outputFd = safeOpen(OUTPUT_FILE_NAME, O_WRONLY | O_CREAT | O_APPEND, 0666);
	dprintf(outputFd,"%s",str);

	safeClose(outputFd);
}



void getSlaves(slaveMonitorADT monitor) {

	for(int i=0; i< monitor->total_slaves; i++)	{
		
	    getOneSlave(monitor, i);
	}
}



void getOneSlave(slaveMonitorADT monitor, int slave_position)	{
	
	char * child_argv[] = SLAVE_ARGV;					
	char * child_envp[] = SLAVE_ENVP;

	int * pipe_read = safePipeD();				//where master will read
	int * pipe_write = safePipeD();				//where master will write

	pid_t pid = safeFork();
	
	if(pid == 0)  {                   //Slave process
        								
		closePipes(monitor);

		safeDup2(pipe_write[0], STDIN_FILENO);		//Change pipe filedescriptors to STDIN and STDOUT and close the old_fd
		safeDup2(pipe_read[1], STDOUT_FILENO);		
		
		safeClose(pipe_read[0]);					//Close pipe filedescriptors that child wont use
		safeClose(pipe_write[1]);
		
		safeExecve(CHILD_PATH, child_argv, child_envp);
	}
	else    {										//App process
		
		safeClose(pipe_read[1]);
		safeClose(pipe_write[0]);					//Close pipe filedescriptors that wont be used
		
		monitor->pipe_fd_read[slave_position] = pipe_read[0];
		monitor->pipe_fd_write[slave_position] = pipe_write[1];
		monitor->slaves_started++;

		if(canAssign(monitor))  {

			assingToSlave(monitor, slave_position,monitor->start_files);
		}
	}
}



void closePipes(slaveMonitorADT monitor)	{

	for(int i=0; i<monitor->slaves_started; i++)   {
		safeClose(monitor->pipe_fd_write[i]);
		safeClose(monitor->pipe_fd_read[i]);
	}
}



void readFromSlaves(slaveMonitorADT monitor, sharedADT shm)    {
	
	char buff[BUFF_LEN]; 
	fd_set read_fd;

	int fd_available;
	int maxFD= getMaxFD(monitor->pipe_fd_read, monitor->total_slaves);

	int files_shown=0;

	while(files_shown < monitor->total_files)   {

		FD_ZERO(&read_fd);

		for(int i=0; i<monitor->total_slaves; i++) {

			FD_SET(monitor->pipe_fd_read[i], &read_fd);
		}

		fd_available = select(maxFD+1, &read_fd, NULL, NULL, NULL);	

		if(fd_available > 0)    {
			
			for(int i=0; i < monitor->total_slaves; i++) {
				
				if(FD_ISSET(monitor->pipe_fd_read[i], &read_fd)) {
					
					memset(buff, 0, sizeof(buff));

					read(monitor->pipe_fd_read[i], buff, BUFF_LEN); 
					writeSlaveOutput(buff, shm);	//passing the shared memory as argument

					if(canAssign(monitor))  {

						assingToSlave(monitor, i,1);
                	
					}

					files_shown++;
				}
			}
		}
	}

//End signal to view:

	char end = EOF;
	writeShm(shm, &end, 1);

}



int getMaxFD(int * fd_array, int fd_amount) {

	int to_ret = 0;

	for(int i=0; i<fd_amount; i++)  {

		if(fd_array[i] > to_ret) {
		
        	to_ret= fd_array[i];
		}
	}

	return to_ret;
}



slaveMonitorADT startSlaveMonitor(const int files_amount, char * files[])    {

	slaveMonitorADT monitor = (slaveMonitorADT) safeMalloc(sizeof(monitor[0]));

	if(files_amount > (SLAVES_MIN * FILES_PER_SLAVE))	{
		
		monitor->total_slaves= files_amount / FILES_PER_SLAVE;
		monitor->start_files= START_FILES;
	}
	else if(files_amount< SLAVES_MIN)    {

		monitor->total_slaves= files_amount;
		monitor->start_files= 1;
	}
	else    {

		monitor->total_slaves= SLAVES_MIN;
		monitor->start_files = 1;
	}

	monitor->pipe_fd_read = (int *) safeMalloc(sizeof(int) * monitor->total_slaves);
	monitor->pipe_fd_write = (int *) safeMalloc(sizeof(int) * monitor->total_slaves);

	monitor->total_files = files_amount;
	monitor->files = files;
	monitor->file_idx = 0;
	monitor->slaves_started=0;
	return monitor;
}



void assingToSlave(slaveMonitorADT monitor, int slave_position,int files_amount) {
	
	for(int i=0; i<files_amount; i++){
		errorManagement(! canAssign(monitor), "All files already assigned");

		char * file= monitor->files[monitor->file_idx];

		int len = strlen(file);
		
		char * new_file = (char *) safeMalloc(sizeof(char) * (len+2));
		strcpy(new_file, file);
		new_file[len] = SEPARATOR;

		new_file[len+1] = '\0';		

		write(monitor->pipe_fd_write[slave_position], new_file, len+1);

		monitor->file_idx += 1;
	}
}



int canAssign(slaveMonitorADT monitor)  {

	return monitor->total_files > monitor->file_idx;
}



