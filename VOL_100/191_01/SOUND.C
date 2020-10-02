
/*  SOUND.C                             Last update: 26 Feb 86  */

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


extern struct raw_sound soundbuff[SND_BUFFSIZE];
extern unsigned sb_inptr, sb_outptr;


int sound(f,d)
long f;
unsigned d;
{
    int mode;
    unsigned np;

    if ((mode = sound_mode()) == 1)
    {
#if DEBUG
        printf("FG\n");
#endif
        if (f == 0)
        {
            spkr_off();
            delay(d);
            return 0;
        }
        if (spkr_freq(f) == -1)
            return -1;
        else
        {
            spkr_on();
            delay(d);
            spkr_off();
            return 0;
        }
    }
    else
    {
#if DEBUG
        printf("BG i=%d o=%d\n",sb_inptr,sb_outptr);
#endif

        if ((np = ((sb_inptr + 1) % SND_BUFFSIZE)) == sb_outptr)
            return -1;  /* Buffer overflow */

#if DEBUG
        printf("Sound duration is %d msec or %d ticks\n",d,MS2TICKS(d));
#endif
        soundbuff[np].count = FREQ2CTR(f);
        soundbuff[np].ticks = MS2TICKS(d);
        sb_inptr = np;
        if (mode <= 3)
            return bwait();
        else
            return 0;
    }
}


int sounds(n,elem)
int n;
struct sound_element *elem;
{
    while (n--)
        if (sound(elem->freq,elem->dur) == -1)
            return -1;

    return 0;
}


    /* BWAIT waits a while until the buffer empties. */
    /*  (It has a timeout counter and will return -1 */
    /*   if it times out before the buffer empties)  */

static int bwait()
{
    register int i = MAXINT;

    while (sb_inptr != sb_outptr)
    {
        if (--i == 0) return -1;
         else         dummy();      /* Force no aliasing */
    }

    return 0;
}

static int dummy()
{
    return 0;
}
