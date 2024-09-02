//shmADT.h

#ifndef SHM_ADT_H
#define SHM_ADT_H
	
	#include "heap.h"
	#include <stdlib.h>
	#include <fcntl.h>
	#include <sys/mman.h>
	#include <unistd.h>
	#include <semaphore.h>
	#include <sys/stat.h>	
	
	typedef struct sharedCDT * sharedADT;

	sharedADT newShared(void * content);

	void killShared(sharedADT shm);

	void mapToMemory(sharedADT shm, size_t len, int prot, int flags);

	void unmap(sharedADT shm, size_t length);


#endif
