#ifndef MYPROCS
#define MYPROCS


#include "kernelVariables.h"
#include "utils.h"
#include "libumps.h"

/* Qui definisco alcuni processi/funzioni di prova per testare
 * le modifiche individualmente */

void test2(){
	while(TRUE){	
		SYSCALL(PASSEREN, 6, 0, 0);
		print("funziona cazzo!\n");
		SYSCALL(VERHOGEN, 6, 0, 0);
	}
}

void test1(){
	while(TRUE){	
		SYSCALL(PASSEREN, 6, 0, 0);
		print("funziona cazzo!\n");
		SYSCALL(VERHOGEN, 6, 0, 0);
	}
}

void test3(){
	while(TRUE){	
		SYSCALL(PASSEREN, 6, 0, 0);
		print("funziona cazzo!\n");
		SYSCALL(VERHOGEN, 6, 0, 0);
	}
}

void test4(){
	while(TRUE){	
		SYSCALL(PASSEREN, 6, 0, 0);
		print("funziona cazzo!\n");
		SYSCALL(VERHOGEN, 6, 0, 0);
	}
}


#endif
