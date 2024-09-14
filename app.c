//aplication.c
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include "heap.h"
#include "beSafe.h"


#define SLAVES_AMOUNT 5
#define FILES_PER_SLAVE 10
#define CHILD_PATH "./slave"
#define SLAVE_ARGV {CHILD_PATH,NULL}
#define SLAVE_ENVP {NULL}
#define SEPARATOR '\n'

typedef struct 
{
	int * pipe_fd_write;		//File descriptors from pipes from each slave. Its seen from the perspective
	int * pipe_fd_read;		// of the master (where master writes and reads)

	int total_slaves;

	char ** files;
	int total_files;
	int file_idx;

} slave_monitor;




static void getSlaves();

static void closePipes(slave_monitor * monitor);

static void getOneSlave(slave_monitor * monitor,int slave_position);

static int getMaxFD(int * fd_array, int fd_amount);

static void readFromSlaves(slave_monitor * monitor,char * buff);

static slave_monitor * startSlaveMonitor(const int files_amount,char * files[]);

static void assingToSlave(slave_monitor * monitor, int slave_position);

static int canAssign(slave_monitor * monitor);

static void writeSlaveOutput(char * str);



int main(int argc, char * argv[])	{

	int files_amount = argc - 1;

	slave_monitor * monitor= startSlaveMonitor(files_amount,argv+1);

	getSlaves(monitor);
	char buff[1000];

	readFromSlaves(monitor,buff);
	closePipes(monitor);
	return 0;
}




static void getSlaves(slave_monitor * monitor)	{

	for(int i=0; i< monitor->total_slaves;i++)	{
		getOneSlave(monitor,i);
	}

}



static void getOneSlave(slave_monitor * monitor,int slave_position)	{
	
	char * child_argv[] = SLAVE_ARGV;					
	char * child_envp[] = SLAVE_ENVP;
	int * pipe_read=safePipeD();				//where master will read
	int * pipe_write= safePipeD();				//where master will write

	monitor->pipe_fd_read[slave_position]=pipe_read[0];
	monitor->pipe_fd_write[slave_position]=pipe_write[1];

	pid_t pid= safeFork();
	
	if(pid==0)	{								//Slave process


		dup2(pipe_write[0],STDIN_FILENO);		//Change pipe filedescriptors to STDIN and STDOUT
		dup2(pipe_read[1],STDOUT_FILENO);		//TO DO: @MatiasColeur hacer safeDup2

		close(pipe_write[0]);					//Close pipe filedescriptors that wont be used anymore
		close(pipe_read[1]);
		
		close(pipe_read[0]);					//Close pipe filedescriptors that child wont use
		close(pipe_write[1]);
		
		safeExecve(CHILD_PATH,child_argv,child_envp);
	
	}
	else{										//App process
		
		close(pipe_read[1]);
		close(pipe_write[0]);					//Close pipe filedescriptors that wont be used
		
		if(canAssign(monitor)){

			assingToSlave(monitor, slave_position);

		}

	}
}


static void closePipes(slave_monitor * monitor)	{

	for(int i=0; i<monitor->total_slaves;i++){
		close(monitor->pipe_fd_write[i]);
		close(monitor->pipe_fd_read[i]);
	}

}

static void readFromSlaves(slave_monitor * monitor,char * buff){

	fd_set read_fd;
	FD_ZERO(&read_fd);

	for(int i=0; i< monitor->total_slaves; i++){

		FD_SET(monitor->pipe_fd_read[i],&read_fd);
		
	}

	int fd_available;
	int maxFD= getMaxFD(monitor->pipe_fd_read,monitor->total_slaves);

	struct timeval timeout;  // Timeout for select
	timeout.tv_sec = 2;
    timeout.tv_usec = 0;

	int files_shown=0;
	while(files_shown < monitor->total_files){		
		fd_available = select(maxFD+1,&read_fd,NULL,NULL,&timeout);	//TO DO: @MatiasColeur hacer safeSelect


		if(fd_available > 0){
			
			for(int i=0; i < monitor->total_slaves;i++)	{
				
				
				if(FD_ISSET(monitor->pipe_fd_read[i],&read_fd)){

					read(monitor->pipe_fd_read[i],buff,1000);
					printf("%s\n",buff);
					//writeSlaveOutput(buff);

					if(canAssign(monitor))	{

						assingToSlave(monitor,i);

					}
					files_shown++;
				}

			}

		}
	}
}


static int getMaxFD(int * fd_array, int fd_amount)	{

	int to_ret=0;

	for(int i=0;i<fd_amount;i++)	{

		if(fd_array[i]> to_ret){
			to_ret= fd_array[i];
		}

	}

	return to_ret;
}


static slave_monitor * startSlaveMonitor(const int files_amount,char * files[]){
	slave_monitor * monitor= (slave_monitor *) malloc(sizeof(slave_monitor));		//TO DO: @MatiasColeur hacer safeMalloc

	if(files_amount > (SLAVES_AMOUNT * FILES_PER_SLAVE))	{
		
		monitor->total_slaves= files_amount / FILES_PER_SLAVE;

	}
	else if(files_amount< SLAVES_AMOUNT){
		monitor->total_slaves= files_amount;
	}
	else{
		monitor->total_slaves= SLAVES_AMOUNT;
	}

	monitor->pipe_fd_read = (int *) malloc(sizeof(int) * monitor->total_slaves);
	monitor->pipe_fd_write = (int *) malloc(sizeof(int) * monitor->total_slaves);

	monitor->total_files=files_amount;
	monitor->files=files;
	monitor->file_idx=0;
	return monitor;
}


static void assingToSlave(slave_monitor * monitor, int slave_position)	{

	errorManagement(! canAssign(monitor), "All files already assigned");


	char * file= monitor->files[monitor->file_idx];

	int len= strlen(file);
	

	char * new_file= (char *) malloc(len+2);
	strcpy(new_file,file);
	new_file[len] = SEPARATOR;

	new_file[len+1] = '\0';		


	write(monitor->pipe_fd_write[slave_position],new_file,len+1);
	// write(monitor->pipe_fd_write[slave_position],"files/prueba.txt\n",strlen("files/prueba.txt\n"));

	monitor->file_idx+=1;

}

static int canAssign(slave_monitor * monitor)	{
	return monitor->total_files > monitor->file_idx;
}


static void writeSlaveOutput(char * str)	{

	printf("%s\n",str);

}