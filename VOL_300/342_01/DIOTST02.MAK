PROJ	=DIOTST02
DEBUG	=0
CC	=qcl
CFLAGS_G	= /AL /W3 /Ze 
CFLAGS_D	= /Zi /Od 
CFLAGS_R	= /Od /Gs /DNDEBUG 
CFLAGS	=$(CFLAGS_G) $(CFLAGS_R)
LFLAGS_G	= /CP:0xffff /NOI /SE:0x80 /ST:0x400 
LFLAGS_D	= 
LFLAGS_R	= 
LFLAGS	=$(LFLAGS_G) $(LFLAGS_R)
RUNFLAGS	=
OBJS_EXT = 	
LIBS_EXT = 	diolibl.lib 

all:	$(PROJ).exe

diotst02.obj:	diotst02.c

$(PROJ).exe:	diotst02.obj $(OBJS_EXT)
	echo >NUL @<<$(PROJ).crf
diotst02.obj +
$(OBJS_EXT)
$(PROJ).exe

$(LIBS_EXT);
<<
	link $(LFLAGS) @$(PROJ).crf

run: $(PROJ).exe
	$(PROJ) $(RUNFLAGS)

