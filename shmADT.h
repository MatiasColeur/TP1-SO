//shmADT.h

#ifndef SHM_ADT_H
#define SHM_ADT_H
	
	#include "heap.h"
	#include "beSafe.h"
	#include <stdlib.h>
	#include <fcntl.h>
	#include <sys/mman.h>
	#include <unistd.h>
	#include <semaphore.h>
	#include <sys/stat.h>	
	#include <string.h>

	#define SHM_NAME "/shm-"
	#define MUTEX_NAME "/semMutex-"
	#define SYNC_NAME "/semSync-"


	typedef struct sharedCDT * sharedADT;


//createShm(): Returns an instance of a sharedADT type, coding it shared memory with @name, having @size space avialiable.

	sharedADT createShm(const char * name, size_t size);




	sharedADT openShm(const char * name, size_t size);



//killShared(): Deletes an instance of a sharedADT type. It handles the dynamic memory and files in uses.

	void killShm(sharedADT shm);




	void closeShm(sharedADT shm);	



//writeShm(): Writes @src in the shared memory. It doesn't delete the previous information written in the shared memory. @size must fit in the remining space in the shared memory.

//	@return the exact space avialiable in the shared memory. Is on the user translate this information to the data type @src is using.

	size_t writeShm(sharedADT shm, const void * src, size_t size);



//readShm(): Reads @size content from the shared memory and writes it into @target.

//	@return the exact space avialiable in the shared memory. Is on the user translate this information to the data type @src is using.

	size_t readShm(sharedADT shm, void * target, size_t size);

#endif
