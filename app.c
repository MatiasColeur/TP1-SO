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





int main(int argc, char * argv[])	{

	slave_monitor * monitor= getSlaves(10);
	sleep(5);
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
		dup2(pipe_read[1],STDOUT_FILENO);

		close(pipe_write[0]);					//Close pipe filedescriptors that wont be used anymore
		close(pipe_read[1]);
		
		close(pipe_read[0]);					//Close pipe filedescriptors that child wont use
		close(pipe_write[1]);
		safeExecve(CHILD_PATH,child_argv,child_envp);
	
	}
	else{										//App process
		monitor->slaves[slave_position]= pid;
		close(pipe_write[0]);					//Close pipe filedescriptors that wont be used
		close(pipe_read[1]);
	}
}


static void closePipes(slave_monitor * monitor)	{

	for(int i=0; i<monitor->total_slaves;i++){
		close(monitor->pipe_fd_write[i]);
		close(monitor->pipe_fd_read[i]);
	}

}