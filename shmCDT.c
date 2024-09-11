//shmCDT.c

#include "shmADT.h"

struct sharedCDT {

	int shmFd;
	void * mapped;
	char * shmName;
	size_t size;
	size_t using;
	sem_t * mutex;	
	sem_t * sync; 
	char * mutexName;
	char * syncName;
}; 



//-----------------------Spawn shmADT----------------------



static void initSemaphore(sharedADT shm) {

	sem_t * sem = sem_open(shm->name, O_CREAT, 0664, 1);
	errorManagement(sem == SEM_FAILED, "shared memory open failed");

	shm->semaphore = sem;
}

static void mapToMemory(sharedADT shm) {

	errorManagement((shm->mapped = mmap(NULL, shm->size, PROT_READ | PROT_WRITE, MAP_SHARED, shm->shmFd, 0)) == MAP_FAILED, 
		"memory map failed");
}

static void truncateFd(sharedADT shm)	{

	errorManagement(ftruncate(shm->shmFd, shm->size) == -1, "ftruncate failed");
}

static void openShm(sharedADT shm)	{

	shm->shmFd = shm_open(shm->name, O_CREAT | O_EXCL | O_RDWR, 0666);	
	errorManagement( shm->shmFd == -1, "shared memory open failed");
}

static sharedADT createBaseShm(const char * name, size_t size)	{

	sharedADT shm = (sharedADT) safeMalloc(sizeof(struct sharedCDT));
	
	shm->size = size;
	shm->using = 0;

	shm->shmName = (char *) safeCalloc(strlen(SHM_NAME) + strlen(name) + 1, sizeof(char));
	shm->mutexName = (char *) safeCalloc(strlen(MUTEX_NAME) + strlen(name) + 1, sizeof(char));
	shm->syncName = (char *) safeCalloc(strlen(SYNC_NAME) + strlen(name) + 1, sizeof(char));

	
	strcat(shm->shmName, SHM_NAME),
	strcat(shm->shmName, MUTEX_NAME),
	strcat(shm->shmName, SYNC_NAME),

	strcat(shm->shmName, name),
	strcat(shm->mutexName, name),
	strcat(shm->syncName, name),
		
	return shm;
}


sharedADT createShm(const char * name, size_t size) {

	sharedADT shm = safeMalloc(sizeof(struct sharedCDT));
	shm->size = size;
	shm->using = 0;
	
	shm->name = safeCalloc(strlen(name)+1,sizeof(name[0]));
	strcat(shm->name, name);

	openShm(shm);
	truncateFd(shm);
	initSemaphore(shm);
	mapToMemory(shm);

	return shm;
}



//-----------------------Kill shmADT----------------------



static void unmapFromMemory(sharedADT shm) {
	
	if (shm->mapped != NULL) {

		errorManagement(munmap(shm->mapped, shm->size) == -1, "memory unmap failed");
	}
}

static void killSemaphore(sharedADT shm)	{

	if (shm->semaphore != NULL) {

		errorManagement(sem_close(shm->semaphore) == -1, "memory shared close failed");
		errorManagement(sem_unlink(shm->name) == -1, "memory shared close failed");
	}
}

static void closeShm(sharedADT shm)	{
	
	errorManagement(shm_unlink(shm->name) == -1, "memory shared close failed");
	errorManagement(close(shm->shmFd) == -1, "shared memory close failed");
}



void killShm(sharedADT shm)	{

	if (shm == NULL) {

		return;
	}

	unmapFromMemory(shm);
	closeShm(shm);
	killSemaphore(shm);

	//To free shm->name and shm, needs to add features to heap.c
}



//-----------------------Read/write----------------------



size_t writeShm(sharedADT shm, const void * src, size_t size)	{

	if(shm == NULL) return 0;
	if(src == NULL) return 0;

//fix:
	if( (shm->size - shm->using) < size)	{	
	
		killShm(shm);
		errorManagement( 1, "write shm failed");
	}
	
	sem_wait(shm->semaphore);
	
		memcpy(shm->mapped + shm->using, src, size); 

	sem_post(shm->semaphore);

	return shm->using += size;
}



size_t readShm(sharedADT shm, void * target, size_t size)	{

	if(shm == NULL) return 0;
	if(target == NULL) return 0;

//fix:
	if(shm->using < size)	{
			
		killShm(shm);
		errorManagement(1, "read shm failed");
	}

	sem_wait(shm->semaphore);
	
		memcpy(target, shm->mapped, size);
		memcpy(shm->mapped, shm->mapped+size, shm->using-size);

	sem_post(shm->semaphore);

	return shm->using -= size;
	
}

/*
int main() {

	sharedADT shm = createShm("Lo que más te haga feliz en esta vida", 1024);
	
	char * s = "Hola\n";
	int dim = strlen(s)+1;

	writeShm(shm, s, dim*sizeof(s[0]));

	char * t = safeMalloc(dim*sizeof(s[0]));

	readShm(shm, t, dim*sizeof(s[0]));

	printf(t);

 	killShm(shm);
	killHeapMonitor();
} */
