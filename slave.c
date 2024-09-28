// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//slave.c

#include <stdio.h>
#include <string.h>
#include "beSafe.h"
#include <unistd.h>
#include "commonData.h"

#define HASH_LENGTH 32		// Same for all hash numbers.
#define TYPE "r"		// Read file.


void getHash(const char * path, char * hash);
void getHashInput(const char * path, char * input);
void getFileName(const char * paht, char * fileName);


int main(int argc, char * argv[])	{

	char buf [BUFF_LEN] = {0};
	int i = 0;

	int readReturn;
	
	int pid;

	while((readReturn = read(STDIN_FILENO, buf+i, 1)) > 0)	{

		if(SEPARATOR == buf[i++])	{

		//Delete the SEPARATOR (is not part of the input path):	

			buf[--i] = '\0';

			char hash[BUFF_LEN] = {0};
			getHash(buf, hash);

			char name[BUFF_LEN] = {0};
			getFileName(buf, name);

        		pid = (int) getpid();

		//Everything is ok? (getpid() is always successful)
			
			if(notNull(hash) && notNull(name))	{
			
				dprintf(STDOUT_FILENO,"%s-%d-%s%c", name, pid, hash, SEPARATOR);
			}
					
		//Reset buffer:
		
			i = 0;
			for(int j=0; j<BUFF_LEN; j++)	{
					
				buf[j] = 0;
			}
		}
	}

	errorManagement(readReturn == -1, "Read failed");	

	killHeapMonitor();
	exit(EXIT_SUCCESS);
}



//char * getHash(const char * path)	{
//
void getHash(const char * path, char * hash) {

	if(isNull(path))	{

    return;
	}

	char input[BUFF_LEN] = {0};
	getHashInput(path, input);

	if(isNull(input))	{

		return;
	}

        FILE * file = popen(input, TYPE);
	errorManagement(file == NULL, "popen failed");

        fgets(hash, HASH_LENGTH+1, file);

	int closed = pclose(file);
	errorManagement(closed == -1, "pclose failed");

}



void getHashInput(const char * path, char * input)	{

	if(isNull(path))	{

		  return;
	}

        strcat(input, COMMAND);
        strcat(input, " ");
        strcat(input, path);

}



void getFileName(const char * path, char * fileName)	{

	if(isNull(path))	{

    return;
	}

//Find the '/' (useful when @path is an absolute path):

    int i=0; while(path[i++]!=0);
    int j=--i; 
	while(path[j]!='/' &&  j!=0)	{

			j--;
	}

    int k=0;
    while(path[j]!=0)       {

            fileName[k++] = path[j++];
    }
}
