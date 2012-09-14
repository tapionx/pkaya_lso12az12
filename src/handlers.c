#include "libumps.h"
#include "myConst.h"
#include "const.h"
#include "uMPStypes.h"
#include "types11.h"
#include "handlers.h"
#include "scheduler.h"
#include "utils.h"
#include "kernelVariables.h"
extern void prova_altracpu();
void sysbk_handler(){

}

void int_handler(){
	U32 cpuid = getPRID();
	/* Capiamo da che linea proviene l'interrupt */
	int line = 0;
	for (line; line < NUM_LINES; line++){
		/* Se abbiamo trovato la linea usciamo */
		if (CAUSE_IP_GET(getCAUSE(), line)) break;
	}
	switch(line){
		case INT_PLT:
			copyState(pareas[cpuid][INT_OLDAREA_INDEX], &(currentProcess[cpuid]->p_s));
			//lock(SCHEDULER_LOCK);
			addReady(currentProcess[cpuid]);
			//free(SCHEDULER_LOCK);
			LDST(&(scheduler_states[cpuid]));	
	}
	
}

void tlb_handler(){

}

void pgmtrap_handler(){

}
