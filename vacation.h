/* vacation.h 
** 00/01/11 Sean Rima (thecivvie@softhome.net)
*/

#ifndef _VACATION_H
#define _VACATION_H

#define _PATH_SED "/bin/sed"        /* which is not mentioned in paths.h */

#define	MAXLINE	1024			/* max line from mail header */
#define	VDB	".vacation.db"		/* dbm's database */
#define	VMSG	".vacation.msg"		/* vacation message */
#define FWD     ".forward"              /* forward file */

typedef struct alias {
	struct alias *next;
	char *name;
} ALIAS;
ALIAS *names;

GDBM_FILE db;

char from[MAXLINE];
char subject[MAXLINE];
char replyto[MAXLINE];
int jflag, rflag;                      /* for the `-j' and `-r' options */

void readheaders(void);
void usage(void);
void setinterval(time_t interval);
int recent(void);
void setreply(void);
void sendmessage(char *myname, char *myrealname);
int junkmail(void);
int nsearch(register char *name, register char *str);
void initialize(char *path, char*myname);
#ifdef DEBUG
void printd (char *message);
char logline[MAXLINE];
#endif

#endif
