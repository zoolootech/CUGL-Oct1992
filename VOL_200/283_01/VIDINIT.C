/* vidinit.c -- 9/5/88, d.c.oshel

   This is the part of CIAO that contains initialization and deinitialization,
   plus all global and external data.  There are no references to functions
   other than the C library and the MSJ low-level I/O function in here.
   */

#include "vidinit.h"

/*
** globals
*/

/* struct MSJ_VideoInfo video; <-- this is in LVIDEO.OBJ */

int vid[16] =  /* vid_init() changes this table if cga */
{
    NRM, BRN, RVR, BLR, 
    UNL, BRU, BLN, BLU, 
    BBN, BBU, NRM, NRM,
    NRM, NRM, NRM, NRM
};

int rasterh  = 12, rasterl  = 13;  /* monochrome cursor default raster lines */

int vid_attr = 7,           /* HEAVILY USED HEREIN */
    vid_page = 0,
    row = 0,
    col = 0; 

int activescreen, activeoffset;

int Initialized = 0;        /* are all the critical pointers set? */ 

char far *hidescreen;       /* pointer to invisible screen buffer */

static union REGS old_vid;
static union REGS __xy;     /* holds machine cursor address for set */

int lm = TOPX,
    rm = BTMX,
    tm = TOPY,
    bm = BTMY;              /* default window margins */

int synchronized = 1;       /* default to hard & soft cursors alike */


/*
** Initialize, Deinitialize
*/

static void init_criticals( int mode )
{
    int i;
    union REGS rx;

    if ( (hidescreen  = (char far *) _fmalloc(SCRLIM)) == 0L ) 
           exit(27);

    video.mode = 7;                /* assume monochrome mode */
    rasterh  = 12;                 /* assume monochrome cursor */
    rasterl  = 13;

    rx.h.ah = 15;                  /* read current video mode */
    int86( 0x10, &rx, &old_vid );
    vid_page = old_vid.h.bh;
    video.mode = old_vid.h.al;     /* 7 means we found monochrome hardware */


    if (mode == 0)             /* request to stay in current mode? */
        mode = video.mode;  

    /* in any case, allow only mono mode 7, or cga modes 2 or 3 */

    if (video.mode != 7)       /* is hardware a color/graphics adapter? */
    {
       if ( mode == 3 )        /* asking for color?  set it up! */
       {
          for (i = 0; i < 16; i++) 
              vid[i] = i;                      /* set color defaults */
          vid[0]  = 0x17 | 0x08;               /* set normal         */
          vid[1]  = 0x06 | 0x08;               /*  "  bold           */
          vid[2]  = 0x71 | 0x08;               /*  "  emphasis       */
          vid[3]  = 0x47 | 0x08 | 0x80;        /*  "  attention      */
       }
       else 
       {
          mode = 2;                 /* only modes 2 and 3 are allowed, here */
          for (i = 4; i < 16; i++)  /* all else defaults to normal */
              vid[i] = vid[0];
       }
       rasterh = 6;               /* cga default cursor raster lines */
       rasterl = 7;
       if (video.mode != mode)      /* not already in the requested mode? */
       {
          rx.h.ah = 0;            /* if not, set requested mode */
          rx.h.al = mode;
          int86( 0x10, &rx, &rx );
       }
       rx.h.ah = 15;              /* read current mode again */
       int86( 0x10, &rx, &rx );
       video.mode = rx.h.al;
    }

    Initialized = 1;              /* we are, basically, but...  */
    vid_attr = vid[0];            /* and ensure vid_attr is set */
}




/* Vid_Init() - This is a "MUST CALL", but if the various routines which
**              depend on finding initialized values and pointers find the
**              Initialized flag set to zero, they will take it on themselves
**              to call vid_init(0).  Any argument to vid_init is valid, but
**              mode 7 is set if the monochrome adapter is in the system, and
**              only modes 2 or 3 are set if the cga card is found.  The 
**              0 argument requests that a valid current mode not be changed;
**              this prevents the firmware from clearing the screen, if it's
**              not absolutely necessary.  Vid_init() will always select the
**              visible screen, and synchronize the soft & machine cursors.
**              Also, selects appropriate default cursor size; cga and mono
**              have different cursors.
*/

void vid_init( int mode )
{
    char far *p;
    union REGS x;

    init_criticals( mode );      /* sets initialized flag on exit */

    /* now that the caller's requested mode is set, we must initialize
       the nine low-level MSJ functions along with the video structure
       */
    MSJ_GetVideoParms( &video );

    /* save startup screen, blank if init_criticals changed the video mode */
    setscreen(1);  /* ensure activescreen is set (still a do nothing?) */
    p = savescreen(&x); /* snowfree here too, please! */
    movedata( ptrseg(p), ptrofs(p), ptrseg(hidescreen), ptrofs(hidescreen), SCRLIM );
    _ffree(p);

    __xy.h.ah = 3;            /* read machine cursor into dh, dl */
    __xy.h.bh = 0;
    int86( 0x10, &__xy, &__xy );
    row = __xy.h.dh;
    col = __xy.h.dl;
    synchronized = 1;
}                              




void vid_exit( void )
{
     if (Initialized)
     {
          if (old_vid.h.al != video.mode)
          {
               old_vid.h.ah = 0;
               int86( 0x10, &old_vid, &old_vid );
          }

          __xy.h.ah = 1;
          int86( 0x10, &__xy, &__xy ); /* old cursor size */
          __xy.h.ah = 2;
          int86( 0x10, &__xy, &__xy);  /* old cursor loc */

          swapscreen(1);
          _ffree(hidescreen);
     }
     setcursize( rasterh, rasterl );
     Initialized = 0;  /* we're NOT initialized any more! */
}


