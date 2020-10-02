PROJ	=DATETEST
DEBUG	=1
CC	=qcl
CFLAGS_G	= /AM /W2 /Ze 
CFLAGS_D	= /Zd /Od 
CFLAGS_R	= /O /Ot /Gs /DNDEBUG 
CFLAGS	=$(CFLAGS_G) $(CFLAGS_D)
LFLAGS_G	= /CP:0xffff /NOI /SE:0x80 /ST:0x2710 
LFLAGS_D	= 
LFLAGS_R	= 
LFLAGS	=$(LFLAGS_G) $(LFLAGS_D)
RUNFLAGS	=
OBJS_EXT = 	
LIBS_EXT = 	lib\mc_wndw.lib 

all:	$(PROJ).exe

datetest.obj:	datetest.c

$(PROJ).exe:	datetest.obj $(OBJS_EXT)
	echo >NUL @<<$(PROJ).crf
datetest.obj +
$(OBJS_EXT)
$(PROJ).exe

$(LIBS_EXT);
<<
	ilink -a -e "link $(LFLAGS) @$(PROJ).crf" $(PROJ)

run: $(PROJ).exe
	$(PROJ) $(RUNFLAGS)

