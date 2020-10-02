/*
  HEADER:              CUG  nnn.nn;
  TITLE:               LEX - A Lexical Analyser Generator
  VERSION:             1.1 for IBM-PC
  DATE:                Jan 30, 1985
  DESCRIPTION:         A Lexical Analyser Generator. From UNIX
  KEYWORDS:            Lexical Analyser Generator YACC C PREP
  SYSTEM:              IBM-PC and Compatiables
  FILENAME:            INTEG.C
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
/*
 * integ -- ascii to long (various bases)
 */
long integ(cp, base)
char *cp;
int base;
{
	int c;
	long n;

	n = 0;
	while (c = *cp++) {
		if (c>='A' && c<='Z')
			c += 'a'-'A';
		if (c>='a' && c<='z')
			c = (c-'a')+10+'0';
		if (c < '0' || c > base+'0')
			break;
		n = n*base + c-'0';
	}
	return(n);
}
