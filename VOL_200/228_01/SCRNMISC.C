/*
HEADER:         CUGXXX;
TITLE:          (MS-DOS) System-independent sound generation;
DATE:           10-30-84;
DESCRIPTION:    System functions to set, move, and write to CRT;
KEYWORDS:       Sound generation, system functions;
FILENAME:       SCRNMISC.C;
WARNINGS:       IBM-PC and close compatibles;
AUTHORS:        Garth Kennedy;
COMPILER:       Lattice C V2.12;
REFERENCES:     US-DISK 1310;
ENDREF
*/
/**
* These are functions written using the Lattice 2.12 compiler
* that set, move and write to the CRT. Standard DOS Calls are
* used. This is extracted from a larger program
*
* Garth Kennedy    Hinsdale, Ill  9 Nov 1984
**/

#include "_main.c"
int curpos[6];		       /* cursor position array */
/**
* curpos[0] - text row, 	     curpos[3] - dot row
* curpos[1] - text column	     curpos[4] - dot column
* curpos[2] - display page number    curpos[5] - dot color
**/

/**
* scrnclr()
*
* Clear screen	  regardless of mode
*
* Garth Kennedy  29 Oct 1984
**/

scrnclr()
{
    int intno = 0x10;		 /* interrupt to VIDEO_IO */
    union REGS inregs;		 /* input regs */
    union REGS outregs; 	 /* output regs */

    inregs.h.ah = 0x0F; 	 /* return current video state */
    int86(intno,&inregs,&outregs);

    inregs.h.ah = 0x00; 	 /* set display mode */
    inregs.h.al = outregs.h.al;    /* to current mode */

    int86(intno,&inregs,&outregs);

    return;
}
/**/

/**
* scrnpos(x,y)
*
* Move the cursor the the row, column set by x,y
*
* Garth Kennedy  29 Oct 1984
**/

scrnpos(x,y)
int x;		/* row */
int y;		/* column */
{
    int intno = 0x10;		 /* interrupt to VIDEO_IO */
    union REGS inregs;		 /* input regs */
    union REGS outregs; 	 /* output regs */

    inregs.h.ah = 0x02; 	 /* set cursor position */
    inregs.h.dh = x;		 /* row to go to */
    inregs.h.dl = y;		 /* column to go to */
    inregs.h.bh = 0x00; 	 /* page 0 */

    int86(intno,&inregs,&outregs);

    return;
}
/**/
/**
*  scrnwhr(curpos)
*
* return position (row/column) and page number
* curpos [0] - row, curpos[1] - column, curpos[2] - page number
* curpos[3] - dot row , curpos[4] - dot column, curpos[5] - color
* Garth Kennedy  30 Oct 1984
**/

scrnwhr(curpos)
int curpos[];
{
    int intno = 0x10;		 /* interrupt to VIDEO_IO */
    union REGS inregs;		 /* input regs */
    union REGS outregs; 	 /* output regs */

    inregs.h.ah = 0x0F; 	 /* return current video state */
    int86(intno,&inregs,&outregs);
    curpos[2] = outregs.h.bh;		/* current page */

    inregs.h.bh = outregs.h.bh;  /* move current page */
    inregs.h.ah = 0x03; 	 /* read cursor position */
    int86(intno,&inregs,&outregs);
    curpos[0] = outregs.h.dh;		 /* row */
    curpos[1] = outregs.h.dl;		 /* column */
    return;
}
/**/
/**
* setcrt(al)
*
* Set the CRT display mode
*  al = 0    40X25 BW Text	 al = 4  320X200 Color Graphics
*  al = 1    40X25 Color Text	 al = 5  320X200 BW Graphics
*  al = 2    80X25 BW Text	 al = 6  640X200 BW Graphics
*  al = 3    80X25 Color Text
*
* Garth Kennedy  9 Nov 1984
**/
setcrt(al)
int al;
{
    int intno = 0x10;		 /* interrupt to VIDEO_IO */
    union REGS inregs;		 /* input regs */
    union REGS outregs; 	 /* output regs */

    inregs.h.ah = 0x00; 	 /* set display mode */
    inregs.h.al = al;		 /* to requested mode */

    int86(intno,&inregs,&outregs);

    return;
}
/**/
/**
* grfpos(dx,dy,l)
*
* position the graphics "cursor" and turn to dot at that point to
* the color given by l.- dx - row, dy - column
*  !!! This does nothing with the text cursor  !!!
*  Garth Kennedy  9 Nov 1984
**/

grfpos(dx,dy,l)
int dx,dy,l;
{
    int intno = 0x10;		 /* interrupt to VIDEO_IO */
    union REGS inregs;		 /* input regs */
    union REGS outregs; 	 /* output regs */

    inregs.h.ah = 0x0C; 	 /* write dot */
    inregs.x.dx = dx;		 /* row to go to */
    inregs.x.cx = dy;		 /* column to go to */
    inregs.h.al = l;		 /* color value */

    int86(intno,&inregs,&outregs);

    return;
}

/**
*  grfwhr(curpos)
*
* return color given dot position (row/column)
* curpos [0] - row, curpos[1] - column, curpos[2] - page number
* curpos[3] - dot row , curpos[4] - dot column, curpos[5] - color
* Garth Kennedy  9 Nov 1984
**/

grfwhr(curpos)
int curpos[];
{
    int intno = 0x10;		 /* interrupt to VIDEO_IO */
    union REGS inregs;		 /* input regs */
    union REGS outregs; 	 /* output regs */

    inregs.x.dx = curpos[3];	 /* dot row */
    inregs.x.cx = curpos[4];	 /* dot column */
    inregs.h.ah = 0x0D; 	 /* read dot color */
    int86(intno,&inregs,&outregs);
    curpos[5] = outregs.h.al;	 /* pull out color of dot */
    return;
}
