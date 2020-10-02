#
# Makefile for UNIX (System V or BSD)
#

LDFLAGS=
CFLAGS= -O

LIBS = -lcurses			# for System V
#LIBS = -ltermcap			# for BSD

H=	ascii.h \
	env.h \
	keymap.h \
	ops.h \
	param.h \
	regexp.h \
	regmagic.h \
	stevie.h \
	term.h

MACH=	unix.o

OBJ=	alloc.o \
	cmdline.o \
	edit.o \
	fileio.o \
	help.o \
	hexchars.o \
	linefunc.o \
	main.o \
	mark.o \
	misccmds.o \
	normal.o \
	ops.o \
	param.o \
	ptrfunc.o \
	regexp.o \
	regsub.o \
	screen.o \
	search.o \
	term.o \
	undo.o \
	version.o

SRC=	$(OBJ:.o=.c) $(MACH:.o=.c)

all : stevie stevie.doc

stevie : $(OBJ) $(MACH)
	$(CC) $(LDFLAGS) $(OBJ) $(MACH) -o stevie $(LIBS)

lint :
	lint $(SRC)

tags :
	ctags $(SRC) $(H)

stevie.doc : stevie.mm
	nroff -rO8 -Tlpr -mm stevie.mm > stevie.doc

print :
	@pr $(H) $(SRC)

cflow :
	cflow $(SRC) > cflow.for
	cflow -r $(SRC) > cflow.rev

clean :
	rm -f $(OBJ) $(MACH)
