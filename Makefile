# Makefile for vacation 1.2.2.a1
# 03/22/00 Sean Rima (thecivvie@softhome.net)
# 07/28/95 Harald Milz (hm@seneca.ix.de)
# 04/02/97 Harald Milz (hm@seneca.muc.de)

# Version 1.2.0
# 12/21/98 Harald Milz (hm@seneca.muc.de)
#

SHELL		= /bin/sh
CC		= gcc

#
CFLAGS		= -O2 -m486 
LFLAGS		= 

LIBS		= -lgdbm
LINT		= lint -abchuvx
LLIBS		=	

# where things go
BINDIR		= /usr/bin
MANDIR		= /usr/man/man
MANEXT1		= 1
MANEXT5		= 5

VERSION 	= 1
SUBVERSION 	= 2
PATCHLEVEL	= 0

# what are we making
SRC		= vacation.c
OBJ		= vacation.o 
BIN		= vacation

# what we are packaging
PACKAGE		= ChangeLog Makefile README tzfile.h \
		  vacation vacation.c vacation.h *.man COPYING \
		  contrib vacation-1.2.0.lsm patches OLD
TGZFILE		= vacation-$(VERSION).$(SUBVERSION).$(PATCHLEVEL).tar.gz

# rules

all:	$(BIN)

install:  all
	install -s -m 755 $(BIN) $(BINDIR)/$(BIN)
	install -m 444 vacation.man $(MANDIR)$(MANEXT1)/vacation.$(MANEXT1)
	install -m 444 forward.man $(MANDIR)$(MANEXT5)/forward.$(MANEXT5)

vacation:	$(SRC)
	$(CC) $(CFLAGS) $(LFLAGS) -o $(BIN) $(SRC) $(LIBS)

debug:	$(SRC)
	$(CC) $(CFLAGS) -DDEBUG $(LFLAGS) -o $(BIN) $(SRC) $(LIBS)

clean:
	rm -f *.o core *.out Makefile.old

clobber: clean
	rm -f vacation 

package: all
	strip vacation
	tar cvf - $(PACKAGE) | gzip > ../$(TGZFILE)

