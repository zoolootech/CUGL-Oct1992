#
#  --- Version 2.2 90-10-12 16:29 ---
# 
#  CTask support routines Make-File for Microsoft make.
#
#    "reqopt" contains required options, and shouldn't normally be changed.
#    "optopt" lists optional optimization options, and may be changed.
#    "model"  is the memory model letter to use (L, S, ...)
#
reqopt=/c /Gs /Zp1 /Zl
optopt=/Ox /W3
model=L

tskallo$(model).obj:  tskalloc.c tsk.h tsklocal.h tskconf.h
   cl $(reqopt) $(optopt) /A$(model) /Fo$* tskalloc.c
   lib ctsupms$(model).lib -+$*.obj;

tsksnap$(model).obj:  tsksnap.c tsk.h tsklocal.h tskconf.h tskdeb.h
   cl $(reqopt) $(optopt) /A$(model) /Fo$* tsksnap.c
   lib ctsupms$(model).lib -+$*.obj;

conout$(model).obj:   conout.c   tsk.h tskconf.h
   cl $(reqopt) $(optopt) /A$(model) /Fo$* conout.c
   lib ctsupms$(model).lib -+$*.obj;

