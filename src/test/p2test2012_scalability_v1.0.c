/*****************************************************************************
 * Copyright 2004, 2005 Michael Goldweber, Davide Brini.                     *
 *                                                                           *
 * This file is part of kaya.                                                *
 *                                                                           *
 * kaya is free software; you can redistribute it and/or modify it under the *
 * terms of the GNU General Public License as published by the Free Software *
 * Foundation; either version 2 of the License, or (at your option) any      *
 * later version.                                                            *
 * This program is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General *
 * Public License for more details.                                          *
 * You should have received a copy of the GNU General Public License along   *
 * with this program; if not, write to the Free Software Foundation, Inc.,   *
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.                  *
 *****************************************************************************/

/*********************************P2TEST.C*******************************
 *
 *	Test program for the Kaya Kernel: phase 2.
 *
 *	Produces progress messages on Terminal0.
 *	
 *      Modified by Marco Di Felice 2012
 */

#include "const.h"
#include "types11.h"
#include "libumps.h"

typedef unsigned int devregtr;

typedef U32 cpu_t;

/* hardware constants */
#define PRINTCHR		2
#define BYTELEN			8
#define RECVD			5
#define TRANSM 			5

#define CLOCKINTERVAL		100000UL	/* interval to V clock semaphore */

#define TERMSTATMASK		0xFF
#define CAUSEMASK		0xFF
#define VMOFF 			0xF8FFFFFF

#define QPAGE			1024

#define KUPBITON		0x8
#define KUPBITOFF		0xFFFFFFF7

#define BADADDR			0xFFFFFFFF /* could be 0x00000000 as well */
#define	TERM0ADDR		0x10000250


// Other constants
#define BASE_VALUE		1000000000
#define DIGIT_ASCII		48


// Number of processes (do not change it, must be a multiple of 4)
#define NUMPROC			16

// Weight constant
#define MAXLOOP			200000
#define MINLOOP			2000

// Semaphores' Keys
#define TERM_MUT 		0
#define STARTCPU		1
#define ENDCPU			2
#define ENDSERVICE		3
#define ENDINTERACT		4
#define BASEKEYSEM		5


// Priorities
#define DEFAULT_PRIORITY  2
#define LOW_PRIORITY	  1
#define HIGH_PRIORITY	  4
#define MEDIUM_PRIORITY	  2



void	proc_cpu(), proc_cpu2(), proc_services(), proc_interactive1(int), proc_interactive2(int);

// Processes' states
state_t pstate[NUMPROC];


/*************************************************************************************************
 * Printing procedures
 *************************************************************************************************/

/* a procedure to print on terminal 0 */
void print(char *msg) {

	char * s = msg;
	devregtr * base = (devregtr *) (TERM0ADDR);
	devregtr status;
	
	SYSCALL(PASSEREN, TERM_MUT, 0, 0);				/* get term_mut lock */
	
	while (*s != '\0') {
	  /* Put "transmit char" command+char in term0 register (3rd word). This 
		   actually starts the operation on the device! */
		*(base + 3) = PRINTCHR | (((devregtr) *s) << BYTELEN);
		
		/* Wait for I/O completion (SYS8) */
		status = SYSCALL(WAITIO, INT_TERMINAL, 0, FALSE);
		
		if ((status & TERMSTATMASK) != TRANSM)
			PANIC();

		s++;	
	}
	
	SYSCALL(VERHOGEN, TERM_MUT, 0, 0);				/* release term_mut */
}


char buffer[30];
/* a procedure to convert an integer into a sequence of characters */
void print_val(int val, char *strp) {
	long base=BASE_VALUE;
	int exp=10, i;

	/* Get the digits of a number through base10 divisions */
	for (i=0; i<exp; i++) {

		int c=val/base;
		val=val%base;
		base=base/10;
		char car=(char)(c+DIGIT_ASCII);
		strp[i]=car;
	}

	strp[exp]='\n';
	strp[exp+1]='\0';

	print(strp);
}

/*********************************************************************************************
 * The core test program																	 
 *********************************************************************************************/

void test() {	

	int i;
	
	cpu_t		time_start, time_end, time_test1start, time_test1end, time_test1, time_test2start, time_test2end, time_test2, time_test3start, time_test3end, time_test3;
	cpu_t		time_test4start, time_test4end, time_test4, time_test;
	
	
	// Starting time of the Test
	time_start=GET_TODLOW;
	
	// Unblock the mutex semaphore for the I/O operations
	SYSCALL(VERHOGEN, TERM_MUT, 0, 0);	
	
	
	print("Scalability Test Start! \n");
	
	/*****************************************************************************************
	 * Test 1: Create NUMPROC homegenous processes and execute them
	 *         All processes have the same priority and perform CPU-intensive operations
	 *****************************************************************************************/

	// Define the processes' states
	
	int stackp=RAMTOP-QPAGE;
	for (i=0; i<NUMPROC; i++) {	
		/* set up process state */
		STST(&pstate[i]);			
	
		pstate[i].reg_sp = stackp;
	
		/* setup processes' PC */
		pstate[i].pc_epc = pstate[i].reg_t9 = (memaddr)proc_cpu;
	
		/* setup status masks */
		pstate[i].status = pstate[i].status | STATUS_IEp | STATUS_INT_UNMASKED;

		/* decrease the stack pointer */
		stackp-=QPAGE;
    
	}
		
	
	print("TEST1: CPU-Intensive Homogeneous Procs \n");
	
	time_test1start=GET_TODLOW;
	
	
	for (i=0; i<NUMPROC; i++)
		
		// Create the processes
		SYSCALL(CREATEPROCESS, (int)&pstate[i], DEFAULT_PRIORITY, 0);
		
	
	for (i=0; i<NUMPROC; i++)
		
		// Start the processes
		SYSCALL(VERHOGEN, STARTCPU, 0, 0);
	
	for (i=0; i<NUMPROC; i++)
		
		// Wait the termination of all the processes
		SYSCALL(PASSEREN, ENDCPU, 0, 0);
	
	time_test1end=GET_TODLOW;
	
	// Compute the TEST1 execution time
	time_test1=time_test1end - time_test1start;
	
	printf(" Execution Time TEST1: \n");
	print_val(time_test1, buffer);

	// Add some delay to wait for the termination of the NUMPROC processes
	cpu_t		time1, time2;
	
	/* do some delay */
	time1 = 0;
	time2 = 0;
	
	while (time2 - time1 < (CLOCKINTERVAL >> 1))  {
		time1 = GET_TODLOW;
		SYSCALL(WAITCLOCK, 0, 0, 0);
		time2 = GET_TODLOW;
	}
	
	
	/*****************************************************************************************
	 * Test 2: Create NUMPROC homegenous processes and execute them
	 *         Processes have different priorities and perform CPU-intensive operations
	 *****************************************************************************************/
	
	// Define the processes' states
	
	stackp=RAMTOP-QPAGE;

	for (i=0; i<NUMPROC; i++) {	
		STST(&pstate[i]);			
		
		pstate[i].reg_sp = stackp;
		
		/* define two classes of processes */
		if (i%2 ==0)
			pstate[i].pc_epc = pstate[i].reg_t9 = (memaddr)proc_cpu;
		else 
			pstate[i].pc_epc = pstate[i].reg_t9 = (memaddr)proc_cpu2;

		/* define the processes' state */
		pstate[i].status = pstate[i].status | STATUS_IEp | STATUS_INT_UNMASKED;
		
		/* decrease the stack pointer */
                stackp-=QPAGE;		

	}
	
	print("TEST2: CPU-Intensive Heterogeneous Procs \n");
	
	time_test2start=GET_TODLOW;
	
	for (i=0; i<NUMPROC; i++)
		if (i%2==0)
			// Create the processes
			SYSCALL(CREATEPROCESS, (int)&pstate[i], LOW_PRIORITY, 0);
		else 
			// Create the processes
			SYSCALL(CREATEPROCESS, (int)&pstate[i], HIGH_PRIORITY, 0);
	
	for (i=0; i<NUMPROC; i++)
		
		// Start the processes
		SYSCALL(VERHOGEN, STARTCPU, 0, 0);
	
	for (i=0; i<NUMPROC; i++)
		
		// Wait the termination of all the processes
		SYSCALL(PASSEREN, ENDCPU, 0, 0);
	
	time_test2end=GET_TODLOW;
	
	// Compute the TEST2 execution time
	time_test2=time_test2end - time_test2start;
	
	
	printf(" Execution Time TEST2: \n");
	print_val(time_test2, buffer);
	
	// Add some delay to wait for the termination of the NUMPROC processes
	time1 = 0;
	time2 = 0;
	
	while (time2 - time1 < (CLOCKINTERVAL >> 1))  {
		time1 = GET_TODLOW;
		SYSCALL(WAITCLOCK, 0, 0, 0);
		time2 = GET_TODLOW;
	}
	
	
	
	
	/*****************************************************************************************
	 * Test 3: Create NUMPROC/2 couples of interacting processes and execute them
	 *         Processes have different priorities and perform semaphores operations
	 *****************************************************************************************/
	
	// Define the processes' states
	stackp=RAMTOP-QPAGE;


	for (i=0; i<NUMPROC; i++) {	
		/* set up p2's state */
		STST(&pstate[i]);			/* create a state area using p1's state    */	
		
		/* stack of p2 should sit above ??????  */
		pstate[i].reg_sp = stackp;
		
		
		if (i%2 ==0) 
		/* p2 starts executing function p2 */
			pstate[i].pc_epc = pstate[i].reg_t9 = (memaddr)proc_interactive1;
		else 
			pstate[i].pc_epc = pstate[i].reg_t9 = (memaddr)proc_interactive2;
		
		pstate[i].reg_a0 = i;
		
		/* p2 has interrupts on and unmasked */
		pstate[i].status = pstate[i].status | STATUS_IEp | STATUS_INT_UNMASKED;
		
		/* decrease the stack pointer */
                stackp-=QPAGE;		

	}
	
	print("TEST3: Interactive Heterogeneous Procs \n");
	
	time_test3start=GET_TODLOW;
	
	for (i=0; i<NUMPROC; i++)
		if (i%2==0)
			// Create the processes
			SYSCALL(CREATEPROCESS, (int)&pstate[i], LOW_PRIORITY, 0);
	
	// Wait for the termination of all the processes
	for (i=0; i<NUMPROC; i++)
		SYSCALL(PASSEREN, ENDINTERACT, 0, 0);
	
	
	time_test3end=GET_TODLOW;

	
	// Compute the TEST3 execution time
	time_test3=time_test3end - time_test3start;
	
	printf(" Execution Time TEST3: \n");
	print_val(time_test3, buffer);



	/*****************************************************************************************
	 * Test 4: Create NUMPROC processes and execute them
	 *         Processes have different priorities and perform different operations (CPU intensive, SO requests, IP synchronization, etc) 
	 *****************************************************************************************/
	
	// Define the processes' states
	stackp=RAMTOP-QPAGE;

	for (i=0; i<NUMPROC; i++) {	
		/* set up p2's state */
		STST(&pstate[i]);			/* create a state area using p1's state    */	
		
		/* stack of p2 should sit above ??????  */
		pstate[i].reg_sp = stackp;
		
		switch(i%4) {
				
			case 0:
				pstate[i].pc_epc = pstate[i].reg_t9 = (memaddr)proc_cpu;
				break;
			case 1:
				pstate[i].pc_epc = pstate[i].reg_t9 = (memaddr)proc_services;
				break;
			case 2:
				pstate[i].pc_epc = pstate[i].reg_t9 = (memaddr)proc_interactive1;
				pstate[i].reg_a0 = i;
				break;
			case 3:	
				pstate[i].pc_epc = pstate[i].reg_t9 = (memaddr)proc_interactive2;
				pstate[i].reg_a0 = i;
				break;
		}
		
		/* p2 has interrupts on and unmasked */
		pstate[i].status = pstate[i].status | STATUS_IEp | STATUS_INT_UNMASKED;
		
		/* decrease the stack pointer */
                stackp-=QPAGE;		

	}
	
	print("TEST4: Heterogenous Procs with different priorities and types \n");
	
	time_test4start=GET_TODLOW;
	
	
	for (i=0; i<NUMPROC; i++) {
		switch (i%4) {
			case 0:
				// Create the processes
				SYSCALL(CREATEPROCESS, (int)&pstate[i], LOW_PRIORITY, 0);
				break;
			case 1:
				// Create the processes
				SYSCALL(CREATEPROCESS, (int)&pstate[i], MEDIUM_PRIORITY, 0);
				break;
			case 2:
				// Create the processes
				SYSCALL(CREATEPROCESS, (int)&pstate[i], HIGH_PRIORITY, 0);
				break;
		}
	}
	
	
	// Wait for the termination of all the processes
	for (i=0; i<(NUMPROC/2); i++)
		SYSCALL(PASSEREN, ENDINTERACT, 0, 0);
	for (i=0; i<(NUMPROC/4); i++)
		SYSCALL(PASSEREN, ENDSERVICE, 0, 0);
	for (i=0; i<(NUMPROC/4); i++)
		SYSCALL(PASSEREN, ENDCPU, 0, 0);
	
	time_test4end=GET_TODLOW;
	time_test4=time_test4end-time_test4start;

   	printf(" Execution Time TEST4: \n");
	print_val(time_test4, buffer);
	
	time_end=GET_TODLOW;
			
	printf(" Total TEST Time: \n");
	time_test=time_end-time_start;
	print_val(time_test, buffer);

	print("TEST Concluded! \n");

	HALT();
	
}



// This is the code of a process performing CPU operations
void proc_cpu() {
	
	long i, val;
	
	SYSCALL(PASSEREN, STARTCPU, 0, 0);

	val=0;
	// Do some computation (meaningless)
	for (i=0; i<MAXLOOP; i++) 
		val=(val+(2*12345)) % MAXLOOP;
	
	SYSCALL(VERHOGEN, ENDCPU, 0, 0);
	
	SYSCALL(TERMINATEPROCESS, -1, 0, 0);			

	PANIC();			
}


// This is the code of a process performing CPU operations
void proc_cpu2() {
	
	long i, val;
	
	SYSCALL(PASSEREN, STARTCPU, 0, 0);
	
	val=0;
	// Do some computation (meaningless)
	for (i=0; i<(MAXLOOP*2); i++) 
		val=(val+(2*12345)) % MAXLOOP;
	
	SYSCALL(VERHOGEN, ENDCPU, 0, 0);
	
	SYSCALL(TERMINATEPROCESS, -1, 0, 0);			
	
	PANIC();			
}

// This is the code of a process performing frequent system service requests
void proc_services() {
	
	
	// Request the CPU clock time
	cpu_t time = SYSCALL(GETCPUTIME, 0, 0, 0);			
	
	cpu_t time1 = 0;
	cpu_t time2 = 0;
	
	/* loop until we are delayed at least half of clock V interval */
	while ((time2 - time1) < (CLOCKINTERVAL >> 1) )  {
		time1 = GET_TODLOW;			/* time of day     */
		SYSCALL(WAITCLOCK, 0, 0, 0);
		time2 = GET_TODLOW;			/* new time of day */
	}
	
	// Request the CPU clock time
	time = SYSCALL(GETCPUTIME, 0, 0, 0);
	
	// Set the TLB Manager (never use it)
	SYSCALL(SPECTLBVEC, (int)&pstate[1], (int)&pstate[1], 0);
	
	// Request the CPU clock time again
	time = SYSCALL(GETCPUTIME, 0, 0, 0);
	
	// Set the Trap Manager (never use it)
	SYSCALL(SPECPRGVEC, (int)&pstate[1], (int)&pstate[1], 0);
	
	SYSCALL(VERHOGEN, ENDSERVICE, 0, 0);
	
	SYSCALL(TERMINATEPROCESS, -1, 0, 0);			
	
	PANIC();			
}


void proc_interactive1(int index) {
	
	int i, val=0;
	
	int key=BASEKEYSEM + index;
	
	SYSCALL(CREATEBROTHER, (int)&pstate[index+1], HIGH_PRIORITY, 0);	
	
	SYSCALL(PASSEREN, key, 0, 0);
	
	// Request the CPU clock time 
	cpu_t time = SYSCALL(GETCPUTIME, 0, 0, 0);
	
	// Perform some meaningless computation
	for (i=0; i< MINLOOP; i++)
		val=val+(i*2)/2;
	
	
	SYSCALL(VERHOGEN, key, 0, 0);
	
	SYSCALL(VERHOGEN, ENDINTERACT, 0, 0);
	
	SYSCALL(TERMINATEPROCESS, -1, 0, 0);
	PANIC();
	
}



void proc_interactive2(int index) {
	
	int key=BASEKEYSEM + (index-1);		
	
	SYSCALL(VERHOGEN, key, 0, 0);
	
	cpu_t time1 = 0;
	cpu_t time2 = 0;
	
	/* loop until we are delayed at least half of clock V interval */
	while ((time2 - time1) < (CLOCKINTERVAL >> 1) )  {
		time1 = GET_TODLOW;			/* time of day     */
		SYSCALL(WAITCLOCK, 0, 0, 0);
		time2 = GET_TODLOW;			/* new time of day */
	}
	
	SYSCALL(PASSEREN, key, 0, 0);
	
	SYSCALL(VERHOGEN, ENDINTERACT, 0, 0);
	
	SYSCALL(TERMINATEPROCESS, -1, 0, 0);
	
	PANIC();
	
}



