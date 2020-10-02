/*
  HEADER:              CUG  nnn.nn;
  TITLE:               LEX - A Lexical Analyser Generator
  VERSION:             1.1 for IBM-PC
  DATE:                Jan 30, 1985
  DESCRIPTION:         A Lexical Analyser Generator. From UNIX
  KEYWORDS:            Lexical Analyser Generator YACC C PREP
  SYSTEM:              IBM-PC and Compatiables
  FILENAME:            OUTB.C
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
 */

#include <stdio.h>
#include "lexlex.h"

extern int yyline;
extern FILE *llout;
extern void newcase(int);
extern void llactr(void);
extern void cclprint(char *);
extern void setline(void);

void nfaprint(np, base)
struct nfa *np;
struct nfa *base;
{
        int i;

        if (np->n_flag&NPRT)
                return;
        np->n_flag |= NPRT;
        fprintf(stdout, "state %d\n", np-base);
        switch (np->n_char) {
        case EPSILON:
                for (i = 0; i < 2; i++)
                        if (np->n_succ[i])
                                fprintf(stdout, "\tepsilon  %d\n", np->n_succ[i] );
                break;
        case FIN:
                fprintf(stdout, "\tfinal state\n");
                break;
        case CCL:
                fprintf(stdout, "\t[");
                cclprint(np->n_ccl);
                fprintf(stdout, "]  %d\n", np->n_succ[0]-base);

                break;
        default:
                putc('\t', stdout);
                chprint(np->n_char);
                fprintf(stdout, "  %d\n", np->n_succ[0]-base);
                break;
        }
        putc('\n', stdout);
        if (np->n_succ[0])
                nfaprint(np->n_succ[0], base);
        if (np->n_succ[1])
                nfaprint(np->n_succ[1], base);
}

void cclprint(cp)
char *cp;
{
        int i;
        int nc;

        nc = 0;
        for (i = 0; i < NCHARS; i++)
                {
                if (cp[i / NBPC] & (1 << (i % NBPC)))
                        nc += chprint(i);
                if(nc >= 64)
                        {
                        nc = 0;
                        fprintf(stdout, "\n\t ");
                        }
                }
}


void llactr()
{
        /*
         * Prior to generating the action routine, create
         * the llstin() routine, which initializes yylex(),
         * per the setting of the "-s" switch.  All hardwired
         * variables have now been removed from yylex(). This
         * allows analyzers to be independent of the standard
         * I/O library and the table name.
         */
        fprintf(llout, "int _A%s(__na__)\t\t/* Action routine */\n\tint __na__;\n{\n", tabname);
}


void newcase(i)
int i;
{
        static int putsw;

        if (!putsw++)
                fprintf(llout, "   switch (__na__)\n      {\n");
        fprintf(llout, "\n      case %d:\n", i);
        setline();
}

void setline()
{
        fprintf(llout, "\n#line %d\n", yyline);
}
