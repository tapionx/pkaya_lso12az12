/** Utilities, constants and tools */

/* Funzione ausiliaria per la "dummy initialization" degli stati delle CPU a 0 */
void cleanState(state_t *state);

/* debugging functions */
void debug(int row, int var); /* value of the var variable at the row line in source */

/* term printing */
void addokbuf(char *strp);
void adderrbuf(char *strp);

/* converts int to string */
char *itoa(int i);
/* printf like funcion, replace % with int */
void printn(char* s, int n);
