/* MODULO RESPONSABILE PER L'INIZIALIZZAZIONE DELLE CPU E DEI RELATIVI REGISTRI */
/* Da specifica */
#include "const.h"
#include "uMPStypes.h"

/* Custom */
#include "utils.h"
#include "myconst.h"
#include "handlers.h"

extern struct list_head readyQueue[NUM_CPU][MAX_PCB_PRIORITY];

extern state_t* pnew_old_areas[NUM_CPU][NUM_AREAS]; /* 8 areas for each cpu */
state_t new_old_areas[NUM_CPU][NUM_AREAS];

/* Questa funzione si occupa di popolare le New area dell'array preso
 * come parametro. L'array deve essere di puntatori a state_t */
HIDDEN void populateNewAreas(state_t* areas[]){
	int id;
	U32 defaultStatus = (STATUS_TE)&~(STATUS_VMc|STATUS_KUc|STATUS_INT_UNMASKED);
	for (id=0; id<NUM_AREAS; id+=2){ /* Le New area sono in pos pari */
		areas[id]->reg_sp = RAMTOP-(id*FRAME_SIZE); /* No smashed stack */
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
		populateNewAreas(pnew_old_areas[id]);
	}
}

void initReadyQueues(){
	
}


