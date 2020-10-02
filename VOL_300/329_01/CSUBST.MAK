# Quick C makefile for  csubst
# Program: Csubst
#

.c.obj:
	qcl -c -DANSI_C -W1 -Ze -AM $*.c

cs_hash.obj : cs_hash.c csubst.h

lexyy.obj : lexyy.c csubst.h

cs_main.obj : cs_main.c csubst.h

cs_xtrct.obj : cs_xtrct.c csubst.h

Csubst.exe : cs_hash.obj lexyy.obj cs_main.obj cs_xtrct.obj 
	del Csubst.lnk
	echo cs_hash.obj+ >>Csubst.lnk
	echo lexyy.obj+ >>Csubst.lnk
	echo cs_main.obj+ >>Csubst.lnk
	echo cs_xtrct.obj  >>Csubst.lnk
	echo Csubst.exe >>Csubst.lnk
	echo Csubst.map >>Csubst.lnk
	link @Csubst.lnk /NOI $(LDFLAGS);
