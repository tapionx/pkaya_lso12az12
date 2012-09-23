#include "libumps.h"
#include "myConst.h"
#include "const.h"
#include "uMPStypes.h"
#include "types11.h"
#include "handlers.h"
#include "scheduler.h"
#include "utils.h"
#include "kernelVariables.h"
#include "syscall.h"
#include "interrupt.h"
#include "asl.e"

void pltHandler(U32 cpuid){ /* TESTATO */
	state_t *oldProcess = (getPRID() == 0)? (state_t *)INT_OLDAREA : &areas[getPRID()][INT_OLDAREA_INDEX];
	/* Non c'è bisogno di mutua esclusione esplicita dato che la addReady già la include! */
	copyState(oldProcess, &(currentProcess[cpuid]->p_s));
	addReady(currentProcess[cpuid]);
	/* ACK del PLT */
	setTIMER(TIME_SLICE);
	/* Richiamo lo scheduler */
	LDST(&(scheduler_states[cpuid]));
}

void pctHandler(U32 cpuid){
	state_t *oldProcess = (getPRID() == 0)? (state_t *)INT_OLDAREA : &areas[getPRID()][INT_OLDAREA_INDEX];
	copyState(oldProcess, &(currentProcess[cpuid]->p_s));
	/* Facciamo la V "speciale" che risveglia tutti i processi bloccati */
	/* estraggo il puntatore al semaforo */
	semd_t *pctsem = getSemd(PCT_SEM);
	/* faccio la V finchè non escono tutti i processi */
	while(pctsem->s_value != 0){
		verhogen(PCT_SEM);
	}
	/* setto di nuovo il PCT a 100ms */
	SET_IT(SCHED_PSEUDO_CLOCK);
	/* Rimetto il processo interrotto in ready */
	addReady(currentProcess[cpuid]);
	/* Richiamo lo scheduler */
	LDST(&(scheduler_states[cpuid]));
}

void terminalHandler(U32 cpuid){
	debug(44,44);
	int line = INT_TERMINAL;
	/* Calcolo il numero del device che ha generato l'interrupt */
	int devNo = getDevNo(GET_PENDING_INT_BITMAP(line));
	/* Calcoliamo l'inizio del registro del controller per scrivere/leggere sui suoi registri */
	int devAddrBase = DEV_ADDR_BASE(line, devNo);
	/* indice del semaforo da usare */
	int termSemNo = GET_TERM_SEM(line, devNo, FALSE);
	/* TODO: Bisogna stare attenti perché quando si chiama 	la 
	 * funzione associata alla syscall (non SYSCALL!) non viene
	 * aggiornato il currentProcess! Quindi quando lo si rimette in
	 * readyQueue da li semplicemente va in loop se aveva fatto una P
	 * (senza V, magari nell'esecuzione principale). Bisogna
	 * aggiornare il currentProcess in ogni syscall dove si usa
	 * currentProcess! */
	/* puntatore ai registri del device */	
	termreg_t *fields = (termreg_t *)devAddrBase;
	/* indice del vettore delle risposte associato al device */
	int termStatusNo = GET_TERM_STATUS(line, devNo, FALSE);
	semd_t *termSem = getSemd(termSemNo);
	/* puntatore al primo processo bloccato sulla coda */
	pcb_t *waitingProc = headBlocked(termSemNo);
	/* se non ci sono processi in coda */
	if (waitingProc != NULL){
		debug(326, termSemNo);
		debug(327, (U32)waitingProc);
		/* Maggior priorità alla trasmissione */
		/* scrivo la risposta nel processo in coda */
		waitingProc->p_s.reg_v0 = fields->transm_status;
	} else {
		/* scrivo lo status nel vettore, il processo non
		 * ha ancora fatto la P */
		devStatus = fields->transm_status;
	}
	verhogen(termSemNo);
	/* acknowledgement al device */
	fields->transm_command = DEV_C_ACK;
	/* Faccio ripartire lo scheduler */
	scheduler();
}
