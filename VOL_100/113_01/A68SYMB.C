/*
	HEADER:		CUG113;
	TITLE:		6800 Cross-Assembler (BDS C Version);
	FILENAME:	A68SYMB.C;
	VERSION:	2.6;
	DATE:		07/22/1985;

	DESCRIPTION:	"This program lets you use your CP/M-80-based computer
			to assemble code for the Motorola 6800, 6801, 6802,
			6803, 6808, and 68701 microprocessors.  The program is
			written in BDS C for the best possible performance on
			8-bit machines.  All assembler features are supported
			except relocation, linkage, listing control, and
			macros.";

	KEYWORDS:	Software Development, Assemblers, Cross-Assemblers,
			Motorola, MC6800, MC6801;

	SEE-ALSO:	CUG149, 6801 Cross-Assembler (Portable);

	SYSTEM:		CP/M-80;
	COMPILERS:	BDS C;

	WARNINGS:	"This package is specifically tailored to CP/M-80
			machines and the rather non-standard, but high-
			performance BDS C compiler.  For other environments,
			use the portable version of this package on CUG149.";

	AUTHORS:	William C. Colley III;
*/

/*
	6800/6801 Cross-Assembler  v 2.6

	May, 1980

	July, 1980 -- Rev. 2.2 consisting of fixing the M errors that
		come from forward references in FDB and FCB pseudo-ops.

	October, 1980 -- Rev. 2.3 consisting of updating the assembly
		language file and I/O routines to match and take
		advantage of BDS C V1.4.

	October, 1983 -- Rev. 2.4 consisting of adding the CPU pseudo-op,
		adding the 6801 CPU's extra opcodes, and speeding up the
		code a bit.

	September, 1984 -- Rev. 2.5 consisting of fixing bugs with the symbol
		table sort, the writing of files to specified drives, and the
		handling of blank input lines.

	June, 1985 -- Rev. 2.6 consisting of fixing a bug in the IF block
		nesting mechanism.

	Copyright (c) 1980,83,84,85 William C. Colley, III.

File:	a68symb.c

Routines to manipulate the symbol table.
*/

/*  Get Globals:  */

#include "a68.h"

/*
This function adds a new entry to the symbol table.  The function
returns values of either 0 or -1.  If the value is 0, the symbol is
already in the table and the global variable sympoint points to the
existing entry.  If the value is -1, the symbol has just been entered
into the table and sympoint points to the new entry.  If the symbol
table is full, the function triggers an abort of the assembly.
*/

addsym(symbol)
char *symbol;
{
    int t;

    if ((t = slookup(symbol)) > 0) wipeout("\nSymbol Table Overflow.\n");
    if (t) movmem(symbol, sympoint, SYMLEN);
    return t;
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
    while(sympoint -> symname[0] & 0x7f) {
	if (symcmp(symbol,sympoint->symname) == 0) return 0;
	if (++sympoint == symend) sympoint = symtbl;
	if (sympoint == temp) return 1;
    }
    return -1;
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

    for (i = j = 0; i < (SYMLEN / 2); ++i)
	j += (*symbol++ << 8) + *symbol++;

    return j % SYMBOLS;
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

    for (i = SYMLEN; i; --i) {
	if (t = (*sym1++ & 0x7f) - (*sym2++ & 0x7f)) break;
    }
    return t;
}

/*
Function to abort an assembly.  The parameter reason holds a string that
will be printed to explain why the assembly bombed.
*/

wipeout(reason)
char *reason;
{
    puts(reason);  exit();
}
 0; i < (SYMLEN / 2); ++i)
	j += (*symbol++ << 8) + *symbol++;

    return j % SYMBOLS;
}

/*
Fun