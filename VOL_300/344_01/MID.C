/*
HEADER:         ;
TITLE:          BASIC like mid() string function
VERSION:        1.0;

DESCRIPTION:    provides a mid() function like BASIC mid$() with similar
                syntax, this comes in handy when converting BASIC code
                to C manually.
                printf("%s\n",mid(string,10,0)); this prints the entire
                    string beginning with string[ 10 ], since the
                    end limit was not given (that is 0)
                printf("%s\n",mid(string,2,5)) prints the string from
                    string[ 2 ] to string [ 5 ]
                A pointer to a static buffer is returned and null
                    terminated. The original string of chars remains
                    unaffected
                On error, this function returns an empty string (first
                    char is null), that is: ""

KEYWORDS:       BASIC,mid,string;
SYSTEM:         Xenix 3.4b, MSDOS;
FILENAME:       mid.c
WARNINGS:       compile with -dNO_PROTOTYPE if your system does not
                support prototyping, with -dFOR_MSDOS if you are compiling
                for MSDOS with an ANSI standard compiler.
                Defaults assume compiling with prototypes for
                Xenix 3.4b on Altos 2086 computer.

SEE-ALSO:       demo.c;
AUTHORS:        Vern Martin, 449 W. Harrison, Alliance, Ohio 44601;
COMPILERS:      ECOSOFT ECO-C88, XENIX 3.4B STANDARD COMPILER;
*/

#include "vernmath.h"
#define LINEIN 255  /* maximum line length to be handled */
#define nl()    retstring[0] = (char) NULL

char *mid(string,first,last)
char *string;
int first,last;
{
/* local char */
    static char retstring[LINEIN+1];
/* local int */
    int x,y,len;
    len = strlen(string);

/* check for overflow */
    if (len > LINEIN) {
        nl();
        return(retstring);
    }

/* make sure first and last set to proper values */
    if ( first > len-1) {
        nl();
        return ( retstring );
    }
    if ( last > len-1 || last <= 0 ) last = len-1;
    if ( first < 0 ) first = 0;

/* copy result into return buffer */
    for ( x = first,y=0; x <= last; x++,y++) retstring[y] = string[x];
    retstring[y] = (char) NULL;

    return(retstring);
}
