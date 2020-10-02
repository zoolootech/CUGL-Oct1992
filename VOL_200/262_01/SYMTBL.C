/*********************************************************
 *             Symbol Table Manipulation Package         *
 *               Robert Ramey     21 Sept 1986           *
 *********************************************************/

#include <stdio.h>

/***********************************************************
These routines use a character pointer to symbols and data.
Pointers to symbol table entries point to the character
string that is the symbol itself.  Pointer returned by symdat
are character pointers to the stored data structure.  These
should be recast to the appropriate structure pointer before
being used.  All other pointers are not expected to be used
by the calling programs except as character pointers so no
casting should be necessary.
**********************************************************/
typedef char SYMBOLENTRY;

struct _symtbl {    /*structure for symbol table data */
    int _element_size; /*size of data not includeing links nor*/
              /*symbol itself */
    int _hash_factor; /*number of chains of symbols */
    SYMBOLENTRY *_hash_address[1]; /*address of start of chain */
};
typedef struct _symtbl SYMBOLTABLE;

/***********************************************************
symmk - used to initialize a symbol table.  If there is not
enough memory available to allocate a symbol table structure
symmk will return NULL.  Otherwise, it will return a pointer
to the created symbol table structure.  This pointer is used
on subsecuent calls to insert, lookup and delete symbols from
the table.  This permits multiple symbol tables with in a 
program all using the same code.
************************************************************/

SYMBOLTABLE  *
symmk(element_size,hash_factor)
int element_size;   /*The amount of data to be associated
             *with each symbol of the table */
register  int hash_factor;/*The number of chains of symbols to be
             *created */
{
    SYMBOLTABLE *stptr;
    register SYMBOLENTRY **st_entry;

    /* get space for symbol table */
    stptr = (SYMBOLTABLE *) /* cast to symbol table ptr*/
        /* size of first two entries in SYMBOLTABLE structure*/
        malloc(sizeof(int) * 2 +
        /* size of hash table */
        sizeof(SYMBOLENTRY *) * hash_factor);
    if(stptr == NULL)
        return NULL;    /* Not enough space for table */
    stptr->_element_size = element_size;
    stptr->_hash_factor = hash_factor;
    st_entry = stptr->_hash_address + hash_factor;
    while(hash_factor--)
        *--st_entry = NULL;
    return stptr;
}
/***************************************************************
_nxtsym - returns address of pointer to next entry in chain
****************************************************************/
SYMBOLENTRY **
_nxtsym(st_entry)
SYMBOLENTRY *st_entry;
{
    return st_entry - 2 * sizeof(SYMBOLENTRY *) ;
}
/****************************************************************
_prvsym - returns address of pointer to previous entry in chain
*****************************************************************/
SYMBOLENTRY **
_prvsym(st_entry)
SYMBOLENTRY *st_entry;
{
    return st_entry - sizeof(SYMBOLENTRY *);
}
/************************************************************
hash - Returns an index into the hash table.
*************************************************************/
int
_hash(stptr,symbol)
SYMBOLTABLE *stptr;
register char *symbol;
{
    /* accumlate hash total from sum of characters in symbol */
    register int hash_total;
    do{
        hash_total += *symbol++;
    }
    while(*symbol);
    return hash_total % stptr->_hash_factor;
}
/****************************************************************
symdat - returns address of data given symbol pointer as argument.
Note that address is cast as a character pointer.  It is the 
callers responsability to recast this as the appropriate structure
pointer if necessary.
******************************************************************/
char *
symdat(stptr, st_entry)
SYMBOLTABLE *stptr;
SYMBOLENTRY *st_entry;
{
    return
    st_entry - stptr->_element_size - 2 * sizeof(SYMBOLENTRY *);
}
/*****************************************************************
symadd - add a symbol with its corresponding data to symbol table.
Returns NULL if there was not enough memory to add symbol.
Otherwise returns pointer to symbol string in table.
******************************************************************/
SYMBOLENTRY *
symadd(stptr,symbol)
SYMBOLTABLE *stptr; /*pointer to symbol table to be used */
char *symbol;   /*pointer to symbol string and data*/
{
    int size, hash_total;
    register SYMBOLENTRY *st_entry;
    SYMBOLENTRY *st_head;

    /* figure start of chain */
    hash_total = _hash(stptr,symbol);

    /*get space for symbol table entry */
    size = stptr->_element_size + 2 * sizeof(SYMBOLENTRY *);

    st_entry = malloc(strlen(symbol) /* size of symbol itself */
        + size /* for data and pointers in table */
        + sizeof(char));    /* Null terminator on symbol */
                     /* and previous entry */
    if(!st_entry)
        return NULL;    /* return if not enough space */

    st_entry += size;   /* remember symbol table entry */
        /* doesn't point to beginning of data but to */
        /*  symbol string itself */

    /* add new entry to start of chain */

    /*before creating new element update reverse pointer on next */
    if(st_head = stptr->_hash_address[hash_total])
        *(_prvsym(st_head)) = st_entry;

    /* first pointer to next element */
    *(_nxtsym(st_entry)) = st_head;

    /* pointer to previous element */
    *(_prvsym(st_entry)) =
        (SYMBOLENTRY *)(stptr->_hash_address + hash_total)
        + 2 * sizeof(SYMBOLENTRY *);
        /* The + 2 is to permit nxtsym function when arg */
        /* is the first element */

    /* finally add symbol string to table */
    strcpy(st_entry,symbol);

    /* modify pointer to start of chain */
    stptr->_hash_address[hash_total] = st_entry;

    return st_entry;
}
/************************************************************
symlkup - find a symbol in a symbol table.  Returns NULL if 
symbol not found.  Otherwise returns pointer to the symbol
string.
*************************************************************/
SYMBOLENTRY *
symlkup(stptr, symbol)
SYMBOLTABLE *stptr;
char *symbol;
{
    SYMBOLENTRY *symptr;
    unsigned int hash_total;
    /* figure out where start of chain is */
    hash_total = _hash(stptr, symbol);
    
    symptr = stptr->_hash_address[hash_total];
    while(symptr){
        if(!strcmp(symbol,symptr))
            return symptr;
        symptr = *_nxtsym(symptr);
    }
    return NULL;
}
/*************************************************************
symdel - Delete a symbol and its data and return space to system.
We make no tests that the argument is actually in the table.
So take care that argument is pointer to current symbol in table.
That is, the argument must have been previously retrieved via
symadd, symlkup, first, or next.  If you forget and simply pass
a pointer to a string as an argument you can be sure of getting
real problems.  No value is returned.
***************************************************************/
void
symdel(stptr,st_entry)
SYMBOLTABLE *stptr;
SYMBOLENTRY *st_entry;
{
    *_nxtsym(*_prvsym(st_entry)) = *_nxtsym(st_entry);
    *_prvsym(*_nxtsym(st_entry)) = *_prvsym(st_entry);
    free(st_entry -
        stptr->_element_size -
         2 * sizeof(SYMBOLENTRY *));
    return;
}
/******************************************************************
symdmp - Used to sequentially retrieve all symbols in table. The
pointer to the "first" symbol is retrieved with symdmp(stptr, 0) while
pointers to subsequent symbols are retrieved with symdmp(sptr, !=0).
It is garenteed that each and every symbol will be retrieved
exactly once.  When there are no more symbols to be retrieved
NULL will be returned.
*******************************************************************/
SYMBOLENTRY *
symdmp(stptr,initial)
SYMBOLTABLE *stptr;
unsigned int initial;
{
    static SYMBOLENTRY *current_symbol;
    static int hash_count;
    if(!initial){
        hash_count = stptr->_hash_factor;
        current_symbol = NULL;
    }
    else
        current_symbol = *_nxtsym(current_symbol);
    while(!current_symbol && hash_count)
        current_symbol = stptr->_hash_address[--hash_count];
    return current_symbol;
}
/********************************************************************
symrmv - delete an entire symbol table and return space to system.
*********************************************************************/
void
symrmv(stptr)
SYMBOLTABLE *stptr;
{
    SYMBOLTABLE *current_symbol;
    while(current_symbol = symdmp(stptr, 0))
        symdel(stptr, current_symbol);
    free(stptr);
    return;
}
count)
        