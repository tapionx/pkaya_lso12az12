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
#include "interrupt.h"
#include "asl.e"



/** Handler per gli interrupt */
void int_handler(){
	U32 cpuid = getPRID();
	/* estraggo il puntatore allo state_t del processo interrotto 
	 * (non è necessariamente quello che ha sollevato l'interrupt!) */
	state_t *oldProcess = (getPRID() == 0)? (state_t *)INT_OLDAREA : &areas[getPRID()][INT_OLDAREA_INDEX];
	
	/* Capiamo da che linea proviene l'interrupt */
	int line = 0;
	for (line; line < NUM_LINES; line++){
		/* Se abbiamo trovato la linea interrompiamo la ricerca */
		if (CAUSE_IP_GET(getCAUSE(), line)){
			break;
		}
	}
	
	switch(line){
		case INT_PLT: {
			pltHandler(cpuid);
			break;
		}
		
		case INT_TIMER: {
			debug(34,34);
			pctHandler(cpuid);
			break;
		}

		case INT_TERMINAL: {
			terminalHandler(cpuid);
			break;
		}
		
		default: {
			/* Ulteriori device non sono supportati o la linea è errata */
			PANIC();
		}
	}
}



/** Handler per i pgmtrap */
void pgmtrap_handler(){
	/* se il processo ha dichiarato un handler per Program Trap
	 * lo eseguo, altrimenti termino il processo e tutta la progenie
	 */
	 
	U32 cpuid = getPRID();
	/* processo chiamante */
	state_t *oldProcess = (getPRID() == 0)? (state_t *)PGMTRAP_OLDAREA : &areas[getPRID()][PGMTRAP_OLDAREA_INDEX];
	/* carico il processo corrente */
	pcb_t *processoCorrente = currentProcess[cpuid];
	/* controllo se il processo ha un handler custom */
	if(processoCorrente->custom_handlers[PGMTRAP_NEWAREA_INDEX] != NULL)
	{ 
		/* copio il processo chiamante nella OLD Area custom */
		copyState(oldProcess, processoCorrente->custom_handlers[PGMTRAP_OLDAREA_INDEX]);
		/* chiamo l'handler custom */
		LDST(processoCorrente->custom_handlers[PGMTRAP_NEWAREA_INDEX]);
	}
	/* altrimenti elimino il processo e tutti i figli */
	else
	{
		terminate_process();
	}	
}



/** Handler per la tlb */
void tlb_handler(){
	/* se il processo ha dichiarato un handler per TLB Exeption
	 * lo eseguo, altrimenti killo il processo e tutta la progenie
	 */
	 
	U32 cpuid = getPRID();
	/* processo chiamante */
	state_t *oldProcess = (getPRID() == 0)? (state_t *)TLB_OLDAREA : &areas[getPRID()][TLB_OLDAREA_INDEX];
	/* carico il processo corrente */
	pcb_t *processoCorrente = currentProcess[cpuid];
	/* controllo se il processo ha un handler custom */
	if(processoCorrente->custom_handlers[TLB_NEWAREA_INDEX] != NULL)
	{ 
		/* copio il processo chiamante nella OLD Area custom */
		copyState(oldProcess, processoCorrente->custom_handlers[TLB_OLDAREA_INDEX]);
		/* chiamo l'handler custom */
		LDST(processoCorrente->custom_handlers[TLB_NEWAREA_INDEX]);
	}
	/* altrimenti elimino il processo e tutti i figli */
	else
	{
		terminate_process();
	}	
}



/** Handler per le syscall */
void sysbk_handler(){
	/* recupero il numero della CPU attuale */
	U32 cpuid = getPRID();
	/* recupero il processo chiamante */
	state_t *oldProcess = (getPRID() == 0)? (state_t *)SYSBK_OLDAREA : &areas[getPRID()][SYSBK_OLDAREA_INDEX];
	/* Incremento il PC del processo chiamante, per evitare loop,
	 * e in questo caso non serve aggiornare anche t9!
	 * (pag 28, 3.7.2 Student Guide) */
	oldProcess->pc_epc += WORD_SIZE;

	/* recupero i parametri della SYSCALL dalla oldProcess */
	U32 *num_syscall = &(oldProcess->reg_a0);
	U32 *arg1 		 =  &(oldProcess->reg_a1);
	U32 *arg2		 =  &(oldProcess->reg_a2);
	U32 *arg3		 =  &(oldProcess->reg_a3);
	
	/* recupero lo stato (kernel-mode o user-mode) */
	U32 *old_status = &(oldProcess->status);

	/* recupero la causa (tipo di eccezione sollevato) */
	U32 *old_cause = &(oldProcess->cause);
	
	/* se il processo era in kernel mode */
	if( (*old_status & STATUS_KUp) == 0 )
	{
		/* controllo se il processo non ha un handler custom */
		if(currentProcess[cpuid]->custom_handlers[SYSBK_NEWAREA_INDEX] == NULL)
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
			}
		}
		/* se il processo ha un custom handler lo chiamo */
		else
		{
			debug(666,1);
			/* copio il processo chiamante nella OLD Area custom */
			copyState(oldProcess, currentProcess[cpuid]->custom_handlers[SYSBK_OLDAREA_INDEX]);
			/* chiamo l'handler custom */
			LDST(currentProcess[cpuid]->custom_handlers[SYSBK_NEWAREA_INDEX]);
		}
	}
	/* se e' stata chiamata la SYSTEM CALL in User Mode lancio TRAP */
	else
	{
		debug(666,2);
		/* copiare SYSCALL OLD AREA -> PROGRAM TRAP OLD AREA */
		if (cpuid == 0){
			copyState((state_t *)SYSBK_OLDAREA, (state_t *)PGMTRAP_OLDAREA);
		} else {
			copyState(&areas[cpuid][SYSBK_OLDAREA_INDEX], &areas[cpuid][PGMTRAP_OLDAREA_INDEX]); 
		}
		/* settare Cause a 10 : Reserved Instruction Exception*/
		if (cpuid == 0){
			((state_t *)PGMTRAP_OLDAREA)->cause = EXC_RESERVEDINSTR;
		} else {
			areas[cpuid][PGMTRAP_OLDAREA_INDEX].cause = EXC_RESERVEDINSTR;
		}
		/* sollevare PgmTrap, se la sbriga lui */
		pgmtrap_handler();
	}
}
