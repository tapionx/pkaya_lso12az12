/* Da specifica */
#include "const.h"
#include "uMPStypes.h"
#include "listx.h"
#include "types11.h"
#include "pcb.e"
#include "asl.e"

/* Custom */
#include "init.h"
#include "utils.h"
#include "myconst.h"
#include "handlers.h"
#include "scheduler.h"

#define	MAXSEM	MAXPROC
#define MAX_PCB_PRIORITY		10
#define MIN_PCB_PRIORITY		0
#define DEFAULT_PCB_PRIORITY		5

/* TEST */
extern int p1test();
extern int p2test();

/* Global kernel variables */
state_t new_old_areas[NUM_CPU][NUM_AREAS]; /* 8 areas for each cpu */

/******************* MAIN **********************/

int main(void)
{		
	p2test();
	return 0;
	/* Inizializzo le new area di tutte le CPU */
	initNewAreas(new_old_areas, NUM_CPU);
	
	/* Inizializzo le strutture dati di Phase1 */
	initPcbs();
	initASL();
	
	scheduler();
	
	return 0;
}
