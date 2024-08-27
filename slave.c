//slave.c
// programa para probar las funciones, hay que hacer la comunicación entre aplication y slave con un pipe, no args
#include <stdio.h>
#include <string.h>
#include "beSafe.h"
#include <unistd.h>


#define COMMAND "md5sum" // Must be a string.
#define TYPE "rw"

int main(int argc, char * argv[])	{

	errorManagement(argc != 2, 
		"Slave main failed. Usage: %s <file path>\n");

//View process needs: Name of file, Md5 and slave ID.

//Looking for file name:

	char * path = argv[1];

	int i=0; while(path[++i]!=0);
	int j=i; while(path[--j]!='\\');

	char fileName[strlen(path+j)];

	int k=0;
	while(path[j]!=0)	{

		fileName[k++] = path[j++];
	}

//Looking for Md5: (No debe volcar el resultado a un archivo en disco, para luego leerlo desde el esclavo, deberá recibir el output de md5sum utilizando algún mecanismo de IPC más sofisticado. Hice literalmente eso, solo para probar las funciones)

//input will be COMMAND + " " + filepath:

	char input[1+strlen(COMMAND)+strlen(argv[1])] = {0};
	
	strcat(input, COMMAND);
	stract(input, " ");
	stract(input, argv[1]);
	
	char buffer [2000] = {0};
	FILE file = popen(input, TYPE);

	fscanf(file, buffer);
	
	char md5[200] = {0};
	strcat(md5, buffer);
	
//Looking for slave ID:

	char slaveID [3] = {0};
	int pid = (int) getpid();
	
	int l=1;
	while(pid!=0)	{
		slaveID[l--] = pid%10;
		pid = pid/10;
	}


	printf("File Name: %s\nMd5: %s\nSlave ID: %s",fileName, md5, slaveID);

}
