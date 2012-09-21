#ifndef MYCONST
#define MYCONST

#include "const.h"
#include "uMPStypes.h"

/* Constants */
/* TODO: trovare  modo di leggere num CPU da emulatore ma potendolo
 * usare come costante (ad esempio per dimensionare gli array) */
#define NCPU_ADDR 0x10000500
#define NUM_CPU 1
/* #define NUM_CPU (int)*(int*)NCPU_ADDR	 */

#define TIME_SCALE BUS_TIMESCALE

/* Interrupt Delivery Controller Processor Interface Register Map
 * Sono separate per ogni CPU ma condividono lo stesso address-space */
#define INBOX_ADDR 0x10000400
#define OUTBOX_ADDR 0x10000404
#define TPR_ADDR 0x10000408
#define BIOS_RES1_ADDR 0x1000040c
#define BIOS_RES2_ADDR 0x10000410

/* Interrupt Routing Tables (formula pag.61 princOfOperations.pdf) */
#define DEV_IRT_BASE 0x10000300 /* Linea 2 */
#define IRT_ENTRY(line,dev) \
	(line == 2? DEV_IRT_BASE:DEV_IRT_BASE+(WORD_SIZE*((line-2)*8+dev)))
#define DYN_IRT_MASK 0x10000000 

/* Interrupt Lines (custom) */
#define INT_PLT 1


/* Dati riguardanti i pcb_t e lo scheduler */
#define MAX_PCB_PRIORITY		10
#define MIN_PCB_PRIORITY		0
#define DEFAULT_PCB_PRIORITY		5
#define TIME_SLICE SCHED_TIME_SLICE*(*(memaddr *)BUS_TIMESCALE) /* espresso in ms, 1ms = BUS_TIMESCALE*1000 clock_ticks */

/* Costanti di utilità */
#define PROCESS_STATUS (STATUS_IEp|STATUS_INT_UNMASKED|STATUS_TE) // TODO Riabilitare interrupt terminali per gestirli correttamente!
#define EXCEPTION_STATUS ~(STATUS_IEc|STATUS_VMc|STATUS_INT_UNMASKED|STATUS_KUc)|STATUS_TE
#define STATUS_TE 0x08000000 /* PLT */
#define STATUS_LINE_7 0x8000 // TERMINAL
#define STATUS_LINE_1 0x200  // PLT
#define STATUS_LINE_2 0x400 // INTERVAL TIMER (PCT)
#define RESET 0
#define PASS 1 /* per CAS */
#define FORBID 0 /* per CAS */
#define NUM_LINES 8 /* Numero delle linee di interrupt totali */

/* Organizzazione della memoria */
#define ROM_RES_FRAME_START 0x20000000
#define ROM_RES_FRAME_END 0x20000FFF
#ifndef QPAGE
	#define QPAGE 1024
#endif

/* Memoria usabile dai processi (i primi NUMCPU frame sono riservati agli handler) */
#define PFRAMES (((*(memaddr *)BUS_INSTALLEDRAM)/FRAME_SIZE)-2) /* 1 Rom Res. Frame + 1 Frame per gli stack degli handler */
#define SFRAMES_START RAMTOP-(NUM_CPU*(FRAME_SIZE)) /* Frame per lo scheduler */
#define PFRAMES_START SFRAMES_START-(NUM_CPU*FRAME_SIZE) /* Frame per i processi */

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

/* Costanti per lo scheduler */

/* In MAX_DEVICES è già considerato un semaforo in più per il PCT.
 * aggiungiamo un semaforo per lo scheduler */
#define NUM_SEMAPHORES MAX_DEVICES + MAXPROC + 1
#define SCHEDULER_SEMLOCK NUM_SEMAPHORES -1 /* Il lock per lo scheduler è l'ultimo */
#define PCT_SEM NUM_SEMAPHORES -2 /* Il lock per il PCT è il penultimo */
#define DEV_SEM_START MAXPROC /* L'indice di dove iniziano i semafori/lock per i device esterni */

/* ******************************************** MACRO PER I/O ******************************************** */
#define DEV_ADDR_INIT 0x10000050
#define DEV_ADDR_BASE(LINENO, DEVNO)	DEV_ADDR_INIT+((LINENO-3)*0x80) + (DEVNO*0x10)

/* Macro per terminali */
#define TERMSTATMASK	0xFF
#define TX_COMMAND	0x02
#define RX_COMMAND	0x01
#define ACK		1
#define RESET	0

/* Queste Macro permettono di ottenere l'N-esimo byte di da una word (da sx a dx)
 * ad es. per ottenere lo status byte dallo status field di un controller */
#define FIRST 0xFF
#define SECOND 0xFF00
#define THIRD 0xFF0000
#define FOURTH 0xFF000000
#define GET_BYTE(N, WHAT)	(N & WHAT)

/* Macro per ottenere gli indici di semafori/lock relativi ai diversi device (in caso di terminale c'è da specificare se R o W) */
#define GET_TERM_SEM(LINENO, DEVNO, READ)	(MAXPROC + ((LINENO - INT_LOWEST) * DEV_PER_INT) + DEVNO + (READ * DEV_PER_INT))
#define GET_TERM_STATUS(LINENO, DEVNO, READ)	(((LINENO - INT_LOWEST) * DEV_PER_INT) + DEVNO + (READ * DEV_PER_INT))

#define GET_DEV_SEM(LINENO, DEVNO)	(MAXPROC + ((LINENO - INT_LOWEST) * DEV_PER_INT) + DEVNO)
#define GET_DEV_STATUS(LINENO, DEVNO)	(((LINENO - INT_LOWEST) * DEV_PER_INT) + DEVNO)

/* Macro per ottenere il numero del device con interrupt pendenti */
#define GET_PENDING_INT_BITMAP(LINENO)		*(U32 *)(PENDING_BITMAP_START + (WORD_SIZE * (LINENO - 3)))
#define IS_NTH_BIT_SET(N, WORD)	((1 << N) & WORD)

/* Macro per ottenere un puntatore allo state_t rappresentante la old area relativa al processore che la esegue */
#define GET_OLD_SYSBK()	(getPRID() == 0)? (state_t *)SYSBK_OLDAREA : &areas[getPRID()][SYSBK_OLDAREA_INDEX];
#define GET_OLD_INT()	(getPRID() == 0)? (state_t *)INT_OLDAREA : &areas[getPRID()][INT_OLDAREA_INDEX];
#define GET_OLD_PGMTRAP()	(getPRID() == 0)? (state_t *)PGMTRAP_OLDAREA : &areas[getPRID()][PGMTRAP_OLDAREA_INDEX];
#define GET_OLD_TLB()	(getPRID() == 0)? (state_t *)TLB_OLDAREA : &areas[getPRID()][TLB_OLDAREA_INDEX];

#endif
