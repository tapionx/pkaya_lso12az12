#ifndef __UTILS
#define __UTILS


/** Utilities, constants and tools */
#include "types11.h"
#include "listx.h"

/* Funzione ausiliaria per la "dummy initialization" degli stati delle CPU a 0 */
void cleanState(state_t *state);
/* Funzione per copiare uno state_t in un altro */
void copyState(state_t *src, state_t *dest);

/* Funzione per l'inizializzazione dello stato di un pcb_t dai parametri */
pcb_t *initPcbState(pcb_t *pcb, U32 status, memaddr pc_epc, U32 reg_sp);

/* debugging functions */
void debug(int row, int var); /* value of the var variable at the row line in source */
void debugn(char* s, int n); /* identico alla printn, comodo da trovare */
void debugs(char *str);

/* term printing */
void addokbuf(char *strp);
void adderrbuf(char *strp);

/* converts int to string */
char *itoa(int i);
/* printf like funcion, replace % with int */
void printn(char* s, int n);

void printn_mutex(char* s, int n);
void stampaCodaMutex(int semKey);
void stampaCodaHeadMutex(struct list_head *head);

/* Trova l'id della CPU con minor workload in un array di procs counter */
int minWorkloadCpu(int procs[], int size);

/* Acquisizione di un lock passato come parametro */
void lock(int semKey);

/* Liberazione di un lock passato come parametro */
void free(int semKey);

/* Stampa la coda dei processi bloccati sul semaforo con chiave semKey */
void stampaCoda(int semKey);

/* Stampa la coda la cui sentinella è head */
void stampaCodaHead(struct list_head *head);

/* Restituisce il numero del device che ha sollevato l'interrupt pendente 
 * @bitmap : [U32] la PENDING_INT_BITMAP */
int getDevNo(U32 bitmap);

#endif
