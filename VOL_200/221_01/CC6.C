/*
HEADER:		Small-C source part 6;
TITLE:		Small-C Compiler for 6809 FLEX system;
VERSION:	2.3;

DESCRIPTION:	"Small-C Compiler which produces 6809 assembler output."
KEYWORDS:	;
SYSTEM:		6809 FLEX;
FILENAME:	CC6.C;
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
** external function declaration
*/
extern int add(), asl(), asr(),
  blanks(),
  ch(), com(),
  dec(), div(), doublereg(),
  errrpt(),
  heir11(),
  immed(), inc(),
  lognot(),
  match(), mod(), mult(),
  needlval(), neg(), nl(),
  outstr(),
  pop(), push(),
  rvalue(),
  store(), streq(), sub();

/*
** external variables declaration
*/
extern int lptr;

extern char *cptr,
  line[linesize];

#ifdef VMS
#include "cc61.c"
#else
#include CC61.C
#endif
