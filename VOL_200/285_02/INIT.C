/*--------------------------------------------------------------------------
**		file: 		INIT.C
**		project:	hoc3, "Higher Order Calculator"
*/

#include <math.h>
#include "hocdecl.h"
#include "hoc3.h"


static struct  {
         char *name ;
         double cval ;
} consts[] = {
   "PI",    3.14159265358979323846,
   "E",     2.71828182845904523536,
   "GAMMA", 0.57721566490153286060,
   "DEG",   57.29577951308322087680,
   "PHI",   1.61803389974989484820,
   "\0",	0.0
} ;

static struct  {
         char     *name ;
         double   (*func)() ;
} builtins[] = {
        "sin",		sin,
        "cos",		cos,
        "atan",		atan,
        "log",		Log,
        "log10",	Log10,
        "exp",		Exp,
        "sqrt",		Sqrt,
        "int",		integer,
        "abs",		fabs,
        "\0",     	(void *) 0
} ;

void init( void)
{
 int i ;
 Symbol *s ;

   for (i=0 ; consts[i].name ; i++)
      (void) install( consts[i].name, VAR, consts[i].cval) ;
   for (i=0 ; builtins[i].name ; i++)
   {
      s = install( builtins[i].name, BLTIN, 0.0) ;
      s->u.ptr = builtins[i].func ;
   }   
}

/* end of init.c for HOC3.y */
