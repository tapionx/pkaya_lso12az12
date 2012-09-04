/* Qui definisco alcuni processi/funzioni di prova per testare
 * le modifiche individualmente */
 
/* Questa funzione stampa una stringa usando un semaforo per garantire
 * mutua esclusione */
void print1(){
	while(TRUE){
		int i=0;
		SYSCALL(PASSEREN, 0, 0, 0);
		for (; i<5; i++){
			addokbuf("CIAO 1\n");
		}
		SYSCALL(VERHOGEN, 0, 0, 0);
	}
}

void print2(){
	while(TRUE){
		SYSCALL(PASSEREN, 0, 0, 0);
		int i=0;	
		for (; i<5; i++){
			addokbuf("CIAO 2\n");
		}
		SYSCALL(VERHOGEN, 0, 0, 0);
	}
}
