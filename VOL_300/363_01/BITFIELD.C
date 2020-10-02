/***********************************************************************
 *
 *      BITFIELD Routines for 68020 Assembler
 *      Routines for instructions:
 *          BFCHG       BFEXTS      BFFFO       BFSET
 *          BFCLR       BFEXTU      BFINS       BFTST
 *                      
 *  Function: char *bitFieldParse(char *p, opDescriptor *d, int *errorPtr)
 *      Parses the expression of the format {offset:width}.
 *      p       - pointer to the input string,
 *      d       - pointer to the operation descriptor structure,
 *      errorPtr - pointer to the error flag.
 *
 *      Offset and width, together with the offset and width format (data
 *      register or immediate), are returned in the bfXtenWord member of the
 *      opDescriptor struct (d->bfXtenWord),
 *      error status is returned in errorPtr via the standard mechanism.
 *      
 *  Function: int bitField(instruction *tablePtr, int size, char *label,
 *                                                  char *op, int *errorPtr)
 *      Builds BFxxx instructions.
 *      tablePtr - pointer to the instruction table,
 *      size      - not used. Included for compatibility with the opDescriptor
 *                  struct definition.
 *      label    - pointer to the label string, empty string if no label
 *      op       - pointer to the input string,
 *      errorPtr - pointer to the error flag.
 *      The routine returns an error code in *errorPtr using the standard
 *      mechanism. 
 *
 *      Version 1.0
 *      Author: Andrew E. Romer
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

/* extension word fields */
#define     REG_OFFSET      0x800       /* bit 11 set indicates that offset
                                         * is contained in the data register
                                         * specified in bits 12-14 */
#define     OFFSET(x)       (x) << 6    /* offset field */
#define     REG_WIDTH       0x20        /* bit 5 set indicates that width
                                         * is contained in the data register
                                         * specified in bits 0-2. */


#define isRegNum(c) ((c >= '0') && (c <= '7'))


char *bitFieldParse(char *p, opDescriptor *d, int *errorPtr)
    {
    long value;                 /* temporary store */

/* Parse offset */
    if (p[0] == '{')
                            /* dummy comment to enable '}' bracket pairing */
        {
        if (p[1] == 'D' && isRegNum(p[2]) && p[3] == ':')
                        /* Offset is specified in a data register (d0 - d7) */
            {
            d->bfXtenWord |= REG_OFFSET | OFFSET(p[2] - '0');
            p += 3;                 /* point to fields' separator ':' */
            } 
        else
                            /* Offset is an immediate operand */
            {
            p = eval(++p, &value, &d->backRef, errorPtr);
                if (*errorPtr > SEVERE)
                    return NULL;
                else if (p[0] != ':')
                                                    /* missing terminator */
                    {
                    NEWERROR(*errorPtr, SYNTAX);
                    return NULL;
                    }
                else
                    d->bfXtenWord |= OFFSET(value & 0x1f);
            }
        /* parse width */
                            /*  dummy comment to enable bracket pairing '{' */
        if (p[1] == 'D' && isRegNum(p[2]) && p[3] == '}')
                        /* Width is specified in a data register (d0 - d7) */
            {
            d->bfXtenWord |= REG_WIDTH | (p[2] - '0');
            return p+4;     /* point beyond the {offset:width} expression */
            }
        else 
                            /* Width is an immediate operand */
            {
            p = eval(++p, &value, &d->backRef, errorPtr);
                if (*errorPtr > SEVERE)
                    return NULL;
                            /*  dummy comment to enable '{' bracket pairing */
                else if (p[0] != '}')
                                                    /* missing terminator */
                    {
                    NEWERROR(*errorPtr, SYNTAX);
                    return NULL;
                    }
                else
                    d->bfXtenWord |= value & 0x1f;
            return p+1;     /* point beyond the {offset:width} expression */
            }
        }
    }

int bitField(instruction *tablePtr, int size, char *label, char *op,
                                                            int *errorPtr)
    {
    variant *variantPtr;
    opDescriptor source, dest;
    unsigned short mask, i;


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
    source.bfXtenWord = 0;
    dest.bfXtenWord = 0;

/* Parse source */
    op = opParse(op, &source, errorPtr);
    if (*errorPtr > SEVERE)
        return NORMAL;

/* Verify source address mode */
    if ((source.mode & variantPtr->source) == 0)
        {
        NEWERROR(*errorPtr, INV_ADDR_MODE);
        return NULL;
        }
/* Parse source */
    if ((variantPtr->source == (DnDirect | CtrAlt))
                        || (variantPtr->source == (DnDirect | Control)))
        {
        op = bitFieldParse(op, &source, errorPtr);
        if(*errorPtr > SEVERE)
            return NORMAL;
        }
    else                                /* variantPtr->source == DnDirect */
        dest.bfXtenWord |= source.reg << 12;

/* Parse destination if required */
    if (variantPtr->dest)
        {
        if (*op != ',')
            {
            NEWERROR(*errorPtr, SYNTAX);
            return NORMAL;
            }
        op = opParse(op+1, &dest, errorPtr);
        if (*errorPtr > SEVERE)
            return NORMAL;

/* Verify address mode */
        if ((dest.mode & variantPtr->dest) == 0)
            {
            NEWERROR(*errorPtr, INV_ADDR_MODE);
            return NULL;
            }
        else                            /* variantPtr->dest == DnDirect */
            source.bfXtenWord |= dest.reg << 12;

        if (variantPtr->dest == (DnDirect | Control))
            {
            op = bitFieldParse(op, &dest, errorPtr);
            if(*errorPtr > SEVERE)
                return NORMAL;
            }
        }

    if (!isspace(*op) && *op != '\0')       /* if operand field contains
                                             * superfluous characters */
        {
        NEWERROR(*errorPtr, SYNTAX);
        return NORMAL;
        }
    mask = variantPtr -> wordmask;

/* Generate the instruction */
    if ((variantPtr->source == (DnDirect | CtrAlt))
                    || (variantPtr->source == (DnDirect | Control)))
        {
        if (pass2)
            output( (long) (mask | effAddr(&source)), WORD);
        loc += 2;
        if (pass2)
            output(source.bfXtenWord, WORD);
        loc += 2;
        extWords(&source, WORD, errorPtr);
        if (variantPtr->dest)
            extWords(&dest, WORD, errorPtr); 
        return NORMAL;
        }
    else                    /* wariantPtr->source must be DnDirect and
                             * variantPtr->dest - DnDirect or Control */
        {
        if (pass2)
            output( (long) (mask | effAddr(&dest)), WORD);
        loc += 2;
        if (pass2)
            output(dest.bfXtenWord, WORD);
        loc += 2;
        extWords(&source, WORD, errorPtr);
        extWords(&dest, WORD, errorPtr);
        return NORMAL;
        }
    return NORMAL;
    }
