// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

//shmCDT.c

#include "shmADT.h"

#define MUTEX_INITIAL 	1                              
#define SYNC_INITIAL 	0
#define KILL_INITIAL 	0

//Since both sem and shm have flags to control read and write permissions, 
// we can afford to enable all permissions for the file itself:


// sem_open:

#define SEM_MODE S_IRWXU | S_IRWXG | S_IRWXO // 00777

#define SEM_OFLAGS O_CREAT | O_RDWR

// shm_open:

#define OPEN_MODE S_IWUSR | S_IRGRP | S_IROTH // 00244

#define OPEN_WRITE_OFLAGS O_CREAT | O_RDWR // Must be O_RDWR, otherwise mmap() will throw EACCES error. See mmap() man for more info.
#define OPEN_READ_OFLAGS  O_CREAT | O_RDWR // Same reason. 

// mmap:

#define MEMORY_OFFSET 0

#define MMAP_FLAGS MAP_SHARED

#define MMAP_READ_OFLAGS PROT_READ 
#define MMAP_WRITE_OFLAGS PROT_WRITE

	
struct sharedCDT {

	int shmFd;
	void * mapped;
	char * shmName;
	size_t size;
	size_t index;

	sem_t * mutex;	
	sem_t * sync; 
	sem_t * kill;

	char * mutexName;
	char * syncName;
	char * killName;
}; 


#define down(x) (sem_wait((x)))

#define up(x) 	(sem_post((x)))


//-----------------------Spawn shmADT----------------------



static sem_t * initSemaphore(const char * semName, size_t initial) {

	sem_t * sem = sem_open(semName, SEM_OFLAGS, SEM_MODE, initial);
	errorManagement(sem == SEM_FAILED, "sem open failed");

	return sem;
}


static void mapToMemory(sharedADT shm, int oflags ) {

	errorManagement((shm->mapped = mmap(NULL, shm->size, oflags, MMAP_FLAGS, shm->shmFd, MEMORY_OFFSET)) == MAP_FAILED, 
		"memory map failed");
}


static void truncateFd(sharedADT shm)	{

	errorManagement(ftruncate(shm->shmFd, shm->size) == -1, "ftruncate failed");
}


static void openShmHandler(sharedADT shm, int oflags)	{

	shm->shmFd = shm_open(shm->shmName, oflags, OPEN_MODE); 	
	
	errorManagement( shm->shmFd == -1, "shared memory open failed");
}


static sharedADT createBaseShm(const char * name, size_t size)	{

	sharedADT shm = (sharedADT) safeMalloc(sizeof(struct sharedCDT));
	
	shm->size = size;
	shm->index = 0;

	shm->shmName = (char *) 	safeCalloc(strlen(SHM_NAME) + strlen(name) + 1, sizeof(char));
	shm->mutexName = (char *) 	safeCalloc(strlen(MUTEX_NAME) + strlen(name) + 1, sizeof(char));
	shm->syncName = (char *) 	safeCalloc(strlen(SYNC_NAME) + strlen(name) + 1, sizeof(char));
	shm->killName = (char *)	safeCalloc(strlen(KILL_NAME) + strlen(name) + 1, sizeof(char));

	strcpy(shm->shmName, SHM_NAME);
	strcpy(shm->mutexName, MUTEX_NAME);
	strcpy(shm->syncName, SYNC_NAME);
	strcpy(shm->killName, KILL_NAME);

	strcat(shm->shmName, name);
	strcat(shm->mutexName, name);
	strcat(shm->syncName, name);
	strcat(shm->killName, name);
		
	return shm;
}


static void createResources(sharedADT shm)	{
	
	shm->mutex = 	initSemaphore(shm->mutexName, MUTEX_INITIAL);
	shm->sync = 	initSemaphore(shm->syncName, SYNC_INITIAL);
	shm->kill = 	initSemaphore(shm->killName, KILL_INITIAL);

	openShmHandler(shm, OPEN_WRITE_OFLAGS);
	truncateFd(shm);
	mapToMemory(shm, MMAP_WRITE_OFLAGS);
}


static void openResources(sharedADT shm)	{

	shm->mutex = 	initSemaphore(shm->mutexName, MUTEX_INITIAL);
	shm->sync = 	initSemaphore(shm->syncName, SYNC_INITIAL);
	shm->kill = 	initSemaphore(shm->killName, KILL_INITIAL);

	openShmHandler(shm, OPEN_READ_OFLAGS);	

	mapToMemory(shm, MMAP_READ_OFLAGS);
}


static void unlinkResources(sharedADT shm)	{

	shm_unlink(shm->shmName);
	sem_unlink(shm->mutexName);
	sem_unlink(shm->syncName);
	sem_unlink(shm->killName);
}



sharedADT createShm(const char * name, size_t size) {

	sharedADT shm = createBaseShm(name, size);
	unlinkResources(shm);	
	createResources(shm);

	return shm;
}



sharedADT openShm(const char * name, size_t size)	{

	sharedADT shm = createBaseShm(name, size);
	openResources(shm);	

	return shm;
}



//-----------------------Kill shmADT----------------------



static void unmapFromMemory(sharedADT shm) {
	
	if (notNull(shm->mapped)) {

		errorManagement(munmap(shm->mapped, shm->size) == -1, "memory unmap failed");
		shm->mapped = NULL;
	}
}

static void closeSemaphore(sem_t * sem)	{

	if (notNull(sem)) {

		errorManagement(sem_close(sem) == -1, "memory shared close failed");
	}
}

static void closeShmHandler(sharedADT shm)	{
	
	errorManagement(close(shm->shmFd) == -1, "shared memory close failed");
}

static void closeResources(sharedADT shm)	{

	unmapFromMemory(shm);
	closeSemaphore(shm->mutex);
	closeSemaphore(shm->sync);
	closeSemaphore(shm->kill);
	closeShmHandler(shm);
}



void killShm(sharedADT shm)	{

	if (isNull(shm)) {

		return;
	}
	
	down(shm->kill);

	unlinkResources(shm);	

	closeShm(shm);
}



void closeShm(sharedADT shm)	{

	if (isNull(shm))	{

		return;
	}

	up(shm->kill);

	closeResources(shm);
	
	freeHeapVariable(shm->shmName);
	freeHeapVariable(shm->syncName);
	freeHeapVariable(shm->mutexName);
	freeHeapVariable(shm->killName);
	freeHeapVariable(shm);
}



//-----------------------Read/write----------------------





static inline int notEnoughSpaceInBuffer(sharedADT shm, size_t size) {
    
	return (shm->size - shm->index) < size;
}


static inline void upNtimes(size_t n, sem_t * sem)	{

	for(size_t i=0; i<n; i++)	{
		up(sem);
	}
}


static inline void downNtimes(size_t n, sem_t * sem)	{

	for(size_t i=0; i<n; i++)	{
		down(sem);
	}
}




size_t writeShm(sharedADT shm, const void * src, size_t size)	{
		
	if(isNull(shm) || isNull(src))	{ 
		
		return 0;
	}

	if(notEnoughSpaceInBuffer(shm, size))	{
		
		//Only the app process will write in the shm:

		down(shm->mutex);
			char ret=EOF;

			memcpy(shm->mapped + shm->index, &ret, 1); 

		up(shm->mutex);
		return 0;
	}

	down(shm->mutex);

		memcpy(shm->mapped + shm->index, src, size); 

	up(shm->mutex);

	upNtimes(size, shm->sync);
	return shm->index += size ; 
}


void writeEndShm(sharedADT shm, char end) {

    char endV[2] = {end, 0};  //probably EOF

    writeShm(shm, endV, 2);
}



size_t readShm(sharedADT shm, void * target, size_t size)	{

	if(isNull(shm) || isNull(target))	{ 
		
		return 0;
	}

	if(notEnoughSpaceInBuffer(shm, size))	{

//Only the view process will write in the shm:
		closeShm(shm);
		errorManagement(1, "read shm failed");
	}

  	downNtimes(size, shm->sync);

	down(shm->mutex);

		memcpy(target, shm->mapped + shm->index, size); 

	up(shm->mutex);

	return shm->index += size;
	
}
