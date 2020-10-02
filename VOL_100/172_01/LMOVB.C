/*
  HEADER:              CUG  nnn.nn;
  TITLE:               LEX - A Lexical Analyser Generator
  VERSION:             1.1 for IBM-PC
  DATE:                Jan 30, 1985
  DESCRIPTION:         A Lexical Analyser Generator. From UNIX
  KEYWORDS:            Lexical Analyser Generator YACC C PREP
  SYSTEM:              IBM-PC and Compatiables
  FILENAME:            LMOVB.C
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

#include "lex.h"

#ifndef CMASK
#define CMASK 0377
#endif

_lmovb(lp, c, st)
int c, st;
struct lextab *lp;
{
	int base;

	while ((base = lp->llbase[st]+c) > lp->llnxtmax ||
			(lp->llcheck[base] & CMASK) != st) {

		if (st != lp->llendst) {
/*
 * This miscompiled on Decus C many years ago:
 *                      st = lp->lldefault[st] & CMASK;
 */
			base = lp->lldefault[st] & CMASK;
			st = base;
		}
		else
                        return(-1);
        }
        return(lp->llnext[base]&CMASK);
}


