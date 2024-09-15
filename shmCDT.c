//shmCDT.c

#include "shmADT.h"

#define MUTEX_INITIAL 1                              
#define SYNC_INITIAL 0


static inline int isNull(const void * data)	{

    return data == NULL;
}

static inline int notNull(const void * data)	{

	return !isNull(data);
}

	
struct sharedCDT {

	int shmFd;
	void * mapped;
	char * shmName;
	size_t size;
	size_t index;
	sem_t * mutex;	
	sem_t * sync; 
	char * mutexName;
	char * syncName;
}; 



//-----------------------Spawn shmADT----------------------



static sem_t * initSemaphore(const char * semName, size_t initial) {

	sem_t * sem = sem_open(semName, O_CREAT, 0664, initial);
	errorManagement(sem == SEM_FAILED, "shared memory open failed");

	return sem;
}

static void mapToMemory(sharedADT shm) {

	errorManagement((shm->mapped = mmap(NULL, shm->size, PROT_READ | PROT_WRITE, MAP_SHARED, shm->shmFd, 0)) == MAP_FAILED, 
		"memory map failed");
}

static void truncateFd(sharedADT shm)	{

	errorManagement(ftruncate(shm->shmFd, shm->size) == -1, "ftruncate failed");
}
//TODO @MaxiChiate: Agregar permisos como parametro, asi el app solo puede escribir y el view solo puede leer.
static void openShmHandler(sharedADT shm)	{

	shm->shmFd = shm_open(shm->shmName, O_CREAT | O_EXCL | O_RDWR, 0666);	
	errorManagement( shm->shmFd == -1, "shared memory open failed");
}

static sharedADT createBaseShm(const char * name, size_t size)	{

	sharedADT shm = (sharedADT) safeMalloc(sizeof(struct sharedCDT));
	
	shm->size = size;
	shm->index = 0;

	shm->shmName = (char *) safeCalloc(strlen(SHM_NAME) + strlen(name) + 1, sizeof(char));
	shm->mutexName = (char *) safeCalloc(strlen(MUTEX_NAME) + strlen(name) + 1, sizeof(char));
	shm->syncName = (char *) safeCalloc(strlen(SYNC_NAME) + strlen(name) + 1, sizeof(char));

	strcpy(shm->shmName, SHM_NAME);
	strcpy(shm->mutexName, MUTEX_NAME);
	strcpy(shm->syncName, SYNC_NAME);

	strcat(shm->shmName, name);
	strcat(shm->mutexName, name);
	strcat(shm->syncName, name);
		
	return shm;
}

static void createResources(sharedADT shm)	{
	
	shm->mutex = initSemaphore(shm->mutexName, MUTEX_INITIAL);
	shm->sync = initSemaphore(shm->syncName, SYNC_INITIAL);
	openShmHandler(shm);
	truncateFd(shm);
	mapToMemory(shm);
}

static void openResources(sharedADT shm)	{
	
	shm->mutex = initSemaphore(shm->mutexName, MUTEX_INITIAL);
	shm->sync = initSemaphore(shm->syncName, SYNC_INITIAL);
	openShmHandler(shm);

	mapToMemory(shm);
}

static void unlinkResources(sharedADT shm)	{

	shm_unlink(shm->shmName);
	shm_unlink(shm->mutexName);
	shm_unlink(shm->syncName);
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
	closeShmHandler(shm);
}



void killShm(sharedADT shm)	{

	if (isNull(shm)) {

		return;
	}

	unlinkResources(shm);	

	closeShm(shm);
}



void closeShm(sharedADT shm)	{

	if (isNull(shm))	{

		return;
	}

	closeResources(shm);
	
	freeHeapVariable(shm->shmName);
	freeHeapVariable(shm->syncName);
	freeHeapVariable(shm->mutexName);
	freeHeapVariable(shm);
}



//-----------------------Read/write----------------------


#define down(x) (sem_wait(x))
#define up(x) (sem_post(x))


static inline int notEnoughSpaceInBuffer(sharedADT shm, size_t size) {
    
	return (shm->size - shm->index) < size;
}



size_t writeShm(sharedADT shm, const void * src, size_t size)	{

	if(isNull(shm) || isNull(src))	{ 
		
		return 0;
	}

	if(notEnoughSpaceInBuffer(shm, size))	{

//Only the app process will write in the shm:

		killShm(shm);
		errorManagement( 1, "write shm failed");
	}
	
	down(shm->mutex);
	
		memcpy(shm->mapped + shm->index, src, size); 

	up(shm->mutex);

	up(shm->sync);
	
	return shm->index += size ; 
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

	down(shm->sync);

	down(shm->mutex);
	
		memcpy(target, shm->mapped, size);

	up(shm->mutex);

	return shm->index += size;
	
}



int main() {

	sharedADT shm = createShm("Lo que mas te haga feliz en esta vida", 1024);
	sharedADT shm2 = openShm("Lo que mas te haga feliz en esta vida", 1024);	
	
	char * s = "Hola\n";
	int dim = strlen(s)+1;

	writeShm(shm, s, dim*sizeof(s[0]));

	char * t = safeMalloc(dim*sizeof(s[0]));

	readShm(shm2, t, dim*sizeof(s[0]));

	printf(t);

	killShm(shm);
	killShm(shm2);
	killHeapMonitor();
}
