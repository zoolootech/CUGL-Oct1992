prtgc.obj:      prtgc.asm
        masm /ML /DOKI prtgc;

gradoki.obj:    gradoki.c
        msc /AS /J /Ox gradoki;
