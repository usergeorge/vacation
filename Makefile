# Makefile for vacation 1.2.5
# 03/22/00 Sean Rima (thecivvie@softhome.net)
# 07/28/95 Harald Milz (hm@seneca.ix.de)
# 04/02/97 Harald Milz (hm@seneca.muc.de)

# Version 1.2.7
# 12/21/98 Harald Milz (hm@seneca.muc.de)
#
SHELL		= /bin/sh
CC		= gcc
ARCH           = $(shell uname -m)
#
ifeq "$(ARCH)" "alpha"
  CFLAGS       = $(RPM_OPT_FLAGS) -Wall
else
ifeq "$(ARCH)" "ppc"
  CFLAGS       = $(RPM_OPT_FLAGS) -fsigned-char -Wall
else
  CFLAGS       = $(RPM_OPT_FLAGS) -m486 -Wall
endif
endif
LFLAGS         = -Xlinker -warn-common

LIBS		= -lgdbm
LINT		= lint -abchuvx
LLIBS		=	

# where things go
BINDIR		= /usr/bin
VACATION        = $(BINDIR)/vacation
VACLOOK         = $(BINDIR)/vaclook
MANDIR		= /usr/man/man
MANEXT1		= 1

VERSION 	= 1
SUBVERSION 	= 2
PATCHLEVEL	= 7

# what are we making
SRC		= vacation.c
OBJ		= vacation.o 
BIN		= vacation

# what we are packaging
PACKAGE		= ChangeLog Makefile README tzfile.h \
		  vacation vacation.c vacation.h *.man COPYING \
		  contrib vacation-1.2.6.lsm patches OLD
TGZFILE		= vacation-$(VERSION).$(SUBVERSION).$(PATCHLEVEL).tar.gz

# rules

all:	$(BIN)

install:  all
	install -s -m 755 $(BIN) $(VACATION)
	install -m 755 vaclook $(VACLOOK)
	./html2man.pl < vacation.html > $(MANDIR)$(MANEXT1)/vacation.$(MANEXT1)
	install -m 444 vaclook.man $(MANDIR)$(MANEXT1)/vaclook.$(MANEXT1)

vacation:	$(SRC)
	$(CC) $(CFLAGS) $(PFLAGS) $(LFLAGS) -D_PATH_VACATION=\"$(VACATION)\" -o $(BIN) $(SRC) $(LIBS)

courier:	$(SRC)
	$(CC) $(CFLAGS) $(PFLAGS) $(LFLAGS) -DCOURIER -D_PATH_VACATION=\"$(VACATION)\" -o $(BIN) $(SRC) $(LIBS)

debug:	$(SRC)
	$(CC) $(CFLAGS) -DDEBUG $(LFLAGS) -o $(BIN) $(SRC) $(LIBS)

clean:
	rm -f *.o core *.out Makefile.old

clobber: clean
	rm -f $(BIN)

package: all
	strip vacation
	tar cvf - $(PACKAGE) | gzip > ../$(TGZFILE)

