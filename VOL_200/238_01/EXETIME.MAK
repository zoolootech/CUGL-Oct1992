OPTIMIZE=/Ox
DISP=HERC
OPTION=/AS /D$(DISP) /J $(OPTIMIZE)

.asm.obj:
        masm /DMSC4 /D$(DISP) /ML $*;

.c.obj:
        msc $(OPTION) $*;

time.obj:      time.c
        msc $(OPTION) time;

time.exe:    time.obj
        link /NOI time,time,nul,gradlib;


