#include "../include/support.h"
#include "../include/cdata.h"
#include "../include/cthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

void func1(){
  printf("Thread 1...\n");
}

void func2(){
  printf("Thread 2...\n");
}

void func3(){
  printf("Thread 3...\n");
}

void test_TCBS_with_the_same_ticket(){
  printf("Testando TCBs com o mesmo ticket. Deve selecionar a de menor TID\n");

  ucontext_t main_context, context1, context2, context3;
  char stack1[SIGSTKSZ], stack2[SIGSTKSZ], stack3[SIGSTKSZ];

  getcontext(&context1);
  getcontext(&context2);
  getcontext(&context3);

  context1.uc_link          = &main_context;      /* contexto a executar no término */
  context1.uc_stack.ss_sp   = stack1;             /* endereço de início da pilha    */
  context1.uc_stack.ss_size = sizeof(stack1);     /*tamanho da pilha */
  makecontext(&context1, (void (*)(void)) func1, 0);

  context2.uc_link = &main_context;
  context2.uc_stack.ss_sp = stack2;
  context2.uc_stack.ss_size = sizeof(stack2);
  makecontext(&context2, (void (*)(void)) func2, 0);

  context3.uc_link = &main_context;
  context3.uc_stack.ss_sp = stack3;
  context3.uc_stack.ss_size = sizeof(stack3);
  makecontext(&context3, (void (*)(void)) func3, 0);

  TCB_t *tcb1;
  tcb1 = malloc(sizeof(TCB_t));
  tcb1->tid = 1;
  tcb1->state = PROCST_APTO;
  tcb1->ticket = 12;
  tcb1->context = context1;

  int ret = AppendFila2(ready, tcb1);
  if (ret != 0){
    perror("Erro ao inserir tcb1 na fila de aptos\n");
    return;
  }
  printf("Thread %d: ticket %d\n", tcb1->tid, tcb1->ticket);

  TCB_t *tcb2 = malloc(sizeof(TCB_t));
  tcb2->tid = 2;
  tcb2->state = PROCST_APTO;
  tcb2->ticket = 12;
  tcb2->context = context2;

  ret = AppendFila2(ready, tcb2);
  if (ret != 0){
    perror("Erro ao inserir tcb2 na fila de aptos\n");
    return;
  }
  printf("Thread %d: ticket %d\n", tcb2->tid, tcb2->ticket);

  TCB_t *tcb3 = malloc(sizeof(TCB_t));
  tcb3->tid = 3;
  tcb3->state = PROCST_APTO;
  tcb3->ticket = 12;
  tcb3->context = context3;

  ret = AppendFila2(ready, tcb3);
  if (ret != 0){
    perror("Erro ao inserir tcb3 na fila de aptos\n");
    return;
  }
  printf("Thread %d: ticket %d\n", tcb3->tid, tcb3->ticket);

  int primeira;
  primeira = scheduler();
  printf("Primeira a executar: %d \n", primeira);

  free(tcb1);
  free(tcb2);
  free(tcb3);
  free(ready);
}

void test_TCBS_with_different_tickets(){
  printf("Testando TCBs com diferentes tickets. Deve selecionar a de ticket mais próximo ao sorteado\n");

  ucontext_t main_context, context1, context2, context3;
  char stack1[SIGSTKSZ], stack2[SIGSTKSZ], stack3[SIGSTKSZ];

  getcontext(&context1);
  getcontext(&context2);
  getcontext(&context3);

  context1.uc_link          = &main_context;      /* contexto a executar no término */
  context1.uc_stack.ss_sp   = stack1;             /* endereço de início da pilha    */
  context1.uc_stack.ss_size = sizeof(stack1);     /*tamanho da pilha */
  makecontext(&context1, (void (*)(void)) func1, 0);

  context2.uc_link = &main_context;
  context2.uc_stack.ss_sp = stack2;
  context2.uc_stack.ss_size = sizeof(stack2);
  makecontext(&context2, (void (*)(void)) func2, 0);

  context3.uc_link = &main_context;
  context3.uc_stack.ss_sp = stack3;
  context3.uc_stack.ss_size = sizeof(stack3);
  makecontext(&context3, (void (*)(void)) func3, 0);

  TCB_t *tcb1;
  tcb1 = malloc(sizeof(TCB_t));
  tcb1->tid = 1;
  tcb1->state = PROCST_APTO;
  tcb1->ticket = Random2();
  tcb1->context = context1;

  int ret = AppendFila2(ready, tcb1);
  if (ret != 0){
    perror("Erro ao inserir tcb1 na fila de aptos\n");
    return;
  }
  printf("Thread %d: ticket %d\n", tcb1->tid, tcb1->ticket);

  TCB_t *tcb2 = malloc(sizeof(TCB_t));
  tcb2->tid = 2;
  tcb2->state = PROCST_APTO;
  tcb2->ticket = Random2();
  tcb2->context = context2;

  ret = AppendFila2(ready, tcb2);
  if (ret != 0){
    perror("Erro ao inserir tcb2 na fila de aptos\n");
    return;
  }
  printf("Thread %d: ticket %d\n", tcb2->tid, tcb2->ticket);

  TCB_t *tcb3 = malloc(sizeof(TCB_t));
  tcb3->tid = 3;
  tcb3->state = PROCST_APTO;
  tcb3->ticket = Random2();
  tcb3->context = context3;

  ret = AppendFila2(ready, tcb3);
  if (ret != 0){
    perror("Erro ao inserir tcb3 na fila de aptos\n");
    return;
  }
  printf("Thread %d: ticket %d\n", tcb3->tid, tcb3->ticket);

  int primeira;
  primeira = scheduler();
  printf("Primeira a executar: %d \n", primeira);

  free(tcb1);
  free(tcb2);
  free(tcb3);
  free(ready);
}

int main(int argc, char *argv[]){
  printf("*** Testando o escalonador... \n");

  test_TCBS_with_the_same_ticket();
  printf("\n");
  test_TCBS_with_different_tickets();

  return 0;
}
