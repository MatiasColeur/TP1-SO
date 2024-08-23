#include "safe.h"

pid_t safeFork(void)	{


//Fork:
	pid_t pid = fork();

//pid = -1 means error:

	if( pid == -1 )	{

		perror("fork");
		exit(EXIT_FAILURE);
	}

//No error, then return pid:

	return pid;

}
