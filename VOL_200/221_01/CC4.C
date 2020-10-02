/*
HEADER:		Small-C source part 4;
TITLE:		Small-C Compiler for 6809 FLEX system;
VERSION:	2.3;

DESCRIPTION:	"Small-C Compiler which produces 6809 assembler output."
KEYWORDS:	;
SYSTEM:		6809 FLEX;
FILENAME:	CC4.C;
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
**  external functions
*/
extern int addglb(), an(),
  ch(), comment(),
  findglb(),
  gch(), getint(),
  illname(), inbyte(), inchar(), inline(),
  kill(),
  nch(),
#ifdef OPTIMIZE
  peephole(),
#endif
  symname();

/*
**  external variables
*/
extern int cmode,
  eof, errcnt,
  lptr,
  macptr, mptr,
  output,
  pause;

extern char *cptr,
  line[linesize],
  macq[macqsize], mline[linesize],
  stage[stagesize], *stagelast, *stagenext;

#ifdef VMS
#include "cc41.c"
#else
#include CC41.C
#endif
