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

/* Quanti frame sono riservati dopo RAMTOP per le aree delle altre CPU? */
#define RES_FRAMES NUM_CPU-1

/* Strutture da inizializzare */
/* KERNEL */
extern int locks[MAXPROC];
extern state_t* pnew_old_areas[NUM_CPU][NUM_AREAS]; /* 8 areas for each cpu */
extern state_t pstate[NUM_CPU];
state_t new_old_areas[NUM_CPU][NUM_AREAS];
/* SCHEDULER */
extern struct list_head readyQueue[NUM_CPU][MAX_PCB_PRIORITY];
extern pcb_t *currentProc[NUM_CPU];
extern memaddr sp[NUM_CPU];

/*****************************************************************************/

/* Questa funzione si occupa di popolare le New area dell'array preso
 * come parametro. L'array deve essere di puntatori a state_t */
HIDDEN void populateNewAreas(int cpuid)
{
	int id;
	//U32 defaultStatus = (STATUS_TE)&~(STATUS_VMc|STATUS_KUc|STATUS_INT_UNMASKED);
	state_t *temp; /* Temp state_t da usare per inizializzare le aree */
	for (id=1; id<NUM_AREAS; id+=2){ /* Le New area sono in pos dispari */
		/* Gli stack delle varie new area sono adiacenti e ognuno occupa
		 * esattamente 1 frame (4KB) per evitare stack smashing */
		temp = pnew_old_areas[cpuid][id];
		STST(temp);
		/* Lo stack va specificato solo per CPU != 0 */
		if (cpuid != 0){
			U32 stackAddr = ROM_RES_FRAME_END+(cpuid*FRAME_SIZE/4);
			temp->reg_sp = stackAddr;
		}
		temp->status = getSTATUS();	
				
		switch(id){
			case NEW_SYSBP:	
				temp->pc_epc = temp->reg_t9 = (memaddr)sysbp_handler;
				break;
			case NEW_TRAP:
				temp->pc_epc = temp->reg_t9 = (memaddr)trap_handler;
				break;
			case NEW_TLB:
				temp->pc_epc = temp->reg_t9 = (memaddr)tlb_handler;
				break;
			case NEW_INTS:
				temp->pc_epc = temp->reg_t9 = (memaddr)ints_handler;
				break;
			default:
				PANIC();
		}
	}
}

/* Questa funzione si occupa di inizializzare le New/Old area in base
 * alla CPU (CPU0 avrà ROM Reserved Frame, le altre un array dedicato) */
void initAreas()
{
	int id;
	/* Faccio puntare le aree delle altre CPU all'array dichiarato */
	for(id=0;id<NUM_CPU;id++){
		pnew_old_areas[id][OLD_SYSBP] = &(new_old_areas[id][OLD_SYSBP]);
		pnew_old_areas[id][NEW_SYSBP] = &(new_old_areas[id][NEW_SYSBP]);
		pnew_old_areas[id][OLD_TRAP] = &(new_old_areas[id][OLD_TRAP]);
		pnew_old_areas[id][NEW_TRAP] = &(new_old_areas[id][NEW_TRAP]);
		pnew_old_areas[id][OLD_TLB] = &(new_old_areas[id][OLD_TLB]);
		pnew_old_areas[id][NEW_TLB] = &(new_old_areas[id][NEW_TLB]);
		pnew_old_areas[id][OLD_INTS] = &(new_old_areas[id][OLD_INTS]);
		pnew_old_areas[id][NEW_INTS] = &(new_old_areas[id][NEW_INTS]);
		/* Populo tutte le new area della CPU id */
		populateNewAreas(id);
	}
}

/* Questa funzione inizializza semplicemente tutte le readyQueue di 
 * tutte le CPU in modo da non incorrere in errori nell'utilizzo delle
 * funzioni di listx.h */
void initReadyQueues()
{
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
 * lo scheduler (eseguito SOLO da CPU0) */
void initCpus()
{
	/* Inizializzo le altre CPU a partire dalla CPU0 */
	STST(&(pstate[0]));
	int id;
	for (id=0; id<NUM_CPU; id++){
		U32 curStatus = getSTATUS();
		pstate[id].status = (curStatus|STATUS_IEp|STATUS_INT_UNMASKED)\
							&~(STATUS_VMc);
		/* Tutte le CPU iniziano eseguendo lo scheduler */
		pstate[id].pc_epc = pstate[id].reg_t9 = (memaddr)scheduler;
		/* Mi assicuro che non ci sia stack smashing tra le CPU */
		pstate[id].reg_sp = RAMTOP-((id*(PFRAMES/NUM_CPU))*FRAME_SIZE);
		debug(id, pstate[id].reg_sp);
		if (id != 0) INITCPU(id, &(pstate[id]), pnew_old_areas[id][0]);
	}
	/* Infine reinizializzo CPU0 in modo che lo stack per i processi inizi
	 * ad un indirizzo che non provochi smashing */
	LDST(&(pstate[0]));
}

/* Questa funzione serve per inizializzare la Interrupt Routing Table 
 * Vogliamo una politica di routing degli interrupt dinamica */
void initIRT()
{
	int line, dev;
	for (line=2; line<8; line++)
		for(dev=0; dev<8; dev++){
			memaddr *entry = (memaddr *)IRT_ENTRY(line,dev);
			/* Devo abilitare la IRT dinamica e solo le CPU installate! */
			*entry = DYN_IRT_MASK+pow(2, NUM_CPU)-1;
		}
	
}

/* Questa funzione inizializza il vettore dei lock a PASS */
void initLock()
{
	int i;
	for(i=0;i<MAXPROC;i++)
	{
		locks[i] = PASS;
	}
}

/* Questa funzione azzera gli stack pointer relativi ad ogni CPU */
void initSP(){
	int i = 0;
	for(; i<NUM_CPU; i++){
		sp[i] = 0;
	}
}
