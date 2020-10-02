/***********************************************************************
 *
 *      OPPARSE.C
 *      Operand Parser for 68000 Assembler
 *
 *    Function: isXreg()
 *      Parses Index Register specification, inserts the upper byte flags
 *      (see below) into extension word.
 *
 *    Function: opParse()
 *      Parses an operand of the 68020 assembly language instruction and
 *      attempts to recognize its addressing mode. The p argument points to
 *      the string to be evaluated, and the function returns a pointer to the
 *      first character beyond the end of the operand.
 *      The function returns a description of the operands that it parses in
 *      an opDescriptor structure. The fields of the operand descriptor are
 *      filled in as appropriate for the mode of the operand that was found:
 *
 *      mode    returns the address mode (symbolic values defined in ASM.H)
 *      data    returns the displacement or address or immediate value
 *      reg     returns the address or data register number
 *      size    returns the size of absolute address (WORD or LONG)
 *      xtenWord returns the indexed operation first extension word. The
 *              7 MSBits are filled in by isXreg(), the 9 LSBits - by opparse.
 *      bfXtenWord bit field extension word
 *      outDisp returns the outer displacement of an indexed operation
 *      backRef TRUE if data is the value of an expression that contains only
 *              constants and backwards references; FALSE otherwise.
 *      odBackRef TRUE if outer displacement is the value of an expression
 *              that contains only constants and backward references; FALSE
 *              otherwise.
 *
 *      The argument errorPtr is used to return an error code via the standard
 *      mechanism.
 *
 *   Usage: char *opParse(char *p, opDescriptor *d, int *errorPtr)
 *
 *      Author: Paul McKee
 *      ECE492    North Carolina State University, 10/10/86
 *
 *    Revision: 10/26/87
 *      Altered the immediate mode case to correctly flag
 *      constructs such as "#$1000(A5)" as syntax errors.
 *
 *      Modified A.E. Romer. Version 1.0
 *          17 March 1991 - converted to ANSI
 *          21 March 1991 - .w and .l suffixes allowed in absolute addressing
 *          May      1991 - isXreg() added.
 *          Summer   1991 - conversion to 68020 format of indirection 
 *                          instruction, index SCALE implementation, addition 
 *                          of 68020 specific instructions
 *
 ************************************************************************/


#include <stdio.h>
#include <ctype.h>
#include "asm.h"


extern char pass2;
extern long loc;


#define isTerm(c)   (isspace(c)         /* operand field terminator */\
                        || (c == ',')   /* source/destination separator */\
                        || c == '{'     /* bit field specification start */\
                        || c == '\0')   /* line terminator */

#define isRegNum(c) ((c >= '0') && (c <= '7'))


char *isXreg(char *p, opDescriptor *d, int *errorPtr)
    {
    long scale;

    if ((p[0] == 'A' || p[0] == 'D') && isRegNum(p[1]))
                        /* set index register field in the extension word */
        {
        d->xtenWord |= XREG_NUM(p[1] - '0');
        if (p[0] == 'A')
            d->xtenWord |= XREG_IS_An;

/* neither size nor scale specified */
        if (p[2] == ')' || p[2] == ']' || p[2] ==',')
            return p+2;             /* Default index register size
                                     * and scale, null displacement */

/* size specified */
        else if (p[2] == '.')
            {
            /* Determine size of index register */
            if (p[3] == 'W')
                ;                   /* default */
            else if (p[3] == 'L')
                d->xtenWord |= XSIZE_LONG;
            else
                {
                NEWERROR(*errorPtr, SYNTAX);
                return NULL;
                }
            if (p[4] == ')' || p[4] == ']' || p[4] == ',')
                                                    /* no scale specified */
                return p+4;                         /* default scale is 1 */

            else if (p[4] == '*')                   /* scale specified */
                {
                p = eval(p+5, &scale, &d->backRef, errorPtr);
                if (*errorPtr > SEVERE)
                    return NULL;
                else if (p[0] != ')' && p[0] != ']' && p[0] != ',')
                                                    /* missing terminator */
                    {
                    NEWERROR(*errorPtr, SYNTAX);
                    return NULL;
                    }
                else
                    switch (scale)
                        {
                        case 1: return p;
                        case 2: d->xtenWord |= SCALE_TWO; return p;
                        case 4: d->xtenWord |= SCALE_FOUR; return p;
                        case 8: d->xtenWord |= SCALE_EIGHT; return p;
                        default: NEWERROR(*errorPtr, SYNTAX); return NULL;
                        }
                }
            }

 /* size not specified, scale specified */
        else if (p[2] == '*')
            {
            p = eval(p+3, &scale, &d->backRef, errorPtr);
            if (*errorPtr > SEVERE)
                return NULL;
            else if (p[0] != ')' && p[0] != ']')
                {
                NEWERROR(*errorPtr, SYNTAX);
                return NULL;
                }
            else
                switch (scale)
                    {
                    case 1: return p;
                    case 2: d->xtenWord |= SCALE_TWO; return p;
                    case 4: d->xtenWord |= SCALE_FOUR; return p;
                    case 8: d->xtenWord |= SCALE_EIGHT; return p;
                    default: NEWERROR(*errorPtr, SYNTAX); return NULL;
                    }
            }

        else                    /* neither ')', nor '.', nor '*' follows Xn */
            {
            NEWERROR(*errorPtr, SYNTAX);
            return NULL;
            }
        }
    else                                    /* incorrect indexing syntax */
        return NULL;
    }


char *opParse(char *p, opDescriptor *d, int *errorPtr)
    {
    char *t;                                        /* temporary pointers */
    long value;
    char ref;
    int  error;

    d->xtenWord = 0;        /* initialize */

    /**********************************************************\
     *****      data or address register direct:          *****
     *****              "Dn" or "An"                      *****
    \**********************************************************/

    if (isRegNum(p[1]) && isTerm(p[2]))
        {
        if (p[0] == 'D')
            {
            d->mode = DnDirect;
            d->reg = p[1] - '0';
            return (p + 2);
            }
        else if (p[0] == 'A')
            {
            d->mode = AnDirect;
            d->reg = p[1] - '0';
            return (p + 2);
            }
        }

    /**********************************************************\
     *****          Stack Pointer (A7) direct:            *****
     *****                  "SP"                          *****
    \**********************************************************/

    if (p[0] == 'S' && p[1] == 'P' && isTerm(p[2]))
        {
        d->mode = AnDirect;
        d->reg = 7;
        return (p + 2);
        }

    /**********************************************************\
     *****          Address register indirect:            *****
     *****      "(An)", "(An)+", "(SP)", or "(SP)+"       *****
    \**********************************************************/

    if (p[0] == '('
            && ((p[1] == 'A' && isRegNum(p[2]))
                                || (p[1] == 'S' && p[2] == 'P'))
            && p[3] == ')')
        {

        if (p[1] == 'S')
            d->reg = 7;
        else
            d->reg = p[2] - '0';

        if (isTerm(p[4]))
                        /* plain address register indirect: (An) or (SP) */
            {
            d->mode = AnInd;
            return p+4;
            }
        else if (p[4] == '+' &&isTerm(p[5]))
                                        /* postincrement: (An)+ or (SP)+ */
            {
            d->mode = AnIndPost;
            return p+5;
            }
        else
            {
            NEWERROR(*errorPtr, SYNTAX);
            return NULL;
            }
        }

    /**********************************************************\
     *****  Address register indirect with predecrement:  *****
     *****              "-(An)" or "-(SP)"                *****
    \**********************************************************/
    
    if (p[0] == '-' && p[1] == '(' && p[4] == ')' && ((p[2] == 'A'
            && isRegNum(p[3])) || (p[2] == 'S' && p[3] == 'P'))
            && isTerm(p[5]))
        {
        if (p[2] == 'S')
            d->reg = 7;
        else
            d->reg = p[3] - '0';
        d->mode = AnIndPre;
        return p+5;
        }

    /********************************************************\
     *****  Address register indirect with index        *****
     *****          "(An,Xn.SIZE*SCALE)"                *****
    \********************************************************/

    if (p[0] == '('
            && ((p[1] == 'A' && isRegNum(p[2]))
                                || (p[1] == 'S' && p[2] == 'P'))
            && p[3] == ','
            && ((t = isXreg(p+4, d, errorPtr)) != NULL))
        {
        if (p[1] == 'S')
            d->reg = 7;
        else
            d->reg = p[2] - '0';
        p = t;
        if (p[0] == ')' && isTerm(p[1]))
            {
            d->mode = AnIndIndex;
            d->data = 0;                        /* Displacement is zero */
            d->backRef = TRUE;
            return p+1;
            }
        else
            {
            NEWERROR(*errorPtr, SYNTAX);
            return NULL;
            }
        }

    /*****************************************************\
     *****                PC relative:               *****
     *****            "(PC,Xn.SIZE*SCALE)"           *****
    \*****************************************************/

    if (p[0] == '(' && p[1] == 'P' && p[2] == 'C')
        {
        if (p[3] == ')' && isTerm(p[4]))            /* plain PC relative */
            {
            d->data = 0;
            d->backRef = TRUE;
            d->mode = PCDisp;
            return p+4;
            }

        else if (p[3] == ',' && (t = isXreg(p+4, d, errorPtr)) != NULL)
            {
            p = t;
            if (p[0] == ')' && isTerm(p[1]))
                {
                d->mode = PCIndex;
                d->data = 0;                        /* Displacement is zero */
                    /* displacement is zero by default in extension word */
                d->backRef = TRUE;
                return p+1;
                }
            else
                {
                NEWERROR(*errorPtr, SYNTAX);
                return NULL;
                }
            }
        }

    /**********************************************************\
     *****              Status Register direct:           *****
     *****                      "SR"                      *****
    \**********************************************************/

    if (p[0] == 'S' && p[1] == 'R' && isTerm(p[2]))
        {
        d->mode = SRDirect;
        return p+2;
        }

    /**********************************************************\
     *****      Condition Code Register direct:           *****
     *****                  "CCR"                         *****
    \**********************************************************/

    if (p[0] == 'C' && p[1] == 'C' && p[2] == 'R' && isTerm(p[3]))
        {
        d->mode = CCRDirect;
        return p+3;
        }

    /**********************************************************\
     *****          User Stack Pointer direct:            *****
     *****                  "USP"                         *****
    \**********************************************************/

    if (p[0] == 'U' && p[1] == 'S' && p[2] == 'P' && isTerm(p[3]))
        {
        d->mode = USPDirect;
        return p+3;
        }

    /**********************************************************\
     *****  Source Function Code register direct (68010): *****
     *****                  "SFC"                         *****
    \**********************************************************/

    if (p[0] == 'S' && p[1] == 'F' && p[2] == 'C' && isTerm(p[3]))
        {
        d->mode = SFCDirect;
        return p+3;
        }

    /***************************************************************\
     *****  Destination Function Code register direct (68010): *****
     *****                      "DFC"                          *****
    \***************************************************************/

    if (p[0] == 'D' && p[1] == 'F' && p[2] == 'C' && isTerm(p[3]))
        {
        d->mode = DFCDirect;
        return p+3;
        }

    /****************************************************************\
     *****       Vector Base Register Direct (68010):           *****
     *****                      "VBR"                               *****
    \****************************************************************/

    if (p[0] == 'V' && p[1] == 'B' && p[2] == 'R' && isTerm(p[3]))
        {
        d->mode = VBRDirect;
        return p+3;
        }

    /****************************************************************\
     *****       Cache Control Register Direct (68020):         *****
     *****                      "CACR"                          *****
    \****************************************************************/

    if (p[0] == 'C' && p[1] == 'A' && p[2] == 'C' && p[3] == 'R'
                                                    && isTerm(p[4]))
        {
        d->mode = CACRDirect;
        return p+4;
        }

    /****************************************************************\
     *****       Cache Address Register Direct (68020):         *****
     *****                      "CAAR"                          *****
    \****************************************************************/

    if (p[0] == 'C' && p[1] == 'A' && p[2] == 'A' && p[3] == 'R'
                                                    && isTerm(p[4]))
        {
        d->mode = CAARDirect;
        return p+4;
        }

    /**********************************************************\
     *****      Master Stack Pointer direct (68020):      *****
     *****                      "MSP"                     *****
    \**********************************************************/

    if (p[0] == 'M' && p[1] == 'S' && p[2] == 'P' && isTerm(p[3]))
        {
        d->mode = MSPDirect;
        return p+3;
        }

    /**********************************************************\
     *****      Interrupt Stack Pointer direct (68020):   *****
     *****                      "ISP"                     *****
    \**********************************************************/

    if (p[0] == 'I' && p[1] == 'S' && p[2] == 'P' && isTerm(p[3]))
        {
        d->mode = ISPDirect;
        return p+3;
        }

    /************************************************************\
     ************************************************************
     *****      Memory (including Program Counter Memory)   *****
     *****                  indirect modes                  *****
     *****               All beginning with "(["            *****
     ************************************************************
    \************************************************************/

    if (p[0] == '(' && p[1] == '[')
        {

        /************************************************************\
         ************************************************************
         *****      Memory (including Program Counter Memory)   *****
         *****      indirect modes, no base displacement        *****
         ************************************************************
        \************************************************************/

        /************************************************************\
         *****      memory indirect, all elements suppressed    *****
         *****                  "([],,)", "([,],)"              *****
        \************************************************************/

        /* not implemented */

        /****************************************************\
         *****      memory indirect without index       *****
         *****              "([An],)", "([An,],)"       *****
        \****************************************************/

        /* not implemented */


        /****************************************************\
         *****  memory indirect without base register   *****
         *****          "([],Xn)", "([,,Xn],)"          *****
        \****************************************************/

        /* not implemented */

        /****************************************************************\
         *****  memory indirect postindexed, no outer displacement  *****
         *****                      "([An],Xn)"                     *****
        \****************************************************************/

        if (p[2] == 'A' && isRegNum(p[3])
                        && p[4] == ']' && p[5] == ','
                        && (t = isXreg(p+6, d, errorPtr)) != NULL
                        && t[0] == ')'&& isTerm(t[1]) )
                        
            {
            d->reg = p[3] - '0';
            d->mode = MemIndPostinx;
            d->xtenWord |= FULL_FORMAT | BASE_NULL | OUTER_NULL | POST_X;
            return t+1;
            }

        /****************************************************************\
         *****  memory indirect preindexed, no outer displacement   *****
         *****                      "([An,Xn])"                     *****
        \****************************************************************/

        if (p[2] == 'A' && isRegNum(p[3])
                        && p[4] == ','
                        && (t = isXreg(p+5, d, errorPtr)) != NULL
                        && t[0] == ']' && t[1] == ')' && isTerm(t[2]))
                        
            {
            d->reg = p[3] - '0';
            d->mode = MemIndPreinx;
            d->xtenWord |= FULL_FORMAT | BASE_NULL | OUTER_NULL;
            return t+2;
            }

        /************************************************\
         *****      memory indirect postindexed     *****
         *****          "([An],Xn,od)"              *****
        \************************************************/

        if (p[2] == 'A' && isRegNum(p[3])
                        && p[4] == ']' && p[5] == ','
                        && (t = isXreg(p+6, d, errorPtr)) != NULL
                        && t[0] == ',')
            {
            t = eval(++t, &d->outDisp, &d->odBackRef, errorPtr);
            if (*errorPtr < SEVERE)
                if (t[0] == ')' && isTerm(t[1]))
                    {
                    d->reg = p[3] - '0';
                    d->mode = MemIndPostinx;
                    d->xtenWord |= FULL_FORMAT | BASE_NULL | POST_X;
                    if (d->odBackRef
                            && d->outDisp >= -32768 && d->outDisp <= 32767)
                        d->xtenWord |=OUTER_WORD;
                    else
                        d->xtenWord |=OUTER_LONG;
                    return t+1;
                    }
                else
                    {
                    NEWERROR(*errorPtr, SYNTAX);
                    return NULL;            /* outer displacement
                                             * incorrectly terminated */
                    }
            else
                return NULL;                /* error in 'eval' */
            }

        /************************************************\
         *****      memory indirect preindexed      *****
         *****          "([An,Xn],od)"              *****
        \************************************************/

        if (p[2] == 'A' && isRegNum(p[3])
                        && p[4] == ','
                        && (t = isXreg(p+5, d, errorPtr)) != NULL
                        && t[0] == ']' && t[1] == ',')
            {
            t = eval(t+2, &d->outDisp, &d->odBackRef, errorPtr);
            if (*errorPtr < SEVERE)
                if (t[0] == ')' && isTerm(t[1]))
                    {
                    d->reg = p[3] - '0';
                    d->mode = MemIndPreinx;
                    d->xtenWord |= FULL_FORMAT | BASE_NULL;
                    if (d->odBackRef
                            && d->outDisp >= -32768 && d->outDisp <= 32767)
                        d->xtenWord |=OUTER_WORD;
                    else
                        d->xtenWord |=OUTER_LONG;
                    return t+1;
                    }
                else
                    {
                    NEWERROR(*errorPtr, SYNTAX);
                    return NULL;            /* outer displacement
                                             * incorrectly terminated */
                    }
            else
                return NULL;                /* error in 'eval' */
            }

        /****************************************************************\
         *****      PC memory indirect, all elements suppressed     *****
         *****                  "([ZPC],,)", "([ZPC,],)"            *****
        \****************************************************************/

        /* not implemented */

        /********************************************************\
         *****      PC memory indirect without index        *****
         *****          "([PC],)", "([PC,],)"               *****
        \********************************************************/

        /* not implemented */

        /****************************************************\
         *****  memory indirect without base register   *****
         *****          "([ZPC],Xn)", "([ZPC,Xn],)"         *****
        \****************************************************/

        /* not implemented */

        /********************************************************************\
         *****  PC memory indirect postindexed, no outer displacement   *****
         *****                      "([PC],Xn)"                         *****
        \********************************************************************/

        if (p[2] == 'P' && p[3] == 'C'
                        && p[4] == ']' && p[5] == ','
                        && (t = isXreg(p+6, d, errorPtr)) != NULL
                        && t[0] == ')'&& isTerm(t[1]) )
                        
            {
            d->reg = 3;
            d->mode = PCMemIndPostinx;
            d->xtenWord |= FULL_FORMAT | BASE_NULL | OUTER_NULL | POST_X;
            return t+1;
            }

        /********************************************************************\
         *****  PC memory indirect preindexed, no outer displacement    *****
         *****                      "([PC,Xn])"                         *****
        \********************************************************************/

        if (p[2] == 'P' && p[3] == 'C'
                        && p[4] == ','
                        && (t = isXreg(p+5, d, errorPtr)) != NULL
                        && t[0] == ')' && isTerm(t[1]))
                        
            {
            d->reg = 3;
            d->mode = PCMemIndPreinx;
            d->xtenWord |= FULL_FORMAT | BASE_NULL | OUTER_NULL;
            return t+1;
            }

        /****************************************************\
         *****      PC memory indirect postindexed      *****
         *****              "([PC],Xn,od)"              *****
        \****************************************************/

        if (p[2] == 'P' && p[3] == 'C'
                        && p[4] == ']' && p[5] == ','
                        && (t = isXreg(p+6, d, errorPtr)) != NULL
                        && t[0] == ',')
            {
            t = eval(++t, &d->outDisp, &d->odBackRef, errorPtr);
            if (*errorPtr < SEVERE)
                if (t[0] == ')' && isTerm(t[1]))
                    {
                    d->reg = 3;
                    d->mode = PCMemIndPostinx;
                    d->xtenWord |= FULL_FORMAT | BASE_NULL | POST_X;
                    if (d->odBackRef
                            && d->outDisp >= -32768 && d->outDisp <= 32767)
                        d->xtenWord |=OUTER_WORD;
                    else
                        d->xtenWord |=OUTER_LONG;
                    return t+1;
                    }
                else
                    {
                    NEWERROR(*errorPtr, SYNTAX);
                    return NULL;            /* outer displacement
                                             * incorrectly terminated */
                    }
            else
                return NULL;                /* error in 'eval' */
            }

        /****************************************************\
         *****      PC memory indirect preindexed       *****
         *****              "([PC,Xn],od)"              *****
        \****************************************************/

        if (p[2] == 'P' && p[3] == 'C'
                        && p[4] == ','
                        && (t = isXreg(p+5, d, errorPtr)) != NULL
                        && t[0] == ']' && t[1] == ',')
            {
            t = eval(++t, &d->outDisp, &d->odBackRef, errorPtr);
            if (*errorPtr < SEVERE)
                if (t[0] == ')' && isTerm(t[1]))
                    {
                    d->reg = 3;
                    d->mode = PCMemIndPreinx;
                    d->xtenWord |= FULL_FORMAT | BASE_NULL;
                    if (d->odBackRef
                            && d->outDisp >= -32768 && d->outDisp <= 32767)
                        d->xtenWord |=OUTER_WORD;
                    else
                        d->xtenWord |=OUTER_LONG;
                    return t+1;
                    }
                else
                    {
                    NEWERROR(*errorPtr, SYNTAX);
                    return NULL;            /* outer displacement
                                             * incorrectly terminated */
                    }
            else
                return NULL;                /* error in 'eval' */
            }

    /************************************************************\
     ************************************************************
     *****      Memory (including Program Counter Memory)   *****
     *****      indirect modes with base displacement       *****
     *****            All beginning with "([<data>"         *****
     ************************************************************
    \************************************************************/

        p = eval(p+2, &d->data, &d->backRef, errorPtr);
            if (*errorPtr < SEVERE)
                {

                /************************************************\
                 *****      memory indirect postindexed     *****
                 *****          "([bd,An],Xn,od)"           *****
                \************************************************/

                if (p[0] == ',' && p[1] == 'A' && isRegNum(p[2])
                                && p[3] == ']' && p[4] == ','
                                && (t = isXreg(p+5, d, errorPtr)) != NULL)
                    {
                    d->reg = p[2] - '0';
                    p = t;
                    d->mode = MemIndPostinx;
                    d->xtenWord |= FULL_FORMAT | POST_X;
                    if (d->backRef && d->data == 0)
                        d->xtenWord |= BASE_NULL;
                    else if (d->backRef && d->data >= -32768
                                                        && d->data <= 32767)
                        d->xtenWord |= BASE_WORD;
                    else
                        d->xtenWord |= BASE_LONG;
                    if (p[0] == ')' && isTerm(p[1]))
                                                /* no outer displacement */
                        {
                        d->outDisp = 0;
                        d->odBackRef = TRUE;
                        d->xtenWord |= OUTER_NULL;
                        return p+1;
                        }
                    else if (p[0] == ',')       /* outer displacement
                                                 * specified */
                        {
                        p = eval(++p, &d->outDisp, &d->odBackRef, errorPtr);
                        if (*errorPtr < SEVERE)
                            if (p[0] == ')' && isTerm(p[1]))
                                {
                                if (d->odBackRef && d->outDisp == 0)
                                    d->xtenWord |= OUTER_NULL;
                                else if (d->backRef && d->outDisp >= -32768
                                                    && d->outDisp <= 32767)
                                    d->xtenWord |= OUTER_WORD;
                                else
                                    d->xtenWord |= OUTER_LONG;
                                return p+1;
                                }
                            else                /* outer displacement
                                                 * incorrectly terminated */
                                {
                                NEWERROR(*errorPtr, SYNTAX);
                                return NULL;
                                }
                        else
                            return NULL;        /* error in 'eval' */
                        }
                    else                        /* either ',' or ')' must
                                                 * follow index register */
                        {
                        NEWERROR(*errorPtr, SYNTAX);
                        return NULL;
                        }
                    }

                /************************************************\
                 *****      memory indirect preindexed      *****
                 *****          "([bd,An,Xn],od)"           *****
                \************************************************/
                else if (p[0] == ',' && p[1] == 'A' && isRegNum(p[2])
                        && p[3] == ','
                        && (t = isXreg(p+4, d, errorPtr)) != NULL
                        && t[0] == ']')
                    {
                    d->reg = p[2] - '0';
                    d->mode = MemIndPreinx;
                    d->xtenWord |= FULL_FORMAT;
                    if (d->backRef && d->data == 0)
                        d->xtenWord |= BASE_NULL;
                    else if (d->backRef && d->data >= -32768
                                                    && d->data <= 32767)
                        d->xtenWord |= BASE_WORD;
                    else
                        d->xtenWord |= BASE_LONG;
                    p = t;
                    if (p[1] == ')' && isTerm(p[2]))
                                                /* no outer displacement */
                        {
                        d->outDisp = 0;
                        d->odBackRef = TRUE;
                        d->xtenWord |= OUTER_NULL;
                        return p+2;
                        }
                    else if (p[1] == ',')       /* outer displacement
                                                 * specified */
                        {
                        p = eval(p+2, &d->outDisp, &d->odBackRef, errorPtr);
                        if (*errorPtr < SEVERE)
                            if (p[0] == ')' && isTerm(p[1]))
                                {
                                if (d->odBackRef && d->outDisp == 0)
                                    d->xtenWord |= OUTER_NULL;
                                else if (d->backRef && d->outDisp >= -32768
                                                    && d->outDisp <= 32767)
                                    d->xtenWord |= OUTER_WORD;
                                else
                                    d->xtenWord |= OUTER_LONG;
                                return p+1;
                                }
                            else
                                return NULL;    /* outer displacement
                                                 * incorrectly terminated */
                        else
                            return NULL;        /* error in 'eval' */
                        }
                    else                        /* either ',' or ')' must
                                                 * follow index register */
                        {
                        NEWERROR(*errorPtr, SYNTAX);
                        return NULL;
                        }
                    }
                /****************************************************\
                 *****      PC memory indirect postindexed      *****
                 *****          "([bd,PC],Xn,od)"               *****
                \****************************************************/

                if (p[0] == ',' && p[1] == 'P' && p[2] == 'C'
                                && p[3] == ']' && p[4] == ','
                                && (t = isXreg(p+5, d, errorPtr)) != NULL)
                    {
                    p = t;
                    d->mode = PCMemIndPostinx;
                    d->xtenWord |= FULL_FORMAT | POST_X;
                    if (d->backRef && d->data == 0)
                        d->xtenWord |= BASE_NULL;
                    else if (d->backRef && d->data >= -32768
                                                        && d->data <= 32767)
                        d->xtenWord |= BASE_WORD;
                    else
                        d->xtenWord |= BASE_LONG;
                    if (p[0] == ')' && isTerm(p[1]))
                                                /* no outer displacement */
                        {
                        d->outDisp = 0;
                        d->odBackRef = TRUE;
                        d->xtenWord |= OUTER_NULL;
                        return p+1;
                        }
                    else if (p[0] == ',')       /* outer displacement
                                                 * specified */
                        {
                        p = eval(++p, &d->outDisp, &d->odBackRef, errorPtr);
                        if (*errorPtr < SEVERE)
                            if (p[0] == ')' && isTerm(p[1]))
                                {
                                if (d->odBackRef && d->outDisp == 0)
                                    d->xtenWord |= OUTER_NULL;
                                else if (d->backRef && d->outDisp >= -32768
                                                    && d->outDisp <= 32767)
                                    d->xtenWord |= OUTER_WORD;
                                else
                                    d->xtenWord |= OUTER_LONG;
                                return p+1;
                                }
                            else                /* outer displacement
                                                 * incorrectly terminated */
                                {
                                NEWERROR(*errorPtr, SYNTAX);
                                return NULL;
                                }
                        else
                            return NULL;        /* error in 'eval' */
                        }
                    else                        /* either ',' or ')' must
                                                 * follow index register */
                        {
                        NEWERROR(*errorPtr, SYNTAX);
                        return NULL;
                        }
                    }

                /****************************************************\
                 *****      PC memory indirect preindexed       *****
                 *****          "([bd,An,Xn],od)"               *****
                \****************************************************/
                else if (p[0] == ',' && p[1] == 'P' && p[2] == 'C'
                        && p[3] == ','
                        && (t = isXreg(p+4, d, errorPtr)) != NULL
                        && t[0] == ']')
                    {
                    d->mode = PCMemIndPreinx;
                    d->xtenWord |= FULL_FORMAT;
                    if (d->backRef && d->data == 0)
                        d->xtenWord |= BASE_NULL;
                    else if (d->backRef && d->data >= -32768
                                                    && d->data <= 32767)
                        d->xtenWord |= BASE_WORD;
                    else
                        d->xtenWord |= BASE_LONG;
                    p = t;
                    if (p[1] == ')' && isTerm(p[2]))
                                                /* no outer displacement */
                        {
                        d->outDisp = 0;
                        d->odBackRef = TRUE;
                        d->xtenWord |= OUTER_NULL;
                        return p+2;
                        }
                    else if (p[1] == ',')       /* outer displacement
                                                 * specified */
                        {
                        p = eval(p+2, &d->outDisp, &d->odBackRef, errorPtr);
                        if (*errorPtr < SEVERE)
                            if (p[0] == ')' && isTerm(p[1]))
                                {
                                if (d->odBackRef && d->outDisp == 0)
                                    d->xtenWord |= OUTER_NULL;
                                else if (d->backRef && d->outDisp >= -32768
                                                    && d->outDisp <= 32767)
                                    d->xtenWord |= OUTER_WORD;
                                else
                                    d->xtenWord |= OUTER_LONG;
                                return p+1;
                                }
                            else
                                return NULL;    /* outer displacement
                                                 * incorrectly terminated */
                        else
                            return NULL;        /* error in 'eval' */
                        }
                    else                        /* either ',' or ')' must
                                                 * follow index register */
                        {
                        NEWERROR(*errorPtr, SYNTAX);
                        return NULL;
                        }
                    }
                }               /* end of all beginning with "([<data>" */
            }                   /* end of all beginning with "([" */

    /****************************************************************\
     *****                 All beginning with                   *****
     *****                      "(<data>"                           *****
    \****************************************************************/

    if (p[0] == '(')
        {
        p = eval(++p, &d->data, &d->backRef, errorPtr);
        if (*errorPtr < SEVERE)
            {

            /************************************************************\
             *****      address register indirect, no index         *****
             *****              (<data>,An)                         *****
            \************************************************************/

            if (p[0] == ',' && ((p[1] == 'A' && isRegNum(p[2])) ||
                    (p[1] == 'S' && p[2] == 'P')))
                            /* address register indirect with displacement */
                {
                if (p[1] == 'S')
                    d->reg = 7;
                else
                    d->reg = p[2] - '0';

                if (p[3] == ')' && isTerm(p[4]))
                                            /* plain address register
                                             * indirect with displacement */
                    {
                    d->mode = AnIndDisp;
                    return p+4;
                    }

                /************************************************************\
                 *****          address register indirect               *****
                 *****          (<data>,An,Xn.SIZE*SCALE)               *****
                \************************************************************/

                else if (p[3] == ',' && (t = isXreg(p+4, d, errorPtr)) != NULL)
                                    /* address register indirect with index */
                    {
                    p = t;
                    if (p[0] == ')' && isTerm(p[1]))
                        {
                        d->mode = AnIndIndex;
                        if (d->backRef && d->data >= -128 && d->data <= 127)
                            d->xtenWord |= d->data & 0xff;
                        else if (d->backRef && d->data >= -32768
                                                        && d->data <= 32767)
                            d->xtenWord |= FULL_FORMAT | BASE_WORD
                                                            | OUTER_NULL;
                        else
                            d->xtenWord |= FULL_FORMAT | BASE_LONG
                                                            | OUTER_NULL;
                        return p+1;
                        }
                    else
                        {
                        NEWERROR(*errorPtr, SYNTAX);
                        return NULL;
                        }
                    }
                }

            /****************************************************************\
             *****                  PC relative, no index               *****
             *****                      (<data>,PC)                     *****
            \****************************************************************/

            if (p[0] == ',' && p[1] == 'P' && p[2] == 'C')
                                        /* PC relative with displacement */
                {
                if (p[3] == ')' && isTerm(p[4]))    /* plain PC relative */
                    {
                    d->mode = PCDisp;
                    return p+4;
                    }

            /********************************************************\
             *****              PC relative                     *****
             *****          (<data>,PC,Xn.SIZE*SCALE)           *****
            \********************************************************/

                else if (p[3] == ',' && (t = isXreg(p+4, d, errorPtr)) != NULL)
                                                /* PC relative with index */
                    {
                    p = t;
                    if (p[0] == ')' && isTerm(p[1]))
                        {
                        d->mode = PCIndex;
                        d->data -= loc;     /* displacement referenced to
                                             * the current value of
                                             * program counter */
                        if (d->backRef && d->data >= -128 && d->data <= 127)
                            d->xtenWord |= d->data & 0xff;
                        else if (d->backRef && d->data >= -32768
                                                        && d->data <= 32767)
                            d->xtenWord |= FULL_FORMAT | BASE_WORD
                                                            | OUTER_NULL;
                        else
                            d->xtenWord |= FULL_FORMAT | BASE_LONG
                                                            | OUTER_NULL;
                        return p+1;
                        }
                    else
                        {
                        NEWERROR(*errorPtr, SYNTAX);
                        return NULL;
                        }
                    }
                }
            }
        }                           /* end of all beginning with "(<data> */

    /********************************************\
     *****           absolute               *****
     *****            <data>                *****
    \********************************************/

    error = *errorPtr;                          /* save current value */
    t = eval(p, &d->data, &d->backRef, errorPtr);
                                            /* is the first token a value? */
    if (*errorPtr < SEVERE)
        {
        p = t;
        if (isTerm(p[0]))
                  /* Determine size of absolute address (must be long if
                     the symbol isn't defined or if the value is too big */
            {
            if (!d->backRef || d->data > 32767 || d->data < -32768)
                d->mode = AbsLong;
            else
                d->mode = AbsShort;
            return p;
            }
        if (p[0] == '.' && isTerm(p[2]))
            if (p[1] == 'W')
                {
                d->mode = AbsShort;
                return p+2;
                }
            else if (p[1] == 'L')
                {
                d->mode = AbsLong;
                return p+2;
                }
            else
                {
                NEWERROR(*errorPtr, SYNTAX);
                return NULL;
                }
        }
    else
        *errorPtr = error;              /* restore old value */

    /************************************************\
     *****         Immediate mode:              *****
     *****             #<data>                  *****
    \************************************************/

    if (p[0] == '#')
        {
        p = eval(++p, &d->data, &d->backRef, errorPtr);

        /* If expression evaluates OK, then return */
        if (*errorPtr < SEVERE)
            {
            if (isTerm(*p))
                {
                d->mode = Immediate;
                return p;
                }
            else
                {
                NEWERROR(*errorPtr, SYNTAX);
                return NULL;
                }
            }
        else
            return NULL;
        }

    /* If the operand doesn't match any pattern, return an error status */
    NEWERROR(*errorPtr, SYNTAX);
    return NULL;
    }
