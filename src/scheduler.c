#include "types11.h"
#include "myConst.h"
#include "libumps.h"
#include "pcb.e"
#include "asl.e"
#include "kernelVariables.h"

void addReady(pcb_t *proc){
	lock(SCHEDULER_LOCK);
	insertProcQ(&(readyQueue), proc);
	free(SCHEDULER_LOCK);
}

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
