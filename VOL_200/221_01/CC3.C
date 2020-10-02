/*
HEADER:		Small-C source part 3;
TITLE:		Small-C Compiler for 6809 FLEX system;
VERSION:	2.3;

DESCRIPTION:	"Small-C Compiler which produces 6809 assembler output."
KEYWORDS:	;
SYSTEM:		6809 FLEX;
FILENAME:	CC3.C;
WARNINGS:	"Requires TSC relocatable assembler, library generater and
                linking loader."

SEE-ALSO:	;
AUTHORS:	Dieter H. Flunkert;
COMPILERS:	VAX VMS C compiler;
*/
#include STDIO.H
#ifdef VMS
#include "ccdef.c"
#else
#include CCDEF.C
#endif

/*
**  define external functions
*/
extern int astreq(),
  blanks(),
  call(), callstk(), cnl(), comment(), cout(),
  errrpt(), expression(),
  findmac(),
  immed(),
  match(), modstk(),
  nl(),
  outdec(), outstr(),
  preprocess(), push(),
  sout(), streq(), swapstk();

/*
**  external variables
*/
extern int cmode, ctext,
  eof,
  iflevel, input, input2,
  lptr,
  nxtlab,
  output,
  skiplevel, stkp,
  wq[wqtabsz], *wqptr;

extern char *cptr,
  *endsearch,
  *glbptr,
  line[linesize], *locptr,
  *startcomp, *statptr, stattab[stattbsize], symtab[symsiz];

#ifdef VMS
#include "cc31.c"
#else
#include CC31.C
#endif
