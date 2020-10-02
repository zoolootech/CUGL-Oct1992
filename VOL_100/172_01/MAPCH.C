/*
  HEADER:              CUG  nnn.nn;
  TITLE:               LEX - A Lexical Analyser Generator
  VERSION:             1.1 for IBM-PC
  DATE:                Jan 30, 1985
  DESCRIPTION:         A Lexical Analyser Generator. From UNIX
  KEYWORDS:            Lexical Analyser Generator YACC C PREP
  SYSTEM:              IBM-PC and Compatiables
  FILENAME:            MAPCH.C
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

/*
 * mapch -- handle escapes within strings
 */

#include "lex.h"
extern FILE *lexin;
extern int   yyline;
extern int   lexchar(void);
extern void  lexerror( char *);

int mapch(delim, esc)
int delim;
int esc;
{
	int c, octv, n;

	if ((c = lexchar())==delim)
		return(EOF);
	if (c==EOF || c=='\n') {
		lexerror("Unterminated string");
		ungetc(c, lexin);
                return(EOF);
        }
        if (c!=esc)
		return(c);
        switch (c=lexchar()) {
        case 't':
                return('\t');
        case 'n':
                return('\n');
        case 'f':
                return('\f');
        case '\"': case '\'':
                return(c);
        case 'e':
                return('\e');
        case 'p':
                return(033);
        case 'r':
                return('\r');
        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7':
                octv = c-'0';
                for (n = 1; (c = lexchar())>='0' && c<='7' && n<=3; n++)
                        octv = octv*010 + (c-'0');
		ungetc(c, lexin);
                return(octv);
        case '\n':

                yyline++;
                return(mapch(delim, esc));
        }
        return(c);
}
