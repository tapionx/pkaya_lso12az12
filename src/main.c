#include "libumps.h"
#include "myConst.h"
#include "const.h"
#include "uMPStypes.h"
#include "types11.h"

/* Indici dei semafori (da usare come variabili di condizione) */
int locks[NUM_CPU];

void sysbk_handler(){
	debug(69, 1);
}

void int_handler(){
	debug(69, 2);
}

void tlb_handler(){
	debug(69, 3);
}

void pgmtrap_handler(){
	debug(69, 4);
}

void prova_altracpu(){
	debug(2,2);
	setTIMER(50);
	setSTATUS(PROCESS_STATUS);
	while(TRUE);
	//SYSCALL(0,0,0,0);
}

/* Variabili del nucleo */
int processCount; /* Contatore della totalita' dei processi */ 
int softBlockCount; /* Contatore dei processi bloccati su semafori */
struct list_head readyQueue; /* Coda dei processi in stato ready */
pcb_t *currentProcess; /* Puntatore al processo correntemente in esecuzione */
state_t areas[NUM_CPU][NUM_AREAS];

/** L'esecuzione del kernel inizia da qui */
int main(){
	debug(0, PROCESS_STATUS);
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
	state_t prova;
	STST(&prova);
	prova.pc_epc = prova.reg_t9 = (memaddr)prova_altracpu;
	prova.reg_sp = RAMTOP-(5*FRAME_SIZE);
	INITCPU(1, &prova, &(areas[1]));
	while(TRUE);
	return 0;
}
