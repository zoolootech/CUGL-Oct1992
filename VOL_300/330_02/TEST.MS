#
#  --- Version 2.2 90-10-12 16:40 ---
# 
#  CTask sample applications Make-File for Microsoft make.
#  Application: TEST.C
#
#    "reqopt" contains required options, and shouldn't normally be changed.
#    "optopt" lists optional optimization options, and may be changed.
#    "model"  is the memory model to use.
#
reqopt=/c /Zp1 /Gs
optopt=/Ox /W3
model=L

.c.obj:
   cl $(reqopt) $(optopt) /A$(model) $*.c

.obj.exe:
   link $*,,,ctaskms ctsupms$(model) /M/ST:16000;

test.obj:     test.c tsk.h tskconf.h

test.exe:     test.obj ctaskms.lib ctsupms$(model).lib

