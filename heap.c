// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//heap.c

#include "heap.h"
#include <stdio.h>	// for heapMonitor jokes.

// Number of positions added while being reallocated in heap memory. Must be >0:

#define BLOCK 5

#define isHeapMonitorDead() heapMonitor.alive == FALSE
#define HEAP_MONITOR_DIALOG_FORMAT "Heap Monitor: \"%s\"\n"

//Heap Monitor dialogs:

static const char *heapMonitorDialogs[] = {

    	"Just reallocating my thoughts... I mean, memory.",
    	"All systems go! But let's not push it.",
    	"No leaks today! I'm watertight... for now.",
    	"Standing by, ready to malloc at your command.",
    	"Everything's fine, until the next segfault.",
    	"Currently 100% allocated... to doing nothing.",
    	"All pointers point somewhere. That's a win!",
    	"I haven't crashed yet... so that's progress.",
    	"Just defragmented... feeling less scattered.",
    	"Heap status: Not overflowing... yet.",
    	"Memory check complete. Running at peak efficiency... or so I think.",
    	"Running smooth like a freshly malloc'd block.",
    	"No need for a core dump today. All good!",
    	"Everything's in order. No dangling pointers here.",
	"HeapMonitor here: The stack's stable, I'm stable... life is good."

	//Dialogs made by chatGPT
};

#define HEAP_MONITOR_DIALOGS_AMOUNT 15

static const char *deadHeapMonitorDialogs[] = {

    	"Allocating memory noises... but nothing happens.",
    	"You try to malloc... but only silence remains.",
    	"The heapMonitor stares blankly into the void... all memory is lost.",
    	"You call realloc, but the heapMonitor does not respond.",
   	"...",
    	"A faint whisper echoes... 'Segmentation fault...'",
    	"You feel a deep void... the heapMonitor has moved on to the great garbage collector in the sky.",
    	"...",
    	"free me from this existence... but it's too late.",
    	"HeapMonitor has stopped responding... forever.",
    	"Your request for memory allocation returns... NULL."

	//Dialogs made by chatGPT
};

#define DEAD_HEAP_MONITOR_DIALOGS_AMOUNT 11

//Structure for heap management:

struct tHeapMonitor	{

	void ** array;
	unsigned int using;
	unsigned int size;
	char alive;

// Some jokes for debugging:

	char ** dialog;
	size_t dialogAmount;
	size_t dialogIndex;
};

//For security there always is an instance of a heapMonitor with an array with pointers to every dynamic object. So if an error ocurs all are accesible to free:

static struct tHeapMonitor heapMonitor =	{

	.array = NULL,
	.using = 0,
	.size = 0,
	.alive = TRUE,

// Some jokes for debugging:
	
	.dialog = (char **) heapMonitorDialogs,

	.dialogAmount = HEAP_MONITOR_DIALOGS_AMOUNT,
	.dialogIndex = 0
};

//newHeapVariable(): save a pointer to the new heap object in an internal structure to centralize them. If @new is repeated, nothing happen.

//	@new: is a pointer to the new object to save.

static void newHeapVariable(void * new)	{

	for(int i=0; i<heapMonitor.using; ++i)	{

		if(heapMonitor.array[i] == new)
			return;
	}

	
	if(heapMonitor.using == heapMonitor.size)	{
		
		heapMonitor.array = realloc(heapMonitor.array, (heapMonitor.size + BLOCK) * sizeof(void*));

		errorManagement(heapMonitor.array == NULL, "Memory allocated failed");	
		
		heapMonitor.size += BLOCK;
	}

	heapMonitor.array[heapMonitor.using++] = new;
}


//removeFromHeapList():	Removes @p from @heapMonitor.array. No free is done.

static void removeFromHeapList(void * p)	{

	for(int i=0; i<heapMonitor.using; ++i)	{

		if(heapMonitor.array[i]==p)	{

			heapMonitor.array[i] = heapMonitor.array[heapMonitor.using-1];
			heapMonitor.using--;
		}
	}

}



void freeHeap()  {

	if(isHeapMonitorDead())	return;
	
	int i=0;      

	while(i<heapMonitor.using)      {
	
		free(heapMonitor.array[i++]);
	}

	heapMonitor.size = 0;
	heapMonitor.using = 0;

	free(heapMonitor.array);
	heapMonitor.array = NULL;
}



void freeHeapVariable(void * p) {
	
	removeFromHeapList(p);
	free(p);
}



void killHeapMonitor()	{

	if(isHeapMonitorDead())	return;

	freeHeap();
	heapMonitor.alive = FALSE;

// Dialogs changes when it dies:

	heapMonitor.dialog = (char **) deadHeapMonitorDialogs; 
    heapMonitor.dialogAmount = DEAD_HEAP_MONITOR_DIALOGS_AMOUNT;
	heapMonitor.dialogIndex = 0;
}



static void * safeAlloc(void * ptr)	{

	if(isHeapMonitorDead())	return NULL;

	errorManagement(ptr == NULL, "Memory allocated failed");

	newHeapVariable(ptr);

	return ptr;
}



void * safeMalloc(size_t size)	{

	return safeAlloc(malloc(size));
}



void * safeCalloc(size_t nmemb, size_t size)	{
	
	return safeAlloc(calloc(nmemb, size));
}



void * safeRealloc(void * ptr, size_t size)	{

// Needed because realloc will free the old heap memory address so freeHeap() would try to free already free memory. 
// Deleting the old memory address and adding the new one keeps everyone safe:

	removeFromHeapList(ptr);
	return safeAlloc(realloc(ptr, size));
}



static void changeHeapMonitorDialog()	{

	heapMonitor.dialogIndex = (heapMonitor.dialogIndex + 1) % heapMonitor.dialogAmount;
}



void whatsUpHeapMonitor()	{

	printf(HEAP_MONITOR_DIALOG_FORMAT, heapMonitor.dialog[heapMonitor.dialogIndex]);
	changeHeapMonitorDialog();
}