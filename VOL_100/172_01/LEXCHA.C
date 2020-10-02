/*
  HEADER:              CUG  nnn.nn;
  TITLE:               LEX - A Lexical Analyser Generator
  VERSION:             1.1 for IBM-PC
  DATE:                Jan 30, 1985
  DESCRIPTION:         A Lexical Analyser Generator. From UNIX
  KEYWORDS:            Lexical Analyser Generator YACC C PREP
  SYSTEM:              IBM-PC and Compatiables
  FILENAME:            LEXCHA.C
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
 * Bob Denny	 28-Aug-82  Remove reference to stdio.h
 * More          20-Nov-83 Scott Guthery -- Adapt for IBM PC & DeSmet C
 *
 * Modified      22-Jun-86 Andrew Ward -- Modified code to compile under 
 *                     	   Lattice C ver 3.0h.  Replaced 0377 with CMASK
 *			   defined in lex.h.  Added int type cast.	 
 */

#include "lex.h"

extern char *llend, *llp2;

int lexchar()
{
        return(llend<llp2? (int)*llend++&CMASK : (int)lexgetc());
}

