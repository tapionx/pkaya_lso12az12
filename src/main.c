#include "libumps.h"
#include "myConst.h"
#include "const.h"
#include "uMPStypes.h"
#include "types11.h"
#include "handlers.h"
#include "scheduler.h"
#include "pcb.e"
#include "myProcs.c"
#include "asl.e"

/* Variabili del nucleo */
int processCount; /* Contatore della totalita' dei processi */ 
int softBlockCount; /* Contatore dei processi bloccati su semafori */
struct list_head readyQueue; /* Coda dei processi in stato ready */
pcb_t *currentProcess[NUM_CPU]; /* Puntatore al processo correntemente in esecuzione */
state_t areas[NUM_CPU][NUM_AREAS]; /* Aree reali per CPU > 0 */
int locks[NUM_SEMAPHORES]; /* Variabili di condizione per CAS */
state_t scheduler_states[NUM_CPU]; /* state_t dello scheduler */
int pctInit = FALSE; /* Lo Pseudo Clock Timer è stato inizializzato? */
U32 devStatus[MAX_DEVICES] = {0}; /* Status in output dei vari device */

/** L'esecuzione del kernel inizia da qui */
int main(){
	
		
	int cpuid, area, lockno; /* Iteratori */

	/** INIZIALIZZAZIONE DEI LOCK */
	for(lockno=0;lockno<(NUM_SEMAPHORES);lockno++){
		locks[lockno] = PASS;
	}
	
	/** INIT CPU0 */
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
	
	/** INIZIALIZZAZIONE STRUTTURE PHASE1 */
	initPcbs();
	initASL();
	
	/** INIT CPU > 0 */
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
	
	/** INIZIALIZZAZIONE DELLO SCHEDULER */
	mkEmptyProcQ(&(readyQueue)); /* Inizializzo la ready queue */
	for(cpuid=0;cpuid<NUM_CPU;cpuid++){
		STST(&(scheduler_states[cpuid]));
		scheduler_states[cpuid].reg_sp = SFRAMES_START-(cpuid*FRAME_SIZE);
		scheduler_states[cpuid].pc_epc = scheduler_states[cpuid].reg_t9 = (memaddr)scheduler;
		/* Il TIMER e' disabilitato durante l'esecuzione dello scheduler */
		scheduler_states[cpuid].status = PROCESS_STATUS & ~(STATUS_TE);
	}

		

		/////////////////////////////
	   // PROCESSO DI PROVA 	  //
		pcb_t* prova1 = allocPcb();
		//STST(&(prova1.p_s));
		prova1->p_s.pc_epc = prova1->p_s.reg_t9 = (memaddr)test1;
		prova1->p_s.reg_sp = PFRAMES_START;
		prova1->p_s.status = prova1->p_s.status | PROCESS_STATUS;
		
		///////////////////////////////
	   //// PROCESSO DI prova2 	  //
		pcb_t* prova2 = allocPcb();
		//STST(&(prova2.p_s));
		prova2->p_s.pc_epc = prova2->p_s.reg_t9 = (memaddr)test2;
		prova2->p_s.reg_sp = PFRAMES_START-4*QPAGE;
		prova2->p_s.status = prova2->p_s.status | PROCESS_STATUS;
		
		///////////////////////////////
	   //// PROCESSO DI prova3 	  //
		pcb_t* prova3 = allocPcb();
		//STST(&(prova3.p_s));
		prova3->p_s.pc_epc = prova3->p_s.reg_t9 = (memaddr)test3;
		prova3->p_s.reg_sp = PFRAMES_START-6*QPAGE;
		prova3->p_s.status = prova3->p_s.status | PROCESS_STATUS;
		
		///////////////////////////////
	   //// PROCESSO DI prova4 	  //
		pcb_t* prova4 = allocPcb();
		//STST(&(prova4.p_s));
		prova4->p_s.pc_epc = prova4->p_s.reg_t9 = (memaddr)test4;
		prova4->p_s.reg_sp = PFRAMES_START-8*QPAGE;
		prova4->p_s.status = prova4->p_s.status | PROCESS_STATUS;
	
	addReady(prova1);
	//addReady(prova2);
	//addReady(prova3);
	//addReady(prova4);
	
	for(cpuid=1;cpuid<NUM_CPU;cpuid++){
		INITCPU(cpuid, &scheduler_states[cpuid], &areas[cpuid]);
	}
	/* bisogna assicurarsi che tutte le altre CPU abbiano iniziato 
	 * l'inizializzazione prima di poter dare il controllo allo scheduler 
	 * anche per la CPU 0 */
	LDST(&(scheduler_states[0]));
	return 0;
}
