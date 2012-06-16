#include "myconst.h"
#include "listx.h"
#include "types11.h"
#include "pcb.e"
#include "asl.e"
#include "libumps.h"
#include "const.h"
#include "scheduler.h"
#include "utils.h"

/* Variabili dello scheduler (condivise da ogni CPU) */
int globalProcs; /* contatore globale dei processi */
int procs[NUM_CPU]; /* contatore dei processi (sia ready che running) */
int softProcs[NUM_CPU]; /* contatore dei processi bloccati su I/O */
pcb_t *currentProc[NUM_CPU]; /* puntatore al processo in esecuzione attuale */

/* Ready Queues */
struct list_head readyQueue[NUM_CPU][MAX_PCB_PRIORITY]; /* coda dei processi in stato ready */
int readyProcs[NUM_CPU][MAX_PCB_PRIORITY]; /* tabella dei processi ready */
int initdQueues = FALSE;

/* Variabili del kernel */
extern state_t pstate[NUM_CPU]; /* stati di load/store per le varie cpu */

/* Getter e Setter */


/****************** IMPLEMENTAZIONE ********************************/

/* Questa funzione inserisce in una readyQueue della CPU[id] il pcb_t 
passato a seconda della priorità */
void addReady(pcb_t *proc){
	/* CONTROLLO PRIMARIO: inizializza le code se non lo sono già */
	if (!initdQueues) {
		initReadyQueues();
		initdQueues = TRUE;
	}
	/* Trova la CPU con minor workload */
	int id = minWorkloadCpu(procs, NUM_CPU);	
	/* Incremento sia il contatore dei processi locali che globali */
	procs[id]++;
	globalProcs++;
	/* Aggiorno la tabella delle readyQueue */
	readyProcs[id][proc->priority];	 	 
	/* Inserisco il pcb_t passato nella readyQueue della CPU id */
	insertProcQ(&(readyQueue[id][proc->priority]), proc);
}

/* Questa funzione si occupa di estrare un processo dalla readyQueue
 * della CPU[id] e di caricarne lo stato su quest'ultima */
void loadReady(){
	/* Trovo la coda con priorità maggiore che ha processi da eseguire */
	int nqueue, priority;
	int id = getPRID();
	for (nqueue=MAX_PCB_PRIORITY; nqueue >= MIN_PCB_PRIORITY; nqueue--){
		/* Inizio a scandire da quelle con priorità più alta */
		if (readyProcs[id][nqueue] > 0){
			/* Salvo lo stato della CPU e carico il pcb */
			STST(&(pstate[id]));
			pstate[id].pc_epc = pstate[id].reg_t9 = (memaddr)scheduler;
			pcb_t *torun = removeProcQ(&(readyQueue[id][nqueue]));
			(readyProcs[id][nqueue])--;
			currentProc[id] = torun;
			LDST(&(torun->p_s));	
			/* NOTA: ogni volta che un processo viene interrotto per la
			 * fine del suo timeslice è necessario, nell'handler
			 * fare in modo che la sua priorità venga incrementata per 
			 * le politiche di aging (anti starvation) per poi 
			 * richiamare addReady su quel processo */
		}
	}
}

/*************************** SCHEDULER ********************************/

extern int key;

/* AVVIO DELLO SCHEDULER - Passaggio del controllo */
void scheduler(){
	/* DEBUG SCHEDULER */ while(!CAS(&key, PASS, FORBID));
						printn("Scheduler avviato sulla CPU %!\n", getPRID());
						globalProcs--;
						CAS(&key, FORBID, PASS);
						if (globalProcs == 0) HALT();
						WAIT();
	loadReady();
	
}


