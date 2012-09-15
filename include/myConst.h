#include "const.h"
#include "uMPStypes.h"

/* Constants */
/* TODO: trovare  modo di leggere num CPU da emulatore ma potendolo
 * usare come costante (ad esempio per dimensionare gli array) */
#define NCPU_ADDR 0x10000500
#define NUM_CPU 4
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
#define TIME_SLICE 5*1000*(*(memaddr *)BUS_TIMESCALE) /* espresso in ms, 1ms = BUS_TIMESCALE*1000 clock_ticks */

/* Costanti di utilità */
#define EXCEPTION_STATUS (STATUS_TE & ~(STATUS_IEp|STATUS_INT_UNMASKED|STATUS_VMp))
#define PROCESS_STATUS (STATUS_IEc|STATUS_TE|STATUS_INT_UNMASKED)
#define STATUS_TE 0x08000000
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
#define SCHEDULER_LOCK MAX_DEVICES+MAXPROC-1


