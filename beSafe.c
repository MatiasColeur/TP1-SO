//beSafe.c

#include "beSafe.h"

// Number of positions added while being reallocated in heap memory. Must be >0:

#define BLOCK 5


//Structure for heap management:

struct tHeapMonitor	{

	void ** array;
	unsigned int using;
	unsigned int size;

};

//For security there always is an instance of a heapMonitor with an array with pointers to every dynamic object. So if an error ocurs all are accesible to free:

static struct tHeapMonitor heapMonitor =	{

//.array will be fill when newHeapVariable() is called:
	.array = NULL,
	.using = 0,
	.size = 0
};

static void newHeapVariable(void * new)	{
	
	if(heapMonitor.using == heapMonitor.size)	{
		
		heapMonitor.array = realloc( heapMonitor.array, 
			heapMonitor.size + BLOCK * sizeof(void*) );
		
	//Assuming BLOCK > 0, if heap = NULL there is an error:

		errorManagement(heapMonitor.array == NULL, 
			"Memory reallocated failed");		

		heapMonitor.size+= BLOCK;
	}

	heapMonitor.array[heapMonitor.using++] = (void *) new;
}


//freeHeap(): free all dynamic variables saved in the static variable @heap->array[].
//	Then free @heap->array structure. Then free @heap.

static void freeHeap()  {

	int i=0;      

//free every object using heap memory:

	while(i<heapMonitor.size)      {
	
		free(heapMonitor.array[i++]);
	}

//Then free the array field in heapMonitor. If .array = NULL nothing happens:

	free(heapMonitor.array);
}



void errorManagement(int condition, const char * errorMessage)	{

	if(condition)	{
	
	//Free all the dynamic variables currently in use before exiting:

		freeHeap();

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

