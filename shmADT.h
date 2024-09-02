//shmADT.h

#ifndef SHM_ADT_H
#define SHM_ADT_H
	
	#include "heap.h"
	#include <fcntl.h>
	#include <sys/mman.h>
	#include <unistd.h>
	#include <semaphore.h>
	
	typedef struct sharedCDT * sharedADT;

	sharedADT newShared(void * content);

	void killShared(sharedADT shm);


#endif
