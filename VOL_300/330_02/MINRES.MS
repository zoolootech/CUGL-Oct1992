#
#  --- Version 2.2 90-10-12 16:39 ---
# 
#  CTask sample applications Make-File for Microsoft make.
#  Application: MINRES.ASM
#
#    "model"  is the memory model to use.
#
model=L

.asm.obj:
   masm /W2 /MX $*.asm;

minres.obj:     minres.asm tsk.mac

minres.exe:  minres.obj ctaskms.lib ctsupms$(model).lib
   link /m minres,,,ctaskms ctsupms$(model)


