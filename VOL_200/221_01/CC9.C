/*
HEADER:		Small-C source part 9;
TITLE:		Small-C Compiler for 6809 FLEX system;
VERSION:	2.3;

DESCRIPTION:	"Small-C Compiler which produces 6809 assembler output."
KEYWORDS:	;
SYSTEM:		6809 FLEX;
FILENAME:	CC9.C;
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
**  external function declarations
*/
extern int cout(),
  nl(),
  ol(), ot(), outstr(),
  streq();

/*
**  external variable declaration
*/
extern int optimize, output;

#ifdef VMS
#include "cc91.c"
#else
#include CC91.C
#endif
