/* Qui definisco alcuni processi/funzioni di prova per testare
 * le modifiche individualmente */
 
 
int COMMON = 0;
 
/* Questa funzione stampa una stringa usando un semaforo per garantire
 * mutua esclusione */
void print1(){
	for(;;){
		SYSCALL(PASSEREN, 0, 0, 0);
		COMMON = 1;
		debug(1,COMMON);
		SYSCALL(VERHOGEN, 0, 0, 0);
	}
}

void print2(){
	for(;;){
		SYSCALL(PASSEREN, 0, 0, 0);
		COMMON = 2;
		debug(2,COMMON);
		SYSCALL(VERHOGEN, 0, 0, 0);
	}
}

void print3(){
	for(;;){
		SYSCALL(PASSEREN, 0, 0, 0);
		COMMON = 3;
		debug(3,COMMON);
		SYSCALL(VERHOGEN, 0, 0, 0);
	}
}

void print4(){
	for(;;){
		SYSCALL(PASSEREN, 0, 0, 0);
		COMMON = 4;
		debug(4,COMMON);
		SYSCALL(VERHOGEN, 0, 0, 0);
	}
}
