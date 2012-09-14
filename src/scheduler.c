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
	while(!emptyProcQ(&(readyQueue))){
		lock(SCHEDULER_LOCK);
		currentProcess[cpuid] = removeProcQ(&(readyQueue));
		free(SCHEDULER_LOCK);
		/* Settiamo il TIME_SLICE un istante prima di mandare il processo
		 * in esecuzione */
		setTIMER(TIME_SLICE);
		LDST(&(currentProcess[cpuid]->p_s));
	}
	WAIT();
}
