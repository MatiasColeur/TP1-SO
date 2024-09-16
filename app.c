// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//aplication.c

#include "slaveMonitorADT.h"


#define SHBUFFER_NAME "TP1 SO 2Q 2024\nGrupo 3\nChiatellino Maximo - 63477 mchiatellino@itba.edu.ar\nColeur Matias - 63461 mcoleur@itba.edu.ar\nCuri Martinez Gonzalo - 63463 gcurimartinez@itba.edu.ar\n"
#define SHBUFFER_SIZE 1024


static inline void wait4ViewProcess();


int main(int argc, char * argv[])	{

	sharedADT shm = createShm(SHBUFFER_NAME, SHBUFFER_SIZE);
	dprintf(STDOUT_FILENO,"%s", SHBUFFER_NAME);
	wait4ViewProcess();

	int files_amount = argc - 1;

	slaveMonitorADT monitor= startSlaveMonitor(files_amount,argv+1);

	getSlaves(monitor);

 	readFromSlaves(monitor, shm);
	closePipes(monitor);

	killShm(shm);
	killHeapMonitor();

	return 0;
}



static inline void wait4ViewProcess()	{

	sleep(2);
}
