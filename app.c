// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//aplication.c

#include "slaveMonitorADT.h"


#define SHBUFFER_NAME "md5\n"
#define SHBUFFER_SIZE 1024


int main(int argc, char * argv[])	{

	sharedADT shm = createShm(SHBUFFER_NAME, SHBUFFER_SIZE);
	dprintf(STDOUT_FILENO,"%s", SHBUFFER_NAME);

	wait4otherProcess();

	int files_amount = argc - 1;

	slaveMonitorADT monitor= startSlaveMonitor(files_amount,argv+1);

	getSlaves(monitor);

 	readFromSlaves(monitor, shm);
	closePipes(monitor);

	killShm(shm);
	killHeapMonitor();

	return 0;
}


