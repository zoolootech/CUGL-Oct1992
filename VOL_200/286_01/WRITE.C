#include <stdio.h>
#include <gds.h>
#include <prtfont.h>

#define ERROR (-1)
#define OK 0

/*==============================================================*
 *                                                              *
 *    This file contains 2 public routines:-                    *
 *      writec(x,y,ch,boxw,boxh,option)                         *
 *          write a single character to the current frame       *
 *          using currently selected font                       *
 *                                                              *
 *      WriteStr(option,direction,x,y,str,ox,oy)                *
 *          write a character string to the current frame       *
 *          using currently selected font                       *
 *                                                              *
 *==============================================================*/

int (*SPACING_FUNC)(); /* special spacing function used by WriteStr
                          if non-none. See the user's manaul    */

writec(x,y,ch,boxw,boxh,option)
register int x,y;
int ch,*boxw,*boxh,option;
{
    char huge *tmpptr;
    struct FONTABLE *tptr;
    int wrtwid,shift,fwid,iw,h,owrtwid,temp,temp1,tmpstyle;
    struct fontspec huge *sp;

    x+=ORGX; y+=ORGY;

    tptr=&FONTTAB[CURFONT];     /* point to current font table */
    if ((ch < tptr->start) || (ch > tptr->lastchar)) {
        /* . . . . . . . . . . . . . . . . . . . . . . . . . . */
        /* required character code does not exist in this font */
        /* replace with default character                      */
        /* . . . . . . . . . . . . . . . . . . . . . . . . . . */
        ch=tptr->defch;
        *boxw=*boxh=0;
        /* return if default character is not valid too ! */
        if ((ch < tptr->start) || (ch > tptr->lastchar)) return;
    }

    if (CURFONT > 0 && (tptr=&FONTTAB[CURFONT])->status==USED) {

        /* fint the pixel data address in memory */
        if (tptr->type == 0) { /* optimize when using font in ROM */
            sp=tptr->chinfo;
            tmpptr=tptr->fontaddr + (ch - tptr->start) * sp->height
                             * ((sp->inkwidth+7) >> 3);
        } else if (tptr->type == 1) {
            sp=tptr->chinfo + (temp=(ch - tptr->start));
            tmpptr=tptr->fontaddr + (tptr->szinfo)[temp];
        } else {
            graderror(6,12,tptr->type);
        }
    } else { /* default font of invalid font number */
        /* use system font */
        tmpptr=((char huge *) ROMFONT) + (ch << 3);
        sp = (struct fontspec huge *) &ROMchinfo;
    }
    iw=sp->inkwidth;    /* load ink-width  */
    temp=h=sp->height;  /* load ink-height */
    fwid=(iw+7)>>3;     /* width in byte */
    *boxh=sp->cellheight; /* box width */
    *boxw = sp->width;    /* box height */
    switch (option & 0x07) {    /* see the user's manaul */
    case 2:     y += *boxh;
    case 1:     x -= *boxw;
                break;
    case 6:     x -= (*boxw - 1) >> 1;
    case 3:     y += *boxh;
                break;
    case 5:     y += (*boxh - 1) >> 1;
    case 4:     x -= (*boxw - 1) >> 1;
                break;
    case 7:     y += (*boxh - 1) >> 1;
    case 0:     break;
    }
    shift=CUR_PLOT;             /* remember current plot type */
    switch ((option & 0x30) >> 4) { /* see the use's manual */
    case 1:
        owrtwid=1;
        goto drawbox;
    case 2:
        owrtwid=0;
        goto drawbox;
    case 3:
        owrtwid=2;
        if (shift != 2)
            owrtwid=(shift+1) & 0x01;
drawbox:  /* draw block */
        PlotType(owrtwid);
        if (sp->topline < temp - 1)
            temp+=temp-1-sp->topline;
        if ((owrtwid=sp->leftmargin+iw) < *boxw)
            owrtwid=*boxw;
        if (sp->leftmargin < 0) {
            owrtwid-=(temp1=sp->leftmargin);
        } else {
            temp1=0;
        }
        tmpstyle=SetStyle(0xffff);
        HorzLine(x-ORGX+temp1,y-h+1-ORGY,owrtwid,temp);
        SetStyle(tmpstyle);
        PlotType(shift);        /* restore plot type */
    case 0:
        break;
    }
    if ((option & 0x08) || !(iw | h)) {
        /* . . . . . . . . . . . . . . . . . . . . . . .  */
        /*  If width or height is zero or option is set   */
        /*  nothing to write, so return                   */
        /* . . . . . . . . . . . . . . . . . . . . . . .  */
        return;
    }
    x += sp->leftmargin;        /* change x,y to top-left-hand corner */
    y -= sp->topline;           /* of the ink box                     */

    /* return if character box is completely outside the window */
    if (x+iw <= WINX1 || x > WINX2 ||
        y+h <= WINY1  || y > WINY2) return;
    shift= x & 0x0f;            /* no. of bits required to shift right */
    owrtwid=(iw+15) >> 4;       /* width in words */
    if (((owrtwid<<4) - iw) < shift) owrtwid++;  /* allocate extra word   */
                                        /* to cater for shift if required */
    if (y < WINY1) {    /* clip top */
        h -= (temp=WINY1-y);
        tmpptr += temp * fwid;
        y = WINY1;
    }
    if ((temp=y+h-1-WINY2) > 0) {  /* clip bottom */
        h -= temp;
    }
    copyfont(fwid,h,owrtwid,tmpptr,CFBUF);      /* copy datat to buffer */
    temp1=0;
    if (x < WINX1) {    /* clip left if required */
        ctextL((temp=WINX1-x),owrtwid,h,h * owrtwid,CFBUF);
        iw -= temp & 0xfff0;
        temp1=((temp & 0x0f)+shift) >> 4;  /* first word of every row in */
                                           /* buffer will be all zero    */
        x = WINX1;
    }
    if ( x+iw-1 > WINX2) {      /* clip right */
        ctextR((iw=WINX2-x+1),owrtwid,h,CFBUF);
    }
    wrtwid=(iw+15) >> 4;
    if (((wrtwid<<4) - iw) < shift) wrtwid++;

    /* Finally, ready to write !! */
    wrtc(x,y,shift,wrtwid,h,owrtwid,CFBUF,temp1);
}

WriteStr(option,direction,x,y,str,ox,oy)
int option,direction,x,y,ox,oy;
char *str;
{
    int ch,w,h,moveoption;

    if ((moveoption=(option>>8) & 0x03) == 2) {
        if (SPACING_FUNC == (int (*)(int *)) NULL) {
            moveoption=1; 
        }
    } 
    while ((ch = *str++)) {
        writec(x,y,ch,&w,&h,(option & 0xff));
        if (moveoption == 0) {
            switch (direction & 0x07) {
            case 0:
            case 7:
            case 1:
                x += w;
                break;
            case 3:
            case 4:
            case 5:
                x -= w;
                break;
            }
            switch (direction & 0x07) {
            case 1:
            case 2:
            case 3:
                y += h;
                break;
            case 5:
            case 6:
            case 7:
                y -= h;
                break;
            }
        } else if (moveoption == 2) {
            if ((*SPACING_FUNC)(&x,&y,w,h,str)) break;
        }
        x += ox;
        y += oy;
    }
LASTX=x;  /* remember last coordinate */
LASTY=y;
}

