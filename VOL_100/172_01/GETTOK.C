/*
  HEADER:              CUG  nnn.nn;
  TITLE:               LEX - A Lexical Analyser Generator
  VERSION:             1.1 for IBM-PC
  DATE:                Jan 30, 1985
  DESCRIPTION:         A Lexical Analyser Generator. From UNIX
  KEYWORDS:            Lexical Analyser Generator YACC C PREP
  SYSTEM:              IBM-PC and Compatiables
  FILENAME:            GETTOK.C
  WARNINGS:            This program is not for the casual user. It will
                       be useful primarily to expert developers.
  CRC:                 N/A
  SEE-ALSO:            YACC and PREP
  AUTHORS:             Charles H. Forsyth
                       Scott Guthery 11100 leafwood lane Austin, TX 78750
                       Andrew M. Ward, Jr.  Houston, Texas (Modifications)
  COMPILERS:           LATTICE C
  REFERENCES:          UNIX Systems Manuals -- Lex Manual on distribution disks
*/
/*
 * Bob Denny     28-Aug-82      Remove reference to stdio.h
 * Scott Guthery 20-Nov-83      Adapt for IBM PC & DeSmet C
 * Andrew Ward   20 Jun 86      Adapt for Lattice C and defined
 *                              argument lltbsiz.
 */

#include "lex.h"

extern char *llend, llbuf[];

int gettoken(lltb, lltbsiz)
char *lltb;
int lltbsiz; /* AMW: fix undefined argument */
{
	char *lp, *tp, *ep;

	tp = lltb;
	ep = tp+lltbsiz-1;
	for (lp = llbuf; lp < llend && tp < ep;) *tp++ = *lp++;
	*tp = 0;
	return( strlen(lltb) );
}
