//heap.h

#ifndef HEAP_H
#define	HEAP_H

	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <errno.h>
	#include <sys/wait.h>
	#include "beSafe.h"
	
	#define TRUE 1
	#define FALSE !TRUE


//WARNING! This library automatically manage heap memory. 

//DO NOT free variables instantiated with theese functions, otherwise undifined behivouroccurs. 

//Remember killing the monitor at the end of the program (see killHeapMonitor()).



//safeMalloc(): Allocates @size bytes and returns a pointer to the allocated memory. The memory is not initialized. If size is 0, then safeMalloc() returns a unique pointer value that can later be successfully passed to free(). Handling all errors.

	void * safeMalloc(size_t size);



//safeCalloc():  allocates memory for an array of @nmemb elements of @size bytes each and returns a pointer to the allocated memory. The memory is set to zero. If @nmemb or @size is 0, then safeCalloc() returns a unique pointer value that can later be successfully passed to free(). Handling errors.

	void * safeCalloc(size_t nmemb, size_t size);



//realloc(): If @ptr is NULL, then the call is equivalent to safeMalloc(@size), for all values of @size.

//	If @size is equal to zero, and @ptr is not NULL, then the call is equivalent to free(@ptr).

//     Unless @ptr is NULL, it must have been returned by an earlier call to safeMalloc() or related functions. If the area pointed to was moved, a free(ptr) is done.

	void * safeRealloc(void * ptr, size_t size);



//freeHeap(): free all dynamic variables saved in an internal structure with pointers to every dynamic object instantiated from this library. 

	void freeHeap();



//killHeapMonitor(): Kills the montitor. This should be use at the end of the program. It also calls freeHeap() so all the heap variables will be free.

	void killHeapMonitor();

#endif
