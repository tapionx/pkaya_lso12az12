#include "myconst.h"
#include "listx.h"
#include "types11.h"
#include "pcb.e"
#include "asl.e"
#include "libumps.h"
#include "const.h"

/* Globali (per ogni CPU) */
int activeCpu[NUM_CPU]; /* Solo CPU0 è attiva all'inizio */
int globalprocs; /* contatore globale dei processi - necessario per 
determinare di volta in volta l'indirizzo del prossimo stack pointer */
int procs[NUM_CPU]; /* contatore dei processi */
int softprocs[NUM_CPU]; /* contatore dei processi bloccati su I/O */
pcb_t *currentproc[NUM_CPU]; /* puntatore al processo in esecuzione attuale */
struct list_head readyQueue[NUM_CPU]; /* coda dei processi in stato ready */
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
			INIT_LIST_HEAD(&(readyQueue[i]));
		initReady = TRUE;
	}
	/* Mi assicuro di separare gli stack (di un FRAME) */
	proc->p_s.reg_sp = RAMTOP-(globalprocs*FRAME_SIZE);
	/* Inserisco il pcb_t passato nella readyQueue della CPU id */
	insertProcQ(&(readyQueue[id]), proc);
}

/* Questa funzione si occupa di estrare un processo dalla readyQueue
 * della CPU[id] e di caricarne lo stato sulla CPU[id] */
void loadReadyFrom(int id){
	struct list_head *pos;
	list_for_each(pos, &(readyQueue[id])){
		pcb_t *curPcb = (container_of(pos, pcb_t, p_next));
		int key = curPcb->p_semkey;
		printn("Trovato processo %\n",key);
	}
	/* Prendo il pcb_t da eseguire dalla readyQueue */
	pcb_t *torun = removeProcQ(&(readyQueue[id]));
	/* Se la CPU non è stata avviata la resetto */
	if (!activeCpu[id]){
		INITCPU(id, &(torun->p_s), &(new_old_areas[id]));
		activeCpu[id] = TRUE;
	} else {
		/* Altrimenti carico semplicemente un nuovo stato */
		LDST(&(torun->p_s));
		/* PASSAGGIO DI CONTROLLO */
	}
}

/* AVVIO DELLO SCHEDULER - Passaggio del controllo */
void scheduler(){
	/* Setto tutte le CPU (tranne la 0) come inattive */
	int id = getPRID();
	activeCpu[id] = TRUE;
	/* Carico i processi dalla readyQueue */
	/* Innanzitutto salvo lo stato per richiamare lo scheduler */
	pstate[id].pc_epc = STST(&(pstate[id]));
	loadReadyFrom(id);
}
