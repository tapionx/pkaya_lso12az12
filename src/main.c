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

int main(void){
	initPcbs();
	initASL();
	/* Enable all the CPUs */
	addokbuf("Enabling all the CPUs!\n");
	
	
	
}
