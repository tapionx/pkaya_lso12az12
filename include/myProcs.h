#ifndef MYPROCS
#define MYPROCS

#include "kernelVariables.h"
#include "utils.h"
#include "libumps.h"
#include "myConst.h"

/* Qui definisco alcuni processi/funzioni di prova per testare
 * le modifiche individualmente */
void exec(int num){
	
}

void test1(){
	SYSCALL(VERHOGEN, 6, 0, 0);
	while(TRUE){
		SYSCALL(WAITCLOCK, 0, 0, 0);
		SYSCALL(PASSEREN, 6, 0, 0);

		SYSCALL(VERHOGEN, 6, 0, 0);
	}
}

void test2(){
	while(TRUE){
		SYSCALL(WAITCLOCK, 0, 0, 0);
		SYSCALL(PASSEREN, 6, 0, 0);

		SYSCALL(VERHOGEN, 6, 0, 0);
	}
}

void test3(){
	while(TRUE){
		SYSCALL(WAITCLOCK, 0, 0, 0);
		SYSCALL(PASSEREN, 6, 0, 0);

		SYSCALL(VERHOGEN, 6, 0, 0);
	}
}

void test4(){
	while(TRUE){
		SYSCALL(WAITCLOCK, 0, 0, 0);
		SYSCALL(PASSEREN, 6, 0, 0);

		SYSCALL(VERHOGEN, 6, 0, 0);
	}
}


#endif
