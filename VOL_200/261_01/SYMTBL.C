/* symtbl.c - the symbol table code for as68
 *	(C) Copyright 1982 Steve Passe
 *	All Rights Reserved
 *
 * version 1.01
 *
 *   8/30/83 ver. 1.01 modified for Aztec ver. 1.05g smp
 *   9/1/83  split off from as68.c, the main module
 *   9/14/87  minor mods for Whitesmith's C on PDP-11 
 */
 
/* begincode */
 
/* includes */
 
#include <stdio.h>
#include "as68.h"
 
/* externals */
 
extern struct _symbol *symtbl;
extern struct _symbol *syms_head;	    /* head of sym table free space */
extern char *syms_tail;			    /* tail of sym table free space */
extern int symspace;			    /* bytes in symbol table */
extern int symbols;			    /* number of symbols in table */
extern struct _mtable mtable[];
 
/* search the mnemonic table for "mnem" */
 
struct _mtable *
mnemsearch(mnem)
char *mnem;
{
    register int result;
    register struct _mtable *bottom = &mtable[0];
    register struct _mtable *top = &mtable[MTSIZE - 1];
    register struct _mtable *middle;
 
    if (strlen(mnem) > 7) return NULL;
 
    while (bottom <= top) {
	middle = bottom + (top - bottom) / 2;
	if ((result = a1strcmp(mnem, middle->_mnem)) < 0)
	    top = middle - 1;
	else if (result	> 0)
	    bottom = middle + 1;
	else return middle;
    }
    return NULL;
}
 
/* search symbol table for entry,
	return index into table if found,
    or
	return NULL
*/
 
symsearch(symbol)
char *symbol;
{
    static int last_sought = NULL;	    /* index of last symbol sought */
 
    register int result;
    int bump = 0;		    /* store index 'bump' factor */
    register struct _symbol *bottom;
    register struct _symbol *top;
    register struct _symbol *middle;
 
    if (!symbol) return last_sought;	    /* want index of last search */
 
    bottom = &symtbl[0];		    /* first entry in table */
    top = &symtbl[symbols];		    /* last entry */
			/* scan for symbol with binary search of pointers */
    while (bottom <= top) {
	middle = bottom + (top - bottom) / 2;
	if ((result = strcmp(symbol, middle->_sym)) < 0) {
	    top = middle - 1;
	    bump = NULL;		    /* ...would	go here... */
	}
	else if (result	> 0) {
	    bottom = middle + 1;
	    bump = 1;   		    /* ...would	go 1 beyond here... */
	}
	else {  		/* place index where found into last_sought */
	    return (last_sought	= (middle - &symtbl[0]));
	}
    }
			/* place index of where it would go into last_sought */
    last_sought	= (middle - &symtbl[0])	+ bump;     /* add bump factor */
    return NULL;
}
 
/* add symbol, it's value, and it's attributes to table,
    if found,
	and permanent return NULL
	and temporary set value and return TRUE
    else
	if room,
	    add it, its value, and its attributes
	    and return TRUE
	else return ERROR   
*/
 
symadd(sym, v, a, search_flag)
char *sym;
long v;
char a;
FLAG search_flag;
{
    register int index;
 
    if (search_flag) {  				/* search first... */
	if (index = symsearch(sym)) {   		  /* found it */
	    if (symtbl[index]._atr & PERMANENT)	{	    /* if permanent */
		return NULL;				    /* can't */
	    }
	    else {      				    /* ok to reset */
		symtbl[index]._val = v; 		/* set new value */
		symtbl[index]._atr = a; 		/* set attribute */
		return TRUE;				    /* done */
	    }
	}
    }
    index = symsearch(NULL);		    /* get the last sought symbol */
 
    if ((syms_tail - (char *) syms_head)	    /* free space in bytes */
	< sizeof(struct	_symbol) + strlen(sym) + 1) {   /* space needed */
	return ERROR;				    /* no room */
    }
					    /* make a hole in array... */
    movmem(symtbl + index, symtbl + index + 1,  /* from index to index + 1 */
	(symbols - index + 1) * sizeof(struct _symbol));    /* second half */
    ++syms_head;				    /* another struct */
    ++symbols;					    /* another symbol */
    syms_tail -= (strlen(sym) + 1);		    /* space for symbol */
    strcpy(syms_tail, sym);			    /* move symbol */
    symtbl[index]._sym = syms_tail;		    /* set ptr to it */
    symtbl[index]._val = v;     		    /* set value */
    symtbl[index]._atr = a;     		    /* set attributes */
    return TRUE;				    /* whew! */
}
 
/* define a PERMANENT symbol (address or EQU)
    if not present and room,
	enter a permanent symbol and its value
	  into table and return TRUE,
    or if present,
	return NULL,
    or if out of symbol space,
	return ERROR
*/
 
symenter(sym, v, r_flg)
char *sym;
long v;
char r_flg;
{
    register int x;

    if (x = symsearch(sym)) {   	    /* symbol already defined */
	symtbl[x]._atr |= 0x04; 	    /* mark as redefine	attempted */
	return NULL;
    }
    return (r_flg == 'r') ?
		symadd(sym, v, 0x03, NO) : symadd(sym, v, 0x01, NO);
}


