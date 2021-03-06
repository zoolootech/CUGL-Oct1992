/*
 * symtab.c
 *
 * This module contains the functions to init the symbol table and
 * hash, store, find or test a symbol.
 *
 */

#include <ctype.h>
#include <stdio.h>
#include "ctocxx.h"

#define NIL 0

static struct symbol *st[TABSIZ];  /* this is the hashing symbol table */
static char sbuf[SYMSIZ];        /* static area for symbol name storage */

int testsym();
char *malloc();
char *strcpy();
char *strcat();


/*--------------------------------------------------------------*/
/* hash does more than just figure out a hash code for a symbol */
/* name.  It also puts the symbol into a temporary static area  */
/* to assure that there is no garbage in the storage area after */
/* the trailing NULL character.                                 */

static hash(symbol_text)
    char symbol_text[];
    {
    register char *ptr;
    register int i;

    /* clear local area */
    for (ptr = sbuf; ptr < sbuf + SYMSIZ; ++ptr)
        *ptr = '\0';

    /* copy symbol into local area - only SYMSIZ-1 characters are used */
    i = 0;
    ptr = symbol_text;

    while (*ptr)
        {
        if (i >= SYMSIZ - 1)
            break;
        sbuf[i++] = *ptr++;
        }

    /* figure hash code */
    ptr = sbuf;
    return((50*(ptr[0]) + 10*(ptr[1]) + (ptr[2]) + ptr[5])%TABSIZ);
    }

/*------------------------------------------------------------*/
/* newpiece will allocate a new area,  assign the symbol name */
/* currently in sbuf, and initialize the rest of the fields.  */
/* newpiece returns the address of the new area allocated.    */

static struct symbol *newpiece(val)
    long val;
    {
    register SYMPTR s;

    s = (SYMPTR) (malloc(sizeof (SYM)));
    strcpy(s->name,sbuf);
    s->muldef = FALSE;
    s->type = 0;    /* the default */
    s->chain  = NIL;
    s->uval.lval = val;
    return(s);
    }
/*------------------------------------------------------------*/

initsymtab()
    {
    int i;

    for (i=0; i<TABSIZ ; i++)
        st[i]=NIL;
    }

/*--------------------------------------------------------------------*/
/* storesym - stores new symbol name and returns ptr to symbol struct */

SYMPTR storesym(x,val)
    char x[];   /* symbol name */
    long val;   /* symbols value */
    {
    int found;
    int h;
    register SYMPTR s;

    h=hash(x);
    if (st[h] == NIL )
        {
        st[h] = newpiece(val);
        }
    else
        {
        /* slot not empty, check collision chain for multiply defined */
        found = FALSE;
        for (s = st[h] ; s != NIL ; s = s->chain)
            {
            if (testsym(s->name))
                {
                /* Aha!  it IS on this chain */
                found = TRUE;
                break;
                }
            }
        if (found)
            {
            /* found old symbol definition */
            s->muldef=TRUE;
            return(s);
            }

        /*link new sym onto collision chain */
        s = st[h];
        st[h] = newpiece(val);
        st[h]->chain = s;
        }

    /* a new piece was allocated onto the beginning of st[h] */
    return(st[h]);
    }

/*------------------------------------------------------------*/
/* findsym - looks up name in symbol table and returns ptr to struct */

SYMPTR findsym(tag)
char *tag;
    {
    register SYMPTR p;
    int h;

    h = hash(tag);
    p = st[h];
    while ( p!= NIL )
        {
        if (testsym(p->name))
            break;              /* match found */
        else
            p=p->chain;
        }
    return(p);
    }

/*----------------------------------------------------------*/
/* testsym - checks for name match, used by findsym()   */

int testsym(name)
    char name[];
    {
    int i;
    i=0;
    while( sbuf[i]==name[i])
        if( sbuf[++i] == '\0' && name[i] == '\0' )
            return(TRUE);
    return(FALSE);
    }

