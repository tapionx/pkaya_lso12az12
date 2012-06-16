#include "myconst.h"
#include "base.h"
#include "uMPStypes.h"
#include "handlers.h"
#include "utils.h"
#include "types11.h"
#include "scheduler.h"

/* Bisogna agire tramite puntatori altrimenti CPU0 rimane esclusa 
 * poiché le sue aree NON sono all'interno del vettore new_old_areas */
extern state_t* pnew_old_areas[NUM_CPU][NUM_AREAS];

/* Handler per le System Call e i Breakpoint */
/* Invocate da SYSCALL(number, arg1, arg2, arg3); */
void sysbp_handler()
{	 
	/* recupero il numero della CPU attuale */
	U32 prid = getPRID();
	/* recupero il processo chiamante */
	state_t *OLDAREA = pnew_old_areas[prid][OLD_SYSBP];
	/* incremento il PC del processo chiamante, per evitare loop */
	/* in questo caso non serve aggiornare anche t9 */
	/* (pag 28, 3.7.2 Student Guide) */
	OLDAREA->pc_epc += WORD_SIZE; /* 4 */
	/* recupero i parametri della SYSCALL dalla OLDAREA */
	U32 *num_syscall =  &(OLDAREA->reg_a0);
	U32 *arg1 		 =  &(OLDAREA->reg_a1);
	U32 *arg2		 =  &(OLDAREA->reg_a2);
	U32 *arg3		 =  &(OLDAREA->reg_a3);

	/* recupero lo stato (kernel-mode o user-mode) */
	U32 *old_status = &(OLDAREA->status);
	
	/* recupero la causa (tipo di eccezione sollevato) */
	U32 *old_cause = &(OLDAREA->cause);

	/* se il processo era in user mode */
	if( (*old_status & STATUS_KUc) != 0 )
	{
		/* gestisci user mode */
		/* copiare SYSCALL OLD AREA -> PROGRAM TRAP OLD AREA */
		copyState(pnew_old_areas[prid][OLD_SYSBP], pnew_old_areas[prid][OLD_TRAP]); 
		/* settare Cause a 10 : Reserved Instruction Exception*/
		pnew_old_areas[prid][OLD_TRAP]->cause = EXC_RESERVEDINSTR;
		/* sollevare PgmTrap, se la sbriga lui */
		trap_handler();
	}
	else
	{
		/* se è stata invocata una SYSCALL tradizionale */
		if( (*old_cause == EXC_SYSCALL) && (*num_syscall <= SPECSYSVEC) && (*num_syscall >= CREATEPROCESS ) )
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
					/* int get_cpu_time() FIXME deve restituire cpu_t, dove è dichiarato?  */
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
				default:
					/* syscall non riconosciuta */
					break;
			}
		}
		/* se il kernel non può gestire questa eccezione */
		else
		{
			/* carico il processo corrente */
			pcb_t *processoCorrente = get_currentproc(prid);
			/* controllo se il processo ha un handler custom */
			if(processoCorrente->custom_handlers[NEW_SYSBP] != NULL)
			{ 
				/* copio il processo chiamante nella OLD Area custom */
				copyState(OLDAREA, processoCorrente->custom_handlers[OLD_SYSBP]); 
				/* chiamo l'handler custom */
				
			}
			/* altrimenti elimino il processo e tutti i figli */
			else
			{
				terminate_process();
			}	
		}
	}
}

void trap_handler()
{
	
	/* se il processo ha dichiarato un handler per Program Trap
	 * lo eseguo, altrimenti killo il processo e tutta la progenie
	 */
	
	/* ottengo il processore corrente */
	U32 prid = getPRID();
	/* ottengo il processo chiamante */
	state_t *OLDAREA = pnew_old_areas[prid][OLD_TRAP];
	/* carico il processo corrente */
	pcb_t *processoCorrente = get_currentproc(prid);
	/* controllo se il processo ha un handler custom */
	if(processoCorrente->custom_handlers[NEW_TRAP] != NULL)
	{ 
		/* copio il processo chiamante nella OLD Area custom */
		copyState(OLDAREA, processoCorrente->custom_handlers[OLD_TRAP]); 
		/* chiamo l'handler custom */
		
	}
	/* altrimenti elimino il processo e tutti i figli */
	else
	{
		terminate_process();
	}	
}

void tlb_handler()
{
	/* se il processo ha dichiarato un handler per TLB Exeption
	 * lo eseguo, altrimenti killo il processo e tutta la progenie
	 */
	/* ottengo il processore corrente */
	U32 prid = getPRID();
	/* ottengo il processo chiamante */
	state_t *OLDAREA = pnew_old_areas[prid][OLD_TLB];
	/* carico il processo corrente */
	pcb_t *processoCorrente = get_currentproc(prid);
	/* controllo se il processo ha un handler custom */
	if(processoCorrente->custom_handlers[NEW_TLB] != NULL)
	{ 
		/* copio il processo chiamante nella OLD Area custom */
		copyState(OLDAREA, processoCorrente->custom_handlers[OLD_TLB]); 
		/* chiamo l'handler custom */
		
	}
	/* altrimenti elimino il processo e tutti i figli */
	else
	{
		terminate_process();
	}	
}

void ints_handler()
{
	debugs("INTERRUPT!\n");
	/* DEBUG SCHEDULER */ setTIMER(500000);
	/* Determina da quale device è arrivato l'interrupt */
}
