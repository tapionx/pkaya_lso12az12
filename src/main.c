#include "libumps.h"
#include "myConst.h"
#include "const.h"
#include "uMPStypes.h"
#include "types11.h"
#include "handlers.h"
#include "scheduler.h"
#include "pcb.e"

/* Variabili del nucleo */
int processCount; /* Contatore della totalita' dei processi */ 
int softBlockCount; /* Contatore dei processi bloccati su semafori */
struct list_head readyQueue; /* Coda dei processi in stato ready */
pcb_t *currentProcess; /* Puntatore al processo correntemente in esecuzione */
state_t areas[NUM_CPU][NUM_AREAS]; /* Aree reali per CPU > 0 */
state_t *pareas[NUM_CPU][NUM_AREAS]; /* Puntatori alle aree di tutte le CPU */
int locks[MAXPROC+MAX_DEVICES]; /* Variabili di condizione per CAS */
state_t scheduler_states[NUM_CPU]; /* state_t dello scheduler */


void prova_altracpu(){
	while(TRUE){
		SYSCALL(PASSEREN,0,0,0);
		debug(getPRID(),getPRID());
		//SYSCALL(VERHOGEN,0,0,0);
	}
}


/** L'esecuzione del kernel inizia da qui */
int main(){
	int cpuid, area, lockno; /* Iteratori */

	/** INIZIALIZZAZIONE DEI LOCK */
	for(lockno=0;lockno<(MAXPROC+MAX_DEVICES);lockno++){
		locks[lockno] = 1; /* PASS */
	}
	
	/** INIZIALIZZAZIONE DELLE NEW AREA */
	for (cpuid = 0; cpuid < NUM_CPU; cpuid++){
		for (area = INT_NEWAREA_INDEX; area < NUM_AREAS; area += 2){
			/* L'inizializzazione dipende dall'area presa in esame */
			switch(area){
				case INT_NEWAREA_INDEX:
					if (cpuid == 0){
						pareas[cpuid][area] = (state_t *)INT_NEWAREA;
						pareas[cpuid][area-1] = (state_t *)INT_OLDAREA;
					} else {
						pareas[cpuid][area] = &(areas[cpuid][area]);
						pareas[cpuid][area-1] = &(areas[cpuid][area-1]);
					}
					pareas[cpuid][area]->pc_epc = pareas[cpuid][area]->reg_t9 = (memaddr)int_handler;
					break;
				case TLB_NEWAREA_INDEX:
					if (cpuid == 0){
						pareas[cpuid][area] = (state_t *)TLB_NEWAREA;
						pareas[cpuid][area-1] = (state_t *)TLB_OLDAREA;
					} else {
						pareas[cpuid][area] = &(areas[cpuid][area]);
						pareas[cpuid][area-1] = &(areas[cpuid][area-1]);
					}
					pareas[cpuid][area]->pc_epc = pareas[cpuid][area]->reg_t9 = (memaddr)tlb_handler;
					break;
				case PGMTRAP_NEWAREA_INDEX:
					if (cpuid == 0){
						pareas[cpuid][area] = (state_t *)PGMTRAP_NEWAREA;
						pareas[cpuid][area-1] = (state_t *)PGMTRAP_OLDAREA;
					} else {
						pareas[cpuid][area] = &(areas[cpuid][area]);
						pareas[cpuid][area-1] = &(areas[cpuid][area-1]);
					}
					pareas[cpuid][area]->pc_epc = pareas[cpuid][area]->reg_t9 = (memaddr)pgmtrap_handler;
					break;
				case SYSBK_NEWAREA_INDEX:
					if (cpuid == 0){
						pareas[cpuid][area] = (state_t *)SYSBK_NEWAREA;
						pareas[cpuid][area-1] = (state_t *)SYSBK_OLDAREA;
					} else {
						pareas[cpuid][area] = &(areas[cpuid][area]);
						pareas[cpuid][area-1] = &(areas[cpuid][area-1]);
					}
					pareas[cpuid][area]->pc_epc = pareas[cpuid][area]->reg_t9 = (memaddr)sysbk_handler;
					break;
			}
			/* Settaggi comuni */
			pareas[cpuid][area]->reg_sp = RAMTOP-(cpuid*FRAME_SIZE);
			pareas[cpuid][area]->status = EXCEPTION_STATUS;
		}
	}

	/** INIZIALIZZAZIONE STRUTTURE PHASE1 */
	initPcbs();
	initASL();		
	
	/** INIZIALIZZAZIONE DELLO SCHEDULER */
	mkEmptyProcQ(&(readyQueue)); /* Inizializzo la ready queue */
	for(cpuid=0;cpuid<NUM_CPU;cpuid++){
		STST(&(scheduler_states[cpuid]));
		scheduler_states[cpuid].reg_sp = SFRAMES_START-(cpuid*FRAME_SIZE);
		scheduler_states[cpuid].pc_epc = scheduler_states[cpuid].reg_t9 = (memaddr)scheduler;
		/* Il TIMER e' disabilitato durante l'esecuzione dello scheduler */
		scheduler_states[cpuid].status = PROCESS_STATUS & ~STATUS_TE;
	}

		/////////////////////////////
	   // PROCESSO DI PROVA 	  //
		pcb_t* prova1 = allocPcb();
		//STST(&(prova1.p_s));
		prova1->p_s.pc_epc = prova1->p_s.reg_t9 = (memaddr)prova_altracpu;
		prova1->p_s.reg_sp = PFRAMES_START;
		prova1->p_s.status = prova1->p_s.status | PROCESS_STATUS;
		
		/////////////////////////////
	   // PROCESSO DI prova2 	  //
		pcb_t* prova2 = allocPcb();
		//STST(&(prova2.p_s));
		prova2->p_s.pc_epc = prova2->p_s.reg_t9 = (memaddr)prova_altracpu;
		prova2->p_s.reg_sp = PFRAMES_START-4*QPAGE;
		prova2->p_s.status = prova2->p_s.status | PROCESS_STATUS;
		
		/////////////////////////////
	   // PROCESSO DI prova3 	  //
		pcb_t* prova3 = allocPcb();
		//STST(&(prova3.p_s));
		prova3->p_s.pc_epc = prova3->p_s.reg_t9 = (memaddr)prova_altracpu;
		prova3->p_s.reg_sp = PFRAMES_START-6*QPAGE;
		prova3->p_s.status = prova3->p_s.status | PROCESS_STATUS;
		
		/////////////////////////////
	   // PROCESSO DI prova4 	  //
		pcb_t* prova4 = allocPcb();
		//STST(&(prova4.p_s));
		prova4->p_s.pc_epc = prova4->p_s.reg_t9 = (memaddr)prova_altracpu;
		prova4->p_s.reg_sp = PFRAMES_START-8*QPAGE;
		prova4->p_s.status = prova4->p_s.status | PROCESS_STATUS;
	
	addReady(prova1);
	addReady(prova2);
	addReady(prova3);
	addReady(prova4);
	
	for(cpuid=1;cpuid<NUM_CPU;cpuid++){
		INITCPU(cpuid, &scheduler_states[cpuid], *(pareas[cpuid]));
	}
	/* bisogna attendere che tutte le altre CPU siano inizializzate
	 * prima di poter dare il controllo allo scheduler anche per la 
	 * CPU 0 */
	LDST(&(scheduler_states[0]));
	return 0;
}
