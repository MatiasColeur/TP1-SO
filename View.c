//view.c

#include "beSafe.h"
#include "heap.h"
#include "shmADT.h"
#include <unistd.h>

int main(int argc, char * argv[])	{

//Gets the buffer:

	char * name;

	if(argc != 1)	{

		name = argv[1];
	}
	else	{

		errorManagement(scanf("%s",name) == EOF, "scanf failed");
	}

	sharedADT shm = createShm(name, 1024);

	char buf;
	char toPrint[1024];
	int i=0;
	int shmSpaceAvailable;

	while((shmSpaceAvailable = readShm(shm, &buf, 1)) > 1024 && buf != EOF)	{
		
		toPrint[i++] = buf;
		
		if(buf == '\n')	{
			
			printf("%s", toPrint);
			i=0;
		}
		
	}
	
	killShm(shm);
	killHeapMonitor();
}
