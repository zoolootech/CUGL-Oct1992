/*
HEADER:         ;
TITLE:          5/4 rouding routine
VERSION:        1.0;

DESCRIPTION:    given a value to round and no. of digits desired, return
                the rouded number

KEYWORDS:       5/4, rounding;
SYSTEM:         Xenix 3.4b, MSDOS;
FILENAME:       nrnd54.c;
WARNINGS:

SEE-ALSO:       ;
AUTHORS:        Vern Martin, 449 W. Harrison, Alliance, Ohio 44601;
COMPILERS:      ECOSOFT ECO-C88, XENIX 3.4B STANDARD COMPILER;
*/

#include "vernmath.h"

double nrnd54(value,digits)
double value;   /* value to round                  */
int digits;     /* number of digits to of accuracy */
{
    double f;

#define F_M1 (((pow(10.0,(double)((digits)*-1))))*.5)
#define rounded (((long)((value+F_M1)*f))/f)

    f = pow(10.0,(double)digits);

    return(rounded);
}
