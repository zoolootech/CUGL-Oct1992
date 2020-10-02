/***********************************************************************
 *
 *      MISCEL - Routines for 68020 Assembler
 *      Routines for instructions:
 *          BKPT, CALLM,
 *
 *  All functions accept the same parameters:
 *
 *      tablePtr - pointer to the instruction table,
 *      size      - not used. Included for compatibility with the opDescriptor
 *                  struct definition.
 *      label    - pointer to the label string, empty string if no label
 *      op       - pointer to the input string,
 *      errorPtr - pointer to the error flag.
 *
 *   and return integer flag.
 *
 *   The functions return an error code in *errorPtr using the standard
 *   mechanism.
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
#include <string.h>
#include "asm.h"

extern long loc;
extern char pass2;

/***********************************************************************
 *
 *  Function bkPoint builds the instruction
 *   BKPT #<data>
 *
 ***********************************************************************/

#define     BKPT_MASK       0x4848

int bkPoint(instruction *tablePtr, int size, char *label, char *op,
                                                            int *errorPtr)
    {
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

    if (size)
        NEWERROR(*errorPtr, UNSIZED);

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
        {
        output( (long) (BKPT_MASK | (source.data & 0x7)), WORD);
        if (source.data < 0 || source.data > 7)
            NEWERROR(*errorPtr, INV_3_BIT_DATA);
        }
    loc += 2;
    }

/***********************************************************************
 *
 *  Function callModule builds the instruction
 *   CALLM #<data>,<ea>
 *
 ***********************************************************************/

#define     CALLM_MASK      0x06c0

int callModule(instruction *tablePtr, int size, char *label, char *op,
                                                            int *errorPtr)
    {
    opDescriptor source, dest;

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

    if (size)
        NEWERROR(*errorPtr, UNSIZED);

    op = opParse(op, &source, errorPtr);                /* parse source */
    if (*errorPtr > SEVERE)
        return NORMAL;
    if (source.mode != Immediate)
        {
        NEWERROR(*errorPtr, INV_ADDR_MODE);
        return NULL;
        }
    if (*op != ',')             /* missing source/destination separator */
        {
        NEWERROR(*errorPtr, SYNTAX);
        return NORMAL;
        }
    op = opParse(op+1, &dest, errorPtr);
    if (*errorPtr > SEVERE)
        return NORMAL;
    if ((dest.mode & Control) == 0)
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
        output( (long) (CALLM_MASK | effAddr(&dest)), WORD);
    loc += 2;
    if (pass2)
        {
        output( (long) (source.data & 0xff), WORD);

        if (source.data < 0 || source.data > 255)
            NEWERROR(*errorPtr, INV_8_BIT_DATA);
        }
    loc += 2;
    extWords(&dest, size, errorPtr);
    return NORMAL;
    }

/***********************************************************************
 *
 *  Function comSwap builds the instruction
 *   CAS Dc,Du,<ea>
 *
 ***********************************************************************/

#define     isRegNum(c)     ((c >= '0') && (c <= '7'))
#define     CAS_BYTEMASK    0x0ac0
#define     CAS_WORDMASK    0x0cc0
#define     CAS_LONGMASK    0x0ec0

int compSwap(instruction *tablePtr, int size, char *label, char *op,
                                                            int *errorPtr)
    {
    opDescriptor source;
    char regc, regu;

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
/* Parse Dc and Du */
    if (op[0] == 'D' && isRegNum(op[1]) && op[2] == ',' && op[3] == 'D'
                    && isRegNum(op[4]) && op[5] == ',')
        {
        regc = op[1] - '0';
        regu = op[4] - '0';
        }
    else
        {
        NEWERROR(*errorPtr, SYNTAX);
        return NORMAL;
        }

/* Parse effective address */
    op = opParse(op+6, &source, errorPtr);
    if (*errorPtr > SEVERE)
        return NORMAL;
    if ((source.mode & MemAlt) == 0)
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
/* generate output */
    if (pass2)
        switch (size)
            {
            case BYTE:
                output( (long) (CAS_BYTEMASK | effAddr(&source)), WORD);
                break;
            case LONG:
                output( (long) (CAS_LONGMASK | effAddr(&source)), WORD);
                break;
            default:        /* size == WORD || size == 0 */
                output( (long) (CAS_WORDMASK | effAddr(&source)), WORD);
                break;
            }
    loc += 2;
    if (pass2)
        output( (long) ((regu << 6) | regc), WORD);
    loc += 2;
    extWords(&source, size, errorPtr);
    return NORMAL;
    }

/***********************************************************************
 *
 *  Function comSwap2 builds the instruction
 *   CAS2 Dc1:Dc2,Du1:Du2,(Rn1):(Rn2)
 *
 ***********************************************************************/

#define     CAS2_WORDMASK   0x0cfc
#define     CAS2_LONGMASK   0x0efc

int compSwap2(instruction *tablePtr, int size, char *label, char *op,
                                                            int *errorPtr)
    {
    opDescriptor source, dest;
    char regc1, regc2, regu1, regu2, regn1, regn2;

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

/* Parse operand string "Dc1:Dc2,Du1:Du2,(Rn1):(Rn2)" */

    if  (    op[0] == 'D' && isRegNum(op[1]) && op[2] == ':'        /* Dc1: */
              && op[3] == 'D' && isRegNum(op[4]) && op[5] == ','    /* Dc2, */
              && op[6] == 'D' && isRegNum(op[7]) && op[8] == ':'    /* Du1: */
              && op[9] == 'D' && isRegNum(op[10]) && op[11] == ','  /* Du2, */
              && op[12] == '(' && (op[13] == 'D' || op[13] == 'A')  /* (R1 */
              && isRegNum(op[14]) && op[15] == ')'&& op[16] == ':'  /* n): */
              && op[17] == '(' && (op[18] == 'D' || op[18] == 'A')  /* (R2 */
              && isRegNum(op[19]) && op[20] == ')' )                /* n) */
        {
        regc1 = op[1] - '0';
        regc2 = op[4] - '0';
        regu1 = op[7] - '0';
        regu2 = op[10] - '0';
        regn1 = op[14] - '0';
        regn2 = op[19] - '0';
        if (op[13] == 'A')
            regn1 |= 8;
        if (op[18] == 'A')
            regn2 |= 8;
        }
    else
        {
        NEWERROR(*errorPtr, SYNTAX);
        return NORMAL;
        }

    if (!isspace(op[21]) && op[21] != '\0')
                            /* if operand field contains
                             * superfluous characters */
        {
        NEWERROR(*errorPtr, SYNTAX);
        return NORMAL;
        }

    if (pass2)
        switch (size)
            {
            case WORD:
            case 0:
                output( (long) CAS2_WORDMASK, WORD);
                break;
            case LONG:
                output( (long) CAS2_LONGMASK, WORD);
                break;
            default:
                output ( (long) CAS2_WORDMASK, WORD);
                NEWERROR(*errorPtr, IGNORED_SIZE);
            }
        loc += 2;
        if (pass2)
            output( (long) (regc1 | (regu1 << 6 ) | (regn1 << 12)), WORD);
        loc += 2;
        if (pass2)
            output( (long) (regc2 | (regu2 << 6 ) | (regn2 << 12)), WORD);
        loc += 2;

        return NORMAL;
    }

/***********************************************************************
 *
 *  Function checkReg builds the instruction
 *   CHK2 <ea>,Rn
 *
 ***********************************************************************/

#define     CHK2_BYTEMASK   0x00c0
#define     CHK2_WORDMASK   0x02c0
#define     CHK2_LONGMASK   0x04c0

int checkReg(instruction *tablePtr, int size, char *label, char *op,
                                                            int *errorPtr)

    {
    opDescriptor source, dest;

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

/* Parse source effective address */
    op = opParse(op, &source, errorPtr);
    if (*errorPtr > SEVERE)
        return NORMAL;
    if ((source.mode & Control) == 0)
        {
        NEWERROR(*errorPtr, INV_ADDR_MODE);
        return NULL;
        }

    if (*op != ',')
                    /* missing source/destination separator */
    {
    NEWERROR(*errorPtr, SYNTAX);
    return NORMAL;
    }

/* Parse dest effective address */
    op = opParse(op+1, &dest, errorPtr);
    if (*errorPtr > SEVERE)
        return NORMAL;
    if ((dest.mode & GenReg) == 0)
        {
        NEWERROR(*errorPtr, INV_ADDR_MODE);
        return NULL;
        }
    if (dest.mode == AnDirect)
        dest.reg |= 8;

    if (!isspace(*op) && *op != '\0')
                            /* if operand field contains
                             * superfluous characters */
        {
        NEWERROR(*errorPtr, SYNTAX);
        return NORMAL;
        }

/* generate output */
    if (pass2)
        switch (size)
            {
            case BYTE:
                output( (long) (CHK2_BYTEMASK | effAddr(&source)), WORD);
                break;
            case LONG:
                output( (long) (CHK2_LONGMASK | effAddr(&source)), WORD);
                break;
            default:        /* size == WORD || size == 0 */
                output( (long) (CHK2_WORDMASK | effAddr(&source)), WORD);
                break;
            }
    loc += 2;
    if (pass2)
        if (strcmp(tablePtr->mnemonic, "CHK2") == 0)
            output( (long) ((dest.reg << 12) | 0x800), WORD);
        else                /* (strcmp(tablePtr->mnemonic, "CMP2") == 0) */
            output( (long) (dest.reg << 12), WORD);
    loc += 2;
    extWords(&source, size, errorPtr);
    return NORMAL;
    }

/***********************************************************************
 *
 *  Functions divsop, divslop, divuop, divulop and divide build the
 *  following instructions:
 *   DIVS <ea>,Dn
 *   DIVS <ea>,Dq
 *   DIVS <ea>,Dr:Dq
 *   DIVSL <ea>,Dr,Dq
 *   DIVU <ea>,Dn
 *   DIVU <ea>,Dq
 *   DIVU <ea>,Dr:Dq
 *   DIVUL <ea>,Dr:Dq
 *
 ***********************************************************************/

#define     DIVS_W          1
#define     DIVS_L          2
#define     DIVS_2L         3
#define     DIVSL_2L        4
#define     DIVU_W          5
#define     DIVU_L          6
#define     DIVU_2L         7
#define     DIVUL_2L        8

#define     DIVS_W_MASK             0x81c0
#define     DIVS_L_MASK             0x4c40
#define     DIVS_L_64BIT_XMASK      0x0c00
#define     DIVS_L_32BIT_XMASK      0x0800

#define     DIVU_W_MASK             0x80c0
#define     DIVU_L_MASK             0x4c40
                                     /* same as DIVS_L_MASK, for all LONG */
#define     DIVU_L_64BIT_XMASK      0x0400
#define     DIVU_L_32BIT_XMASK      0x0000
                                    /* unnecessary, included for symmetry */

int divide(int inst_flag, int size, char *label, opDescriptor source,
                                                    char *op, int *errorPtr)
    {

/* op points to the first character of the destination part of the operand
 * field */

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
    switch (inst_flag)
        {
        case DIVS_W:
            if (op[0] == ',' && op[1] == 'D' && isRegNum(op[2]))
                if (!isspace(op[3]) && op[3] != '\0')
                                                /* if operand field contains
                                                 * superfluous characters */
                    {
                    NEWERROR(*errorPtr, SYNTAX);
                    return NORMAL;
                    }
                else
                    {
                    if (pass2)
                        output( (long) (DIVS_W_MASK | effAddr(&source)
                                | ((op[2] - '0') << 9)), WORD);
                    loc += 2;
                    extWords(&source, size, errorPtr);
                    return NORMAL;
                    }
            else
                {
                NEWERROR(*errorPtr, SYNTAX);
                return NORMAL;
                }

        case DIVS_L:
            if (op[0] == ',' && op[1] == 'D' && isRegNum(op[2]))
                if (!isspace(op[3]) && op[3] != '\0')
                                                /* if operand field contains
                                                 * superfluous characters */
                    {
                    NEWERROR(*errorPtr, SYNTAX);
                    return NORMAL;
                    }
                else
                    {
                    if (pass2)
                        output( (long) (DIVS_L_MASK | effAddr(&source)), WORD);
                    loc += 2;
                    if (pass2)
                        output( (long) (DIVS_L_32BIT_XMASK |
                            ((op[2] - '0') << 12) | (op[2] - '0')),WORD);
                    loc += 2;
                    extWords(&source, size, errorPtr);
                    return NORMAL;
                    }
            else
                {
                NEWERROR(*errorPtr, SYNTAX);
                return NORMAL;
                }

        case DIVS_2L:
            if (op[0] == ',' && op[1] == 'D' && isRegNum(op[2])
                        && op[3] == ':' && op[4] == 'D' && isRegNum(op[5]))
                if (!isspace(op[6]) && op[6] != '\0')
                                                /* if operand field contains
                                                 * superfluous characters */
                    {
                    NEWERROR(*errorPtr, SYNTAX);
                    return NORMAL;
                    }
                else
                    {
                    if (pass2)
                        output( (long) (DIVS_L_MASK | effAddr(&source)), WORD);
                    loc += 2;
                    if (pass2)
                        output( (long) (DIVS_L_64BIT_XMASK |
                            ((op[5] - '0') << 12) | (op[2] - '0')), WORD);
                    loc += 2;
                    extWords(&source, size, errorPtr);
                    return NORMAL;
                    }

        case DIVSL_2L:
            if (op[0] == ',' && op[1] == 'D' && isRegNum(op[2])
                        && op[3] == ':' && op[4] == 'D' && isRegNum(op[5]))
                if (!isspace(op[6]) && op[6] != '\0')
                                                /* if operand field contains
                                                 * superfluous characters */
                    {
                    NEWERROR(*errorPtr, SYNTAX);
                    return NORMAL;
                    }
                else
                    {
                    if (pass2)
                        output( (long) (DIVS_L_MASK | effAddr(&source)), WORD);
                    loc += 2;
                    if (pass2)
                        output( (long) (DIVS_L_32BIT_XMASK |
                            ((op[5] - '0') << 12) | (op[2] - '0')), WORD);
                    loc += 2;
                    extWords(&source, size, errorPtr);
                    return NORMAL;
                    }

        case DIVU_W:
            if (op[0] == ',' && op[1] == 'D' && isRegNum(op[2]))
                if (!isspace(op[3]) && op[3] != '\0')
                                                /* if operand field contains
                                                 * superfluous characters */
                    {
                    NEWERROR(*errorPtr, SYNTAX);
                    return NORMAL;
                    }
                else
                    {
                    if (pass2)
                        output( (long) (DIVU_W_MASK | effAddr(&source)
                                | ((op[2] - '0') << 9)), WORD);
                    loc += 2;
                    extWords(&source, size, errorPtr);
                    return NORMAL;
                    }
            else
                {
                NEWERROR(*errorPtr, SYNTAX);
                return NORMAL;
                }

        case DIVU_L:
            if (op[0] == ',' && op[1] == 'D' && isRegNum(op[2]))
                if (!isspace(op[3]) && op[3] != '\0')
                                                /* if operand field contains
                                                 * superfluous characters */
                    {
                    NEWERROR(*errorPtr, SYNTAX);
                    return NORMAL;
                    }
                else
                    {
                    if (pass2)
                        output( (long) (DIVU_L_MASK | effAddr(&source)), WORD);
                    loc += 2;
                    if (pass2)
                        output( (long) (DIVU_L_32BIT_XMASK |
                            ((op[2] - '0') << 12) | (op[2] - '0')), WORD);
                    loc += 2;
                    extWords(&source, size, errorPtr);
                    return NORMAL;
                    }
            else
                {
                NEWERROR(*errorPtr, SYNTAX);
                return NORMAL;
                }

        case DIVU_2L:
            if (op[0] == ',' && op[1] == 'D' && isRegNum(op[2])
                        && op[3] == ':' && op[4] == 'D' && isRegNum(op[5]))
                if (!isspace(op[6]) && op[6] != '\0')
                                                /* if operand field contains
                                                 * superfluous characters */
                    {
                    NEWERROR(*errorPtr, SYNTAX);
                    return NORMAL;
                    }
                else
                    {
                    if (pass2)
                        output( (long) (DIVU_L_MASK | effAddr(&source)), WORD);
                    loc += 2;
                    if (pass2)
                        output( (long) (DIVU_L_64BIT_XMASK |
                            ((op[5] - '0') << 12) | (op[2] - '0')), WORD);
                    loc += 2;
                    extWords(&source, size, errorPtr);
                    return NORMAL;
                    }

        case DIVUL_2L:
            if (op[0] == ',' && op[1] == 'D' && isRegNum(op[2])
                        && op[3] == ':' && op[4] == 'D' && isRegNum(op[5]))
                if (!isspace(op[6]) && op[6] != '\0')
                                                /* if operand field contains
                                                 * superfluous characters */
                    {
                    NEWERROR(*errorPtr, SYNTAX);
                    return NORMAL;
                    }
                else
                    {
                    if (pass2)
                        output( (long) (DIVU_L_MASK | effAddr(&source)), WORD);
                    loc += 2;
                    if (pass2)
                        output( (long) (DIVU_L_32BIT_XMASK |
                            ((op[5] - '0') << 12) | (op[2] - '0')), WORD);
                    loc += 2;
                    extWords(&source, size, errorPtr);
                    return NORMAL;
                    }

        default:
            {
            NEWERROR(*errorPtr, SYNTAX);
            return NORMAL;
            }
        }
/*  return NORMAL; */               /* superfluous, safety measure */
    }

int divsop(instruction *tablePtr, int size, char *label, char *op,
                                                            int *errorPtr)
    {
    opDescriptor source;

/* Parse source effective address */
    op = opParse(op, &source, errorPtr);
    if (*errorPtr > SEVERE)
        return NORMAL;

    if ((source.mode & Data) == 0)
        {
        NEWERROR(*errorPtr, INV_ADDR_MODE);
        return NORMAL;
        }

    if (!size || size == WORD)      /* accept unsized default */
        return divide(DIVS_W, size, label, source, op, errorPtr);
    else if (size == LONG)
        {

/* Establish destination format: either Dq, or Dr:Dq */
        if (op[0] == ',' && op[3] == ':')           /* Dr:Dq */
            return divide(DIVS_2L, size, label, source, op, errorPtr);
        else                                        /* Dq */
            return divide(DIVS_L, size, label, source, op, errorPtr);
        }
    else
        {
        NEWERROR(*errorPtr, SYNTAX);
        return NORMAL;
        }
    }

int divslop(instruction *tablePtr, int size, char *label, char *op,
                                                            int *errorPtr)
    {
    opDescriptor source;

/* Parse source effective address */
    op = opParse(op, &source, errorPtr);
    if (*errorPtr > SEVERE)
        return NORMAL;

    if ((source.mode & Data) == 0)
        {
        NEWERROR(*errorPtr, INV_ADDR_MODE);
        return NORMAL;
        }

    return divide(DIVSL_2L, size, label, source, op, errorPtr);
    }

int divuop(instruction *tablePtr, int size, char *label, char *op,
                                                            int *errorPtr)
    {
    opDescriptor source;

/* Parse source effective address */
    op = opParse(op, &source, errorPtr);
    if (*errorPtr > SEVERE)
        return NORMAL;

    if ((source.mode & Data) == 0)
        {
        NEWERROR(*errorPtr, INV_ADDR_MODE);
        return NORMAL;
        }

    if (!size || size == WORD)      /* accept unsized default */
        return divide(DIVU_W, size, label, source, op, errorPtr);
    else if (size == LONG)
        {

/* Establish destination format: either Dq, or Dr:Dq */
        if (op[0] == ',' && op[3] == ':')           /* Dr:Dq */
            return divide(DIVU_2L, size, label, source, op, errorPtr);
        else                                        /* Dq */
            return divide(DIVU_L, size, label, source, op, errorPtr);
        }
    else
        {
        NEWERROR(*errorPtr, SYNTAX);
        return NORMAL;
        }
    }

int divulop(instruction *tablePtr, int size, char *label, char *op,
                                                            int *errorPtr)
    {
    opDescriptor source;

/* Parse source effective address */
    op = opParse(op, &source, errorPtr);
  if (*errorPtr > SEVERE)
        return NORMAL;

    if ((source.mode & Data) == 0)
        {
        NEWERROR(*errorPtr, INV_ADDR_MODE);
        return NORMAL;
        }

    return divide(DIVUL_2L, size, label, source, op, errorPtr);
    }

  /***********************************************************************
 *
 *  Function rtm builds the instruction
 *   RTM Rn
 *
 ***********************************************************************/

#define     RTMMASK    0x06c0

int rtm(instruction *tablePtr, int size, char *label, char *op,
                                                            int *errorPtr)
    {
    int reg;

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

    if (size)
        NEWERROR(*errorPtr, IGNORED_SIZE);

    if ((op[0] == 'D' || op[0] == 'A') && isRegNum(op[1]))
        if (!isspace(op[2]) && op[2] != '\0')
                                /* if operand field contains
                                 * superfluous characters */
            {
            NEWERROR(*errorPtr, SYNTAX);
            return NORMAL;
            }
        else
            {
            reg = op[1] - '0';
            if (op[0] == 'A')
                reg |= 8;               /* set the D/A bit */
            if (pass2)
                output( (long) (RTMMASK | reg), WORD);
            loc += 2;
            }
    }


  /***********************************************************************
 *
 *  Function pck builds the instructions
 *   PACK -(Ax),-(Ay),#<adjustment>
 *   PACK Dx,Dy,#<adjustment>
 *   UNPK -(Ax),-(Ay),#<adjustment>
 *   UNPK Dx,Dy,#<adjustment>
 *
 ***********************************************************************/

int pck(int mask, char *label, char *op, int *errorPtr)
    {
    long value;
    char backRef;

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

    if (op[0] == '-' && op[1] == '(' && op[2] == 'A' && isRegNum(op[3])
            && op[4] == ')' && op[5] == ','
            && op[6] == '-' && op[7] == '(' && op[8] == 'A' && isRegNum(op[9])
            && op[10] == ')' && op[11] == ',' && op[12] == '#')
        {
        mask |= 8 | (op[3] - '0') | ((op[9] - '0') << 9);
        op += 13;
        }
    else if (op[0] == 'D' && isRegNum(op[1]) && op[2] == ','
            && op[3] == 'D' && isRegNum(op[4]) && op[5] == ',' && op[6] == '#')
        {
        mask |= (op[1] - '0') | ((op[4] - '0') << 9);
        op += 7;
        }
    else
        {
        NEWERROR(*errorPtr, SYNTAX);
        return NORMAL;
        }
    op = eval(op, &value, &backRef, errorPtr);
    if (*errorPtr > SEVERE)
        return NORMAL;
    if (pass2)
        output( (long) mask, WORD);
    loc += 2;
    if (pass2)
        output( (long) (value & 0xFFFF), WORD);
    loc += 2;
    if (value < -32768 || value > 32767)
        NEWERROR(*errorPtr, INV_16_BIT_DATA);
    return NORMAL;
    }

#define     PACKMASK    0x8140
#define     UNPKMASK    0x8180

int pack(instruction *tablePtr, int size, char *label, char *op,
                                                            int *errorPtr)
    {
    if (size)
        NEWERROR(*errorPtr, IGNORED_SIZE);
    return pck(PACKMASK, label, op, errorPtr);
    }


int unpack(instruction *tablePtr, int size, char *label, char *op,
                                                            int *errorPtr)
    {
    if (size)
        NEWERROR(*errorPtr, IGNORED_SIZE);
    return pck(UNPKMASK, label, op, errorPtr);
    }
