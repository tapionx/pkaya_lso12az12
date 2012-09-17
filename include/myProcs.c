#include "kernelVariables.h"

/* Qui definisco alcuni processi/funzioni di prova per testare
 * le modifiche individualmente */
 
void prova_altracpu(){
	while(TRUE){
		SYSCALL(PASSEREN,0,0,0);
		SYSCALL(VERHOGEN,0,0,0);
	}
}
