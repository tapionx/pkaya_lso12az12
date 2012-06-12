#include "const.h"
#include "uMPStypes.h"
#include "listx.h"
#include "types11.h"

#include "pcb.e"
#include "asl.e"

#include "utils.h"

#define	MAXSEM	MAXPROC

#define MAX_PCB_PRIORITY		10
#define MIN_PCB_PRIORITY		0
#define DEFAULT_PCB_PRIORITY		5


#define TRANSMITTED	5
#define TRANSTATUS    2
#define ACK	1
#define PRINTCHR	2
#define CHAROFFSET	8
#define STATUSMASK	0xFF
#define	TERM0ADDR	0x10000250
#define DEVREGSIZE 16       
#define READY     1
#define DEVREGLEN   4
#define TRANCOMMAND   3
#define BUSY      3


/******************* MAIN TEST **********************/

int main(void)
{
    /* Popolare le 4 "New Areas" nel "ROM Reserved Frame" */

    /* Inizializzare le strutture dati di fase 1 */
    initPcbs();
	initASL();

    /* Inizializzare le variabili dello scheduler */

    int processCount;
    int softBlockCount;
    pcb_t* currentProcess;
    /* Ready Queue */

    /* Inizializzare i semafori a 0, un semaforo per ogni sub-device */

    /* Inserire il processo test nella Ready Queue allocPcb() */

	addokbuf("White Flag ON\n");
}
