//beSafe.c

#include "beSafe.h"


void errorManagement(int condition, const char * errorMessage)	{

	if(condition)	{
	
	//Free all the dynamic variables currently in use before exiting:

		freeHeap();

	//Free heap monitor:

		killHeapMonitor();

	//Then handle errors as expected:

		perror(errorMessage);
		exit(EXIT_FAILURE);
	}
}



pid_t safeFork(void)	{

//Instantiating fork:

	pid_t pid = fork();

//pid = -1 means error:

	errorManagement(pid == -1, "Fork failed");

//No error, then return pid:

	return pid;
}



pid_t safeWaitpid(pid_t pid, int * wstatus, int options)	{

//Wait:
	pid_t w = waitpid(pid, wstatus, options);

//If waitpid returns -1 means error:

	errorManagement(w == -1, "Wait failed");

//It is fine:

	return w;

}



pid_t safeWait(int * wstatus)	{

//From wait() manual: wait(&status) == waitpid(-1, &wstatus, 0);

	return safeWaitpid(-1, wstatus, 0);
}



void safePipe(int pipefd[])	{

	int returnValue = pipe(pipefd);
	errorManagement(returnValue == -1, "Pipe failed");
}



int * safePipeD(void)	{

	int * pipefd = (int *) safeMalloc( 2*sizeof(int) );

	safePipe(pipefd);
	
	return pipefd;
}

