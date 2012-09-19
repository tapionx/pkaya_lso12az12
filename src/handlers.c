#include "libumps.h"
#include "myConst.h"
#include "const.h"
#include "uMPStypes.h"
#include "types11.h"
#include "handlers.h"
#include "scheduler.h"
#include "utils.h"
#include "kernelVariables.h"
#include "syscall.h"

void sysbk_handler(){
	/* recupero il numero della CPU attuale */
	U32 prid = getPRID();
	/* recupero il valore del timer al momento della chiamata */
	U32 oldTimer = getTIMER();
	/* recupero il processo chiamante */
	state_t *OLDAREA = (prid == 0)? (state_t *)SYSBK_OLDAREA : &areas[prid][SYSBK_OLDAREA_INDEX];
	/* incremento il PC del processo chiamante, per evitare loop */
	/* in questo caso non serve aggiornare anche t9 */
	/* (pag 28, 3.7.2 Student Guide) */
	OLDAREA->pc_epc += WORD_SIZE; /* 4 */
	/* Aggiorno il pcb corrente */
	copyState(OLDAREA, &(currentProcess[prid]->p_s));
	/* recupero i parametri della SYSCALL dalla OLDAREA */
	U32 *num_syscall = &(OLDAREA->reg_a0);
	U32 *arg1 		 =  &(OLDAREA->reg_a1);
	U32 *arg2		 =  &(OLDAREA->reg_a2);
	U32 *arg3		 =  &(OLDAREA->reg_a3);

	/* recupero lo stato (kernel-mode o user-mode) */
	U32 *old_status = &(OLDAREA->status);

	/* recupero la causa (tipo di eccezione sollevato) */
	U32 *old_cause = &(OLDAREA->cause);

	/* carico il processo corrente */
	pcb_t *processoCorrente = currentProcess[prid];
	
	/* se il processo era in kernel mode */
	if( (*old_status & STATUS_KUc) == 0 )
	{
		/* controllo se il processo non ha un handler custom */
		if(processoCorrente->custom_handlers[SYSBK_NEWAREA_INDEX] == NULL)
		{		
			/* eseguo la SYSCALL adeguata */
			switch(*num_syscall)
			{
				case CREATEPROCESS:
					/* int create_process(state_t *statep, int priority) */
					create_process((state_t*) *arg1, (int) *arg2);
					break;
				case CREATEBROTHER:
					/* int create_brother(state_t *statep, int priority) */
					create_brother((state_t*) *arg1, (int) *arg2);
					break;
				case TERMINATEPROCESS:
					/* void terminate_process() */
					terminate_process();
					break;
				case VERHOGEN:
					/* void verhogen(int semKey) */
					verhogen((int) *arg1);
					break;
				case PASSEREN:
					/* void passeren(int semKey) */
					passeren((int) *arg1);
					break;
				case GETCPUTIME:
					/* int get_cpu_time()  */
					get_cpu_time();
					break;
				case WAITCLOCK:
					/* void wait_clock() */
					wait_clock();
					break;
				case WAITIO:
					/* int wait_io(int intNo, int dnum, int waitForTermRead) */
					wait_io((int) *arg1, (int) *arg2, (int) *arg3);
					break;
				case SPECPRGVEC:
					/* void specify_prg_state_vector(state_t *oldp, state_t *newp) */
					specify_prg_state_vector((state_t*) *arg1, (state_t*) *arg2);
					break;
				case SPECTLBVEC:
					/* void specify_tlb_state_vector(state_t *oldp, state_t *newp) */
					specify_tlb_state_vector((state_t*) *arg1, (state_t*) *arg2);
					break;
				case SPECSYSVEC:
					/* void specify_sys_state_vector(state_t *oldp, state_t *newp) */
					specify_sys_state_vector((state_t*) *arg1, (state_t*) *arg2);
					break;
				/* Se e' stata chiamata una SYSCALL non esistente */
				default:
					/* killo il processo */
					terminate_process();
					break;
			} /*switch*/
			/* ritorno il controllo al processo chiamante */
			LDST(&(currentProcess[prid]->p_s));
		} /* if */
		/* se il processo ha un custom handler lo chiamo */
		else
		{
			debug(666, 666);
			/* copio il processo chiamante nella OLD Area custom */
			copyState(OLDAREA, processoCorrente->custom_handlers[SYSBK_OLDAREA_INDEX]);
			/* Ripristino il timer al momento della chiamata */
			setTIMER(oldTimer);
			/* chiamo l'handler custom */
			LDST(processoCorrente->custom_handlers[SYSBK_NEWAREA_INDEX]);
		}
	}
	/* se e' stata chiamata la SYSTEM CALL in User Mode lancio TRAP */
	else
	{
		debug(666,666);
		/* copiare SYSCALL OLD AREA -> PROGRAM TRAP OLD AREA */
		if (prid == 0){
			copyState((state_t *)SYSBK_OLDAREA, (state_t *)PGMTRAP_OLDAREA);
		} else {
			copyState(&areas[prid][SYSBK_OLDAREA_INDEX], &areas[prid][PGMTRAP_OLDAREA_INDEX]); 
		}
		/* settare Cause a 10 : Reserved Instruction Exception*/
		if (prid == 0){
			((state_t *)PGMTRAP_OLDAREA)->cause = EXC_RESERVEDINSTR;
		} else {
			areas[prid][PGMTRAP_OLDAREA_INDEX].cause = EXC_RESERVEDINSTR;
		}
		/* sollevare PgmTrap, se la sbriga lui */
		pgmtrap_handler();
	}
}

void int_handler(){
	U32 cpuid = getPRID();
	/* Capiamo da che linea proviene l'interrupt */
	int line = 0;
	for (line; line < NUM_LINES; line++){
		/* Se abbiamo trovato la linea usciamo */
		if (CAUSE_IP_GET(getCAUSE(), line)){
			break;
		}
	}
	
	switch(line){
		case INT_PLT:
			if (cpuid == 0){
				copyState((state_t *)INT_OLDAREA, &(currentProcess[cpuid]->p_s));
			} else {
				copyState(&areas[cpuid][INT_OLDAREA_INDEX], &(currentProcess[cpuid]->p_s));
			}
			/* Non c'è bisogno di mutua esclusione esplicita dato che la addReady già la include! */
			addReady(currentProcess[cpuid]);
			LDST(&(scheduler_states[cpuid]));
			break;
		
		case INT_TIMER:
			/* Facciamo la V "speciale" che risveglia tutti i processi bloccati */
			/* TODO FARE IL WHILE PER FARE V CHE SVEGLIA TUTTO! */
			
			
			SET_IT(100000);
			break;
	}
	
}
void pgmtrap_handler(){

	// TEMPORANEO
	PANIC();

	/* se il processo ha dichiarato un handler per Program Trap
	 * lo eseguo, altrimenti termino il processo e tutta la progenie
	 */
	 
	U32 prid = getPRID();
	/* processo chiamante */
	state_t *OLDAREA = (prid == 0)? (state_t *)PGMTRAP_OLDAREA : &areas[prid][PGMTRAP_OLDAREA_INDEX];
	/* carico il processo corrente */
	pcb_t *processoCorrente = currentProcess[prid];
	/* controllo se il processo ha un handler custom */
	if(processoCorrente->custom_handlers[PGMTRAP_NEWAREA_INDEX] != NULL)
	{ 
		/* copio il processo chiamante nella OLD Area custom */
		copyState(OLDAREA, processoCorrente->custom_handlers[PGMTRAP_OLDAREA_INDEX]);
		/* chiamo l'handler custom */
		LDST(processoCorrente->custom_handlers[PGMTRAP_NEWAREA_INDEX]);
	}
	/* altrimenti elimino il processo e tutti i figli */
	else
	{
		terminate_process();
	}	
}

void tlb_handler(){
	
	/* se il processo ha dichiarato un handler per TLB Exeption
	 * lo eseguo, altrimenti killo il processo e tutta la progenie
	 */
	 
	U32 prid = getPRID();
	/* processo chiamante */
	state_t *OLDAREA = (prid == 0)? (state_t *)TLB_OLDAREA : &areas[prid][TLB_OLDAREA_INDEX];
	/* carico il processo corrente */
	pcb_t *processoCorrente = currentProcess[prid];
	/* controllo se il processo ha un handler custom */
	if(processoCorrente->custom_handlers[TLB_NEWAREA_INDEX] != NULL)
	{ 
		/* copio il processo chiamante nella OLD Area custom */
		copyState(OLDAREA, processoCorrente->custom_handlers[TLB_OLDAREA_INDEX]);
		/* chiamo l'handler custom */
		LDST(processoCorrente->custom_handlers[TLB_NEWAREA_INDEX]);
	}
	/* altrimenti elimino il processo e tutti i figli */
	else
	{
		terminate_process();
	}	
}
