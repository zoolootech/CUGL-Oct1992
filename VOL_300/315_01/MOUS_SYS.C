
/* mous_sys.c is a function to support the Microsoft mouse for people */
/* who have mouse.sys but not mouse.lib.  Its use assumes that mouse.sys */
/* has been installed at boot-up.  Written 8/89 by T Clune.  Written for */
/* and Copyright (c) 1989 by Eye Research Institute, Boston MA.  All rights */
/* reserved. NOTE WELL: Does not support mouse calls that return values */
/* in registers SI and DI, but mouselib.c makes no such call. */

#include <dos.h>

/* cmousel() is the mouse software interrupt call.  It's name and type */
/* are used to make them identical to the mouse.lib call. */

int cmousel(a, b, c, d)
int *a, *b, *c, *d;
{
    union REGS regs;
    regs.x.ax= *a;
    regs.x.bx= *b;
    regs.x.cx= *c;
    regs.x.dx= *d;
    int86(0x33, &regs, &regs);
    *a= regs.x.ax;
    *b= regs.x.bx;
    *c= regs.x.cx;
    *d= regs.x.dx;
    	
    return 0;  /* well, it IS int, after all. */
}
