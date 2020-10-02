/*
  HEADER:              CUG  nnn.nn;
  TITLE:               LEX - A Lexical Analyser Generator
  VERSION:             1.1 for IBM-PC
  DATE:                Jan 30, 1985
  DESCRIPTION:         A Lexical Analyser Generator. From UNIX
  KEYWORDS:            Lexical Analyser Generator YACC C PREP
  SYSTEM:              IBM-PC and Compatiables
  FILENAME:            LEXECH.C
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
 * Modified 02-Dec-80 Bob Denny -- Conditionalize debug code for reduced size
 * Modified 29-May-81 Bob Denny -- Clean up overlay stuff for RSX.
 * More     19-Mar-82 Bob Denny -- New C library & compiler
 * Bob Denny 28-Aug-82             Move stdio dependencies to lexerr(), 
 *			  	   lexget(), lexech() and mapch(). 
 *			           This is one of 4 modules in lexlib which 
 *                                 depend upon the standard I/O package.
 * Scott Guthery 20-Nov-83         Adapt for IBM PC & DeSmet C
 *
 * Modified 22-Jun-86 Andrew Ward -- Modified code to compile under Lattice C
 *                                 version 3.0h.  Changed FILE pointer def.
 *                                 Typed lexecho as void.
 */

#include "lex.h"

extern char *llend, llbuf[];

void lexecho(fp)
FILE *fp;
{
	char *lp;
        for (lp = llbuf; lp < llend;)
		putc(*lp++, fp);
}



