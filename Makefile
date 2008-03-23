# Makefile Authors
# Chris Samuel (chris@csamuel.org)
# Sean Rima (thecivvie@softhome.net)
# Harald Milz (hm@seneca.ix.de)
#
SHELL		= /bin/sh
CC		= gcc
ARCH           = $(uname -m)
#
# Default CFLAGS for all builds, architecture flags get appended below.
#
CFLAGS		= $(RPM_OPT_FLAGS) -Wall -D_FORTIFY_SOURCE=2
ifeq "$(ARCH)" "x86_64"
# Comment out the line below to build native 64-bit (will not read 32-bit dbs)
  CFLAGS       = $(CFLAGS) -m32
else
ifeq "$(ARCH)" "ppc"
  CFLAGS       = $(CFLAGS) -fsigned-char
else
ifeq "$(ARCH)" "ppc64"
  CFLAGS       = $(CFLAGS) -fsigned-char
endif
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
PATCHLEVEL	= 6

# what are we making
SRC		= vacation.c
OBJ		= vacation.o 
BIN		= vacation

# what we are packaging
PACKAGE		= ChangeLog Makefile README tzfile.h \
		  vacation vacation.c vacation.h *.man COPYING \
		  contrib vacation-1.2.2.lsm patches OLD
TGZFILE		= vacation-$(VERSION).$(SUBVERSION).$(PATCHLEVEL).tar.gz

# rules

all:	$(BIN)

install:  all
	install -s -m 755 $(BIN) $(VACATION)
	install -m 755 vaclook $(VACLOOK)
	install -m 444 vacation.man $(MANDIR)$(MANEXT1)/vacation.$(MANEXT1)
	install -m 444 vaclook.man $(MANDIR)$(MANEXT1)/vaclook.$(MANEXT1)

vacation:	$(SRC)
	$(CC) $(CFLAGS) $(PFLAGS) $(LFLAGS) -D_PATH_VACATION=\"$(VACATION)\" -o $(BIN) $(SRC) $(LIBS)

debug:	$(SRC)
	$(CC) $(CFLAGS) -DDEBUG $(LFLAGS) -o $(BIN) $(SRC) $(LIBS)

clean:
	rm -f *.o core *.out *~ Makefile.old vacation

clobber: clean
	rm -f $(BIN)

package: all
	strip vacation
	tar cvf - $(PACKAGE) | gzip > ../$(TGZFILE)

