//shmCDT.c

#include "shmADT.h"

struct sharedCDT {

	int shm_fd;
	void * mapped;
	char * name;
	size_t size;
	sem_t * semaphore;

}; 

static void initSemaphore(sharedADT shm, char * sem_name) {
	sem_t * sem = sem_open(sem_name, O_CREAT);
	errorManagement(sem == SEM_FAILED, "sem_open");
	shm->semaphore = sem;
}

static void mapToMemory(sharedADT shm) {
	errorManagement((shm->mapped = mmap(NULL, shm->size, PROT_READ | PROT_WRITE, MAP_SHARED, shm->shm_fd, 0)) == MAP_FAILED, "mmap failed");
}


sharedADT createShm(char * name, size_t size) {
	sharedADT shm = safeMalloc(sizeof(struct sharedCDT));
	shm->size = size;
	shm->name = name;

	errorManagement((shm->shm_fd = shm_open(name, O_CREAT | O_EXCL | O_RDWR, 0666)) == -1, "shm_open");
	//errorManagement(ftruncate(shm->shm_fd, shm->size) == -1, "ftruncate");

	if (ftruncate(shm->shm_fd, shm->size) == -1) {
		perror("ftruncate");
		close(shm->shm_fd);
		free(shm);
		return NULL;
	}

	initSemaphore(shm, name);
	mapToMemory(shm);

	return shm;
}

static void unmap(sharedADT shm) {
	errorManagement(munmap(shm->mapped, shm->size) == -1, "munmap");
}


//TO do
void killShared(sharedADT shm)	{
	if (shm == NULL) {
		return;
	}
	if (shm->mapped != NULL) {
		unmap(shm);
	}
	errorManagement(close(shm->shm_fd) == -1, "close");

	if (shm->semaphore != NULL) {
		errorManagement(sem_close(shm->semaphore) == -1, "close");
		errorManagement(sem_unlink(shm->name) == -1, "close");
	}

	if (shm->name != NULL) {
		free(shm->name);
	}

	unmap(shm);

}



// int main() {
// 	sharedADT shm = createShm("Lo que más te haga feliz en esta vida", 1024);
// 	killShared(shm);
// killHeapMonitor();
// }