#include "kernelVariables.h"

/* Qui definisco alcuni processi/funzioni di prova per testare
 * le modifiche individualmente */
 
void prova_altracpu(){
	while(TRUE){
		SYSCALL(PASSEREN,0,0,0);
			lock(0);
			printn("% SBENG TRICCHETTACCHE\n\n", currentProcess[getPRID()]);
			free(0);
			//debug(1,currentProcess[getPRID()]);
		SYSCALL(VERHOGEN,0,0,0);
	}
}

void test1(){
	while(TRUE){
		SYSCALL(PASSEREN,1,0,0);
		addokbuf("1\n");
		SYSCALL(VERHOGEN,1,0,0);
	}
}

void test2(){
	while(TRUE){
		SYSCALL(PASSEREN,1,0,0);
		addokbuf("2\n");
		SYSCALL(VERHOGEN,1,0,0);
	}
}

void test3(){
	while(TRUE){
		SYSCALL(PASSEREN,1,0,0);
		addokbuf("3\n");
		SYSCALL(VERHOGEN,1,0,0);
	}
}

void test4(){
	while(TRUE){
		SYSCALL(PASSEREN,1,0,0);
		addokbuf("4\n");
		SYSCALL(VERHOGEN,1,0,0);
	}
}
