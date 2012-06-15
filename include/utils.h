/** Utilities, constants and tools */
#include "types11.h"

/* Funzione ausiliaria per la "dummy initialization" degli stati delle CPU a 0 */
void cleanState(state_t *state);

/* Funzione per l'inizializzazione dello stato di un pcb_t dai parametri */
pcb_t *initPcbState(pcb_t *pcb, U32 status, memaddr pc_epc, U32 reg_sp);

/* debugging functions */
void debug(int row, int var); /* value of the var variable at the row line in source */

/* term printing */
void addokbuf(char *strp);
void adderrbuf(char *strp);

/* converts int to string */
char *itoa(int i);
/* printf like funcion, replace % with int */
void printn(char* s, int n);
