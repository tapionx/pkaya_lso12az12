#include "kernelVariables.h"
#include "utils.h"
#include "libumps.h"

/* Qui definisco alcuni processi/funzioni di prova per testare
 * le modifiche individualmente */

void nop(){}


void handler(){
	debug(6666666,6666666);
	debug(6666666,6666666);
	debug(6666666,6666666);
	currentProcess[getPRID()]->custom_handlers[SYSBK_OLDAREA_INDEX]->pc_epc += 4;
	LDST(currentProcess[getPRID()]->custom_handlers[SYSBK_OLDAREA_INDEX]);
}

void test2(){
	state_t newarea, oldarea;
	cleanState(&newarea);
	cleanState(&oldarea);
	newarea.pc_epc = newarea.reg_t9 = (memaddr)handler;
	newarea.reg_sp = RAMTOP;
	newarea.status = EXCEPTION_STATUS;
	debug(77, currentProcess[getPRID()]->custom_handlers[SYSBK_NEWAREA_INDEX]);
	SYSCALL(SPECSYSVEC,&oldarea,&newarea,0);
	debug(1,1);
	debug(78, currentProcess[getPRID()]->custom_handlers[SYSBK_NEWAREA_INDEX]);
	debug(79, &newarea);
	debug(90, currentProcess[getPRID()]->custom_handlers[SYSBK_NEWAREA_INDEX]->pc_epc);
	debug(91, (memaddr)handler);
	SYSCALL(0,0,0,0);
	debug(1,1);
	while(TRUE);
}

void test1(){
	while(TRUE);
	int time = SYSCALL(GETCPUTIME,0,0,0);
	debug(2, time);
	while(TRUE);
}

void test3(){
	while(TRUE){
		SYSCALL(PASSEREN,5,0,0);
		printn("888888888888888\n",1);
		printn("888888888888888\n",1);
		printn("888888888888888\n",1);
		SYSCALL(VERHOGEN,5,0,0);
	}
}

void test4(){
	while(TRUE){
		SYSCALL(PASSEREN,5,0,0);
		printn("88888888888888888888\n",1);
		printn("88888888888888888888\n",1);
		printn("88888888888888888888\n",1);
		SYSCALL(VERHOGEN,5,0,0);
	}
}
