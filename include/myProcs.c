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
	SYSCALL(VERHOGEN, 6, 0, 0);
	while(TRUE){	
		SYSCALL(PASSEREN, 6, 0, 0);
		print("\n\nATTENZIONE ATTENZIONE ATTENZIONE!!\n");
		SYSCALL(VERHOGEN, 6, 0, 0);
	}
}

void test2(){
	while(TRUE){	
		SYSCALL(PASSEREN, 6, 0, 0);
		print("\n\nNON USARE UN PCB_T COME PARAMETRO DELLA COPYSTATE!!\n");
		SYSCALL(VERHOGEN, 6, 0, 0);
	}
}

void test3(){
	while(TRUE){	
		SYSCALL(PASSEREN, 6, 0, 0);
		print("\n\nHAI CAPITO???!\n");
		SYSCALL(VERHOGEN, 6, 0, 0);
	}
}

void test4(){
	while(TRUE){	
		SYSCALL(PASSEREN, 6, 0, 0);
		print("\n\nMARCO PARLO CON TE...E CON VOI CHE NON VE NE SIETE ACCORTI!! XD\n");
		SYSCALL(VERHOGEN, 6, 0, 0);
	}
}


#endif
