/* Si occupa di avviare tutte le CPU (populando le relative new area) specificate nel valore NUM_CPU */
#include "myconst.h"

/* Questa funzione inizializza semplicemente tutte le readyQueue di 
 * tutte le CPU in modo da non incorrere in errori nell'utilizzo delle
 * funzioni di listx.h */
void initReadyQueues();

/* Funzione che serve per inizializzare le CPU > 0 */
void initCpus();

/* Questa funzione si occupa di inizializzare le New/Old area in base
 * alla CPU (CPU0 avrà ROM Reserved Frame, le altre un array dedicato) */
void initAreas();

