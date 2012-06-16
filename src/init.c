/* MODULO RESPONSABILE PER L'INIZIALIZZAZIONE DELLE CPU E DEI RELATIVI REGISTRI */
/* Da specifica */
#include "const.h"
#include "uMPStypes.h"
#include "pcb.e"

/* Custom */
#include "utils.h"
#include "myconst.h"
#include "handlers.h"
#include "scheduler.h"

/* Strutture da inizializzare */
/* KERNEL */
extern state_t* pnew_old_areas[NUM_CPU][NUM_AREAS]; /* 8 areas for each cpu */
extern state_t pstate[NUM_CPU];
state_t new_old_areas[NUM_CPU][NUM_AREAS];
/* SCHEDULER */
extern struct list_head readyQueue[NUM_CPU][MAX_PCB_PRIORITY];
extern pcb_t *currentProc[NUM_CPU];

/*****************************************************************************/

/* Questa funzione si occupa di popolare le New area dell'array preso
 * come parametro. L'array deve essere di puntatori a state_t */
HIDDEN void populateNewAreas(int cpuid){
	int id;
	state_t **areas = pnew_old_areas[cpuid]; 
	U32 defaultStatus = (STATUS_TE)&~(STATUS_VMc|STATUS_KUc|STATUS_INT_UNMASKED);
	for (id=0; id<NUM_AREAS; id+=2){ /* Le New area sono in pos pari */
		areas[id]->reg_sp = RAMTOP-(cpuid*FRAME_SIZE); /* No smashed stack */
		areas[id]->status = defaultStatus;
		switch(id){
			case NEW_SYSBP:
				areas[id]->pc_epc = areas[id]->reg_t9 = (memaddr)sysbp_handler;
				break;
			case NEW_TRAP:
				areas[id]->pc_epc = areas[id]->reg_t9 = (memaddr)trap_handler;
				break;
			case NEW_TLB:
				areas[id]->pc_epc = areas[id]->reg_t9 = (memaddr)tlb_handler;
				break;
			case NEW_INTS:
				areas[id]->pc_epc = areas[id]->reg_t9 = (memaddr)ints_handler;
				break;
			default:
				PANIC();
		}
	}
}

/* Questa funzione si occupa di inizializzare le New/Old area in base
 * alla CPU (CPU0 avrà ROM Reserved Frame, le altre un array dedicato) */
void initAreas(){
	int id;
	/* Faccio in modo che le aree di CPU0 puntino al Rom Reserved Frame */
	pnew_old_areas[0][NEW_SYSBP] = (state_t*)SYSBK_NEWAREA;
	pnew_old_areas[0][OLD_SYSBP] = (state_t*)SYSBK_OLDAREA;
	pnew_old_areas[0][NEW_TRAP] = (state_t*)PGMTRAP_NEWAREA;
	pnew_old_areas[0][OLD_TRAP] = (state_t*)PGMTRAP_OLDAREA;
	pnew_old_areas[0][NEW_TLB] = (state_t*)TLB_NEWAREA;
	pnew_old_areas[0][OLD_TLB] = (state_t*)TLB_OLDAREA;
	pnew_old_areas[0][NEW_INTS] = (state_t*)INT_NEWAREA;
	pnew_old_areas[0][OLD_INTS] = (state_t*)INT_OLDAREA;
	populateNewAreas(0);
	/* Faccio puntare le aree delle altre CPU all'array dichiarato */
	for(id=1;id<NUM_CPU;id++){
		pnew_old_areas[id][NEW_SYSBP] = &(new_old_areas[id][NEW_SYSBP]);
		pnew_old_areas[id][OLD_SYSBP] = &(new_old_areas[id][OLD_SYSBP]);
		pnew_old_areas[id][NEW_TRAP] = &(new_old_areas[id][NEW_TRAP]);
		pnew_old_areas[id][OLD_TRAP] = &(new_old_areas[id][OLD_TRAP]);
		pnew_old_areas[id][NEW_TLB] = &(new_old_areas[id][NEW_TLB]);
		pnew_old_areas[id][OLD_TLB] = &(new_old_areas[id][OLD_TLB]);
		pnew_old_areas[id][NEW_INTS] = &(new_old_areas[id][NEW_INTS]);
		pnew_old_areas[id][OLD_INTS] = &(new_old_areas[id][OLD_INTS]);
		/* Populo tutte le new area della CPU id */
		populateNewAreas(id);
	}
}

/* Questa funzione inizializza semplicemente tutte le readyQueue di 
 * tutte le CPU in modo da non incorrere in errori nell'utilizzo delle
 * funzioni di listx.h */
void initReadyQueues(){
	int id, priority;
	for (id=0; id<NUM_CPU; id++){
		for (priority=0; priority<MAX_PCB_PRIORITY; priority++){
			mkEmptyProcQ(&(readyQueue[id][priority]));
		}
		/* Inizializzo anche il vettore dei currentProcess */
		currentProc[id] = NULL;
	}
}

/* Funzione che serve per inizializzare le CPU > 0 e avvia su ognuna
 * lo scheduler */
void initCpus(){
	STST(&(pstate[0]));
	int id;
	for (id=1; id<NUM_CPU; id++){
		pstate[id].status = getSTATUS();
		pstate[id].pc_epc = pstate[id].reg_t9 = (memaddr)scheduler;
		/* Mi assicuro che non ci sia stack smashing tra gli scheduler */
		pstate[id].reg_sp = pstate[0].reg_sp - (id*FRAME_SIZE);
		INITCPU(id, &(pstate[id]), pnew_old_areas[id][0]);
	}
}

/* Questa funzione serve per inizializzare la Interrupt Routing Table 
 * Vogliamo una politica di routing degli interrupt dinamica */
void initIRT(){
	int line, dev;
	for (line=2; line<8; line++)
		for(dev=0; dev<8; dev++){
			memaddr *entry = (memaddr *)IRT_ENTRY(line,dev);
			*entry = DEFAULT_IRT_MASK;
		}
}
