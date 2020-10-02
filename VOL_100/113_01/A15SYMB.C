/*
	HEADER:		CUG113;
	TITLE:		1802 Cross-Assembler (BDS C Version);
	FILENAME:	A15SYMB.C;
	VERSION:	1.2;
	DATE:		07/22/1985;

	DESCRIPTION:	"This program lets you use your CP/M-80-based computer
			to assemble code for the RCA 1802, 1804, 1805, 1805A,
			1806, AND 1806A microprocessors.  The program is
			written in BDS C for the best possible performance on
			8-bit machines.  All assembler features are supported
			except relocation, linkage, listing control, and
			macros.";

	KEYWORDS:	Software Development, Assemblers, Cross-Assemblers,
			RCA, CDP1802, CDP1805A;

	SEE-ALSO:	CUG149, 1805A Cross-Assembler (Portable);

	SYSTEM:		CP/M-80;
	COMPILERS:	BDS C;

	WARNINGS:	"This package is specifically tailored to CP/M-80
			machines and the rather non-standard, but high-
			performance BDS C compiler.  For other environments,
			use the portable version of this package on CUG149.";

	AUTHORS:	William C. Colley III;
*/

/*
	1805A Cross-Assembler  v 1.2

	Copyright (c) 1980, 82, 83, 85 William C. Colley, III.

	July 1982 -- Adapted from my 1802 cross-assembler.  WCC3.

	Vers 1.0 -- March 1983 -- Added 1805A opcodes to the 1805 set.  WCC3.

	Vers 1.1 -- March 1983 -- Added CPU pseudo-op to combine 1802 and 1805A
			cross-assemblers into a single program.  WCC3.

	Vers 1.2 -- June 1985 -- Fixed IF block nesting mechanism bug and bug
			in 1805A SCAL opcode.  WCC3.

File:	a15symb.c

Routines to manipulate the symbol table.
*/

/*  Get Globals:  */

#include "a15.h"

/*
This function adds a new entry to the symbol table.  The function returns
values of either 0 or -1.  If the value is 0, the symbol is already in the
table and the global variable sympoint points to the existing entry.  If the
value is -1, the symbol has just been entered into the table and sympoint
points to the new entry.  If the symbol table is full, the function triggers
an abort of the assembly.
*/

addsym(symbol)
char *symbol;
{
    int t;

    if ((t = slookup(symbol)) > 0) wipeout("\nSymbol Table Overflow.\n");
    if (t != 0) movmem(symbol, sympoint, SYMLEN);
    return(t);
}

/*
This function checks the symbol table for a given symbol.  The function returns
one of three values as follows:

	 1 = symbol not found and symbol table full.
	 0 = symbol found.  sympoint points to the matching entry.
	-1 = symbol not found.  sympoint points to where the symbol
	     should have been.

The symbol table is accessed using a hashing function with linear rehashing.
*/

slookup(symbol)
char *symbol;
{
    int temp;

    temp = sympoint = &symtbl[hash(symbol)];
    while((sympoint -> symname[0] & 0x7f) != '\0') {
	if (symcmp(symbol,sympoint->symname) == 0) return(0);
	if (++sympoint == symend) sympoint = symtbl;
	if (sympoint == temp) return(1);
    }
    return(-1);
}

/*
This function returns a hash value for a given symbol.  The hash value
is calculated by folding the symbol name up into 16 bits (2 bytes, thus
the symbol length must be even) mod the number of symbols.
*/

hash(symbol)
char *symbol;
{
    char i;
    unsigned j;

    for (i = j = 0; i < (SYMLEN / 2); i++)
	j += (*symbol++ << 8) + *symbol++;
    return(j % SYMBOLS);
}

/*
Function to sort the symbol table.  The function
returns the number of entries in the table.
*/

sortsym()
{
    int n, symcmp();
    struct symbtbl *tptr;

    n = 0;
    for (tptr = sympoint = symtbl; tptr < symend; ++tptr) {
	if (tptr -> symname[0] & 0x7f) {
	    movmem(tptr->symname,(sympoint++)->symname,(SYMLEN+2));  ++n;
	}
    }
    qsort(&symtbl,n,(SYMLEN+2),&symcmp);  return n;
}

/*
This function compares two symbols.  It returns zero if the
symbols are the same, not zero if they are different.
*/

symcmp(sym1,sym2)
char *sym1, *sym2;
{
    char i;
    int t;

    for (i = 0; i < SYMLEN; i++)
	if ((t = (*sym1++ & 0x7f) - (*sym2++ & 0x7f)) != 0) break;
    return(t);
}

/*
Function to abort an assembly.  The parameter reason holds a string that
will be printed to explain why the assembly bombed.  Note that I can't just
call exit since this will not restore the currently logged disk drive, and
software that changes the currently logged disk drive annoys me greatly.
*/

wipeout(reason)
char *reason;
{
    puts(reason);  exit();
}
ies in the table.
*/

sortsym()
{
    int n, symcmp();
    struct symbtbl *tptr;

    n = 0;
    for (t