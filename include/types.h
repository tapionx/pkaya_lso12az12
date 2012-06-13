/* Ogni CPU può essere rappresentata tramite una struttura dati
	 * all'interno della quale memorizziamo l'ID della CPU e le varie
	 * aree Old/New */ 

typedef struct Areas{
	state_t SysBp;  /* System Call & Break Points */
	state_t Trap;   /* Program Trap */
	state_t Tlb;    /* Translation Lookaside Buffer */
	state_t Ints;   /* Interrupts */
} Areas;
	 
typedef struct CPU{
	int id;
	Areas new;
	Areas old;
} CPU;

