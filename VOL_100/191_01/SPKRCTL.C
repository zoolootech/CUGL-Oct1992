
/*  SPKRCTL.C                           Last update: 09 Feb 86  */

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


void spkr_cntr(n)
unsigned n;
{
    unsigned lo_byte, hi_byte;

    lo_byte = n & 0xff;
    hi_byte = (n >> 8) & 0xff;

    outp(SPKTMR+1,182);
    outp(SPKTMR,lo_byte);
    outp(SPKTMR,hi_byte);
}

void spkr_freq(f)
long f;
{
    spkr_cntr(FREQ2CTR(f));
}

void spkr_on()
{
    outp(KBCTL,inp(KBCTL)|0x3);
}

void spkr_off()
{
    outp(KBCTL,inp(KBCTL)&0xfc);
}

