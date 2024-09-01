//beSafe.h

#ifndef BE_SAFE_H
#define	BE_SAFE_H

	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <errno.h>
	#include <sys/wait.h>
	
	#define TRUE 1
	#define FALSE !TRUE


//errorManagement(): checks if @condition is true (non-zero) and handlethe error redirecting @errorMessage to stderr exiting with errno value.

//	@condition: Result of a comparation with an error return value.
//	@errorMessage: String of error message, this will be argument of perror().

//	Call example: errorManagement(fork() == -1, "Fork failed"); //When calling fork(), -1 is returned if there is an error.

	void errorManagement(int condition, const char * errorMessage);



//safeMalloc(): Allocates @size bytes and returns a pointer to the allocated memory. The memory is not initialized. If size is 0, then safeMalloc() returns a unique pointer value that can later be successfully passed to free(). Handling all errors.

	void * safeMalloc(size_t size);



//safeCalloc():  allocates memory for an array of @nmemb elements of @size bytes each and returns a pointer to the allocated memory. The memory is set to zero. If @nmemb or @size is 0, then safeCalloc() returns a unique pointer value that can later be successfully passed to free(). Handling errors.

	void * safeCalloc(size_t nmemb, size_t size);



//realloc(): If @ptr is NULL, then the call is equivalent to safeMalloc(@size), for all values of @size.

//	If @size is equal to zero, and @ptr is not NULL, then the call is equivalent to free(@ptr).

//     Unless @ptr is NULL, it must have been returned by an earlier call to safeMalloc() or related functions. If the area pointed to was moved, a free(ptr) is done.

	void * safeRealloc(void * ptr, size_t size);



//safeFork(void): calls fork() wrapping the error management.

//	@return pid of the current process: 
//		0 means child process.
//		>0 means parent process, in this case @return=child pid.

	pid_t safeFork(void);



//safeWait(): calls wait(@wstatus) wrapping error management.

//See wait() linux manual for more information about its parameters.

//	@return the process ID of the terminated child.

	pid_t safeWait(int * wstatus);



//safeWaitpid(): calls waitpid() wrapping the error management. 

//See waitpid() linux manual for more information about its parameters.

//	@return the process ID of the child whose state has changed. 
//	If WNOHANG was specified and one or more child(ren) specified by pid exist, but have not yet changed state, then 0 is returned. See waitpid() manual for more information.

	pid_t safeWaitpid(pid_t pid, int * wstatus, int options);	



//safePipe(): calls pipe() wrapping the error management.

//	@pipefd: is a 2 slots vector of integers. Is used to return two files descriptors referring to the ends of the pipe as follows:

//		pipefd[0] refers to the read end of the pipe. 
//		pipefd[1] refers to the write end of the pipe. 

	void safePipe(int pipefd[]);



//safePipeD(): same as safePipe() without a static vector as argument.

//	@return instance of pipefd. See safePipe().

	int * safePipeD(void);



//freeHeap(): free all dynamic variables saved in an internal structure with pointersto every dynamic object instantiated from this library. Optional use.

	void freeHeap();



//killHeapMonitor(): Kills the montitor. This should be use at the end of the program.

	void killHeapMonitor();

#endif
