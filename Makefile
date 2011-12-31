# Makefile Authors
# Chris Samuel (chris@csamuel.org)
# Sean Rima (thecivvie@softhome.net)
# Harald Milz (hm@seneca.ix.de)
#
SHELL		= /bin/sh
CC		= gcc
ARCH		= $(shell uname -m)
OS		= $(shell uname -s)
# Some git logic for the gitpackage target.
GIT		= $(shell bash -c "type -p git")
ifeq "$(GIT)" ""
GITCOMMAND="echo The git command is not installed, or not in your PATH, sorry."
else
GITVERSION=$(shell test -d .git && git describe)
GITCOMMAND="git archive --prefix=$(GITVERSION)/ --output=../$(GITVERSION).tar.gz $(GITVERSION)"
endif
#
# Default CFLAGS for all builds, architecture flags get appended below.
CFLAGS		= $(RPM_OPT_FLAGS) -g -Wall
ifeq "$(ARCH)" "x86_64"
# Uncomment below for backwards compatibility of gdbm files.
#  CFLAGS       = $(CFLAGS) -m32
else
ifeq "$(ARCH)" "ppc"
  CFLAGS       = $(CFLAGS) -fsigned-char
else
ifeq "$(ARCH)" "ppc64"
  CFLAGS       = $(CFLAGS) -fsigned-char
endif
endif
endif

LFLAGS		= -Xlinker -warn-common
IFLAGS		= 

LIBS		= -lgdbm
LINT		= lint -abchuvx
LLIBS		=	

 
ifeq "$(OS)" "FreeBSD"
# PREFIX for FreeBSD
PREFIX		= $(DESTDIR)/usr/local
IFLAGS		= -I/usr/local/include
LIBS		= -L/usr/local/lib -lgdbm
else
# PREFIX for Linux
PREFIX		= $(DESTDIR)/usr
endif

# where things go
BINDIR		= $(PREFIX)/bin
VACATION        = $(BINDIR)/vacation
VACLOOK         = $(BINDIR)/vaclook
MANDIR		= $(PREFIX)/share/man
MANEXT1		= 1

VERSION 	= 1
SUBVERSION 	= 2
PATCHLEVEL	= 7

# what are we making
EXTRAS		= strlcpy.c strlcat.c rfc822.c
SRC		= vacation.c $(EXTRAS)
OBJ		= vacation.o 
BIN		= vacation
HDR		= vacation.h

# what we are packaging
PACKAGE		= ChangeLog Makefile README tzfile.h \
		  vacation vacation.c vacation.h *.man COPYING rfc822.c \
		  contrib vacation-1.2.6.lsm patches OLD strlcpy.c strlcat.c
TGZFILE		= vacation-$(VERSION).$(SUBVERSION).$(PATCHLEVEL).tar.gz

# rules

all:	$(BIN)

install:  all
	install -D -s -m 755 $(BIN) $(VACATION)
	install -D -m 755 vaclook $(VACLOOK)
	install -D -m 444 vacation-en.man $(MANDIR)/man$(MANEXT1)/vacation.$(MANEXT1)
	install -D -m 444 vacation-de.man $(MANDIR)/de/man$(MANEXT1)/vacation.$(MANEXT1)
	install -D -m 444 vaclook.man $(MANDIR)/man$(MANEXT1)/vaclook.$(MANEXT1)

vacation:	$(SRC) $(HDR)
	$(CC) $(CFLAGS) $(IFLAGS) $(PFLAGS) $(LFLAGS) -D_PATH_VACATION=\"$(VACATION)\" -o $(BIN) $(SRC) $(LIBS)

courier:	$(SRC) $(HDR)
	$(CC) $(CFLAGS) $(IFLAGS) $(PFLAGS) $(LFLAGS) -DCOURIER -D_PATH_VACATION=\"$(VACATION)\" -o $(BIN) $(SRC) $(LIBS)

debug:	$(SRC) $(HDR)
	$(CC) $(CFLAGS) $(IFLAGS) -DDEBUG $(LFLAGS) -o $(BIN) $(SRC) $(LIBS)

clean:
	rm -f *.o core *.out *~ Makefile.old vacation

clobber: clean
	rm -f $(BIN)

package: all
	strip vacation
	tar cvf - $(PACKAGE) | gzip > ../$(TGZFILE)

gitpackage: clean
	@if [ \! -d .git ]; then echo "This is not a git managed directory, sorry!"; exit 1; fi
	eval $(GITCOMMAND)
	tar -C .. -xf ../$(GITVERSION).tar.gz
	sed -i -e "s/GITVERSION/$(GITVERSION)/" ../$(GITVERSION)/vacation.h
	sed -i -e "s/GITVERSION/$(GITVERSION)/" ../$(GITVERSION)/vacation.spec
	tar -C .. --remove-files -czf ../$(GITVERSION).tar.gz $(GITVERSION)
