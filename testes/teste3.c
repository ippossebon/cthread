#include "../include/support.h"
#include "../include/cdata.h"
#include "../include/cthread.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_THREADS 500

void functionWithParam (void* parameter)
{
	printf("Thread executed funcion with parameter \"%d\"\n", *(int*)parameter);
}

int main ()
{
	int threads[MAX_THREADS];
	int i;

	for (i = 0; i < MAX_THREADS; i++)
	{
		threads[i] = ccreate(&functionWithParam, (void*)&i);
		cjoin(threads[i]);
	}

	puts("Main finished.");
	return 0;
}
