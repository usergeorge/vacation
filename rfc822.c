/*****************************************************************************

NAME:
   rfc822.c -- code for slicing and dicing RFC822 mail headers

ENTRY POINTS:
   nxtaddr() -- parse the next address out of an RFC822 header
   reply_hack() -- append hostname to local header addresses 

THEORY:
   How to parse RFC822 headers in C. This is not a fully conformant
implementation of RFC822 or RFC2822, but it has been in production use
in a widely-deployed MTA (fetcmail) since 1996 without complaints.
Really perverse combinations of quoting and commenting could break it.

AUTHOR:
   Eric S. Raymond <esr@thyrsus.com>, 1997.  This source code example
is part of fetchmail and the Unix Cookbook, and are released under the
MIT license.

******************************************************************************/
#include  <stdio.h>
#include  <ctype.h>
#include  <string.h>
#include  <stdlib.h>

#ifndef lint
/*static char sccsid[] = "from: @(#)vacation.c  5.19 (Berkeley) 3/23/91";*/
static char rcsid[] __attribute__ ((unused)) =
  "$Id$";
#endif /* not lint */

#include  <unistd.h>
#ifdef DEBUG
static int verbose;
#endif /* DEBUG */
const char *program_name = "rfc822";

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define HEADER_END(p)	((p)[0] == '\n' && ((p)[1] != ' ' && (p)[1] != '\t'))

char *
reply_hack (char *buf /* header to be hacked */ ,
	    const char *host /* server hostname */ ,
	    size_t * length)
/* hack message headers so replies will work properly */
{
  char *from, *cp, last_nws = '\0', *parens_from = NULL;
  int parendepth, state, has_bare_name_part, has_host_part;

  if (strncasecmp ("From:", buf, 5)
      && strncasecmp ("To:", buf, 3)
      && strncasecmp ("Reply-To:", buf, 9)
      && strncasecmp ("Return-Path:", buf, 12)
      && strncasecmp ("Cc:", buf, 3)
      && strncasecmp ("Bcc:", buf, 4)
      && strncasecmp ("Resent-From:", buf, 12)
      && strncasecmp ("Resent-To:", buf, 10)
      && strncasecmp ("Resent-Cc:", buf, 10)
      && strncasecmp ("Resent-Bcc:", buf, 11)
      && strncasecmp ("Apparently-From:", buf, 16)
      && strncasecmp ("Apparently-To:", buf, 14)
      && strncasecmp ("Sender:", buf, 7)
      && strncasecmp ("Resent-Sender:", buf, 14))
    {
      return (buf);
    }

  /*
   * This is going to foo up on some ill-formed addresses.
   * Note that we don't rewrite the fake address <> in order to
   * avoid screwing up bounce suppression with a null Return-Path.
   */

  parendepth = state = 0;
  has_host_part = has_bare_name_part = FALSE;
  for (from = buf; *from; from++)
    {
#if defined ( DEBUG )
      if (verbose)
	{
	  printf ("state %d: %s", state, buf);
	  printf ("%*s^\n", from - buf + 10, " ");
	}
#endif /* DEBUG */
      if (state != 2)
	{
	  if (*from == '(')
	    ++parendepth;
	  else if (*from == ')')
	    --parendepth;
	}

      if (!parendepth && !has_host_part)
	switch (state)
	  {
	  case 0:		/* before header colon */
	    if (*from == ':')
	      state = 1;
	    break;

	  case 1:		/* we've seen the colon, we're looking for addresses */
	    if (!isspace ((unsigned char) *from))
	      last_nws = *from;
	    if (*from == '<')
	      state = 3;
	    else if (*from == '@' || *from == '!')
	      has_host_part = TRUE;
	    else if (*from == '"')
	      state = 2;
	    /*
	     * Not expanding on last non-WS == ';' deals with groupnames,
	     * an obscure misfeature described in sections
	     * 6.1, 6.2.6, and A.1.5 of the RFC822 standard.
	     */
	    else if ((*from == ',' || HEADER_END (from))
		     && has_bare_name_part
		     && !has_host_part && last_nws != ';')
	      {
		int hostlen;
		char *p;

		p = from;
		if (parens_from)
		  from = parens_from;
		while (isspace ((unsigned char) *from) || (*from == ','))
		  --from;
		from++;
		hostlen = strlen (host);
		for (cp = from + strlen (from); cp >= from; --cp)
		  cp[hostlen + 1] = *cp;
		*from++ = '@';
		memcpy (from, host, hostlen);
		from = p + hostlen + 1;
		has_host_part = TRUE;
	      }
	    else if (from[1] == '('
		     && has_bare_name_part
		     && !has_host_part && last_nws != ';' && last_nws != ')')
	      {
		parens_from = from;
	      }
	    else if (!isspace ((unsigned char) *from))
	      has_bare_name_part = TRUE;
	    break;

	  case 2:		/* we're in a string */
	    if (*from == '"')
	      {
		char *bp;
		int bscount;

		bscount = 0;
		for (bp = from - 1; *bp == '\\'; bp--)
		  bscount++;
		if (!(bscount % 2))
		  state = 1;
	      }
	    break;

	  case 3:		/* we're in a <>-enclosed address */
	    if (*from == '@' || *from == '!')
	      has_host_part = TRUE;
	    else if (*from == '>' && (from > buf && from[-1] != '<'))
	      {
		state = 1;
		if (!has_host_part)
		  {
		    int hostlen;

		    hostlen = strlen (host);
		    for (cp = from + strlen (from); cp >= from; --cp)
		      cp[hostlen + 1] = *cp;
		    *from++ = '@';
		    memcpy (from, host, hostlen);
		    from += hostlen;
		    has_host_part = TRUE;
		  }
	      }
	    break;
	  }

      /*
       * If we passed a comma, reset everything.
       */
      if ((from > buf && from[-1] == ',') && !parendepth)
	{
	  has_host_part = has_bare_name_part = FALSE;
	  parens_from = NULL;
	}
    }

  *length = strlen (buf);
  return (buf);
}

char *
nxtaddr (const char *hdr
	 /* header to be parsed, NUL to continue previous hdr */ )
/* parse addresses in succession out of a specified RFC822 header */
{
  static char address[BUFSIZ];
  static size_t tp;
  static const char *hp;
  static int state, oldstate;
#if defined ( DEBUG )
  static const char *orighdr;
#endif /* DEBUG */
  int parendepth = 0;

#define START_HDR	0	/* before header colon */
#define SKIP_JUNK	1	/* skip whitespace, \n, and junk */
#define BARE_ADDRESS	2	/* collecting address without delimiters */
#define INSIDE_DQUOTE	3	/* inside double quotes */
#define INSIDE_PARENS	4	/* inside parentheses */
#define INSIDE_BRACKETS	5	/* inside bracketed address */
#define ENDIT_ALL	6	/* after last address */

#define NEXTTP()	((tp < sizeof(address)-1) ? tp++ : tp)

  if (hdr)
    {
      hp = hdr;
      state = START_HDR;
#if defined ( DEBUG )
      orighdr = hdr;
#endif /* DEBUG */
      tp = 0;
    }

  for (; *hp; hp++)
    {
#if defined ( DEBUG )
      if (verbose)
	{
	  printf ("state %d: %s", state, orighdr);
	  printf ("%*s^\n", hp - orighdr + 10, " ");
	}
#endif /* DEBUG */

      if (state == ENDIT_ALL)	/* after last address */
	return (NULL);
      else if (HEADER_END (hp))
	{
	  state = ENDIT_ALL;
	  if (tp)
	    {
	      while (tp > 0 && isspace ((unsigned char) address[tp - 1]))
		tp--;
	      address[tp] = '\0';
	      tp = 0;
	      return (address);
	    }
	  return (NULL);
	}
      else if (*hp == '\\')	/* handle RFC822 escaping */
	{
	  if (state != INSIDE_PARENS)
	    {
	      address[NEXTTP ()] = *hp++;	/* take the escape */
	      address[NEXTTP ()] = *hp;	/* take following unsigned char */
	    }
	}
      else
	switch (state)
	  {
	  case START_HDR:	/* before header colon */
	    if (*hp == ':')
	      state = SKIP_JUNK;
	    break;

	  case SKIP_JUNK:	/* looking for address start */
	    if (*hp == '"')	/* quoted string */
	      {
		oldstate = SKIP_JUNK;
		state = INSIDE_DQUOTE;
		address[NEXTTP ()] = *hp;
	      }
	    else if (*hp == '(')	/* address comment -- ignore */
	      {
		parendepth = 1;
		oldstate = SKIP_JUNK;
		state = INSIDE_PARENS;
	      }
	    else if (*hp == '<')	/* begin <address> */
	      {
		state = INSIDE_BRACKETS;
		tp = 0;
	      }
	    else if (*hp != ',' && !isspace ((unsigned char) *hp))
	      {
		--hp;
		state = BARE_ADDRESS;
	      }
	    break;

	  case BARE_ADDRESS:	/* collecting address without delimiters */
	    if (*hp == ',')	/* end of address */
	      {
		if (tp)
		  {
		    address[NEXTTP ()] = '\0';
		    state = SKIP_JUNK;
		    tp = 0;
		    return (address);
		  }
	      }
	    else if (*hp == '(')	/* beginning of comment */
	      {
		parendepth = 1;
		oldstate = BARE_ADDRESS;
		state = INSIDE_PARENS;
	      }
	    else if (*hp == '<')	/* beginning of real address */
	      {
		state = INSIDE_BRACKETS;
		tp = 0;
	      }
	    else if (*hp == '"')	/* quoted word, copy verbatim */
	      {
		oldstate = state;
		state = INSIDE_DQUOTE;
		address[NEXTTP ()] = *hp;
	      }
	    else if (!isspace ((unsigned char) *hp))	/* just take it, ignoring whitespace */
	      address[NEXTTP ()] = *hp;
	    break;

	  case INSIDE_DQUOTE:	/* we're in a quoted string, copy verbatim */
	    address[NEXTTP ()] = *hp;
	    if (*hp == '"')
	      state = oldstate;
	    break;

	  case INSIDE_PARENS:	/* we're in a parenthesized comment, ignore */
	    if (*hp == '(')
	      ++parendepth;
	    else if (*hp == ')')
	      --parendepth;
	    if (parendepth == 0)
	      state = oldstate;
	    break;

	  case INSIDE_BRACKETS:	/* possible <>-enclosed address */
	    if (*hp == '>')	/* end of address */
	      {
		address[NEXTTP ()] = '\0';
		state = SKIP_JUNK;
		++hp;
		tp = 0;
		return (address);
	      }
	    else if (*hp == '<')	/* nested <> */
	      tp = 0;
	    else if (*hp == '"')	/* quoted address */
	      {
		address[NEXTTP ()] = *hp;
		oldstate = INSIDE_BRACKETS;
		state = INSIDE_DQUOTE;
	      }
	    else		/* just copy address */
	      address[NEXTTP ()] = *hp;
	    break;
	  }
    }

  return (NULL);
}

/* rfc822.c end */
