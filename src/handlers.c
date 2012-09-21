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
#include "asl.e"

/* ATTENZIONE! USARE UN VALORE DI RITORNO PER LE SYSCALL E NON COPIARE DIRETTAMENTE NEL V0! */

void sysbk_handler(){
	/* recupero il numero della CPU attuale */
	U32 cpuid = getPRID();
	/* recupero il processo chiamante */
	state_t *oldProcess = GET_OLD_SYSBK();
	/* incremento il PC del processo chiamante, per evitare loop */
	/* in questo caso non serve aggiornare anche t9 */
	/* (pag 28, 3.7.2 Student Guide) */
	oldProcess->pc_epc += WORD_SIZE; /* 4 */
	/* Aggiorno il pcb corrente */
	//copyState(oldProcess, &(currentProcess[cpuid]->p_s));
	/* recupero i parametri della SYSCALL dalla oldProcess */
	U32 *num_syscall = &(oldProcess->reg_a0);
	U32 *arg1 		 =  &(oldProcess->reg_a1);
	U32 *arg2		 =  &(oldProcess->reg_a2);
	U32 *arg3		 =  &(oldProcess->reg_a3);
	U32 result;	/* Risultato della system call (ove disponibile) */
	
	/* recupero lo stato (kernel-mode o user-mode) */
	U32 *old_status = &(oldProcess->status);

	/* recupero la causa (tipo di eccezione sollevato) */
	U32 *old_cause = &(oldProcess->cause);

	/* carico il processo corrente */
	pcb_t *processoCorrente = currentProcess[cpuid];
	
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
					result = create_process((state_t*) *arg1, (int) *arg2);
					break;
				case CREATEBROTHER:
					/* int create_brother(state_t *statep, int priority) */
					result = create_brother((state_t*) *arg1, (int) *arg2);
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
					result = get_cpu_time();
					break;
				case WAITCLOCK:
					/* void wait_clock() */
					wait_clock();
					break;
				case WAITIO:
					/* int wait_io(int intNo, int dnum, int waitForTermRead) */
					result = wait_io((int) *arg1, (int) *arg2, (int) *arg3);
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
			/* Inserisco il risultato in uscita nel registro v0 */
			oldProcess->reg_v0 = result;
			/* Aggiorno lo state_t del processo prima di rimetterlo in coda */
			copyState(oldProcess, &(currentProcess[cpuid]->p_s));
			addReady(currentProcess[cpuid]);
			/* Richiamo lo scheduler */
			scheduler();
		}
		/* se il processo ha un custom handler lo chiamo */
		else
		{
			/* copio il processo chiamante nella OLD Area custom */
			copyState(oldProcess, processoCorrente->custom_handlers[SYSBK_OLDAREA_INDEX]);
			/* chiamo l'handler custom */
			LDST(processoCorrente->custom_handlers[SYSBK_NEWAREA_INDEX]);
		}
	}
	/* se e' stata chiamata la SYSTEM CALL in User Mode lancio TRAP */
	else
	{
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

void int_handler(){
	U32 cpuid = getPRID();
	/* estraggo il puntatore allo state_t del processo interrotto 
	 * (non è necessariamente quello che ha sollevato l'interrupt!) */
	state_t *oldProcess = GET_OLD_INT();
	
	/* Capiamo da che linea proviene l'interrupt */
	int line = 0;
	debug(146, CAUSE_IP_GET(0, 7));
	for (line; line < NUM_LINES; line++){
		/* Se abbiamo trovato la linea usciamo */
		if (CAUSE_IP_GET(getCAUSE(), line)){
			break;
		}
	}
	/* Calcolo il numero del device che ha generato l'interrupt */
	int devNo = getDevNo(GET_PENDING_INT_BITMAP(line));
	/* Calcoliamo l'inizio del registro del controller per scrivere/leggere sui suoi registri */
	int devAddrBase = DEV_ADDR_BASE(line, devNo);
	
	switch(line){
		case INT_PLT: {
			/* Non c'è bisogno di mutua esclusione esplicita dato che la addReady già la include! */
			addReady(currentProcess[cpuid]);
			/* ACK del PLT */
			setTIMER(TIME_SLICE);
			/* Richiamo lo scheduler */
			scheduler();
			break;
		}
		
		case INT_TIMER: {
			/* Facciamo la V "speciale" che risveglia tutti i processi bloccati */
			/* estraggo il puntatore al semaforo */
			semd_t *pctsem = getSemd(PCT_SEM);
			/* faccio la V finchè non escono tutti i processi */
			while(pctsem->s_value != 0){
				verhogen(PCT_SEM);
			}
			/* setto di nuovo il PCT a 100ms */
			SET_IT(SCHED_PSEUDO_CLOCK);
			/* Rimetto il processo interrotto in ready */
			copyState(oldProcess, &(currentProcess[cpuid]->p_s));
			addReady(currentProcess[cpuid]);
			/* Richiamo lo scheduler */
			scheduler();
			break;
		}
		
		case INT_TERMINAL: {
			/* TODO: Bisogna stare attenti perché quando si chiama la 
			 * funzione associata alla syscall (non SYSCALL!) non viene
			 * aggiornato il currentProcess! Quindi quando lo si rimette in
			 * readyQueue da li semplicemente va in loop se aveva fatto una P
			 * (senza V, magari nell'esecuzione principale). Bisogna
			 * aggiornare il currentProcess in ogni syscall dove si usa
			 * currentProcess! */
			termreg_t *fields = (termreg_t *)devAddrBase;
			int termSemNo = GET_TERM_SEM(line, devNo, FALSE);
			int termStatusNo = GET_TERM_STATUS(line, devNo, FALSE);
			lock(termSemNo);
			semd_t *termSem = getSemd(termSemNo);
			pcb_t *waitingProc = headBlocked(termSemNo);
			if (waitingProc != NULL){
				/* Maggior priorità alla trasmissione */
				waitingProc->p_s.reg_v0 = fields->transm_status;
			} else {
				devStatus[termStatusNo] = fields->transm_status;
			}
			free(termSemNo);
			verhogen(termSemNo);
			fields->transm_command = DEV_C_ACK;
			state_t *prova = oldProcess;
			copyState(oldProcess, &(currentProcess[cpuid]->p_s));
			addReady(currentProcess[cpuid]);
			scheduler();
			break;
		}
		
		default: {
			PANIC();
		}
	}
}






void pgmtrap_handler(){

	/* se il processo ha dichiarato un handler per Program Trap
	 * lo eseguo, altrimenti termino il processo e tutta la progenie
	 */
	 
	U32 cpuid = getPRID();
	/* processo chiamante */
	state_t *oldProcess = GET_OLD_PGMTRAP();
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

void tlb_handler(){
	
	/* se il processo ha dichiarato un handler per TLB Exeption
	 * lo eseguo, altrimenti killo il processo e tutta la progenie
	 */
	 
	U32 cpuid = getPRID();
	/* processo chiamante */
	state_t *oldProcess = GET_OLD_TLB();
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
