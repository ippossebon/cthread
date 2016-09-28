#include "../include/support.h"
#include "../include/cdata.h"
#include "../include/cthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ucontext.h>

PFILA2 ready;
PFILA2 blocked;

unsigned int ready_size = 0;
unsigned int blocked_size = 0;
unsigned int thread_count = 0;

int initialized = 0;
int is_cyield = 0;

// NAO ESQUECER DE DAR FREE
ucontext_t current_context;
ucontext_t final_context;
ucontext_t scheduler_context;
TCB_t* current_tcb;

PFILA2 joined_tcbs;
unsigned int joined_tcbs_size = 0;

void scheduler();
void dispatcher(int scheduled_tid);
void finishThread();
int existsTID(int tid);
void printQueue(PFILA2 queue);
int unlockThread(int tid);
void createSchedulerContext();

void init(){
    /* Inicialização de current, que corresponde a thread que está executando atualmente. */
    getcontext(&current_context);

    char current_stack[SIGSTKSZ];
    current_context.uc_link = &final_context;
    current_context.uc_stack.ss_sp = current_stack;
    current_context.uc_stack.ss_size = sizeof(current_stack);

    // Thread executando é a main.
    current_tcb = malloc(sizeof(TCB_t));
    current_tcb->tid = 0;
    current_tcb->state = PROCST_EXEC;
    current_tcb->ticket = Random2() % 256;
    current_tcb->context = current_context;

    // Inicialização de final_context
    getcontext(&final_context);
    char final_stack[SIGSTKSZ];
    final_context.uc_stack.ss_sp = final_stack;
    final_context.uc_stack.ss_size = sizeof(final_stack);

    makecontext(&final_context, (void (*)(void))finishThread, 0);

    joined_tcbs = malloc(sizeof(JOINED));
    joined_tcbs_size++;
    int ret = CreateFila2(joined_tcbs);
    if (ret == -1){
        perror("[addJoinedTCBs]Erro ao criar joined_tcbs.\n");
    }

    createSchedulerContext();

    initialized = 1;
}

int getNewTID(){
    return ++thread_count;
}

int addReady(TCB_t* tcb){
    if (ready == NULL){
        ready = malloc(sizeof(TCB_t));
        ready_size++;

        int ret = CreateFila2(ready);
        if (ret == -1){
            perror("[addReady]Erro ao criar fila.\n");
            return -1;
        }
        return AppendFila2(ready, tcb);
    }
    else{
        ready = realloc(ready, sizeof(TCB_t)*(ready_size+1));
        ready_size++;
        return AppendFila2(ready, tcb);
    }
}

int addBlocked(TCB_t* tcb){
    if (blocked == NULL){

        blocked = malloc(sizeof(TCB_t));
        blocked_size++;

        int ret = CreateFila2(blocked);
        if (ret == -1){
            perror("[addBlocked]Erro ao criar fila.\n");
            return -1;
        }

        return AppendFila2(blocked, tcb);
    }
    else{
        blocked = realloc(blocked, sizeof(TCB_t)*(blocked_size+1));
        blocked_size++;
        return AppendFila2(blocked, tcb);
    }
}

int addJoinedTCBs(JOINED* joined){
    if (!initialized){
        init();
    }

    joined_tcbs = realloc(joined_tcbs, sizeof(JOINED)*(joined_tcbs_size+1));
    joined_tcbs_size++;
    return AppendFila2(joined_tcbs, joined);

}

/* Retorna o identificador da thread criada. Em caso de erro, retorna -1.*/
int ccreate (void* (*start)(void*), void *arg){
    // *arg contem o numero de parametros da funcao start e os parametros
    if (!initialized){
        init();
    }

    ucontext_t *new_context = malloc(sizeof(ucontext_t));
    char *stack = malloc(sizeof(char) * SIGSTKSZ);
    getcontext(new_context);

    // Possivelmente, precisaremos de uma funcao para generalizar este tratamento.
    new_context->uc_link = &final_context;
    new_context->uc_stack.ss_sp = stack;
    new_context->uc_stack.ss_size = SIGSTKSZ;

    TCB_t *tcb = malloc(sizeof(TCB_t));
    tcb->tid = getNewTID();
    tcb->state = PROCST_CRIACAO;
    tcb->ticket = Random2() % 256;
    tcb->context = *new_context;
    //printf("[ccreate] Incializou TCB com TID %d e ticket %d \n", tcb->tid, tcb->ticket);
    // antes era 0
    makecontext(&tcb->context, (void (*)(void)) start, 1, arg);
    int r = addReady(tcb);

    if (r != 0){
        perror("[ccreate] Erro ao adicionar elemento a READY\n");
        return -1;
    }

    return tcb->tid;
}

/* Libera a CPU voluntariamente. Retorna 0 quando executada com sucesso,
 -1 caso contrário.*/
int cyield (void){
    is_cyield = 1;

    if (!initialized){
        init();
    }
/*
    printf("[cyield] Thread %d vai ceder a CPU\n", current_tcb->tid);
    printf("[cyield] READY = ");
    printQueue(ready);
    printf("\n");
*/
    /* Flag é para controle de execução; garante que a thread não entrará
    em loop, dado que fazemos o getcontext() aqui.*/
    int flag = 0;

    if (getcontext(&(current_tcb->context)) == -1){
        perror("[cyield] Erro ao pegar o contexto da thread.");
        return -1;
    }

    if (flag == 0){
        flag = 1;

        addReady(current_tcb);

        scheduler();
    }

    return 0;
}

/* Sincronização de término: uma thread é bloqueada até que outra termine sua
execução. Quando essa thread terminar, retorna 0 se a execução terminou com
sucesso, -1 caso contrário.*/
int cjoin(int tid){
    int flag = 0;

    if (!initialized){
        init();
    }

    /* Threads que chamarem cjoin() para uma thread que ja está sincronizada com
    outra recebem -1 como retorno, bem como threads que já executaram. */
    if(existsTID(tid)){
        /* Verifica se alguma outra thread fez cjoin() para a thread em questão.
        Se não, prossegue. */
        if(joined_tcbs != NULL && FirstFila2(joined_tcbs) == 0){
            JOINED* item;
            item = (JOINED*)GetAtIteratorFila2(joined_tcbs);

            while(item != NULL && item->waiting_for_tid != tid){
                if(NextFila2(joined_tcbs) == 0){
                    item = (JOINED*)GetAtIteratorFila2(joined_tcbs);
                }
                else{
                    perror("[cjoin] Erro ao setar o iterador para o próximo item da fila.\n");
                    return -1;
                }
            }
            if (item != NULL && item->waiting_for_tid == tid){
                perror("[cjoin] Erro: já existe uma thread esperando por essa thread!");
                return -1;
            }
        }

        JOINED* joined;
        joined = (JOINED*)malloc(sizeof(JOINED));

        if (getcontext(&(current_tcb->context)) == -1){
            return -1;
        }

        if (flag == 0){
            flag = 1;
            joined->tid = current_tcb->tid;
            joined->waiting_for_tid = tid;
            addJoinedTCBs(joined);

            TCB_t* current_copy = malloc(sizeof(TCB_t));
            *current_copy = *current_tcb;
            current_copy->state = PROCST_BLOQ; // novo
            /* Retira a thread atual de execução e bloqueia. Chama o escalonador.*/
            if (addBlocked(current_copy) == -1){
                perror("[cjoin] Erro ao colocar thread na fila BLOCKED\n");
                return -1;
            }
            scheduler();
        }
    }
    else{
        return -1;
    }

    return 0;
}

/* sem é o semáforo a ser inicializado; count é um número positivo ou negativo
que representa a quantidade de recursos que o semáforo vai gerenciar.
Para exclusão mútua, count = 1.
Ainda, cada variável semáforo deve ter
associado uma estrutura que registre as threads que estão bloqueadas, esperando
por sua liberação. Na inicialização essa lista deve estar vazia.
Retorna 0 se executada corretamente, -1 caso contrário.*/
int csem_init (csem_t *sem, int count){
    if (!initialized){
        init();
    }

    int ret;
    sem->count = count;
    sem->fila = (FILA2 *) malloc(sizeof(FILA2));
    ret = CreateFila2(sem->fila);

    if (ret != 0){
        return -1;
    }

    return 0;
}

void createSchedulerContext(){
    scheduler_context.uc_stack.ss_sp = (char*) malloc(SIGSTKSZ);
    scheduler_context.uc_stack.ss_size = SIGSTKSZ;

    getcontext(&scheduler_context);
    makecontext(&scheduler_context, (void (*)(void)) scheduler, 0, NULL);
}

/* Solicitação de recurso. Retorna 0 se executada corretamente, -1 caso
contrário. */
int cwait (csem_t *sem){
    if (!initialized){
        init();
    }

    //printf("[cwait] thread %d solicitou recurso\n", current_tcb->tid);

    sem->count--;

    if (sem->count < 0){
        // Bloqueia a thread
        TCB_t* current_copy = malloc(sizeof(TCB_t));
        *current_copy = *current_tcb;
        current_copy->state = PROCST_BLOQ;

        //printf("[cwait] count = %d\n", sem->count);

        if (addBlocked(current_copy) == -1){
            perror("[cwait] Erro ao colocar thread na fila BLOCKED\n");
            return -1;
        }

        //printf("[cwait] Colocou thread %d em BLOCKED\n", current_copy->tid);

        // Adiciona thread na fila de espera desse semaforo e chama o escalonador.
        if (AppendFila2(sem->fila, current_copy) == 0){
            //printf("[cwait] Colocou thread %d na fila do semáforo.\n", current_copy->tid);
        }
        else{
            perror("[cwait] Nao colocou thread na fila de espera");
            return -1;
        }
/*
        printf("[cwait] READY = ");
        printQueue(ready);
        printf("\n");
*/
        //swapcontext(&current_copy->context, &scheduler_context); //&final_context
        int flag;
        flag = 0;
        if (getcontext(&(current_copy->context)) == -1){
            perror("[cyield] Erro ao pegar o contexto da thread.");
            return -1;
        }

        if (flag == 0){
            flag = 1;
            scheduler();
        }
    }

    // Se o recurso estiver livre, é atribuído para a thread e ela continua a execução.
    //printf("[cwait] Thread %d ganhou o recurso. Count = %d\n", current_tcb->tid, sem->count);

    return 0;
}

/* Liberação de recurso. Retorna 0 se executada corretamente, -1 caso contrário.*/
int csignal (csem_t *sem){
    if (!initialized){
        init();
    }
    sem->count++;

    //printf("[csignal] Thread %d liberou o recurso; count = %d\n", current_tcb->tid, sem->count);
    if(sem->count <= 0){ // Verifica se tem threads esperando
        /* Se houver threads esperando, segue a política FIFO e pega a primeira.
        A thread que estava esperando vai para o estado apto. */

        if (FirstFila2(sem->fila) == 0){
            TCB_t* thread = malloc(sizeof(TCB_t));
            thread = (TCB_t*)GetAtIteratorFila2(sem->fila);

            DeleteAtIteratorFila2(sem->fila);
            // Retira da lista de bloqueados e coloca em READY
            //printf("[csignal] Thread vai %d ser liberada\n", thread->tid);
            unlockThread(thread->tid);
        }
        else{
            perror("[csignal] Erro ao colocar o iterador no primeiro da fila.");
            return -1;
        }
    }

    return 0;
}


/* Fornece o nome dos integrantes do grupo.
name: ponteiro para uma área de memória onde deve ser escrito um string que
contém os nomes dos componentes do grupo e seus números de cartão. Deve ser
uma linha por componente.
size: quantidade máxima de caracteres que podem ser copiados para o string de
identificação dos componentes do grupo.
Retorna 0 se executada corretamente, -1 caso contrário. */
int cidentify (char *name, int size){
    name = malloc(sizeof(char) * size);

    char *student = "Isadora Pedrini Possebon - 00228551\n";
    int i;

    while (i < sizeof(student) && i < size){
        *name = student[i];
        name++;
        i++;
    }
    //strcpy("Isadora Possebon", name);
    return 0;
}

// Retira a thread da fila de bloqueados e coloca-a na fila de aptos.
int unlockThread(int tid){
    TCB_t* item;
    TCB_t* item_copy = (TCB_t*) malloc(sizeof(TCB_t));

    if (FirstFila2(blocked) == 0){
        item = (TCB_t*) GetAtIteratorFila2(blocked);
        while(item != NULL){
            if(item->tid == tid){
                *item_copy = *item;
                if (DeleteAtIteratorFila2(blocked) == 0){
                    item_copy->state = PROCST_APTO;
                    addReady(item_copy);
                    return 0;
                }
                else{
                    perror("[unlockThread] erro ao remover item da lista.");
                    return -1;
                }
            }
            NextFila2(blocked);
            item = (TCB_t*) GetAtIteratorFila2(blocked);
        }
    }
    return -1;
}

int isWaitingForAThread(int tid){
    JOINED *item;

    if (FirstFila2(joined_tcbs) == 0){
        item = (JOINED*) GetAtIteratorFila2(joined_tcbs);
        while (item != NULL){
            if (item->tid == tid){
                return 1;
            }
            if (NextFila2(joined_tcbs) == 0){
                item = (JOINED*) GetAtIteratorFila2(joined_tcbs);
            }
        }
    }
    return 0;
}

void finishThread(){
    JOINED *item;

    if (joined_tcbs != NULL && FirstFila2(joined_tcbs) == 0){
        /* Verifica se alguma thread está esperando a atual terminar. */
        item = (JOINED*) GetAtIteratorFila2(joined_tcbs);
        while (item != NULL && item->waiting_for_tid != current_tcb->tid){
            if (NextFila2(joined_tcbs) == 0){
                item = (JOINED*) GetAtIteratorFila2(joined_tcbs);
            }
        }
        // Se existe alguém esperando por esta thread
        if (item != NULL && item->waiting_for_tid == current_tcb->tid){
            // Apaga referência de espera
            int waiting_thread_tid;
            waiting_thread_tid = item->tid;
            DeleteAtIteratorFila2(joined_tcbs);

            // Se a thread não estiver esperando por mais nenhuma, desbloqueia-a.
            if (!isWaitingForAThread(waiting_thread_tid)){
                unlockThread(item->tid);
            }
        }
    }
    // provalvemente, aqui vai o free
    /*
    printf("[finishThread] Terminou a thread. Vai escolher outra. READY: ");
    printQueue(ready);
    printf("\n");
    */scheduler();
}

void scheduler(){
    int chosen_ticket = Random2() % 256;
    int to_execute_tid = -1;
    int minor_difference = 99999;
    int tickets_diff = 99999;
    int tids_diff = 0;
    TCB_t* item;
/*
    printf("[scheduler] Chosen ticket: %d\n", chosen_ticket);
    printf("[scheduler] READY = ");
    printQueue(ready);
    printf("\n");
*/
    if (FirstFila2(ready) == 0){
        item = (TCB_t*)GetAtIteratorFila2(ready);
        while(item != NULL){
            // Procura o bilhete sorteado - ou o mais próximo.
            tickets_diff = abs(chosen_ticket - item->ticket);
            if (tickets_diff < minor_difference){
                minor_difference = tickets_diff;
                to_execute_tid = item->tid;
            }
            else if (tickets_diff == minor_difference){
                // Compara TIDs. A thread de menor TID ganha a CPU
                tids_diff = to_execute_tid - item->tid;
                if (tids_diff > 0){
                    to_execute_tid = item->tid;
                }
            }
            if(NextFila2(ready) == 0){
                item = (TCB_t*)GetAtIteratorFila2(ready);
            }
            else{
                perror("[scheduler] Erro ao setar o iterador para o próximo item da fila de ready.\n");
                return;
            }
        }
    }
    //printf("[scheduler] Vai executar thread %d\n", to_execute_tid);
    dispatcher(to_execute_tid);
}

/* Retorna 0 se executou corretamente, -1 caso contrário.*/
void dispatcher(int scheduled_tid){
    TCB_t* item;
    TCB_t* thread_to_execute;
    TCB_t* previous_thread;

    /* Procura pela thread que deve ser executada.*/
    if(FirstFila2(ready) == 0){
        item = (TCB_t*)GetAtIteratorFila2(ready);

        while(item != NULL && item->tid != scheduled_tid){
            NextFila2(ready);
            item = (TCB_t*)GetAtIteratorFila2(ready);
        }
        if (item != NULL && item->tid == scheduled_tid){
            /* Retira a thread da fila de aptos, coloca na fila de executando e altera o seu estado. */
            //printf("[dispatcher] Encontrou a thread que deve ser executada. Vai executar %d\n", thread_to_execute->tid);
            thread_to_execute = malloc(sizeof(TCB_t));
            thread_to_execute = item;
            thread_to_execute->state = PROCST_EXEC;

            previous_thread = malloc(sizeof(TCB_t));
            previous_thread = current_tcb;

            current_tcb = thread_to_execute;

            TCB_t* item = malloc(sizeof(TCB_t));
            FirstFila2(ready);
            item = (TCB_t*) GetAtIteratorFila2(ready);

            while (item != NULL && item->tid != scheduled_tid){
                NextFila2(ready);
                item = (TCB_t*) GetAtIteratorFila2(ready);
            }


            if (item != NULL && item->tid == scheduled_tid){
                // Remove thread que será executada de READY
                if (DeleteAtIteratorFila2(ready) != 0){
                    perror("[dispatcher] Erro ao retirar elemento de READY");
                    exit(1);
                }
            }

            setcontext(&(thread_to_execute->context));
        }
    }
}

int existsInQueue(int tid, PFILA2 queue){
    TCB_t* item;

    if (queue == NULL){
        perror("[existsInQueue] Fila null.");
        return 0;
    }
    if(FirstFila2(queue) == 0){

        item = (TCB_t*)GetAtIteratorFila2(queue);
        while(item != NULL && item->tid != tid){
            if(NextFila2(queue) == 0){
                item = (TCB_t*)GetAtIteratorFila2(queue);
                if (item == NULL){
                    //perror("[existsInQueue] Erro no iterator\n");
                    return 0;
                }
            }
            else{
                perror("[existsInQueue]: Erro ao setar o iterador para o próximo item da fila.\n");
                return 0;
            }
        }
        if (item->tid == tid){
            return 1;
        }
    }
    return 0;
}

/* Retorna 1 se existe uma thread com a TID em questão; 0, caso contrário.*/
int existsTID(int tid){
    if (existsInQueue(tid, ready) == 1
        || existsInQueue(tid, blocked) == 1
        || current_tcb->tid == tid){
        return 1;
    }
    return 0;
}

/* Retorna TCB correspondente ao TID. NULL, se tid não for encontrado.*/
TCB_t* getTCB(int tid){

    if (existsInQueue(tid, ready)){
        TCB_t* item;

        if(FirstFila2(ready) == 0){
            item = (TCB_t*)GetAtIteratorFila2(ready);
            while(item != NULL && item->tid != tid){
                if(NextFila2(ready) == 0){
                    item = (TCB_t*)GetAtIteratorFila2(ready);
                }
                else{
                    perror("[getTCB]: Erro ao setar o iterador para o próximo item da fila.\n");
                }
            }
            if (item->tid == tid){
                return item;
            }
        }
    }
    else if(existsInQueue(tid, blocked)){
        TCB_t* item;

        if(FirstFila2(blocked) == 0){
            item = (TCB_t*)GetAtIteratorFila2(blocked);
            while(item != NULL && item->tid != tid){
                if(NextFila2(blocked) == 0){
                    item = (TCB_t*)GetAtIteratorFila2(blocked);
                }
                else{
                    perror("[getTCB]: Erro ao setar o iterador para o próximo item da fila.\n");
                }
            }
            if (item->tid == tid){
                return item;
            }
        }
    }
    return NULL;
}

void printQueue(PFILA2 queue){
    TCB_t* item;

    if (FirstFila2(queue) == 0){
      item = (TCB_t*)GetAtIteratorFila2(queue);
      while(item != NULL){
        printf(" %d ", item->tid);
        if(NextFila2(queue) == 0){
          item = (TCB_t*)GetAtIteratorFila2(queue);
        }
        else{
          perror("[printQueue] Erro ao setar o iterador para o próximo item da fila.\n");
        }
      }
    }
    else{
        perror("[printQueue] Fila vazia.\n");
    }
}
