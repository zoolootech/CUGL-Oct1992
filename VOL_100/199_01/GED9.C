/*
Header:          CUG199;
Title:           Module 9 of ged editor;
Last Updated:    11/29/87;

Description:    "PURPOSE: manages virtual memory";

Keywords:        e, editor, qed, ged, DeSmet, MSDOS;
Filename:        ged9.c;
Warnings:       "O file must be present during link for ged";

Authors:         G. Nigel Gilbert, James W. Haefner, and Mel Tearle;
Compilers:       DeSmet 2.51;

References:
Endref;
*/

/*
e/qed/ged  screen editor

(C) G. Nigel Gilbert, MICROLOGY, 1981 - August-December 1981

Modified:  Aug-Dec   1984:  BDS-C 'e'(vers 4.6a) to 'qe' (J.W. Haefner)
           March     1985:  BDS-C 'qe' to DeSmet-C 'qed' (J.W. Haefner)

           May       1986:  qed converted to ged         (Mel Tearle)
           August    1987:  ged converted to MSC 4.0     (Mel Tearle)

File:      ged9.c

Functions: initvm, freememslot, swapout, writepage,
           swappin, readpage, fatalerror
*/


#ifndef  TC
#include "ged.h"
#else
#include "ged.t"
#endif


/* initialise virtual memory system
 */
void initvm()
{
int  slot;

unsigned  int mem, stk;
unsigned  char  *base, *pagemem, *stkaddr;

#ifdef  MSC
  mem = _memavl();
  stk = stackavail();
  slotsinmem = ( mem - stk )/PAGESIZE;
  pagemem = (char *)( 65535 - mem );
#else
  freeall( 5000 );
  stkaddr = _showsp();
  pagemem = _memory();
  slotsinmem = ( stkaddr-STKSPACE-pagemem )/PAGESIZE;
#endif

if ( slotsinmem < 3 )  {
     putstr( " More memory needed to run 'ged' " );
     exit(0);
}

if ( slotsinmem > MAXMEMSLOTS )  slotsinmem = MAXMEMSLOTS;

for ( slot = 0, base = pagemem;
      slot < slotsinmem; slot++, base += PAGESIZE )  {
      usage[slot] = FREE;
      slotaddr[slot] = base;
}
pclock = 0;

/* reserve slot 0 for tp
 */
tp = ( struct addr * )pagemem;

tppages  = 1;
usage[0] = NOTAVAIL;

/* force balloc to find a new page to start
 */
pageloc[(newpage = 0)] = FREE;
allocp = PAGESIZE+1;

/* paging file not yet created
 */
pagefd = NOFILE;
strcpy( pagingfile, " :$$$.@@@" );
pagingfile[0] = ( ( pagingdisk ) ? pagingdisk : ( char )( curdsk+'A' ) );

/* don't remove - needed to initialize environment
 */
puttext();
}


/* returns the number of a free memory slot,
 * possibly by swapping out the least recently
 * used page currently in memory
 */
int freememslot()
{
int  use, u;
int  i, slot;

for ( use = MAXINT, i = 0; use && i < slotsinmem; i++ )
      if ( ( u = usage[i] ) != NOTAVAIL && u < use )  {
             use  = u;
             slot = i;
      }
/* no free slots */
if ( use )
     swapout( slot );
return  slot;
}


/* swaps page currently in memory at 'slot' to disk,
 * updating pageloc to show new location
 */
void swapout(slot)
int  slot;
{
int  *pl;

/* find page number of page at 'slot'
 */
for ( pl = &pageloc[0]; *pl != slot; pl++ );
     *pl =-( writepage( slot ) );
    /* update pageloc with disk slot written to */
}


/* writes page currently in memory at 'slot' to disk;
 * returns disk slot where written
 */
unsigned writepage(slot)
int slot;
{
unsigned loc;
long sekbytes;

if ( pagefd == NOFILE )  {
     if ( ( pagefd = creat( pagingfile ) ) == FAIL )
            fatalerror( " Can't create a buffer file" );
            for ( loc = 0; loc < MAXSLOTS; loc++ )
                  dskslots[loc] = FREE;
}

for ( loc = 0; dskslots[loc] != FREE; loc++ );   /* find a free slot */

sekbytes = 1L*loc*PAGESIZE;

if ( lseek( pagefd, sekbytes, ABSOLUTE ) == -1L )
     fatalerror(" Bad seek in writing buffer ");

if ( write( pagefd, slotaddr[slot], PAGESIZE ) == FAIL )
     fatalerror(" Can't write to buffer - disk full " );

dskslots[loc] = INUSE;
usage[slot]   = FREE;
return  loc;
}


/* get 'page', currently on disk,
 * into memory and return slot where placed
 */
int swappin(page)
int  page;
{
int  slot;

readpage( ( slot = freememslot() ), -pageloc[page] );
usage[slot] = INUSE;

return pageloc[page] = slot;
}


/* read a page from disk into memory at 'memslot'
 */
void readpage(memslot,dskslot)
unsigned  memslot, dskslot;
{
long sekbytes;

sekbytes = 1L*dskslot*PAGESIZE;

if ( lseek( pagefd, sekbytes, ABSOLUTE ) == -1L )
     fatalerror(" Bad seek in reading buffer ");

if ( read( pagefd, slotaddr[memslot], PAGESIZE ) != PAGESIZE )
     fatalerror(" Can't read buffer ");

dskslots[dskslot] = FREE;
}
