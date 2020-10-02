/*
HEADER:		Small-C source part 7;
TITLE:		Small-C Compiler for 6809 FLEX system;
VERSION:	2.3;

DESCRIPTION:	"Small-C Compiler which produces 6809 assembler output."
KEYWORDS:	;
SYSTEM:		6809 FLEX;
FILENAME:	CC7.C;
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
**  external function declaration
*/
extern int add(), addglb(),
  blanks(),
  callfunction(), ch(), clearstage(),
  doublereg(),
  gch(), getloc(), getmem(),
  errrpt(), expression(),
  findglb(), findloc(), findstat(),
  heir1(),
  immed(), inbyte(), indirect(),
  junk(),
  match(),
  nch(), needbrack(), nl(), numeric(),
  outbyte(), outdec(), outstr(),
  printlabel(), push(), putint(), putmem(), putstk(),
  setstage(), symname(),
  testjump();

/*
**  external variable declaration
*/
extern int constval[2],
  litlab, litptr;

extern char litq[litabsz],
  quote[2];

#ifdef VMS
#include "cc71.c"
#else
#include CC71.C
#endif
