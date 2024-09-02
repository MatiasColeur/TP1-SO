//beSafe.c

#include "beSafe.h"


void errorManagement(int condition, const char * errorMessage)	{

	if(condition)	{
	
		freeHeap();

		killHeapMonitor();

		perror(errorMessage);
		exit(EXIT_FAILURE);
	}
}



pid_t safeFork(void)	{

	pid_t pid = fork();

	errorManagement(pid == -1, "Fork failed");

	return pid;
}



pid_t safeWaitpid(pid_t pid, int * wstatus, int options)	{

	pid_t w = waitpid(pid, wstatus, options);

	errorManagement(w == -1, "Wait failed");

	return w;

}



pid_t safeWait(int * wstatus)	{

//wait(&status) == waitpid(-1, &wstatus, 0);

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

