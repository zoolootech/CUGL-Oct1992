#
#  --- Version 2.2 90-10-12 16:28 ---
# 
#  CTask kernel Make-File for Microsoft make.
#
#    "reqopt" contains required options, and shouldn't normally be changed.
#    "optopt" lists optional optimization options, and may be changed.
#
reqopt=/c /AL /Gs /Zp1 /Zl /NTCTASK_TEXT 
optopt=/Ox /W3

.c.obj:
   cl $(reqopt) $(optopt) $*.c
   lib ctaskms.lib -+$*.obj;

.asm.obj:
   masm /W2 /MX $*;
   lib ctaskms.lib -+$*.obj;

tskmain.obj:   tskmain.c  tsk.h tsklocal.h tskconf.h tskdeb.h

stskmain.obj:  tskmain.c  tsk.h tsklocal.h tskconf.h tskdeb.h
   cl $(reqopt) $(optopt) /DSECONDARY /Fo$* tskmain.c

tsktask.obj:   tsktask.c  tsk.h tsklocal.h tskconf.h

tsktutl.obj:   tsktutl.c  tsk.h tsklocal.h tskconf.h

tskutil.obj:   tskutil.c  tsk.h tsklocal.h tskconf.h

tskgrp.obj:    tskgrp.c   tsk.h tsklocal.h tskconf.h

tskinst.obj:   tskinst.c  tsk.h tsklocal.h tskconf.h

tsktick.obj:   tsktick.c  tsk.h tsklocal.h tskconf.h

tsktops.obj:   tsktops.c  tsk.h tsklocal.h tskconf.h

tskttsk.obj:   tskttsk.c  tsk.h tsklocal.h tskconf.h

tsksub.obj:    tsksub.c   tsk.h tsklocal.h tskconf.h

tskname.obj:   tskname.c  tsk.h tsklocal.h tskconf.h

tskmemw.obj:   tskmemw.c  tsk.h tsklocal.h tskconf.h

tskporw.obj:   tskporw.c  tsk.h tsklocal.h tskconf.h

tskhot.obj:    tskhot.c  tsk.h tsklocal.h tskconf.h

tsktsub.obj:   tsktsub.c  tsk.h tsklocal.h tskconf.h

tskflg.obj:    tskflg.c   tsk.h tsklocal.h tskconf.h

tskcnt.obj:    tskcnt.c   tsk.h tsklocal.h tskconf.h

tskrsc.obj:    tskrsc.c   tsk.h tsklocal.h tskconf.h

tskmsg.obj:    tskmsg.c   tsk.h tsklocal.h tskconf.h

tskpip.obj:    tskpip.c   tsk.h tsklocal.h tskconf.h

tskwpip.obj:   tskwpip.c  tsk.h tsklocal.h tskconf.h

tskbuf.obj:    tskbuf.c   tsk.h tsklocal.h tskconf.h

tsksio.obj:    tsksio.c   tsk.h sio.h tskconf.h

tskprt.obj:    tskprt.c   tsk.h prt.h tskconf.h

tskasm.obj:    tskasm.asm   tsk.mac tskconf.h tskdeb.h

tskque.obj:    tskque.asm   tsk.mac tskconf.h

tsktim.obj:    tsktim.asm   tsk.mac tskconf.h tskdeb.h

tskdos.obj:    tskdos.asm   tsk.mac tskconf.h

tsksec.obj:    tsksec.asm   tsk.mac tskconf.h

tskres.obj:    tskres.asm   tsk.mac tskconf.h

tskbios.obj:   tskbios.asm  tsk.mac tskconf.h

tskint17.obj:  tskint17.asm tsk.mac tskconf.h

tskkbd.obj:    tskkbd.asm   tsk.mac tskconf.h tskdeb.h

tsksioi.obj:   tsksioi.asm  tsk.mac tskconf.h

tskprti.obj:   tskprti.asm  tsk.mac tskconf.h

tskstck.obj:   tskstck.asm  tsk.mac tskconf.h tskdeb.h

tskems.obj:    tskems.asm  tsk.mac tskconf.h

tskprf.obj:    tskprf.asm  tsk.mac tskconf.h

tskndp.obj:    tskndp.asm  tsk.mac tskconf.h

# The following is ony required if CODE_SHARING is enabled

tskstub.obj:   tskstub.asm  tsk.mac tskconf.h
   masm /W2 /MX /DGEN_JTAB $*;
   lib ctaskms.lib -+$*.obj;

