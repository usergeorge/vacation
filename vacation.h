/*
 * Copyright (c) 1988 Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Arthur David Olson of the National Cancer Institute.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *                   
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR `AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 */

/* vacation.h 
** 00/01/11 Sean Rima (thecivvie@softhome.net)
*/

#ifndef _VACATION_H
#define _VACATION_H

#define VACVERS "1.2.7.1"
#define _PATH_SED "/bin/sed"	/* which is not mentioned in paths.h */

#define	MAXLINE	1024		/* max line from mail header */
#define	VDB	".vacation.db"	/* dbm's database */
#define	VMSG	".vacation.msg"	/* vacation message */
#define	OLDVMSG	".vacation.old"	/* previous vacation message */
#ifndef COURIER
#define FWD     ".forward"	/* forward file */
#define OLDFWD  ".forward.old"	/* previous forward file */
#else
#define FWD     ".courier"	/* forward file */
#define OLDFWD  ".courier.old"	/* previous forward file */
#endif

typedef struct alias
{
  struct alias *next;
  char *name;
} ALIAS;
ALIAS *names;

static GDBM_FILE db;

char from[MAXLINE];
char subject[MAXLINE];
char replyto[MAXLINE];
int jflag, rflag;		/* for the `-j' and `-r' options */

void readheaders (void);
void usage (void);
void setinterval (time_t interval);
int recent (void);
void setreply (void);
void sendmessage (char *myname, char *myrealname);
int junkmail (void);
int nsearch (register char *name, register char *str);
void initialize (char *path, char *myname);
#ifdef DEBUG
void printd (char *message);
char logline[MAXLINE];
#endif

/* OpenBSD string handling function prototypes */
size_t strlcpy (char *dst, const char *src, size_t siz);
size_t strlcat (char *dst, const char *src, size_t siz);

/* fetchmail RFC2822 parsing function */
char *nxtaddr (const char *);

#endif
