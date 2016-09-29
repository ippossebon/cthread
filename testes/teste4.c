#include "../include/support.h"
#include "../include/cdata.h"
#include "../include/cthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void func(void* n)
{
	sleep(*(int*)n);
	cyield();
	printf("%d\n", *(int*)n);
	return;
}

int main()
{
	int threads[5];
	int params[5]={2,4,3,1,5};
	int i;

	for(i=0;i<5;++i)
		threads[i] = ccreate(&func, (void *) &(params[i]));

	for(i=0;i<5;++i)
		cjoin(threads[i]);

	return 0;
}
