/***********************************************************************
 *
 *      ASSEMBLE.C
 *      Assembly Routines for 68020 Assembler
 *
 *    Function: processFile()
 *      Assembles the input file. For each pass, the function passes each line 
 *      of the input file to assemble() to be assembled. The routine also makes
 *      sure that errors are printed on the screen and listed in the listing 
 *      file and keeps track of the error counts and the line number.
 *
 *      assemble()
 *      Assembles one line of assembly code. The line argument points to the 
 *      line to be assembled, and the errorPtr argument is used to return an 
 *      error code via the standard mechanism. The routine first determines if 
 *      the line contains a label and saves the label for later use. It then 
 *      calls instLookup() to look up the instruction (or directive) in the 
 *      instruction table. If this search is successful and the parseFlag for 
 *      that instruction is TRUE, it defines the label and parses the source 
 *      and destination operands of the instruction (if appropriate) and 
 *      searches the variant list for the instruction, calling the proper 
 *      routine if a match is found. If parseFlag is FALSE, it passes pointers 
 *      to the label and operands to the specified routine for processing.
 *
 *   Usage: processFile()
 *
 *      assemble(char *line, int *errorPtr)
 *
 *      Author: Paul McKee
 *      ECE492    North Carolina State University, 12/13/86
 *
 *      Modified: A.E.Romer, 1991. Version 1.0
 *
 *      Question: What is 'return NORMAL' for? Nothing ever checks it.
 *
 ************************************************************************/


#include <stdio.h>
#include <ctype.h>
#include "asm.h"


#define     SKIPSPACE(ptr)      while (isspace(*ptr)) ptr++
#define     SKIPNONSPACE(ptr)   while (!isspace(*ptr)) ptr++


extern long loc;                        /* The assembler's location counter */
extern char pass2;                           /* Flag set during second pass */
extern char endFlag;      /* Flag set when the END directive is encountered */
extern char continuation;     /* TRUE if the listing line is a continuation
                               * (used by 'listing.c')                      */
extern char noOperand;        /* TRUE if the instruction has no operand.
                               * Used by listLine. */

extern int lineNum;
extern int errorCount, warningCount;

extern char line[LBUF_SIZE];        /* Source line */
extern FILE *inFile;                /* Input file */
extern FILE *listFile;              /* Listing file */
extern char listFlag;



int pickMask(int size, variant *variantPtr, int *errorPtr)
    {
    if (!size || size & variantPtr->sizes)
        if (size & (BYTE | SHORT))
            return variantPtr->bytemask;
        else if (!size || size == WORD)
            return variantPtr->wordmask;
        else
            return variantPtr->longmask;
    NEWERROR(*errorPtr, INV_SIZE_CODE);
    return variantPtr->wordmask;
    }


int strcap(char *d, char *s)
                            /* convert s into capitals in d, except characters
                             * delimited by CHAR_DELIMITER
                             */
    {
    char capFlag;

    capFlag = TRUE;
    while (*s)
        {
        if (capFlag)
            *d = toupper(*s);
        else
            *d = *s;
        if (*s == CHAR_DELIMITER)
            capFlag = (char) !capFlag;
        d++;
        s++;
        }
    *d = '\0';

    return NORMAL;
    }


int processFile(void)
    {
    char capLine[256];
    int error;
    char pass;

    hashInit();                        /* initialize hash table for symbols */

    pass2 = FALSE;
    for (pass = 0; pass < 2; pass++)
        {
        loc = 0;
        lineNum = 1;
        endFlag = FALSE;
        errorCount = warningCount = 0;
        while(!endFlag && fgets(line, LBUF_SIZE, inFile))
                    /* read up to (LBUF_SIZE - 1) characters, append '\0' */
            {
            strcap(capLine, line);              /* convert to capitals,
                                                 * except quoted characters */
            error = OK;
            continuation = FALSE;
            if (pass2 && listFlag)
                listLoc();                  /* insert program counter value
                                             * in the listing line */
            assemble(capLine, &error);
            if (pass2)
                {
                if (error > MINOR)
                    errorCount++;
                else if (error > WARNING)
                    warningCount++;
                if (listFlag)
                    {
                    listLine();             /* complete listing line */
                    printError(listFile, error, -1);
                    }
                printError(stderr, error, lineNum);
                }
            lineNum++;
            }
        if (!pass2)
            {
            pass2 = TRUE;
            puts("Pass 2");
            }
        rewind(inFile);
        }
    return NORMAL;
    }

int assemble(char *line, int *errorPtr)
    {
    instruction *tablePtr;
    variant *variantPtr;
    opDescriptor source, dest;
    char label[SIGCHARS+1], size, v, sourceParsed, destParsed;
    unsigned short mask, i;


    label[0] = '\0';                             /* initialize label buffer */
    noOperand = FALSE;              /* Most instructions have operands. Those
                                     * that do not will have noOperand set to
                                     * TRUE ('zeroOp' in build.c, trapcc.c */

    if (*line == '\n' || *line == '*')
        return NORMAL;              /* ignore empty lines and comment lines */

    if (!isspace(*line))
        if (isalpha(*line) || *line == '_' || *line == '.')
                                    /* valid for first character of a label */
            {
            label[0] = line[0];
            for (i = 1; (!isspace(line[i]) && line[i] != ':'); i++)
                {
                if (i < SIGCHARS)
                    label[i] = line[i];
                if (!isalnum(line[i]) && line[i] != '_')
                          /* not valid for subsequent character of a label */
                    NEWERROR(*errorPtr, INV_LABEL);
                }
            label[i] = '\0';
            line += i;
            if (*line == ':')
                line += 1;                        /* skip label terminator */

/* Create label only if nothing else on line, otherwise the label will be
 * created when the instruction is parsed, to allow for labels defined by
 * directives */

            SKIPSPACE(line);
            if (*line == '*' || *line == '\0')       /* label only on line */
                {
                        /* Move location counter to a word boundary and
                           fix the listing before creating a label */
                if (loc & 1)                    /* if not at word boundary */
                    {
                    loc++;
                    listLoc();
                    }
                create(label, loc, errorPtr);
                return NORMAL;
                }
            }
        else
            {
            NEWERROR(*errorPtr, INV_LABEL);
            SKIPNONSPACE(line);                     /* skip invalid label */
            }

    SKIPSPACE(line);

    if (*line == '\0' || *line == '*')
        return NORMAL;              /* ignore empty lines and comment lines */

/* p now points to the first character of the instruction field */
    line = instLookup(line, &tablePtr, &size, errorPtr);
                                                    /* identify instruction */
    if (*errorPtr > SEVERE)
        return NORMAL;
    SKIPSPACE(line);

/* Case 1: instruction can be processed by 'assemble' */

    if (tablePtr->parseFlag)
        {
                        /* Move location counter to a word boundary and fix
                           the listing before assembling an instruction */
        if (loc & 1)                         /* if not at word boundary */
            {
            loc++;
            listLoc();
            }
        if (*label)
            {
            create(label, loc, errorPtr);
            if (*errorPtr > SEVERE)
                return NORMAL;
            }

        sourceParsed = destParsed = FALSE;        /* nothing parsed yet */
        variantPtr = tablePtr->variantPtr;
        for (v = 0; v < tablePtr->variantCount; v++, variantPtr++)
                                     /* for each variant of the instruction */
            {
/* Parse source only if required for this variant, but not parsed yet */
            if (!sourceParsed && variantPtr->source)
                {
                line = opParse(line, &source, errorPtr);
                if (*errorPtr > SEVERE)
                    return NORMAL;
                sourceParsed = TRUE;
                                    /* ensure source parsed once only */
                }
/* Parse destination only if required for this variant, but not parsed yet */
            if (!destParsed && variantPtr->dest)
                {
                if (*line != ',')
                                    /* missing source/destination separator */
                    {
                    NEWERROR(*errorPtr, SYNTAX);
                    return NORMAL;
                    }
                line = opParse(line+1, &dest, errorPtr);
                if (*errorPtr > SEVERE)
                    return NORMAL;
                if (!isspace(*line) && *line != '\0')
                                        /* if operand field contains
                                         * superfluous characters */
                    {
                    NEWERROR(*errorPtr, SYNTAX);
                    return NORMAL;
                    }
                destParsed = TRUE;
                                    /* ensure destination parsed once only */
                }
/* Source not required, therefore destination not required either */

            if (variantPtr->source == 0)
                {
                mask = pickMask( (int) size, variantPtr, errorPtr);
                                                    /* get size mask */
                (*variantPtr->exec)(mask, (int) size,
                                                    &source, &dest, errorPtr);
                                                        /* build code */
                return NORMAL;
                }

/* Source required, destination not required */

            else if ((source.mode & variantPtr->source)
                                                        && !variantPtr->dest)
                                        /* If source address mode is as
                                         * required for this variant */
                {
                if (!isspace(*line) && *line != '\0')
                                        /* if superfluous characters follow */
                    {
                    NEWERROR(*errorPtr, SYNTAX);
                    return NORMAL;
                    }
                mask = pickMask( (int) size, variantPtr, errorPtr);
                (*variantPtr->exec)(mask, (int) size,
                                                &source, &dest, errorPtr);
                return NORMAL;
                }

/* Both source and destination required */

            else if (source.mode & variantPtr->source
                                && dest.mode & variantPtr->dest)
                                             /* If both are as required for
                                              * this variant */
                {
                mask = pickMask( (int) size, variantPtr, errorPtr);
                (*variantPtr->exec)(mask, (int) size,
                                                    &source, &dest, errorPtr);
                return NORMAL;
                }
            }
        NEWERROR(*errorPtr, INV_ADDR_MODE);
        }

/* Case 2: instruction can't be processed by 'assemble' */

    else
        {
        (*tablePtr->exec)(tablePtr, (int) size, label, line, errorPtr);
        return NORMAL;
        }
     }
