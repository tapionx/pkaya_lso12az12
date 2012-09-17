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
