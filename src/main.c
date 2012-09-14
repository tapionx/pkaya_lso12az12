#include "libumps.h"
#include "myConst.h"
#include "const.h"
#include "uMPStypes.h"
#include "types11.h"
#include "handlers.h"
#include "scheduler.h"

/* Variabili del nucleo */
int processCount; /* Contatore della totalita' dei processi */ 
int softBlockCount; /* Contatore dei processi bloccati su semafori */
struct list_head readyQueue; /* Coda dei processi in stato ready */
pcb_t *currentProcess; /* Puntatore al processo correntemente in esecuzione */
state_t areas[NUM_CPU][NUM_AREAS]; /* Aree reali per CPU > 0 */
state_t *pareas[NUM_CPU][NUM_AREAS]; /* Puntatori alle aree di tutte le CPU */
int locks[MAXPROC+MAX_DEVICES]; /* Variabili di condizione per CAS */
state_t scheduler_states[NUM_CPU]; /* state_t dello scheduler */

int common; /* VARIABILE DI PROVA */

void prova_altracpu(){
	int locknum = 5;
	while(TRUE){
		lock(locknum);
		common = getPRID();
		debug(getPRID(), common);
		free(locknum);
	}
}

/** L'esecuzione del kernel inizia da qui */
int main(){
	/* Disabilito tutto per sicurezza */
	/** INIT CPU0 */
	/* Init delle new area */
		pareas[0][INT_NEWAREA_INDEX] = (state_t *)INT_NEWAREA;
		pareas[0][INT_OLDAREA_INDEX] = (state_t *)INT_OLDAREA;
		cleanState((state_t *)INT_NEWAREA);
		((state_t *)INT_NEWAREA)->pc_epc = ((state_t *)INT_NEWAREA)->reg_t9 = (memaddr)int_handler;
		((state_t *)INT_NEWAREA)->reg_sp = RAMTOP;
		((state_t *)INT_NEWAREA)->status = EXCEPTION_STATUS;
		
		pareas[0][TLB_NEWAREA_INDEX] = (state_t *)TLB_NEWAREA;
		pareas[0][TLB_OLDAREA_INDEX] = (state_t *)TLB_OLDAREA;
		cleanState((state_t *)TLB_NEWAREA);
		((state_t *)TLB_NEWAREA)->pc_epc = ((state_t *)TLB_NEWAREA)->reg_t9 = (memaddr)tlb_handler;
		((state_t *)TLB_NEWAREA)->reg_sp = RAMTOP;
		((state_t *)TLB_NEWAREA)->status = EXCEPTION_STATUS;
		
		pareas[0][PGMTRAP_NEWAREA_INDEX] = (state_t *)PGMTRAP_NEWAREA;
		pareas[0][PGMTRAP_OLDAREA_INDEX] = (state_t *)PGMTRAP_OLDAREA;
		cleanState((state_t *)PGMTRAP_NEWAREA);
		((state_t *)PGMTRAP_NEWAREA)->pc_epc = ((state_t *)PGMTRAP_NEWAREA)->reg_t9 = (memaddr)pgmtrap_handler;
		((state_t *)PGMTRAP_NEWAREA)->reg_sp = RAMTOP;
		((state_t *)PGMTRAP_NEWAREA)->status = EXCEPTION_STATUS;
		
		pareas[0][SYSBK_NEWAREA_INDEX] = (state_t *)SYSBK_NEWAREA;
		pareas[0][SYSBK_OLDAREA_INDEX] = (state_t *)SYSBK_OLDAREA;
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
			currentArea->pc_epc = currentArea->reg_t9 = (memaddr)pgmtrap_handler;
			currentArea->reg_sp = RAMTOP-(cpuid*FRAME_SIZE);
			currentArea->status = EXCEPTION_STATUS;	
			
			currentArea = &(areas[cpuid][SYSBK_NEWAREA_INDEX]);
			currentArea->pc_epc = currentArea->reg_t9 = (memaddr)sysbk_handler;
			currentArea->reg_sp = RAMTOP-(cpuid*FRAME_SIZE);
			currentArea->status = EXCEPTION_STATUS;	
		}
	}
	
	/** Inizializziamo i puntatori alle aree di tutte le CPU */
	int i = 0;
	int j = 0;
	
	for (i=1; i<NUM_CPU; i++){
		for (j=0; j<NUM_AREAS; j++){
			pareas[i][j] = &(areas[i][j]);
		}
	}
	
	/** AZZERAMENTO DEI LOCK */
	for(i=0;i<(MAXPROC+MAX_DEVICES);i++){
		locks[i] = 1;
	}
	
	/** INIZIALIZZAZIONE DELLO SCHEDULER */
	
	mkEmptyProcQ(&(readyQueue)); /* Inizializzo la ready queue */
	for(i=0;i<NUM_CPU;i++){
		STST(&(scheduler_states[i]));
		scheduler_states[i].reg_sp = SFRAMES_START-(i*FRAME_SIZE);
		scheduler_states[i].pc_epc = scheduler_states[i].reg_t9 = (memaddr)scheduler;
		/* Il TIMER e' disabilitato durante l'esecuzione dello scheduler */
		scheduler_states[i].status = PROCESS_STATUS & ~STATUS_TE;
	}

		/////////////////////////////
	   // PROCESSO DI PROVA 	  //
		pcb_t prova1;
		STST(&(prova1.p_s));
		prova1.p_s.pc_epc = prova1.p_s.reg_t9 = (memaddr)prova_altracpu;
		prova1.p_s.reg_sp = PFRAMES_START;
		prova1.p_s.status = prova1.p_s.status | PROCESS_STATUS;
		
		/////////////////////////////
	   // PROCESSO DI prova2 	  //
		pcb_t prova2;
		STST(&(prova2.p_s));
		prova2.p_s.pc_epc = prova2.p_s.reg_t9 = (memaddr)prova_altracpu;
		prova2.p_s.reg_sp = PFRAMES_START-4*QPAGE;
		prova2.p_s.status = prova2.p_s.status | PROCESS_STATUS;
		
		/////////////////////////////
	   // PROCESSO DI prova3 	  //
		pcb_t prova3;
		STST(&(prova3.p_s));
		prova3.p_s.pc_epc = prova3.p_s.reg_t9 = (memaddr)prova_altracpu;
		prova3.p_s.reg_sp = PFRAMES_START-6*QPAGE;
		prova3.p_s.status = prova3.p_s.status | PROCESS_STATUS;
		
		/////////////////////////////
	   // PROCESSO DI prova4 	  //
		pcb_t prova4;
		STST(&(prova4.p_s));
		prova4.p_s.pc_epc = prova4.p_s.reg_t9 = (memaddr)prova_altracpu;
		prova4.p_s.reg_sp = PFRAMES_START-8*QPAGE;
		prova4.p_s.status = prova4.p_s.status | PROCESS_STATUS;
	
	addReady(&prova1);
	addReady(&prova2);
	addReady(&prova3);
	addReady(&prova4);
	
	for(i=1;i<NUM_CPU;i++){
		INITCPU(i, &scheduler_states[i], *(pareas[i]));
	}
	/* bisogna attendere che tutte le altre CPU siano inizializzate
	 * prima di poter dare il controllo allo scheduler anche per la 
	 * CPU 0 */
	LDST(&(scheduler_states[0]));
	return 0;
}
