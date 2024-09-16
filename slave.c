// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//slave.c

#include <stdio.h>
#include <string.h>
#include "beSafe.h"
#include <unistd.h>


#define COMMAND "md5sum"	// Must be a string.
#define HASH_LENGTH 32		// Same for all hash numbers.
#define TYPE "r"		// Read file.
#define SEPARATOR '\n'		// Char for differentiate paths.

#define NullRetNull(x) if((x) == NULL) return NULL


char * getHash(const char * path);
char * getHashInput(const char * path);
char * getFileName(const char * path);

int main(int argc, char * argv[])	{

	char buf [100] = {0};
	int i = 0;

	int readReturn;
	
	char * hash;
	char * name;
	int pid;

	while((readReturn = read(STDIN_FILENO, buf+i, 1)) > 0)	{

		if(SEPARATOR == buf[i++])	{

		//Delete the SEPARATOR (is not part of the input path):	

			buf[--i] = '\0';

			hash = getHash(buf);

        		name = getFileName(buf);

        		pid = (int) getpid();
		//Everything is ok? (getpid() is always successful)
			
			if(hash != NULL && name != NULL)	{
			
				dprintf(STDOUT_FILENO,"%s-%d-%s", name, pid, hash);
			}
					
		//Reset buffer:
		
			i = 0;
			for(int j=0; buf[j]!=0; j++)	{
					
				buf[j] = 0;
			}

			freeHeap();
		}
	}

	errorManagement(readReturn == -1, "Read failed");	

	killHeapMonitor();
	exit(EXIT_SUCCESS);
}



char * getHash(const char * path)	{

	NullRetNull(path);

        char * buffer = safeCalloc(HASH_LENGTH+1, sizeof(char));
	
	char * input = getHashInput(path);
	NullRetNull(input);

        FILE * file = popen(input, TYPE);
	errorManagement(file == NULL, "popen failed");

        fgets(buffer, HASH_LENGTH+1, file);

	int closed = pclose(file);
	errorManagement(closed == -1, "pclose failed");

	return buffer; 

}



char * getHashInput(const char * path)	{

	NullRetNull(path);

	char * input = safeCalloc(1+strlen(COMMAND)+strlen(path), sizeof(char));
	
        strcat(input, COMMAND);
        strcat(input, " ");
        strcat(input, path);

	return input;

}



char * getFileName(const char * path)	{

	NullRetNull(path);	

//Find the '/' (useful when @path is an absolute path):

        int i=0; while(path[i++]!=0);
        int j=--i; 
	while(path[j]!='/' &&  j!=0)	{

			j--;
	}

        char * fileName = safeCalloc(i-j, 1);

        int k=0;
        while(path[j]!=0)       {

                fileName[k++] = path[j++];
        }
		
	return fileName;
}
