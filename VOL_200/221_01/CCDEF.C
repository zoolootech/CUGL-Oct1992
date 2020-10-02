/*
HEADER:		Definitiones.;
TITLE:		Small-C Compiler for 6809 FLEX system;
VERSION:	2.3;

DESCRIPTION:	"Small-C Compiler definitions."
KEYWORDS:	;
SYSTEM:		6809 FLEX;
FILENAME:	CCDEF.C;
WARNINGS:	"Requires TSC relocatable assembler, library generater and
                linking loader."

SEE-ALSO:	;
AUTHORS:	Dieter H. Flunkert;
COMPILERS:	VAX VMS C compiler;
*/
/*#define VMS*/    /* if you run it on a vax vms system */
/*#define OPTIMIZE*/
#define VERSION "small-c V2.3"
#define BREAK 3
#ifndef VMS
#define CMD_LINE
#endif
#define PHASE2
/* Edited 25-Jan-82 Jeff Lomicka  */
/* - Test and fix changes of 23-sep-81 */
/* - VMS version, open for write is weird */
/*   main returns status code 2 on error. */
/* Edited 23-Sep-81 to incorporate changes from */
/* Dr. Dobbs Journal, Feb. 81    */
/* Changed for 6809 CPU from Dieter Flunkert    */
/*        06-Nov-85                             */
/* Define system dependent parameters */
/* Stand-alone definitions   */
#ifdef VMS
#define entry vmsentry  /* entry is reserved in VMS */
#define NULL 0
#define EOL 10
#define CLS 26
#define CR 13
#define LF 10
#define YES 1
#define NO 0
/*#define TAB 9 */
#include <STDIO.H>
/* FLEX definitions (if not stand-alone) */
#endif
#define SWSIZ 4
#define SWTABSZ 100
#define ERRCODE 0
/*
** define static table
*/
#define stattbsize 690
#define statlabel 14
#define statsize 23
/* Define the symbol table parameters */
#define symsiz 14
#define symtbsz 8008
#define numglbs 512
#define MASKGLBS 511
#define startglb symtab
#define startstat stattab
#define endglb startglb+numglbs*symsiz
#define endstat startstat+stattbsize
#define startloc endglb+symsiz
#define endloc symtab+symtbsz-symsiz
/* Define symbol table entry format */
#define name 0
#define ident 9
#define type 10
#define storage 11
#define offset 12
#define offsize 2
/* System wide name size (for symbols) */
#define namesize 9
#define namemax  8
/* Define possible entries for "ident" */
#define variable 1
#define array 2
#define pointer 3
#define function 4
#define MACRO 5
#define LABEL 6
/* Define possible entries for "type" */
#define cchar (1<<2)
#define cint (2<<2)
/* Define possible entries for "storage" */
#define statik 1
#define stkloc 2
#define EXTERNAL 3
#define automatic 4
/* Define the "while" statement queue */
#define wqtabsz 100
#define wqsiz 4
#define wqmax wq+wqtabsz-wqsiz
/* Define entry offsets in while queue */
#define wqsym 0
#define wqsp 1
#define wqloop 2
#define wqlab 3
/* Define the literal pool   */
#define litabsz 1000
#define litmax litabsz-1
/* Define the input line   */
#define linesize 80
#define linemax linesize-1
#define mpmax linemax
/* Define staging buffer size  */
#define stagesize 800
/* Define the macro (define) pool  */
#define macqsize 500
#define macmax macqsize-1
/* Define statement types (tokens)  */
#define stif 1
#define stwhile 2
#define streturn 3
#define stbreak 4
#define stcont 5
#define stasm 6
#define stexp 7
#define STDO 8
#define STFOR 9
#define STSWITCH 10
/*#define STGOTO 11*/
#define STCASE 12
#define STDEF 13
/*#define STLABEL 14*/
