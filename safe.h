#ifndef SAFE_H
#define	SAFE_H

	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <errno.h>

//safeFork(void): calls fork() wrapping the error management.

//	@return pid of the current process: 
//		0 means child process.
//		>0 means parent process, in this case @return=child pid.

	pid_t safeFork(void);

#endif
