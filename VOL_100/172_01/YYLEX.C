/*
  HEADER:              CUG  nnn.nn;
  TITLE:               LEX - A Lexical Analyser Generator
  VERSION:             1.1 for IBM-PC
  DATE:                Jan 30, 1985
  DESCRIPTION:         A Lexical Analyser Generator. From UNIX
  KEYWORDS:            Lexical Analyser Generator YACC C PREP
  SYSTEM:              IBM-PC and Compatiables
  FILENAME:            YYLEX.C
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
 * yylex for lex tables
 */
#include "lex.h"

#define YYSTYPE int
#define ERROR   256     /* yacc's value */
extern YYSTYPE yylval;
tst__b(c, tab)
int    c;
char            tab[];
{
	return(tab[(c >> 3) & 037] & (1 << (c & 07)) );
}

struct  lextab  *_tabp = NULL;

extern char     *llsave[];      /* Right-context buffer                 */
char    llbuf[100];             /* work buffer                          */
char    *llp1   = &llbuf[0];    /* pointer to next avail. in token      */
char    *llp2   = &llbuf[0];    /* pointer to end of lookahead          */
char    *llend  = &llbuf[0];    /* pointer to end of token              */
char    *llebuf = &llbuf[sizeof( llbuf)];
int     lleof;
int     yylval  = 0; /* This is not compatible */
int     yyline  = 0;

int yylex()
{
	int c, st;
	int final, l, llk, i;

	struct lextab *lp;
	char *cp;

	/*
	 * Call llstin() to default lexin to stdin
	 * and assign _tabp to "real" table.
	 */
	llstin();                       /* Initialize yylex() variables */

loop:
	llk = 0;
	if(llset())
		return(0);              /* Prevent EOF loop     */
	st = 0;
	final = -1;
	lp = _tabp;

	do {
		if (lp->lllook && (l = lp->lllook[st])) {
			for (c=0; c<NBPW; c++)
				if (l&(1<<c))
					llsave[c] = llp1;
			llk++;
		}
		if ((i = lp->llfinal[st]) != -1) {
			final = i;
			llend = llp1;
		}
		if ((c = llinp()) < 0)
			break;
		if ((cp = lp->llbrk) && llk==0 && tst__b(c, cp)) {
			llp1--;
			break;
		}
	} while ((st = (*lp->llmove)(lp, c, st)) != -1);


	if(llp2 < llp1)
		llp2 = llp1;
	if(final == -1) {
		llend = llp1;
		if (st == 0 && c < 0)
			return(0);
		if ((cp = lp->llill) && tst__b(c, cp)) {
			lexerror("Illegal character: %c (%03o)", c, c);
			goto loop;
		}
		return(ERROR);
	}
	if(c = (final >> 11) & 037)
		llend = llsave[c-1];
	if((c = (*lp->llactr)(final&03777)) >= 0)
		return(c);
	goto loop;
}

llinp()
{

	int c;
	struct lextab *lp;
	char *cp;

	lp = _tabp;
	cp = lp->llign;                         /* Ignore class         */
	for(;;) {
		/*
		 * Get the next character from the save buffer (if possible)
		 * If the save buffer's empty, then return EOF or the next
		 * input character.  Ignore the character if it's in the
		 * ignore class.
		 */
		c = (llp1 < llp2) ? *llp1 & CMASK : (lleof) ? EOF : lexgetc();
		if(c >= 0) {                   /* Got a character?     */
			if(cp && tst__b(c, cp))
				continue;       /* Ignore it            */
			if(llp1 >= llebuf) {   /* No, is there room?   */
				lexerror("Token buffer overflow");
				exit(1);
			}
			*llp1++ = c;            /* Store in token buff  */
		} else
			lleof = 1;              /* Set EOF signal       */
		return(c);
	}
}

llset()
/*
 * Return TRUE if EOF and nothing was moved in the look-ahead buffer
 */
{
	char *lp1, *lp2;

	for(lp1 = llbuf, lp2 = llend; lp2 < llp2;)
		*lp1++ = *lp2++;
	llend = llp1 = llbuf;
	llp2 = lp1;
	return(lleof && lp1 == llbuf);
}

/*
 * Re-initialize yylex() so that it can be re-used on
 * another file.
 */
llinit()
   {
   llp1 = llp2 = llend = llbuf;
   llebuf = llbuf + sizeof(llbuf);
   lleof = yylval = yyline = 0;
   }



