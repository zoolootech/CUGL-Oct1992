#include <stdio.h>
#include <gds.h>
#include <prtfont.h>
#include <fcntl.h>
#include <io.h>

#define ERROR (-1)
#define OK 0

getfontn()
{
    register int loop;

    for (loop=1; loop < MAXNFONT ; loop++) {
        if (FONTTAB[loop].status==NOT_USED) {
            FONTTAB[loop].status=USED;
            return(loop);
        }
    }
    return(ERROR);
}

LoadFont(fontfilename)
char *fontfilename;
{
    int fp,fontn, count, loop;
    char hbuf[16];

    if ((fontn=getfontn()) == ERROR) {
        graderror(4,13);
    }
    if ((fp=open(fontfilename,O_RDONLY | O_BINARY)) == ERROR) {
        graderror(2,6,fontfilename);
        FONTTAB[fontn].status=NOT_USED;
        return(0);
        }
    if ((count=read(fp,CFBUF,256)) <= 0) {
errorexit:
        graderror(4,14);
    }
    if (CFBUF[0]!=51) goto errorexit;
    for (loop=1; loop<=count; loop++) {
        if (CFBUF[loop]==0) goto cont;
    }
    goto errorexit;
cont:
    loop++;
    lseek(fp,(long) loop, SEEK_SET);
    loop+=16;
    if (read(fp,hbuf,16) != 16)
        goto errorexit;
    close(fp);
    FONTTAB[fontn].type=hbuf[0];
    switch(hbuf[0]) {
    case 0:
        readfont0(fontfilename,fontn,hbuf,loop);
        break;
    case 1:
        readfont1(fontfilename,fontn,hbuf,loop);
        break;
    default:
        goto errorexit;
    }
    return(fontn);
}

readfont0(fontfilename,fontn,hbuf,pos)
int fontn, pos;
char hbuf[], *fontfilename;
{
    struct fontspec huge *tptr;
    register struct FONTABLE *ptr;
    unsigned int nuchar;
    long temp;
    int fn;

    void far *farcalloc();

    ptr=&FONTTAB[fontn];
    ptr->start = hbuf[3] + (hbuf[4] << 8);
    tptr=ptr->chinfo=(struct fontspec huge *)
                   farcalloc(1L,(long) sizeof(struct fontspec));
    nuchar=hbuf[1]+(hbuf[2]<<8);
    ptr->lastchar=ptr->start - 1 + nuchar;
    ptr->direction=0;
    ptr->defch = hbuf[11];
    temp=((long) (hbuf[6]+7)>>3) * hbuf[7] * nuchar;
    if ((ptr->fontaddr=
             (char huge *) nor_ptr((int far *) farcalloc(temp,1L))) ==
                   (char huge *) NULL) {
        graderror(4,15);
    }
    fn=dosopen(fontfilename);
    dosseek(fn,pos);
    if (dosread(fn,ptr->fontaddr,temp) != temp) {
        graderror(4,14);
    }
    dosclose(fn);
    tptr->width = hbuf[5];
    tptr->inkwidth = hbuf[6];
    tptr->height = hbuf[7];
    tptr->leftmargin = hbuf[8];
    tptr->topline = hbuf[9];
    tptr->cellheight = hbuf[10];
}

readfont1(fontfilename,fontn,hbuf,pos)
int fontn, pos;
char *fontfilename,hbuf[];
{
    struct fontspec huge *tptr;
    register struct FONTABLE *ptr;
    int nuchar, loop, fn;
    long temp,temp2;
    long huge *szptr;

    void far *farcalloc();

    ptr=&FONTTAB[fontn];
    ptr->start=hbuf[3]+(hbuf[4]<<8);
    nuchar=hbuf[1]+(hbuf[2]<<8);
    ptr->lastchar=ptr->start - 1 + nuchar;
    ptr->direction=0;
    ptr->defch=hbuf[8];
    if ((ptr->szinfo=(long huge *) farcalloc((long) nuchar,(long) sizeof(long)))
                     == (long *) NULL) {
        graderror(4,15);
    }
    if ((tptr=ptr->chinfo=(struct fontspec huge *)
             farcalloc((long) nuchar,(long) sizeof(struct fontspec)))
                == (struct fontspec *) NULL) {
        graderror(4,15);
    }
    fn=dosopen(fontfilename);
    if (dosseek(fn,pos)==(-1)) graderror(4,21);
    temp2=(long) nuchar*sizeof(struct fontspec);
    if (dosread(fn,(char huge *) ptr->chinfo,temp2)
            != temp2) {
        graderror(4,14);
    }
    temp=0L;
    szptr=ptr->szinfo;
    for (loop=0; loop<nuchar; loop++) {
        *szptr++=temp;
        temp+=(long) (((tptr->inkwidth + 7) >> 3) * tptr->height);
        tptr++;
    }
    if ((ptr->fontaddr=(char huge *) farcalloc(temp,1L)) == (char huge *)NULL) {
        graderror(4,15);
    }
    if (dosread(fn,ptr->fontaddr,temp) != temp) {
        graderror(4,14);
    }
    dosclose(fn);
}

int RemvFont(fontn)
int fontn;
{   
    register struct FONTABLE *ptr;

    if ((ptr=&FONTTAB[fontn])->status == NOT_USED) return(ERROR);
    if (ptr -> status == PERMANENT) return(OK);
    if (CURFONT==fontn) CURFONT=0;
    farfree(ptr->chinfo);
    farfree(ptr->fontaddr);
    ptr->status = NOT_USED;
    return(OK);
}


