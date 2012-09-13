#include "types11.h"
#include "myConst.h"
#include "libumps.h"
#include "pcb.e"
#include "asl.e"

extern int processCount; /* Contatore della totalita' dei processi */ 
extern int softBlockCount; /* Contatore dei processi bloccati su semafori */
extern struct list_head readyQueue; /* Coda dei processi in stato ready */
extern pcb_t *currentProcess; /* Puntatore al processo correntemente in esecuzione */
int locks[MAXPROC+MAX_DEVICES]; /* Variabili di condizione per CAS */

void addReady(pcb_t *proc){
	lock(SCHEDULER_LOCK);
	insertProcQ(&(readyQueue), proc);
	free(SCHEDULER_LOCK);
}

void scheduler(){
	while(!emptyProcQ(&(readyQueue))){
		lock(SCHEDULER_LOCK);
		pcb_t *torun = removeProcQ(&(readyQueue));
		free(SCHEDULER_LOCK);
		LDST(&(torun->p_s));
	}
}
