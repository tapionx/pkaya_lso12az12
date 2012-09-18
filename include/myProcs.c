#include "kernelVariables.h"

/* Qui definisco alcuni processi/funzioni di prova per testare
 * le modifiche individualmente */

void test1(){
	while(TRUE){
		SYSCALL(PASSEREN,5,0,0);
		debug(1, 1);
		debug(1, 2);
		SYSCALL(VERHOGEN,5,0,0);
	}
}

void test2(){
	while(TRUE){
		SYSCALL(PASSEREN,5,0,0);
		debug(2, 1);
		debug(2, 2);
		SYSCALL(VERHOGEN,5,0,0);
	}
}

void test3(){
	while(TRUE){
		SYSCALL(PASSEREN,5,0,0);
		debug(3, 1);
		debug(3, 2);
		SYSCALL(VERHOGEN,5,0,0);
	}
}

void test4(){
	while(TRUE){
		SYSCALL(PASSEREN,5,0,0);
		debug(4, 1);
		debug(4, 2);
		SYSCALL(VERHOGEN,5,0,0);
	}
}
