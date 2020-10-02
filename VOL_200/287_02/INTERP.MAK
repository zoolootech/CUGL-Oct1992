OPTIMIZE=/Ox
DISP=HERC
OPTION=/AS /J $(OPTIMIZE)

.asm.obj:
        masm /D$(DISP) /ML $*;

execfunc.obj:   execfunc.asm

interp.obj:      interp.c gds.h
        msc $(OPTION) interp;

interp.exe:      interp.obj execfunc.obj
        link /NOI interp execfunc,interp,nul,gradlib;

