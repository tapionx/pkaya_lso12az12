#ifndef MYPROCS
#define MYPROCS


#include "kernelVariables.h"
#include "utils.h"
#include "libumps.h"

/* Qui definisco alcuni processi/funzioni di prova per testare
 * le modifiche individualmente */


void test1(){
	int TERM_MUT = 0;
	SYSCALL(VERHOGEN, TERM_MUT, 0, 0);
	while(TRUE){	
		print("\n\nATTENZIONE ATTENZIONE ATTENZIONE!!\n");
	}
}

void test2(){
	while(TRUE){	
		print("\n\nCACCA CACCA CACCA!!\n");
	}
}

void test3(){
	while(TRUE){	
		SYSCALL(PASSEREN, 6, 0, 0);
		addokbuf("\n\nHAI CAPITO???!\n");
		SYSCALL(VERHOGEN, 6, 0, 0);
	}
}

void test4(){
	while(TRUE){	
		SYSCALL(PASSEREN, 6, 0, 0);
		addokbuf("\n\nMARCO PARLO CON TE...E CON VOI CHE NON VE NE SIETE ACCORTI!! XD\n");
		SYSCALL(VERHOGEN, 6, 0, 0);
	}
}


#endif
