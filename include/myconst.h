/* Constants */
/* TODO: trovare  modo di leggere num CPU da emulatore ma potendolo
 * usare come costante (ad esempio per dimensionare gli array) */
#define NCPU_ADDR 0x10000500
#define NUM_CPU 4

/* Dati riguardanti i pcb_t */
#define MAX_PCB_PRIORITY		10
#define MIN_PCB_PRIORITY		0
#define DEFAULT_PCB_PRIORITY		5
#define TIME_SLICE 5

/* Costanti di utilità */
#define STATUS_TE 0x08000000
#define RESET 0
#define PASS 0 /* per CAS */
#define FORBID 1 /* per CAS */

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

/* identificativi New e Old Area generici */
#define NUM_AREAS 8
#define NEW_SYSBP 0
#define NEW_TRAP 2
#define NEW_TLB 4
#define NEW_INTS 6
#define OLD_SYSBP 1
#define OLD_TRAP 3
#define OLD_TLB 5
#define OLD_INTS 7
