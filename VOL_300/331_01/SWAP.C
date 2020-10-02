/*
HEADER:         CUG199.09;
TITLE:          GED (nee QED) screen editor -- part 9;
DATE:           10/10/86;

DESCRIPTION:   "Virtual memory manager for GED editor. Eg. swapout,
                pageout.";
KEYWORDS:       text storage, memory management, virtual storage, paging;
SYSTEM:         MS-DOS;
FILENAME:       GED9.C;
AUTHORS:        G. Nigel Gilbert, James W. Haefner, Mel Tearle, G. Osborn;
COMPILERS:      DeSmet C;
*/

/*
     e/qed/ged  screen editor

    (C) G. Nigel Gilbert, MICROLOGY, 1981
           August-December 1981

    Modified:  Aug-Dec   1984:   BDS-C 'e'(vers 4.6a) to 'qe' (J.W. Haefner)
               March     1985:   BDS-C 'qe' to DeSmet-C 'qed' (J.W. Haefner)
               May       1986:   converted to ged - Mel Tearle

    FILE:      ged9.c

    FUNCTIONS: initvm, freememslot, swapout, writepage,
               swappin, readpage, fatalerror

    PURPOSE:   manages virtual memory

*/

#include <stdio.h>
#include "ged.h"

int allo;

/* Initialize virtual memory system.  There are MAXSLOTS pages.  The data in a given
   page may be either in RAM or in a temporary disc file.  The most recently
   used data is kept in RAM.  There are up to RAMSLOTS of pages in RAM.

   If virtslot[page] is positive then the value is the slot number in RAM
   which contains the text for that page.  If negative then the disc slot
   number is -(virtslot[page] + 1).

   Selective use is made of 32-bit pointers.  Most of the program uses 16-bit
   pointers and would be difficult and inefficient to convert to a large
   data model.  Pointer arithmetic on far pointers cannot cross a 64k boundary.
   Huge pointer calculations can cross these boundaries.  The use of far and
   huge pointers is controlled by the preprocessor directives in ged.h.

   If the page size is evenly divisible into 64 k then a page will never
   span a 64 k boundary and far pointers can be used in time critical functions.
   The far pointer arithmetic is significantly faster for far pointers and
   they are used in this version.  There are also places where huge
   pointer arithmetic must be used in order to obtain correct carry
   propogation.

 */
initvm()
{
    int   slot;
    unsigned int size;
    static unsigned long int lsize;
    static char  HUGE * hptr = 0L;
    char  HUGE * cdecl hptr1;
    int i;
    long int ll;
    char  *malloc();
    char *ptr;
    char HUGE * malloc2();
    char buf[40];
    if (sizeof(*tp) != sizeof(long))
        cerr(92);  /* see comments in deltp() */

/* fullmem set by -M load time option */
    if (hptr == 0L) {
        lsize =  (long) fullmem*1024L;
        hptr = malloc2(&lsize);  /* adjusts lsize downward */
    }
    hptr1 = hptr;
    slotsinmem = lsize/PAGESIZE;

    if ( slotsinmem < 3 )  {
        error1( "Can't allocate enough memory" );
        xit();
    }
    allo = lsize/1024L;
    for (slot = 0;  slot < slotsinmem;  slot++) {
        usage[slot] = 0;
        auxloc[slot] = 0;
        slotaddr[slot] = hptr1;
        hptr1 += PAGESIZE;
    }
    for (i = 0;  i < MAXSLOTS;  i++) {
        virtslot[i] = 0;
        dskslots[i] = 0;
    }

    clock = 1;

/* reserve slot 0 for an array of tp structures.  Additional slots
 * will be taken later for tp if needed.
 */
    tp = (struct addr HUGE *)slotaddr[0];
    tpslots  = 1;
    usage[tpslots-1] = -1;

/* assign first page for line injection. */
    newpage = 0;
    virtslot[newpage] = freememslot();
    allocp = 0;
    usage[virtslot[newpage]] = MAXINT;

/* paging file not yet created */
    pagefd = NOFILE;
    return;
}


/* Returns the number of a free memory slot, possibly by swapping out
   the least recently used page currently in memory.  If that page is
   already on disc then the RAM slot is immediately reused.
   On the initial read, text is stored starting at the top of allocated
   memory and working downward.  The tp array works upward from low memory.
   The portion used by tp must be contiguous.  newpage is not swaped out.
 */
int freememslot()
{
    int  use, u;
    int  i, slot, nclk;

    nclk = 1;
    use = MAXINT;
    slot = slotsinmem-1;
    for ( i = slotsinmem-1; (use != 0) && (i >= tpslots); i-- ) {
        if ( ( u = usage[i] ) < use )  {
            use  = u;
            slot = i;
        }
    }
    if ( use > 0 ) {
        swapout( slot );  /* no free slots. make one */
    }
    auxloc[slot] = 0;  /* RAM slot has no disc association */
    if (!goteof)
        usage[slot] = clock++;
    return  slot;
}


/* swaps page currently in memory at 'slot' to disk,
 * updating virtslot to show new location
 */
swapout(slot)
int  slot;
{
    int  pg;

/* find page number for ram data at 'slot' */
    for ( pg = 0; (virtslot[pg] != slot) && (pg < MAXSLOTS); pg++ )
        ;
    if(pg==MAXSLOTS)
        cerr(90);   /* memory is probably clobbered */

    if (auxloc[slot] == 0) {
        virtslot[pg] =  -writepage( slot ) - 1;  /* update virtslot with disk slot written to */
    }
    else {
        virtslot[pg] = auxloc[slot];  /* page already exists on disc */
        auxloc[slot] = 0;  /* RAM slot will lose its disc association with this record */
    }
    usage[slot] = 0;  /* the ram slot is free */
    return;
}


/* Writes page currently in memory at 'slot' to disk.
 * Returns disk slot where written.  Locations near the beginning
 * of the file are used when available to miminize total file size.
 */
writepage(slot)
int slot;
{
    int i;
    unsigned loc;
    long int sekbytes;
    long int lseek1();

    long int dbuf[PAGESIZE/sizeof(long)];
    long int *pt1;
    long int FAR *pt2;


    if ( pagefd == NOFILE )  {       /* haven't opened paging file yet */

        if ( (pagefd = creatb(pagingfile)) == FAIL )
            fatalerror( " Can't create a buffer file" );
        for ( loc = 0; loc < MAXSLOTS; loc++ )
            dskslots[loc] = 0;
    }

    for ( loc = 0; dskslots[loc] != 0; loc++ )   /* find a free slot */
        ;
    if (loc >= MAXSLOTS)
        cerr(93);         /* table corrupt */
    sekbytes = (long) loc*PAGESIZE;

    if ( lseek1( pagefd, sekbytes) == -1L )
        fatalerror(" Bad seek in writing buffer ");

/* move far data to near data */
    pt2 = (long int FAR *) slotaddr[slot];
    pt1 = &dbuf[0];
    for (i = 1; i <= PAGESIZE/sizeof(long); i++)
        *pt1++ = *pt2++;

    if ( write( pagefd, dbuf, PAGESIZE ) == FAIL )
        fatalerror(" Can't write to buffer - disk full " );
    dskslots[loc] = 1;
    usage[slot]   = 0;
    return  loc;
}


/* get 'page', currently on disk,
 * into memory and return slot where placed
 * the disc location is saved in auxloc in case the page needs to be
 * rolled out again without change.
 *
 * The priority is set to 1 when read.  If read for a global string search
 * the priority stays at the lowest priority and the same memory slot will
 * be used for the next page.  The established page priority is therefore
 * mostly unaffected by global searches.  If loaded for gettext() the
 * priority will immediately be raised to 'clock' and the page will
 * usually stay resident for a long time.
 */
swappin(page)
int  page;
{
    int  slot, dslot;
int i;
    if(virtslot[page] >= 0)
        cerr(91);          /* table corrupt */
    slot = freememslot();
    readpage( slot, -1 -virtslot[page] );
    usage[slot] = 1;
    auxloc[slot] = virtslot[page]; /* remember where it came from */
    virtslot[page] = slot;
    return slot;
}


/* read a page from disk into
 * memory at 'memsl'
 */
readpage(memsl,dskslot)
int memsl, dskslot;
{
    long sekbytes;
    int i;
    long int *pt1;
    long int FAR *pt2;
    long int dbuf[PAGESIZE/sizeof(long)];
    char buf[80];
    long int lseek1();

    sekbytes = (long) dskslot*PAGESIZE;
    if ( lseek1( pagefd, sekbytes ) == -1L )
        fatalerror(" Bad seek in reading buffer ");
    i=read( pagefd, dbuf, PAGESIZE );
    if ( i != PAGESIZE )
        fatalerror(" Can't read disc buffer ");

/* move near data to far data */
    pt2 = (long int FAR *) slotaddr[memsl];
    pt1 = &dbuf[0];
    for (i = 1; i <= PAGESIZE/sizeof(long); i++)
        *pt2++ = *pt1++;

    return;
}


/* displays message, returns to a
 * known good screen
 */
fatalerror(message)
char  *message;
{
    error( message );
    moveline( goodline-cline );
    longjmp( 0, 1 );
}

/* There are two pools from which memory is allocated when
   using the Microsoft compiler ver 4.00.  Choose the larger of the two.
 */
char HUGE *malloc2(amnt)
unsigned long int *amnt;
{
    char HUGE *halloc();
    char HUGE *hptr1;
    char *ptr;
    unsigned long int hsize,ssize;
    unsigned int isize;

    ssize = hsize = *amnt+PAGESIZE;

/* halloc always returns a pointer with an offset of 0.  That is required
 * in order to avoid lines crossing a 64 k boundary.  The call to
 * malloc below does not return a 0 offset, but the entire data space
 * in that case is within the span of the DS register so no element
 * anywhere will cross a 64 k boundary.
 */
    do {
        hsize -= PAGESIZE;
        hptr1 = halloc(hsize,1L);
    }
    while ( (hptr1 == 0L) && (hsize > 0L) );

    do {
        ssize -= PAGESIZE;
        if (ssize < 0x10000L)
            ptr = malloc(isize = ssize);
        else
            ptr = 0;
    }
    while ( ( ptr == 0) && (ssize > 0L) );

/* give back the small one unless the small one satisfies the -M option */
    if (hsize > ssize && ssize < *amnt ) {
        free(ptr);
        *amnt = hsize;
        return hptr1;
    }
    else {
        hfree(hptr1);
        *amnt = ssize;
        return (char HUGE *) ptr;
    }
}
putallo()
{
    char buf[50];
    sprintf(buf,"%d kb allocated                  ",allo);
    scr_aputs(43,0,buf,ATTR3);
    return;
}






