/*
  HEADER:              CUG  nnn.nn;
  TITLE:               LEX - A Lexical Analyser Generator
  VERSION:             1.1 for IBM-PC
  DATE:                Jan 30, 1985
  DESCRIPTION:         A Lexical Analyser Generator. From UNIX
  KEYWORDS:            Lexical Analyser Generator YACC C PREP
  SYSTEM:              IBM-PC and Compatiables
  FILENAME:            LEXSRT.C
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
 * Modified 22-Jun-86 Andrew Ward -- Modified code to compile under Lattice C
 *                                 version 3.0h. Modified code to use LATTICE
 * 				   sort functions and argument type checking.
 */
/*
 * Quick Sort: Used when builtin function not available
 */
#define LATTICE

#ifdef LATTICE
extern  void q_exc( char *, char * );
extern  void q_tex( char *, char *, char * );
#else
extern  void q_exc();
extern  void q_tex();
#endif
extern  void q_sort();
static  int     size;
int     (*qs__cmp)();

#ifndef LATTICE

void qsort(a, n, es, fc) /* Lattice qsort function used when available */
char *a;
int n, es;
int (*fc)();
{
        qs__cmp = fc;
        size = es;
        q_sort(a, a+n*es);
}

#endif

void q_sort(a, l)
char *a, *l;
{
       char *i, *j;
       int es;
       char *lp, *hp;
       int n, c;

       es = size;

start:
       if((n=l-a) <= es)
               return;
       n = es * ( n/(2*es));
       hp = lp = a+n;
       i = a;
       j = l-es;
       for(;;) {
               if(i < lp) {
                       if((c = (*qs__cmp)(i, lp)) == 0) {
                               q_exc(i, lp -= es);
                               continue;
                       }
                       if(c < 0) {
                               i += es;
                               continue;
                       }
               }

loop:
               if(j > hp) {
                       if((c = (*qs__cmp)(hp, j)) == 0) {
                               q_exc(hp += es, j);
                               goto loop;
                       }
                       if(c > 0) {
                               if(i == lp) {
                                       q_tex(i, hp += es, j);
                                       i = lp += es;
                                       goto loop;
                               }
                               q_exc(i, j);
                               j -= es;
                               i += es;

                               continue;
                       }
                       j -= es;
                       goto loop;
               }


               if(i == lp) {
                       if(lp-a >= l-hp) {
                               q_sort(hp+es, l);
                               l = lp;
                       } else {
                               q_sort(a, lp);
                               a = hp+es;
                       }
                       goto start;
               }
               q_tex(j, lp -= es, i);
               j = hp -= es;
       }
}

void q_exc(i, j)
char *i, *j;
{
       char *ri, *rj, c;
       int n;

       n = size;
       ri = i;
       rj = j;
       do {
               c = *ri;
               *ri++ = *rj;
               *rj++ = c;
       } while(--n);
}

void q_tex(i, j, k)
char *i, *j, *k;
{
       char *ri, *rj, *rk;
       int c;
       int n;

       n = size;
       ri = i;
       rj = j;
       rk = k;
       do {
               c = *ri;
               *ri++ = *rk;
               *rk++ = *rj;
               *rj++ = c;
       } while(--n);
}






