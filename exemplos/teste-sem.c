//
// Barbeiro.c: Este programa implementa um dos classicos de programação
//             concorrente: o barbeiro dorminhoco.
//             O objetivo deste programa é testar a implementação do
//             micro kernel desenvolvido na disciplina INF01142
//
// Primitivas testadas: ccreate, cjoin, cyield, cwait e csignal.
//
// Este programa é basedo na solução de Tanenbaum apresentada no livro
// "Modern Operating System" (Prentice Hall International 2 ed.).
//
// Disclamer: este programa foi desenvolvido para auxiliar no desenvolvimento
//            de testes para o micronúcleo. NÃO HÁ garantias de estar correto.

#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<errno.h>
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
  printf("Threads using the resource = %d\n", resource);
  sleepao();
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
