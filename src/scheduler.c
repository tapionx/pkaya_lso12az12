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

/* Metodo per restituire il processo corrente della cpu N */
pcb_t *getCurrentProc(U32 cpuid)
{
	return currentProc[cpuid];
}

/* Metodo per decrementare il numero di processi della CPU n */
void decreaseProcsCounter(U32 cpuid)
{
	procs[cpuid] -= 1;
}

/* Metodo per decrementare il soft block count della CPU n */
void decreaseSoftProcsCounter(U32 cpuid)
{
	softProcs[cpuid] -= 1;
}

/* Metodo per incrementare il soft block count della CPU n */
void increaseSoftProcsCounter(U32 cpuid)
{
	softProcs[cpuid] += 1;
}


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
			/* Aggiorno il TPR della CPU */
			memaddr *TPR = (memaddr *)TPR_ADDR;
			*TPR = torun->priority;
			/* Lancio il nuovo processo */
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
	WAIT();
	int id = getPRID();
	/* Innanzitutto se lo scheduler è stato richiamato dalla fine di un
	 * TIME_SLICE è bene che il processo sia reinserito nelle readyQueue
	 * in base alla priorità e tenendo conto dell'aging */
	if (currentProc[id] != NULL){
		(currentProc[id]->priority)++;
		addReady(currentProc[id]);
	}
	
	/* Inizializzo il timer a TIME_SLICE */
	setTIMER(TIME_SLICE);
	/* Lancio l'esecuzione del prossimo processo */
	loadReady();
}


