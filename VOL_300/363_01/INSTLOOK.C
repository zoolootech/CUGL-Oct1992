/***********************************************************************
 *
 *      INSTLOOKUP.C
 *      Instruction Table Lookup Routine for 68020 Assembler
 *
 *    Function: instLookup()
 *      Parses an instruction and looks it up in the instruction table. The 
 *      input to the function is a pointer to the instruction on a line of 
 *      assembly code. The routine scans the instruction and notes the size 
 *      code if present. It then (binary) searches the instruction table for 
 *      the specified opcode. If it finds the opcode, it returns a pointer to 
 *      the instruction table entry for that instruction (via the instPtrPtr 
 *      argument) as well as the size code or 0 if no size was specified (via 
 *      the sizePtr argument). If the opcode is not in the instruction table, 
 *      then the routine returns INV_OPCODE.
 *
 *      (which is not used - A.E.Romer)
 *
 *      The routine returns an error value via the standard mechanism. 
 *
 *   Usage: char *instLookup(char *p, instruction *(*instPtrPtr),
 *                                          char *sizePtr, int *errorPtr)
 *
 *  Errors: SYNTAX
 *          INV_OPCODE
 *          INV_SIZE_CODE
 *
 *      Author: Paul McKee
 *      ECE492    North Carolina State University,  9/24/86
 *
 *      Modified A.E. Romer. Version 1.0
 *          16 March 1991   ANSI functions, braces layout.
 *          29 May   1991   opcode string storage changed to allow numeric
 *                          characters (for CAS2 etc.) 
 *
 ************************************************************************/


#include <stdio.h>
#include <ctype.h>
#include "asm.h"

#define     INST_CHARS      8           /* size of instruction string store */

extern instruction instTable[];
extern int tableSize;


char *instLookup(char *p, instruction *(*instPtrPtr), char *sizePtr,
                                                            int *errorPtr)
    {
    char opcode[INST_CHARS];
    int i, hi, lo, mid, cmp;

/*  printf("InstLookup: Input string is \"%s\"\n", p); */

    i = 0;
    do
        {
        if (i < INST_CHARS - 1)
            opcode[i++] = *p;
        p++;
        } while (isalnum(*p));  /* store opcode string in opcode[] */

    opcode[i] = '\0';
    if (*p == '.')
        if (isspace(p[2]) || p[2] == '\0')     /* end of instruction string */
            {
            if (p[1] == 'B')
                *sizePtr = BYTE;
            else if (p[1] == 'W')
                *sizePtr = WORD;
            else if (p[1] == 'L')
                *sizePtr = LONG;
            else if (p[1] == 'S')
                *sizePtr = SHORT;
            else
                {
                *sizePtr = 0;
                NEWERROR(*errorPtr, INV_SIZE_CODE);
                }
            p += 2;
            }
        else
            {
            NEWERROR(*errorPtr, SYNTAX);
            return NULL;
            }
    else if (!isspace(*p) && *p != '\0')
        {
        NEWERROR(*errorPtr, SYNTAX);
        return NULL;
        }
    else
        *sizePtr = 0;

    lo = 0;
    hi = tableSize - 1;
    do
        {
        mid = (hi + lo) / 2;
        cmp = strcmp(opcode, instTable[mid].mnemonic);
        if (cmp > 0)
            lo = mid + 1;
        else if (cmp < 0)
            hi = mid - 1;
        } while (cmp && (hi >= lo));
    if (cmp == 0)                       /* match */
        {
        *instPtrPtr = &instTable[mid];
        return p;
        }
    else
        {
        NEWERROR(*errorPtr, INV_OPCODE);
        return NULL;
        }

    return NORMAL;                /* can the program ever reach this point? */
    }

