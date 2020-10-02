/*
HEADER:         ;
TITLE:          BASIC ncvs() function in C
VERSION:        1.4;

DESCRIPTION:    performs function of BASIC ncvs(), unpacking a string
                of 4 chars previously packed by mks()

KEYWORDS:       ncvs,single precision,BASIC;
SYSTEM:         Xenix 3.4b, MSDOS;
FILENAME:       ncvs.c
WARNINGS:       compile with -dNO_PROTOTYPE if your system does not
                support prototyping, with -dFOR_MSDOS if you are compiling
                for MSDOS with an ANSI standard compiler.
                Defaults assume compiling with prototypes for
                Xenix 3.4b on Altos 2086 computer.

SEE-ALSO:       vmks();
AUTHORS:        Vern Martin, 449 W. Harrison, Alliance, Ohio 44601;
                My thanks to Mike Thompson of ACT Consulting in
                Canton, Ohio for his help several years ago to write
                the original version.
COMPILERS:      ECOSOFT ECO-C88, XENIX 3.4B STANDARD COMPILER;
*/

#include "vernmath.h"

#define WIDTH 4
#define XTYPE float
#define KTYPE float

double ncvs(string)
char *string;
{
    int i,s,e;
    XTYPE x = (XTYPE) 1;
    unsigned char c[WIDTH];
    static KTYPE k[WIDTH] = {
        (KTYPE) 1.1920928955078125e-07,
        (KTYPE) 3.0517578125000000e-05,
        (KTYPE) 7.8125000000000000e-03,
    };

/* convert chars to unsigned chars */
        for (i=0;i<=WIDTH-1;i++) {
        c[i] = (unsigned char) string[i];
    }
/* last char in string determines power to raise final total by */
    e=c[WIDTH-1]-129;

/* determine sign */
/* next to last char determines sign */
    if (c[WIDTH-2] < (unsigned char) 128) {
        s=1;  /* if it is positive, sign is 1 */
    }
    else {
        s = (-1);
        /* make it think its positive */
        c[WIDTH-2] -= (unsigned char) 128;
    }

/* total first 3 chars multiplied by there respective factors */
    for (i=0;i<WIDTH-1;i++) {
        x += (XTYPE) ((KTYPE) c[i] * k[i]);
    }

/* raise the total by the power */
    x *= (XTYPE) pow(2.,(double)e)*(XTYPE)s;
    return(x);
}
