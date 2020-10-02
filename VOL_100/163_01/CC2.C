/*
** Small-C Compiler Version 2.0
**
** Portions Copyright 1982 J. E. Hendrix
**
** Modified for 8088/PCDOS by D. R. Hicks
**
** Part 2
*/
#define NOCCARGC  /* (be sure to remove if [f]printf is used anywhere) */
#include "errno.h"
#include "stdio.h"
#include "cc.def"
 
/*
** external references in part 1
*/
extern char
#ifdef DYNAMIC
 *symtab,
 *stage,
#ifdef HASH
 *macn,
#endif
 *macq,
 *pline,
 *mline,
#else
  symtab[SYMTBSZ],
  stage[STAGESIZE],
#ifdef HASH
  macn[MACNSIZE],
#endif
  macq[MACQSIZE],
  pline[LINESIZE],
  mline[LINESIZE],
#endif
  alarm, *glbptr, *line, *lptr, *cptr, *cptr2, *cptr3,
 *locptr, msname[NAMESIZE], pause, quote[2],
 *stagelast, *stagenext;
extern int
#ifdef DYNAMIC
  *wq,
#else
  wq[WQTABSZ],
#endif
#ifndef HASH
  mack,
#endif
  ctext, ccode, ch, csp, eof, errcnt, errflag, iflevel,
  input, input2, listfp, macptr, nch,
  nxtlab, op[16], opindex, opsize, output, pptr,
  skiplevel, *wqptr;
 
/*
** external reference in part 4
*/
extern int
  comment(), external();
 
/*
** external reference in part 5
*/
extern int
  peephole();
 
#include "cc21.c"
#include "cc22.c"

