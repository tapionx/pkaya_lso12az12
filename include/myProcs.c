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
		//debug(19, test1);
		SYSCALL(PASSEREN,5,0,0);
		//addokbuf("1\n");
		//debug(99, 1);
		//debug(99, 1);
		SYSCALL(VERHOGEN,5,0,0);
	}
}

void test2(){
	while(TRUE){
		//debug(19, test2);
		SYSCALL(PASSEREN,5,0,0);
		//addokbuf("2\n");
		//debug(99, 2);
		//debug(99, 2);
		SYSCALL(VERHOGEN,5,0,0);
	}
}

void test3(){
	while(TRUE){
		//debug(19, test3);
		SYSCALL(PASSEREN,5,0,0);
		//addokbuf("3\n");
		//debug(99, 3);
		//debug(99, 3);
		SYSCALL(VERHOGEN,5,0,0);
	}
}

void test4(){
	while(TRUE){
		//debug(19, test4);
		SYSCALL(PASSEREN,5,0,0);
		//addokbuf("4\n");
		//debug(99, 4);
		//debug(99, 4);
		SYSCALL(VERHOGEN,5,0,0);
	}
}
