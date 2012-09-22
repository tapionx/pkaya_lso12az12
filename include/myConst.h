#include "const.h"
#include "uMPStypes.h"



/***************************** CPU */
/* Numero di processori presenti e massimo */
#define NCPU_ADDR 0x10000500
#define NUM_CPU 4 //*(U32 *)NCPU_ADDR
#define MAX_CPU 16

/* Inter Processor Communication */
#define INBOX_ADDR 0x10000400
#define OUTBOX_ADDR 0x10000404
#define TPR_ADDR 0x10000408
#define BIOS_RES1_ADDR 0x1000040c
#define BIOS_RES2_ADDR 0x10000410

/* Interrupt Routing Tables (pag.61 princOfOperations.pdf) */
#define DEV_IRT_BASE 0x10000300 /* Linea 2 */
#define IRT_ENTRY(line,dev) (line == 2? DEV_IRT_BASE:DEV_IRT_BASE+(WORD_SIZE*((line-2)*8+dev)))
#define DYN_IRT_MASK 0x10000000



/***************************** SCHEDULER */
#define MAX_PCB_PRIORITY	10
#define MIN_PCB_PRIORITY	0
#define DEFAULT_PCB_PRIORITY	5
#define TIME_SLICE SCHED_TIME_SLICE*(*(memaddr *)BUS_TIMESCALE) /* espresso in ms, 1ms = BUS_TIMESCALE*1000 clock_ticks */



/***************************** STATUS */
#define NULL_STATUS 0
/* SCHEDULER STATUS: Kernel mode, VM off, Interrupt disabilitati ma non mascherati (per la WAIT), PLT disabilitato */
#define SCHEDULER_STATUS NULL_STATUS | STATUS_INT_UNMASKED & ~(STATUS_KUp|STATUS_VMp|STATUS_IEp|STATUS_TE)
/* EXCEPTION STATUS: Interrupt mascherati, VM off, PLT on e Kernel mode */
#define EXCEPTION_STATUS NULL_STATUS | STATUS_TE & ~(STATUS_VMc|STATUS_KUc|STATUS_INT_UNMASKED)
#define STATUS_TE 0x08000000 /* PLT */
#define STATUS_LINE_7 0x8000 // Mask terminal
#define STATUS_LINE_1 0x200  // Mask PLT
#define STATUS_LINE_2 0x400 // Mask Interval Timer (PCT)



/***************************** CAS E MUTEX */
#define PASS 1
#define FORBID 0 



/***************************** MEMORY */
#define SFRAMES_START RAMTOP-(NUM_CPU*(FRAME_SIZE)) /* Frame iniziale per gli scheduler */
#define QPAGE FRAME_SIZE/4



/***************************** SYSCALL */
/* NUMERI DELLE SYSTEM CALL */

#define CREATEPROCESS		1
#define CREATEBROTHER		2
#define TERMINATEPROCESS	3
#define VERHOGEN			4
#define PASSEREN			5
#define GETCPUTIME			6
#define WAITCLOCK			7
#define WAITIO				8
#define SPECPRGVEC			9
#define SPECTLBVEC			10
#define SPECSYSVEC			11



/***************************** NEW AND OLD AREAS */
/* identificativi New e Old Area generici (l'ordine è come da manuale) */
#define NUM_AREAS 8

#define INT_OLDAREA_INDEX 0
#define INT_NEWAREA_INDEX 1
#define TLB_OLDAREA_INDEX 2
#define TLB_NEWAREA_INDEX 3
#define PGMTRAP_OLDAREA_INDEX 4
#define PGMTRAP_NEWAREA_INDEX 5
#define SYSBK_OLDAREA_INDEX 6
#define SYSBK_NEWAREA_INDEX 7

/* Macro per ottenere un puntatore allo state_t rappresentante la old area relativa al processore che la esegue */
#define GET_OLD_SYSBK()	(getPRID() == 0)? (state_t *)SYSBK_OLDAREA : &areas[getPRID()][SYSBK_OLDAREA_INDEX];
#define GET_OLD_INT()	(getPRID() == 0)? (state_t *)INT_OLDAREA : &areas[getPRID()][INT_OLDAREA_INDEX];
#define GET_OLD_PGMTRAP()	(getPRID() == 0)? (state_t *)PGMTRAP_OLDAREA : &areas[getPRID()][PGMTRAP_OLDAREA_INDEX];
#define GET_OLD_TLB()	(getPRID() == 0)? (state_t *)TLB_OLDAREA : &areas[getPRID()][TLB_OLDAREA_INDEX];



/***************************** SEMAPHORES */
/* In MAX_DEVICES è già considerato un semaforo in più per il PCT, aggiungiamo un semaforo per lo scheduler */
#define NUM_SEMAPHORES MAX_DEVICES + MAXPROC + 1
#define SCHEDULER_SEMLOCK NUM_SEMAPHORES -1 /* Il lock per lo scheduler è l'ultimo */
#define PCT_SEM NUM_SEMAPHORES -2 /* Il lock per il PCT è il penultimo */



/***************************** I/O */
/* Generici */
#define DEV_ADDR_INIT 0x10000050
#define DEV_ADDR_BASE(LINENO, DEVNO)	DEV_ADDR_INIT+((LINENO-3)*0x80) + (DEVNO*0x10)
#define NUM_LINES 8 /* Numero delle linee di interrupt totali */

/* Terminali */
#define TERMSTATMASK	0xFF
#define TX_COMMAND	0x02
#define RX_COMMAND	0x01
#define ACK		1
#define RESET	0

/* Macro per ottenere il numero del device con interrupt pendenti */
#define GET_PENDING_INT_BITMAP(LINENO)		*(U32 *)(PENDING_BITMAP_START + (WORD_SIZE * (LINENO - 3)))

/* Macro per ottenere un puntatore allo state_t rappresentante la old area relativa al processore che la esegue */
#define GET_OLD_SYSBK()	(getPRID() == 0)? (state_t *)SYSBK_OLDAREA : &areas[getPRID()][SYSBK_OLDAREA_INDEX];
#define GET_OLD_INT()	(getPRID() == 0)? (state_t *)INT_OLDAREA : &areas[getPRID()][INT_OLDAREA_INDEX];
#define GET_OLD_PGMTRAP()	(getPRID() == 0)? (state_t *)PGMTRAP_OLDAREA : &areas[getPRID()][PGMTRAP_OLDAREA_INDEX];
#define GET_OLD_TLB()	(getPRID() == 0)? (state_t *)TLB_OLDAREA : &areas[getPRID()][TLB_OLDAREA_INDEX];

/* In const.h manca l'interrupt line del PLT */
#define INT_PLT 1

/* Macro per ottenere gli indici di semafori/lock relativi ai diversi device (in caso di terminale c'è da specificare se R o W) */
#define GET_TERM_SEM(DEVNO, READ)	(MAXPROC + ((INT_TERMINAL - INT_LOWEST) * DEV_PER_INT) + DEVNO + (READ * DEV_PER_INT))
#define GET_TERM_STATUS(DEVNO, READ)	(((INT_TERMINAL - INT_LOWEST) * DEV_PER_INT) + DEVNO + (READ * DEV_PER_INT))

#define GET_DEV_SEM(LINENO, DEVNO)	(MAXPROC + ((LINENO - INT_LOWEST) * DEV_PER_INT) + DEVNO)
#define GET_DEV_STATUS(LINENO, DEVNO)	(((LINENO - INT_LOWEST) * DEV_PER_INT) + DEVNO)


/***************************** UTILITIES */
/* Ritorna 1 se l'n-esimo bit della word è acceso, 0 altrimenti */
#define IS_NTH_BIT_SET(N, WORD)	((1 << N) & WORD)
