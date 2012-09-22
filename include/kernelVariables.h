#ifndef __KERNEL
#define __KERNEL

/* Variabili del nucleo */
extern int processCount; /* Contatore della totalita' dei processi */ 
extern int softBlockCount; /* Contatore dei processi bloccati su semafori */
extern struct list_head readyQueue; /* Coda dei processi in stato ready */
extern pcb_t *currentProcess[NUM_CPU]; /* Puntatore al processo correntemente in esecuzione */
extern int locks[NUM_SEMAPHORES]; /* Variabili di condizione per CAS */
extern state_t scheduler_states[NUM_CPU]; /* state_t dello scheduler */

extern state_t areas[NUM_CPU][NUM_AREAS]; /* Aree reali per CPU > 0 */
extern int pctInit; /* Lo Pseudo Clock Timer è stato inizializzato? */
extern U32 devStatus; /* Status in output dei vari device */

#endif
