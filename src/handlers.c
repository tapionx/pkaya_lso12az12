#include "myconst.h"
#include "base.h"

/*TODO: per ora dummy handlers */

/* Handler per le System Call */
/* Invocate da extern unsigned int SYSCALL(number, arg1, arg2, arg3); */
void sysbp_handle()
{ 
	/* recuperare il numero della SYSCALL invocata dal registro reg_a0 */
	U32 num_syscall;
	/* CPU->old->SysBp; */
	
	switch(num_syscall) 
	{
		case CREATEPROCESS:
			break;
		case CREATEBROTHER:
			break;
		case TERMINATEPROCESS:
			break;
		case VERHOGEN:
			break;
		case PASSEREN:
			break;
		case GETCPUTIME:
			break;
		case WAITCLOCK:
			break;
		case WAITIO:
			break;
		case SPECPRGVEC:
			break;
		case SPECTLBVEC:
			break;
		case SPECSYSVEC:
			break;
		default:
			/* SOLLEVARE ERRORE */
			break;
	}
	
	debug(1, 155);
	while(1);
}

void trap_handle()
{
	debug(2, 155);
	while(1);
}

void tlb_handle()
{
	debug(3, 155);
	while(1);
}

void ints_handle()
{
	debug(4, 155);
	while(1);
}
