#include "../include/support.h"
#include "../include/cdata.h"
#include "../include/cthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

void* func0(void *arg) {
	printf("Eu sou a thread ID0 imprimindo %d\n", *((int *)arg));
	return NULL;
}

void* func1(void *arg) {
	printf("Eu sou a thread ID1 imprimindo %d\n", *((int *)arg));
    return NULL;
}

int main(int argc, char *argv[]) {

	int	id0, id1;
	int i1 = 28;
	int i2 = 57;

	id0 = ccreate(func0, (void *)&i1);
    cyield();
	id1 = ccreate(func1, (void *)&i2);

	printf("Eu sou a main apos a criacao de ID0 e ID1\n");

	cjoin(id0);
	cjoin(id1);

	printf("Eu sou a main voltando para terminar o programa\n");
    return 0;
}
