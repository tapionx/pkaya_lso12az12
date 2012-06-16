#include <pcb.e>
#include "utils.h"
#include "myconst.h"

/* Elemento sentinella (dummy) della lista pcbFree */
LIST_HEAD(pcbFree_h);

/************ GESTIONE DEI PCB **************/


/* #1 ------------------------------------------------------*/
/* Inizializza la struttura dati (senza inizializzare i membri dei vari pcb) */
void initPcbs()
{
	HIDDEN pcb_t pcbFree_table[MAXPROC];
	int i; /* Counter */
    /* Inizializzazione della sentinella */
    INIT_LIST_HEAD(&(pcbFree_h));
    /* Inizializzazione della lista pcbFree */
    for (i = 0; i < MAXPROC; i++)
    {
        pcb_t* newPcbEl = &pcbFree_table[i];
        /* Infine congiungo opportunamente i concatenatori di elemento e sentinella/head */
        list_add(&(newPcbEl->p_next), &(pcbFree_h));
    }
}


/* #2 ------------------------------------------------------*/
/* Libera spazio per un processo: quello puntato da p */
void freePcb(pcb_t *p)
{
    /* Non c'è bisogno di reinizializzare *p poiché verrà inizializzato in fase di allocazione */
    list_add(&(p->p_next), &(pcbFree_h));
}


/* #3 ------------------------------------------------------*/
/* Alloca un elemento della lista dei pcb liberi per un processo attivo  (POP)*/
pcb_t* allocPcb()
{
   /* Se la lista pcbFree è vuota ritorna NULL */
   if (list_empty(&(pcbFree_h))) return NULL;

   /* Altrimenti: */
   /* Salvo il puntatore del pcb da allocare */
   pcb_t* allocPpcb = container_of(pcbFree_h.next, pcb_t, p_next);
   /* Stacco l'elemento dalla lista */
   list_del(pcbFree_h.next);
   /* Inizializzo i campi del pcb_t puntato da allocPpcb */
   INIT_LIST_HEAD(&(allocPpcb->p_next));
   INIT_LIST_HEAD(&(allocPpcb->p_child));
   INIT_LIST_HEAD(&(allocPpcb->p_sib));
   allocPpcb->p_parent = NULL;
   cleanState(&(allocPpcb->p_s));
   allocPpcb->priority = DEFAULT_PCB_PRIORITY;
   allocPpcb->p_semkey = -1; /* non è bloccato su alcun semaforo */
   /* Inizializzo il custom handler a NULL */
   int i;
   for(i=0;i<8;i++)
   {
		allocPpcb->custom_handlers[i] = NULL;
   }
   /* Ritorno il puntatore al pcb_t */
   return allocPpcb;
}


/************ GESTIONE DELLE CODE DI PCB **************/

/* #4 ------------------------------------------------------*/
/* Inizializza la lista dei PCB, inizializzando l'elemento sentinella */
void mkEmptyProcQ(struct list_head *head)
{
    /* inizializzo i puntatori della sentinella */
    INIT_LIST_HEAD(head);
}


/* #5 ------------------------------------------------------*/
/* Restituisce TRUE se la lista puntata da HEAD è vuota, false altrimenti */
int emptyProcQ(struct list_head *head)
{ 
    return list_empty(head);
}



/* #6 -------------------------------------------------------*/
/* inserisce l’elemento puntato da p nella coda dei processi puntata da head. 
 * L’inserimento deve avvenire tenendo conto della priorita’ di ciascun pcb 
 * (campo p-­‐>priority). La coda dei processi deve essere ordinata in base 
 * alla priorita' dei PCB, in ordine decrescente (i.e. l’elemento di testa  
 * e’ l’elemento con la priorita' piu’ alta). 
 *
*/
void insertProcQ(struct list_head* head, pcb_t* p)
{
    /* Salvo la priorità di p */
    int p_priority = p->priority;
    struct list_head* pos;
    /* Se la lista è vuota aggiungo l'elemento in coda (il list_for_each
     * non verrebbe neanche iniziato) */
    if (list_empty(head)){
        list_add(&(p->p_next), head);
	 }
    else
        list_for_each(pos, head){ /* Inizia da head->next */
            pcb_t* curPcb = container_of(pos, pcb_t, p_next);
            /* Salvo la priorità dell'elemento corrente */
            int cur_priority = curPcb->priority;
            
            /* Se la priorità di p è < di quella dell'elemento corrente
             * e l'elemento successivo è la sentinella, devo forzatamente
             * inserire p in coda dopo l'elemento corrente */
            if (p_priority < cur_priority && list_next(pos) == head)
            {
                list_add(&(p->p_next), pos);
                return; /* Non continuo il ciclo */
            }
            /* Se invece la priorità di p è > a quella dell'elemento 
             * corrente lo aggiungo prima di lui */
            if (p_priority > cur_priority)
            {
                list_add_tail(&(p->p_next), pos);
                return; /* Non continuo il ciclo */
            }
            else { /* Caso priorità uguale, se ci sono più processi con
            stessa priorità lo aggiungo alla fine di questi secondo 
            politica fair FIFO */
				/* Trovo la priorità del prossimo processo */
				pcb_t *next = container_of(list_next(pos), pcb_t, p_next);
				int next_priority = next->priority;
				if (p_priority > next_priority || list_next(pos) == head){
					list_add(&(p->p_next), pos);
					return;
				}
			}
              
            /* Il caso rimanente è se l'elemento da inserire ha priorità
             * minore di quello corrente ma non siamo giunti alla fine 
             * della lista. Allora è necessario continuare a scandire 
             * gli elementi finché non si trova una posizione utile per 
             * l'inserimento ricadendo nei primi due casi */
        }
}



/* #7 ------------------------------------------------------*/
/*
 * Restituisce puntatore all’elemento di testa della coda 
 * dei processi da head, SENZA RIMUOVERLO.
 * Ritorna NULL se la coda non ha elementi.
 *
*/
pcb_t *headProcQ(struct list_head *head)
{
    /* Se la lista è vuota ritorno null */
    if(list_empty(head))
        return NULL;
    /* estraggo l'indirizzo del primo elemento della lista */
    struct list_head *first = list_next(head);
    /* estraggo l'indirizzo del contenitore di first */
    pcb_t *firstPcb = container_of(first, pcb_t, p_next);
    /* lo restituisco */
    return firstPcb;
}



/* #8 ------------------------------------------------------*/
/*
 * Rimuove il primo elemento dalla coda dei processi puntata da head.
 * ritorna NULL se la coda è vuota, altrimenti ritorna il puntatore all'
 * elemento rimosso dalla lista.
 *
*/
pcb_t* removeProcQ(struct list_head *head)
{
    /* Se la lista è vuota ritorno NULL */
    if (list_empty(head))
    	return NULL;
	/* estraggo l'indirizzo del primo elemento della lista */
    struct list_head* first = list_next(head);
    /* estraggo l'indirizzo del contenitore di first */
    pcb_t* firstPcb = container_of(first, pcb_t, p_next);
    /* elimino l'elemento */
    list_del(first);
    /* ritorno il puntatore all'elemento rimosso */
    return firstPcb;
}



/* #9 ------------------------------------------------------*/
/* 
 * Rimuove il PCB puntato da p dalla coda dei processi puntata da head.
 * se p non è presente nella coda restiutisce NULL. (NOTA: p può
 * trovarsi in una posizione arbitraria della coda.
 *
*/    
pcb_t *outProcQ(struct list_head *head, pcb_t *p)
{
    /*  Scorro la lista dei pcb  */
    struct list_head* pos;
    list_for_each(pos, head)
    {
        /*  Estraggo il puntatore all'elemento corrente */
        pcb_t* curr = container_of(pos, pcb_t, p_next);
        /*  Se l'elemento corrente è uguale a quello da eliminare */
        if(p == curr)
        {
            list_del((&(p->p_next)));
            return p;
        }
    }
    /* ritorno null se l'elemento richiesto non è presente */
    return NULL;
}


/************ GESTIONE DEGLI ALBERI DI PCB **************/

/* #10 ------------------------------------------------------*/
/* restuisce TRUE se il PCB puntato da p non ha figli,restituisce FALSE altrimenti*/
int emptyChild(pcb_t *p) 
{
   return list_empty(&(p->p_child));
}


/* #11 ------------------------------------------------------*/
/* Inserisce il PCB puntato da p come figlio del PCB puntato da prnt */
void insertChild(pcb_t *prnt,pcb_t *p) 
{
    p->p_parent = prnt;
    /* aggiungo l'elemento in coda alla lista dei figli*/
    list_add_tail(&(p->p_sib), &(prnt->p_child));
}



/* #12 ------------------------------------------------------*/
/* Rimuove il primo figlio del PCB puntato da p. Se p non ha figli, restituisce NULL */
pcb_t* removeChild(pcb_t *p)
{
	/*Controllo se p non ha figli.*/    
    if (list_empty(&(p->p_child)))
    {
        return NULL;
	} 
	else 
	{
		/* estraggo l'indirizzo del primo concatenatore figlio */
        struct list_head* firstChild = list_next(&(p->p_child));
        /* estraggo l'indirizzo del contenitore del primo concatenatore */
        pcb_t* firstPcb = container_of(firstChild, pcb_t, p_sib); 
        /* elimino il puntatore al padre */
        firstPcb->p_parent = NULL;
        /* elimino il figlio dalla lista */
        list_del(firstChild);	
        /* restituisco il puntatore al figlio rimosso */
		return firstPcb;
	}
}



/* #13 ------------------------------------------------------*/
/* 
 * Rimuove il PCB puntato da p dalla lista dei figli del padre. Se il PCB puntato da p non 
 * ha un padre,restituisce NULL. Altrimenti restituisce elem rimosso(p).A differenza della 
 * removeChild,p può trovarsi in una posizione arbitraria
 * (ossia non è necessariamente il primo figlio del padre)
 * 
*/
pcb_t* outChild(pcb_t *p) 
{
    /*controllo se p ha un padre.Se non ha un padre restituisco NULL*/
    if ((&(p->p_parent))== NULL)
        return NULL;
        
        else 
        {
            /*elimino elemento dall'albero*/
            list_del (&(p->p_sib));
            p->p_parent = NULL;
            /*restituisco elemento da rimuovere*/
            return p;
        }
}


