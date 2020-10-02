/***********************************************************************
 *
 *      MAIN.C
 *      Main Module for 68020 Assembler
 *
 *    Function: main()
 *      Parses the command line, opens the input file and output files, and 
 *      calls processFile() to perform the assembly, then closes all files.
 *
 *   Usage: main(argc, argv);
 *      int argc;
 *      char *argv[];
 *
 *      Author: Paul McKee
 *      ECE492    North Carolina State University, 12/13/86
 *
 *      Modified A.E. Romer. Version 1.0
 *          17 March 1991:  ANSI functions, braces layout.
 *          26 April 1991:  Default extension for input files added, VMS
 *                          compatibility removed.
 *
 ************************************************************************/


#include <stdio.h>
#include <ctype.h>
#include "asm.h"


#define     FNAMSIZE        13          /* name = 8, dot = 1, ext = 3,
                                         * plus terminating '\0' */
#define     DEFAULT_IFX     ".ASM"      /* default input-file extension */
#define     DEFAULT_OFX     ".H68"      /* default output-file extension */
#define     DEFAULT_LFX     ".LIS"      /* default listing-file extension */

extern FILE *inFile;                                          /* Input file */
extern FILE *listFile;                                      /* Listing file */
extern FILE *objFile;                                        /* Object file */
extern char line[256];                                       /* Source line */
extern int errorCount, warningCount;       /* Number of errors and warnings */


extern char listFlag;   /* True if a listing is desired */
extern char objFlag;    /* True if an object code file is desired */
extern char xrefFlag;   /* True if a cross-reference is desired */
extern char cexFlag;    /* True is Constants are to be EXpanded */
/**********************************************************************
 *
 *  Function getopt() scans the command line arguments passed
 *  via argc and argv for options of the form "-x". It returns
 *  the letters of the options, one per call, until all the
 *  options have been returned; it then returns EOF. The argi
 *  argument is set to the number of the argv string that
 *  is currently being examined.
 *
 *********************************************************************/


int getopt(int argc, char *argv[], char *optstring, int *argi)
    {
    static char *scan = NULL;   /* Scan pointer */
    static int optind = 0;      /* argv index */

    char c;
    char *place;

    if (scan == NULL || *scan == '\0')
        {
        if (optind == 0)
            optind++;

        if (optind >= argc || argv[optind][0] != '-' ||
                                                    argv[optind][1] == '\0')
            {
            *argi = optind;
            return(EOF);
            }
        if (strcmp(argv[optind], "--")==0)
            {
            optind++;
            *argi = optind;
            return(EOF);
            }

        scan = argv[optind]+1;
        optind++;
        }

    c = *scan++;
    place = strchr(optstring, c);

    if (place == NULL || c == ':')
        {
        fprintf(stderr, "Unknown option -%c\n", c);
        *argi = optind;
        return('?');
        }

    place++;
    if (*place == ':')
        {
        if (*scan != '\0')
            scan = NULL;
        else
            optind++;
        }

    *argi = optind;
    return c;
    }


/**********************************************************************
 *
 *  Function help() prints out a usage explanation if a bad
 *  option is specified or no filename is given.
 *
 *********************************************************************/

int help()
    {
    puts("Usage: asm [-cln] infile[.ext]\nor:    asm -h\nor:    asm ?\n");
    puts("Options: -c  Show full target code for lomg instructions");
    puts("         -l  Produce listing file (infile.lis)");
    puts("         -n  Produce NO object file (infile.h68)");
    puts("         -h  Print (this) help message");
    puts("          ?  Print (this) help message");
    exit(0);
    return NORMAL;
    }


int setFlags(int argc, char *argv[], int *argi)
    {
    int option;

    while ((option = getopt(argc, argv, "chln", argi)) != EOF)
        {
        switch (option)
            {
            case 'c' : cexFlag = TRUE; break;
            case 'h' : help(); break;
            case 'l' : listFlag = TRUE; break;
            case 'n' : objFlag = FALSE; break;
            case '?' : help(); break;
            }
        }

    return NORMAL;
    }


int main(int argc, char *argv[])
    {
    char fileName[FNAMSIZE], outName[FNAMSIZE], *p;
    int i, j;

    puts("68020 Assembler by AER\n");
    setFlags(argc, argv, &i);
    /* Check whether a name was specified */
    if (i >= argc)
        {
        fputs("No input file specified\n\n", stderr);
        help();
        }
    if ((strcmp("?", argv[i])) == 0)
        help();

    for (j = 0; j < FNAMSIZE; j++)
        {
        *(fileName + j) = *(argv[i] + j);
        if (*(argv[i] + j) == '\0')
            break;
        }

    if (strchr(fileName, '.'))
        ;                           /* do nothing if extension specified */
    else
        strcpy((fileName + strlen(fileName)), DEFAULT_IFX);
                                    /* append default input-file extension */
    inFile = fopen(fileName, "r");
    if (!inFile)
        {
        fprintf(stderr, "Input file \"%s\" not found\n", fileName);
        exit(0);
        }

    /* Process output file names in their own buffer */
    strcpy(outName, fileName);

    /* Replace extension in output file names */
    p = strchr(outName, '.');

    if (listFlag)
        {
        strcpy(p, DEFAULT_LFX); /* append listing-file extension */
        initList(outName);
        }

    if (objFlag)
        {
        strcpy(p, DEFAULT_OFX); /* append output-file extension */
        initObj(outName);
        }

    /* Assemble the file */
    processFile();

    /* Close files and print error and warning counts */
    fclose(inFile);
    if (listFlag)
        {
        putc('\n', listFile);
        if (errorCount > 0)
            fprintf(listFile, "%d error%s detected\n", errorCount,
                                            (errorCount > 1) ? "s" : "");
        else
            fprintf(listFile, "No errors detected\n");
        if (warningCount > 0)
            fprintf(listFile, "%d warning%s generated\n", warningCount,
                (warningCount > 1) ? "s" : "");
        else
            fprintf(listFile, "No warnings generated\n");
        fclose(listFile);
        }
    if (objFlag)
        finishObj();
    if (errorCount > 0)
        fprintf(stderr, "%d error%s detected\n", errorCount,
                                                (errorCount > 1) ? "s" : "");
    else
        fprintf(stderr, "No errors detected\n");
    if (warningCount > 0)
        fprintf(stderr, "%d warning%s generated\n", warningCount,
            (warningCount > 1) ? "s" : "");
    else
        fprintf(stderr, "No warnings generated\n");

    return NORMAL;
    }
