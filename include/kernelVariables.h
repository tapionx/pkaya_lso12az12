/* Variabili del nucleo */
extern int processCount; /* Contatore della totalita' dei processi */ 
extern int softBlockCount; /* Contatore dei processi bloccati su semafori */
extern struct list_head readyQueue; /* Coda dei processi in stato ready */
extern pcb_t *currentProcess[NUM_CPU]; /* Puntatore al processo correntemente in esecuzione */
extern state_t areas[NUM_CPU][NUM_AREAS];
extern int locks[MAXPROC+MAX_DEVICES]; /* Variabili di condizione per CAS */
extern state_t scheduler_states[NUM_CPU]; /* state_t dello scheduler */

