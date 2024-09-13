//aplication.c
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include "heap.h"
#include "beSafe.h"


#define SLAVES_AMOUNT 5
#define CHILD_PATH "./slave"
#define SLAVE_ARGV {CHILD_PATH,NULL}
#define SLAVE_ENVP {NULL}

typedef struct 
{
	int pipe_fd_write[SLAVES_AMOUNT];		//File descriptors from pipes from each slave. Its seen from the perspective
	int pipe_fd_read[SLAVES_AMOUNT];		// of the master (where master writes and reads)

	pid_t slaves[SLAVES_AMOUNT];

	int total_slaves;

} slave_monitor;




static slave_monitor * getSlaves(const int slaves_amount);

static void closePipes(slave_monitor * monitor);

static void getOneSlave(slave_monitor * monitor,int slave_position);

static int getMaxFD(int * fd_array, int fd_amount);

static void readFromSlaves(slave_monitor * monitor,char * buff);

static slave_monitor * startSlaveMonitor(const int files_amount);

static void assingToSlave(slave_monitor * monitor, int slave_position,char * file);

int main(int argc, char * argv[])	{

	slave_monitor * monitor= getSlaves(10);
	char buff[1000];
	readFromSlaves(monitor,buff);
	closePipes(monitor);
	return 0;
}

static slave_monitor * getSlaves(const int files_amount)	{

	slave_monitor * monitor= (slave_monitor *) malloc(sizeof(slave_monitor));
	monitor->total_slaves=SLAVES_AMOUNT;

	for(int i=0; i< monitor->total_slaves;i++)	{
		getOneSlave(monitor,i);
	}

	return monitor;
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
		
		monitor->slaves[slave_position]= pid;
		close(pipe_read[1]);
		close(pipe_write[0]);					//Close pipe filedescriptors that wont be used
		
		char * file= "files/prueba.txt\n";
		// write(pipe_write[1],file,strlen(file));
		assingToSlave(monitor, slave_position,file);

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

	int condition=1;
	
	struct timeval timeout;  // Timeout for select
	timeout.tv_sec = 2;
    timeout.tv_usec = 0;


	while(condition){		
		fd_available = select(maxFD+1,&read_fd,NULL,NULL,&timeout);	//TO DO: @MatiasColeur hacer safeSelect


		if(fd_available > 0){
			
			for(int i=0; i < monitor->total_slaves;i++)	{
				
				
				if(FD_ISSET(monitor->pipe_fd_read[i],&read_fd)){
					read(monitor->pipe_fd_read[i],buff,1000);
				}

			}

		}
		else{
			condition=0;
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

static slave_monitor * startSlaveMonitor(const int files_amount){

	slave_monitor * monitor= (slave_monitor *) malloc(sizeof(slave_monitor));
	monitor->total_slaves=SLAVES_AMOUNT;

	return monitor;
}

static void assingToSlave(slave_monitor * monitor, int slave_position,char * file)	{

	write(monitor->pipe_fd_write[slave_position],file,strlen(file));

}
