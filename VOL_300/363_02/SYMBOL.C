/***********************************************************************
 *
 *  SYMBOL.C
 *  Symbol Handling Routines for 68020 Assembler
 *
 *  Function: create()
 *  Pass 1:
 *      If the symbol has not been created earlier creates a new symbol and
 *      clears the flags. If the symbol exists MULTDEF flag is set.
 *  Pass 2:
 *      If      MULTDEF flag is set then MULTIPLE_DEFS error is issued,
 *      else if value passed is different from value stored then PHASE_ERROR
 *              is issued,
 *      else    BACKREF flag is set so that all references to
 *              that symbol encountered before this point are known to be
 *              forward references, encountered after this point - backward
 *              references.
 *
 *      In addition in all cases the routine returns a pointer to the 
 *      structure (type symbolDef) which contains the symbol that was found
 *      or created. The routine uses a hash function to index into an array
 *      of pointers to ordered linked lists of symbol definitions.
 *
 *      Note: no use is made of the ordering of the list, it could just as
 *      well be unordered.
 *
 *      create() is called by modules 'assemble', 'directiv', and 'movem'
 *      
 *
 *  Function: lookup()
 *      Searches the symbol table for the symbol whose name is specified.
 *      If      the symbol exists the pointer to the symbolDef structure is
 *              returned,
 *      else    UNDEFINED error is issued and NULL is returned.
 *
 *      lookup() is called by modules directiv, eval, movem, and opparse;
 *
 *   Usage: symbolDef *create(char *sym, long value, int *errorPtr)
 *          symbolDef *lookup(char *sym, int *errorPtr)
 *
 *      Author: Paul McKee
 *      ECE492    North Carolina State University,  11/28/86
 *
 *      Modified A.E. Romer. Version 1.0
 *          17 March 1991. ANSI function definitions.
 *          31 March 1991. Bug corrected. Multiple symbol definitions were not 
 *                         flagged as errors.
 *
 ************************************************************************/


#include <stdio.h>
#include <ctype.h>
#include "asm.h"

/* HASHSIZE is the range of the hash function. hash()
   returns values from 0 to HASHSIZE-1. */
    
#define HASHSIZE 27

static symbolDef *htable[HASHSIZE];
extern char pass2;                           /* Flag set during second pass */


void hashInit(void)                                /* initialize hash table */
    {
    int h;

    for (h = 0; h < HASHSIZE; h++)
        htable[h] = NULL;
    }


int hash(char *symbol)                 /* evaluate hash No for given symbol */
    {
    int sum;

    sum = 0;
    while (*symbol)
        {
        sum += (isupper(*symbol)) ? (*symbol - 'A') : 26;
        symbol++;
        }
    return (sum % HASHSIZE);
    }


symbolDef *create(char *sym, long value, int *errorPtr)
    {
    int h, cmp;
    symbolDef *s, *last, *t;
    h = hash(sym);

    if ((s = htable[h]) != NULL)         /* at least one entry at this hash */
        {

        /* Search the linked list for a matching symbol */
        last = NULL;
        while (s && (cmp = strcmp(s->name, sym)) < 0)
                                               /* find nearest lower symbol */
            {
            last = s;
            s = s->next;
            }

        if (s && (cmp == 0))                                 /* match found */
            {
            if (!pass2)                                           /* pass 1 */
                s->flags |= MULTDEF;                            /* set flag */
            else                                                  /* pass 2 */
                if (value != s->value)
                    {
                    NEWERROR(*errorPtr, PHASE_ERROR);
                    }
                else if (s->flags & MULTDEF)
                    {
                    NEWERROR(*errorPtr, MULTIPLE_DEFS);
                    }
                else
                    s->flags |= BACKREF;
            t = s;
            }

        /* No match. Insert the symbol in the list, between lower and higher
         * symbol (ordered list) */
        else if (!pass2)
            if (last)               /* there are higher entries, the symbol
                                     * goes after an existing symbol */
                {
                t = (symbolDef *) malloc(sizeof(symbolDef));
                last->next = t;
                t->next = s;
                strcpy(t->name, sym);
                t->value = value;
                t->flags = 0;
                }
            else                    /* no higher entries, the symbol
                                     * goes at the head of a list */
                {
                t= (symbolDef *) malloc(sizeof(symbolDef));
                t->next = htable[h];
                htable[h] = t;
                strcpy(t->name, sym);
                t->value = value;
                t->flags = 0;
                }
        }

    else                        /* No entry at this hash. Must be pass 1,
                                 * in pass 2 the entry would be there */

        {
        t = (symbolDef *) malloc(sizeof(symbolDef));
        htable[h] = t;
        t->next = NULL;
        strcpy(t->name, sym);
        t->value = value;
        t->flags = 0;
        }
    return t;
    }


symbolDef *lookup(char *sym, int *errorPtr)
    {
    symbolDef *s;
    int cmp;

    if ((s = htable[hash(sym)]) == NULL)           /* no entry at this hash */
        {
        NEWERROR(*errorPtr, UNDEFINED);
        return NULL;
        }

/* There is an entry, search the linked list for a matching symbol */

    while (s && (cmp = strcmp(s->name, sym)) < 0)
                                               /* find nearest lower symbol */
        s = s->next;

    if (cmp == 0)                                            /* match found */
        return s;
    else                                                        /* no match */
        {
        NEWERROR(*errorPtr, UNDEFINED);
        return NULL;
        }
    }

