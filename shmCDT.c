//shmCDT.c

#include "shmADT.h"

struct sharedCDT {

	int shm_fd;
	void * mapped;
	char * name;
	sem_t * semaphore;

}; 


//TO DO
sharedADT newShared(void * content)	{

	return safeCalloc(1, sizeof(sharedADT[0]));
}

//TO do
void killShared(sharedADT shm)	{
	if (shm == NULL) {
		return;
	}
	if (shm->mapped != NULL) {
		struct stat st;
		if (fstat(shm->shm_fd, &st) == -1) {
			perror("fstat");
			exit(EXIT_FAILURE);
		}
		unmap(shm, st.st_size);
	}

	if (shm->shm_fd != -1) {
		if (close(shm->shm_fd) == -1) {
			perror("close");
		}
	}

	if (shm->semaphore != NULL) {
		if (sem_close(shm->semaphore) == -1) {
			perror("sem_close");
		}
	}

	if (sem_unlink(shm->name) == -1) {
		perror("sem_unlink");
	}

	if (shm->name != NULL) {
		free(shm->name);
	}

	free(shm);
}

void mapToMemory(sharedADT shm, size_t len, int prot, int flags) {

	struct stat st;
	if (fstat(shm->shm_fd, &st) == -1) {
		perror("fstat");
		close(shm->shm_fd);
		exit(EXIT_FAILURE);
	}
	size_t filesize = st.st_size;

	void * map = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_PRIVATE, shm->shm_fd, 0);
	if (map == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}
	shm->mapped = map;
}

void unmap(sharedADT shm, size_t length) {
	if (munmap(shm->mapped, length) == -1) {
		perror("munmap");
		exit(EXIT_FAILURE);
	} 
}
