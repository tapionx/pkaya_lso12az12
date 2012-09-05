/*************************** Funzioni e valori di utilita' **********************************/

#include "const.h"
#include "uMPStypes.h"
#include "listx.h"
#include "types11.h"

/* Custom */
#include "handlers.h"
#include "myconst.h"
#include "utils.h"


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

/* This function is a debugging function. Through the emulator you can
 * see the value of both the parameters.
 * line : the line of the source where you're debugging
 * var : the value of the variable you want to check
 * REMEMBER TO ADD A BREAKPOINT FOR THIS FUNCTION */
void debug(int row, int var){
	return;
}

/* Wrapper per la printn (più comodo da trovare quando si deve pulire
 * il codice dalla roba usata per il debug/testing */
void debugn(char *str, int value){
	printn(str,value);
}
void debugs(char *str){ /* no value */
	addokbuf(str);
}

/* Funzione ausiliaria per la "dummy initialization" degli stati delle CPU */
void cleanState(state_t* state)     
{
	int i; /* Counter */
    state->entry_hi = 0;
    state->cause = 0;
    state->status = 0;
    state->pc_epc = 0;
    /* scorro tutti i registri */
    for (i = 0; i<29; i++)
        state->gpr[i] = 0;
    state->hi = 0;
    state->lo = 0;
}

/* Funzione che serve per copiare il primo state_t nel secondo */
void copyState(state_t *from, state_t *to){
	int i;
	to->entry_hi = from->entry_hi;
	to->cause = from->cause;
	to->pc_epc = from->pc_epc;
	for (i=0; i<29; i++){
		to->gpr[i] = from->gpr[i];
	}
	to->hi = from->hi;
	to->lo = from->lo;
}

/* Funzione ausiliaria per inizializzare un pcb_t con gli argomenti 
 * forniti (registri speciali, pc_epc ecc.) */
pcb_t *initPcbState(pcb_t *pcb, U32 status, memaddr pc_epc, U32 reg_sp){
	state_t *pcbs = &(pcb->p_s);
	pcbs->status = status;
	pcbs->pc_epc = pcbs->reg_t9 = pc_epc;
	pcbs->reg_sp = reg_sp;
	return pcb;
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
void printn(char* s, int n)
{
    /* carattere da rimpiazzare */
    char token = '%';
    int i = 0;
    /* inizializzo la stringa da stampare ogni volta */
    char c[2];
    c[1] = '\0';
    /* scorro tutta la stringa da stampare*/
    while(s[i] != '\0')
    {
        /* se non trovo il token */
        if(s[i] != token)
        {
            /* stampo una stringa con solo quel carattere */
            c[0] = (int)s[i];
            addokbuf(c);
        }
        /* se trovo il token */
        else
        {
            /* stampo l'intero */
            addokbuf(itoa(n));
        }
        i++;
    }
}

/* Trova l'id della CPU con minor workload in un array di procs counter */
int minWorkloadCpu(int procs[], int size){
	int i;
	int min = procs[0];
	int id = 0;
	for (i=1; i<size; i++){
		if (procs[i] < min){
			min = procs[i];
			id = i;
		}
	}
	return id;
}

/* Elevamento a potenza tra interi */
int pow(int b, int e){
	int i, res = b;
	if (e < 0 || b <= 0) return 0; /* error, no floating point support */
	for (i=1; i<e; i++){
		res *= b;
	}
	
	return res;
}

/* Acquisizione di un lock passato come parametro */
void lock(int semKey)
{
	extern int locks[MAXPROC];
	while(!CAS(&locks[semKey],PASS,FORBID));
}

/* Liberazione di un lock passato come parametro */
void free(int semKey)
{
	extern int locks[MAXPROC];
	CAS(&locks[semKey],FORBID,PASS);
}
