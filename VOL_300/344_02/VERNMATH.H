/*
HEADER:         ;
TITLE:          Vern's Math Header;
VERSION:        1.0;

DESCRIPTION:    header to include for using vern's math routines, these
                routines help in interpreting BASIC data files storing
                packed double, single and integer precision numbers.
                Also some rounding functions and a function to print
                at tabs.

                PLEASE NOT: two macros in this header file give the
                BASIC integer precision cvd() and mki() functions.
                This packs and unpacks an integer in a string of
                2 chars in length

KEYWORDS:       double precision, single precision, BASIC data files
                print at tab,integer presicion,strings;
SYSTEM:         Xenix 3.4b, MSDOS;
FILENAME:       vmath.h;
WARNINGS:       compile with -dNO_PROTOTYPE if your system does not
                support prototyping, with -dFOR_MSDOS if you are compiling
                for MSDOS with an ANSI standard compiler.
                Defaults assume compiling with prototypes for
                Xenix 3.4b on Altos 2086 computer.

SEE-ALSO:       ncvd.c, ncvs.c, vmkd.c, vmks.c, print_tab.c;
AUTHORS:        Vern Martin, 449 W. Harrison, Alliance, Ohio 44601;
COMPILERS:      ECOSOFT ECO-C88, XENIX 3.4B STANDARD COMPILER;
*/

#include <stdio.h>

#define MKD_LEN     8       /* length of double precision packed
                            variable created by nmkd() */
#define MKS_LEN     4       /* single presicion: nmks() */
#define MKI_LEN     2       /* integer precision: mki() */

#ifdef FOR_MSDOS
#   include <math.h>
#   include <string.h>
#   define MAIN_TYPE int
#else
#   define MAIN_TYPE void
#endif

#ifndef NO_PROTOTYPE

    double nrnd54(double,int),rndnick(double),ncvd(char *),ncvs(char *);

    int instr(int,char *,char *);

    char *mid(char *,int,int),*right(char *,int),*vmkd(double),*vmks(double);

#   ifdef FOR_MSDOS
    int print_tab(FILE *,int,int,...);
#   else
    int print_tab(),strlen(char *);
    double  fmod(double, double),frexp(double, int *),pow(double, double);
#   endif

#else

    double nrnd54(),rndnick(),ncvd(),ncvs();

    int instr(),print_tab();

    char *mid(),*right(),*vmkd(),*vmks();

#ifndef FOR_MSDOS
    double fmod(),frexp(),pow();
    int strlen();
#endif

#endif

/* cvi() -- unpack single precision number (mbasic) */
#define cvi(str)    ((int)((unsigned char) str[0]))

/* mki() -- pack single precision number */
#define mki(to,number) { to[0] = (char) number; to[1] = (char) '\0'; }
