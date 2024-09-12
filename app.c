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


static pid_t * getSlaves(const int slaves_amount);


int main(int argc, char * argv[])	{

	pid_t * slaves= getSlaves(10);

	for(int i; i< SLAVES_AMOUNT; i++){

		printf("pid: %d\n",slaves[i]);
		
	}

	return 0;
}

static pid_t * getSlaves(const int files_amount)	{

	int max_slaves= SLAVES_AMOUNT;
	pid_t * slaves;


	if (files_amount< SLAVES_AMOUNT)	{
		
		max_slaves= files_amount;
	
	}

	slaves = (pid_t *) malloc(max_slaves * sizeof(pid_t *));
	char * child_argv[] = SLAVE_ARGV;					
	char * child_envp[] = SLAVE_ENVP;
	int * pipe_fd_in;
	int * pipe_fd_out;
	
	for(int i=0; i<max_slaves;i++){

		pipe_fd_in = safePipeD();				
		pipe_fd_out= safePipeD();

		pid_t pid= safeFork();
		
		if(pid==0)	{								//Slave process


			dup2(pipe_fd_in[0],STDIN_FILENO);		//Change pipe filedescriptors to STDIN and STDOUT
			dup2(pipe_fd_out[1],STDOUT_FILENO);

			close(pipe_fd_in[0]);					//Close pipe filedescriptors that wont be use anymore
			close(pipe_fd_out[1]);

			safeExecve(CHILD_PATH,child_argv,child_envp);
		
		}
		else{										//App process
			slaves[i]= pid;
		}

	}

	return slaves;
}