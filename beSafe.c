//beSafe.c

#include "beSafe.h"

// Number of positions added while being reallocated in heap memory. Must be >0:

#define BLOCK 5


//Structure for heap management:

struct tHeapMonitor	{

	void ** array;
	unsigned int using;
	unsigned int size;
	char alive;
};

//For security there always is an instance of a heapMonitor with an array with pointers to every dynamic object. So if an error ocurs all are accesible to free:

static struct tHeapMonitor heapMonitor =	{

//.array will be fill when newHeapVariable() is called:
	.array = NULL,
	.using = 0,
	.size = 0,
	.alive = TRUE
};

//newHeapVariable(): save a pointer to the new heap object in an internal structure to centralize them. If @new is repeated, nothing happen.

//	@new: is a pointer to the new object to save.

static void newHeapVariable(void * new)	{

	for(int i=0; i<heapMonitor.using; ++i)	{

		if(heapMonitor.array[i] == new)
			return;
	}

	
	if(heapMonitor.using == heapMonitor.size)	{
		
		heapMonitor.array = realloc(heapMonitor.array, 
			heapMonitor.size + BLOCK * sizeof(void*));

		errorManagement(heapMonitor.array == NULL, "Memory allocated failed");	
		
		heapMonitor.size+= BLOCK;
	}

	heapMonitor.array[heapMonitor.using++] = (void *) new;
}



static void removeFromHeapList(void * p)	{

	for(int i=0; i<heapMonitor.using; ++i)	{

		if(heapMonitor.array[i]==p)	{

			heapMonitor.array[i] = heapMonitor.array[heapMonitor.using-1];
			heapMonitor.using--;
		}
	}

}

/*int main()	{

	int * p1 = safeMalloc(sizeof(int));
	int * p2 = safeCalloc(1, sizeof(int));
	p1 = safeRealloc(p1, 2 * sizeof(int));
	
	void * pV = (void *) p1;
	
	freeHeap();
	return 0;


}*/


void freeHeap()  {

	int i=0;      

//free every object using heap memory:

	while(i<heapMonitor.using)      {
	
		free(heapMonitor.array[i++]);
	}

//heapMonitor now is empty (It will allocate new memory for it if new heap variables are used):

	heapMonitor.size = 0;
	heapMonitor.using = 0;
	heapMonitor.array = NULL;
	free(heapMonitor.array);
}



void killHeapMonitor()	{

//If heapMonitor is alive then kill it:

	if(heapMonitor.alive == TRUE )	{

	//Free the array field in heapMonitor. If .array = NULL nothing happens:
	
		free(heapMonitor.array);
		heapMonitor.alive = FALSE;
	}
}



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



static void * safeAlloc(void * ptr)	{

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

//Needed because realloc will free the old heap memory address so freeHeap() will try to free already free memory. Deleting the old memory address and adding the new one everyone is safe:

	removeFromHeapList(ptr);
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

