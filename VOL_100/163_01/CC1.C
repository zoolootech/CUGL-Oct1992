/*
** Small-C Compiler Version 2.0
**
** Portions Copyright 1982 J. E. Hendrix
**
** Converted to 8088/PCDOS by D. R. Hicks
**
** Part 1
*/
#include "errno.h"
#include "stdio.h"
#include "cc.def"
 
/*
** miscellaneous storege
*/
char
  alarm,            /* audible alarm on errors?              */
  monitor,          /* monitor function headers?             */
  pause,            /* pause for operator on errors?         */
#ifdef DYNAMIC
  *stage,           /* output staging buffer                 */
  *symtab,          /* symbol table                          */
  *litq,            /* literal pool                          */
#ifdef HASH
  *macn,            /* macro name buffer                     */
#endif
  *macq,            /* macro string buffer                   */
  *pline,           /* parsing buffer                        */
  *mline,           /* macro buffer                          */
#else
  stage[STAGESIZE],
  symtab[SYMTBSZ],
  litq[LITABSZ],
#ifdef HASH
  mac[MACNSIZE],
#endif
 macq[MACQSIZE],
 pline[LINESIZE],
 mline[LINESIZE],
 swq[SWTABSZ],
#endif
 *line,             /* points to pline or mline              */
 *lptr,             /* ptr to either                         */
 *glbptr,           /* points to next entries                */
 *locptr,           /* ptr to next local symbol              */
 *stagenext,        /* next addr in stage                    */
 *stagelast,        /* last addr in stage                    */
 quote[2],          /* literal string for '"'                */
 *cptr,             /* work ptrs to any char buffer          */
 *cptr2,
 *cptr3,
 msname[NAMESIZE],  /* macro symbol name array               */
 ssname[NAMESIZE];  /* static symbol name array              */
 int
#ifdef STGOTO
   nogo,            /* >0 disables goto statements           */
   noloc,           /* >0 disables block locals              */
#endif
   op[16],          /* function addresses of binary operators */
   op2[16],         /* same for unsigned operators           */
   opindex,         /* index to matched operator             */
   opsize,          /* size of operator in bytes             */
   swactive,        /* true inside a switch                  */
   swdefault,       /* default label #, else 0               */
  *swnext,          /* address of next entry                 */
  *swend,           /* address of last table entry           */
#ifdef DYNAMIC
  *wq,              /* while queue                           */
#else
   wq[WQTABSZ],
#endif
#ifdef CMD_LINE
   argcs,           /* static argc                           */
  *argvs,           /* static argv                           */
#endif
  *wqptr,           /* ptr to next entry                     */
  litptr,           /* ptr to next entry                     */
  macptr,           /* macro buffer index                    */
#ifndef HASH
  mack,             /* variable k for findmac routine        */
#endif
  pptr,             /* ptr to parsing buffer                 */
  oper,             /* address of binary operator function   */
  ch,               /* current character of line being scanned */
  nch,              /* next character of line being scanned  */
  declared,         /* # of local bytes declared, else -1 when done */
  iflevel,          /* if... nest level                      */
  skiplevel,        /* level at which #if... skipping started */
  nxtlab,           /* next available label                  */
  litlab,           /* label # assigned to literal pool      */
  csp,              /* compiler relative stk ptr             */
                    /*
                     * set to zero at start of function
                     * (after BP is pushed)
                     * decremented by push
                     * incremented by pop
                     * top of stack is addressed by
                     *   "[BP]-" outdec(-csp)
                     */
  argstk,           /* function arg sp                       */
  argtop,
  ncmp,             /* #open compound statements             */
  errcnt,           /* error count */
  errflag,          /* non-zero after 1st error in statement */
  eof,              /* set non-zero on final input eof       */
  input,            /* fd # for input file                   */
  input2,           /* fd # for "include" file               */
  output,           /* fd # for output file                  */
  files,            /* non-zero if file list specified on command line */
  filearg,          /* cur file arg index                    */
  glbflag,          /* non-zero if internal globals          */
  ctext,            /* non-zero to intermix c-source         */
  ccode,            /* non-zero while parsing c-code         */
                    /*     zero when passing assembly code   */
  dmode,            /* non-zero when in DATASEG segment      */
                    /*     zero when in CODESEG segment      */
  listfp,           /* file pointer to list device           */
  lastst,           /* last executed statement type          */
 *iptr;             /* work ptr to any int buffer            */
 
#ifdef SEPARATE
/*
** external references in part 2
*/
extern int
  addmac(), addsym(), addwhile(), alpha(), amatch(), blanks(),
  bump(), clearstage(), col(), delwhile(), endst(), errdec(),
  error(), externs(), findglb(), findloc(), gch(), getint(),
  getlabel(), illname(), inbyte(), inline(), junk(),
  kill(), lout(), match(), multidef(), needtoken(),
  nextsym(), nl(), numeric(), outbyte(), outdec(), outstr(),
  postlabel(), preprocess(), printlabel(), putint(),
  readwhile(), setstage(), sout(), streq(), symname(),
  upper();
 
/*
** external references in part 3
*/
extern int
  constexpr(), expression(), number(), qstr(),
  test(), stowlit();
 
/*
** external references in part 4
*/
extern int
  add(), and(), asl(), asr(), comment2(), endfun(), defstora(),
  div(), eq(), entry(), ge(), genzeros(),
  gt(), header(), jump(), le(), lt(), mod(), modstk(),
  mult(), ne(), or(), point(), ret(), sub(), startfun(), startglob(),
  startlit(), sw(), trailer(), uge(), ugt(), ule(), ult(), xor();
#endif
 
/*
** external references in C.LIB
*/
extern int
  index(), rindex(), strcpy(), strncpy(), strlen();
 
#include "cc11.c"
#include "cc12.c"
#include "cc13.c"
 
#ifndef SEPARATE
#include "cc21.c"
#include "cc22.c"
#include "cc31.c"
#include "cc32.c"
#include "cc33.c"
#include "cc41.c"
#include "cc42.c"
#endif

