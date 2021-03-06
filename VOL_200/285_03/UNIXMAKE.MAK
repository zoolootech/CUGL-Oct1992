# Makefile for bison
#   Copyright (C) 1984 Bob Corbett and Free Software Foundation, Inc.

#BISON is distributed in the hope that it will be useful, but WITHOUT ANY
#WARRANTY.  No author or distributor accepts responsibility to anyone
#for the consequences of using it or for whether it serves any
#particular purpose or works at all, unless he says so in writing.
#Refer to the BISON General Public License for full details.

#Everyone is granted permission to copy, modify and redistribute BISON,
#but only under the conditions described in the BISON General Public
#License.  A copy of this license is supposed to have been given to you
#along with BISON so you can know your rights and responsibilities.  It
#should be in a file named COPYING.  Among other things, the copyright
#notice and this notice must be preserved on all copies.

# In other words, you are welcome to use, share and improve this program.
# You are forbidden to forbid anyone else to use, share and improve
# what you give them.   Help stamp out software-hoarding!  */


# For sysV, you must uncomment the following lines
#CFLAGS = -g -DUSG
#LIBS = -lPW
#and comment out the assignment of CFLAGS below.
#Also, if getopt.c gives any trouble, delete it from OBJECTS.
#so the system's getopt will be used.


DESTDIR=
# where the installed binary goes
BINDIR = $(DESTDIR)/usr/local/bin

# where the parsers go
PARSERDIR = $(DESTDIR)/usr/local/lib

# names of parser files
PFILE = bison.simple
PFILE1 = bison.hairy

# It is unwise ever to compile a program without symbols.
CFLAGS = -g

PFILES = -DXPFILE=\"$(PARSERDIR)/$(PFILE)\" \
	 -DXPFILE1=\"$(PARSERDIR)/$(PFILE1)\"

OBJECTS = LR0.o allocate.o closure.o conflicts.o derives.o files.o	\
	  getargs.o gram.o lalr.o					\
	  lex.o main.o nullable.o output.o print.o reader.o symtab.o	\
	  warshall.o							\
	  getopt.o

start: bison

clean:
	rm -f *.o core bison

install: bison
	install bison $(BINDIR)
	cp $(PFILE) $(PFILE1) $(PARSERDIR)

bison: $(OBJECTS)
	$(CC) $(LDFLAGS) -o bison $(OBJECTS) $(LIBS)

dist:	bison-dist.tar bison-dist.tar.Z

.PHONY: bison-dist.tar
bison-dist.tar:
	tar chvf bison-dist.tar \
		COPYING Makefile REFERENCES bison.1 bison.simple bison.hairy \
		LR0.c allocate.c closure.c conflicts.c \
		derives.c files.c getargs.c gram.c lalr.c lex.c main.c \
		nullable.c output.c print.c reader.c symtab.c warshall.c \
		files.h gram.h lex.h machine.h new.h state.h symtab.h types.h \
		bison.cld build.com vmsgetargs.c vmshlp.mar README \
		bison.texinfo bison.info* \
		-C ../lib getopt.c
bison-dist.tar.Z: bison-dist.tar
	compress < bison-dist.tar > bison-dist.tar.Z

# This file is different to pass the parser file names
# to the compiler.
files.o: files.c files.h new.h gram.h
	$(CC) -c $(CFLAGS) $(PFILES) files.c

LR0.o: machine.h new.h gram.h state.h
closure.o: machine.h new.h gram.h
conflicts.o: machine.h new.h files.h gram.h state.h
derives.o: new.h types.h gram.h
getargs.o: files.h
lalr.o: machine.h types.h state.h new.h gram.h
lex.o: files.h symtab.h lex.h
main.o: machine.h
nullable.o: types.h gram.h new.h
output.o: machine.h new.h files.h gram.h state.h
print.o: machine.h new.h files.h gram.h state.h
reader.o: files.h new.h symtab.h lex.h gram.h
symtab.o: new.h symtab.h gram.h
warshall.o: machine.h
