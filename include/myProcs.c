#include "kernelVariables.h"

/* Qui definisco alcuni processi/funzioni di prova per testare
 * le modifiche individualmente */

void nop(){}

void test1(){
	while(TRUE){
		SYSCALL(PASSEREN,5,0,0);
		printn("Ciao! Sono il processo % :)\n", 1);
		printn("E al processo % piacciono le banane!\n", 1);
		printn("Ora il processo % vola viaaa!\n\n", 1);
		SYSCALL(VERHOGEN,5,0,0);
	}
}

void test2(){
	while(TRUE){
		SYSCALL(PASSEREN,5,0,0);
		printn("Ciao! Sono il processo % :)\n", 2);
		printn("E al processo % piacciono le banane!\n", 2);
		printn("Ora il processo % vola viaaa!\n\n", 2);
		SYSCALL(VERHOGEN,5,0,0);
	}
}

void test3(){
	while(TRUE){
		SYSCALL(PASSEREN,5,0,0);
		printn("Ciao! Sono il processo % :)\n", 3);
		printn("E al processo % piacciono le banane!\n", 3);
		printn("Ora il processo % vola viaaa!\n\n", 3);
		SYSCALL(VERHOGEN,5,0,0);
	}
}

void test4(){
	while(TRUE){
		SYSCALL(PASSEREN,5,0,0);
		printn("Ciao! Sono il processo % :)\n", 4);
		printn("E al processo % piacciono le banane!\n", 4);
		printn("Ora il processo % vola viaaa!\n\n", 4);
		SYSCALL(VERHOGEN,5,0,0);
	}
}
