// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

//slaveMonitorADT.h

#ifndef SLAVE_MONITOR_ADT_H
#define SLAVE_MONITOR_ADT_H

    	#include <stdio.h>
    	#include <sys/types.h>
    	#include <string.h>
    	#include "heap.h"
    	#include "beSafe.h"
    	#include "shmADT.h"

    	#define SLAVES_AMOUNT 5
    	#define FILES_PER_SLAVE 10

	#define CHILD_PATH "./slave"
	#define SLAVE_ARGV {CHILD_PATH, NULL}
	#define SLAVE_ENVP {NULL}

    	#define SEPARATOR '\n'
	#define OUTPUT_FILE_NAME "output.txt"
	#define COMMAND "md5sum"	
	

	typedef struct slaveMonitorCDT * slaveMonitorADT;



    	void getSlaves();



    	void closePipes(slaveMonitorADT monitor);



    	void getOneSlave(slaveMonitorADT monitor, int slave_position);



    	int getMaxFD(int * fd_array, int fd_amount);



    	void readFromSlaves(slaveMonitorADT monitor, sharedADT shm);



    	slaveMonitorADT startSlaveMonitor(const int files_amount, char * files[]);



    	void assingToSlave(slaveMonitorADT monitor, int slave_position);



    	int canAssign(slaveMonitorADT monitor);



#endif
