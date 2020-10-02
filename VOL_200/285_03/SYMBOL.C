/*--------------------------------------------------------------------------
**		file: 		SYMBOL.C
**		project:	hoc3, "Higher Order Calculator"
*/

#include <stdio.h>
#include <malloc.h>

#include "hocdecl.h"

static Symbol *symlist = (Symbol *) 0 ;

/*--------------------------------------------------------------------------
**	LOOKUP	-	find symbol in table
*/
Symbol *lookup( char *s)      /* find s in symbol table */
{
 Symbol *sp ;

   for (sp = symlist ; sp != (Symbol *) 0 ; sp = sp->next)
      if (strcmp( sp->name, s) == 0)
         return sp ;                /* found  */

   return (Symbol *) 0 ;            /* symbol not found */
}

/*--------------------------------------------------------------------------
**	INSTALL	-	install symbol in table
*/
Symbol *install( char *s, int t, double d)    /* install s in symbol table */
{
 Symbol *sp ;

 	sp = (Symbol *) emalloc( sizeof( Symbol)) ;

	sp->name = emalloc( strlen(s) + 1) ;
	strcpy( sp->name, s) ;

	sp->type = t ;
	sp->u.val = d ;
	sp->next = symlist ;		/* add symbol to front of list */
	symlist = sp ;

	return sp ;
}

/*--------------------------------------------------------------------------
**	EMALLOC	-	allocate memory for string, complain on error.
*/
char *emalloc( unsigned n)
{
 char *p ;

   p = (char *) malloc( n) ;
   if (p == (char *)0)
      execerror("out of memory", (char *) 0) ;
   return p ;
}

/* end of symbol.c for HOC3.y */
