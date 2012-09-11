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
#include "myProcs.c"

#define	MAXSEM	MAXPROC
#define MAX_PCB_PRIORITY		10
#define MIN_PCB_PRIORITY		0
#define DEFAULT_PCB_PRIORITY		5

/* TESTS */
extern int p1test();
extern int p2test();

/* Global kernel variables */
state_t *pnew_old_areas[NUM_CPU][NUM_AREAS]; /* puntatori alle new_old areas */
state_t new_old_areas[NUM_CPU][NUM_AREAS]; /* 8 areas for each cpu */
state_t pstate[NUM_CPU]; /* stati di load/store per lo scheduler */
int locks[MAXPROC]; /* Vettore di variabili di condizione per i semafori */

/******************* MAIN **********************/

int main(void)
{
	/************* INIZIALIZZAZIONE DEL SISTEMA */
	/* Inizializzazione del vettore dei lock a PASS */
	initLock();
	/* Inizializzo le new (e old) area di tutte le CPU */
	initAreas(pnew_old_areas, NUM_CPU);
	/* Inizializzo le strutture dati di Phase1 */
	initPcbs();
	initASL();
	/* Inizializzo le strutture dello scheduler */
	initReadyQueues();
	
	
	/************* CARICAMENTO DEI PROCESSI NELLE READY QUEUE */
	
		/* Test phase2 */
		//pcb_t *phase2 = allocPcb();
		//STST(&(phase2->p_s));
		//phase2->p_s.status = getSTATUS();
		//phase2->p_s.pc_epc = phase2->p_s.reg_t9 = (memaddr)p2test;
		//phase2->p_s.reg_sp = PFRAMES_START;
		//addReady(phase2);
		
		/* Test di alcuni processi di prova */
		pcb_t *test1 = allocPcb();
		STST(&(test1->p_s));
		test1->p_s.status = getSTATUS();
		(test1->p_s).pc_epc = (test1->p_s).reg_t9 = (memaddr)print1;
		test1->p_s.reg_sp = PFRAMES_START;
		addReady(test1);
		
		/* Test di alcuni processi di prova */
		pcb_t *test2 = allocPcb();
		STST(&(test2->p_s));
		test2->p_s.status = getSTATUS();
		test2->p_s.pc_epc = test2->p_s.reg_t9 = (memaddr)print2;
		test2->p_s.reg_sp = test1->p_s.reg_sp-FRAME_SIZE;
		addReady(test2);
		
		pcb_t *test3 = allocPcb();
		STST(&(test3->p_s));
		test3->p_s.status = getSTATUS();
		test3->p_s.pc_epc = test3->p_s.reg_t9 = (memaddr)print3;
		test3->p_s.reg_sp = test2->p_s.reg_sp-FRAME_SIZE;
		addReady(test3);

		pcb_t *test4 = allocPcb();
		STST(&(test4->p_s));
		test4->p_s.status = getSTATUS();
		test4->p_s.pc_epc = test4->p_s.reg_t9 = (memaddr)print4;
		test4->p_s.reg_sp = test3->p_s.reg_sp-FRAME_SIZE;
		addReady(test4);
	
	/************* ESECUZIONE DEI PROCESSI */
	/* Inizializzo la Interrupt Routing Table dinamica */
	/* initIRT(); */
	/* Inizializzo le altre CPU e faccio partire lo scheduler */
	initCpus();
	return 0;
}
