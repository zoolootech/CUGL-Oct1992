/***********************************************************************
 *
 *
 *      TRAPCC - Routine for 68020 Assembler
 *          Routine for instructions:
 *           TRAPCC (TRAPHS)   TRAPGT    TRAPLT      TRAPRA
 *           TRAPCS (TRAPLO)   TRAPHI    TRAPMI      TRAPSR
 *           TRAPEQ            TRAPLE    TRAPNE      TRAPVC
 *           TRAPGE            TRAPLS    TRAPPL      TRAPVS
 *
 *  int trapcc(instruction *tablePtr, int size, char *label, char *op,
 *                                                          int *errorPtr)
 *
 *      tablePtr - pointer to the instruction table,
 *      size     - 0 indicates no immediate operand, WORD/LONG valid sizes
 *                 of the immediate operand,
 *      label    - pointer to the label string, empty string if no label
 *      op       - pointer to the input string,
 *      errorPtr - pointer to the error flag.
 *
 *      The routine returns an error code in *errorPtr using the standard
 *      mechanism.
 *
 *
 *      Author: Andrew E. Romer. Version 1.0
 *      38 Bolsover Road, Worthing, West Sussex, England BN13 1NT.
 *
 *      Date: May 1991
 *
 ***********************************************************************/


#include <stdio.h>
#include <ctype.h>
#include "asm.h"

extern long loc;
extern char pass2;
extern char noOperand;      /* TRUE if the instruction has no operand.
                             * Used by listLine. */

int trapcc(instruction *tablePtr, int size, char *label, char *op,
                                                            int *errorPtr)
    {
    variant *variantPtr;
    opDescriptor source;

/* op points to the first character of the operand field */

/* Move location counter to a word boundary and fix the listing before
 * assembling the instruction */

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

    variantPtr = tablePtr->variantPtr;

    switch (size)
        {
        case 0:                 /* no immediate operand */
            if (pass2)
                {
                output( (long) variantPtr->bytemask, WORD);
                noOperand = TRUE;
                }
            loc += 2;
            break;
        case WORD:                      /* 16-bit immediate operand */
            op = opParse(op, &source, errorPtr);
            if (*errorPtr > SEVERE)
                return NORMAL;
            if (source.mode != Immediate)
                {
                NEWERROR(*errorPtr, INV_ADDR_MODE);
                return NULL;
                }
            if (!isspace(*op) && *op != '\0')
                                    /* if operand field contains
                                     * superfluous characters */
                {
                NEWERROR(*errorPtr, SYNTAX);
                return NORMAL;
                }
            if (pass2)
                output( (long) variantPtr->wordmask, WORD);
            loc += 2;
            if (pass2)
                {
                output( (long) (source.data & 0xffff), WORD);
                if (source.data < -32768 || source.data > 32767)
                    NEWERROR(*errorPtr, INV_16_BIT_DATA);
                }
            loc += 2;
            break;
        case LONG:                      /* 32-bit immediate operand */
            op = opParse(op, &source, errorPtr);
            if (*errorPtr > SEVERE)
                return NORMAL;
            if (source.mode != Immediate)
                {
                NEWERROR(*errorPtr, INV_ADDR_MODE);
                return NULL;
                }
            if (!isspace(*op) && *op != '\0')
                                    /* if operand field contains
                                     * superfluous characters */
                {
                NEWERROR(*errorPtr, SYNTAX);
                return NORMAL;
                }
            if (pass2)
                output( (long) variantPtr->longmask, WORD);
            loc += 2;
            if (pass2)
                {
                output( (long) source.data, LONG);
                if (source.data >= -32768 && source.data <= 32767)
                    NEWERROR(*errorPtr, EXCESSIVE_SIZE);
                }
            loc += 4;
            break;
        default:
            NEWERROR(*errorPtr, INV_SIZE_CODE);
            break;
        }
    return NORMAL;
    }



