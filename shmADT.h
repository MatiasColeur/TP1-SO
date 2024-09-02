//shmADT.h

#ifndef SHM_ADT_H
#define SHM_ADT_H
	
	#include "heap.h"
	#include <fcntl.h>
	#include <sys/mman.h>
	#include <unistd.h>
	#include <semaphore.h>
	

	typedef struct shared	{

		int shm_fd;
		void * mapped;
		char * name;
		sem_t * semaphore;
			
	}	shared_t;



	shared_t * newShared(void * content);

	void killShared(shared_t * shm);


#endif
