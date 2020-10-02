PROJ	=CA
DEBUG	=0
CC	=qcl
AS	=qcl
CFLAGS_G	= /AS /W3 /Ze /G2 
CFLAGS_D	= /Zi /Zr /Od 
CFLAGS_R	= /O /Ot /DNDEBUG 
CFLAGS	=$(CFLAGS_G) $(CFLAGS_R)
AFLAGS_G	= /Cx /W1 /P2 
AFLAGS_D	= /Zi /l 
AFLAGS_R	= /DNDEBUG 
AFLAGS	=$(AFLAGS_G) $(AFLAGS_R)
LFLAGS_G	= /CP:0x20 /NOI /SE:0x80 /ST:0xfa0 
LFLAGS_D	= /CO 
LFLAGS_R	= 
LFLAGS	=$(LFLAGS_G) $(LFLAGS_R)
RUNFLAGS	=@ca.prj /red  /CDE
OBJS_EXT = 	
LIBS_EXT = 	

.asm.obj: ; $(AS) $(AFLAGS) -c $*.asm

all:	$(PROJ).EXE

glob_ca.obj:	glob_ca.c $(H)

ifdef_ca.obj:	ifdef_ca.c $(H)

intf_ca.obj:	intf_ca.c $(H)

linkl_ca.obj:	linkl_ca.c $(H)

parse_ca.obj:	parse_ca.c $(H)

tdef_ca.obj:	tdef_ca.c $(H)

util_ca.obj:	util_ca.c $(H)

utlg_ca.obj:	utlg_ca.c $(H)

xrf_ca.obj:	xrf_ca.c $(H)

exit_ca.obj:	exit_ca.c $(H)

types_ca.obj:	types_ca.c $(H)

func_ca.obj:	func_ca.c $(H)

$(PROJ).EXE:	glob_ca.obj ifdef_ca.obj intf_ca.obj linkl_ca.obj parse_ca.obj tdef_ca.obj \
	util_ca.obj utlg_ca.obj xrf_ca.obj exit_ca.obj types_ca.obj func_ca.obj $(OBJS_EXT)
	echo >NUL @<<$(PROJ).crf
glob_ca.obj +
ifdef_ca.obj +
intf_ca.obj +
linkl_ca.obj +
parse_ca.obj +
tdef_ca.obj +
util_ca.obj +
utlg_ca.obj +
xrf_ca.obj +
exit_ca.obj +
types_ca.obj +
func_ca.obj +
$(OBJS_EXT)
$(PROJ).EXE

$(LIBS_EXT);
<<
	qlink $(LFLAGS) @$(PROJ).crf

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)

