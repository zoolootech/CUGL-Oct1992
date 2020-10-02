
model=L
compsw=/A$(model) /Ox /Gs /Zd /DFAST
linksw=/EXE /ST:8192 /NOE /F /PAC /BATCH /LI /MAP
#compsw=/A$(model) /Zi /Od
#linksw=/CO /ST:8192 /NOE /BATCH /LI
argv=c:\lib\msc\setargv.obj
prflib=$(model)PROFILE


.C.OBJ	       :
		 cl /c $(compsw) $*.C
		 echo $* >>awk.bld


AWK1.OBJ       : $*.C regex.h awk.h
		 cl /c $(compsw) $*.C

AWK2.OBJ       : $*.C awk.h      

AWK3.OBJ       : $*.C awk.h obstack.h

AWKTAB.C       : $*.Y
		 yacc $*

AWKTAB.OBJ     : $*.C awk.h      

DEBUG.OBJ      : $*.C awk.h      

OBSTACK.OBJ    : $*.C awk.h obstack.h

REGEX1.OBJ     : $*.C awk.h regex.h

REGEX2.OBJ     : $*.C awk.h regex.h

$ALWAYS        :
		 if not exist awk.lib lib awk;
		 maklib awk.lib awk.bld
		 if exist awk.bld del awk.bld
		 if exist awk.bak del awk.bak

AWK.EXE        : awk1.obj awk.lib
		 link $(linksw) awk1 $(argv),$*,$*,awk $(prflib);
