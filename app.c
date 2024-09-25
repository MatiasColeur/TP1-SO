//aplication.c

#include "slaveMonitorADT.h"


#define SHBUFFER_NAME "md5"


int main(int argc, char * argv[])	{

	sharedADT shm = createShm(SHBUFFER_NAME, SHBUFFER_SIZE);
	dprintf(STDOUT_FILENO,"%s", SHBUFFER_NAME);
	dprintf(STDOUT_FILENO, "%c", SEPARATOR);

	wait4otherProcess();

	int files_amount = argc - 1;

	slaveMonitorADT monitor= startSlaveMonitor(files_amount,argv+1);

	getSlaves(monitor);

 	readFromSlaves(monitor, shm);
	writeEndShm(shm, EOF);

	closePipes(monitor);
	killShm(shm);
	killHeapMonitor();

	return 0;
}


