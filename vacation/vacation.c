/*
 * Copyright (c) 1983, 1987 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef lint
char copyright[] =
"@(#) Copyright (c) 1983, 1987 Regents of the University of California.\n\
 All rights reserved.\n";
#endif /* not lint */

#ifndef lint
/*static char sccsid[] = "from: @(#)vacation.c	5.19 (Berkeley) 3/23/91";*/
static char rcsid[] __attribute__ ((unused)) = "$Id$";
#endif /* not lint */

/*
**  Vacation
**  Copyright (c) 1983  Eric P. Allman
**  Berkeley, California
*/

/*  ported to Linux and
**  Copyright (c) 1993-1998 Harald Milz (hm@seneca.muc.de)
*/

/*  some tiny patches recognizing/generating headers
**  Mark Seuffert (moak@pirate.de)
*/

/*  Raaah! Someone has changed the behaviour of vfork() in the kernel >= 2.2.x
**  We'll use fork instead, that works.
**  Patrik Schindler (poc@pocnet.net)
*/

#include <sys/param.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <gdbm.h>
#include <time.h>
#include <syslog.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <paths.h>
#include "tzfile.h"
#include "vacation.h"

/*
 *  VACATION -- return a message to the sender when on vacation.
 *
 *	This program is invoked as a message receiver.  It returns a
 *	message specified by the user to whomever sent the mail, taking
 *	care not to return a message too often to prevent "I am on
 *	vacation" loops.
 */

int main(int argc, char **argv)
{
  extern int optind, opterr;
  extern char *optarg;
  struct passwd *pw;
  ALIAS *cur;
  time_t interval;
  int ch, iflag, nflag;
  char *vacation;

  openlog ("vacation", LOG_PID, LOG_MAIL);  
  opterr = iflag = nflag = rflag = 0;
  interval = -1;
#ifdef _PATH_VACATION
  vacation = _PATH_VACATION;
#else
  vacation = argv[0];
#endif
  if (argc == 1)
    nflag = 1;
  while ((ch = getopt(argc, argv, "a:It:jr")) != EOF)
    switch((char)ch) {
    case 'a':			/* alias */
      if (!(cur = (ALIAS *)malloc((u_int)sizeof(ALIAS)))) {
	perror ("malloc");
	exit (-1);
      }
      cur->name = optarg;
      cur->next = names;
      names = cur;
      break;
    case 'I':			/* init the database */
      iflag = 1;
      break;
    case 't':
      if (isdigit(*optarg)) {
	interval = atol(optarg) * SECSPERDAY;  /* unit is `days' */
	if (interval < 0)
	  usage();
      }
      else
	interval = LONG_MAX;
      break;
    case 'j':		/* don't check To: and Cc: fields */
      jflag = 1;
      break;
    case 'r':               /* "Reply-To:" overrides "From:" */
      rflag = 1;
      break;
    case '?':
    default:
      usage();
      break;
    }
  argc -= optind;
  argv += optind;
  
  if (argc != 1) {
    if (!iflag && !nflag)
      usage();
    if (!(pw = getpwuid(getuid()))) {
      syslog(LOG_ERR,
	     "vacation: no such user uid %u.\n", getuid());
      exit(1);
    }
  }
  else if (!(pw = getpwnam(*argv))) {
    syslog(LOG_ERR, "vacation: no such user %s.\n", *argv);
    exit(1);
  }
  if (chdir(pw->pw_dir)) {
    syslog(LOG_NOTICE,
	   "vacation: no such directory %s.\n", pw->pw_dir);
    exit(1);
  }
  
  db = gdbm_open(VDB, 128, ((iflag || nflag) ? GDBM_NEWDB : GDBM_WRITER),
		 0644, NULL);
  if (!db) {
    syslog(LOG_NOTICE, "vacation: %s: %s\n", VDB, strerror(errno));
    exit(1);
  }

  if (interval != -1)
    setinterval(interval);

  if (iflag) {
    (void) gdbm_close(db);
    exit(0);
  }
	
  if (nflag) {
    (void) gdbm_close(db);
    initialize(vacation, pw->pw_name);
    exit(0);
  }

  if (!(cur = malloc((u_int)sizeof(ALIAS))))
    exit(1);
  cur->name = pw->pw_name;
  cur->next = names;
  names = cur;

#ifdef DEBUG
  printd ("next message");
#endif
  readheaders();
  if (!recent()) {
    setreply();
    (void) gdbm_close(db);
    sendmessage(pw->pw_name, pw->pw_gecos);
  } else
    (void) gdbm_close(db);
  exit(0);
  /* NOTREACHED */
}

/*
 * readheaders --
 *	read mail headers
 */
void readheaders(void)
{
  register ALIAS *cur;
  register char *p;
  int tome, cont;
  char buf[MAXLINE];
  char uucpfrom[MAXLINE];
//  char sender[MAXLINE];
//  char domain[MAXLINE];

    cont = tome = 0;
    while (fgets(buf, sizeof(buf), stdin) && *buf != '\n')
      switch(toupper(*buf)) {
      case 'F':         /* "From: " or "From "*/
        cont = 0;
        if (!strncasecmp(buf, "From:", 5)) {     /* "From:" */
          if ((p = index(buf, '>'))) {          /* address in <> ? */
            *p = '\0';                        /* let string end here */
            p = index(buf, '<') + 1;          /* and start copy here */
	}
	else {                              /* address isolated */
	  for (p = buf + 6; *p && *p != ' '; ++p);
	  *p = '\0';
	  p = buf + 6;                      /* start copy here */
	}
	(void) strcpy(from, p);             /* actual copy */
	if ((p = index(from, '\n')))          /* was there a NL ? */
	  *p = '\0';
#ifdef DEBUG
	sprintf (logline, "From: >%s<", from);
	printd (logline);
#endif
      }
      else if (!strncasecmp(buf, "From ", 5)) { /* "From " */  
	for (p = buf + 5; *p && *p != ' '; ++p);
	*p = '\0';
	(void)strcpy(uucpfrom, buf + 5);  /* this contains a bang path */
	if ((p = index(uucpfrom, '\n')))
	  *p = '\0';
#ifdef DEBUG
	sprintf (logline, "From >%s<", uucpfrom);
	printd (logline);
#endif
      }
      else 
	break;                              /* other "F" header */
      if (index(from, '@') > rindex(from, '.')) { /* domain addr. complete? */
	if ((p = rindex(uucpfrom, '!'))) {
	  strcpy(from, p+1);
	  strcat(from, "@");
	  *p = '\0';
	}
	else
	  exit (1);                         /* this should not occur */
	if ((p = rindex(uucpfrom, '!'))) 
	  strcat(from, p+1);
	else
	  strcat(from, uucpfrom);
      }
#ifdef DEBUG
      sprintf (logline, "From: now: >%s<", from);
      printd (logline);
#endif
      if (junkmail())
	exit(0);
      break;
    case 'R':		/* "Reply-To: " */
      cont = 0;
      if (!strncasecmp(buf, "Reply-To:", 9)) { /* much simpler than From: */
	for (p = buf + 10; *p && *p != ' '; ++p);
	*p = '\0';
	(void)strcpy(replyto, buf + 10);
	if ((p = index(replyto, '\n')))
	  *p = '\0';
#ifdef DEBUG
	sprintf (logline, "Reply-To: >%s<", replyto);
	printd (logline);
#endif
	if (junkmail())
	  exit(0);
      }
      break;
    case 'S':		/* "Subject" */
      cont = 0;
      if (!strncasecmp(buf, "Subject:", 8)) {
	(void)strcpy(subject, buf + 9);
	if ((p = index(subject, '\n')))
	  *p = '\0';
#ifdef DEBUG
	sprintf (logline, "Subject found: >%s<", subject);
	printd (logline);
#endif
	if (junkmail())
	  exit(0);
      }
      break;
    case 'P':		/* "Precedence:" */
      cont = 0;
      if (strncasecmp(buf, "Precedence", 10) ||
	  (buf[10] != ':' && buf[10] != ' ' && buf[10] != '\t'))
	break;
      if (!(p = index(buf, ':')))
	break;
      while (*++p && isspace(*p));
      if (!*p)
	break;
      if (!strncasecmp(p, "junk", 4) ||
	  !strncasecmp(p, "list", 4) ||
	  !strncasecmp(p, "bulk", 4))
	exit(0);
      break;
    case 'C':		/* "Cc:" */
      if (strncasecmp(buf, "Cc:", 3))
	break;
      cont = 1;
      goto findme;
    case 'T':		/* "To:" */
      if (strncasecmp(buf, "To:", 3))
	break;
      cont = 1;
      goto findme;
    default:
      if (!isspace(*buf) || !cont || tome) {
	cont = 0;
	break;
      }
    findme: 
      for (cur = names; !tome && cur; cur = cur->next)
	tome += nsearch(cur->name, buf);
    }
  if (!jflag && !tome)
    exit(0);
  if (!*from) {
    syslog(LOG_NOTICE, "vacation: no \"From:\" line.\n");
    exit(2);
  }
  if (rflag && (*replyto != 0x0))
    strcpy (from, replyto);
#ifdef DEBUG
  sprintf (logline, "From: %s  Reply-To: %s\n", from, replyto);
  printd (logline);
#endif
}


/*
 * nsearch --
 *	do a nice, slow, search of a string for a substring.
 */
int nsearch(register char *name, register char *str)
{
  register int len;
  register char c;
  c=tolower(*name);

  for (len = strlen(name); *str; ++str)
    if (tolower(*str) == c && !strncasecmp(name, str, len))
      return(1);
  return(0);
}

/*
 * junkmail --
 *	read the header and return if automagic/junk/bulk mail
 */
int junkmail(void)
{
  static struct ignore {
    char *name;
    int	len;
  } ignore[] = {
  /* I am not sure whether this list is near to complete. Maybe a 
     separate file containing all the exceptions would be more
     handy than hardwiring this into the code. hm */
    {"-request", 8},		{"postmaster", 10},	{"uucp", 4},
    {"mailer-daemon", 13},	{"mailer", 6},		{"-relay", 6},
    {"-activists", 10},           {"-programmers", 12},     {NULL, 0},
  };
  register struct ignore *cur;
  register int len;
  register char *p;

  /*
   * This is mildly amusing, and I'm not positive it's right; trying
   * to find the "real" name of the sender, assuming that addresses
   * will be some variant of:
   *
   * From site!site!SENDER%site.domain%site.domain@site.domain
   */
  if (!(p = index(from, '%')))
    if (!(p = index(from, '@'))) {
      if ((p = rindex(from, '!')))
	++p;
      else
	p = from;
      for (; *p; ++p);
    }
  len = p - from;
  for (cur = ignore; cur->name; ++cur)
    if (len >= cur->len &&
	!strncasecmp(cur->name, p - cur->len, cur->len))
      return(1);
  return(0);
}

#define	VIT	"__VACATION__INTERVAL__TIMER__"

/*
 * recent --
 *	find out if user has gotten a vacation message recently.
 *	use bcopy for machines with alignment restrictions
 */
int recent(void)
{
  datum key, data;
  time_t then, next;
  
  /* get interval time */
  key.dptr = VIT;
  key.dsize = sizeof(VIT);
  data = gdbm_fetch(db, key);
  if (data.dptr == NULL)
    next = SECSPERDAY * DAYSPERWEEK;    /* default repeat time is one week */
  else
    bcopy(data.dptr, &next, sizeof(next));
  
	/* get record for this address */
  key.dptr = from;
  key.dsize = strlen(from);
  data = gdbm_fetch(db, key);
  if (data.dptr != NULL) {
    bcopy(data.dptr, &then, sizeof(then));
    if (next == LONG_MAX || then + next > time(NULL))
      return(1);
  }
  return(0);
}

/*
 * setinterval --
 *	store the reply interval
 */
void setinterval(time_t interval)
{
  datum key, data;
  
  key.dptr = VIT;
  key.dsize = sizeof(VIT);
  data.dptr = (char *) &interval;
  data.dsize = sizeof(interval);
  (void) gdbm_store(db, key, data, GDBM_REPLACE);
}

/*
 * setreply --
 *	store that this user knows about the vacation.
 */
void setreply(void)
{
  datum key, data;
  time_t now;

  key.dptr = from;
  key.dsize = strlen(from);
  (void)time(&now);
  data.dptr = (char *) &now;
  data.dsize = sizeof(now);
  (void) gdbm_store(db, key, data, GDBM_REPLACE);
}

/*
 * sendmessage --
 *	exec sendmail to send the vacation file to sender
 */
void
sendmessage(char *myname, char *myrealname)
{
	register char *p;
	FILE *mfp, *sfp;
	int i;
	int pvect[2];
	char buf[MAXLINE];
	char line[MAXLINE];

	if ((p = index(myrealname, ','))) {         /* realname with , ? */
		*p = '\0';                        /* let string end here */
	}
	if (index(myrealname, '(') || index(myrealname, ')')) {
		*buf='\"';                        /* put " around realnames with () */
		(void) strcpy(buf+1, myrealname);
		strcat (buf,"\"");
	} else {                                  /* this is a normal realname */
		(void) strcpy(buf, myrealname);
	}
#ifdef DEBUG
	sprintf (logline, "sendmessage: using realname >%s<", buf);
	printd (logline);
#endif

	mfp = fopen(VMSG, "r");
	if (mfp == NULL) {
		syslog(LOG_NOTICE, "vacation: no ~%s/%s file.\n", myname, VMSG);
		exit(1);
	}
	if (pipe(pvect) < 0) {
		syslog(LOG_ERR, "vacation: pipe: %s", strerror(errno));
		exit(1);
	}
	i = fork();
	if (i < 0) {
		syslog(LOG_ERR, "vacation: fork: %s", strerror(errno));
		exit(1);
	}
	if (i == 0) {
		dup2(pvect[0], 0);
		close(pvect[0]);
		close(pvect[1]);
		fclose(mfp);
		execl(_PATH_SENDMAIL, "sendmail", "-f", myname, "-F", buf, from, NULL);
		syslog(LOG_ERR, "vacation: can't exec %s: %s",
			_PATH_SENDMAIL, strerror(errno));
		exit(1);
	}
	close(pvect[0]);
	sfp = fdopen(pvect[1], "w");
	fprintf(sfp, "To: %s\n", from);
	while (fgets(buf, sizeof buf, mfp)) {
		char	*sp, *fromp, *subjp, *nextp;

		subjp = strstr(buf, "$SUBJECT");
#ifdef DEBUG
		if (subjp) {
			sprintf (logline, "sendmessage: found $SUBJECT %s\n", subjp);
			printd (logline);
		}
#endif
		fromp = strstr(buf, "$FROM");
		for (sp = buf; subjp || fromp; sp = nextp) {
			if (subjp && !(fromp && fromp < subjp)) {
				*subjp = '\0';
				nextp = subjp + 8;
				sprintf (line, "%s%s", sp, subject);
				subjp = NULL;
			} else {
				*fromp = '\0';
				nextp = fromp + 5;
				sprintf (line, "%s%s", sp, from);
				fromp = NULL;
			}
			fputs(line, sfp);
		}
		fputs(sp, sfp);
	}
	fclose(mfp);
	fclose(sfp);
}

void usage(void)
{
  puts ("usage: vacation [ -I ]");
  puts ("or:    vacation [ -j ] [ -a alias ] [ -tN ] [ -r ] login");
  exit(1);
}


void initialize (char *path, char *myname)
{
  char *editor;
  char ebuf[PATH_MAX];
  FILE *message;
  FILE *forward;

  if (((forward = fopen (FWD, "w")) == NULL)) {
    fprintf (stderr, "vacation: cannot open %s\n", FWD);
    exit (1);
  }
  fprintf (forward, "\\%s, \"|%s %s\"\n", myname, path, myname);
  fclose (forward);

  if (((editor = getenv ("VISUAL")) == NULL))
    if (((editor = getenv ("EDITOR")) == NULL))
      editor = _PATH_VI;
#ifdef DEBUG
	sprintf (logline, "editor: %s\n", editor);
	printd (logline);
#endif
        
  if (((message = fopen (VMSG, "w")) == NULL)) {
    fprintf (stderr, "vacation: cannot open %s\n", VMSG);
    exit (1);
  }
  fprintf (message, "Subject: away from my mail\n\n");
  fprintf (message, "I will not be reading my mail for a while.\n");
  fprintf (message, "Your mail concerning \"$SUBJECT\"\n");
  fprintf (message, "will be read when I'm back.\n");
  fclose (message);
  sprintf (ebuf, "%s %s", editor, VMSG);
#ifdef DEBUG
	sprintf (logline, "calling editor with \"%s\"\n", ebuf);
	printd (logline);
#endif	
  system (ebuf);
  exit (0);
}


#ifdef DEBUG
void printd (char *message)
{ 
  FILE *file;
  time_t now;
  char *timestr;
  char *p;

  time (&now);
  timestr = ctime (&now);
  if (p = index(timestr, '\n')) 
    *p = '\0';
  file = fopen ("/tmp/vacation.log", "a");
  fprintf (file, "%s %s\n", timestr, message);
  fclose (file);
}
#endif
