//beSafe.c

#include "beSafe.h"

void errorManagement(int errorConditionResult, const char * errorMessage)	{

	if(condition)	{
	
		perror(errorMessage);
		exit(EXIT_FAILURE);
	}
}

pid_t safeFork(void)	{

//Fork:instantiating
	pid_t pid = fork();

//pid = -1 means error:

	errorManagement(pid == -1, "Fork failed");

//No error, then return pid:

	return pid;
}

void safePipe(int pipefd[])	{

	errorManagement(pipe(pipefd) == -1, "Pipe failed");
}

//TO DO: check how to free before exit.

int * safePipeD(void)	{

	int * pipefd = malloc( 2 * sizeof(int));
	
	errorManagement(pipefd == NULL, "Memory allocated failed");

	safePipe(pipefd);
	
	return pipefd;
}

