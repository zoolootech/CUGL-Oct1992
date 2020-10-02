#
# this is the makefile to the 8048 cross assembler
# and the 8048 disassembler.
#

OBJECTS = a48.o a48eval.o a48util.o
CFLAGS = -O
XDIR = /usr2/panas

a48: $(OBJECTS) a48.h
	cc $(CFLAGS) $(OBJECTS) -o a48

dis48: dis48.c dis48tbl.c dis48.h
	cc $(CFLAGS) dis48.c -o dis48

all: a48 dis48

clean:
	rm *.o

install:
	cp a48 $(XDIR)/xdev
	cp dis48 $(XDIR)/xdev
