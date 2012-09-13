#include "libumps.h"
#include "myConst.h"
#include "const.h"
#include "uMPStypes.h"
#include "types11.h"
#include "handlers.h"
#include "scheduler.h"

void prova_altracpu(){
	//setTIMER(50);
	//setSTATUS(PROCESS_STATUS);
	int i=0;
	while(TRUE){
		i++;
		i++;
	}
	//SYSCALL(0,0,0,0);
}

/* Variabili del nucleo */
int processCount; /* Contatore della totalita' dei processi */ 
int softBlockCount; /* Contatore dei processi bloccati su semafori */
struct list_head readyQueue; /* Coda dei processi in stato ready */
pcb_t *currentProcess; /* Puntatore al processo correntemente in esecuzione */
state_t areas[NUM_CPU][NUM_AREAS];
int locks[MAXPROC+MAX_DEVICES]; /* Variabili di condizione per CAS */
state_t scheduler_states[NUM_CPU]; /* state_t dello scheduler */

/** L'esecuzione del kernel inizia da qui */
int main(){
	/** INIT CPU0 */
	/* Init delle new area */
		cleanState((state_t *)INT_NEWAREA);
		((state_t *)INT_NEWAREA)->pc_epc = ((state_t *)INT_NEWAREA)->reg_t9 = (memaddr)int_handler;
		((state_t *)INT_NEWAREA)->reg_sp = RAMTOP;
		((state_t *)INT_NEWAREA)->status = EXCEPTION_STATUS;
		cleanState((state_t *)TLB_NEWAREA);
		((state_t *)TLB_NEWAREA)->pc_epc = ((state_t *)TLB_NEWAREA)->reg_t9 = (memaddr)tlb_handler;
		((state_t *)TLB_NEWAREA)->reg_sp = RAMTOP;
		((state_t *)TLB_NEWAREA)->status = EXCEPTION_STATUS;
		cleanState((state_t *)PGMTRAP_NEWAREA);
		((state_t *)PGMTRAP_NEWAREA)->pc_epc = ((state_t *)PGMTRAP_NEWAREA)->reg_t9 = (memaddr)pgmtrap_handler;
		((state_t *)PGMTRAP_NEWAREA)->reg_sp = RAMTOP;
		((state_t *)PGMTRAP_NEWAREA)->status = EXCEPTION_STATUS;
		cleanState((state_t *)SYSBK_NEWAREA);
		((state_t *)SYSBK_NEWAREA)->pc_epc = ((state_t *)SYSBK_NEWAREA)->reg_t9 = (memaddr)sysbk_handler;
		((state_t *)SYSBK_NEWAREA)->reg_sp = RAMTOP;
		((state_t *)SYSBK_NEWAREA)->status = EXCEPTION_STATUS;
	
	/* Init strutture phase1 */
	initPcbs();
	initASL();	
	
	/** INIT CPU > 0 */
	if (NUM_CPU > 1){
		/* Init delle new area */
		int cpuid,area;
		state_t *currentArea;
		
		for (cpuid=1; cpuid<NUM_CPU; cpuid++){
			currentArea = &(areas[cpuid][INT_NEWAREA_INDEX]);
			currentArea->pc_epc = currentArea->reg_t9 = (memaddr)int_handler;
			currentArea->reg_sp = RAMTOP-(cpuid*FRAME_SIZE);
			currentArea->status = EXCEPTION_STATUS;	
			
			currentArea = &(areas[cpuid][TLB_NEWAREA_INDEX]);
			currentArea->pc_epc = currentArea->reg_t9 = (memaddr)tlb_handler;
			currentArea->reg_sp = RAMTOP-(cpuid*FRAME_SIZE);
			currentArea->status = EXCEPTION_STATUS;	
			
			currentArea = &(areas[cpuid][PGMTRAP_NEWAREA_INDEX]);
			currentArea->pc_epc = currentArea->reg_t9 = (memaddr)int_handler;
			currentArea->reg_sp = RAMTOP-(cpuid*FRAME_SIZE);
			currentArea->status = EXCEPTION_STATUS;	
			
			currentArea = &(areas[cpuid][SYSBK_NEWAREA_INDEX]);
			currentArea->pc_epc = currentArea->reg_t9 = (memaddr)int_handler;
			currentArea->reg_sp = RAMTOP-(cpuid*FRAME_SIZE);
			currentArea->status = EXCEPTION_STATUS;	
		}
	}
	
	/** AZZERAMENTO DEI LOCK */
	int i = 0;
	for(i=0;i<(MAXPROC+MAX_DEVICES);i++){
		//locks[i] = 0;
	}
	
	/** INIZIALIZZAZIONE DELLO SCHEDULER */
	
	mkEmptyProcQ(&(readyQueue)); /* Inizializzo la ready queue */
	for(i=0;i<NUM_CPU;i++){
		STST(&scheduler_states[i]);
		scheduler_states[i].reg_sp = RAMTOP-(NUM_CPU*FRAME_SIZE)-(i*FRAME_SIZE);
		scheduler_states[i].pc_epc = scheduler_states[i].reg_t9 = (memaddr)scheduler;
		scheduler_states[i].status = scheduler_states[i].status | PROCESS_STATUS;
	} 
	
	/////////////////////////////
	
	pcb_t prova;
	STST(&prova.p_s);
	prova.p_s.pc_epc = prova.p_s.reg_t9 = (memaddr)prova_altracpu;
	prova.p_s.status = prova.p_s.status | PROCESS_STATUS;
	
	
	for(i=0;i<NUM_CPU;i++){
		prova.p_s.reg_sp = RAMTOP-(NUM_CPU*FRAME_SIZE)-(i*FRAME_SIZE);
		addReady(&prova);
		INITCPU(i, &scheduler_states[i], &areas[i]);
	}
	
	return 0;
}
