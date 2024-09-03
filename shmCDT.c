//shmCDT.c

#include "shmADT.h"

struct sharedCDT {

	int shm_fd;
	void * mapped;
	char * name;
	size_t size;
	sem_t * semaphore;

}; 

static sem_t * initSemaphore(char * sem_name) {
	sem_t * sem = sem_open(sem_name, O_CREAT);
		if (sem == SEM_FAILED) {
			perror("sem_open");
			return NULL;
		}
	return sem;
}

static void mapToMemory(sharedADT shm) {
	shm->mapped = mmap(NULL, shm->size, PROT_READ | PROT_WRITE, MAP_SHARED, shm->shm_fd, 0);
	if (shm->mapped == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}
}


sharedADT createShm(char * name, size_t size, char * sem_name) {
	sharedADT shm = safeMalloc(sizeof(struct sharedCDT));
	if (shm == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	shm->size = size;
	shm->name = name;

	shm->shm_fd = shm_open(name, O_CREAT | O_EXCL | O_RDWR, 0666);
	if (shm->shm_fd == -1) {
		perror("shm_open");
		free(shm);
		exit(EXIT_FAILURE);
	}

	if (ftruncate(shm->shm_fd, shm->size) == -1) {
		perror("ftruncate");
		close(shm->shm_fd);
		free(shm);
		return NULL;
	}

	shm->semaphore = initSemaphore(sem_name);
	shm->mapped = mapToMemory(shm);

	return shm;
}

static void unmap(sharedADT shm) {
	if (munmap(shm->mapped, shm->size) == -1) {
		perror("munmap");
		exit(EXIT_FAILURE);
	}
}


//TO do
void killShared(sharedADT shm)	{
	if (shm == NULL) {
		return;
	}
	if (shm->mapped != NULL) {
		unmap(shm);
	}

	if (close(shm->shm_fd) == -1) {
		perror("close");
	}

	if (shm->semaphore != NULL) {
		if (sem_close(shm->semaphore) == -1) {
			perror("sem_close");
		}
		if (sem_unlink(shm->name) == -1) {
			perror("sem_unlink");
		}
	}

	if (shm->name != NULL) {
		free(shm->name);
	}

	unmap(shm);
	free(shm);
}







