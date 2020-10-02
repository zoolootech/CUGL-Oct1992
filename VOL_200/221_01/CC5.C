/*
HEADER:		Small-C source part 5;
TITLE:		Small-C Compiler for 6809 FLEX system;
VERSION:	2.3;

DESCRIPTION:	"Small-C Compiler which produces 6809 assembler output."
KEYWORDS:	;
SYSTEM:		6809 FLEX;
FILENAME:	CC5.C;
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
extern int add(), and(), asl(), asr(),
  blanks(),
  ch(), clearstage(),
  div(), doublereg(),
  eq(),
  ge(), getlabel(), gt(),
  heir7(),
  immed(), inbyte(),
  jump(),
  le(), loadsec(), lt(),
  match(), mod(), mult(),
  needbrack(), needlval(), ne(), nl(),
  or(), outdec(), outstr(),
  postlabel(), pop(), push(), pushsec(),
  rvalue(),
  setstage(), store(), streq(), sub(),
  testjump(), testnoopt(), testtruejump(),
  uge(), ugt(), ule(), ult(),
  xor();

/*
* external value declaration
*/
extern int lptr;

extern char *cptr,
  line[linesize];

#ifdef VMS
#include "cc51.c"
#else
#include CC51.C
#endif
