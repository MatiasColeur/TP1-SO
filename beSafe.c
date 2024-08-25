//beSafe.c

#include "beSafe.h"

// Number of positions added while being reallocated in heap memory. Must be >0:

#define BLOCK 5


//Structure for heap management:

typedef struct tHeap    {

	void ** inUse;
	unsigned int using;
	unsigned int size;

} * tHeap;

//For security there always is an instance of a tHeap with pointers to every dynamic variable. 
//So if an error ocurs all are accesible to free:

static tHeap heap = calloc(1, sizeof(struct tHeap));

static void newHeapVariable(void * new)	{
	
	if(heap->using == size)	{
		
		heap = realloc(heap, size + BLOCK*sizeof(void*));
		
	//Assuming BLOCK > 0, if heap = NULL there is an error:
		errorManagement(heap == NULL, "Memory reallocated failed");		
	}

	heap->inUse[using++] = (void *) new;
}


//freeHeap(): free all dynamic variables saved in the static variable @heap->inUse[].
//	Then free @heap->inUse structure. Then free @heap.

static void freeHeap()  {

	int i=0;      
	
	while(i<heap->size)      {
	
		free(heap->inUse[i++]);
	}

	free(heap->inUse);
	free(heap);
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

	int returnValue = pipe(pipefd);
	errorManagement(returnValue == -1, "Pipe failed");
}



int * safePipeD(void)	{

	int * pipefd = malloc( 2 * sizeof(int) );
	
	errorManagement(pipefd == NULL, "Memory allocated failed");

	newHeapVariable(pipefd);

	safePipe(pipefd);
	
	return pipefd;
}

