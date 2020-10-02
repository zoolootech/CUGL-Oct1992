/***********************************************************************
 *
 *      LISTING.C
 *      Listing File Routines for 68020 Assembler
 *
 *    Function: initList()
 *      Opens the specified listing file for writing. If the file cannot be 
 *      opened, then the routine prints a message and exits.
 *
 *      listLine()
 *      Writes the current listing line to the listing file. If the line is 
 *      not a continuation, then the routine includes the source line,
 *      formatted into fields, as the last part of the listing line. If an 
 *      error occurs during the writing, the routine prints a message and 
 *      exits.
 *
 *      listLoc()
 *      Starts the process of assembling a listing line by printing the 
 *      location counter value into listData and initializing listPtr.
 *
 *      listObj()
 *      Prints the data whose size and value are specified in the object field 
 *      of the current listing line. Bytes are printed as two digits, words as 
 *      four digits, and longwords as eight digits. A space follows each group 
 *      of digits. 
 *           If the item to be printed will not fit in the object code field, 
 *      and cexFlag is TRUE, then the remainder of the data will be printed
 *      on a continuation line. Otherwise, elipses ("...") will be printed to 
 *      indicate the omission of values from the listing, and the excessive 
 *      data will not be shown in the listing file. 
 *
 *   Usage: initList(char *name)
 *
 *      listLine()
 *
 *      listLoc()
 *
 *      listObj(int data, int size)
 *
 *      Author: Paul McKee
 *      ECE492    North Carolina State University, 12/13/86
 *
 *      Modified A.E. Romer. Version 1.0
 *          17 March 1991:  ANSI functions, braces layout.
 *          Summer   1991:  Numerous small changes.
 *
 ************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include "asm.h"

#define     DATA_SIZE       44      /* maximum size of object code */
#define     ELLIPSIS        5       /* size of ellipsis (" ... ") */

/* listing fields widths, excluding following space */

#define     LINENO_FIELD    5
#define     LOC_FIELD       8
#define     LABEL_FIELD     9
#define     OPCODE_FIELD    8
#define     OPERAND_FIELD   16

/* Declarations of global variables */
extern long loc;
extern char pass2, cexFlag, continuation;
extern char line[256];
extern FILE *listFile;
extern int lineNum;
extern char noOperand;       /* TRUE if the instruction has no operand.
                              * Used by listLine. */

static char listData[DATA_SIZE];
                             /* Buffer in which location-and-object-code 
                              * field of the listing lines is assembled */

extern char *listPtr;        /* Pointer to above buffer (this pointer is
                              * global because it is actually manipulated
                              * by equ() and set() to put specially 
                              * formatted information in the listing) */

int initList(char *name)
    {

    listFile = fopen(name, "w");
    if (!listFile)
        {
        puts("Can't open listing file");
        exit(0);
        }

    return NORMAL;
    }


int listLine(void)
    {
    int i, j;

    if (!continuation)
        fprintf(listFile, "%0*d ", LINENO_FIELD, lineNum);
    else
        for (i = 0; i < LINENO_FIELD+1; i++)
            fputc(' ', listFile);               /* do not print line number
                                                 * on continuation line */
    fprintf(listFile, "%-*.*s", DATA_SIZE, DATA_SIZE, listData);
    if (!continuation)
        {
        i = 0;                              /* 'line' character count */
        j = 0;                              /* listing line character count */
        if (line[0] != '*' && line[0] != '\n')
            {
            while (!isspace(line[i]))           /* if label exists */
                {
                putc(line[i++], listFile);      /* copy label to list file */
                j++;
                }
            while (line[i] == '\t' || line[i] == ' ')
                i++;                            /* skip blanks */
            if (line[i] != '\n' && line[i] != '\0')
                {
                while (j < LABEL_FIELD)
                    {
                    putc(' ', listFile);
                                        /* fill remaining space with blanks */
                    j++;
                    }
                putc(' ', listFile);        /* add field separating blank */
                j++;
                }

            while (!isspace(line[i]) && line[i] != '\0')    /* opcode string */
                {
                putc(line[i++], listFile);      /* copy opcode string */
                j++;
                }
            while (line[i] == '\t' || line[i] == ' ')
                i++;                            /* skip blanks */
            if (line[i] != '\n' && line[i] != '\0')
                if (noOperand == TRUE)
                    {
                    while (j < LABEL_FIELD + OPCODE_FIELD + OPERAND_FIELD)
                        {
                        putc(' ', listFile);
                                    /* fill remaining space with blanks */
                        j++;
                        }
                    putc(' ', listFile);   /* add field separating blank */
                    j++;

                    while (line[i] != '\n' && line[i] != '\0')
                                                        /* comment string */
                        putc(line[i++], listFile);    /* copy comment string */
                    }
                else                /* noOperand == FALSE */
                    {
                    while (j < LABEL_FIELD + OPCODE_FIELD)
                        {
                        putc(' ', listFile);
                                    /* fill remaining space with blanks */
                        j++;
                        }
                    putc(' ', listFile);   /* add field separating blank */
                    j++;

                    while (!isspace(line[i]) && line[i] != '\0')
                                                        /* operand string */
                        if (line[i] == CHAR_DELIMITER)  
                                                    /* quoted string start */
                            {
                            putc(line[i++], listFile);
                            j++;
                            while (line[i] != CHAR_DELIMITER
                                    && line[i] != '\0' && line[i] != '\n')
                                { 
                                putc(line[i++], listFile);
                                j++;
                                }
                            if (line[i]== CHAR_DELIMITER)
                                {
                                putc(line[i++], listFile);
                                j++;
                                }
                            } 
                        else
                            {
                            putc(line[i++], listFile);
                                                    /* copy operand string */
                            j++;
                            }
                    while (line[i] == '\t' || line[i] == ' ')
                        i++;                            /* skip blanks */
                    if (line[i] != '\n' && line[i] != '\0')
                        {
                        while (j < LABEL_FIELD + OPCODE_FIELD + OPERAND_FIELD)
                            {
                            putc(' ', listFile);
                                        /* fill remaining space with blanks */
                            j++;
                            }
                        putc(' ', listFile);   /* add field separating blank */
                        j++;
                        }

                    while (line[i] != '\n' && line[i] != '\0')
                                                        /* comment string */
                        putc(line[i++], listFile);    /* copy comment string */
                    }
            putc('\n', listFile);
            }
        else            /* line[0]=!= '*' || line[0] == '\n' */
            fprintf(listFile, "%s", line);      /* copy lines without code */
        }
    else                /* continuation */
        putc('\n', listFile);
    if ferror(listFile)
        {
        fputs("Error writing to listing file\n", stderr);
        exit(0);
        }

    return NORMAL;
    }


int listLoc(void)
    {
    sprintf(listData, "%0*lX ", LOC_FIELD, loc);
    listPtr = listData + LOC_FIELD + 1;

    return NORMAL;
    }


int listObj(long data, int size)
    {
    int i;
    if (listPtr - listData + 2*size + ELLIPSIS > DATA_SIZE)
        if (cexFlag)
            {
            listLine();
            listPtr = listData;
            for (i = 0; i < LOC_FIELD+1; i++)
                *listPtr++ = ' ';
            continuation = TRUE;
            }
        else
            {
            strcpy(listPtr, "...");
            return NORMAL;
            }
    switch (size)
        {
        case BYTE : sprintf(listPtr, "%02X ", data & 0xFF);
                    listPtr += 3;
                    break;
        case WORD : sprintf(listPtr, "%04X ", data & 0xFFFF);
                    listPtr += 5;
                    break;
        case LONG : sprintf(listPtr, "%08lX ", data);
                    listPtr += 9;
                    break;
        default   : printf("LISTOBJ: INVALID SIZE CODE!\n");
                    exit(0);
        }

    return NORMAL;
    }

