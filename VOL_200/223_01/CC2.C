/*
** Small-C Compiler Version 2.7 (fas)
**
** Copyright 1982, 1983 J. E. Hendrix
**
** Part 2
*/
#include stdio.h
#include cc.def

extern char
#ifdef DYNAMIC
 *symtab,
 *stage,
 *macn,
 *macq,
 *pline,
 *mline,
#else
  symtab[SYMTBSZ],
  stage[STAGESIZE],
  macn[MACNSIZE],
  macq[MACQSIZE],
  pline[LINESIZE],
  mline[LINESIZE],
#endif
#ifdef OPTIMIZE
  optimize,
#endif
  alarm, *glbptr, *line, *lptr, *cptr, *cptr2,  *cptr3,
 *locptr, msname[NAMESIZE],  pause,  quote[2],
 *stagelast, *stagenext;
extern int
#ifdef DYNAMIC
  *wq,
#else
  wq[WQTABSZ],
#endif
  inclevel, input2[6],				/* fas 2.7 */
  ccode,  ch,  csp,  eof,  errflag,  iflevel,
  input,  listfp,  macptr,  nch,
  nxtlab,  op[16],  opindex,  opsize,  output,  pptr,
  skiplevel,  *wqptr;

#include cc21.c
#include cc22.c

