CFLAGS=		-O

OFILES=		basic.o bind.o buffer.o crypt.o \
		display.o eval.o exec.o file.o fileio.o \
		input.o isearch.o line.o \
		lock.o main.o random.o region.o search.o spawn.o \
                tcap.o termio.o \
		window.o word.o menu.o

CFILES=		basic.c bind.c buffer.c crypt.c \
		display.c eval.c exec.c file.c fileio.c \
		input.c isearch.c line.c \
		lock.c main.c random.c region.c search.c spawn.c \
		tcap.c termio.c \
		window.c word.c menu.c

HFILES=		estruct.h edef.h efunc.h epath.h ebind.h evar.h

emacs:		$(OFILES)
		$(CC) $(CFLAGS) $(OFILES) -ltermcap -lc -o emacs

$(OFILES):	$(HFILES)
