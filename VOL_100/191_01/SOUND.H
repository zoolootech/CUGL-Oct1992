
/*  SOUND.H                             Last update: 25 Feb 86  */

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


/* ---------------- */
/*  COMPILER FLAGS  */
/*  Set one only!   */
/* ---------------- */

#define C_AZTEC   0
#define C_ECOSOFT 0
#define C_LATTICE 1

/* ---------------------------------- */
/*         Sound Buffer Size          */
/*   See Tech Info in documentation.  */
/* ---------------------------------- */

#define SND_BUFFSIZE 64


/* ------------------- */
/*  I/O Ports for IBM  */
/* ------------------- */

#define SPKTMR 0x42     /* 8253 port */
#define KBCTL 0x61      /* PPI interface */


/* ------------------- */
/*  Clock information  */
/* ------------------- */

#define STD_CLOCK 119318000L     /* Base clock in MHz times 100 */
#define TICK10 182               /* System tick rate per sec times 10 */

    /* FREQ2CTR translates a frequency to an 8253 counter value */
    /* MS2TICKS translates a millesecond count to a system tick count */

#define FREQ2CTR(f)  (f == 0 ? 0 : ((unsigned)(STD_CLOCK/(long)(f))))
#define MS2TICKS(m)  ((unsigned) ((((long)(m)*(long)TICK10)+5000) / 10000L) )

#define MAXFREQ (STD_CLOCK)             /* Max generatable frequency */
#define MINFREQ (STD_CLOCK/MAXUINT)     /* Min generatable frequency */


/* ------------------------------- */
/*  Music Defaults and Structures  */
/* ------------------------------- */

#define DEF_OCTAVE 4            /* Default Octave for play() */
#define DEF_TEMPO  32           /* Default Tempo for play() */

struct sound_element { long freq;  unsigned dur; };
struct raw_sound { unsigned count, ticks; };


/* -------------------------------- */
/*  Compiler-Dependent Definitions  */
/* -------------------------------- */

#if C_AZTEC
#define outp(x,v) outport(x,v)
#define inp(x) inport(x)
#endif

#if C_ECOSOFT
#define outp(x,v) outportb(x,v)
#define inp(x) inportb(x)
#endif

#if C_LATTICE
#define void int
#endif

/* ---------------------------------------- */
/*  Symbols Needed, But Not Always Defined  */
/* ---------------------------------------- */

#ifndef isdigit
#define isdigit(x) ((x) >= '0' && (x) <= '9')
#endif

#ifndef MAXUINT
#define MAXUINT 0xffff  /* Maximum unsigned integer value */
#endif

#ifndef MAXINT
#define MAXINT 0x7fff   /* Maximum signed integer value */
#endif

/* ---------------- */
/*  End of SOUND.H  */
/* ---------------- */

