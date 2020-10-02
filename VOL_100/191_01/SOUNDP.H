
/*  SOUNDP.H                            Last update: 26 Feb 86  */

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


/* ------------------------------------------------------------ */
/*                         ** NOTE **                           */
/*                                                              */
/*      This file contains prototypes for all the accessable    */
/*      sound library functions.  Do not include this file      */
/*      unless your compiler supports prototyping.              */
/* ------------------------------------------------------------ */

extern int  play (char *music);
extern int  sound(long freq, unsigned duration);
extern int  sounds(struct sound_element sound_array);
extern int  sound_init (int mode,int install_brk);
extern void sound_done(void);
extern void quiet(void);
extern void sound_bchk(int *used, int *unused);
extern void spkr_on(void);
extern void spkr_off(void);
extern void spkr_freq(long freq);
extern void spkr_cntr(unsigned count);

/* ----------------- */
/*  End of SOUNDP.H  */
/* ----------------- */


