PROJ	=I8255T01
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
LIBS_EXT = 	

all:	$(PROJ).exe

i8255to1.obj:	i8255to1.c

scnvfn01.obj:	scnvfn01.c

$(PROJ).exe:	i8255to1.obj scnvfn01.obj $(OBJS_EXT)
	echo >NUL @<<$(PROJ).crf
i8255to1.obj +
scnvfn01.obj +
$(OBJS_EXT)
$(PROJ).exe

$(LIBS_EXT);
<<
	link $(LFLAGS) @$(PROJ).crf

run: $(PROJ).exe
	$(PROJ) $(RUNFLAGS)

