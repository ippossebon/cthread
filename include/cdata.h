/*
 * cdata.h: arquivo de inclus�o de uso apenas na gera��o da libpithread
 *
 * Esse arquivo pode ser modificado. ENTRETANTO, deve ser utilizada a TCB fornecida.
 *
 */
#ifndef __cdata__
#define __cdata__
#include <ucontext.h>

#define	PROCST_CRIACAO	0
#define	PROCST_APTO	1
#define	PROCST_EXEC	2
#define	PROCST_BLOQ	3
#define	PROCST_TERMINO	4

/* NÂO ALTERAR ESSA struct */
typedef struct s_TCB {
	int		tid; 		// identificador da thread
	int		state;		// estado em que a thread se encontra
					// 0: Criação; 1: Apto; 2: Execuçãoo; 3: Bloqueado e 4: Término
        int		ticket;		// 0-255: bilhete de loteria da thread
	ucontext_t 	context;	// contexto de execu��o da thread (SP, PC, GPRs e recursos)
} TCB_t;

typedef struct s_JOINED{
	int tid;
	int waiting_for_tid;
} JOINED;

#endif
