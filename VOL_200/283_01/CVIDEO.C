
/* CVIDEO.C - after Microsoft Systems Journal, Nov. 1988, pp. 3-4 */

#include "video.h"
#include <stdlib.h>
#include <malloc.h>
#include <dos.h>


/* Considerations:  The original CVIDEO.C included calls to int86()
                    indiscriminately in places other than GetVideoParms().
                    This destroys the generality of the algorithm, by
                    making it impossible to write to a general buffer
                    other than the physical screen buffer, by forcing
                    some CVIDEO routines to call ROM BIOS.

                    What I've done is to add a couple of service routines
                    to ASMVIDEO.ASM, and remove all int86()'s from this
                    and my own libraries, with the exception of routines
                    which read or update the physical cursor location, or
                    which initialize the module.

                    See additional opinions in VIDEO.H.

                    12/10/88, d.c.oshel
                    */

struct MSJ_VideoInfo video = { 0 };

static union REGS inregs, outregs;
static int i, j, n, sz, sza;
static char far * p;
static char * q;

/* Functions */

/*-------------------------------------------------------------*/
/* MSJ_GetVideoParms()  fills a struct MSJ_VideoInfo with data */
/*-------------------------------------------------------------*/

void MSJ_GetVideoParms( struct MSJ_VideoInfo * sptr)
{
	char far *CrtMode    = (char far *) 0x00400049;
    char far *CrtCols    = (char far *) 0x0040004A;
    int  far *CrtStart   = (int  far *) 0x0040004E;
    int  far *CrtAddr    = (int  far *) 0x00400063;
    char far *CrtRows    = (char far *) 0x00400084;
    char far *CrtEgaInfo = (char far *) 0x00400087;

    /* -- synchronize video writes? -- */

    inregs.h.ah = 0x12;
    inregs.h.bl = 0x10;
    int86( 0x10, &inregs, &outregs );
    if ( outregs.h.bl == 0x10 )
    {
    	if ( *CrtAddr == 0x3D4 )
            sptr->SnowFlag = 1;
        else
            sptr->SnowFlag = 0;
        sptr->rows = 25;
    }
    else
    {
    	if ( (*CrtEgaInfo & 0x08) && (*CrtAddr == 0x3D4) )
            sptr->SnowFlag = 1;
        else
            sptr->SnowFlag = 0;
        sptr->rows = *CrtRows + 1;
    }


    /* -- color or monochrome? -- */

    if (*CrtAddr == 0x3D4)
    {
    	sptr->ColorFlag = 1;
        sptr->SegAddr = 0xB800;
    }
    else
    {
    	sptr->ColorFlag = 0;
        sptr->SegAddr = 0xB000;
    }

    /* -- copy remaining parameters from BIOS -- */

    sptr->mode = *CrtMode;
    sptr->columns = *CrtCols;
    sptr->BufferStart = *CrtStart;
}



/*----------------------------------------------*/
/* MSJ_ClrScr()  clears the entire viewing area */
/*----------------------------------------------*/

void MSJ_ClrScr( char VideoAttr, struct MSJ_VideoInfo *sptr )
{
    MSJ_SetFldAttr( ' ',0,0,VideoAttr,(sptr->rows * sptr->columns),sptr );
}



/*-----------------------------------------------------*/
/* MSJ_ClrRegion()  clears the specified screen region */
/*-----------------------------------------------------*/

void MSJ_ClrRegion( char row1, char col1, char row2, char col2, char VideoAttr )
{
    for ( i = row1; i <= row2; i++ )
        MSJ_SetFldAttr( ' ', i, col1, VideoAttr, col2-col1+1, &video );
}



/*--------------------------------------------------------*/
/* MSJ_TextBox()  draws a box around the specified region */
/*--------------------------------------------------------*/

void MSJ_TextBox( char row1, char col1, char row2, char col2, char VideoAttr, struct MSJ_VideoInfo *sptr )
{
    MSJ_DispCharAttr( 0xDA, row1, col1, VideoAttr, sptr );
    for ( i = col1 + 1; i < col2; i++ )
        MSJ_DispCharAttr( 0xC4, row1, i, VideoAttr, sptr );
    MSJ_DispCharAttr( 0xBF, row1, col2, VideoAttr, sptr );
    for ( i = row1 + 1; i < row2; i++ )
        MSJ_DispCharAttr( 0xB3, i, col2, VideoAttr, sptr );
    MSJ_DispCharAttr( 0xD9, row2, col2, VideoAttr, sptr );
    for ( i = col2 - 1; i > col1; i-- )
        MSJ_DispCharAttr( 0xC4, row2, i, VideoAttr, sptr );
    MSJ_DispCharAttr( 0xC0, row2, col1, VideoAttr, sptr );
    for ( i = row2 - 1; i > row1; i-- )
        MSJ_DispCharAttr( 0xB3, i, col1, VideoAttr, sptr );
}



/*-------------------------------------------------------------------*/
/* MSJ_SaveRegion()  saves the contents of a specified screen region */
/*-------------------------------------------------------------------*/


void MSJ_SaveRegion( char row1, char col1, 
                     char row2, char col2, 
                     int * ScreenBuffer, 
                     struct MSJ_VideoInfo * sptr )
{
    /* this folderol is to provide memory-model independence, considering
       both Prosise's prototype and my own primitives
       */

    n = col2-col1+1;
    sz = n * sizeof(int);
    sza = (row2-row1+1) * sz;

    p = (char far *) _fmalloc( sza );
    q = (char *) ScreenBuffer;

    for ( j = 0, i = row1; i <= row2; j++, i++ )
        MSJ_MovScrBuf( p+(j*sz), i, col1, n, &video );

    for ( i = 0; i < sza; i++ )
        *(q+i) = *(p+i);

    _ffree(p);
}


/*----------------------------------------------------------------------*/
/* MSJ_RestRegion()  restores the contents of a specified screen region */
/*----------------------------------------------------------------------*/

void MSJ_RestRegion( char row1, char col1, 
                     char row2, char col2, 
                     int * ScreenBuffer, 
                     struct MSJ_VideoInfo * sptr )
{
    /* this folderol is to provide memory-model independence, considering
       both Prosise's prototype and my own primitives
       */

    n = col2-col1+1;
    sz = n * sizeof(int);
    sza = (row2-row1+1) * sz;

    p = (char far *) _fmalloc( sza );
    q = (char *) ScreenBuffer;

    for ( i = 0; i < sza; i++ )
        *(p+i) = *(q+i);

    for ( j = 0, i = row1; i <= row2; j++, i++ )
        MSJ_MovBufScr( p+(j*sz), i, col1, n, &video );

    _ffree(p);
}

/* eof */

