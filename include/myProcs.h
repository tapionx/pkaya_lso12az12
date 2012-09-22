#ifndef MYPROCS
#define MYPROCS

#include "kernelVariables.h"
#include "utils.h"
#include "libumps.h"
#include "myConst.h"

/* Qui definisco alcuni processi/funzioni di prova per testare
 * le modifiche individualmente */


void test1(){
	SYSCALL(VERHOGEN, 6, 0, 0);
	while(TRUE){	
		SYSCALL(PASSEREN, 6, 0, 0);
		addokbuf("ATTENZIONE!\n");
		SYSCALL(VERHOGEN, 6, 0, 0);
	}
}

void test2(){
	while(TRUE){	
		SYSCALL(PASSEREN, 6, 0, 0);
		addokbuf("Sei stato scelto per partecipare al nostro prorgamma!\n");
		SYSCALL(VERHOGEN, 6, 0, 0);
	}
}

void test3(){
	while(TRUE){	
		SYSCALL(PASSEREN, 6, 0, 0);
		addokbuf("Hai capito stronzo?\n");
		SYSCALL(VERHOGEN, 6, 0, 0);
	}
}

void test4(){
	while(TRUE){	
		SYSCALL(PASSEREN, 6, 0, 0);
		addokbuf("Ma sei morto o cosa?\n");
		SYSCALL(VERHOGEN, 6, 0, 0);
	}
}


#endif
