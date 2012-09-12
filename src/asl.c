#include <asl.e>
#include <pcb.e>

/************ GESTIONE DEI SEMAFORI **************/

/* Array di SEMD con dimensione massima MAXPROC*3 (restando "larghi") */
HIDDEN semd_t semd_table[MAXPROC+MAX_DEVICES];

/* Elemento sentinella della lista si semafori liberi */
HIDDEN struct list_head semdFree_h;

/* Elemento sentinella dell'ASL*/
HIDDEN struct list_head semd_h;

/* #14 ------------------------------------------------------*/
/* 
 * Restituisce il puntatore al SEMD nella ASL la cui chiave è pari
 * a key. Se non esiste un elemento nella ASL con chiave uguale a key,
 * viene restituito NULL.
 * 
 * TODO: si può trasformare l'iterazione sulla lista dei semafori in un 
 * accesso diretto poiché alla chiave key corrisponde il semaforo key?
*/
semd_t* getSemd(int key)
{
	/* Per ogni semd nella ASL */
	struct list_head* cur = &(semd_h);
	list_for_each(cur, &(semd_h))
    {
		/*ottengo il puntatore all'array  di semd_t */
        semd_t* curSem = container_of (cur, semd_t, s_next); 
        if (curSem == NULL) return NULL;
        /* ottengo il valore della chiave */
        int allockey = (curSem->s_key); 
        /* se la chiave è uguale a key*/
        if (key == allockey)
        {
			/* ritorno il puntatore al semd_t */
			return curSem; 
        }
    }
    /* altrimenti se la key non è presente restituisco NULL */
	return NULL; 
}

/* #15 ------------------------------------------------------*/
/* 
 * Viene inserito il PCB puntato da p nella coda dei processi 
 * bloccati associata al SEMD con chiave key. Se il semafoto 
 * corrispondente non è presente nella ASL, alloca un nuovo SEMD dalla 
 * lista di quelli liberi (semdFree) e lo inserisce nella ASL, settando 
 * i campi in maniera opportuna (i.e.key e s_procQ). Se non è possibile 
 * allocare un nuovo SEMD perchè la lista di quelli liberi è vuota, 
 * restituisce TRUE. In tutti gli altri casi False 
 * 
*/
int insertBlocked(int key, pcb_t *p)
{
	/* Non posso permettere l'inserimento di piu' di MAXPROC semafori */
	if (key > MAXPROC) 
		return TRUE;
    /* richiamo la funzione che mi trova il puntatore al semaforo, se presente nell'ASL altrimenti NULL */
    semd_t* psem = getSemd(key);
    /* Setto la chiave del semaforo su cui p e' bloccato */
    p->p_semkey = key;
    /*se ho trovato il puntatore alla struttura semd_t con chiave = a key */
    if (psem != NULL) 
    {
		/* decremento il valore del semaforo */
		psem->s_value--;
        /* la inserisco nella coda dei processi bloccati del semd_t */
        insertProcQ(&(psem->s_procQ), p);
        /* ritorno falso */
        return FALSE;
    }  
    /* Se la lista SemdFree non è vuota.. */
    if (!(list_empty(&(semdFree_h)))) 
    {    
		/* ottengo il puntatore a un semdFree */
        semd_t* allocsemd = container_of (list_next(&(semdFree_h)), semd_t, s_next); 
        /* tolgo l'elemento dalla lista semdFree */
        list_del(list_next(&(semdFree_h))); 
        
        /* Inizializzo il semd_t puntato da allocsemd */
        allocsemd->s_value = 0;
        /* Setto la chiave del semaforo nel processo */
        allocsemd->s_key = key;
        mkEmptyProcQ(&(allocsemd->s_procQ));
        /* alloco nella ASL un nuovo SEMD dalla lista semdFree */
        list_add(&(allocsemd->s_next), &(semd_h));
        /* Infine inserisco il processo bloccato sul semaforo */
        insertProcQ(&(allocsemd->s_procQ), p);
		/* ritorno falso */
        return FALSE; 
    }
    else 
    {
		/* ...altrimenti mi faccio restituire true perchè semdFree è vuota */
        return TRUE; 
    }

}

/* #16 ------------------------------------------------------*/
/*
 * Ritorna il primo PCB dalla coda dei processi bloccati (s_procQ) 
 * associata al SEMD della ASL con chiave key. 
 * Se tale descrittore non esiste nella ASL restituisce NULL. 
 * Altrimenti restituisce l´elemento rimosso. 
 * Se la coda dei processi bloccati per il semaforo diventa vuota, 
 * rimuove il descrittore corrispondente dalla ASL e lo inserisce 
 * nella coda dei descrittori liberi(semdFree).
 * 
*/
pcb_t* removeBlocked(int key)
{
	/*richiamo la funzione che mi trova il puntatore al semaforo, se presente nell'ASL altrimenti NULL */
    semd_t* psem = getSemd(key); 
    /* se ho trovato il puntatore alla struttura semd_t con chiave = a key.. */
    if (psem != NULL) 
    {
        /* Ritornera' NULL se il processo non esiste, il processo stesso altrimenti */
        pcb_t* removedPcb = removeProcQ(&(psem->s_procQ));
        /* Prima controllo che la coda dei processi bloccati non sia vuota.
         * In caso contrario tolgo il semaforo dalla ASL e lo inserisco nella semdFree */
        if (removedPcb != NULL)
        {
			/* Incremento il valore del semaforo */
			psem->s_value++;
			if (list_empty(&(psem->s_procQ))){
				/* Dealloco il semaforo dalla ASL */
				list_del(&(psem->s_next));
				/* Lo inserisco nella semdFree */
				list_add(&(psem->s_next), &(semdFree_h));
				/* Infine ritorno il processo rimosso */
			}
			return removedPcb;
		}
    }
    /* Se il semaforo non e' nella ASL ritorno NULL */
    return NULL;
}

/* #17 ------------------------------------------------------*/
/*
 * Rimuove il PCB puntato da p dalla coda del semaforo su cui e' 
 * bloccato (indicato da p->p_semKey). Se il PCB non compare in tale 
 * coda, allora restituisce NULL (condizione di errore). Altrimenti, 
 * restituisce p.
 * 
*/
pcb_t* outBlocked(pcb_t* p)
{
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
            /* ne ritorno l'indirizzo */
            return p;
        }
    }
    /* se non c'è ritorno NULL */
    return NULL;
}

/* #18 ------------------------------------------------------*/
/*
 * Restituisce (senza rimuovere) il puntatore al pcb che si trova
 * in testa alla coda dei processi associata al semd con chiave key.
 * Ritorna NULL se il semd non compare nella ASL oppure se compare
 * ma la sua coda dei processi e' vuota. 
 */
pcb_t* headBlocked(int key)
{
	semd_t* sem = getSemd(key);
	if (sem == NULL || list_empty(&(sem->s_procQ))) return NULL;
	pcb_t* firstPcb = headProcQ(&(sem->s_procQ));
	return firstPcb;
} 

/* #19 ------------------------------------------------------*/
/*
 * Rimuove il PCB puntato da p dalla coda del semaforo su cui e'
 * bloccato (indicato da p->p_semkey). Inoltre, elimina tutti i processi
 * dell'albero radicato in p (ossia tutti i processi che hanno come avo
 * p) dalle eventuali code dei semafori su cui sono bloccati. 
 *
 */
void outChildBlocked(pcb_t* p)
{
	/* Caso base: p non ha figli */
	if (list_empty(&(p->p_child)))
		 outBlocked(p);
	else {
		/* Caso ricorsivo/induttivo */
		struct list_head* childIt = &(p->p_child);
		/* scorro su ogni figlio */
		list_for_each(childIt, &(p->p_child))
		{
			pcb_t* curPcb = container_of(childIt, pcb_t, p_sib);
			/* elimino i processi figli */
			outChildBlocked(curPcb);
		}
		/* Infine rimuovo il processo originario */
		outBlocked(p);
	}
}

/* #20 ------------------------------------------------------*/
/*
 * Inizializza la lista dei semdFree in modo da contenere tutti gli
 * elementi della semd_table. Questo metodo viene invocato una volta 
 * sola durante l'inizializzazione della struttura dati. 
 *
 */
void initASL()
{
	/* contatore */
	int i;
	/* la sentinella */
	INIT_LIST_HEAD(&(semdFree_h));
	INIT_LIST_HEAD(&(semd_h));
	/* fino al numero massimo di processi */
	for (i=0; i < MAXPROC; i++)
	{
		/* aggiungo il semaforo alla lista */
		list_add(&(semd_table[i].s_next), &(semdFree_h)); 
	}	
}
