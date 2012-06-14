#include "myconst.h"
#include "listx.h"
#include "types11.h"

/* Globali - comuni a tutte le CPU */
int procs; /* contatore dei processi (per tutte le CPU) */
int softprocs; /* contatore dei processi bloccati su I/O */

/* Specifiche per ogni CPU */
pcb_t *currentproc[NUM_CPU]; /* puntatore al processo in esecuzione attuale */
struct list_head readyQueue[NUM_CPU]; /* coda dei processi in stato ready */

