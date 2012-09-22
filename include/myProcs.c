#ifndef MYPROCS
#define MYPROCS


#include "kernelVariables.h"
#include "utils.h"
#include "libumps.h"

/* Qui definisco alcuni processi/funzioni di prova per testare
 * le modifiche individualmente */


void test1(){	
	while(TRUE);
	print("111111111111\n");
	while(TRUE);
}

void test2(){
	setTIMER(999999999);
	while(TRUE)
	print("222222222222\n");
	
}

void test3(){
	setTIMER(999999999);
	while(TRUE)
	print("33333333\n");
	
}

void test4(){
	setTIMER(999999999);
	while(TRUE)
	print("4444444\n");
}


#endif
