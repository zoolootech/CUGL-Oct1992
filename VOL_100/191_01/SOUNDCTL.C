
/*  SOUNDCTL.C                          Last update: 27 Feb 86  */

/* ------------------------------------------------------------ */
/*      This is a portion of the SOUND EFFECTS LIBRARY.         */
/*                                                              */
/*      Copyright (C) 1986 by Paul Canniff.                     */
/*      All rights reserved.                                    */
/*                                                              */
/*      This library has been placed into the public domain     */
/*      by the author.  Use is granted for non-commercial       */
/*      pusposes, or as an IMBEDDED PORTION of a commercial     */
/*      product.                                                */
/*                                                              */
/*      Paul Canniff                                            */
/*      PO Box 1056                                             */
/*      Marlton, NJ 08053                                       */
/*                                                              */
/*      CompuServe ID: 73047,3715                               */
/*                                                              */
/* ------------------------------------------------------------ */



#define DEBUG 0

#include <stdio.h>
#include "sound.h"

#if C_AZTEC
#include <model.h>
#endif

#if C_ECOSOFT
#include <dos.h>
#endif

#if C_LATTICE
#include <dos.h>
#endif

#define HTMRINT 0x8
#define UTMRINT 0x1c
#define BRKINT 0x23
#define SPAREINT 0x60
#define OLDINT 0x61

typedef int (*PFI)();

static long old_usr_tmr_int=0;          /* Old user timer int vector */
static long old_hw_tmr_int=0;           /* Old hardware timer int vector */
static long old_break_int=0;            /* Old Ctrl-break int vector */
static int inited = 0;                  /* Current system state */

/* -------------------------------------------------------------------- *
 *      MUS_OPEN                Open music routines.                    *
 *      int mus_open(bm)                                                *
 *      int bm;                                                         *
 *                                                                      *                                                                      
 *      Sets interrupt vectors, stores needed sgement values.           *
 *                                                                      *
 *      bm = break mode.  If 1, a Ctrl-break handler is set up, which   *
 *              will (a) shut down the music stuff, then (b) call       *
 *              whatever break handler was previously in effect.        *
 *                                                                      *
 *      Returns:         0  Open OK                                     *
 *                      -1  If interrupt at 0x60 already in use         *
 *                      -2  If already "open"                           *
 * -------------------------------------------------------------------- */

static int current_mode = 1;

int sound_init(mode,ibh)
int mode,ibh;
{
    extern long install(), snd_giv();
    extern int snd_irh(), snd_brk();
    long vec;

#if DEBUG
    printf("In sound init, mode = %d\n",mode);
#endif

    if (mode < 1 || mode > 5) return -1;        /* Check parameter range */

    sound_done();                               /* Clean up if necessary */

    if (snd_giv(SPAREINT) != 0)             /* Is "spare int" in use? */
        return -1;

    if (snd_giv(OLDINT) != 0)               /* Is "old int" in use? */
        return -1;

    if (ibh) old_break_int = install(BRKINT,snd_brk);                        
     else    old_break_int = 0;


        /* NOTE:  Modes 3 and 5 not currently in operation! */
        /*        Treated as modes 2 and 4 respectively     */

    if (mode == 3 || mode == 5)
        --mode;

    if (mode == 2 || mode == 4)
    {
        old_usr_tmr_int = snd_giv(UTMRINT);
#if DEBUG
        printf("Placing old user timer vector %8lx at %02x\n",
            old_usr_tmr_int,OLDINT);
#endif
        snd_siv(OLDINT,old_usr_tmr_int);
        install(UTMRINT,snd_irh);
        old_hw_tmr_int = 0;
    }

#if DEBUG
    printf("INT 0x%2x = %08lx\n",HTMRINT,snd_giv(HTMRINT));
    printf("INT 0x%2x = %08lx\n",UTMRINT,snd_giv(UTMRINT));
    printf("INT 0x%2x = %08lx\n",BRKINT,snd_giv(BRKINT));
    printf("INT 0x%2x = %08lx\n",SPAREINT,snd_giv(SPAREINT));
    printf("INT 0x%2x = %08lx\n",OLDINT,snd_giv(OLDINT));
#endif

    inited = 1;
    current_mode = mode;

#if DEBUG
    printf("Init done, mode set to %d\n",current_mode);
#endif

    return 0;
}


/* -------------------------------------------------------------------- *
 *      MUS_CLOSE               Close music routines.                   *
 *      int mus_close()                                                 *
 *                                                                      *                                                                      
 *      Resets interrupt vectors.                                       *
 *                                                                      *
 *      Returns:         0  Close OK                                    *
 *                      -2  If not "open"                               *
 * -------------------------------------------------------------------- */

sound_done()
{
#if DEBUG
    printf("In sound_done, hushing speaker ...\n");
#endif

    quiet();

    if (old_break_int != 0)
    {
#if DEBUG
        printf("  Restoring BRK\n");
#endif
        snd_siv(BRKINT,old_break_int);
        old_break_int = 0;
    }

    if (old_usr_tmr_int != 0)
    {
#if DEBUG
        printf("  Restoring UTMR\n");
#endif
        snd_siv(UTMRINT,old_usr_tmr_int);
        old_usr_tmr_int = 0;
    }

    if (old_hw_tmr_int != 0)
    {
#if DEBUG
        printf("  Restoring HTMR\n");
#endif
        snd_siv(HTMRINT,old_hw_tmr_int);
        old_hw_tmr_int = 0;
    }

#if DEBUG
    printf("  Clearing Spare & Old Timer\n");
#endif
    snd_siv(SPAREINT,0L);
    snd_siv(OLDINT,0L);

    inited = 0;

    return 0;
}


int sound_mode()
{
    return current_mode;
}


#if C_AZTEC
#define COMMON_INSTALL 1
typedef unsigned SEGREGS[4]; 
#define SEGADDR(x) (x)
#define XCSEG(x) (x[0])
#ifdef _LARGECODE
#define LPROG 1
#else
#define LPROG 0
#endif
#endif

#if C_LATTICE
#define COMMON_INSTALL 1
typedef struct SREGS SEGREGS; 
#define SEGADDR(x) (&(x))
#define XCSEG(x) ((x).cs)
#ifdef I8086P
#define LPROG 1
#endif
#ifdef I8086L
#define LPROG 1
#endif
#ifndef LPROG
#define LPROG 0
#endif
#endif

#if C_ECOSOFT
#define COMMON_INSTALL 1
#define LPROG 0
typedef struct SREG SEGREGS;
#define SEGADDR(x) (&(x))
#define XCSEG(x) ((x).cs)
#endif


#if COMMON_INSTALL

static long install(v,fp)
int v;
PFI fp;
{
    long oldvec, vec, snd_giv();
    SEGREGS sr;

    oldvec = snd_giv(v);
#if DEBUG
    printf("Install %02x  Old = %08lx ",v,oldvec);
#endif

#if LPROG
    snd_siv(v,fp);
#if DEBUG
    printf(" Long New = %08lx \n\n",fp);
#endif
#else
    segread(SEGADDR(sr));
    vec = ((long) XCSEG(sr) << 16) | (long) ((unsigned) fp & 0xffff);
#if DEBUG
    printf(" CS = %04x   Short New = %08lx \n\n",XCSEG(sr),vec);
#endif
    snd_siv(v,vec);
#endif

#if DEBUG
    printf("done\n");
#endif
    return oldvec;
}

#endif


