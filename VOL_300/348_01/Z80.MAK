#
# this is the makefile to the 8048 cross assembler
#

OBJECTS = z80a.o z80aeval.o z80autil.o
CFLAGS = -O

z80a: $(OBJECTS)
	cc $(CFLAGS) $(OBJECTS) -o z80a

clean:
	rm *.o
