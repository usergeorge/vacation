CC=	g++
CFLAGS=	-Wall -W -Werror -fno-rtti
CDEFS=	-DUNIX -D__LINUX__
COPT=	-O3 -I./
LFLAGS= -s
SRCDIR= . 

ALL: chaosfix

TARGET= f

OBJS=	chaosfix.o \
	crc.o \
	logputs.o \
	netmail.o \
	radek.o \
	sendmail.o \
	xchmsg.o

%.o: %.c
	$(CC) $(CFLAGS) $(CDEFS) $(COPT) -c $<

%.o: $(SRCDIR)%.c
	$(CC) $(CFLAGS) $(CDEFS) $(COPT) -c $<

# Thunderbirds are go!


chaosfix: $(OBJS)
	$(CC) $(LFLAGS) -o chaosfix $(OBJS)

clean:
	-rm -f *.o
	-rm -f *~
	-rm -f core                                                                                                
	-rm -f chaosfix