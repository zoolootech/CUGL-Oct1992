/*=========================================================

 TITLE disk_io.c.

 This module cotains a functions which interface either
 directly with DOS or BIOS in order to perform disk i/o.

==========================================================*/


#include <stdio.h>
#include <dos.h>

extern union REGS inregs,outregs;
extern int drive,track,sector,head,sect_size;

/*-----------------Function Prototypes----------------------*/

void    disk_recal(int);
int     disk_reset(void);
int     disk_read(int,int,int,int,char *,int);
int     disk_write(int,int,int,int,char *,int);
int     disk_seek(int,int,int,int);
int     disk_format(int,int,int *,int,int,int,int);
void    disk_test(int,int,int,int);

/*----------------------------------------------------------*/

/*
** Declare all disks uninitialised, and init track,sector
** and head info.
*/

int disk_reset() {

    inregs.h.ah = 0;
    int86(0x13,&inregs,&outregs);
    return outregs.x.ax;

}

/*
** Read nsect's into *bufptr.
** Up to five retries are made if an error occurs.  Register
** ax returns the error code.
*/

int disk_read(drv,trk,sect,hd,bufptr,nsect)
int drv,trk,sect,hd,nsect; char *bufptr; {

struct SREGS segregs;
int i,j,reg_ax,cflag;
char    far *longptr;

    longptr = (char far *)(bufptr);
    for (j = 0; j < nsect; j++) {
        for (i = 2; i > 0; i--) {
            segregs.es  = FP_SEG(longptr);
            inregs.x.bx = FP_OFF(longptr + j * sect_size);
            inregs.h.ah = 2;
            inregs.h.al = 1;                    /* read one sector at a time */
            inregs.h.dl = drv;
            inregs.h.dh = hd;
            inregs.h.ch = trk;
            inregs.h.cl = sect + j;
            int86x(0x13,&inregs,&outregs,&segregs);
            reg_ax = outregs.x.ax;              /* save the status */
            cflag  = outregs.x.cflag;
            if (!cflag) break;                  /*  no error    */
            disk_reset();
            disk_recal(drv);                    /* try to recover the error */
        }
        if (cflag) {
            j = (j ? 0 : j - 1);            /* back up to sector before bad one*/
            disk_seek(drv,trk,sect+j,hd);       /* return to last position */
            break;
        }
    }
    return (!cflag ? 0 : reg_ax);   /* return error in ax */
}
/*
 Write nscet's from *bufptr.
 Up to five retries are made if an error occurs.  Register
 ax returns the error code.
*/

int disk_write(drv,trk,sect,hd,bufptr,nsect)
int drv,trk,sect,hd,nsect; char *bufptr; {

struct SREGS segregs;
int i,j,reg_ax,cflag;
char    far *longptr;

    longptr = (char far *)(bufptr);
    for (j = 0; j < nsect; j++) {
        for (i = 2; i > 0; i--) {
            segregs.es  = FP_SEG(longptr);
            inregs.x.bx = FP_OFF(longptr + j * sect_size);
            inregs.h.ah = 3;
            inregs.h.al = 1;            /* write one at a time */
            inregs.h.dl = drv;
            inregs.h.dh = hd;
            inregs.h.ch = trk;
            inregs.h.cl = sect + j;
            int86x(0x13,&inregs,&outregs,&segregs);
            reg_ax = outregs.x.ax;          /* save the status */
            cflag  = outregs.x.cflag;
            if (!outregs.x.cflag) break;
            disk_reset();
            disk_recal(drv);            /* try to recover the error */
        }
        if (cflag) {
            j = (j ? 0 : j - 1);            /* back up to sector before bad one*/
            disk_seek(drv,trk,sect,hd);     /* return to last position */
            break;
        }
    }
    return (!cflag ? 0 : reg_ax);               /* return error in ax */
}

/*
** Seek to track ,sector and head.
*/

int disk_seek(drv,trk,sect,hd)
int drv,trk,sect,hd; {

struct SREGS segregs;
int i,reg_ax;

    inregs.h.ah = 4;
    inregs.h.al = 1;
    inregs.h.dl = drv;
    inregs.h.dh = hd;
    inregs.h.ch = trk;
    inregs.h.cl = sect;
    int86x(0x13,&inregs,&outregs,&segregs);
    return (outregs.x.ax);              /* return status */
}
/*
** Format a track according to the Alien Disk parameters
*/

int disk_format(fdrive,ftrack,skew,fhead,fbytes,fsect,nsect)
int fdrive, ftrack, *skew, fhead, fbytes,fsect,nsect; {

struct SREGS segregs;
int i,reg_ax;
char format_buf[256];
char far *fptr;

    fptr = format_buf;
    for (i = fsect; i <= nsect; i++) {  /* build the format table IBM Tech 5-56 */
        *fptr++ = ftrack;
        *fptr++ = fhead;
        *fptr++ = *(skew + i);
        *fptr++ = fbytes;
    }
    fptr = format_buf;
    for (i = 5; i > 0; i--) {
        inregs.x.bx = FP_OFF(fptr);     /* seg:off of format table */
        segregs.es  = FP_SEG(fptr);
        inregs.h.ah = 5;            /* format function */
        inregs.h.dl = fdrive;
        inregs.h.dh = fhead;
        inregs.h.ch = ftrack;
        int86x(0x13,&inregs,&outregs,&segregs);
        if (!outregs.x.cflag) return (0);
        reg_ax = outregs.x.ax;          /* save the status */
        disk_recal(fdrive);         /* try to recover the error */
    }
    return (reg_ax);                /* return error in ax */
}

/*
**  A simple Disk Test performing a verify on the sector.
*/

void disk_test(drv,hd,trk,sect) char drv,hd,trk,sect; {

    inregs.h.ah = 4;        /* just verify  */
    inregs.h.al = 1;        /* just read one sector */
    inregs.h.dl = drv;
    inregs.h.dh = hd;
    inregs.h.ch = trk;
    inregs.h.cl = sect;
    int86(0x13,&inregs,&outregs);

}

void disk_recal(drv) int drv; {

    inregs.h.ah = 4;
    inregs.h.al = 1;        /* just read one sector */
    inregs.h.dl = drv;
    inregs.h.dh = 0;
    inregs.h.ch = 0;
    inregs.h.cl = 1;
    int86(0x13,&inregs,&outregs);

}
