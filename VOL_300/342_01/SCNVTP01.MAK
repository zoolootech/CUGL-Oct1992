PROJ	=SCNVTP01
DEBUG	=1
CC	=qcl
CFLAGS_G	= /AS /W3 /Ze 
CFLAGS_D	= /Zi /Od 
CFLAGS_R	= /Od /Gs /DNDEBUG 
CFLAGS	=$(CFLAGS_G) $(CFLAGS_D)
LFLAGS_G	= /CP:0xffff /NOI /SE:0x80 /ST:0x800 
LFLAGS_D	= /CO 
LFLAGS_R	= 
LFLAGS	=$(LFLAGS_G) $(LFLAGS_D)
RUNFLAGS	=
OBJS_EXT = 	
LIBS_EXT = 	

all:	$(PROJ).exe

scnvtp01.obj:	scnvtp01.c

scnvfn01.obj:	scnvfn01.c

$(PROJ).exe:	scnvtp01.obj scnvfn01.obj $(OBJS_EXT)
	echo >NUL @<<$(PROJ).crf
scnvtp01.obj +
scnvfn01.obj +
$(OBJS_EXT)
$(PROJ).exe

$(LIBS_EXT);
<<
	ilink -a -e "link $(LFLAGS) @$(PROJ).crf" $(PROJ)

run: $(PROJ).exe
	$(PROJ) $(RUNFLAGS)

