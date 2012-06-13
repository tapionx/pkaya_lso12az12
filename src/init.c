/* MODULO RESPONSABILE PER L'INIZIALIZZAZIONE DELLE CPU E DEI RELATIVI REGISTRI */
/* Da specifica */
#include "const.h"
#include "uMPStypes.h"

/* Custom */
#include "utils.h"
#include "myconst.h"
#include "handlers.h"

state_t new_old_areas[NUM_CPU][NUM_AREAS]; /* 8 areas for each cpu */

/* Si occupa di avviare tutte le CPU (populando le relative new area) specificate nel valore NUM_CPU */
void initCPUs(){
	/* Default values per le new area */
	state_t defaults;
	initState_t(&(defaults));
	defaults.reg_sp = RAMTOP;
	defaults.status = 0|STATUS_IEc|STATUS_TE; /* 0 is safe */
	/* Init: per ogni cpu bisogna inizializzare le 4 new area come da specifica */
	int id=1; /* partiamo dalla seconda CPU */
	for (id; id<NUM_CPU; id++){
		new_old_areas[id][NEW_SYSBP].pc_epc = (memaddr)sysbp_handler;
		new_old_areas[id][NEW_TRAP].pc_epc = (memaddr)trap_handler;
		new_old_areas[id][NEW_TLB].pc_epc = (memaddr)tlb_handler;
		new_old_areas[id][NEW_INTS].pc_epc = (memaddr)ints_handler;
		INITCPU(id, &defaults, &(new_old_areas[id]));
	}
}


