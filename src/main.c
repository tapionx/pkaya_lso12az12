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
state_t pstate[NUM_CPU]; /* stati di load/store per le varie cpu */

/******************* MAIN **********************/

/* TEST SCHEDULER */
int globo = 1;
extern void proc1();
extern void proc2();
extern void proc3();

int main(void)
{		
	/* Inizializzo le new area di tutte le CPU */
	initNewAreas(new_old_areas, NUM_CPU);
	
	/* Inizializzo le strutture dati di Phase1 */
	initPcbs();
	initASL();
	
	/* Inizializzo i pcb dei 3 processi di prova */
	pcb_t *pcb1 = allocPcb();
	pcb_t *pcb2 = allocPcb();
	pcb_t *pcb3 = allocPcb();
	
	initPcbState(pcb1, getSTATUS(), (memaddr)proc1, NULL);
	initPcbState(pcb2, getSTATUS(), (memaddr)proc2, NULL);
	initPcbState(pcb3, getSTATUS(), (memaddr)proc3, NULL);
	
	addReady(0, pcb1);
	printn("Inserito il pcb 1\n",0);
	addReady(0, pcb2);
	printn("Inserito il pcb 2\n",0);
	addReady(0, pcb3);
	printn("Inserito il pcb 3\n",0);
	/* Carico i processi nelle readyQueue */
	scheduler();
	
	return 0;
}

void proc1(){
	addokbuf("Eseguito p1!\n");
	globo++;
	LDST(&(pstate[0]));
}

void proc2(){
	addokbuf("Eseguito p2!\n");
	globo++;
	LDST(&(pstate[0]));
}

void proc3(){
	addokbuf("Eseguito p3!\n");
	globo++;
	LDST(&(pstate[0]));
}
