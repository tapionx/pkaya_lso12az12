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
	lock(SCHEDULER_SEMLOCK);
	if (proc != NULL)
		insertProcQ(&(readyQueue), proc);
	free(SCHEDULER_SEMLOCK);
}

/**
 * Funzione principale per lo scheduling dei processi
 */
void scheduler(){
	/* TODO: ricordarsi di gestire i casi di deadlock (procCount e softProcCount) 
	 * e di modificare tutte le variabili del caso nelle syscall ecc. */
	int cpuid = getPRID();
	while(TRUE){
		lock(SCHEDULER_SEMLOCK);
		if(!emptyProcQ(&(readyQueue))){
			currentProcess[cpuid] = removeProcQ(&(readyQueue));
			/* Se il processo è da terminare ne liberiamo il pcb e lo scartiamo */
			if (currentProcess[cpuid]->wanted){
				freePcb(currentProcess[cpuid]);
				free(SCHEDULER_SEMLOCK);
				LDST(&(scheduler_states));
			}
			/* Settiamo il TIME_SLICE un istante prima di mandare il processo
			 * in esecuzione */
			setTIMER(TIME_SLICE);
			free(SCHEDULER_SEMLOCK);
			/* Setto lo pseudo-clock-timer (SOLO LA PRIMA VOLTA, si
			 * resetta ad ogni interrupt/V) */
			if (!pctInit){
				SET_IT(SCHED_PSEUDO_CLOCK);
				pctInit = TRUE;
			}
			//debug(100,100);
			LDST(&(currentProcess[cpuid]->p_s));
		}
		free(SCHEDULER_SEMLOCK);
	}
}
