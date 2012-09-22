#include "uMPStypes.h"
#include "types11.h"
#include "pcb.e"
#include "asl.e"
#include "myConst.h"
#include "kernelVariables.h"

/** Handlers delle 11 System Call */

/* System Call #1  : Create Process
 * genera un processo figlio del processo chiamante
 * statep   = indirizzo dello state_t da caricare nel nuovo processo
 * priority = priorità del nuovo processo
 * return  -> 0 ok / -1 errore (coda piena PLB)
 */
int create_process(state_t *statep, int priority)
{
	U32 cpuid = getPRID();
	/* recupero il processo chiamante */
	state_t *oldProcess = GET_OLD_SYSBK();
	/* ottengo il processo corrente */
	pcb_t *processoCorrente = currentProcess[cpuid];
	/* alloco un nuovo processo */
	pcb_t *nuovoProcesso = allocPcb();
	/* se non è possibile allocare un nuovo processo */
	if(nuovoProcesso == NULL)
	{
		/* setto il registro v0 a -1 (specifiche-failure) */
		return -1;
		/* riprendo l'esecuzione del processo chiamante */
		LDST(&(oldProcess));
	}
	/* altrimenti, se posso allocare il processo */
	/* copio lo stato nel processo figlio */
	copyState(statep, &(nuovoProcesso->p_s));
	/* setto la priorità del nuovo processo */
	nuovoProcesso->priority = priority;
	/* inserisco il nuovo processo come figlio del chiamante */
	insertChild(processoCorrente, nuovoProcesso);
	/* inserisco il nuovo processo in qualche ready queue */
	addReady(nuovoProcesso);
	/* Ritorno 0 = OK */
	return 0;
}

/* System Call #2  : Create Brother
 * genera un processo fratello del processo chiamante
 * statep   = indirizzo del nuovo processo
 * priority = priorità del nuovo processo
 * return  -> 0 ok / -1 errore
 */
int create_brother(state_t *statep, int priority)
{
	U32 cpuid = getPRID();
	/* recupero il processo chiamante */
	state_t *oldProcess = GET_OLD_SYSBK();
	/* ottengo il processo corrente */
	pcb_t *processoCorrente = currentProcess[cpuid];
	/* alloco un nuovo processo */
	pcb_t *nuovoProcesso = allocPcb();
	/* se non è possibile allocare un nuovo processo */
	/* o se il processo chiamante non ha un padre */
	if(nuovoProcesso == NULL)
	{
		/* setto il registro v0 a -1 (specifiche-failure) */
		return -1;
		/* riprendo l'esecuzione del processo chiamante */
		LDST(&(oldProcess));
	}
	/* altrimenti, se posso allocare il processo */
	/* copio lo stato nel processo figlio */
	copyState(statep, &(nuovoProcesso->p_s));
	/* setto la priorità del nuovo processo */
	nuovoProcesso->priority = priority;
	/* inserisco il nuovo processo come FRATELLO del chiamante */
	list_add_tail(&(nuovoProcesso->p_sib), &(processoCorrente->p_sib));
	nuovoProcesso->p_parent = processoCorrente->p_parent;
	/* inserisco il nuovo processo in qualche ready queue */
	addReady(nuovoProcesso);	
	/* ritorno 0 = OK */
	return 0;
}

/* System Call #3  : Terminate Process
 * termina il processo corrente e tutti i discendenti
 * LIBERARE TUTTE LE RISORSE
 */
void terminate_process()
{
	/* ottengo il processore corrente*/
	U32 cpuid = getPRID();
	/* ottengo il processo corrente */
	pcb_t *processoCorrente = currentProcess[cpuid]; 
	/* elimino il processo e tutti i figli da tutti i semafori */
	/* cioe' tutti i processi in stato di WAIT */
	outChildBlocked(processoCorrente); /* Marca tutti i figli come da terminare */
	/* Infine termina il processo chiamante liberando il suo pcb e ritornando il controllo allo scheduler
	 * NOTA: Il processo terminato non sarà certamente nella readyQueue! */
	freePcb(currentProcess[cpuid]);
	scheduler();
}

/* System Call #4  : Verhogen
 * esegue la V sul semaforo con chiave semKey
 * il primo processo in coda sul semaforo va in esecuzione
 */
void verhogen(int semKey){
	lock(semKey);
	semd_t *semaphore = getSemd(semKey);
	semaphore->s_value += 1;
	pcb_t *toWake = removeBlocked(semKey);
	if (toWake != NULL){
		addReady(toWake); // sveglio il prossimo
	}
	free(semKey);
}

/* System Call #5  : Passeren
 * esegue la P sul semaforo con chiave semKey
 * il processo che ha chiamato la syscall si mette in attesa
 */
void passeren(int semKey){
	lock(semKey);
	int cpuid = getPRID();
	semd_t *semaphore = getSemd(semKey);
	semaphore->s_value -= 1;
	if (semaphore->s_value < 0){
		insertBlocked(semKey, currentProcess[cpuid]);
		LDST(&(scheduler_states[cpuid]));
	}
	free(semKey);
}

/* System Call #6  : Get CPU Time
 * restituisce il tempo CPU usato dal processo in millisecondi
 * -> IL KERNEL DEVE TENERE LA CONTABILITA DEL TEMPO CPU DEI PROCESSI
 */
int get_cpu_time()
{
	/* TODO: bisogna inizializzare un pcb con il TOD e qui restituire la differenza tra il TOD attuale e il suo! */
	U32 cpuid = getPRID();
	/* ottengo il processo corrente */ 
	return currentProcess[cpuid]->time;
	/* continuo l'esecuzione */
}

/* System Call #7  : Wait Clock
 * esegue una P sul semaforo dello PSEUDO CLOCK TIMER (PCT)
 * il PCT esegue una V ogni 100 millisecondi e sblocca tutti i processi
 */
void wait_clock()
{
	/* Semplicemente chiamiamo una P sul semaforo dedicato al PCT */
	passeren(PCT_SEM);
}

/* System Call #8  : Wait I/O
 * esegue una P sul semaforo del device specificato dai parametri
 * intline = linea di interrupt da 3 a 7 (5)
 * dnum  = numero del device, da 0 a 7   (8)
 * read = TRUE: terminal read, FALSE: terminal write
 * return -> status del device
 */
int wait_io(int intline, int dnum, int read)
{
	/* calcolo il numero del semaforo da usare */
	int semKey = GET_TERM_SEM(dnum, read);
	U32 cpuid = getPRID();
	passeren(semKey);
	/* calcolo indice del vettore delle risposte */
	int statusNum = GET_TERM_STATUS(dnum, read);
	/* Se la P non era bloccante (interrupt anticipato!) ritorno il valore */
	return devStatus;
}

/* System Call #9  : Specify PRG State Vector
 * definire handler personalizzato per Program Trap per il processo corrente
 * oldp = indirizzo della custom OLDAREA
 * newp = indirizzo della custom NEWAREA
 */
void specify_prg_state_vector(state_t *oldp, state_t *newp)
{
	U32 cpuid = getPRID();
	/* copio i custom handlers nel pcb_t del processo chiamante*/
	currentProcess[cpuid]->custom_handlers[PGMTRAP_NEWAREA_INDEX] = newp;
	currentProcess[cpuid]->custom_handlers[PGMTRAP_OLDAREA_INDEX] = oldp;
}

/* System Call #10 : Specify TLB State Vector
 * definire handler personalizzato per TLB Exception per il processo corrente
 * oldp = indirizzo della custom OLDAREA
 * newp = indirizzo della custom NEWAREA
 */
void specify_tlb_state_vector(state_t *oldp, state_t *newp)
{
	U32 cpuid = getPRID();
	/* copio i custom handlers nel pcb_t del processo chiamante*/
	currentProcess[cpuid]->custom_handlers[TLB_NEWAREA_INDEX] = newp;
	currentProcess[cpuid]->custom_handlers[TLB_OLDAREA_INDEX] = oldp;
}

/* System Call #11 : Specify SYS State Vector
 * definire handler personalizzato per SYSCALL/BreakPoint per il processo corrente
 * oldp = indirizzo della custom OLDAREA
 * newp = indirizzo della custom NEWAREA
 */
void specify_sys_state_vector(state_t *oldp, state_t *newp)
{
	U32 cpuid = getPRID();
	/* copio i custom handlers nel pcb_t del processo chiamante*/
	currentProcess[cpuid]->custom_handlers[SYSBK_NEWAREA_INDEX] = newp;
	currentProcess[cpuid]->custom_handlers[SYSBK_OLDAREA_INDEX] = oldp;
}
