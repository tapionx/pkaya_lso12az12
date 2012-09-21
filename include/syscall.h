#ifndef __SYSCALL
#define __SYSCALL

/* Handlers delle 11 System Call */

/* System Call #1  : Create Process
 * genera un processo figlio del processo chiamante
 * statep   = indirizzo fisico del nuovo processo
 * priority = priorità del nuovo processo
 * return  -> 0 ok / -1 errore (coda piena PLB)
 */
int create_process(state_t *statep, int priority);

/* System Call #2  : Create Brother
 * genera un processo fratello del processo chiamante
 * statep   = indirizzo del nuovo processo
 * priority = priorità del nuovo processo
 * return  -> 0 ok / -1 errore
 */
int create_brother(state_t *statep, int priority);

/* System Call #3  : Terminate Process
 * termina il processo corrente e tutti i discendenti
 * LIBERARE TUTTE LE RISORSE
 */
void terminate_process();

/* System Call #4  : Verhogen
 * esegue la V sul semaforo con chiave semKey
 * il primo processo in coda sul semaforo va in esecuzione
 */
void verhogen(int semKey);

/* System Call #5  : Passeren
 * esegue la P sul semaforo con chiave semKey
 * il processo che ha chiamato la syscall si mette in attesa
 */
void passeren(int semKey);

/* System Call #6  : Get CPU Time
 * restituisce il tempo CPU usato dal processo in millisecondi
 * -> IL KERNEL DEVE TENERE LA CONTABILITA DEL TEMPO CPU DEI PROCESSI
 */
int get_cpu_time();

/* System Call #7  : Wait Clock
 * esegue una P sul semaforo dello PSEUDO CLOCK TIMER (PCT)
 * il PCT esegue una V ogni 100 millisecondi e sblocca tutti i processi
 */
void wait_clock();

/* System Call #8  : Wait I/O
 * esegue una P sul semaforo del device specificato dai parametri
 * intNo = linea di interrupt
 * dnum  = numero del device
 * waitForTermRead = operazione di terminal read/write
 * return -> status del device
 */
int wait_io(int intNo, int dnum, int waitForTermRead);

/* System Call #9  : Specify PRG State Vector
 * definire handler personalizzato per Program Trap per il processo corrente
 * oldp = indirizzo della custom OLDAREA
 * newp = indirizzo della custom NEWAREA
 */
void specify_prg_state_vector(state_t *oldp, state_t *newp);

/* System Call #10 : Specify TLB State Vector
 * definire handler personalizzato per TLB Exception per il processo corrente
 * oldp = indirizzo della custom OLDAREA
 * newp = indirizzo della custom NEWAREA
 */
void specify_tlb_state_vector(state_t *oldp, state_t *newp);

/* System Call #11 : Specify SYS State Vector
 * definire handler personalizzato per SYSCALL/BreakPoint per il processo corrente
 * oldp = indirizzo della custom OLDAREA
 * newp = indirizzo della custom NEWAREA
 */
void specify_sys_state_vector(state_t *oldp, state_t *newp);

#endif
