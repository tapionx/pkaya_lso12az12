/* Qui definisco alcuni processi/funzioni di prova per testare
 * le modifiche individualmente */
 
/* Questa funzione stampa una stringa usando un semaforo per garantire
 * mutua esclusione */
void print1(){
	while(TRUE){
		//debug(1603, 1);
	}
}

void print2(){
	//SYSCALL(PASSEREN, 0, 0, 0);
	while(TRUE){
		//debug(1603, 2);
	}
}
