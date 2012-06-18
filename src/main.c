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

/* TESTS */
extern int p1test();
extern int p2test();

/* Global kernel variables */
state_t* pnew_old_areas[NUM_CPU][NUM_AREAS]; /* 8 areas for each cpu */
state_t pstate[NUM_CPU]; /* stati di load/store per le varie cpu */

/******************* MAIN **********************/

int main(void)
{				
	/************* INIZIALIZZAZIONE DEL SISTEMA */
	
	/* PERCHÉ SE ABILITO IL TIMER STO CAZZO DI PROCESSORE VA IN
	 * KERNEL PANIC?? EH?? CAZZO! SONO LE 6:35 E ANCORA NON HO CAPITO
	 * PERCHÉ! PORCA TROIA! */
	int status = getSTATUS();
	//setSTATUS(status|STATUS_IEp|STATUS_TE);
	/* Inizializzo le new (e old) area di tutte le CPU */
	initAreas(pnew_old_areas, NUM_CPU);
	/* Inizializzo le strutture dati di Phase1 */
	initPcbs();
	initASL();
	
	/************* CARICAMENTO DEI PROCESSI */
	
		/* Test phase2 
		pcb_t *phase2 = allocPcb();
		STST(&(phase2->p_s));
		phase2->p_s.status = getSTATUS();
		phase2->p_s.pc_epc = phase2->p_s.reg_t9 = (memaddr)p2test;
		addReady(phase2); */
	
	/************* ESECUZIONE DEI PROCESSI */
	/* Inizializzo la Interrupt Routing Table */
	initIRT();
	/* Inizializzo le altre CPU e faccio partire lo scheduler */
	initCpus();
	/* Avvio lo scheduler su CPU0 */
	scheduler();
	return 0;
}
