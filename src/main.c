#include "libumps.h"
#include "myConst.h"
#include "const.h"
#include "uMPStypes.h"
#include "types11.h"
#include "handlers.h"
#include "scheduler.h"
#include "pcb.e"
#include "asl.e"
#include "myProcs.h"

/* Variabili del nucleo */
int processCount; /* Contatore della totalita' dei processi */
int softBlockCount; /* Contatore dei processi bloccati su semafori */
struct list_head readyQueue; /* Coda dei processi in stato ready */
pcb_t *currentProcess[MAX_CPU]; /* Puntatore al processo correntemente in esecuzione */
state_t areas[MAX_CPU][NUM_AREAS]; /* Aree reali per CPU > 0 */
int locks[NUM_SEMAPHORES]; /* Variabili di condizione per CAS */
state_t scheduler_states[MAX_CPU]; /* state_t dello scheduler */
int pctInit; /* Lo Pseudo Clock Timer è stato inizializzato? */
U32 devStatus; /* Status in output dei vari device (solo uno perché solo il terminale è usato) */

/* Processo di test */
extern void p2test();

/** L'esecuzione del kernel inizia da qui */
int main(){	
	
	/* Interval Timer inizialmente non settato */
	pctInit = FALSE;
	
	int cpuid, area, lockno; /* Iteratori */

	/***************************** INIZIALIZZAZIONE DEI LOCK */
	for(lockno=0;lockno<(NUM_SEMAPHORES);lockno++){
		locks[lockno] = PASS;
	}
	
	
	/***************************** INIT CPU0 */
	/* Init delle new area */
	((state_t *)INT_NEWAREA)->pc_epc = ((state_t *)INT_NEWAREA)->reg_t9 = (memaddr)int_handler;
	((state_t *)INT_NEWAREA)->reg_sp = RAMTOP;
	((state_t *)INT_NEWAREA)->status = EXCEPTION_STATUS;
	
	((state_t *)TLB_NEWAREA)->pc_epc = ((state_t *)TLB_NEWAREA)->reg_t9 = (memaddr)tlb_handler;
	((state_t *)TLB_NEWAREA)->reg_sp = RAMTOP;
	((state_t *)TLB_NEWAREA)->status = EXCEPTION_STATUS;
	
	((state_t *)PGMTRAP_NEWAREA)->pc_epc = ((state_t *)PGMTRAP_NEWAREA)->reg_t9 = (memaddr)pgmtrap_handler;
	((state_t *)PGMTRAP_NEWAREA)->reg_sp = RAMTOP;
	((state_t *)PGMTRAP_NEWAREA)->status = EXCEPTION_STATUS;

	((state_t *)SYSBK_NEWAREA)->pc_epc = ((state_t *)SYSBK_NEWAREA)->reg_t9 = (memaddr)sysbk_handler;
	((state_t *)SYSBK_NEWAREA)->reg_sp = RAMTOP;
	((state_t *)SYSBK_NEWAREA)->status = EXCEPTION_STATUS;
	
		
	/***************************** INIT CPU > 0 */
	if (NUM_CPU > 1){
		/* Init delle new area */
		
		for (cpuid=1; cpuid<NUM_CPU; cpuid++){
			
			areas[cpuid][INT_NEWAREA_INDEX].pc_epc = areas[cpuid][INT_NEWAREA_INDEX].reg_t9 = (memaddr)int_handler;
			areas[cpuid][INT_NEWAREA_INDEX].reg_sp = RAMTOP-(cpuid*FRAME_SIZE);
			areas[cpuid][INT_NEWAREA_INDEX].status = EXCEPTION_STATUS;	
			
			areas[cpuid][TLB_NEWAREA_INDEX].pc_epc = areas[cpuid][TLB_NEWAREA_INDEX].reg_t9 = (memaddr)tlb_handler;
			areas[cpuid][TLB_NEWAREA_INDEX].reg_sp = RAMTOP-(cpuid*FRAME_SIZE);
			areas[cpuid][TLB_NEWAREA_INDEX].status = EXCEPTION_STATUS;	
			
			areas[cpuid][PGMTRAP_NEWAREA_INDEX].pc_epc = areas[cpuid][PGMTRAP_NEWAREA_INDEX].reg_t9 = (memaddr)pgmtrap_handler;
			areas[cpuid][PGMTRAP_NEWAREA_INDEX].reg_sp = RAMTOP-(cpuid*FRAME_SIZE);
			areas[cpuid][PGMTRAP_NEWAREA_INDEX].status = EXCEPTION_STATUS;	
			
			areas[cpuid][SYSBK_NEWAREA_INDEX].pc_epc = areas[cpuid][SYSBK_NEWAREA_INDEX].reg_t9 = (memaddr)sysbk_handler;
			areas[cpuid][SYSBK_NEWAREA_INDEX].reg_sp = RAMTOP-(cpuid*FRAME_SIZE);
			areas[cpuid][SYSBK_NEWAREA_INDEX].status = EXCEPTION_STATUS;
		}
	}
	
	
	/***************************** INIZIALIZZAZIONE STRUTTURE PHASE1 */
	initPcbs();
	initASL();
	
	
	/*****************************  INIZIALIZZAZIONE DEGLI SCHEDULER */
	/* Inizializzo la ready queue */
	mkEmptyProcQ(&(readyQueue));
	for(cpuid=0;cpuid<NUM_CPU;cpuid++){
		STST(&(scheduler_states[cpuid]));
		scheduler_states[cpuid].reg_sp = SFRAMES_START-(cpuid*FRAME_SIZE);
		scheduler_states[cpuid].pc_epc = scheduler_states[cpuid].reg_t9 = (memaddr)scheduler;
		/* Il TIMER e' disabilitato durante l'esecuzione dello scheduler */
		scheduler_states[cpuid].status = SCHEDULER_STATUS;
	}
	
	
	/***************************** AGGIUNTA DEI PROCESSI */
	///* P2 test */
	//pcb_t* p2 = allocPcb();
	//STST(&(p2->p_s));
	//p2->p_s.pc_epc = p2->p_s.reg_t9 = (memaddr)p2test;
	//p2->p_s.reg_sp = PFRAMES_START;
	//p2->p_s.status |= |STATUS_INT_UNMASKED;
	//addReady(p2);
	

	/* Prova 1 */
	pcb_t* prova1 = allocPcb();
	STST(&(prova1->p_s));
	prova1->p_s.pc_epc = prova1->p_s.reg_t9 = (memaddr)test1;
	prova1->p_s.reg_sp = PFRAMES_START;
	prova1->p_s.status = PROCESS_STATUS;
	//~ debug(0, prova1);
	addReady(prova1);
	
	
	/* Prova 2 */
	pcb_t* prova2 = allocPcb();
	STST(&(prova2->p_s));
	prova2->p_s.pc_epc = prova2->p_s.reg_t9 = (memaddr)test2;
	prova2->p_s.reg_sp = PFRAMES_START-FRAME_SIZE;
	prova2->p_s.status = PROCESS_STATUS;
	//~ debug(1, prova2);
	addReady(prova2);
	
	
	/* Prova 3 */
	pcb_t* prova3 = allocPcb();
	STST(&(prova3->p_s));
	prova3->p_s.pc_epc = prova3->p_s.reg_t9 = (memaddr)test3;
	prova3->p_s.reg_sp = PFRAMES_START-2*FRAME_SIZE;
	prova3->p_s.status = PROCESS_STATUS;
	//~ debug(2, prova3);
	addReady(prova3);
	
	
	/* Prova 4 */
	pcb_t* prova4 = allocPcb();
	STST(&(prova4->p_s));
	prova4->p_s.pc_epc = prova4->p_s.reg_t9 = (memaddr)test4;
	prova4->p_s.reg_sp = PFRAMES_START-3*FRAME_SIZE;
	prova4->p_s.status = PROCESS_STATUS;
	//~ debug(3, prova4);
	addReady(prova4);
	
	
	
	/***************************** INIZIALIZZAZIONE DELLE CPU */
	for(cpuid=1;cpuid<NUM_CPU;cpuid++){
		INITCPU(cpuid, &scheduler_states[cpuid], &areas[cpuid]);
	}
	/* Bisogna assicurarsi che tutte le altre CPU abbiano iniziato 
	 * l'inizializzazione prima di poter dare il controllo allo scheduler 
	 * anche per la CPU 0 */
	LDST(&(scheduler_states[0]));
	return 0;
}
