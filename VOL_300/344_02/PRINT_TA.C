/*
HEADER:         ;
TITLE:          print at tabs function
VERSION:        1.0;

DESCRIPTION:    provides a print at a given tab position function.
                This function was designed to mimic the tab() function
                of BASIC including its idiosyncrisies. This is hady
                If you have to maintain reports written both in BASIC
                and C, as I have had to do over the years.
          syntax: print_tab(file_pointer,tab_position,car_ret_flag,args...)
                Note: tab position 1 is the first char of the line
                examples:
                    print_tab(fp_report,10,1,"hello") -- this prints
                        to fp_report at tab position 10, the string "hello"
                        and DOES A CARRIAGE RETURN (flag = 1)
              print_tab(fp_report,40,0,"sum = %lf, avg = %lf",sum,avg) --
                    prints to fp_report at tab 40, two values plus
                    message string, and does not do a carriage returns

              RULES TO KEEP IN MIND:
                    print_tab() must keep track of where the current
                    imaginary print head is. It assumes that it is
                    all the way on the left margin on the initial call.
                    Either use the print_tab() exclusively to print
                    everything to a file, or make sure the last call
                    to print_tab() requested a carriage return, and
                    Any printing before the next call to print_tabn()
                    ended in a carriage return.

KEYWORDS:       tab,BASIC;
SYSTEM:         Xenix 3.4b, MSDOS;
FILENAME:       print_tab.c
WARNINGS:       My compiler did not have stdarg.h so I used varargs.h,
                you may have to modify some if you are using stdarg.h
                I'm not sure.
                compile with -dNO_PROTOTYPE if your system does not
                support prototyping, with -dFOR_MSDOS if you are compiling
                for MSDOS with an ANSI standard compiler.
                Defaults assume compiling with prototypes for
                Xenix 3.4b on Altos 2086 computer.

SEE-ALSO:       demo.c
AUTHORS:        Vern Martin, 449 W. Harrison, Alliance, Ohio 44601;
COMPILERS:      ECOSOFT ECO-C88, XENIX 3.4B STANDARD COMPILER;
*/


#include <stdio.h>
#include <varargs.h>
#ifdef FOR_MSDOS
#   include <stdlib.h>
#endif

#define done(x) { va_end(args); return(x); }

int print_tab(fp,tab,crflag,va_alist)
FILE *fp;   /* file to print to */
int tab;    /* tab position */
int crflag; /* if == 1, then carriage return at end of print */
va_dcl      /* whatever other arguments passed to sprintf */
{
/* local va_list */
    va_list args;   /* args for sprintf */
/* local char */
#define PRBUF_MAX 254
    static char prbuf[ PRBUF_MAX + 1 ]; /* print buffer */
            /* static to avoid setting up every time */
    char *fmt;  /* format for vsprintf() */
    char *crstring = (crflag == 1) ? "\n" : "";
/* local int */
    int cct;    /* char count, no. of chars printed to prbuf */
    static int tabpos = 1;/* absolute tab position, this is where the
            print head is */
    int spaces; /* number of spaces to add in order to achiev tab */
    int output = 0; /* number of characters in output to file */
    int add = 0;    /* output + add == total chars to file */

/* initialize args */
    va_start(args);

    fmt = va_arg(args,char *);

    if ( (cct = vsprintf(prbuf,fmt,args)) > PRBUF_MAX) done(-10);

    if (tab == 0) spaces = tab; else spaces = tab - tabpos;

/* if current print head postion exceeds position desired, advance to begin
    of next line, and tab desired spaces */
    if (spaces < 0) {
        if (fprintf(fp,"\n") < 0) done(-15);
        tab--;  /* this is necessary to match what tab() in BASIC
            does */
        spaces = tab;
        tabpos = 1;
        add = 2; /* count \n + null at end of string */
    }

/* add one more to the tabbing, to agree with BASIC tab()  */

    if ( (output = fprintf(fp,"%*.s%s%s",spaces," ",prbuf,crstring))
        < 0) done(-20);

    if (crflag == 1) {
        tabpos = 1;
    }
    else {

        if (tab == 0) tabpos += cct; else tabpos += cct + tab - tabpos;

    }

/* return number of characters placed in file */
    done(output + add);
}
