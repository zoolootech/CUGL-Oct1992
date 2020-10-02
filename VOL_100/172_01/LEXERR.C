/*
  HEADER:              CUG  nnn.nn;
  TITLE:               LEX - A Lexical Analyser Generator
  VERSION:             1.1 for IBM-PC
  DATE:                Jan 30, 1985
  DESCRIPTION:         A Lexical Analyser Generator. From UNIX
  KEYWORDS:            Lexical Analyser Generator YACC C PREP
  SYSTEM:              IBM-PC and Compatiables
  FILENAME:            LEXERR.C
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
 * Copyright (c) 1978 Charles H. Forsyth
 *
 */

#include "lex.h"

extern int yyline;

lexerror(s, arg1, arg2, arg3)
char *s;
int arg1, arg2, arg3;
{
	if(yyline)
		fprintf(stderr, "%d: ", yyline);
	fprintf(stderr, s, arg1, arg2, arg3);
}


