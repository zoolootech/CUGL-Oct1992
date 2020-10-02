/*  005   2-Jan-87  volume.c

         This module contains functions to get and set information about
         DOS disk volumes.

         Copyright (c) 1987 by Blue Sky Software.  All rights reserved.
*/

#include <dos.h>

#ifndef NULL
#define NULL (0)
#endif


/******************************************************************************
 **                   C U R R E N T _ D R I V E                              **
 *****************************************************************************/

int
current_drive() {      /* return the current drive # in DOS A, B, ... format */

   return((bdos(25,0,0) & 0xFF)+'A');
}


/******************************************************************************
 **                     C H A N G E _ D R I V E                              **
 *****************************************************************************/

int
change_drive(drive)    /* changes the current drive */
int drive;
{

   return(bdos(14,drive-'A',0));
}


/******************************************************************************
 **                         G E T V O L S I Z                                **
 *****************************************************************************/

getvolsiz(drive,totalp,freep,clszp)    /* return total, free space on volume */
int drive;
unsigned long *totalp, *freep;
unsigned int *clszp;
{
   union REGS r;
   unsigned int clustersiz;

   r.h.ah = 54;                /* get disk free space dos int */
   r.h.dl = drive - 'A' + 1;   /* for drive A = 1, B = 2... */
   intdos(&r,&r);

   if (r.x.ax != -1) {                         /* -1 if error */
      clustersiz = r.x.ax * r.x.cx;
      *totalp =  (long) clustersiz * r.x.dx;
      *freep  =  (long) clustersiz * r.x.bx;
      *clszp = clustersiz;
   } else {
     *totalp = 0;
     *freep  = 0;
     *clszp  = 0;
   }
}


/******************************************************************************
 **                         G E T _ S E T _ V O L                            **
 *****************************************************************************/

int
get_set_vol(op,np)     /* get and or set the volume label of the current disk */
char *op, *np;
{
   union REGS r;
   char buffer[128];
   static struct {                     /* fcb to search for volume label */
      unsigned char extend;
      unsigned char notused[5];
      unsigned char attribute;
      unsigned char drive;
      unsigned char name[11];
      unsigned char misc[25];
   } fcb = { 0xff, 0, 0, 0, 0, 0, 0x08, 0 };

   *op = '\0';                         /* init old name buffer to null string */

   bdos(0x1a,buffer,0);                /* set the DTA addr for search */

   fcb.drive = 0;                      /* want to use current drive */
   strncpy(fcb.name,"???????????",11); /* set fcb name to wildcard search */

   r.h.ah = 17;                        /* search first, fcb */
   r.x.dx = (int) &fcb;                /* offset of the FCB */
   intdos(&r,&r);                      /* let dos do the looking */

   if (r.h.al == 0)                    /* al = 0 if found */
      strncat(op,buffer+8,11);         /* put it in the caller's buffer */

   if (np != NULL)                     /* is label to be set? */

      if (r.h.al == 0) {                       /* is there an existing label? */
         strcpyfill(buffer+24,np,11,' ');      /* move new name to dta */
         r.h.ah = 23;                          /* fcb rename */
         r.x.dx = (int) buffer;
         intdos(&r,&r);

      } else {         /* no existing label, create a new one */

         strcpyfill(fcb.name,np,11,' ');       /* set new name in fcb */
         r.h.ah = 22;                          /* create file */
         r.x.dx = (int) &fcb;
         intdos(&r,&r);
      }

   return(r.h.al == 0xff ? -1 : 0);    /* return 0 if worked okay */
}
