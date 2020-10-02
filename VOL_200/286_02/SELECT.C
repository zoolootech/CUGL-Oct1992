#include <stdio.H>
#include <gds.h>
#include <prtfont.h>

SelectFont(fontnu)
int fontnu;
{
    register int temp;

    if (FONTTAB[fontnu].status != NOT_USED) {
        temp=CURFONT;
        CURFONT=fontnu;
        return(temp);
    } else
        return(-1);
}

SelectFrame(frnu)
int frnu;
{   register struct fdesc *fptr;
    register int temp;

    fptr = &FTABLE[temp=CUR_FRAME];
    fptr->forgx = ORGX;
    fptr->forgy = ORGY;
    fptr->fwinx1 = WINX1;
    fptr->fwinx2 = WINX2;
    fptr->fwiny1 = WINY1;
    fptr->fwiny2 = WINY2;
    if ((fptr=&FTABLE[frnu])->status == NOT_USED)
        return(-1);
    CUR_FRAME=frnu;
    LNBYTE=fptr->ln_byte;
    LADDRO=LNBYTE & 0x0f;
    LADDRS=LNBYTE >> 4;
    FRM_ST=fptr->faddr;
    WINX1=fptr->fwinx1;
    WINY1=fptr->fwiny1;
    WINX2=fptr->fwinx2;
    WINY2=fptr->fwiny2;
    XLIMIT=fptr->horz;
    YLIMIT=fptr->vert;
    ORGX=fptr->forgx;
    ORGY=fptr->forgy;
    return(temp);
}
