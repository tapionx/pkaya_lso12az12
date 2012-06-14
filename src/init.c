/* MODULO RESPONSABILE PER L'INIZIALIZZAZIONE DELLE CPU E DEI RELATIVI REGISTRI */
/* Da specifica */
#include "const.h"
#include "uMPStypes.h"

/* Custom */
#include "utils.h"
#include "myconst.h"
#include "handlers.h"

/* Questa funzione si occupa di inizializzare un singolo slot di 
 * new area, passato come parametro, secondo le specifiche di phase2 */
HIDDEN void initNewArea(state_t toinit[]){
	int i;
	for (i=0; i<NUM_AREAS; i+=2){ /* new area hanno id pari, max 8 aree */
		toinit[i].reg_sp = RAMTOP;
		toinit[i].status |= (STATUS_TE)&~(STATUS_VMc|STATUS_KUc|STATUS_INT_UNMASKED);
		switch(i){
			case NEW_SYSBP: toinit[i].pc_epc = toinit[i].reg_t9 = (memaddr)sysbp_handler; break;
			case NEW_TRAP: toinit[i].pc_epc = toinit[i].reg_t9 =  (memaddr)trap_handler; break;
			case NEW_TLB: toinit[i].pc_epc = toinit[i].reg_t9 =  (memaddr)tlb_handler; break;
			case NEW_INTS: toinit[i].pc_epc = toinit[i].reg_t9 =  (memaddr)ints_handler; break;
			default: PANIC();
		}
	}
}

/* Questa funzione si occupa di inizializzare le new area di tutte le
 * cpu installate */
void initNewAreas(state_t toinit[][NUM_AREAS], const int ncpu){
	int id;
	for (id=0; id<ncpu; id++){ /* per ogni cpu installata */
		initNewArea(toinit[id]);
	}
}


