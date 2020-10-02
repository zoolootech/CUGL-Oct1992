
COMPSW = /AS /Ot /FPa /W3
LINKSW = /EXE /F /PACKCODE
# COMPSW = /AS /Od /FPa /W3 /Zi
# LINKSW = /CO

OBJS   = advent.obj database.obj verb.obj english.obj turn.obj itverb.obj  \
	 saveadv.obj adventdb.obj

.c.obj	    :
		cl -c $(COMPSW) $*.c

.obj.exe    :
		link <@<
$(OBJS)
$*
$*
$(LINKSW);
<

advent.exe  :	$(OBJS)

$(OBJS)     :	advent.h

