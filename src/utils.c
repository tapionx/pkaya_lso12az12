/*************************** Funzioni e valori di utilita' **********************************/

#include "const.h"
#include "uMPStypes.h"
#include "listx.h"
#include "types11.h"
#include "asl.e"
#include "pcb.e"

/* Custom */
#include "handlers.h"
#include "myConst.h"
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

extern int locks[NUM_SEMAPHORES];


/* This function is a debugging function. Through the emulator you can
 * see the value of both the parameters.
 * line : the line of the source where you're debugging
 * var : the value of the variable you want to check
 * REMEMBER TO ADD A BREAKPOINT FOR THIS FUNCTION */
void debug(int row, int var){
	return;
}

/* Questa funzione stampa il contenuto di una coda data la chiave di un semaforo */
void stampaCoda(int semKey){
	semd_t* pSem = getSemd(semKey);
	struct list_head* qHead =  &(pSem->s_procQ);
	struct list_head* cur = qHead;
	addokbuf("[");
	list_for_each(cur, qHead){
		pcb_t* curPcb = container_of(cur, pcb_t, p_next);
		printn("%  ", (U32)curPcb);
	}
	addokbuf("]\n");
}

/* Questa funzione stampa il contenuto di una coda data la testa di essa */
void stampaCodaHead(struct list_head *head){
	struct list_head* cur = head;
	addokbuf("[  ");
	list_for_each(cur, head){
		pcb_t* curPcb = container_of(cur, pcb_t, p_next);
		printn("%  ", (U32)curPcb);
	}
	addokbuf("]\n");
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
	/* Brutto ma molto più efficente! */
	to->entry_hi = from->entry_hi;
	to->cause = from->cause;
	to->status = from->status;
	to->pc_epc = from->pc_epc;
	to->gpr[0] = from->gpr[0];
	to->gpr[1] = from->gpr[1];
	to->gpr[2] = from->gpr[2];
	to->gpr[3] = from->gpr[3];
	to->gpr[4] = from->gpr[4];
	to->gpr[5] = from->gpr[5];
	to->gpr[6] = from->gpr[6];
	to->gpr[7] = from->gpr[7];
	to->gpr[8] = from->gpr[8];
	to->gpr[9] = from->gpr[9];
	to->gpr[10] = from->gpr[10];
	to->gpr[11] = from->gpr[11];
	to->gpr[12] = from->gpr[12];
	to->gpr[13] = from->gpr[13];
	to->gpr[14] = from->gpr[14];
	to->gpr[15] = from->gpr[15];
	to->gpr[16] = from->gpr[16];
	to->gpr[17] = from->gpr[17];
	to->gpr[18] = from->gpr[18];
	to->gpr[19] = from->gpr[19];
	to->gpr[20] = from->gpr[20];
	to->gpr[21] = from->gpr[21];
	to->gpr[22] = from->gpr[22];
	to->gpr[23] = from->gpr[23];
	to->gpr[24] = from->gpr[24];
	to->gpr[25] = from->gpr[25];
	to->gpr[26] = from->gpr[26];
	to->gpr[27] = from->gpr[27];
	to->gpr[28] = from->gpr[28];
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

void printn_mutex(char* s, int n){
	lock(5);
	printn(s, n);
	free(5);
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
void lock(int key)
{
	while(!CAS(&locks[key],PASS,FORBID));
}

/* Liberazione di un lock passato come parametro */
void free(int key)
{
	CAS(&locks[key],FORBID,PASS);
}

/* Restituisci l'indice del device con interrupt pending 
 * (il bit acceso meno significativo nella interrupt pending bitmap) */
int getDevNo(U32 bitmap){
	int i=0;
	for(i=0; i<WORD_SIZE; i++){
		if (IS_NTH_BIT_SET(i, bitmap)) return i;
	}
}
