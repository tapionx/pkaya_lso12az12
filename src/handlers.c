#include "myconst.h"
#include "base.h"

/*TODO: per ora dummy handlers */

/* Handler per le System Call */
/* Invocate da extern unsigned int SYSCALL(number, arg1, arg2, arg3); */
void sysbp_handler()
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
}

void trap_handler()
{

}

void tlb_handler()
{

}

void ints_handler()
{

}
