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
state_t *pareas[NUM_CPU][NUM_AREAS]; /* Puntatori alle aree di tutte le CPU */
int locks[MAXPROC+MAX_DEVICES]; /* Variabili di condizione per CAS */
state_t scheduler_states[NUM_CPU]; /* state_t dello scheduler */

/** L'esecuzione del kernel inizia da qui */
int main(){
	
	//initPcbs();
	//initASL();
	
	//semd_t *semd = getSemd(0);
	
	//printn("semaforo inizializzato %\n", semd);
	//printn("valore: %\n", semd->s_value);
	
	//pcb_t *p1 = allocPcb();
	//printn("p1 %\n", p1);
	//pcb_t *p2 = allocPcb();
	//printn("p2 %\n", p2);
	//pcb_t *p3 = allocPcb();
	//printn("p3 %\n", p3);
	//pcb_t *p4 = allocPcb();
	//printn("p4 %\n", p4);
	
	//printn("primo %\n", headBlocked(0));	
	
	//insertBlocked(0, p1);
	//printn("inserito %\n", p1); 
	//printn("valore %\n", semd->s_value);
	//printn("primo %\n", headBlocked(0));
	
		//stampaCoda(0);
	
	//insertBlocked(0, p2);
	//printn("inserito %\n", p2); 
	//printn("valore %\n", semd->s_value);
	//printn("primo %\n", headBlocked(0));
	
		//stampaCoda(0);
	
	//insertBlocked(0, p3);
	//printn("inserito %\n", p3); 
	//printn("valore %\n", semd->s_value);
	//printn("primo %\n", headBlocked(0));
	
		//stampaCoda(0);
	
	//pcb_t *rimosso1 = removeBlocked(0);
	//printn("rimosso %\n", rimosso1);
	//pcb_t *head1 = headBlocked(0);
	//printn("il primo e %\n", head1);
	
		//stampaCoda(0);
	
	//insertBlocked(0, p4);
	//printn("inserito %\n", p4); 
	//printn("valore %\n", semd->s_value);
	//printn("primo %\n", headBlocked(0));
	
		//stampaCoda(0);
		
	//insertBlocked(0, p1);
	//printn("inserito %\n", p1); 
	//printn("valore %\n", semd->s_value);
	//printn("primo %\n", headBlocked(0));
	
	//stampaCoda(0);
		
	//pcb_t *del1 = removeBlocked(0);
	//printn("rimosso  %\n", del1);
	//printn("ci aspettavamo %\n", p2);
	//printn(" valore %\n", semd->s_value);
	//printn("primo %\n", headBlocked(0));
	
		//stampaCoda(0);

	//pcb_t *del2 = removeBlocked(0);
	//printn("rimosso  %", del2);
	//printn(" valore %\n", semd->s_value);
	//printn("primo %\n", headBlocked(0));
	
	//pcb_t *del3 = removeBlocked(0);
	//printn("rimosso  %", del3);
	//printn(" valore %\n", semd->s_value);
	//printn("primo %\n", headBlocked(0));
		
	//pcb_t *del4 = removeBlocked(0);
	//printn("rimosso  %", del4);
	//printn(" valore %\n", semd->s_value);
	//printn("primo %\n", headBlocked(0));
	
	//insertBlocked(0, p1);
	//printn("inserito % ", p1); 
	//printn("valore %\n", semd->s_value);
	//printn("primo %\n", headBlocked(0));
		
	//del1 = removeBlocked(0);
	//printn("rimosso  %", del1);
	//printn(" valore %\n", semd->s_value);
	//printn("primo %\n", headBlocked(0));
		
	//insertBlocked(0, p1);
	//printn("inserito % ", p1); 
	//printn("valore %\n", semd->s_value);
	//printn("primo %\n", headBlocked(0));
		
	//insertBlocked(0, p2);
	//printn("inserito % ", p2); 
	//printn("valore %\n", semd->s_value);
	//printn("primo %\n", headBlocked(0));
		
	//del1 = removeBlocked(0);
	//printn("rimosso  %", del1);
	//printn(" valore %\n", semd->s_value);
	//printn("primo %\n", headBlocked(0));
		
	//insertBlocked(0, p3);
	//printn("inserito % ", p3); 
	//printn("valore %\n", semd->s_value);
	//printn("primo %\n", headBlocked(0));
		
	//del1 = removeBlocked(0);
	//printn("rimosso  %", del1);
	//printn(" valore %\n", semd->s_value);
	//printn("primo %\n", headBlocked(0));
		
	//addokbuf("FINE DELLE TRASMISSIONI\n");
	//PANIC();
	
	
	
	int cpuid, area, lockno; /* Iteratori */

	/** INIZIALIZZAZIONE DEI LOCK */
	for(lockno=0;lockno<(MAXPROC+MAX_DEVICES);lockno++){
		locks[lockno] = 1; /* PASS */
	}
	
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
	
	/** INIZIALIZZAZIONE STRUTTURE PHASE1 */
	initPcbs();
	initASL();
	
	/** INIT CPU > 0 */
	if (NUM_CPU > 1){
		/* Init delle new area */
		int cpuid,area;
		state_t *currentArea;
		
		for (cpuid=1; cpuid<NUM_CPU; cpuid++){
			
			pareas[cpuid][INT_NEWAREA_INDEX] = &(areas[cpuid][INT_NEWAREA_INDEX]);
			pareas[cpuid][INT_OLDAREA_INDEX] = &(areas[cpuid][INT_OLDAREA_INDEX]);
			pareas[cpuid][INT_NEWAREA_INDEX]->pc_epc = pareas[cpuid][INT_NEWAREA_INDEX]->reg_t9 = (memaddr)int_handler;
			pareas[cpuid][INT_NEWAREA_INDEX]->reg_sp = RAMTOP-(cpuid*FRAME_SIZE);
			pareas[cpuid][INT_NEWAREA_INDEX]->status = EXCEPTION_STATUS;	
			
			pareas[cpuid][TLB_NEWAREA_INDEX] = &(areas[cpuid][TLB_NEWAREA_INDEX]);
			pareas[cpuid][TLB_OLDAREA_INDEX] = &(areas[cpuid][TLB_OLDAREA_INDEX]);
			pareas[cpuid][TLB_NEWAREA_INDEX]->pc_epc = pareas[cpuid][TLB_NEWAREA_INDEX] ->reg_t9 = (memaddr)tlb_handler;
			pareas[cpuid][TLB_NEWAREA_INDEX]->reg_sp = RAMTOP-(cpuid*FRAME_SIZE);
			pareas[cpuid][TLB_NEWAREA_INDEX]->status = EXCEPTION_STATUS;	
			
			pareas[cpuid][PGMTRAP_NEWAREA_INDEX] = &(areas[cpuid][PGMTRAP_NEWAREA_INDEX]);
			pareas[cpuid][PGMTRAP_OLDAREA_INDEX] = &(areas[cpuid][PGMTRAP_OLDAREA_INDEX]);
			pareas[cpuid][PGMTRAP_NEWAREA_INDEX]->pc_epc = pareas[cpuid][PGMTRAP_NEWAREA_INDEX] ->reg_t9 = (memaddr)pgmtrap_handler;
			pareas[cpuid][PGMTRAP_NEWAREA_INDEX]->reg_sp = RAMTOP-(cpuid*FRAME_SIZE);
			pareas[cpuid][PGMTRAP_NEWAREA_INDEX]->status = EXCEPTION_STATUS;
			
			pareas[cpuid][SYSBK_NEWAREA_INDEX] = &(areas[cpuid][SYSBK_NEWAREA_INDEX]);
			pareas[cpuid][SYSBK_OLDAREA_INDEX] = &(areas[cpuid][SYSBK_OLDAREA_INDEX]);
			pareas[cpuid][SYSBK_NEWAREA_INDEX]->pc_epc = pareas[cpuid][SYSBK_NEWAREA_INDEX] ->reg_t9 = (memaddr)sysbk_handler;
			pareas[cpuid][SYSBK_NEWAREA_INDEX]->reg_sp = RAMTOP-(cpuid*FRAME_SIZE);
			pareas[cpuid][SYSBK_NEWAREA_INDEX]->status = EXCEPTION_STATUS;
		}
	}
	
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
		prova1->p_s.pc_epc = prova1->p_s.reg_t9 = (memaddr)test1;
		prova1->p_s.reg_sp = PFRAMES_START;
		prova1->p_s.status = prova1->p_s.status | PROCESS_STATUS;
		
		/////////////////////////////
	   // PROCESSO DI prova2 	  //
		pcb_t* prova2 = allocPcb();
		//STST(&(prova2.p_s));
		prova2->p_s.pc_epc = prova2->p_s.reg_t9 = (memaddr)test2;
		prova2->p_s.reg_sp = PFRAMES_START-4*QPAGE;
		prova2->p_s.status = prova2->p_s.status | PROCESS_STATUS;
		
		/////////////////////////////
	   // PROCESSO DI prova3 	  //
		pcb_t* prova3 = allocPcb();
		//STST(&(prova3.p_s));
		prova3->p_s.pc_epc = prova3->p_s.reg_t9 = (memaddr)test3;
		prova3->p_s.reg_sp = PFRAMES_START-6*QPAGE;
		prova3->p_s.status = prova3->p_s.status | PROCESS_STATUS;
		
		/////////////////////////////
	   // PROCESSO DI prova4 	  //
		pcb_t* prova4 = allocPcb();
		//STST(&(prova4.p_s));
		prova4->p_s.pc_epc = prova4->p_s.reg_t9 = (memaddr)test4;
		prova4->p_s.reg_sp = PFRAMES_START-8*QPAGE;
		prova4->p_s.status = prova4->p_s.status | PROCESS_STATUS;
	
	addReady(prova1);
	addReady(prova2);
	addReady(prova3);
	addReady(prova4);
	
	for(cpuid=1;cpuid<NUM_CPU;cpuid++){
		INITCPU(cpuid, &scheduler_states[cpuid], pareas[cpuid][0]);
	}
	/* bisogna attendere che tutte le altre CPU siano inizializzate
	 * prima di poter dare il controllo allo scheduler anche per la 
	 * CPU 0 */
	LDST(&(scheduler_states[0]));
	return 0;
}
