/*
HEADER:		Small-C source part 2;
TITLE:		Small-C Compiler for 6809 FLEX system;
VERSION:	2.3;

DESCRIPTION:	"Small-C Compiler which produces 6809 assembler output."
KEYWORDS:	;
SYSTEM:		6809 FLEX;
FILENAME:	CC2.C;
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
extern int addglb(), addloc(), addwhile(), amatch(),
  blanks(),
  ch(), comment(), constexpr(), cout(),
  debug(), declloc(), defname(), defstorage(),
  delwhile(), doexpression(), dumplits(),
  endst(), entry(), errrpt(),
  findglb(), findloc(),
  gch(), getlabel(), getint(),
  illname(), inbyte(), inchar(), inline(),
  junk(), jump(),
  kill(),
  match(), modstk(), multidef(),
  needbrack(), nextsym(), nl(), number(),
  outbyte(), outdec(), outstr(),
  postlabel(), printlabel(), putint(),
  readwhile(), ret(),
  sout(), streq(), sw(), symname(),
  tab(), test();

/*
**  external varibles
*/
extern int argstk,
  ctext, cmode,
  DEFDEBUG, declared, declstat,
  eof,
  first_func,
  lastst, litlab, litptr, lptr,
  monitor,
#ifdef STGOTO
  nogo, noloc,
#endif
  ncmp,
  *startcomp, statlab, stdecl, stkp,
  stlab, swactive, swdefault, *swnext, *swend;
extern char *cptr, *cptr2,
  line[linesize], *locptr,
  *glbptr,
  *statptr,symtab[symtbsz];

#ifdef VMS
#include "cc21.c"
#else
#include CC21.C
#endif
