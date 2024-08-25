//beSafe.c

#include "beSafe.h"

#define BLOCK 5


//Structure for heap management:

typedef struct tHeap    {

	void ** inUse;
	unsigned int size;

}  tHeap;

//For security there always is an instance of a tHeap with pointers to every dynamic variable. 
//So if an error ocurs all are accesible to free:

static tHeap heap;

//freeHeap(): free all dynamic variables saved in the static variable @heap.inUse[].
//	Then free @heap.inUse structure.

static void freeHeap()  {

	int i=0;      
	
	while(i<heap.size)      {
	
		free(heap.inUse[i++]);
	}

	free(heap.inUse);
}



void errorManagement(int errorConditionResult, const char * errorMessage)	{

	if(condition)	{
	
	//Free all the dynamic variables currently in use before exiting:
		freeHeap();

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



int * safePipeD(void)	{

	int * pipefd = malloc( 2 * sizeof(int) );
	
	errorManagement(pipefd == NULL, "Memory allocated failed");

	safePipe(pipefd);
	
	return pipefd;
}

