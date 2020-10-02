/*
  HEADER: CUG    nnn.nn;
  TITLE:         LEX - A Lexical Analyser Generator
  VERSION:       1.1 for IBM-PC
  DATE:          20 June 1986
  DESCRIPTION:   A Lexical Analyser Generator. From UNIX
  KEYWORDS:      Lexical Analyser Generator YACC C PREP
  SYSTEM:        IBM-PC and Compatiables
  FILENAME:      LEX.H
  WARNINGS:      This program is not for the casual user. It will
                 be useful primarily to expert developers.
  CRC:           N/A
  SEE-ALSO:      YACC and PREP
  AUTHORS:       Scott Guthery 11100 leadfwood lane Austin, TX 78750
  COMPILERS:     DESMET-C
  REFERENCES:    UNIX Systems Manuals
*/
/*
 * Bob Denny 28-Aug-82  Remove reference to FILE *lexin to
 * 			eliminate dependency on standard I/O library. Only
 * 			lexgetc() used it, and it's there now.
 *                      Add EOF definition for standalone uses.
 *                      Corrected comment for llnxtmax.
 *
 * Scott Guthery 20-Nov-83      Adapt for IBM PC & DeSmet C.  Removed
 *                      equivalence of yylval and lexval since
 *                      a multi-typed parser wants yylval to be
 *                      typed to be the union of the types (YYSTYPE).
 * Andrew Ward 20 Jun 86  Modified to include special Cterp stdio lib
 *                      and defined yytext and lexlast; values which YACC 
 *			expects YYLEX to set.  Defined CMASK and the files
 *			lexin and lexout.	 
 */

/*
 * lex library header file -- accessed through
 *      #include <lex.h>
 */


#ifdef C_terp
#include "\lc\cterp\stdio.h"
#else
#include <stdio.h>
#endif

/*
 * Description of scanning tables. The entries at the front of
 * the struct must remain in place for the assembler routines to find.
 */

struct  lextab {
	int     llendst;                /* Last state number            */
	char    *lldefault;             /* Default state table          */
	char    *llnext;                /* Next state table             */
	char    *llcheck;               /* Check table                  */
	int     *llbase;                /* Base table                   */
	int     llnxtmax;               /* Last in next table           */
	int     (*llmove)();            /* Move between states          */
	int     *llfinal;               /* Final state descriptions     */
	int     (*llactr)();            /* Action routine               */
	int     *lllook;                /* Look ahead vector if != NULL */
	char    *llign;                 /* Ignore char vec if != NULL   */
	char    *llbrk;                 /* Break char vec if != NULL    */
	char    *llill;                 /* Illegal char vec if != NULL  */
};

#define NBPW             16
#define LEXERR          256
#define LEXSKIP         (-1)
#define LEXECHO(fp) {lexecho((fp));}
#define CMASK 0377
#define lextext llbuf
/* AMW: define yytext as llbuf and see if it works */
#define yytext llbuf
#define lexlast llend

extern char llbuf[];
extern FILE *lexin;
extern FILE *lexout;
extern struct lextab *lexswitch( struct lextab * );
