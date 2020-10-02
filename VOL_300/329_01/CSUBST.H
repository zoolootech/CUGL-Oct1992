/* csubst.h */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifndef MEGAMAX
#include <assert.h>
#else
#define assert(X)
#endif 


/* These are the values of the global variable Mode */
#define APPLY_SUBSTS 1   /* apply the substitutions */
#define EXTRACT_SYMBOLS 2 /* extract the symbols  */
#define READ_SUBSTS 3 /* read in the substitutions */
#define PRINT_TRUNCATED 4 /* print the truncated symbols */
#define READ_IGNORES 5 	/* read the ignore symbols */

#define SYMBOL_SIZE 132 /* the maximum length of an identifier */
typedef int symbol_type_t;     /* to indicate type of symbol */

/* values for symbol_type_t: */
#define TYPE_MACRO 		1
#define TYPE_IDENTIFIER 	2
#define TYPE_STRING 		3

