#include "types11.h"
#include "myConst.h"
#include "libumps.h"
#include "pcb.e"
#include "asl.e"
#include "kernelVariables.h"

/**
 * Questa funzione si occupa di aggiungere un pcb_t (puntato da proc)
 * alla readyQueue dello scheduler. Agisce già in mutua esclusione
 * utilizzando la CAS e un intero globale per lo scheduler.
 */
void addReady(pcb_t *proc){
	lock(SCHEDULER_LOCK);
	insertProcQ(&(readyQueue), proc);
	free(SCHEDULER_LOCK);
}

/**
 * Funzione principale per lo scheduling dei processi
 */
void scheduler(){
	int cpuid = getPRID();
	while(TRUE){
		lock(SCHEDULER_LOCK);
		if(!emptyProcQ(&(readyQueue))){
			currentProcess[cpuid] = removeProcQ(&(readyQueue));
			/* Settiamo il TIME_SLICE un istante prima di mandare il processo
			 * in esecuzione */
			setTIMER(TIME_SLICE);
			free(SCHEDULER_LOCK);
			LDST(&(currentProcess[cpuid]->p_s));
		}
		free(SCHEDULER_LOCK);
	}
}
