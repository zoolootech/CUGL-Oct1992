
PROGRAM		= hoc3.exe

#	Microsoft C v5.1 macros definitions

MSC		= cl
CFLAGS	= -c -Ox -W2
INCL	= -I.
CC		= $(MSC) $(CFLAGS) $(INCL)

YACC	= bison -ld

.y.c:
	$(YACC) $*.y
	
.c.obj:
	$(CC) $*.c ;

hoc3.c:		hoc3.y

hoc3.obj:	hoc3.c hocdecl.h

init.obj:	init.c hocdecl.h

math.obj:	math.c hocdecl.h

symbol.obj: symbol.c hocdecl.h

hoc3.exe:	hoc3.obj init.obj math.obj symbol.obj
		link hoc3.obj init.obj math.obj symbol.obj,,, /NOI /STACK 0x7FFF ;
