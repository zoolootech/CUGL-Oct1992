/*
  HEADER:              CUG  nnn.nn;
  TITLE:               LEX - A Lexical Analyser Generator
  VERSION:             1.1 for IBM-PC
  DATE:                Jan 30, 1985
  DESCRIPTION:         A Lexical Analyser Generator. From UNIX
  KEYWORDS:            Lexical Analyser Generator YACC C PREP
  SYSTEM:              IBM-PC and Compatiables
  FILENAME:            OUTA.C
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
 * Modified 02-Dec-80 Bob Denny -- Conditionalize debug code for smaller size
 *                           01 -- Removed ending() function code from here
 *                                  to lex.c, so ytab.c code could share the
 *                                  same overlay region as this module.
 *                           02 -- Removed nfaprint(), llactr(), newcase(),
 *                                  cclprint(), chprint() and setline(),
 *                                  the rest of this can share an overlay.
 *                                  They're in 'out2.c'. This is now 'out1.c'.
 *          29-May-81 Bob Denny -- More extern hacking for RSX overlaying.
 *          19-Mar-82 Bob Denny -- New compiler and library
 *          03-May-82 Bob Denny -- Final touches, remove unreferenced autos
 *          28-Aug-82 Bob Denny -- Put "=" into table initializers to make
 *                                  new compiler happy. Add "-s" code to
 *                                  supress "#include <stdio.h>" in output.
 *                                  Tables output 8 values/line instead of
 *                                  16.  Overran R.H. edge on 3 digit octals.
 *                                  Change output format for readability.
 *          31-Aug-82 Bob Denny -- Add lexswitch( ...) to llstin so table
 *                                 name selected by -t switch is automatically
 *                                 switched-to at yylex() startup time. Removed
 *                                  hard reference to "lextab" from yylex();
 *                                  This module generates extern declaration
 *                                  for forward reference.
 *          14-Apr-83 Bob Denny -- Add VAX11C support.  Remove usage of remote
 *                                  formats (damn!) not supported on VAX-11 C.
 *                                 Use "short int" for 16-bit table items under
 *                                  VAX-11 C to save size.
 *                                 Contitional out flaky debug code.  Probably
 *                                  related to non-functional minimization.
 *			20-Nov-83 Scott Guthery -- Adapt for IBM PC & DeSmet C
 *			26-Dec-83 Scott Guthery -- Removed "extern FILE lexin" from llstin
 *						   code and put it lex.h.
 *          22-Jun-86 Andrew M. Ward -- Modified code to emit proper header info
 *                                      for Lattice C.
 */

/*
 * lex -- output human- and machine-readable tables
 */

#include "stdio.h"
#include "lexlex.h"

extern struct dfa dfa[];
extern char *ignore;
extern char infile[];
extern char *illeg;
extern char *breakc;
extern int nlook;
extern void dospccl(char *, char *, char *);
extern void refccl(char *, char *, char *);
extern void vstart(char *, char *), vend(void), vel(char *, int);
extern void heading(void);
extern int  ndfa;
int ven=0;
char strdec1[] = "\nstruct lextab %s =\t{\n";
char strdec2[] = "\t\t\t%d,\t\t/* Highest state */\n";
char strdec3[] = "\t\t\t_D%s\t/* --> \"Default state\" table */\n";
char strdec4[] = "\t\t\t_N%s\t/* --> \"Next state\" table */\n";
char strdec5[] = "\t\t\t_C%s\t/* --> \"Check value\" table */\n";
char strdec6[] = "\t\t\t_B%s\t/* --> \"Base\" table */\n";
char strdec7[] = "\t\t\t%d,\t\t/* Index of last entry in \"next\" */\n";
char strdec8[] = "\t\t\t%s,\t\t/* --> Byte-int move routine */\n";
char strdec9[] = "\t\t\t_F%s\t/* --> \"Final state\" table */\n";
char strdec10[] = "\t\t\t_A%s\t/* --> Action routine */\n";
char strdec11[] = "\n\t\t\t%s%s\t/* Look-ahead vector */\n";


char ptabnam[] = { "         " };

/*
 * Print the minimised DFA, and at the same time, construct the vector which
 * indicates final states by associating them with their translation index.
 * (DFA printout supressed ifndef DEBUG.)
 */

void dfaprint()
{
       struct dfa *st;
       int i, fi, k, l;

       vstart("LL16BIT _F%s", tabname);
       for(i = 0; i < ndfa; i++) {
               st = &dfa[i];
               k = -1;
               if(fi = st->df_name->s_final) {
                       k = nfa[fi].n_trans - trans;
                       if (nfa[fi].n_flag & FLOOK) {
                                k |= (nfa[fi].n_look+1)<<11;
                       }
                }
               if(l = st->df_name->s_look)
                       ;
               vel(" %d,", k);
       }
       vel(" %d,", -1);        /* blocking state */

       vend();
}


void heading()
{
       fprintf(llout,
        "/*\n * Created by IBM PC LEX from file \"%s\"\n", infile);
       if(sflag == 0)                  /* If "standalone" switch off */
               {
               fprintf(llout,
                       " *\t- for use with standard I/O\n */\n");
               fprintf(llout, "\n");
               }
       else
               fprintf(llout, " *\t- for use with stand-alone I/O\n */\n");

       fprintf(llout, "#include \"lex.h\"\n");
       fprintf(llout, "#define LL16BIT int\n");
       fprintf(llout, "extern int _lmov%c();\n",
                       (ndfa <= 255) ? 'b' : 'i');
               fprintf(llout, "extern struct lextab *_tabp;\n");
               fprintf(llout, "int lexval;\n");
               fprintf(llout, "extern int yyline;\n");
               fprintf(llout, "extern char *llend;\n");
               fprintf(llout, "extern void llstin();\n");
               fprintf(llout, "char *lbuf;\n");
}

void dfawrite()
{
       struct move *dp;
       int i, a, kk;
       struct dfa *st, *def;
       struct set *xp;
       char *xcp;

       setline();
       fprintf(llout,
               "\n#define\tLLTYPE1\t%s\n", ndfa <= 255 ? "char" : "LL16BIT" );

       vstart("LLTYPE1 _N%s", tabname);

       for (i = 0; i <= llnxtmax; i++)
               if( ( xp = move[i].m_next) != NULL )
               {
                      kk = xp->s_state - &dfa[0];
                      vel(" %d,", kk );
               }
               else
                      vel(" %d,", ndfa);
       vend();

       vstart("LLTYPE1 _C%s", tabname);
       for (i = 0; i <= llnxtmax; i++)
               if( ( st = move[i].m_check) != NULL )
               {
                      kk = st - &dfa[0];
                      vel(" %d,", kk );
               }
               else
                       vel(" %d,", -1);
       vend();

       vstart("LLTYPE1 _D%s", tabname);
       for (i = 0; i < ndfa; i++)
               if( ( def = dfa[i].df_default) != NULL )
               {
                       kk = def - dfa;
                       vel(" %d,", kk );
               }
               else
                       vel(" %d,", ndfa); /* refer to blocking state */
       vend();

       vstart("LL16BIT _B%s", tabname);
       for (i = 0; i < ndfa; i++)
               if( (dp = dfa[i].df_base) != NULL )
               {
                       kk = dp - &move[0];
                       vel(" %d,", kk );
               }
               else
                       vel(" %d,", 0);
       vel(" %d,", 0); /* for blocking state */
       vend();

       if( nlook )
       {
               fprintf(llout, "char    *llsave[%d];\n", nlook);
               vstart("LL16BIT _L%s", tabname);
               a = nlook <= NBPC ? NCHARS-1 : -1;
               for (i = 0; i < ndfa; i++)
                       vel(" 0%o,", dfa[i].df_name->s_look & a);
               vel(" %d,", 0);
               vend();
       }

       dospccl(ignore, "LLIGN", "X");
       dospccl(breakc, "LLBRK", "Y");
       dospccl(illeg, "LLILL", "Z");

       strcpy(ptabnam, tabname); strcat(ptabnam,",");
       fprintf(llout, strdec1, tabname);
       fprintf(llout, strdec2, ndfa);
       fprintf(llout, strdec3, ptabnam);
       fprintf(llout, strdec4, ptabnam);
       fprintf(llout, strdec5, ptabnam);
       fprintf(llout, strdec6, ptabnam);
       fprintf(llout, strdec7, llnxtmax);
       fprintf(llout, strdec8, ndfa<=255?"_lmovb":"_lmovi");
       fprintf(llout, strdec9, ptabnam);
       fprintf(llout, strdec10, ptabnam);
       fprintf(llout, strdec11, nlook?"_L":"", nlook?ptabnam:"NULL,   ");

       refccl(ignore, "Ignore", "X");
       refccl(breakc, "Break", "Y");
       refccl(illeg, "Illegal", "Z");
       fprintf(llout, "\t\t\t};\n");

       if(sflag == 0)                  /* If stdio flavor */
               {
               fprintf(llout, "\n/* Standard I/O selected */\n");

               fprintf(llout, "FILE *lexin = NULL;\n\n");
               fprintf(llout, "\nextern struct lextab *lexswitch();");
               fprintf(llout, "\nvoid llstin()\n   {\n   if((FILE *)lexin == NULL)\n");
               fprintf(llout, "      lexin = stdin;\n");
               }
       else                            /* Stand-alone flavor */
               {
               fprintf(llout, "\n/* Stand-alone selected */\n");
               fprintf(llout, "\llstin()\n   {\n");
               }

       fprintf(llout, "   if(_tabp == NULL)\n");
       fprintf(llout, "      lexswitch(&%s);\n   }\n\n", tabname);
       fclose(llout);
}

void dospccl(cp, s, tag)
char *cp;
char *s, *tag;
{
       int n;
       char cclnam[16];

       if (cp==0)
               return;
       fprintf(llout, "#define\t%s\t%s\n", s, tag);
       strcpy(cclnam, tag); strcat(cclnam, tabname);
       vstart("char _%s", cclnam);
       for (n = sizeof(ccls[0]); n--;)

               vel(" 0%o,", *cp++&CMASK );
       vend();
}

void refccl(cp, nm, tag)
char *cp, *nm, *tag;
{
       if (cp==0)
               fprintf(llout, "\t\t\tNULL,\t\t/* No %s class */\n", nm);
       else
               fprintf(llout, "\t_%s%s,\t/* %s class */\n", tag, tabname, nm);
}

void vstart(fmt, str)
char *fmt;
char *str;              /*** WATCH IT ***/
{
       putc('\n', llout);
       ven = 0;
       fprintf(llout, fmt, str);
       fprintf(llout, "[] =\n   {\n  ");
}

void vend()
{
       fprintf(llout, "\n   };\n");
}

void vel(fmt, val)
char *fmt;
int val;                /*** WATCH IT ***/
{
       fprintf(llout, fmt, val);
       if((++ven&07)==0)
               fprintf(llout, "\n  ");
}
