/* Qui definisco alcuni processi/funzioni di prova per testare
 * le modifiche individualmente */
 
/* Questa funzione stampa una stringa usando un semaforo per garantire
 * mutua esclusione */
void print1(){
		SYSCALL(PASSEREN, 0, 0, 0);
		addokbuf("1\n");
		SYSCALL(VERHOGEN, 0, 0, 0);
}

void print2(){
		SYSCALL(PASSEREN, 0, 0, 0);	
		addokbuf("2\n");
		SYSCALL(VERHOGEN, 0, 0, 0);
}
