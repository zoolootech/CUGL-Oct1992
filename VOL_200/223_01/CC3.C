/*
** Small-C Compiler Version 2.7 (fas)
**
** Copyright 1982, 1983 J. E. Hendrix
**
** Part 3
*/
#include stdio.h
#include cc.def

extern char
#ifdef DYNAMIC
 *stage,
 *litq,
#else
  stage[STAGESIZE],
  litq[LITABSZ],
#endif
 *glbptr, *lptr,  ssname[NAMESIZE],  quote[2], *stagenext,
  explevel,						/* fas 2.4 */
  level, itype;						/* fas 2.3 */
extern int
  ch,  csp,  litlab,  litptr,  nch,  op[16],  op2[16],
  *arrptr, *sarrptr,					/* fas 2.6 */
  arrcount,						/* fas 2.6 */
  oper,  opindex,  opsize;


#include cc31.c
#include cc32.c
#include cc33.c

