
/* KbqGad.c ---> Keyboard Queue Gadgets.
 *
 * Kbq_x() are taken from "Controlling The Keyboard Buffer" by Steven Gruel
 * (The C Users Journal 7/90, pgs 85-6) w/ modifactions to return Keys.h
 * values for Extented Characters.  These can be used in TSRs, unlike the 
 * routines in <conio.h>.
 *
 * Get..() routines were suggested by similar routines in CUG-273.
 *
 * Author: J.Ekwall                                     13 September 91
 *
 * Copyrighted to the Public Domain.  Unlimited Distribution Authorized.
 *
 * User Assumes All Risks/Liabilities.
 *
 * Last Update: 13 September 91/EK
 */

#include <stdek.h>
#include <gadgets.h>
#include <stdio.h>
#include <dos.h>

#define KBQSEG  0x40
#define KBQRD   0x1A
#define KBQWRT  0x1C
#define KBQBTM  0x1E
#define KBQTOP  0x3E

int GetaKey(unsigned char *List, int CaseLess)
/* Wait until one of the charaacters in the list (or Esc) is pressed. */
{
    int c;
    
    if (CaseLess) strupr(List);
    do {
       c = Kbq_read(); if (CaseLess) c = toupper(c);
    } while (c != 27 && strchr(List, c) == NULL);
    return c;
}

unsigned int GetxKey(void)
/* return an ascii value or 256 * ScanCode */
{
    union REGS rg;

    while (1) {
       rg.h.ah = 1; int86(0x16, &rg, &rg);
       if (rg.x.flags & 0x40) { int86(0x28, &rg, &rg); continue; }
       rg.h.ah = 0; int86(0x16, &rg, &rg);
       if (rg.h.al == 0) return rg.h.ah << 8; else return rg.h.al;
    }
}

void Kbq_flush(void) { poke(KBQSEG, KBQWRT, peek(KBQSEG, KBQRD)); }

int Kbq_poll(void)
{                       /* Returns chr, Keys.h code     or Zero.  No Wait. */
    if (peek(KBQSEG, KBQWRT) == peek(KBQSEG,KBQRD)) return 0;
    return Kbq_read();
}

int Kbq_snoop(int Which1)
{         /* Report the "Nth" Keystroke in Queue.  (w/o Removal). */
    int i, Fill, Empty, Keystroke;

    Fill = peek(KBQSEG, KBQWRT);  Empty = peek(KBQSEG,KBQRD);
    if (Fill == Empty) return 0;
    if ((i = Fill - Empty) < 0) i += 32; if ((Which1 *= 2) > i) return 0;
    if ((i = Empty + Which1 - 2) >= KBQTOP) i -= 32;
    Keystroke = peek(KBQSEG, i);
    if ((Keystroke & 127) == 0) return ((Keystroke >> 8) | 128);
    return (Keystroke & 127);
}

int Kbq_stuff(unsigned char ch)
{                               /* Shove a Keystroke    Into Keyboard Queue. */
    int Fill, KeyStroke;

    if (!ch) return 0;
    if (ch < 128) KeyStroke = ch; else KeyStroke = (ch & 127) << 8;
    Fill = peek(KBQSEG, KBQWRT); poke(KBQSEG, Fill, KeyStroke);
    if ((Fill += 2) >= KBQTOP) Fill = KBQBTM;
    if (Fill == peek(KBQSEG,KBQRD)) return 0;   /* Full */
    poke(KBQSEG, KBQWRT, Fill); return 1;
}

int Kbq_tally(void)
{       /* Report Number of Keystrokes in Queue. */
    int i;

    i = (peek(KBQSEG, KBQWRT) - peek(KBQSEG,KBQRD)) / 2;
    if (i < 0) return i + 16;  else return i;
}

