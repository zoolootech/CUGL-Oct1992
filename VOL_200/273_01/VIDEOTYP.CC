#include <dos.h>
#include <ctype.h>
#include <stdlib.h>
int color, mono, cga, ega, scrseg, bios;
video_type()
/* This will determine the Video type of the display.
   This routine defines the following global definition.
   int color, mono, cga, ega, bios, scrseg;
   They can be used by other functions if you define them as external
   definitions.
*/
{
char *pt_cga, *pt_bios;
union REGS inregs, outregs;
        color=mono=cga=ega=0;
        int86(0x11,&inregs,&inregs);
        if((inregs.h.al & 0x30) == 0x30) {
            mono=1;
            scrseg=0xb000u;
        }
        else {
            color=1;
            scrseg=0xb800u;
        }
        inregs.h.ah=0x12;
        inregs.h.bl=0x10;
        int86(0x10,&inregs,&outregs);
        if(inregs.h.bl != outregs.h.bl) ega=1;
        if(color && !ega) cga=1;
        pt_cga=getenv("CGA");
        if(pt_cga && toupper(*pt_cga) == 'Y') cga=1;
        pt_bios=getenv("BIOS");
        if(pt_bios && toupper(*pt_bios) == 'Y') bios=1;
}
