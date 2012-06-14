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

#define	MAXSEM	MAXPROC
#define MAX_PCB_PRIORITY		10
#define MIN_PCB_PRIORITY		0
#define DEFAULT_PCB_PRIORITY		5

/* Global kernel variables */
state_t new_old_areas[NUM_CPU][NUM_AREAS]; /* 8 areas for each cpu */
state_t pstate[NUM_CPU]; /* stati di load/store per le varie cpu */

/******************* MAIN **********************/

int main(void)
{	
	/* Inizializzo le new area di tutte le CPU */
	initNewAreas(new_old_areas, NUM_CPU);


	/* Leggi bitmap dei device installati */
	
	return 0;
}
