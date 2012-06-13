/* Da specifica */
#include "const.h"
#include "uMPStypes.h"
#include "listx.h"
#include "types11.h"
#include "pcb.e"
#include "asl.e"

/* Custom */
#include "types.h"
#include "utils.h"
#include "myconst.h"
#include "handlers.h"

#define	MAXSEM	MAXPROC
#define MAX_PCB_PRIORITY		10
#define MIN_PCB_PRIORITY		0
#define DEFAULT_PCB_PRIORITY		5

CPU procs[NUM_CPU];
state_t init;

/******************* MAIN TEST **********************/

int main(void)
{	
	/* init default */
	initState_t(&init);
	/* Popolare le 4 "New Areas" nel "ROM Reserved Frame" */
	/* ... ed accendere le CPU */
	initCPUs(&init,procs);
	addokbuf("CPUs initialized\n");
}
