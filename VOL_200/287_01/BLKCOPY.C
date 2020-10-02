#include <stdio.h>
#include <gds.h>

#define ERROR (-1)
#define OK 0
#define VOIDBLOCK 1

/*==============================================================*
 *  This file contains the following routines:                  *
 *                                                              *
 *==============================================================*/

int far *phyaddr();

BlockCopy(sframe,sx,sy,dframe,dx,dy,length,height)
int sframe,sx,sy,dframe,dx,dy,length,height;
{
    int curframe, far *sptr, far *dptr, old_x, old_y;
    int far *lcl_sfrmst, lcl_slnbyte, far *lcl_dfrmst, lcl_dlnbyte;

    if ((length<=0) || (height <= 0)) return(OK); /* height must be positive */
    sx += ORGX;              /* get absolute position */
    sy += ORGY;
    dx += ORGX;
    dx = (dx & 0xfff0) | (sx & 0x000f);
    dy += ORGY;

    curframe=CUR_FRAME;

    old_x=sx; old_y=sy;
    if (SelectFrame(sframe)==ERROR) {
        SelectFrame(curframe);
        return(ERROR);
    }
    if (blockclip(&sx,&sy,&length,&height) == VOIDBLOCK) {
        SelectFrame(curframe);
        return(OK);
    }
    dx+=(sx-old_x);
    dy+=(sy-old_y);
    lcl_sfrmst=FRM_ST;
    lcl_slnbyte=LNBYTE;

    old_x=dx; old_y=dy;
    if (SelectFrame(dframe)==ERROR) {
        SelectFrame(curframe);
        return(ERROR);
    }
    if (blockclip(&dx,&dy,&length,&height) == VOIDBLOCK) {
        SelectFrame(curframe);
        return(OK);
    }
    sx+=(dx-old_x);
    sy+=(dy-old_y);
    lcl_dfrmst=FRM_ST;
    lcl_dlnbyte=LNBYTE;


    do {
        FRM_ST=lcl_sfrmst;
        LNBYTE=lcl_slnbyte;
        CUR_FRAME=sframe;
        sptr=phyaddr(sx,sy++);
        FRM_ST=lcl_dfrmst;
        LNBYTE=lcl_dlnbyte;
        CUR_FRAME=dframe;
        dptr=phyaddr(dx,dy++);
        CUR_FRAME=0;    /* always physical address */
        hlcopy(sptr,dptr,sx,length); /* draw a line with height 1 each time */
    } while (--height);
    CUR_FRAME=dframe;
    SelectFrame(curframe);
    return(OK);
}

