#include <stdio.h>
#include <gds.h>
#include <prtfont.h>

#define ERROR (-1)
#define OK 0

CreateFrame(horizontal,vertical)
int horizontal,vertical;
{
    char huge *halloc();
    int ff,width;
    register struct fdesc *fptr;

    if ((ff=freeframe()) == 0) {
        graderror(6,7);
    }
    fptr = & FTABLE[ff];
    width=(horizontal + 15) >> 4;
    if ((fptr->faddr=nor_ptr((int far *) halloc(((long) width)*vertical,2))) ==
                             (int far *) NULL) {
        graderror(6,8,horizontal,vertical);
    }
    fptr->forgx=fptr->forgy=fptr->fwinx1=fptr->fwiny1=0;
    fptr->status=USED;
    fptr->ln_byte=width << 1;
    fptr->fwinx2=fptr->horz=(width << 4) - 1;
    fptr->fwiny2=fptr->vert=vertical - 1;
    return(ff);
}

freeframe()
{
    register int loop;

    for(loop=1; loop < NFRAME ; loop++)
        if (FTABLE[loop].status == NOT_USED)
            return(loop);
    return(0);
}

int RemvFrame(ff)
int ff;
{
    register struct fdesc *fptr;

    if ((fptr=&FTABLE[ff])->status == NOT_USED) return(ERROR);
    if (fptr->status == PERMANENT) return(OK);
    if (ff == CUR_FRAME) CUR_FRAME=0;
    hfree(fptr->faddr);
    return(OK);
}

