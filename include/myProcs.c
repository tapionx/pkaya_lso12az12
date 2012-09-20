#ifndef MYPROCS
#define MYPROCS


#include "kernelVariables.h"
#include "utils.h"
#include "libumps.h"

/* Qui definisco alcuni processi/funzioni di prova per testare
 * le modifiche individualmente */

void test2(){		
	SYSCALL(WAITCLOCK,0,0,0);
	debug(1,1);
	for(;;);
}

void test1(){
	SYSCALL(WAITCLOCK,0,0,0);
	debug(1,1);
	for(;;);
}

void test3(){
	SYSCALL(WAITCLOCK,0,0,0);
	debug(1,1);
	for(;;);
}

void test4(){
	SYSCALL(WAITCLOCK,0,0,0);
	debug(1,1);
	for(;;);
}


#endif
