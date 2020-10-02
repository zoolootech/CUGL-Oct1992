/***********************************************************************
 *
 *      CODEGEN.C
 *      Code Generation Routines for 68020 Assembler
 *
 *    Function: output()
 *      Places the data whose size and value are specified onto the output 
 *      stream at the current location contained in global variable loc. That 
 *      is, if a listing is being produced, it calls listObj() to print the 
 *      data in the object code field of the current listing line; if an
 *      object file is being produced, it calls outputObj() tooutput the data 
 *      in the form of S-records. 
 *
 *      effAddr()
 *      Computes the 6-bit effective address code used by the 68020 in most 
 *      cases to specify address modes. This code is returned as the value of 
 *      effAddr(). The desired addressing mode is determined by the field of 
 *      the opDescriptor which is pointed to by the operand argument. The 
 *      lower 3 bits of the output contain the register code and the upper 
 *      3 bits the mode code. 
 *
 *      extWords()
 *      Computes and outputs (using output()) the extension words for the 
 *      specified operand. The generated extension words are determined from 
 *      the data contained in the opDescriptor pointed to by the op argument 
 *      and from the size code of the instruction, passed in the size argument.
 *      The errorPtr argument is used to return an error code by the standard 
 *      mechanism. 
 *
 *   Usage: output(int data, int size)
 *
 *      effAddr(opDescriptor *operand)
 *
 *      extWords(opDescriptor *op, int size, int *errorPtr)
 *
 *      Author: Paul McKee
 *      ECE492    North Carolina State University, 12/13/86
 *
 *  Modified A.E. Romer. Version 1.0
 *      17 March 1991:  ANSI functions, braces layout.
 *      8 May 1991:     68020-specific effective address modes added to
 *                      effAddr().
 *      9 May 1991:     68020-specific extension words.
 *
 ************************************************************************/


#include <stdio.h>
#include "asm.h"

extern long loc;
extern char pass2;
extern FILE *listFile;

extern char listFlag;                       /* True if a listing is desired */
extern char objFlag;              /* True if an object code file is desired */


int output(long data, int size)
    {

    if (listFlag)
        listObj(data, size);
    if (objFlag)
        outputObj(loc, data, size);

    return NORMAL;
    }


int effAddr(opDescriptor *operand)
    {
    switch (operand->mode)
        {
        case DnDirect:          return 0x00 | operand->reg;
        case AnDirect:          return 0x08 | operand->reg;
        case AnInd:             return 0x10 | operand->reg;
        case AnIndPost:         return 0x18 | operand->reg;
        case AnIndPre:          return 0x20 | operand->reg;
        case AnIndDisp:         return 0x28 | operand->reg;
        case AnIndIndex:
        case MemIndPostinx:
        case MemIndPreinx:      return 0x30 | operand->reg;
        case AbsShort:          return 0x38;
        case AbsLong:           return 0x39;
        case PCDisp:            return 0x3A;
        case PCIndex:
        case PCMemIndPostinx:
        case PCMemIndPreinx:    return 0x3B;
        case Immediate:         return 0x3C;

        default:    printf("INVALID EFFECTIVE ADDRESSING MODE!\n");
                    exit (0);
        }
    }

int extWords(opDescriptor *op, int size, int *errorPtr)

/* The anding of data with 0xFFFF or 0xFF below is supefluous, I think.
 * I believe it is done again in output() */

    {
    long disp;
    
    switch (op->mode)
        {
        case DnDirect:          /* Dn */
        case AnDirect:          /* An or SP */
        case AnInd:             /* (An) or (SP) */
        case AnIndPost:         /* (An)+ or (SP)+ */
        case AnIndPre:          /* -(An) or -(SP) */
            break;                      /* no extension words required */

        case AnIndDisp:         /* (<data>,An) */
                        if (pass2)
                            {
                            disp = op->data;
                            output(disp & 0xFFFF, WORD);
                            if (disp < -32768 || disp > 32767)
                                NEWERROR(*errorPtr, INV_DISP);
                            }
                        loc += 2;
                        break;

        case PCDisp:            /* (<data>,PC) */
                        if (pass2)
                            {
                            disp = op->data - loc;
                            output(disp & 0xFFFF, WORD);
                            if (disp < -32768 || disp > 32767)
                                NEWERROR(*errorPtr, INV_DISP);
                            }
                        loc += 2;
                        break;

        case AnIndIndex:        /* (<data>,An,Xn.SIZE*SCALE) */
        case MemIndPostinx:     /* ([bd,An],Xn,od) */
        case MemIndPreinx:      /* ([bd,An,Xn],od) */
        case PCIndex:           /* (<data>,PC,Xn.SIZE*SCALE) */
        case PCMemIndPostinx:   /* ([bd,PC],Xn,od) */
        case PCMemIndPreinx:    /* ([bd,An,Xn],od) */
                        if (pass2)
                            output(op->xtenWord, WORD);
                        if (op->xtenWord & FULL_FORMAT)
                            {
                            if ((op->xtenWord & BASE) == BASE_LONG)
                                {
                                if (pass2)
                                    output(op->data, LONG);
                                loc +=4;
                                } 
                            if ((op->xtenWord & BASE) == BASE_WORD)
                                {
                                if (pass2)
                                    output(op->data, WORD);
                                loc +=2;
                                }
                            if ((op->xtenWord & OUTER) == OUTER_LONG)
                                {
                                if (pass2)
                                    output(op->outDisp, LONG);
                                loc +=4;
                                } 
                            if ((op->xtenWord & OUTER) == OUTER_WORD)
                                {
                                if (pass2)
                                    output(op->outDisp, WORD);
                                loc +=2;
                                }
                            }
                        loc += 2;
                        break;

        case AbsShort:          /* <data>.W */
                        if (pass2)
                            {
                            output(op->data & 0xFFFF, WORD);
                            if (op->data < -32768 || op->data > 32767)
                                NEWERROR(*errorPtr, INV_ABS_ADDRESS);
                            }
                        loc += 2;
                        break;

        case AbsLong:           /* <data>.L */
                        if (pass2)
                            output(op->data, LONG);
                        loc += 4;
                        break;

        case Immediate:         /* #<data> */
                        if (!size || size == WORD)
                            {
                            if (pass2)
                                {
                                output(op->data & 0xFFFF, WORD);
                                if (op->data > 0xffff)
                                    NEWERROR(*errorPtr, INV_16_BIT_DATA);
                                }
                            loc += 2;
                            break;
                            }
                        else if (size == BYTE)
                            {
                            if (pass2)
                                {
                                output(op->data & 0xFF, WORD);
                                if (op->data < -32768 || op->data > 32767)
                                    NEWERROR(*errorPtr, INV_8_BIT_DATA);
                                }
                            loc += 2;
                            }
                        else if (size == LONG)
                            {
                            if (pass2)
                                output(op->data, LONG);
                            loc += 4;
                            }
                        break;
        default:
                    printf("INVALID EFFECTIVE ADDRESSING MODE!\n");
                    exit(0);
        }

    return NORMAL;
    }

