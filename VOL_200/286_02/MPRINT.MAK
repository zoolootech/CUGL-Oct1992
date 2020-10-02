OPTIMIZE=/Ox
DISP=HERC
OPTION=/AS /J $(OPTIMIZE)

.asm.obj:
        masm /D$(DISP) /ML $*;

execfunc.obj:   execfunc.asm

mprint.obj:      mprint.c
        msc $(OPTION) mprint;

mprint.exe:      mprint.obj execfunc.obj
        link /NOI mprint execfunc,mprint,nul,gradlib;

