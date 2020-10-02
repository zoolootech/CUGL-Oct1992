#include <stdio.h>
#include <gds.h>
#include <prtfont.h>

/*==============================================================*
 *                                                              *
 *    This file contain functions for printing.                 *
 *    Only PrintFrame should be used by user.                   *
 *                                                              *
 *    Number of dots per pass (ndot) and number of dots between *
 *      2 pins on the print head are fixed in this file. Use    *
 *      pframe2.c if you want them to be variable.              *
 *                                                              *
 *==============================================================*/

#define ERROR (-1)
#define OK 0

#define ndot 8  /* number of dots printed per pass */
#define dpp 3   /* number of dots printed between 2 pins on print head
                   in vertical mode 1 */
extern int sixlpi;   /* spacing for six lines per inch */

/* main program for printing frame */
PrintFrame(flag,page,line,tlsp,offset)
int flag,line,offset,tlsp;
char **page;
/*
    flag : flag to control vertical and horizontal density
    page : pointer to a array of character pointer containing text data
    line : number of lines of text data
    tlsp : number of point between 2 base lines of text
  offset : position when text data should begin (can be + or -)
*/
{
    int hf,vf,remain,count1,count2,y;

    MAXPH=FTABLE[CUR_FRAME].vert+1; /* maximum height */
    y=0;
    hf = flag & 0x0f;           /* horizontal flag */
    vf = (flag & 0xf0) >> 4;    /* vertical flag   */
    if (line <= 0) {    /* if line <= 0, no text data */
        if (vf == 1) {
            remain = (MAXPH+dpp-1)/dpp;
        } else {
            remain = MAXPH;
        }
        goto moreg;     /* print graphics data only */
    }
    if (offset != 0) {
        if (offset > 0) {   /* text data are printed first */
            line -= (count1=offset/tlsp);   /* count1 contains lines to print */
            count2 = offset - count1 * tlsp;
            while (count1--) {
                ptext(*page++);   /* print text data only */
                skiplong(tlsp); 
            }
            if (count2) {       /* since offset is not multiple of tlsp */
                ptext(*page++); /* we need to do some adjustment at the end */
                line--;
                skiplong(count2);
            }
            count1 = tlsp - count2;
        } else {   /* offset < 0, print graphics data first */
            count1 = -offset;
        }

        /* print some graphics data only */
        if (vf == 1) {
            printg1(hf,0,count1);
            y = count1 * dpp;
        } else {
            printg0(hf,0,count1);
            y = count1;
        }
    }

    /* begin merge print of text data and graphics data */
    if (vf == 1) {    /* high vertical resolution */
        for (remain=(MAXPH-y+dpp-1)/dpp; remain>0; ) {
            prtg1(hf,y++,remain);   /* print first pass of graphics data */
            ptext(*page++);         /* then test data */
            line--;
            for(count1=dpp-1; count1>0; count1--) {
                skipdot(1);  /* print remaining graphics data for that line */
                prtg1(hf,y++,remain);
            }
            skipdot((remain-1)*dpp+1);  /* long skip */
            remain -= ndot;
            y += (ndot-1)*dpp;
            if (remain <= 0) break;
            if (line <= 0) goto moreg;  /* if text data is shorter than */
                                        /* graphics data, goto moreg */
            printg1(hf,y,tlsp-ndot);    /* else fill up the remaining line */
            y+=(tlsp-ndot)*3;
            remain -= tlsp - ndot;
        }
    } else {    /* normal vertical density */
        for (remain=MAXPH-y; remain > 0; ) {
            prtg0(hf,y,remain);    /* graphics data */
            ptext(*page++);        /* text data */
            line--;
            skippt(ndot);          /* skip */
            y+=ndot;
            remain -= ndot;
            if (remain <= 0) break;
            if (line <= 0) goto moreg;  /* no more text data, goto moreg */
            printg0(hf,y,tlsp-ndot);    /* fill up remaining line */
            y += tlsp-ndot;             /* update y coordinate */
            remain -= tlsp-ndot;
        }
    }
    if (line > 0) {                     /* more text data ? */
        while (line--) {                /* yes, print remaining */
            ptext(*page++);
            skiplong(tlsp);
       }
    }
    goto restore;
moreg:
    if (vf == 1) {              /* more graphics data */
        printg1(hf,y,remain);   /* high verical density */
    } else {
        printg0(hf,y,remain);   /* normal vertical density */
    }
restore:
    prtpc(&setp,sixlpi);        /* set back to 6 line per inch */
    pbflush();                  /* flush anything in output buffer */
}

/* prints one pass in vertical mode 0 for graphics data only */
static prtg0(option,y,height)
int option,y,height;
{
    int far *ptr[ndot];
    int outw[ndot], length;
    register int loop;

    option &= 0x7f;
    if (option >= NHEADER) option=0;
/*    if (y+height > MAXPH)
        height=MAXPH-y; */
    if (height > ndot)
        height=ndot;

    /* if frame width larger than printer width */
    if ((length = XLIMIT+1) > MAXPSIZ[option])
        length=MAXPSIZ[option];  /* print up to printer width only */
    length &= ~0x0f;             /* length has to be multiple of 16 */
    prtpc(&header[option],length);     /* print printer control code */
    NULLPRT = header[option].factor-1; /* null print factor, see user manual */

    for( loop=0; loop < height; ) {    /* calculate addr. for 1 dot of lines */
        ptr[loop] = calcaddr(0,y++);
        outw[loop++]=0;
    }
    for ( ; loop < ndot ;)
        outw[loop++]=0;          /* clear remaining word to 0 if necessary */
    for (length=length>>4; length > 0; length--) {
        for (loop=0; loop<height; loop++)   /* read one word from every line */
            outw[loop] = exchange(fr_read(ptr[loop]++));
        prt(outw);               /* put them into output buffer */
    }
   length=optprt(option);    /* remove trailing zeros */
   if (length) prtpc(&endg,length);      /* include an carriage return */
   pbflush();         /* flush the buffer */
}

/* prints one pass in vertical mode 1 with graphics data only */
static prtg1(option,y,height)
int option,y,height;
{
    int far *ptr[ndot];
    int outw[ndot], length;
    register int loop;

    option &= 0x7f;
    if (option >= NHEADER) option=0;
/*    if (y+height*dpp > MAXPH)
        height=(MAXPH-y+dpp-1)/dpp; */
    if (height > ndot)
        height=ndot;
    else if (y+height*dpp-dpp+1 > MAXPH)
        height=(MAXPH-y+dpp-1)/dpp;
    if (height==0) return;

    if ((length = XLIMIT+1) > MAXPSIZ[option])
        length=MAXPSIZ[option];
    length &= ~0x0f;
    prtpc(&header[option],length);      /* print control codes */
    NULLPRT = header[option].factor-1;  /* set up null print value */

    for( loop=0; loop < height; ) {     /* get one word from each line */
        ptr[loop] = calcaddr(0,y);
        y+=dpp;                         /* skip dpp line */
        outw[loop++]=0;
    }
    for ( ; loop < ndot ;)      /* clear remaining words if necessary */
        outw[loop++]=0;
    for (length=length>>4; length > 0; length--) {
        for (loop=0; loop<height; loop++)
            outw[loop] = exchange(fr_read(ptr[loop]++));
        prt(outw);      /* put them into buffer */
    }
    length=optprt(option);     /* remove trailing zeros */
    if (length) prtpc(&endg,length);
    pbflush();          /* flush the print buffer */
}
/* prints the frame starting from y for height given in vertical mode 0 */
static printg0(option,y,height)
int option;
register int y, height;
{
    int count,pass;

    for(; height>0; y+=ndot, height-=ndot) {
        prtg0(option,y,height);
        skippt(height);
    }
}

/* prints the frame starting from y for height given for vertical mode 1 */
static printg1(option,y,height)
int option;
register int y,height;
{
    int count1;

    for (; height>0 ; height-=ndot, y+=(ndot-1)*dpp) {
        prtg1(option,y++,height);
        for(count1=dpp-1; count1>0; count1--) {
            skipdot(1); /* skip a little */
            prtg1(option,y++,height);
        }
        skipdot((height-1)*dpp+1);      /* big skip */
    }
}

/* skip desire number of dots */
skiplong(number)
int number;
{
    prtpc(&skp,number);
    pbflush();
}

/* skip not more than ndot of dots */
skippt(number)
int number;
{
    if (number > ndot) number=ndot;
    prtpc(&skp,number);
    pbflush();
}

skipdot(number)
int number;
{
    if (number > (ndot-1)*dpp+1) number=(ndot-1)*dpp+1;
    prtpc(&skd,number);
    pbflush();
}

/* print a line of text */
ptext(cptr)
register char *cptr;
{
    while (*cptr) {
        if (*cptr==0x80) *cptr=0;
        prtc(*cptr++);
    }
    prtc(0x0d);
    pbflush();
}

/* print control codes */
static prtpc(pcptr,number)
struct prtcmd *pcptr;
int number;
{
    int loop,temph,templ,len;

    number *= pcptr->factor;
    temph = pcptr->numh;
    templ = pcptr->numl;
    len = pcptr->length;
    for (loop=0; loop<len; loop++) {
        if (loop==temph)
            prtc((number>>8) & 0xff);
        else if (loop==templ)
            prtc(number & 0xff);
        else
            prtc(pcptr->code[loop]);
    }
}

/* optimize print length */
static optprt(option)
int option;
{
    int tlen, temp;

    tlen=header[option].length;
    if ((tlen=rmv0(tlen)) == 0) return(0);      /* remove trailing zero */
    if ((temp=header[option].numh) >= 0) {
        CFBUF[temp] = (tlen >> 8) & 0xff;
    }
    if ((temp=header[option].numl) >= 0) {
        CFBUF[temp] = tlen & 0xff;
    }
    return(PBCOUNT);    /* number of characters in buffer */
}

