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
#include <string.h>
#include "commonData.h"

#define SHBUFF_SIZE 1024 //TODO sacar esto


int main(int argc, char * argv[]) {

    char shmname[1000] = {0};

	if (argc != 1)  {

        errorManagement(isNull(argv[1]) || (sizeof(argv[1]) != sizeof(char)), "incorrect argument type");
		strcat(shmname, argv[1]);
	}
    else    {
        
        int i = 0;
        do  {

            errorManagement(read(STDIN_FILENO, shmname+i, 1) == -1, "read failed");        

        }   while(shmname[i++] != '\0');

        shmname[i] = '\0';
    }

    sharedADT shm = openShm(shmname, SHBUFF_SIZE);

	char buf;
	char toPrint[SHBUFF_SIZE];
	int i=0;
	int shmSpaceRead;

	while((shmSpaceRead = readShm(shm, &buf, 1)) != SHBUFF_SIZE && buf != EOF)	{
		
		toPrint[i++] = buf;
		
		if(buf == SEPARATOR)	{
			
            toPrint[i+1] = '\0';
            dprintf(STDOUT_FILENO,"%s", toPrint);
			i=0;
		}
		
	}
	
	closeShm(shm);
	killHeapMonitor();

    // int fd;	
	// struct stat st;

    // fd = shm_open(shmname, O_RDWR, 0666);
    // if (fd == -1) {
    //     perror("Error al abrir memoria compartida");
    //     exit(EXIT_FAILURE);
    // }

	// if (fstat(fd, &st) == -1) {
    //     perror("Error al obtener tamaño de la memoria compartida");
    //     close(fd);
    //     exit(EXIT_FAILURE);
    // }
    // size_t size = st.st_size;


    // void * mapped_mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    // if (mapped_mem == MAP_FAILED) {
    //     perror("Error al mapear memoria");
    //     close(fd);
    //     exit(EXIT_FAILURE);
    // }

	// char * shmbuff = (char *) mapped_mem;

	// char * token = strtok(shmbuff, ",");
	// while (token != NULL) {

	//     printf("%s\n", token);
	// 	token = strtok(NULL, ",");
	// } 


    // munmap(mapped_mem, size);
    // close(fd);
    // shm_unlink(shmname);  // Elimina el segmento de memoria compartida cuando ya no se necesita


    return 0;
}
