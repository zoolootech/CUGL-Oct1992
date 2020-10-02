/*:file:version:date: "%n    V.%v;  %f"
 * "SORTX.C    V.7;  20-Oct-91,14:57:32"
 *
 *  Program: Sort
 *  Purpose: Text line sorter: sorts standard input and writes to
 *           standard output. Similar to MS-DOS sort, but collating
 *           sequence is not quite the same.  This sort is much, much
 *           faster than MS-DOS Sort on medium to large files.
 *
 *  Usage:   SORTX [/R] [/+n]         (or SORTX [-R] [-+n], UNIX)
 *
 *              Where /R  means sort descending.
 *                    /+n sort key begins in n'th column of text line.
 *              Command line options must be separated by white space.
 *
 *           MS-DOS examples:
 *           C>dir | SORTX /R /+10
 *           ...writes directory sorted by extension in descending order
 *           C>SORTX < Afile.txt > Sorted.txt
 *           ...writes sorted Afile.txt to Sorted.txt
 *
 *           DOS users should compile in a LARGE data model, SMALL code;
 *           (COMPACT, for Turbo C users).
 *
 *      Copyright (c) 1991  Bert C. Hughes
 *                          200 N.Saratoga
 *                          St.Paul, MN 55104
 *                          Compuserve 71211,577
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "tavltree.h"

#define BUFSIZE 4608               /*arbitrary but make it large*/
char IObuffer[BUFSIZE];            /*buffer for "stdin" & "stdout" */

#define MAX_STR 500

typedef struct {
            int line_no;
            char text[MAX_STR];
        } LineRec, *PLineRec;

#define LR_SIZE(x)  (sizeof(int)+sizeof(char)+sizeof(char)*strlen(((PLineRec)(x))->text))

char default_name[] = "SORTX";     /* For error messages */
char *program_name = default_name;

unsigned CmpOfs;                   /* Offset into text lines */

void error(const char *message, int code);

#if defined __MSDOS__
#define DEFAULT_SWT '/'       /* default for MS-DOS */
#else
#define DEFAULT_SWT '-'       /* UNIX ? */
#endif

char swt = DEFAULT_SWT;       /* command line switch character */

/*
 * Get options from the command line:
 *     If your system does not have "getopt", you will have to write one -
 *     but it only needs to handle the "R" option and "+n" option.
 *     A dummy routine may simply return 0 -then you can sort with no options
 *     TurboC v2.0 includes "getopt.c" in sample routines; apparently it is
 *     a part of UNIX system V (?)
 *
 * AMENDMENT: the "dummy" provided below works for this program - to add
 *            options you will need the real "getopt".
 */

#if defined USE_GETOPT

extern int  getopt(int argc, char *argv[], char *optionS);
extern char *optarg;

#else

char default_optarg[1] = "";
char *optarg;

int  getopt(int argc, char *argv[], char *optionS)
{
 /*
    NOT a general purpose "getopt"; in fact, this one will ignore "optionS".
    It looks for options interesting to this program only.
  */

    char *p;
    static int optind;
    static char Reverse_Swt[3] = {DEFAULT_SWT, 'R','\0'};
    static char Column_Swt[3]  = {DEFAULT_SWT, '+','\0'};

    optarg = default_optarg;

    while (++optind < argc) {
        if (strstr(argv[optind],Reverse_Swt))      /* option /R  */
            return('R');

        if (p = strstr(argv[optind],Column_Swt)) { /* option /+n */
            optarg = p+2;
            return('+');
        }
    } /* end while */

    return(EOF);  /* no more argv's */
}
#endif


void error(const char *message, int code)
{
    fprintf(stderr,"\n%s: ",program_name);
    if (errno)
        perror(message);
    else
        fprintf(stderr,"%s\n",message);
    exit(code);
}

int cmp(PLineRec key1, PLineRec key2)
{
    int cmpval;
    static char nullstr[] = "";
    char *p1 = ((strlen(key1->text) < CmpOfs) ? nullstr : &key1->text[CmpOfs]);
    char *p2 = ((strlen(key2->text) < CmpOfs) ? nullstr : &key2->text[CmpOfs]);

    if (!(cmpval = strcmp(p1,p2)))
        return(key1->line_no - key2->line_no);
    else
        return(cmpval);
}

int reverse_cmp(PLineRec key1, PLineRec key2)
{
    return(-cmp(key1,key2));
}

void *ident(void *p)
{
    return(p);
}

void *create(const void *p)
{
    register unsigned size = LR_SIZE(p);
    PLineRec q = malloc(size);
    if (q) memcpy(q,p,size);
    return(q);
}

void *copy(void *dest, const void *src)
{
    return(memcpy(dest,src,LR_SIZE(src)));
}


main(int argc, char **argv)
{
    int (*compare)() = cmp;
    LineRec item;
    TAVL_treeptr List;
    TAVL_nodeptr p;
    int options;
    long Uval;
    char emsg[64];

   /* get options: ?reverse sort?  ?compare beginning at column N? */

    do {
        options = getopt(argc, argv, "R+:");
        switch (options) {
            case 'R':   compare = reverse_cmp;
                        break;

            case '+':   Uval = strtol(optarg,NULL,10);
                        if ((Uval > 0) && (Uval < MAX_STR-1))
                            CmpOfs = Uval - 1;
                        else {
                            sprintf(emsg,"%c+n  :invalid \"n\": requires  0 < n < %d",swt,MAX_STR-1);
                            error(emsg,1);
                        }
                        break;

            case '?':   fprintf(stderr,"\nUsage: %s [%cR] [%c+n]\n",program_name,swt,swt);
                        fprintf(stderr,"INPUT is from \"standard input\", ");
                        fprintf(stderr,"OUTPUT is to \"standard output\".\n");
                        exit(1);
        }
    } while (options != EOF);

    if (!(List = tavl_init(compare,ident,create,free,copy,malloc,free)))
        error("At tavl_init",1);

 /* read file into tree */

    setbuf(stdin,IObuffer);

    while (fgets(item.text,MAX_STR-2,stdin)) {
        if (!tavl_insert(List,&item,0))
            error("At tavl_insert",1);
        item.line_no++;                 /* Use line #'s to distinguish */
    }                                   /* identical lines - otherwise */
                                        /* duplicates will be eliminated! */
 /* write file from tree in order */

    setbuf(stdout,IObuffer);

    p = tavl_reset(List);
    while (p = tavl_succ(p)) {
        tavl_getdata(List,p,&item);
        fputs(item.text,stdout);
    }
}
