#include "kernelVariables.h"

/* Qui definisco alcuni processi/funzioni di prova per testare
 * le modifiche individualmente */

void nop(){}


void handler(){
	debug(6666666,6666666);
	debug(6666666,6666666);
	debug(6666666,6666666);
}

void test1(){
	state_t newarea, oldarea;
	newarea.pc_epc = newarea.reg_t9 = (memaddr)handler;
	newarea.reg_sp = RAMTOP;
	newarea.status = EXCEPTION_STATUS;
	SYSCALL(SPECSYSVEC,&oldarea,&newarea,0);
	debug(1,1);
	while(TRUE);
}

void test2(){
	int time = SYSCALL(GETCPUTIME,0,0,0);
	debug(2, time);
	while(TRUE);
}

void test3(){
	while(TRUE){
		SYSCALL(PASSEREN,5,0,0);
		printn("888888888888888\n");
		printn("888888888888888\n");
		printn("888888888888888\n");
		SYSCALL(VERHOGEN,5,0,0);
	}
}

void test4(){
	while(TRUE){
		SYSCALL(PASSEREN,5,0,0);
		printn("88888888888888888888\n");
		printn("88888888888888888888\n");
		printn("88888888888888888888\n");
		SYSCALL(VERHOGEN,5,0,0);
	}
}
