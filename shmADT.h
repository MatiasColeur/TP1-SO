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

	sharedADT createShm(char * name, size_t size);

	void killShared(sharedADT shm);

	void mapToMemory(sharedADT shm);

	sem_t * initSemaphore(char * sem_name);

	void unmap(sharedADT shm);


#endif
