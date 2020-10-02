/*
HEADER:         ;
TITLE:          BASIC mkd() in C
VERSION:        1.0;

DESCRIPTION:    performs BASIC mkd() packing a value into a string 8 chars
                long

KEYWORDS:       ;
SYSTEM:         Xenix 3.4b, MSDOS;
FILENAME:       vmkd.c
WARNINGS:       compile with -dNO_PROTOTYPE if your system does not
                support prototyping, with -dFOR_MSDOS if you are compiling
                for MSDOS with an ANSI standard compiler.
                Defaults assume compiling with prototypes for
                Xenix 3.4b on Altos 2086 computer.

SEE-ALSO:       ;
AUTHORS:        Vern Martin, 449 W. Harrison, Alliance, Ohio 44601;
COMPILERS:      ECOSOFT ECO-C88, XENIX 3.4B STANDARD COMPILER;
*/

#include "vernmath.h"

#define WIDTH 8

char *vmkd(amt)
double amt;
{
/* local int */
    int e;      /* exponent */
    double mantissa = frexp(amt,&e);

    int i,sign = 1;

    static unsigned char c[WIDTH];
    static double k[ WIDTH -1 ] = {
        2.7755575615628914e-17,
        7.1054273576010019e-15,
        1.8189894035458565e-12,
        4.6566128730773926e-10,
        1.1920928955078125e-07,
        3.0517578125000000e-05,
        7.8125000000000000e-03,
    };

/* make mantissa a positive number if necessary */
    if (mantissa < 0.0) {
        sign = -1;
        mantissa *= (double) sign;
    }

/* adjust result by multiplying mantissa by 2 until mantissa is greater than 1,
    each time you multiply the mantissa by 2, subtract 1 from
    the power you raise two by, in this way we duplicate the values
    returned by BASIC's mkd() which returns 1.x * 2^e, where as
    frexp() returns .x * 2^e */

    while(mantissa < 1.0) {
        mantissa *= 2.0;
        e--;
    }

    c[ WIDTH - 1 ] = (unsigned char) (e + 129);

/* total first 3 chars multiplied by there respective factors */
    for ( i = WIDTH - 2; i >= 0;i-- ) {
        if (mantissa < k[ i ]) c[ i ] = (unsigned char) 0;
        else {
            c[ i ] = (unsigned char) (mantissa/k[ i ]);
            mantissa -= c[ i ] * k[ i ];
        }
    }

/* unless the number is negative, reverse next to last char */
    if (sign != -1) {
        c[ WIDTH - 2 ] -= (unsigned char) 128;
    }

    if (amt == 0.0) c[ WIDTH - 1 ] = (unsigned char) 0;


    return((char *)c);
}
