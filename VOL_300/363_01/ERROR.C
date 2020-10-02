/***********************************************************************
 *
 *      ERROR.C
 *      Error message printer for 68020 Assembler
 *
 *    Function: printError()
 *      Prints an appropriate message to the specified output file according 
 *      to the error code supplied. If the errorCode is OK, no message is 
 *      printed; otherwise an WARNING or ERROR message is produced. The line 
 *      number will be included in the message unless lineNum = -1.
 *
 *   Usage: printError(FILE *outFile, int errorCode, int lineNum)
 *
 *      Author: Paul McKee
 *      ECE492    North Carolina State University,  12/12/86
 *
 *      Modified A.E. Romer     16 March 1991
 *      Version 1.0
 *
 ************************************************************************/


#include <stdio.h>
#include "asm.h"

int printError(FILE *outFile, int errorCode, int lineNum)
    {
    char numBuf[20];

    if (lineNum >= 0)
        sprintf(numBuf, " in line %d", lineNum);
    else
        numBuf[0] = '\0';

            if (errorCode > MINOR)
                fprintf(outFile, "    ERROR");
            else if (errorCode > WARNING)
                fprintf(outFile, "    WARNING");

    switch (errorCode)
        {
        
/* Severe errors */     
        case SYNTAX          :
            fprintf(outFile, "%s: Invalid syntax\n", numBuf);
            break;
        case INV_OPCODE      :
            fprintf(outFile, "%s: Invalid opcode\n", numBuf);
            break;
        case INV_ADDR_MODE   :
            fprintf(outFile, "%s: Invalid addressing mode\n", numBuf);
            break;
        case LABEL_REQUIRED  :
            fprintf(outFile, "%s: Label required with this directive\n",
                                                                    numBuf);
            break;
        case PHASE_ERROR     :
            fprintf(outFile, "%s: Symbol value differs ", numBuf);
            fprintf(outFile, "between first and second pass\n", numBuf);
            break;
        case UNIMPLEMENTED:
            fprintf(outFile, "%s: This code is not implemented\n", numBuf);
            break;
        case INV_SHORT_BRANCH:
            fprintf(outFile, "%s: Short branch to the immediately ", numBuf);
            fprintf(outFile, "following instruction is not allowed\n", numBuf);
            break;

/* Errors */
        case UNDEFINED       :
            fprintf(outFile, "%s: Undefined symbol\n", numBuf);
            break;
        case DIV_BY_ZERO     :
            fprintf(outFile, "%s: Division by zero attempted\n", numBuf);
            break;
        case MULTIPLE_DEFS   :
            fprintf(outFile, "%s: Symbol multiply defined\n", numBuf);
            break;
        case REG_MULT_DEFS   :
            fprintf(outFile, "%s: Register list multiply defined\n", numBuf);
            break;
        case REG_LIST_UNDEF  :
            fprintf(outFile, "%s: Register list symbol not ", numBuf);
            fprintf(outFile, "previously defined\n", numBuf);
            break;
        case INV_FORWARD_REF :
            fprintf(outFile, "%s: Forward references not ", numBuf);
            fprintf(outFile, "allowed with this directive\n", numBuf);
            break;
        case INV_LENGTH      :
            fprintf(outFile, "%s: Block length is less that zero\n",
                                                                    numBuf);
            break;

/* Minor errors */
        case INV_SIZE_CODE   :
            fprintf(outFile, "%s: Invalid size code\n", numBuf);
            break;
            break;
        case INV_VECTOR_NUM  :
            fprintf(outFile, "%s: Invalid vector number\n", numBuf);
            break;
        case INV_BRANCH_DISP :
            fprintf(outFile, "%s: Branch instruction ", numBuf);
            fprintf(outFile, "displacement is out of range or invalid\n",
                                                                    numBuf);
            break;
        case INV_DISP          :
            fprintf(outFile, "%s: Displacement out of range\n", numBuf);
            break;
        case INV_ABS_ADDRESS :
            fprintf(outFile, "%s: Absolute address exceeds 16 bits\n",
                                                                    numBuf);
            break;
        case INV_3_BIT_DATA  :
            fprintf(outFile, "%s: Immediate data exceeds 3 bits\n",
                                                                    numBuf);
            break;
        case INV_8_BIT_DATA  :
            fprintf(outFile, "%s: Immediate data exceeds 8 bits\n",
                                                                    numBuf);
            break;
        case INV_16_BIT_DATA :
            fprintf(outFile, "%s: Immediate data exceeds 16 bits\n",
                                                                    numBuf);
            break;
        case ODD_ADDRESS     :
            fprintf(outFile, "%s: Origin value is odd, location", numBuf);
            fprintf(outFile, " counter set to next higher address\n", numBuf);
            break;
        case NOT_REG_LIST    :
            fprintf(outFile, "%s: The symbol specified is not ", numBuf);
            fprintf(outFile, "a register list symbol\n", numBuf);
            break;
        case REG_LIST_SPEC   :
            fprintf(outFile, "%s: Register list symbol used in ", numBuf);
            fprintf(outFile, "an expression\n", numBuf);
            break;
        case INV_SHIFT_COUNT :
            fprintf(outFile, "%s: Invalid constant shift count\n",
                                                                    numBuf);
            break;
        case INV_LABEL       :
            fprintf(outFile, "%s: Invalid label character\n",
                                                            numBuf);

/* Warnings */
            break;
        case ASCII_TOO_BIG   :
            fprintf(outFile, "%s: ASCII constant exceeds 4 characters\n",
                                                            numBuf);
            break;
        case NUMBER_TOO_BIG  :
            fprintf(outFile, "%s: Numeric constant exceeds 32 bits\n",
                                                                    numBuf);
            break;
        case INCOMPLETE      :
            fprintf(outFile, "%s: Evaluation of expression ", numBuf);
            fprintf(outFile, "could not be completed\n", numBuf);
            break;
        case EXCESSIVE_SIZE  :
            fprintf(outFile, "%s: Excessive size\n", numBuf);
            break;
        case UNSIZED         :
            fprintf(outFile, "%s: Unsized instruction, size ignored\n", numBuf);
            break;
        case IGNORED_SIZE    :
            fprintf(outFile, "%s: Invalid or illegal size ignored\n", numBuf);
            break;
        case CORRECTED_SIZE  :
            fprintf(outFile, "%s: Invalid size, corrected\n", numBuf);
            break;
        case INV_QUICK_CONST :
            fprintf(outFile,
                "%s: Quick arithmetic constant too small or too big\n",
                                                               numBuf);
            break;
        case INV_MOVEQ_CONST :
            fprintf(outFile,
                "%s: MOVEQ instruction constant exceeds 8 bits,\n", numBuf);
            fprintf(outFile,
                "             Least significant 8 bits used\n");
            break;
        default :
            if (errorCode)
                fprintf(outFile, "%s: No message defined\n", numBuf);
        }

    return NORMAL;
    }

