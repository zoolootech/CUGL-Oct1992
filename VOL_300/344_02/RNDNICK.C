/*
HEADER:         ;
TITLE:          round to nearest nickel;
VERSION:        1.0;

DESCRIPTION:    function rounds value to nearest nickel

KEYWORDS:       rouding, nickel;
SYSTEM:         Xenix 3.4b, MSDOS;
FILENAME:       rndnick.c
WARNINGS:       compile with -dNO_PROTOTYPE if your system does not
                support prototyping, with -dFOR_MSDOS if you are compiling
                for MSDOS with an ANSI standard compiler.
                Defaults assume compiling with prototypes for
                Xenix 3.4b on Altos 2086 computer.

SEE-ALSO:       vernmath.h
AUTHORS:        Vern Martin, 449 W. Harrison, Alliance, Ohio 44601;
COMPILERS:      ECOSOFT ECO-C88, XENIX 3.4B STANDARD COMPILER;
*/

#include "vernmath.h"

#define ROUND_TO_NEAREST    (5.0)   /* round to neareast 5 cents */
#define CUT_OFF             (3)     /* less than this and round down,
                                        greater or ==, and round up */

double rndnick(amt)
double amt; /* amount to round */
{
    double remainder;   /* remainder after modulus operation */

    /* multiply amount by 100 and then throw away anything
        that is right of the decimal */
    /* then perform modulus arithmetic to find out what remainder
        is afger deviding by the ROUND_TO_NEAREST factor (nickel) */


    amt = nrnd54(amt,2) * 100;

    /* round output of fmod(), it is not an exact number and must be ! */
    remainder = nrnd54(fmod(amt,ROUND_TO_NEAREST),0);

    /* now, if remainder is less than the cut off, subtract it from
        the amount, else add the difference between the remainder
        and the cut off, thus rounding up to the nearest nickel */

    if ( (int) remainder < CUT_OFF )  {
        amt -= remainder;
    }
    else {
        amt += ROUND_TO_NEAREST - remainder;
    }

    return( amt * .01 );

}
