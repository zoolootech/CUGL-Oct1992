/*
** Small-C Compiler Version 2.0
**
** Portions Copyright 1982 J. E. Hendrix
**
** Modified for 8088/PCDOS by D. R. Hicks
**
** Part 5
**
** This is a new Part.  The optimizer that was formerly in cc42.c has been
** expanded and made into a separate module.  This allows it to be separately
** compiled and allows the source to be manipulated separately.
*/
 
#define NOCCARGC /* (be sure to remove if [f]printf is used anywhere) */
 
/*
** external references in part 1
*/
extern int
  output;
 
/*
** external references in part 2
*/
extern int
  cout(), nl(), ol(), ot(), outstr(), streq();
 
#include "cc51.c"
#include "cc52.c"

