#include "kernelVariables.h"
#include "utils.h"
#include "libumps.h"

/* Qui definisco alcuni processi/funzioni di prova per testare
 * le modifiche individualmente */


void test2(){
	while(TRUE){
		SYSCALL(PASSEREN,5,0,0);
		printn("Io sono il processo %\n\n", (U32)currentProcess[getPRID()]);
		SYSCALL(VERHOGEN,5,0,0);
	}
}



void test3(){
	while(TRUE){
		SYSCALL(PASSEREN,5,0,0);
		printn("Io sono il processo %\n\n", (U32)currentProcess[getPRID()]);
		SYSCALL(VERHOGEN,5,0,0);
	}
}

void test4(){
	while(TRUE){
		SYSCALL(PASSEREN,5,0,0);
		printn("Io sono il processo %\n\n", (U32)currentProcess[getPRID()]);
		SYSCALL(VERHOGEN,5,0,0);
	}
}


void test1(){		
		/////////////////////////////
	   // PROCESSO DI prova2 	  //
		pcb_t* prova2 = allocPcb();
		//STST(&(prova2.p_s));
		prova2->p_s.pc_epc = prova2->p_s.reg_t9 = (memaddr)test2;
		prova2->p_s.reg_sp = PFRAMES_START-4*QPAGE;
		prova2->p_s.status = prova2->p_s.status | PROCESS_STATUS;
		
		/////////////////////////////
	   // PROCESSO DI prova3 	  //
		pcb_t* prova3 = allocPcb();
		//STST(&(prova3.p_s));
		prova3->p_s.pc_epc = prova3->p_s.reg_t9 = (memaddr)test3;
		prova3->p_s.reg_sp = PFRAMES_START-6*QPAGE;
		prova3->p_s.status = prova3->p_s.status | PROCESS_STATUS;
		
		/////////////////////////////
	   // PROCESSO DI prova4 	  //
		pcb_t* prova4 = allocPcb();
		//STST(&(prova4.p_s));
		prova4->p_s.pc_epc = prova4->p_s.reg_t9 = (memaddr)test4;
		prova4->p_s.reg_sp = PFRAMES_START-8*QPAGE;
		prova4->p_s.status = prova4->p_s.status | PROCESS_STATUS;
		
	SYSCALL(PASSEREN,5,0,0);
	
	printn("IO SONO IL PROCESSO PADRE [ %] E I MIEI FIGLI SONO: ", (U32)currentProcess[getPRID()]); 
	
	int res = SYSCALL(CREATEPROCESS,(U32)&(prova2->p_s),DEFAULT_PCB_PRIORITY,0);
	if (res == 0){
		pcb_t *child = container_of(list_next(&(currentProcess[getPRID()]->p_child)), pcb_t, p_sib);
		printn("% ", (U32)child);
	} else {
		printn("% ", (U32)res);
	}
	
	res = SYSCALL(CREATEPROCESS,(U32)&(prova3->p_s),DEFAULT_PCB_PRIORITY,0);
	if (res == 0){
		pcb_t *child = container_of(list_next(list_next(&(currentProcess[getPRID()]->p_child))), pcb_t, p_sib);
		printn("% ", (U32)child);
	} else {
		printn("% ", (U32)res);
	}
	
	res = SYSCALL(CREATEPROCESS,(U32)&(prova4->p_s),DEFAULT_PCB_PRIORITY,0);
	if (res == 0){
		pcb_t *child = container_of(list_next(list_next(list_next(&(currentProcess[getPRID()]->p_child)))), pcb_t, p_sib);
		printn("% \n\n", (U32)child);
	} else {
		printn("% \n\n", (U32)res);
	}
	
	SYSCALL(VERHOGEN,5,0,0);
	while(TRUE);
}


