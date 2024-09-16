// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

//beSafe.c

#include "beSafe.h"


void errorManagement(int condition, const char * errorMessage)	{

	if(condition)	{
	
		freeHeap();

		killHeapMonitor();

		perror(errorMessage);
		exit(EXIT_FAILURE);
	}
}



pid_t safeFork(void)	{

	pid_t pid = fork();

	errorManagement(pid == -1, "Fork failed");

	return pid;
}



pid_t safeWaitpid(pid_t pid, int * wstatus, int options)	{

	pid_t w = waitpid(pid, wstatus, options);

	errorManagement(w == -1, "Wait failed");

	return w;

}



pid_t safeWait(int * wstatus)	{

//wait(&status) == waitpid(-1, wstatus, 0);

	return safeWaitpid(-1, wstatus, 0);
}



void safePipe(int pipefd[])	{

	int returnValue = pipe(pipefd);
	errorManagement(returnValue == -1, "Pipe failed");
}



int * safePipeD(void)	{

	int * pipefd = (int *) safeMalloc(2 * sizeof(int));

	safePipe(pipefd);
	
	return pipefd;
}



void safeExecve(char * path, char * argv[],char * envp[])	{

	int returnValue= execve(path,argv,envp);
	errorManagement(returnValue== -1, "Execve failed");
}



void safeDup2(int old_fd,int new_fd)	{

	int returnVaue= dup2(old_fd,new_fd);

	errorManagement(returnVaue == -1, "Dup2 failed");
	close(old_fd);
}



int safeOpen(char * path, int flags, mode_t mode)	{

	int fd = open(path,flags,mode);
	errorManagement(fd== -1, "Open failed");

	return fd;
}