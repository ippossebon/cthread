#include "../include/support.h"
#include "../include/cthread.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_THREADS 50
#define MAX_WORK 100

csem_t* semaphore;
int workDone = 0;
int lastWork = -1;

void makeWork()
{
	while(workDone < MAX_WORK)
	{
		cwait(semaphore);
		lastWork = workDone;
		workDone++;
		csignal(semaphore);
		if (workDone <= lastWork || workDone -1 != lastWork)
		{
			puts("Mutual Exclusion error!!!!!!!");
			printf("%d -> %d\n", lastWork, workDone);
			exit(0);
		}
		if (workDone < MAX_WORK)
		{
			printf("Work #%d done.\n", workDone);
			cyield();
		}
	}
	return;
}

int main()
{
	int ids[MAX_THREADS];
	int i = 0;

	semaphore= malloc(sizeof(csem_t));
	puts("Started.");

	if (csem_init(semaphore, 1) != 0){
		perror("Erro na inicilização do semáforo");
		return -1;
	}


	for (i=0; i < MAX_THREADS; i++)
	{
		ids[i] = ccreate(&makeWork, NULL);
		printf("Thread %d created...\n", ids[i]);
	}

	for (i=0; i < MAX_THREADS; i++)
	{
		cjoin(ids[i]);
		printf("Thread using TID %d finished.\n", ids[i]);
	}

	return 0;
}
