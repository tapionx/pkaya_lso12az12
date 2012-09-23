#include <asl.e>
#include <pcb.e>
#include "myConst.h"

/************ GESTIONE DEI SEMAFORI **************/

/* Array di SEMD con dimensione massima NUM_SEMAPHORES */
HIDDEN semd_t semd_table[NUM_SEMAPHORES];

void initASL(){
	int i;
	/* fino al numero massimo di processi */
	for (i=0; i < NUM_SEMAPHORES; i++){
		/* I device hanno una P sempre bloccante! I processi vengono risvegliati dall'interrupt! */
		semd_table[i].s_value = 0; //mutex
		semd_table[i].s_key = i;
		mkEmptyProcQ(&(semd_table[i].s_procQ));
	}	
}

semd_t* getSemd(int key)
{
	if (key < 0 || key > NUM_SEMAPHORES) return NULL; /* Valore della chiave non valido */
	return &(semd_table[key]);
}

int insertBlocked(int key, pcb_t *p)
{
	/* Non posso permettere l'inserimento di piu' di MAXPROC semafori */
	if (key > NUM_SEMAPHORES || key < 0 || p == NULL)
		return TRUE;
    p->p_semkey = key;
	insertProcQ(&(semd_table[key].s_procQ), p);
    return FALSE;
}  

pcb_t* removeBlocked(int key)
{
	/* Non posso permettere l'inserimento di piu' di MAXPROC semafori */
	if (key > NUM_SEMAPHORES || key < 0) 
		return NULL;
	pcb_t* removedPcb = removeProcQ(&(semd_table[key].s_procQ));
	if (removedPcb != NULL) removedPcb->p_semkey = -1; // reset della semkey
	return removedPcb;
}

pcb_t* headBlocked(int key)
{
	/* Non posso permettere l'inserimento di piu' di MAXPROC semafori */
	if (key > NUM_SEMAPHORES || key < 0) 
		return NULL;
	pcb_t* firstPcb = headProcQ(&(semd_table[key].s_procQ));
	return firstPcb;
} 

pcb_t* outBlocked(pcb_t* p)
{
	/* Per prima cosa marco il processo come da terminare. Se non lo
	 * facessi a priori rischierei che mentre cerco il processo in qualche
	 * coda questo venga prelevato e inserito nella readyQueue dello 
	 * scheduler! */
    /* estraggo la chiave */
    int semKey = p->p_semkey;
	/* estraggo il puntatore al semd */
    semd_t* pSem = getSemd(semKey);
    /* Se il semaforo non esiste nella ASL ritorno NULL */
    if (pSem == NULL) {
		return NULL;
	}
    /* Dobbiamo verificare che p sia presente nella coda di pSem */
    struct list_head* qHead =  &(pSem->s_procQ);
    /* inizializzo l'elemento corrente per il ciclo for */
    struct list_head* cur = qHead;
    /* scorro sulla lista */
    list_for_each(cur, qHead){
		/* estraggo l'indirizzo del contenitore dell'elemento corrente */	
        pcb_t* curPcb = container_of(cur, pcb_t, p_next);
        /* se l'elemento è quello che cerco */
        if (curPcb == p){
			/* rimuovo l'elemento */
            list_del(cur);
            pSem->s_value--;
            /* ne ritorno l'indirizzo */
            return p;
        }
    }
    /* se non c'è ritorno NULL */
    return NULL;
}


/* #19 ------------------------------------------------------*/
/*
 * Rimuove il PCB puntato da p dalla coda del semaforo su cui e'
 * bloccato (indicato da p->p_semkey). Inoltre, elimina tutti i processi
 * dell'albero radicato in p (ossia tutti i processi che hanno come avo
 * p) dalle eventuali code dei semafori su cui sono bloccati. 
 * 
 * MODIFICA PHASE2: Marca anche i processi da rimuovere come da terminare
 * (poiché è l'unico motivo per cui si utilizza) (nella outBlocked!)
 *
 */
void outChildBlocked(pcb_t* p)
{
	/* Caso base: p non ha figli */
	if (!list_empty(&(p->p_child))){
		/* Caso ricorsivo/induttivo */
		struct list_head* childIt = &(p->p_child);
		/* scorro su ogni figlio */
		list_for_each(childIt, &(p->p_child))
		{
			pcb_t* curPcb = container_of(childIt, pcb_t, p_sib);
			/* elimino i processi figli */
			outChildBlocked(curPcb);
		}
	}
	/* Infine rimuovo il processo originario */
	if(outBlocked(p) == NULL) p->wanted = TRUE;
	
}
