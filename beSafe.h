//beSafe.h

#ifndef BE_SAFE_H
#define	BE_SAFE_H

	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <errno.h>
	#include <sys/wait.h>
	#include "heap.h"
	#include <fcntl.h>

	#define TRUE 1 
	#define FALSE !TRUE


//WARNING! This library includes and uses the heap.h library; for heap memory variables, use only heap.h functions.



//-----------------------Errors--------------------------------------------------->>



//errorManagement(): checks if @condition is true (non-zero) and handlethe error redirecting @errorMessage to stderr exiting with errno value.

//	@condition: Result of a comparation with an error return value.
//	@errorMessage: String of error message, this will be argument of perror().

//	Call example: errorManagement(fork() == -1, "Fork failed"); //When calling fork(), -1 is returned if there is an error.

	void errorManagement(int condition, const char * errorMessage);



//-----------------------Process--------------------------------------------------->>



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



//safeExecve(): calls execve() wrapping the error management

//See execve() linux manual for more information about its parameters.

	void safeExecve(char * path, char * argv[],char * envp[]);



//safeDup2: calls dup2() wrapping the error management and closes the old file descriptor

//See dup2() linux manual for more information about its parameters and functionality

	void safeDup2(int old_fd, int new_fd);



//safeOpen: calls open() wrapping the error management and returns the file descriptor

//	@return file descriptor of the file opened

//See open() linux manual for more information about its parameters and functionality

	int safeOpen(char * path, int flags, mode_t mode);


//safeOpen: calls close() wrapping the error management

// @fd is the file descriptor which would be close

void safeClose(int fd);



//-----------------------General--------------------------------------------------->>



static inline int isNull(const void * data)	{

    return data == NULL;
}



static inline int notNull(const void * data)	{

	return !isNull(data);
}



#endif
