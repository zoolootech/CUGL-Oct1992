/*
HEADER:         ;
TITLE:          verns math functions demostration program
VERSION:        1.0;

DESCRIPTION:    demonstrates vern's math functions

KEYWORDS:       demonstration;
SYSTEM:         Xenix 3.4b, MSDOS;
FILENAME:       demo.c
WARNINGS:       compile with -dNO_PROTOTYPE if your system does not
                support prototyping, with -dFOR_MSDOS if you are compiling
                for MSDOS with an ANSI standard compiler.
                Defaults assume compiling with prototypes for
                Xenix 3.4b on Altos 2086 computer.

SEE-ALSO:       ncvd.c, ncvs.c, vmkd.c, vmks.c, print_tab.c,nrnd54.c.
AUTHORS:        Vern Martin, 449 W. Harrison, Alliance, Ohio 44601;
COMPILERS:      ECOSOFT ECO-C88, XENIX 3.4B STANDARD COMPILER;
*/


#include "vernmath.h"

MAIN_TYPE main()
{

    double test = 10.34897578;
    char i[ 2 ];
    static char search[] = "SEARCH THIS STRING";
    static char find1[] = "THIS";
    static char find2[] = "XYZ";
    int x;

    printf("original test value = %lf\n",test);
    for (x = 0; x < 7; x++) {
        printf("%lf, rounded to %d places = %lf\n",test,x,nrnd54(test,x));
    }

    printf("value rounded to nearest nickel = %lf\n",rndnick(test));

    printf("value packed, unpacked (mkd/cvd) = %lf\n",ncvd(vmkd(test)));
    printf("value packed, unpacked (mks/cvs) = %lf\n",ncvs(vmks(test)));

    mki(i,3);
    printf("value packed, unpacked (mki/cvi) = %d\n",cvi(i));

    printf("searching \"%s\" for \"%s\", instr() returned %d\n",
            search,find1,instr(0,search,find1));

    printf("searching \"%s\" for \"%s\", instr() returned %d\n",
            search,find2,instr(0,search,find2));

    printf("print \"%s\", right 4 chars = \"%s\"\n",
            search,right(search,4));

    printf("print \"%s\" from char 10 on = \"%s\"\n",
            search,mid(search,10,0));

    printf("print \"%s\" from char 10 to 12 = \"%s\"\n",
            search,mid(search,10,12));

    for (x = 0; x < 75; x += 8) {
        print_tab(stdout,x,0,"hello");
    }

    /* don't tab to anywhere, but have print_tab() do a line feed */
    /* this clears print_tab() of previous tabs and advances to next line */
    print_tab(stdout,0,1,"");

}
