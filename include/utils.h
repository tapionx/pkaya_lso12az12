/** Utilities and tools */

extern char okbuf[2048];			/* sequence of progress messages */
extern char errbuf[128];			/* contains reason for failing */
extern char msgbuf[128];			/* nonrecoverable error message before shut down */

extern char *mp;

typedef unsigned int devreg;

/* term printing */
devreg termstat(memaddr *stataddr);
unsigned int termprint(char * str, unsigned int term);
void addokbuf(char *strp);
void adderrbuf(char *strp);

/* debugging functions */
void debug(int row, int var); /* value of the var variable at the row line in source */



