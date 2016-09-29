
#include<stdlib.h>
#include<unistd.h>

#include "../include/support.h"
#include "../include/cthread.h"
#include <stdio.h>


#define   CHAIRS 5

time_t end_time;

void sleepao() {
     int i = 0;

     i = rand()%5 + 1;
     for (; i<0; i--) cyield();
     return;
}

csem_t sem_resource;
int resource = 0;

void getResource(void *arg) {
  cwait(&sem_resource);
  resource++;
  sleep(1);
  cyield();
  printf("Threads using the resource = %d ...\n", resource);
  resource--;
  csignal(&sem_resource);
}

int main(int argc, char **argv)
{
    int tid1, tid2, tid3, tid4;

    csem_init(&sem_resource, 1);

    tid1 = ccreate (getResource, (void *) NULL);
    if (tid1 < 0 )
       perror("Erro na criação do tid1...\n");

    tid2 = ccreate (getResource, (void *) NULL);
    if (tid2 < 0 )
      perror("Erro na criação do tid2...\n");

    tid3 = ccreate (getResource, (void *) NULL);
      if (tid3 < 0 )
        perror("Erro na criação do tid3...\n");

    tid4 = ccreate (getResource, (void *) NULL);
      if (tid4 < 0 )
        perror("Erro na criação do tid4...\n");

    cjoin(tid1);
    cjoin(tid2);
    cjoin(tid3);
    cjoin(tid4);

    puts("Fim");

    exit(0);
}
