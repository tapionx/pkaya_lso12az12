#include "types11.h"
#include "myConst.h"
#include "libumps.h"
#include "pcb.e"
#include "asl.e"
#include "kernelVariables.h"

/**
 * Questa funzione si occupa di aggiungere un pcb_t (puntato da proc)
 * alla readyQueue dello scheduler. Agisce già in mutua esclusione
 * utilizzando la CAS e un intero globale per lo scheduler.
 */
void addReady(pcb_t *proc){
	lock(SCHEDULER_SEMLOCK);
	if (proc != NULL)
		insertProcQ(&(readyQueue), proc);
	free(SCHEDULER_SEMLOCK);
}

/**
 * SCHEDULER: Il sistema si divide in due livelli di astrazione principali:
 * System e Processes. Il primo definisce l'interazione tra le componenti
 * utili al kernel stesso (scheduler, handlers per interrupt, syscall ecc.)
 * e il secondo l'interazione tra le prime e i processi. Ogni volta che un
 * processo viene interrotto, per una syscall o un interrupt, lo scheduler
 * è chiamato dopo aver risolto l'interruzione (a meno di PANIC e simili).
 * Questo perché si ipotizza che all'interno della readyQueue i processi
 * con priorità più alta siano più "urgenti" e quindi da eseguire il prima
 * possibile (soprattutto se in attesa di interrupt da un device!). Il 
 * troncamento del TIME_SLICE di eventuali processi a priorità uguale o 
 * minore è un trade-off per ottenere uno scheduler round robin (con priorità)
 * più fair, efficente e manutenibile (poiché il codice critico si trova
 * quasi esclusivamente all'interno dello scheduler). Inoltre richiamando
 * lo scheduler come funzione e non come state_t da caricare si riduce
 * l'overhead di un ordine di grandezza (context switch vs chiamata).
 */
void scheduler(){
	/* TODO: ricordarsi di gestire i casi di deadlock (procCount e softProcCount) 
	 * e di modificare tutte le variabili del caso nelle syscall ecc. */
	 while(1){
		int cpuid = getPRID();
		lock(SCHEDULER_SEMLOCK);
		currentProcess[cpuid] = NULL;
		if(!emptyProcQ(&(readyQueue))){
			currentProcess[cpuid] = removeProcQ(&(readyQueue));
			/* Se il processo è da terminare ne liberiamo il pcb e lo scartiamo */
			if (currentProcess[cpuid]->wanted){
				freePcb(currentProcess[cpuid]);
				free(SCHEDULER_SEMLOCK);
				scheduler();
			}
			/* Settiamo il TIME_SLICE un istante prima di mandare il processo
			 * in esecuzione */
			setTIMER(TIME_SLICE);
			/* Setto lo pseudo-clock-timer (SOLO LA PRIMA VOLTA, si
			 * resetta ad ogni interrupt/V). Non possiamo attivarlo nel 
			 * main per motivi di coerenza: se due o più processi volessero
			 * usare il PCT per sincronizzarsi, e questo fosse inizializzato
			 * durante il main, il primo processo caricato partirebbe istantaneamente
			 * mentre gli altri rimarrebbero in attesa che il PCT scada di nuovo */
			if (!pctInit){
				SET_IT(SCHED_PSEUDO_CLOCK);
				pctInit = TRUE;
			}
			free(SCHEDULER_SEMLOCK);
			LDST(&(currentProcess[cpuid]->p_s));
		} else {
			free(SCHEDULER_SEMLOCK);
		}
	}
	WAIT();
}
