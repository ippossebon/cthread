#include "../include/support.h"
#include "../include/cthread.h"
#include <stdio.h>
#include <stdlib.h>

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
    int i;
    puts("*** Testando cidentify()...");
    char* small_string;
    cidentify(small_string, 15);

    printf("%s\n", small_string);

    char big_string[104] = "";
    cidentify(big_string, sizeof(big_string));
    printf("%s\n", big_string);


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

  printf("\nTeste finalizado com sucesso.\n");
  return 0;
}
