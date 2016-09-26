#include "../include/support.h"
#include "../include/cdata.h"
#include "../include/cthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

void* func1(void* arg){
  printf("Thread 1...\n");
  return;
}

void* func2(void* arg){
  printf("Thread 2...\n");
  return;
}

void* func3(void* arg){
  printf("Thread 3...\n");
  return;
}

int main(int argc, char **argv)
{
  printf("*** Testando ccreate()... \n");
  int tid1, tid2, tid3;

  tid1 = ccreate(func1, (void *) NULL);
  if (tid1 < 0){
    perror("Erro ao criar thread 1.");
    return -1;
  }

  tid2 = ccreate(func2, (void *) NULL);
  if (tid2 < 0){
    perror("Erro ao criar thread 2.");
    return -1;
  }

  tid3 = ccreate(func3, (void *) NULL);
  if (tid3 < 0){
    perror("Erro ao criar thread 3.");
    return -1;
  }

  TCB_t* item;
  if (FirstFila2(ready) == 0){
    item = (TCB_t*)GetAtIteratorFila2(ready);
    printf("Fila de aptos: ");

    while(item != NULL){
      printf(" %d ", item->tid);
      if(NextFila2(ready) == 0){
        item = (TCB_t*)GetAtIteratorFila2(ready);
      }
      else{
        perror("Erro ao setar o iterador para o próximo item da fila de ready.\n");
        return -1;
      }
    }
  }
  else{
    perror("Erro: Fila de aptos vazia. \n");
    return -1;
  }

  printf("\nTeste finalizado com sucesso.\n");

  return 0;
}
