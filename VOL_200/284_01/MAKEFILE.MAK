# Makefile for portable 8080 engine and vertual CP/M-80 system

# rules

.c.obj:
		tcc $(CFLAGS) -c $<
.c.exe:
		tcc $(CFLAGS) $(LDFLAGS) $<
		erase $*.obj

# options

CFLAGS =	-ms -a -d -f- -N- -p- -u -A- -G -O -Z-
LDFLAGS =	-M

# dependencies

OBJS =		main.obj machine.obj trace.obj monitor.obj util.obj bios.obj

all:		i8080.exe newdsk.exe cpmfile.exe mkhex.exe

i8080.exe:      $(OBJS)
		tcc $(LDFLAGS) -ei8080.exe $(OBJS)

main.obj:
machine.obj:	compile.h config.h defs.h
trace.obj:	compile.h config.h defs.h
monitor.obj:	compile.h config.h defs.h mon.h
util.obj:	compile.h config.h defs.h
bios.obj:	          config.h        bios.h

# utilities

clean:
		erase main.obj
		erase machine.obj
		erase trace.obj
		erase monitor.obj
		erase util.obj
		erase bios.obj
		erase newdsk.obj
		erase cpmfile.obj
		erase i8080.map
		erase newdsk.map
		erase cpmfile.map
		erase mkhex.map
clobber:
		erase i8080.exe
		erase newdsk.exe
		erase cpmfile.exe
		erase mkhex.exe
