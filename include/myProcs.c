#ifndef MYPROCS
#define MYPROCS


#include "kernelVariables.h"
#include "utils.h"
#include "libumps.h"

/* Qui definisco alcuni processi/funzioni di prova per testare
 * le modifiche individualmente */

void test2(){		
	SYSCALL(PASSEREN, 6, 0, 0);
	print("funziona cazzo!\n");
	SYSCALL(VERHOGEN, 6, 0, 0);
}

void test1(){
	SYSCALL(PASSEREN, 6, 0, 0);
	print("funziona cazzo!\n");
	SYSCALL(VERHOGEN, 6, 0, 0);
}

void test3(){
	SYSCALL(PASSEREN, 6, 0, 0);
	print("funziona cazzo!\n");
	SYSCALL(VERHOGEN, 6, 0, 0);
}

void test4(){
	SYSCALL(PASSEREN, 6, 0, 0);
	print("funziona cazzo!\n");
	SYSCALL(VERHOGEN, 6, 0, 0);
}


#endif
