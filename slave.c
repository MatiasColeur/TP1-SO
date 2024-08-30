//slave.c

#include <stdio.h>
#include <string.h>
#include "beSafe.h"
#include <unistd.h>


#define COMMAND "md5sum"	// Must be a string.
#define HASH_LENGTH 32		// Same for all hash numbers.
#define TYPE "r"		// Read file.

char * getHash(const char * path);
char * getHashInput(const char * path);

int main(int argc, char * argv[])	{


	char * a = getHash(argv[1]);
	
	printf(a);

	freeHeap();
	
	return 0;
/*
	errorManagement(argc != 1, 
		"Slave main failed. No arguments expected\n");

//View process needs: Name of file, Md5 and slave ID.

//Looking for file name:

	char * path = argv[1];

	int i=0; while(path[++i]!=0);
	int j=i; while(path[--j]!='\\');

	char * fileName = calloc(strlen(path+j), sizeof(char));

	int k=0;
	while(path[j]!=0)	{

		fileName[k++] = path[j++];
	}

	printf("File name: %s\n", fileName);

//Looking for Md5: (No debe volcar el resultado a un archivo en disco, para luego leerlo desde el esclavo, deberá recibir el output de md5sum utilizando algún mecanismo de IPC más sofisticado. Hice literalmente eso, solo para probar las funciones)

//input will be COMMAND + " " + filepath:

	char * input = calloc(1+strlen("md5sum")+strlen(argv[1]), sizeof(char));
	
	strcat(input, COMMAND);
	strcat(input, " ");
	strcat(input, argv[1]);
	
	char buffer [2000] = {0};
	FILE * file = popen(input, TYPE);

	fscanf(file, buffer);
	
	char md5[200] = {0};
	strcat(md5, buffer);

	free(input);
	
//Looking for slave ID:

	char slaveID [3] = {0};
	int pid = (int) getpid();
	
	int l=1;
	while(pid!=0)	{
		slaveID[l--] = pid%10;
		pid = pid/10;
	}


	printf("File Name: %s\nMd5: %s\nSlave ID: %s",fileName, md5, slaveID);

	free(fileName);*/
}



char * getHash(const char * path)	{

	if(path == NULL)	return NULL;	

        char * buffer = safeCalloc(HASH_LENGTH+2, sizeof(char));
	char * input = getHashInput(path);

        FILE * file = popen(input, TYPE);

        fgets(buffer, HASH_LENGTH+2, file);

	strcat(buffer, "\n");	

	return buffer; 

}

char * getHashInput(const char * path)	{

	
	char * input = safeCalloc(1+strlen(COMMAND)+strlen(path), sizeof(char));
	
        strcat(input, COMMAND);
        strcat(input, " ");
        strcat(input, path);

	return input;

}

