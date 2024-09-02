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
}

void mapToMemory(size_t len, int prot, int flags, int fd) {

	struct stat st;
	if (fstat(fd, &st) == -1) {
		perror("fstat");
		close(fd);
		exit(EXIT_FAILURE);
	}
	size_t filesize = st.st_size;

	void * map = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (map == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}
	sharedCDT.mapped = map;
}

void unmap(size_t length) {
	if (munmap(sharedCDT.mapped, length) == -1) {
		perror("munmap");
		exit(EXIT_FAILURE);
	} 
}
