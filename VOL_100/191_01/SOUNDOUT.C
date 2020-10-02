
/*  SOUNDOUT.C                          Last update: 26 Feb 86  */

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


#include <stdio.h>
#include "sound.h"

struct raw_sound soundbuff[SND_BUFFSIZE];
unsigned sb_inptr = 0, sb_outptr = 0;

static unsigned tick_counter = 0;


void sound_out()
{
    unsigned c;

    while (tick_counter-- == 0)
    {
        if (sb_inptr == sb_outptr)
        {
            tick_counter = 0;
            spkr_off();
            return;
        }
        else
        {
            sb_outptr = (sb_outptr + 1) % SND_BUFFSIZE;

            tick_counter = soundbuff[sb_outptr].ticks;
            if ((c = soundbuff[sb_outptr].count) == 0)
                spkr_off();
            else
            {
                spkr_cntr(c);
                spkr_on();
            }
        }
    }
}


void quiet()
{
    sb_inptr = sb_outptr = 0;
    tick_counter = 0;
    spkr_off();
}


void sound_bchk(used,remaining)
int *used, *remaining;
{
    if (sb_inptr >= sb_outptr)
        *used = sb_inptr - sb_outptr;
    else
        *used = sb_inptr + (SND_BUFFSIZE - sb_outptr);

    *remaining = SND_BUFFSIZE - *used;
}
