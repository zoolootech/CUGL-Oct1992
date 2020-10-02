/*
HEADER:		Small-C compiler source part 1;
TITLE:		Small-C Compiler for 6809 FLEX system;
VERSION:	2.3;

DESCRIPTION:	"Small-C Compiler which produces 6809 assembler output."
KEYWORDS:	;
SYSTEM:		6809 FLEX;
FILENAME:	CC1.C;
WARNINGS:	"Requires TSC relocatable assembler, library generater and
                linking loader."

SEE-ALSO:	;
AUTHORS:	Dieter H. Flunkert;
COMPILERS:	VAX VMS C compiler;
*/
/*
**   small-c compiler version 2.3
**
**  history:
**  13-jul-86  addet ! and ~ operator.  dhf
**  13-jul-86  addet +=, -=, *= etc.    dhf
**  18-jul-86  addet ?:, && and || operator. dhf
**  22-jul-86  compare of pointers are correct now. dhf
**  25-jul-86  made it a bit faster (changed & and | to && and ||) dhf
**  27-jul-86  improved zero init of global variables. dhf
**  29-oct-86  static declaration added. dhf
*/
#include STDIO.H
#ifdef VMS
#include "CCDEF.C"
#else
#include CCDEF.C
#endif

/*
**  Type definitions
*/
char _char[] = "char";
char _int[] = "int";
/* Now reserve some storage words  */
char stattab[stattbsize]; /* static symbol table */
char symtab[symtbsz]; /* symbol table */
char *glbptr,*locptr,*statptr;  /* ptrs to next entries */
char *endsearch;  /* compound start for searching locals */
char *startcomp;  /* start of pointer in current compound */
char swq[SWTABSZ];
#ifdef CMD_LINE
int argcs,*argvs[];   /* statig argc and argv */
#endif
int wq[wqtabsz];  /* while queue */
int *wqptr;   /* ptr to next entry */
char litq[litabsz];  /* literal pool */
int litptr;   /* ptr to next entry */
char macq[macqsize];  /* macro string buffer */
int macptr;   /* and its index */
int stdecl; /* greater 1 if static variable was declared */
char stage[stagesize]; /* staging buffer */
char *stagenext;  /* next adr in stage */
char *stagelast;  /* last adr in stage */
char line[linesize];  /* parsing buffer */
char mline[linesize]; /* temp macro buffer */
int lptr,mptr;  /* ptrs into each */
/* Misc storage */
int nxtlab,  /* next avail label # */
 statlab,    /* next static label number */
 stlab,      /* internal static label */
 litlab,  /* label # assigned to literal pool */
 constval[2], /* [0] = constant identifier */
              /* [1] = constant value */
#ifdef PHASE2
 monitor,   /* monitor compile */
 pause,   /* pause on error */
 DEFDEBUG,  /* debug feature enabled */
#endif
 first_func, /* 1 first function else 0 */
 declared, /* local symbol decl counter */
 stkp,  /* compiler relative stk ptr */
 argstk,  /* function arg sp */
#ifdef STGOTO
 nogo,  /* > 0 disables goto statements */
 noloc, /* > 0 disables block locals */
#endif
 ncmp,  /* # open compound statements */
 swactive, /* true inside a switch */
 swdefault, /* default label # else 0 */
 *swnext, /* address of next entry */
 *swend, /* address of last table entry */
 errcnt,  /* # errors in compilation */
 eof,  /* set non-zero on final input eof */
 input,  /* iob # for input file */
 output,  /* iob # for output file (if any) */
 input2,  /* iob # for "include" file */
 iflevel, /* depth of #ifdef */
 skiplevel, /* zero, if no skip */
#ifdef OPTIMIZE
 optimize, /* non-zero if user wants optimization */
#endif
 ctext,  /* non-zero to intermix c-source */
 cmode,  /* non-zero while parsing c-code */
   /* zero when passing assembly code */
 lastst;  /* last executed statement type */
char quote[2]; /* literal string for '"' */
char *cptr,  /* work ptr to any char buffer */
 *cptr2;
int *iptr;  /* work ptr to any int buffer */
/*
**  external functions used
*/
extern int addglb(), addloc(), addmac(), addstatic(), amatch(),
  blanks(), cnl(), ch(), constexpr(),
  defstorage(), doasm(), dumpzero(),
  entry(), endst(), errrpt(), declexternal(),
  findglb(), findloc(),
  getint(), getlabel(),
  header(), illname(),
  kill(),
  match(), multidef(),
  needbrack(),needsub(), newfunc(), nl(), number(), ns(),
  outbyte(), outdec(), outstr(),
  pl(), point(), printlab(),
  qstr(),
  sout(), stowlit(), symname(),
  trailer(),
  upper();
#ifdef VMS
#include "cc11.c"
#else
#include cc11.c
#endif
