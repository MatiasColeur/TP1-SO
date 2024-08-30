//beSafe.c

#include "beSafe.h"

// Number of positions added while being reallocated in heap memory. Must be >0:

#define BLOCK 5


void * safeAlloc(void * ptr);


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

//newHeapVariable(): save a pointer to the new heap object in an internal structure to centralize them. 

//	@new: is a pointer to the new object to save.

static void newHeapVariable(void * new)	{
	
	if(heapMonitor.using == heapMonitor.size)	{
		
		heapMonitor.array = safeAlloc( realloc(heapMonitor.array,
			heapMonitor.size + BLOCK * sizeof(void*)) );		
		heapMonitor.size+= BLOCK;
	}

	heapMonitor.array[heapMonitor.using++] = (void *) new;
}



void freeHeap()  {

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



void * safeAlloc(void * ptr)	{

	errorManagement(ptr == NULL, "Memory allocated failed");

	newHeapVariable(ptr);

	return ptr;

}



void * safeMalloc(size_t size)	{

	return safeAlloc(malloc(size));
}



void * safeCalloc(size_t nmemb, size_t size)	{
	
	return safeAlloc(calloc(nmemb, size));
}



void * safeRealloc(void * ptr, size_t size)	{

	return safeAlloc(realloc(ptr, size));
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

