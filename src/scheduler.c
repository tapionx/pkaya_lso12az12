#include "myconst.h"
#include "listx.h"
#include "types11.h"
#include "pcb.e"
#include "asl.e"
#include "libumps.h"
#include "const.h"

/* Globali (per ogni CPU) */
int globalprocs; /* contatore globale dei processi - necessario per 
determinare di volta in volta l'indirizzo del prossimo stack pointer */
int procs[NUM_CPU]; /* contatore dei processi */
int softprocs[NUM_CPU]; /* contatore dei processi bloccati su I/O */
pcb_t *currentproc[NUM_CPU]; /* puntatore al processo in esecuzione attuale */
struct list_head readyQueue[NUM_CPU][MAX_PCB_PRIORITY]; /* coda dei processi in stato ready */
int initReady = FALSE; /* Stato dell'inizializzazione della readyQueue */

/* Variabili del kernel */
extern new_old_areas[NUM_CPU][NUM_AREAS];
extern state_t pstate[NUM_CPU]; /* stati di load/store per le varie cpu */

/* Forward callback */
void scheduler();


/* Questa funzione inserisce nella readyQueue della CPU[id] il pcb_t 
passato */
void addReady(int id, pcb_t *proc){
	/* Innanzitutto inizializzo le code, se non lo sono, per evitare
	 * puntatori non inizializzati (e possibili kernel panic) */
	procs[id]++;
	globalprocs++;
	if (!initReady){ /* Eseguito solo una volta per tutte le code */
		int i;
		procs[id] = 0;
		for (i=0; i<NUM_CPU; i++)
			INIT_LIST_HEAD(&(readyQueue[i][proc->priority]));
		initReady = TRUE;
	}
	/* Mi assicuro di separare gli stack (di un FRAME) */
	proc->p_s.reg_sp = RAMTOP-(globalprocs*FRAME_SIZE);
	/* Inserisco il pcb_t passato nella readyQueue della CPU id */
	insertProcQ(&(readyQueue[id][proc->priority]), proc);
}

/* Questa funzione si occupa di estrare un processo dalla readyQueue
 * della CPU[id] e di caricarne lo stato sulla CPU[id] */
void loadReadyFrom(int id){
	struct list_head *pos;
	list_for_each(pos, &(readyQueue[id][0])){
		pcb_t *curPcb = (container_of(pos, pcb_t, p_next));
		int key = curPcb->p_semkey;
	}
	/* Prendo il pcb_t da eseguire dalla readyQueue */
	pcb_t *torun = removeProcQ(&(readyQueue[id][0]));
	/* Altrimenti carico semplicemente un nuovo stato */
	LDST(&(torun->p_s));
	/* PASSAGGIO DI CONTROLLO */
}

/* AVVIO DELLO SCHEDULER - Passaggio del controllo */
void scheduler(){
	
}


