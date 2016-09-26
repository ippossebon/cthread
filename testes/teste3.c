/*

	A ideia desse teste é criar uma thread na main que aponta para function1.
	Na function1, cria-se mais MAX_THREADS threads, cada uma redirecionando para function2.
	Na function2, cria-se mais MAX_THREADS threads, cada uma redirecionando para function3.
	Na function3, cada thread imprime um valor no visor. Esse valor é incrementado a cada thread que é criada.

*/

#include "../include/support.h"
#include "../include/cdata.h"
#include "../include/cthread.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_THREADS 10

csem_t* mutex1;
csem_t* mutex2;
int threadCounter = 0;

void function3()
{
	cyield();
	printf("%d\n", threadCounter);
}

void function2()
{
	int threads[MAX_THREADS];
	int i = 0;

	for (i = 0; i < MAX_THREADS; i++)
	{
		cwait(mutex2);
		threadCounter++;
		csignal(mutex2);
		puts("[2] voltou da csignl");

		threads[i] = ccreate(&function3, NULL);
		cyield();
	}

}

void function1()
{
	int threads[MAX_THREADS];
	int i = 0;

	for (i = 0; i < MAX_THREADS; i++)
	{
		cwait(mutex1);
		threadCounter++;
		csignal(mutex1);
		puts("[1] voltou da csignl");
		threads[i] = ccreate(&function2, NULL);
	}

	for (i = 0; i < MAX_THREADS; i++)
	{
		cjoin(threads[i]);
	}

}

int main ()
{
	mutex1 = malloc(sizeof(csem_t));
	mutex2 = malloc(sizeof(csem_t));

	int mainThread;
	csem_init(mutex1, 1);
	csem_init(mutex2, 1);

	mainThread = ccreate(&function1, NULL);
	cjoin(mainThread);
	puts("voltou");
	return 0;
}
