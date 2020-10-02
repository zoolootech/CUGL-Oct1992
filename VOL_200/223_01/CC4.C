/*
** Small-C Compiler Version 2.7 (fas)
**
** Copyright 1982, 1983 J. E. Hendrix
**
** Part 4
*/
#include stdio.h
#include cc.def

extern char
 *macn,
 *cptr, *symtab,              /*37*/
#ifdef OPTIMIZE
 optimize,
#endif

nbflg, m80flg,						/* fas 2.2 */

level, itype, explevel,					/* fas 2.4 */

 *stagenext, ssname[NAMESIZE];
extern int
 beglab,  csp, output;

#include cc41.c
#include cc42.c
