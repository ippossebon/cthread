#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/support.h"
#include "../include/cdata.h"
#include "../include/cthread.h"

#define NUM_THREADS 5

int ids[NUM_THREADS];

void func_teste(void *arg)
{
	puts("Starting work");
	sleep(1);
	puts("Finishing work");
}

int main ()
{
	int i = 0;

	for (i = 0; i < NUM_THREADS; i++)
	{
		ids[i] = ccreate(&func_teste, NULL);
		printf("Thread %d created.\n", ids[i]);
		cjoin(ids[i]);
		printf("Thread %d finished.\n", ids[i]);
	}

	puts("Main finishing...");
	return 0;
}
