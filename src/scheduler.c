#include "types11.h"
#include "myConst.h"
#include "libumps.h"
#include "pcb.e"
#include "asl.e"
#include "kernelVariables.h"
#include "handlers.h"

void dummyFunction(){
	setSTATUS(PROCESS_STATUS);
}

state_t dummyProcess;

/**
 * Questa funzione si occupa di aggiungere un pcb_t (puntato da proc)
 * alla readyQueue dello scheduler. Agisce già in mutua esclusione
 * utilizzando la CAS e un intero globale per lo scheduler.
 */
void addReady(pcb_t *proc){
	lock(SCHEDULER_SEMLOCK);
	if (proc != NULL)
		insertProcQ(&(readyQueue), proc);
	free(SCHEDULER_SEMLOCK);
}

/**
 * SCHEDULER: Entry point
 */
void scheduler(){
	/* TODO: ricordarsi di gestire i casi di deadlock (procCount e softProcCount) 
	 * e di modificare tutte le variabili del caso nelle syscall ecc. */
	int cpuid = getPRID();
	lock(SCHEDULER_SEMLOCK);
	/* Gli interrupt potrebbero arrivare anche mentre non ci sono processi
	 * nella readyQueue e lo scheduler va in WAIT! Allora currentProcess
	 * ancora punterebbe al processo che era stato caricato in precedenza */
	currentProcess[cpuid] = NULL;
	if(!emptyProcQ(&(readyQueue))){
		currentProcess[cpuid] = removeProcQ(&(readyQueue));
		/* Se il processo è da terminare ne liberiamo il pcb e lo scartiamo */
		if (currentProcess[cpuid]->wanted){
			freePcb(currentProcess[cpuid]);
			free(SCHEDULER_SEMLOCK);
			LDST(&(scheduler_states[cpuid]));
		}
		/* Setto lo pseudo-clock-timer (SOLO LA PRIMA VOLTA, si
		 * resetta ad ogni interrupt/V). Non possiamo attivarlo nel 
		 * main per motivi di coerenza: se due o più processi volessero
		 * usare il PCT per sincronizzarsi, e questo fosse inizializzato
		 * durante il main, il primo processo caricato partirebbe istantaneamente
		 * mentre gli altri rimarrebbero in attesa che il PCT scada di nuovo */
		if (!pctInit){
			SET_IT(SCHED_PSEUDO_CLOCK);
			pctInit = TRUE;
		}
		free(SCHEDULER_SEMLOCK);
		/* Settiamo il TIME_SLICE un istante prima di mandare il processo
		 * in esecuzione */
		setTIMER(TIME_SLICE);
		LDST(&(currentProcess[cpuid]->p_s));
	}
	free(SCHEDULER_SEMLOCK);
	WAIT();
	devStatus = 0;
	/* BUG: La prima volta dopo la WAIT non va nell'int_handler ma ritorna
	 * nel main. Per evitare questo comportamento si utilizza un dummyProcess
	 * il cui unico compito è quello di catturare gli interrupt la prima
	 * volta. */
	dummyProcess.status = PROCESS_STATUS;
	dummyProcess.reg_sp = SFRAMES_START-getPRID()*FRAME_SIZE;
	dummyProcess.pc_epc = dummyProcess.reg_t9 = (memaddr)dummyFunction;
	LDST(&(dummyProcess));
}
