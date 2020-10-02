
/*  PLAY.C                              Last update: 20 Feb 86  */

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


    /* The following table lists the frequencies of the 84 notes */
    /* in terms of Hz * 100.  It is taken from the ??? scale.    */


static long std_note_freq[] = {  
   /* Pause */             0, 
   /* Octave 0 */       1635,   1732,   1835,   1945,   2060,   2183,  
                        2312,   2450,   2596,   2750,   2914,   3087,
   /* Octave 1 */       3270,   3465,   3671,   3889,   4120,   4365,
                        4625,   4900,   5191,   5500,   5827,   6174,
   /* Octave 2 */       6541,   6930,   7342,   7778,   8241,   8731,
                        9250,   9800,  10383,  11000,  11654,  12347,
   /* Octave 3 */      13081,  13859,  14683,  15556,  16481,  17461,
                       18500,  19600,  20765,  22000,  23308,  24694,
   /* Octave 4 */      26163,  27718,  29366,  31113,  32963,  34923,
                       36999,  39200,  41530,  44000,  46616,  49388,
   /* Octave 5 */      52325,  55437,  58733,  62225,  65926,  69846, 
                       73999,  78399,  83061,  88000,  93233,  98777,
   /* Octave 6 */     104650, 110873, 117466, 124451, 132851, 139691, 
                      147998, 156798, 166122, 176000, 186466, 197553,
   /* Octave 7 */     209300, 221746, 234932, 248902, 263702, 279383,
                      295996, 313596, 332244, 352000, 372931, 395107, 
   /* Octave 8 */     418601  };


#define MAX_OCTAVE 7
#define LEGATO 0
#define NORMAL 1
#define STACCATO 2
#define NPO 12          /* Notes per octave */
#define FLAT -1
#define SHARP 1

static int note_length=1, note_mode=NORMAL, tempo=DEF_TEMPO;

int play(mstr)
char *mstr;
{
    int dots, fs, error, note, time, i;
    static int octave=DEF_OCTAVE;
    char c;

    error = 0;

    while (!error)
    {
        switch (c = toupper(*mstr++))
        {
            case '\0':  return 0;   /* End of string */

            case ' ':
            case ';':
            case ',':   break;      /* Ignore - used for decoration */

            case 'A':
            case 'B':   
            case 'C':   
            case 'D':   
            case 'E':   
            case 'F':   
            case 'G':   fs = flatsharp(&mstr);
                        note = noteval(c) + fs + (NPO * octave) + 1;
                        if (note == 0) note = 1;  /* Trap for low C-flat */
                        time = compute_time(&mstr);
                        dots = num_dots(&mstr);
                        do_note(note,time,dots);
                        break;

            case 'L':   if ((i = extrint(&mstr)) < 0)
                            error = 1;
                        else
                        {
                            if (i < 1 || i > 64) error = 1;
                            note_length = i;
#if DEBUG
                            printf("Note Len set to %d\n",i);
#endif
                        }
                        break;

            case 'M':   switch (toupper(*mstr++))
                        {
                            case 'N':   note_mode = NORMAL;
                                        printf("Mode NORMAL\n");
                                        break;

                            case 'L':   note_mode = LEGATO;
                                        printf("Mode LEGATO\n");
                                        break;

                            case 'S':   note_mode = STACCATO;
                                        printf("Mode STACCATO\n");
                                        break;

                            default:    error = 1;
                                        break;
                        }
                        break;

            case 'N':   if ((note = extrint(&mstr)) < 0)
                            error = 1;
                        else
                        {
                            if ((time = extrint(&mstr)) < 0)
                                time = note_length;
                            if (do_note(note,time,0) != 0)
                                error = 1;
                        }
                        break;

            case 'O':   octave = extrint(&mstr);
                        if (octave < 0 || octave > 6)
                            error = 1;
#if DEBUG
                        else
                            printf("Octave set to %d\n",octave);
#endif
                        break;

            case '<':   if (octave > 0) octave--;
                        break;

            case '>':   if (octave < MAX_OCTAVE) octave++;
                        break;

            case 'P':   if ((time = extrint(&mstr)) < 0)
                            error = 1;
                        else
                        {
#if DEBUG
                            printf("Pausing 1/%d %d dots\n",time,dots);
#endif
                            dots = num_dots(&mstr);
                            do_note(0,time,dots);
                        }
                        break;

            case 'T':   tempo = extrint(&mstr);
                        if (tempo < 4) tempo = 4;
                        if (tempo > 256) tempo = 256;
#if DEBUG
                        printf("Tempo is %d\n",tempo);
#endif
                        break;

            default:    error = 1;
                        break;

        }
    }

    return (error ? -1 : 0);
}


static int do_note(n,t,d)
int n,t,d;
{
    long freq;
    unsigned tot_time, pause_time, note_time, dot_high, dot_low;

    if (n < 0 || n > 84) return -1;

#if DEBUG
    printf("    Doing note %d t %d dots %d\n",n,t,d);
#endif

    dot_high = dot_low = 1;
    while (d--)
    {
        dot_high *= 3;
        dot_low *= 2;
    }

    freq = std_note_freq[n];
    tot_time = ((4000 * dot_high) / (t * tempo * dot_low));
    pause_time = (tot_time / 8) * note_mode;
    note_time = tot_time - pause_time;

#if DEBUG
    printf("    Freq %ld time T %d P %d N %d\n",freq,tot_time,
                          pause_time,note_time);
#endif

    sound(freq,note_time);
    sound(0L,pause_time);

    return 0;
}

    /*  EXTRINT -- extracts integer, advances pointer */

static int extrint(sp)
char **sp;
{
    register int digits = 0, val = 0;

    while (digits < 3 && isdigit(**sp))
    {
        val = (val * 10) + (**sp - '0');
        (*sp)++;
        digits++;
    }

    if (digits == 0) return -1;
      else           return val;
}


static int noteval(c)
char c;
{
    static int note_lookup[] = { 9,11,0,2,4,5,7 };

    if (c < 'A' || c > 'G') return 0;
      else                  return note_lookup[c-'A'];
}

static int flatsharp(sp)
char **sp;
{
    if (**sp == '+' || **sp == '#')
    {
#if DEBUG
        printf("  Note is sharp\n");
#endif
        (*sp)++;
        return SHARP;
    }
    else if (**sp == '-')
    {
#if DEBUG
        printf("  Note is flat\n");
#endif
        (*sp)++;
        return FLAT;
    }
    else
    {
        return 0;
    }
}


static int compute_time(sp)
char **sp;
{
    int l;

    if ((l = extrint(sp)) < 0)  return note_length;
      else                      return l;
}


static int num_dots(sp)
char **sp;
{
    int count;

    count = 0;
    while (**sp == '.')
    {
        (*sp)++;
        count++;
    }
    return count;
}


