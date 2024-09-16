// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

//view.c

#include "beSafe.h"
#include "heap.h"
#include "shmADT.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>


int main(int argc, char * argv[]) {

	if (argc != 2) {
		exit(EXIT_FAILURE);
	}

	char * shmname = argv[1];
    int fd;	
	struct stat st;

    fd = shm_open(shmname, O_RDWR, 0666);
    if (fd == -1) {
        perror("Error al abrir memoria compartida");
        exit(EXIT_FAILURE);
    }

	if (fstat(fd, &st) == -1) {
        perror("Error al obtener tamaño de la memoria compartida");
        close(fd);
        exit(EXIT_FAILURE);
    }
    size_t size = st.st_size;


    void * mapped_mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped_mem == MAP_FAILED) {
        perror("Error al mapear memoria");
        close(fd);
        exit(EXIT_FAILURE);
    }

	char * shmbuff = (char *) mapped_mem;

	char * token = strtok(shmbuff, ",");
	while (token != NULL) {
		printf("%s\n", token);
		token = strtok(NULL, ",");
	} 


    munmap(mapped_mem, size);
    close(fd);
    shm_unlink(shmname);  // Elimina el segmento de memoria compartida cuando ya no se necesita

    return 0;
}
