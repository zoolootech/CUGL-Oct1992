/*
** Small-C Compiler Version 2.0
**
** Portions Copyright 1982 J. E. Hendrix
**
** Modified for 8088/PCDOS by D. R. Hicks
**
** Part 3
*/
#define NOCCARGC /* (be sure to remove if [f]printf is used anywhere) */
#include "errno.h"
#include "stdio.h"
#include "cc.def"
 
/*
** dummy symbol table entries for cast
*/
char castint[2] = {VARIABLE, CINT};
char castchar[2] = {VARIABLE, CCHAR};
char castpint[2] = {POINTER, CINT};
char castpchar[2] = {POINTER, CCHAR};
 
/*
** external references in part 1
*/
extern char
#ifdef DYNAMIC
 *stage,
 *litq,
#else
  stage[STAGESIZE],
  litq[LITABSZ],
#endif
 *glbptr, *lptr, ssname[NAMESIZE], quote[2], *stagenext;
extern int
  ch, csp, litlab, litptr, nch, op[16], op2[16],
  oper, opindex, opsize;
 
/*
** external references in part 2
*/
extern int
  addsym(), blanks(), bump(), clearstage(), endst(),
  error(), findglb(), findloc(), gch(), getlabel(),
  inbyte(), junk(), match(), needlval(), needtoken(),
  nextop(), nl(), numeric(), outbyte(), outdec(),
  outstr(), postlabel(), printlabel(), putint(),
  setstage(), streq(), symname(), ot(), ol();
 
/*
** external references in part 4
*/
extern int
  add(), address(), addr2(), and(), asl(), asr(), call(), callstk(),
  com(), dec(), div(), doublereg(), eq(), eq0(),
  ge(), ge0(), getloc(), getmem(), gt(), gt0(),
  immed(), immed2(), inc(), indirect(), jump(),
  le(), le0(), lneg(), loadargc(), lt(), lt0(),
  mod(), modstk(), move(), mult(), ne(), ne0(),
  neg(), or(), pop(), push(), putmem(),
  putstk(), ret(), sub(), swap(), swapstk(),
  testjump(), uge(), ugt(), ule(), ult(), ult0(),
  xor(), zerojump();
 
#include "cc31.c"
#include "cc32.c"
#include "cc33.c"

