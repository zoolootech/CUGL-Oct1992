#
#  --- Version 2.2 90-10-12 16:40 ---
# 
#  CTask sample applications Make-File for Microsoft make.
#  Application: TESTMOD.ASM
#
#    "model"  is the memory model to use.
#
model=L

.asm.obj:
   masm /W2 /MX $*;

testmod.obj:   testmod.asm tsk.mac

testmod.exe:   testmod.obj ctaskms.lib ctsupms$(model).lib
   link /M /NODEF testmod,,,ctaskms ctsupms$(model)

