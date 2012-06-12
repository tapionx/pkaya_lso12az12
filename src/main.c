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


/******************* MAIN TEST **********************/

int main(void)
{
	/* DEBUGGING TEST */
	int ciao = 23+5; 
	debug(38, ciao); /* expected 28 in $a1 */
	debug(39, TRUE); /* expected 1 in $a1 */
	debug(40, FALSE); /* expected 0 in $a1 */
	
	
    /* Popolare le 4 "New Areas" nel "ROM Reserved Frame" */
	
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
}
