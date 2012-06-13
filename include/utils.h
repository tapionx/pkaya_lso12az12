/** Utilities, constants and tools */
void initCPU(int, state_t*, CPU*);
void initCPUs(state_t *, CPU *);

/* Funzione ausiliaria per la "dummy initialization" degli stati delle CPU */
void initState_t(state_t* newState);

/* debugging functions */
void debug(int row, int var); /* value of the var variable at the row line in source */

/* term printing */
void addokbuf(char *strp);
void adderrbuf(char *strp);

/* converts int to string */
char *itoa(int i);
/* converts int to strings and prints it */
void print(int n);
