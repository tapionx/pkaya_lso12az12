#include "uMPStypes.h"
#include "types11.h"
#include "pcb.e"
#include "asl.e"
#include "myConst.h"
#include "kernelVariables.h" 

/* Handlers delle 11 System Call */

/* System Call #1  : Create Process
 * genera un processo figlio del processo chiamante
 * statep   = indirizzo dello state_t da caricare nel nuovo processo
 * priority = priorità del nuovo processo
 * return  -> 0 ok / -1 errore (coda piena PLB)
 */
void create_process(state_t *statep, int priority)
{
	/* ottengo il processo corrente */
	pcb_t *processoCorrente = currentProcess[getPRID()];
	/* alloco un nuovo processo */
	pcb_t *nuovoProcesso = allocPcb();
	/* se non è possibile allocare un nuovo processo */
	if(nuovoProcesso == NULL)
	{
		/* setto il registro v0 a -1 (specifiche-failure) */
		processoCorrente->p_s.reg_v0 = -1;
		/* riprendo l'esecuzione del processo chiamante */
		LDST(&(processoCorrente->p_s));
	}
	/* altrimenti, se posso allocare il processo */
	/* copio lo stato nel processo figlio */
	copyState(statep, &(nuovoProcesso->p_s));
	/* setto la priorità del nuovo processo */
	nuovoProcesso->priority = priority;
	/* inserisco il nuovo processo come figlio del chiamante */
	insertChild(processoCorrente, nuovoProcesso);
	/* setto il registro v0 a 0 (specifiche-success) */
	processoCorrente->p_s.reg_v0 = 0;
	/* inserisco il nuovo processo in qualche ready queue */
	addReady(nuovoProcesso);
}

/* System Call #2  : Create Brother
 * genera un processo fratello del processo chiamante
 * statep   = indirizzo del nuovo processo
 * priority = priorità del nuovo processo
 * return  -> 0 ok / -1 errore
 */
void create_brother(state_t *statep, int priority)
{
	/* ottengo il processo corrente */
	pcb_t *processoCorrente = currentProcess[getPRID()];
	/* alloco un nuovo processo */
	pcb_t *nuovoProcesso = allocPcb();
	/* se non è possibile allocare un nuovo processo */
	/* o se il processo chiamante non ha un padre */
	if(nuovoProcesso == NULL)
	{
		/* setto il registro v0 a -1 (specifiche-failure) */
		processoCorrente->p_s.reg_v0 = -1;
		/* riprendo l'esecuzione del processo chiamante */
		LDST(&(processoCorrente->p_s));
	}
	/* altrimenti, se posso allocare il processo */
	/* copio lo stato nel processo figlio */
	copyState(statep, &(nuovoProcesso->p_s));
	/* setto la priorità del nuovo processo */
	nuovoProcesso->priority = priority;
	/* inserisco il nuovo processo come FRATELLO del chiamante */
	list_add_tail(&(nuovoProcesso->p_sib), &(processoCorrente->p_sib));
	nuovoProcesso->p_parent = processoCorrente->p_parent;
	/* setto il registro v0 a 0 (specifiche-success) */
	processoCorrente->p_s.reg_v0 = 0;
	/* inserisco il nuovo processo in qualche ready queue */
	addReady(nuovoProcesso);	
}

/* System Call #3  : Terminate Process
 * termina il processo corrente e tutti i discendenti
 * LIBERARE TUTTE LE RISORSE
 */
void terminate_process()
{
	/* ottengo il processore corrente*/
	U32 prid = getPRID();
	/* ottengo il processo corrente */
	pcb_t *processoCorrente = currentProcess[prid]; 
	/* elimino il processo e tutti i figli da tutti i semafori */
	/* cioe' tutti i processi in stato di WAIT */
	outChildBlocked(processoCorrente); /* Marca tutti i figli come da terminare */
	/* Infine termina il processo chiamante liberando il suo pcb e ritornando il controllo allo scheduler
	 * NOTA: Il processo terminato non sarà certamente nella readyQueue! */
	freePcb(currentProcess[prid]);
	LDST(&(scheduler_states[prid]));
}

/* System Call #4  : Verhogen
 * esegue la V sul semaforo con chiave semKey
 * il primo processo in coda sul semaforo va in esecuzione
 */
void verhogen(int semKey){
	lock(semKey);
	int cpuid = getPRID();
	semd_t *semaphore = getSemd(semKey);
	semaphore->s_value += 1;
	if (semaphore->s_value <= 0){
		pcb_t *toWake = removeBlocked(semKey);
		//debug(108,toWake);
		/* Controllo se il processo da inserire non sia da terminare,
		 * in tal caso potrebbe portare a problemi poiché ci potrebbe
		 * essere un processo marcato ma non rimosso (ancora) 
		 * dalla coda e se questo dovesse ancora dover effettuare la V si
		 * bloccherebbe l'accesso alla Critical Section! */
		while (toWake != NULL && toWake->wanted){
			semaphore->s_value += 1;
			freePcb(toWake);
			toWake = removeBlocked(semKey);
		}		
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
	state_t *oldProcess = (cpuid == 0)? (state_t *)SYSBK_OLDAREA : &areas[cpuid][SYSBK_OLDAREA_INDEX];
	semd_t *semaphore = getSemd(semKey);
	semaphore->s_value -= 1;
	if (semaphore->s_value < 0){
		copyState(oldProcess, &(currentProcess[cpuid]->p_s));
		insertBlocked(semKey, currentProcess[cpuid]);
		free(semKey);
		//debug(138,138);	
		LDST(&(scheduler_states[cpuid]));
	} else {
		free(semKey);
	}
}

/* System Call #6  : Get CPU Time
 * restituisce il tempo CPU usato dal processo in millisecondi
 * -> IL KERNEL DEVE TENERE LA CONTABILITA DEL TEMPO CPU DEI PROCESSI
 */
void get_cpu_time()
{
	U32 cpuid = getPRID();
	/* ottengo il processo corrente */ 
	currentProcess[cpuid]->p_s.reg_v0 = currentProcess[cpuid]->time;
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
	U32 cpuid = getPRID();
	state_t *OLDAREA = (cpuid == 0)? (state_t *)SYSBK_OLDAREA : &areas[cpuid][SYSBK_OLDAREA_INDEX];
	int statusNum = GET_TERM_STATUS(intline, dnum, read);
	int semKey = GET_TERM_SEM(intline,dnum,read);
	if (devStatus[statusNum] != 0){
		debug(11,11);
		OLDAREA->reg_v0 = devStatus[statusNum];
		copyState(OLDAREA, &(currentProcess[cpuid]->p_s));
		devStatus[statusNum] = 0; /* Altrimenti status condivisi! */
		LDST(&(currentProcess[cpuid]->p_s));
	} else {
		debug(22,22);
		lock(semKey);
		semd_t *semaphore = getSemd(semKey);
		semaphore->s_value -= 1;
		copyState(OLDAREA, &(currentProcess[cpuid]->p_s));
		insertBlocked(semKey, currentProcess[cpuid]);
		debug(888888,888888);
		free(semKey);
		LDST(&(scheduler_states[cpuid]));
	}

	//while(devStatus[statusNum] == 0)
		//debug(33333,33333);
	//debug(11, devStatus[statusNum]);
	//OLDAREA->reg_v0 = devStatus[statusNum];
	//copyState(OLDAREA, &(currentProcess[cpuid]->p_s));
	//debug(66666, (currentProcess[cpuid]->p_s).reg_v0);
	//devStatus[statusNum] = 0;

}

/* System Call #9  : Specify PRG State Vector
 * definire handler personalizzato per Program Trap per il processo corrente
 * oldp = indirizzo della custom OLDAREA
 * newp = indirizzo della custom NEWAREA
 */
void specify_prg_state_vector(state_t *oldp, state_t *newp)
{
	U32 prid = getPRID();
	/* copio i custom handlers nel pcb_t del processo chiamante*/
	currentProcess[prid]->custom_handlers[PGMTRAP_NEWAREA_INDEX] = newp;
	currentProcess[prid]->custom_handlers[PGMTRAP_OLDAREA_INDEX] = oldp;
}

/* System Call #10 : Specify TLB State Vector
 * definire handler personalizzato per TLB Exception per il processo corrente
 * oldp = indirizzo della custom OLDAREA
 * newp = indirizzo della custom NEWAREA
 */
void specify_tlb_state_vector(state_t *oldp, state_t *newp)
{
	U32 prid = getPRID();
	/* copio i custom handlers nel pcb_t del processo chiamante*/
	currentProcess[prid]->custom_handlers[TLB_NEWAREA_INDEX] = newp;
	currentProcess[prid]->custom_handlers[TLB_OLDAREA_INDEX] = oldp;
}

/* System Call #11 : Specify SYS State Vector
 * definire handler personalizzato per SYSCALL/BreakPoint per il processo corrente
 * oldp = indirizzo della custom OLDAREA
 * newp = indirizzo della custom NEWAREA
 */
void specify_sys_state_vector(state_t *oldp, state_t *newp)
{
	U32 prid = getPRID();
	/* copio i custom handlers nel pcb_t del processo chiamante*/
	currentProcess[prid]->custom_handlers[SYSBK_NEWAREA_INDEX] = newp;
	currentProcess[prid]->custom_handlers[SYSBK_OLDAREA_INDEX] = oldp;
}
