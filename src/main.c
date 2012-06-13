#include "const.h"
#include "uMPStypes.h"
#include "listx.h"
#include "types11.h"
#include "utils.h"
#include "pcb.e"
#include "asl.e"

#define	MAXSEM	MAXPROC
#define MAX_PCB_PRIORITY		10
#define MIN_PCB_PRIORITY		0
#define DEFAULT_PCB_PRIORITY		5

#define NUM_CPU	4

/******************* MAIN TEST **********************/

int main(void)
{
	/* Popolare le 4 "New Areas" nel "ROM Reserved Frame" */
	/* ... ed accendere le CPU */
    
    /* Ogni CPU può essere rappresentata tramite una struttura dati
	 * all'interno della quale memorizziamo l'ID della CPU e le varie
	 * aree Old/New */
	typedef struct Areas{
		state_t Sysbp;  /* System Call & Break Points */
		state_t Trap;   /* Program Trap */
		state_t Tlb;    /* Translation Lookaside Buffer */
		state_t Ints;   /* Interrupts */
	} Areas;
	 
	typedef struct CPU{
		int id;
		Areas new;
		Areas old;
	} CPU;
	 
	CPU procs[NUM_CPU];
	Areas dummy;
	initState_t(&(dummy.Sysbp));
	initState_t(&(dummy.Trap));
	initState_t(&(dummy.Tlb));
	initState_t(&(dummy.Ints));
	
	/* New Area iniziali (comuni a tutte le CPU) */
	int id;
	for (id = 1; id < NUM_CPU; id++){
		CPU *cur = &(procs[id]);
		cur->id = id;
		debug(51, id);
		INITCPU(id, &(dummy), &(cur->new.Sysbp));
		INITCPU(id, &(dummy), &(cur->new.Trap));
		INITCPU(id, &(dummy), &(cur->new.Tlb));
		INITCPU(id, &(dummy), &(cur->new.Ints));
	}
	
	
    
	
    /* Inizializzare le strutture dati di fase 1 */
    initPcbs();
    initASL();
    addokbuf("Phase1 structures initialized!\n");
	
    /* Inizializzare le variabili dello scheduler */

    int processCount;
    int softBlockCount;
    pcb_t* currentProcess;
    /* Ready Queue */

    /* Inizializzare i semafori a 0, un semaforo per ogni sub-device */

    /* Inserire il processo test nella Ready Queue allocPcb() */

	addokbuf("White Flag ON\n");

    /* test print() */
    int n = 42;
    print(n);
}
