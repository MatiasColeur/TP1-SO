//shmCDT.c

#include "shmADT.h"
//TO DO
shared_t * newShared(void * content)	{

	shared_t * shm = safeCalloc(1, sizeof(shared_t));
	return shm;
}
//TO do
void killShared(shared_t * shm)	{

	;
}
