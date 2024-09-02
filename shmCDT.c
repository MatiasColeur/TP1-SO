//shmCDT.c

#include "shmADT.h"

struct sharedCDT   {

	int shm_fd;
	void * mapped;
	char * name;
	sem_t * semaphore;

}; 


//TO DO
sharedADT newShared(void * content)	{

	sharedADT shm = safeCalloc(1, sizeof(sharedADT[0]));
	return shm;
}
//TO do
void killShared(sharedADT shm)	{

	;
}
