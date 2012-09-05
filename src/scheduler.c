#include "myconst.h"
#include "listx.h"
#include "types11.h"
#include "pcb.e"
#include "asl.e"
#include "libumps.h"
#include "const.h"
#include "scheduler.h"
#include "utils.h"
#include "handlers.h"

/* Variabili dello scheduler (condivise da ogni CPU) */
int runningProcsCounter; /* contatore dei processi running globale */
int readyProcsCounter; /* contatore dei processi ready globale */
/* NOTA: Per ottenere un totale dei processi nel sistema basta sommare i
 * due contatori */
int procs[NUM_CPU]; /* contatore dei processi per CPU (sia ready che running) */
int softProcs[NUM_CPU]; /* contatore dei processi bloccati su I/O per CPU */
pcb_t *currentProc[NUM_CPU]; /* puntatore al processo in esecuzione attuale per CPU */

/* Ready Queues */
struct list_head readyQueue[NUM_CPU][MAX_PCB_PRIORITY]; /* coda dei processi in stato ready */
int readyProcs[NUM_CPU][MAX_PCB_PRIORITY]; /* tabella dei contatori processi ready */
int initdQueues = FALSE;

/* Variabili del kernel */
extern state_t *pnew_old_areas[NUM_CPU][NUM_AREAS]; /* 8 areas for each cpu */
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
	/* Trova la CPU con minor workload */
	int id = minWorkloadCpu(procs, NUM_CPU);
	/* Incremento sia il contatore dei processi locali che globali */
	procs[id]++;
	readyProcsCounter++;
	/* Aggiorno la tabella delle readyQueue */
	readyProcs[id][proc->priority]++;	 	
	/* Inserisco il pcb_t passato nella readyQueue della CPU id */
	insertProcQ(&(readyQueue[id][proc->priority]), proc);
	pcb_t *boh = container_of(list_head(readyQueue[id][proc->priority]), pcb_t, p_next); 
	debug(boh, proc);
}


/* Questa funzione si occupa di estrare un processo dalla readyQueue
 * della CPU[id] e di caricarne lo stato su quest'ultima */
void loadReady(){
	/* Trovo la coda con priorità maggiore che ha processi da eseguire */
	int nqueue, priority;
	int id = getPRID();
	state_t temp;
	for (nqueue = MIN_PCB_PRIORITY; nqueue <= MAX_PCB_PRIORITY; nqueue++){
		/* Inizio a scandire da quelle con priorità più alta */
		if (readyProcs[id][nqueue] > 0){
			/* Rimuovi il processo dalla ready queue per evitare che 
			 * venga caricato due o più volte */
			pcb_t *torun = removeProcQ(&(readyQueue[id][nqueue]));
			/* Decremento sia il contatore specifico che quello globale */
			readyProcs[id][nqueue]--;
			readyProcsCounter--;
			/* Incremento il contatore dei processi running */
			runningProcsCounter++;
			/* Salvo un puntatore al processo correntemente in exe */
			currentProc[id] = torun;
			/* Aggiorno il TPR (priorità attuale) della CPU */
			memaddr *TPR = (memaddr *)TPR_ADDR;
			*TPR = torun->priority;
			/* Inizializzo il timer a TIME_SLICE e lo attivo */
			setTIMER(TIME_SLICE);
			setSTATUS(getSTATUS()|STATUS_TE);
			/* Setto correttamente lo stack pointer */
			torun->p_s.reg_sp = PFRAMES_START-(id*FRAME_SIZE);
			/* Lancio il nuovo processo */
			LDST(&(torun->p_s));
		}
	}
	/* Non ci sono processi nelle readyQueue, metto in attesa la CPU */
}

/*************************** SCHEDULER ********************************/

extern int key;

/* AVVIO DELLO SCHEDULER - Passaggio del controllo */
void scheduler(){
	int id = getPRID();
	STST(&pstate[id]);
	/* Salvo lo stato corrente in modo da riprendere l'esecuzione dello
	 * scheduler dopo ogni context switch */
	pstate[id].pc_epc = pstate[id].reg_t9 = (memaddr)scheduler;
	pstate[id].status = (getSTATUS());
	/* Finché ci sono processi pronti ad essere eseguiti */
	while(readyProcsCounter != 0){
		/* Innanzitutto se lo scheduler è stato richiamato dalla fine di un
		 * TIME_SLICE è bene che il processo sia reinserito nelle readyQueue
		 * in base alla priorità e tenendo conto dell'aging */
		if (currentProc[id] != NULL){
			debug(126, currentProc[id]->priority);
			(currentProc[id]->priority)--;
			debug(128, currentProc[id]->priority);
		}
		/* Lancio l'esecuzione del prossimo processo */
		loadReady();
	}
	WAIT();
}


