/*************************** Funzioni e valori di utilita' **********************************/

#include "const.h"
#include "uMPStypes.h"
#include "listx.h"
#include "types11.h"

/* Custom */
#include "handlers.h"
#include "types.h"
#include "myconst.h"


#define	MAXSEM	MAXPROC
#define MAX_PCB_PRIORITY		10
#define MIN_PCB_PRIORITY		0
#define DEFAULT_PCB_PRIORITY		5


#define TRANSMITTED	5
#define TRANSTATUS    2
#define ACK	1
#define PRINTCHR	2
#define CHAROFFSET	8
#define STATUSMASK	0xFF
#define	TERM0ADDR	0x10000250
#define DEVREGSIZE 16       
#define READY     1
#define DEVREGLEN   4
#define TRANCOMMAND   3
#define BUSY      3

/* Struttura di default delle new area (PC va cambiato a seconda del tipo!) */
HIDDEN state_t default_state;

/* This function is a debugging function. Through the emulator you can
 * see the value of both the parameters.
 * line : the line of the source where you're debugging
 * var : the value of the variable you want to check
 * REMEMBER TO ADD A BREAKPOINT FOR THIS FUNCTION */
void debug(int row, int var){
	return;
}

/* Funzione che, dato l'id di una CPU, provvede a inizializzarne i registri come da specifica */
void initCPU(int id, state_t *init, CPU cpus[])
{
	debug(39, id);
	CPU *cur = &(cpus[id]);
	cur->id = id;
	/* common init */
	init->reg_sp = RAMTOP;
	init->status = STATUS_TE|STATUS_IEc|STATUS_INT_UNMASKED;
	init->status = init->status & ~STATUS_VMc & ~STATUS_KUc;
	/* specific init */
	init->pc_epc = (memaddr)sysbp_handle;
	INITCPU(id, init, &(cur->new.SysBp));
	init->pc_epc = (memaddr)trap_handle;
	INITCPU(id, init, &(cur->new.Trap));
	init->pc_epc = (memaddr)tlb_handle;
	INITCPU(id, init, &(cur->new.Tlb));
	init->pc_epc = (memaddr)ints_handle;
	INITCPU(id, init, &(cur->new.Ints));
} 


/* Funzione che, dato il numero di CPU installate, provvede ad inizializzarle con gli handler corretti */
void initCPUs(state_t *init, CPU cpus[]){
	int i;
	for (i=1; i<NUM_CPU; i++){
		initCPU(i, init, cpus);
	}
}
/* Funzione ausiliaria per la "dummy initialization" degli stati delle CPU */
void initState_t(state_t* newState)     
{
	int i; /* Counter */
    newState->entry_hi = 0;
    newState->cause = 0;
    newState->status = 0;
    newState->pc_epc = 0;
    /* scorro tutti i registri */
    for (i = 0; i<29; i++)
        newState->gpr[i] = 0;
    newState->hi = 0;
    newState->lo = 0;
}

/******************************************************************************
 * I/O Routines to write on a terminal
 ******************************************************************************/

char okbuf[2048];			/* sequence of progress messages */
char errbuf[128];			/* contains reason for failing */
char msgbuf[128];			/* nonrecoverable error message before shut down */

char *mp = okbuf;

typedef unsigned int devreg;

/* This function returns the terminal transmitter status value given its address */ 
devreg termstat(memaddr *stataddr) {
	return((*stataddr) & STATUSMASK);
}

/* This function prints a string on specified terminal and returns TRUE if 
 * print was successful, FALSE if not   */
unsigned int termprint(char * str, unsigned int term) {

	memaddr *statusp;
	memaddr *commandp;
	
	devreg stat;
	devreg cmd;
	
	unsigned int error = FALSE;
	
	if (term < DEV_PER_INT) {
		/* terminal is correct */
		/* compute device register field addresses */
		statusp = (devreg *) (TERM0ADDR + (term * DEVREGSIZE) + (TRANSTATUS * DEVREGLEN));
		commandp = (devreg *) (TERM0ADDR + (term * DEVREGSIZE) + (TRANCOMMAND * DEVREGLEN));
		
		/* test device status */
		stat = termstat(statusp);
		if ((stat == READY) || (stat == TRANSMITTED)) {
			/* device is available */
			
			/* print cycle */
			while ((*str != '\0') && (!error)) {
				cmd = (*str << CHAROFFSET) | PRINTCHR;
				*commandp = cmd;

				/* busy waiting */
				while ((stat = termstat(statusp)) == BUSY);
				
				/* end of wait */
				if (stat != TRANSMITTED) {
					error = TRUE;
				} else {
					/* move to next char */
					str++;
				}
			}
		}	else {
			/* device is not available */
			error = TRUE;
		}
	}	else {
		/* wrong terminal device number */
		error = TRUE;
	}

	return (!error);		
}


/* This function places the specified character string in okbuf and
 *	causes the string to be written out to terminal0 */
void addokbuf(char *strp) {

	termprint(strp, 0);
}


/* This function places the specified character string in errbuf and
 *	causes the string to be written out to terminal0.  After this is done
 *	the system shuts down with a panic message */
void adderrbuf(char *strp) {

	termprint(strp, 0);
		
	PANIC();
}


/* SMART PRINTING INT UTILITY */
/* http://opensource.apple.com/source/groff/groff-10/groff/libgroff/itoa.c */

#define INT_DIGITS 19       /* enough for 64 bit integer */

/* Converts Int to String */
char *itoa(int i)
{
  /* Room for INT_DIGITS digits, - and '\0' */
  static char buf[INT_DIGITS + 2];
  char *p = buf + INT_DIGITS + 1;   /* points to terminating '\0' */
  if (i >= 0) {
    do {
      *--p = '0' + (i % 10);
      i /= 10;
    } while (i != 0);
    return p;
  }
  else {            /* i < 0 */
    do {
      *--p = '0' - (i % 10);
      i /= 10;
    } while (i != 0);
    *--p = '-';
  }
  return p;
}

/* Printf like function, replace % with int */
void print(char* s, int n)
{
    char token = '%';
    int i = 0;
    char* c[2];
    c[1] = '\0';
    while(s[i] != '\0')
    {
        if(s[i] != token)
        {
            c[0] = (int)s[i];
            addokbuf(c);
        }
        else
        {
            addokbuf(itoa(n));
        }
        i++;
    }
}


