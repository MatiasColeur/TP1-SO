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
	int * pipe_fd_in[SLAVES_AMOUNT];		//File descriptors from pipes from each slave [read,write]
	int * pipe_fd_out[SLAVES_AMOUNT];

	pid_t slaves[SLAVES_AMOUNT];

	int total_slaves;

} slave_monitor;




static slave_monitor * getSlaves(const int slaves_amount);

static void closePipes(slave_monitor * monitor);

static void getOneSlave(slave_monitor * monitor,int slave_position);





int main(int argc, char * argv[])	{

	slave_monitor * monitor= getSlaves(10);

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


static void closePipes(slave_monitor * monitor)	{

	for(int i=0; i<monitor->total_slaves;i++){
		close(monitor->pipe_fd_in[i][0]);
		close(monitor->pipe_fd_in[i][1]);
		close(monitor->pipe_fd_out[i][0]);
		close(monitor->pipe_fd_out[i][1]);
	}

}

static void getOneSlave(slave_monitor * monitor,int slave_position)	{
	
	char * child_argv[] = SLAVE_ARGV;					
	char * child_envp[] = SLAVE_ENVP;
	monitor->pipe_fd_in[slave_position] = safePipeD();				
	monitor->pipe_fd_out[slave_position]= safePipeD();

	pid_t pid= safeFork();
	
	if(pid==0)	{								//Slave process


		dup2(monitor->pipe_fd_in[slave_position][0],STDIN_FILENO);		//Change pipe filedescriptors to STDIN and STDOUT
		dup2(monitor->pipe_fd_out[slave_position][1],STDOUT_FILENO);

		close(monitor->pipe_fd_in[slave_position][0]);					//Close pipe filedescriptors that wont be use anymore
		close(monitor->pipe_fd_out[slave_position][1]);

		safeExecve(CHILD_PATH,child_argv,child_envp);
	
	}
	else{										//App process
		monitor->slaves[slave_position]= pid;
	}
}