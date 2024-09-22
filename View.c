
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



int main(int argc, char * argv[]) {

    char shmname[1000] = {0};

	if (argc != 1)  {

        errorManagement(isNull(argv[1]) || (sizeof(argv[1][0]) != sizeof(char)), "incorrect argument type");
		strcpy(shmname, argv[1]);
	}
    else    {
        
        int i = -1;
        do  {
	
            i++;
            errorManagement(read(STDIN_FILENO, shmname+i, 1) == -1, "read failed");        

        }   while(shmname[i] != SEPARATOR);

        shmname[i] = '\0';
    }

    sharedADT shm = openShm(shmname, SHBUFFER_SIZE);

	char buf = 0;
	char toPrint[SHBUFFER_SIZE];
	int i=0;

	while( (readShm(shm, &buf, 1) != SHBUFFER_SIZE) && (buf != EOF) )	{
		
		toPrint[i++] = buf;
		
		if(buf == SEPARATOR)	{
			
            toPrint[i] = '\0';
            dprintf(STDOUT_FILENO,"%s", toPrint);
			i=0;
		}
		
	}

	closeShm(shm);
	killHeapMonitor();

}
