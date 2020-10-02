/*
HEADER:		;
TITLE:		Get free diskspace;
VERSION:	1.0;

DESCRIPTION:	Calculates the amount of free space on disk.  The drive
		number is entered on the command line  as in the following
		example: fbyte c:.  If no drive number is entered then the
		current drive is assumed.;

KEYWORDS:	Dos utilities;
SYSTEM:		MSDOS;
FILENAME:	FBYTE;
WARNINGS:	None;

AUTHORS:	Dr. Ronald J. Terry;
COMPILERS:	Turbo C;
*/
#include <stdio.h>
#include <dos.h>

void main(int argc, char *argv[]) /* get command line arguments */
{
    struct dfree space;
    unsigned long freespace;
    unsigned long c,bps,spc;
    unsigned char drive[3],drvno,curdrv;
    argc = argc + 0;
    curdrv = 'A' + getdisk();
    strncpy(drive,argv[1],2);
    drive[3] = NULL;
    drive[0] = toupper(drive[0]);
    if(drive[0])
    {
    drvno = drive[0] - 'A'+ 1;  /* determine drive number */
    }
    else
    {
    drvno = 0;
    drive[0] = curdrv;
    }
    getdfree(drvno,&space);        /* get disk free space */
    c = (unsigned long) space.df_avail;
    bps =(unsigned long) space.df_bsec;
    spc = (unsigned long) space.df_sclus;
    if(space.df_sclus==0xffff)
    printf("Can't determine available space");
    else
    {
    freespace = c*bps*spc;    /* calculate space in bytes */
    printf("Space available on drive %c: = %lu\n",drive[0],freespace);
    }
}

