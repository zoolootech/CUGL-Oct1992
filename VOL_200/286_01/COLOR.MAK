OPTIMIZE=/Ox
DISP=HERC
OPTION=/AS /J /D$(DISP) $(OPTIMIZE)

.asm.obj:
        masm /D$(DISP) /ML $*;

ftable.obj:     ftable.c
        msc $(OPTION) ftable.c;

calcaddr.obj:   calcaddr.asm

plottype.obj:   plottype.asm

