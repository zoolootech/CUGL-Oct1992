/*
  HEADER:              CUG  nnn.nn;
  TITLE:               LEX - A Lexical Analyser Generator
  VERSION:             1.1 for IBM-PC
  DATE:                Jan 30, 1985
  DESCRIPTION:         A Lexical Analyser Generator. From UNIX
  KEYWORDS:            Lexical Analyser Generator YACC C PREP
  SYSTEM:              IBM-PC and Compatiables
  FILENAME:            impure.c
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
 * More     03-May-82 Bob Denny -- Final touches, remove unreferenced autos
 * More     29-Aug-82 Bob Denny -- Clean up -d printouts
 * More     29-Aug-82 Bob Denny -- Reformat for readability and comment
 *                                 while learning about LEX.
 * More     20-Nov-83 Scott Guthery -- Adapt for IBM PC & DeSmet C
 *
 * Modified 22-Jun-86 Andrew Ward -- Modified code to compile under Lattice C
 *                                 version 3.0h.  Corrected several errors
 *                                 from the assumption that pointers and
 *                                 integers are the same size.     
 *                                 New debug code for LATTICE C using assert
 *                                 to test for wild pointers.
 */
/*
 * impure.c  -- Impure data for ytab.c and min.c
 *
 * Created 02-Dec-80 Bob Denny -- Impure data from ytab.c and min.c moved
 *                                                                  here so they can reside in overlays.
 * More    19-Mar-82 Bob Denny -- New C library & compiler
 * More    20-Nov-83 Scott Guthery -- Adapt for IBM PC & DeSmet C
 */

#include <stdio.h>
#include "lexlex.h"
#include "ytab.h"

/*
 * min's
 */
struct set **oldpart;
int **newpart;
int nold;
int nnew;

/*
 * ytab's
 */

extern struct nlist {
	struct  nlist   *nl_next;
	struct  nfa     *nl_base;
	struct  nfa     *nl_end;
	struct  nfa     *nl_start;
	struct  nfa     *nl_final;
	     } *nlist;

extern struct des {
      struct nfa *d_start;
      struct nfa *d_final;
      };

int nlook = 0;
static int yyline = 0;
char *breakc;
char *ignore;
char *illeg;

/* char buffer[150]; */
int str_len;

extern char *ccl;

/*
 * Copied from ytab.c just before yyparse() ... kludgy.
 */
#define YYMAXDEPTH 150

/*
 * These are impure data for the parser driver yyparse().
 */
