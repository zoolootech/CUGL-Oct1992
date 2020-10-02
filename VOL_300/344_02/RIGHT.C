/*
HEADER:         ;
TITLE:          BASIC like right() string function
VERSION:        1.4;

DESCRIPTION:    provides BASIC like right() string function with
                similar syntax and return
                printf("%s\n",right(string,4)); would print the
                4 rightmost chars of string "string"

KEYWORDS:       BASIC,right,string;
SYSTEM:         Xenix 3.4b, MSDOS;
FILENAME:       right.c
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

char *right(string,len)
char *string;
int len;
{
/* local int */
    int slen;

    slen = strlen(string);
    if (len >= slen || len <= 0) return(string);
    return( ( string + slen - len) );
}
