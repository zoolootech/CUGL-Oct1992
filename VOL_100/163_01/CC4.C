/*
** Small-C Compiler Version 2.0
**
** Portions Copyright 1982 J. E. Hendrix
**
** Modified for 8088/PCDOS by D. R. Hicks
**
** Part 4
**
** Most modifications that were necessary for the 8088 processor occurred
** in this Part.  The only other significant modification necessary was in
** Part 1 to accommodate the generation of code and data in separate segments.
** and to allow the generation of a function prologue so that BP could be
** used to address the current stack frame.  Most modifications necessary for
** the PCDOS environment occurred in the I/O and library routines.
*/
 
/*
** General scheme:
**
** Primary register:  AX
** Secondary register:  BX
** Argument count register:  DX
**
** Code and data are generated in separate segments (the stack is contained
** in the data segment).  CS points at the code segment.  DS, ES, and SS all
** point at the data segment.  Thus, a data address can be relative to any one
** of the three data segment pointers interchangeably.
**
** The current stack frame is addressed by register BP, and addressing of
** arguments and local variables is done relative to BP (rather than relative
** to SP as was done in the 8080 version).  Thus, all local variables are
** addressed with negative offsets from PB, while arguments are addressed
** with positive offsets from BP.
**
** A stack frame looks as follows (stack grows down -- highest addresses first)
**
**   Parameter 1
**   Parameter 2
**   ...
**   Parameter N
**   Instruction pointer (return address)
**   Caller's BP                           <-- BP points here
**   Local variables
**   ...
**   Local variables
**   Evaluation stack entries
**   ...
**   Evaluation stack entries              <-- SP points here
**
** Note that the saved BP values create a chain of stack frames.
** This feature can be used to generate a trace-back on errors.
*/
#define NOCCARGC /* (be sure to remove if [f]printf is used anywhere) */
#include "errno.h"
#include "stdio.h"
#include "cc.def"
 
/*
** external references in part 1
*/
extern char
#ifdef HASH
  *mach,
#endif
#ifdef DYNAMIC
  *macq,
#else
  macq[MACQSIZE],
#endif
  *stagenext, ssname[NAMESIZE];
extern int
  csp, dmode, output;
 
/*
** external references in part 2
*/
extern int
#ifdef HASH
  search(),
#else
  findmac(),
#endif
  clearstage(), col(), cout(), getint(), getlabel(),
  nl(), numeric(),  ol(), ot(), outbyte(), printlabel(),
  lout(),  outdec(),  outstr(),  streq();
 
/*
** external references in part 3
*/
extern int const();
 
#include "cc41.c"
#include "cc42.c"

