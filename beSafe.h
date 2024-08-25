//beSafe.h
//version 0.5.0

#ifndef BE_SAFE_H
#define	BE_SAFE_H

	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <errno.h>



//errorManagement(): checks if @condition is true (non-zero) and handlethe error redirecting @errorMessage to stderr exiting with errno value.

	void errorManagement(int condition, const char * errorMessage);



//safeFork(void): calls fork() wrapping the error management.

//	@return pid of the current process: 
//		0 means child process.
//		>0 means parent process, in this case @return=child pid.

	pid_t safeFork(void);



//safePipe(): calls pipe() wrapping the error management.

//	@pipefd: is a 2 slots vector of integers. Is used to return two files descriptors referring to the ends of the pipe as follows:

//		pipefd[0] refers to the read end of the pipe. 
//		pipefd[1] refers to the write end of the pipe. 

	void safePipe(int pipefd[]);



//safePipeD(): same as safePipe() without a static vector as argument.

//	@return instance of pipefd. See safePipe().

	int * safePipeD(void);

	#endif
