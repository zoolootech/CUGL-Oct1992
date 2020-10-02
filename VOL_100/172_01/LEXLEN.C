/*
  HEADER:              CUG  nnn.nn;
  TITLE:               LEX - A Lexical Analyser Generator
  VERSION:             1.1 for IBM-PC
  DATE:                Jan 30, 1985
  DESCRIPTION:         A Lexical Analyser Generator. From UNIX
  KEYWORDS:            Lexical Analyser Generator YACC C PREP
  SYSTEM:              IBM-PC and Compatiables
  FILENAME:            LEXLEN.C
  WARNINGS:            This program is not for the casual user. It will
                       be useful primarily to expert developers.
  CRC:                 N/A
  SEE-ALSO:            YACC and PREP
  AUTHORS:             Charles H. Forsyth
	               Bob Denny
                       Scott Guthery 11100 leafwood lane Austin, TX 78750
  COMPILERS:           LATTICE C
  REFERENCES:          UNIX Systems Manuals -- Lex Manual on distribution disks
*/
/*
 * Copyright (c) 1978 Charles H. Forsyth
 *
 * Bob Denny -- removed stdio dependency.
 * Scott Guthery -- Adapt for IBM PC & DeSmet C
 */

#include "lex.h"

extern char *llend, llbuf[];

int lexlength()
/*
 * Return the length of the current token
 */
{
	 return(llend - llbuf);
}



