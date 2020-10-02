OPTIMIZE=/Oalt /Gs
DISP=HERC
OPTION=/AS /D$(DISP) /J $(OPTIMIZE)

.asm.obj:
        masm /D$(DISP) /ML $*;

.c.obj:
        cl /c $(OPTION) $*.c

calcaddr.obj:   calcaddr.asm

line.obj:       line.asm

vhln.obj:       vhln.asm

plottype.obj:   plottype.asm

circ.obj:       circ.asm

prt.obj:        prt.asm

misc.obj:       misc.asm

dosread.obj:    dosread.asm

chardisp.obj:   chardisp.asm

hlnlmt.obj:     hlnlmt.asm

hlcopy.obj:     hlcopy.asm

global.obj:     global.c gds.h

cline.obj:      cline.c gds.h

circle.obj:     circle.c gds.h

fcircle.obj:    fcircle.c gds.h

coord.obj:      coord.c gds.h

rect.obj:       rect.c gds.h

vhline.obj:     vhline.c gds.h

prtfont.obj:    prtfont.c gds.h prtfont.h

sinval.obj:     sinval.c

tanval.obj:     tanval.c

init.obj:       init.c gds.h

ellipse.obj:    ellipse.c gds.h

arcsupp.obj:    arcsupp.c gds.h

arc2.obj:       arc2.c gds.h

Earc2.obj:      Earc2.c gds.h

select.obj:     select.c gds.h

environ.obj:    environ.c gds.h

blkcopy.obj:    blkcopy.c gds.h

blkclip.obj:    blkclip.c gds.h

blkldsv.obj:    blkldsv.c gds.h

fill.obj:       fill.c gds.h

write.obj:      write.c gds.h prtfont.h

font.obj:       font.c gds.h prtfont.h

frame.obj:      frame.c gds.h prtfont.h

pframe.obj:     pframe.c gds.h prtfont.h

arcpoint.obj:   arcpoint.c gds.h

readstr.obj:    readstr.c gds.h

xhline.obj:     xhline.c gds.h

nextxy.obj:     nextxy.c gds.h

draw.obj:       draw.c gds.h GRADENV.h

graderr.obj:    graderr.c gds.h

prtcntl.obj:    prtcntl.c gds.h prtfont.h

ftable.obj:     ftable.c gds.h prtfont.h

prtgc.obj:      prtgc.asm
        masm /D$(DISP) /DEPSON /ML $*;

