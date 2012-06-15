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

/* Global kernel variables */
state_t new_old_areas[NUM_CPU][NUM_AREAS]; /* 8 areas for each cpu */

/******************* MAIN **********************/

extern int p1test();

int main(void)
{		
	/* Inizializzo le new area di tutte le CPU */
	initNewAreas(new_old_areas, NUM_CPU);
	
	/* Inizializzo le strutture dati di Phase1 */
	initPcbs();
	initASL();

	/* TEST SCHEDULER */
	pcb_t *test = allocPcb();
	state_t *test_state = &(test->p_s);
	test_state->status = getSTATUS();
	test_state->status |= (STATUS_IEc|STATUS_TE)&~(STATUS_VMc|STATUS_KUc);
	test_state->reg_sp = RAMTOP-FRAME_SIZE;
	test_state->pc_epc = test_state->reg_t9 = (memaddr)test;
	addReady(test);
	
	scheduler();
	
	return 0;
}
