/***********************************************************************
 *
 *      INSTTABLE.C
 *      Instruction Table for 68020 Assembler
 *
 * Description: This file contains two kinds of data structure declarations: 
 *      "variant lists" and the instruction table. First in the file are 
 *      "variant lists," one for each different instruction. Then comes the 
 *      instruction table, which contains the mnemonics of the various 
 *      instructions, a pointer to the variant list for each instruction, and
 *      other data. Finally, the variable tableSize is initialized to contain 
 *      the number of instructions in the instruction table.
 *
 *      Author: Paul McKee
 *      ECE492    North Carolina State University, 12/13/86
 *
 *      Modified A.E. Romer, Version 1.0     16 March 1991
 *          5 October 1991 - TRAP instruction got lost somehow, restored
 ************************************************************************/

/*********************************************************************

          HOW THE INSTRUCTION TABLE AND VARIANT LISTS ARE USED

     The procedure which instLookup() and assemble() use to look up
and verify an instruction (or directive) is as follows. Once the
mnemonic of the instruction has been parsed and stripped of its size
code and trailing spaces, the instLookup() does a binary search on the
instruction table to determine if the mnemonic is present. If it is
not found, then the INV_OPCODE error results. If the mnemonic is
found, then assemble() examines the field parseFlag for that entry.
This flag is TRUE if the mnemonic represents a normal instruction that
can be parsed by assemble(); it is FALSE if the instruction's operands
have an unusual format (as is the case for MOVEM and DC).

      If the parseFlag is TRUE, then assemble will parse the
instruction's operands, check them for validity, and then pass the
data to the proper routine which will build the instruction. To do
this it uses the pointer in the instruction table to the instruction's
"variant list" and scans through the list until it finds an particular
"variant" of the instruction which matches the addressing mode(s)
specified.  If it finds such a variant, it checks the instruction's
size code and passes the instruction mask for the appropriate size
(there are three masks for each variant) to the building routine
through a pointer in the variant list for that variant.

*********************************************************************/


#include <stdio.h>
#include "asm.h"

/* Define size code masks for instructions that allow more than one size */

#define BW  (BYTE | WORD)
#define WL  (WORD | LONG)
#define BWL (BYTE | WORD | LONG)
#define BL  (BYTE | LONG)


/* Define the "variant lists" for each different instruction */

/* NOTE: in the following variant lists it is essential that variants that do
 * not require destination (for instance 'asl' MemAlt variant) precede variants
 * that do. The reason is that 'assemble' parses variants in the order of this
 * listing, so if the variant in the code parsed were one that does not require
 * destination, 'assemble' would try to parse the destination if the variants
 * were listed in the wrong order, and fail.
 */

variant abcdva[] =
    {
        { DnDirect, DnDirect, BYTE, twoReg, 0xC100, 0xC100, 0 },
        { AnIndPre, AnIndPre, BYTE, twoReg, 0xC108, 0xC108, 0 }
    };

variant addva[] =
    {
        { Immediate, DataAlt,  BWL, immedInst, 0x0600, 0x0640, 0x0680 },
        { Immediate, AnDirect, WL,  quickMath, 0,      0x5040, 0x5080 },
        { All,       DnDirect, BWL, arithReg,  0xD000, 0xD040, 0xD080 },
        { DnDirect,  MemAlt,   BWL, arithAddr, 0xD100, 0xD140, 0xD180 },
        { All,       AnDirect, WL,  arithReg,  0,      0xD0C0, 0xD1C0 },
    };

variant addava[] =
    {
        { All, AnDirect, WL, arithReg, 0, 0xD0C0, 0xD1C0 },
    };

variant addiva[] =
    {
        { Immediate, DataAlt, BWL, immedInst, 0x0600, 0x0640, 0x0680 }
    };

variant addqva[] =
    {
        { Immediate, DataAlt, BWL, quickMath, 0x5000, 0x5040, 0x5080 },
        { Immediate, AnDirect, WL, quickMath, 0,      0x5040, 0x5080 }
    };

variant addxva[] =
    {
        { DnDirect, DnDirect, BWL, twoReg, 0xD100, 0xD140, 0xD180 },
        { AnIndPre, AnIndPre, BWL, twoReg, 0xD108, 0xD148, 0xD188 }
    };

variant andva[] =
    {
        { Data,      DnDirect, BWL, arithReg,  0xC000, 0xC040, 0xC080 },
        { DnDirect,  MemAlt,   BWL, arithAddr, 0xC100, 0xC140, 0xC180 },
        { Immediate, DataAlt,  BWL, immedInst, 0x0200, 0x0240, 0x0280 }
    };

variant andiva[] =
    {
        { Immediate, DataAlt,   BWL,  immedInst,  0x0200, 0x0240, 0x0280 },
        { Immediate, CCRDirect, BYTE, immedToCCR, 0x023C, 0x023C, 0 },
        { Immediate, SRDirect,  WORD, immedWord,  0,      0x027C, 0 }
    };

variant aslva[] =
    {
        { MemAlt,    0,        WORD, oneOp,    0,      0xE1C0, 0 },
        { DnDirect,  DnDirect, BWL,  shiftReg, 0xE120, 0xE160, 0xE1A0 },
        { Immediate, DnDirect, BWL,  shiftReg, 0xE100, 0xE140, 0xE180 }
    };

variant asrva[] =
    {
        { MemAlt,    0,        WORD, oneOp,    0,      0xE0C0, 0 },
        { DnDirect,  DnDirect, BWL,  shiftReg, 0xE020, 0xE060, 0xE0A0 },
        { Immediate, DnDirect, BWL,  shiftReg, 0xE000, 0xE040, 0xE080 }
    };

variant bccva[] =
    {
        { Absolute, 0, SHORT | LONG, branch, 0x6400, 0x6400, 0x6400 }
    };

variant bchgva[] =
    {
        { DnDirect,  MemAlt,   BYTE, arithAddr, 0x0140, 0x0140, 0 },
        { DnDirect,  DnDirect, LONG, arithAddr, 0,      0x0140, 0x0140 },
        { Immediate, MemAlt,   BYTE, staticBit, 0x0840, 0x0840, 0 },
        { Immediate, DnDirect, LONG, staticBit, 0,      0x0840, 0x0840 }
    };

variant bclrva[] =
    {
        { DnDirect, MemAlt, BYTE, arithAddr, 0x0180, 0x0180, 0 },
        { DnDirect, DnDirect, LONG, arithAddr, 0, 0x0180, 0x0180 },
        { Immediate, MemAlt, BYTE, staticBit, 0x0880, 0x0880, 0 },
        { Immediate, DnDirect, LONG, staticBit, 0, 0x0880, 0x0880 }
    };

variant bcsva[] =
    {
        { Absolute, 0, SHORT | LONG, branch, 0x6500, 0x6500, 0x6500 }
    };

variant beqva[] =
    {
        { Absolute, 0, SHORT | LONG, branch, 0x6700, 0x6700, 0x6700 }
    };

variant bfchgva[] =
    {
        { DnDirect | CtrAlt, 0, 0, NULL, 0x0, 0xEAC0, 0x0 }
    };

variant bfclrva[] =
    {
        { DnDirect | CtrAlt, 0, 0, NULL, 0x0, 0xECC0, 0x0 }
    };

variant bfextsva[] =
    {
        { DnDirect | Control, DnDirect, 0, NULL, 0x0, 0xEBC0, 0x0 }
    };

variant bfextuva[] =
    {
        { DnDirect | Control, DnDirect, 0, NULL, 0x0, 0xE9C0, 0x0 }
    };

variant bfffova[] =
    {
        { DnDirect | Control, DnDirect, 0, NULL, 0x0, 0xEDC0, 0x0 }
    };

variant bfinsva[] =
    {
        { DnDirect, DnDirect | Control, 0, NULL, 0x0, 0xEFC0, 0x0 }
    };

variant bfsetva[] =
    {
        { DnDirect | CtrAlt, 0, 0, NULL, 0x0, 0xEEC0, 0x0 }
    };

variant bftstva[] =
    {
        { DnDirect | CtrAlt, 0, 0, NULL, 0x0, 0xE8C0, 0x0 }
    };

variant bgeva[] =
    {
        { Absolute, 0, SHORT | LONG, branch, 0x6C00, 0x6C00, 0x6C00 }
    };

variant bgtva[] =
    {
        { Absolute, 0, SHORT | LONG, branch, 0x6E00, 0x6E00, 0x6E00 }
    };

variant bhiva[] =
    {
        { Absolute, 0, SHORT | LONG, branch, 0x6200, 0x6200, 0x6200 }
    };

variant bhsva[] =
    {
        { Absolute, 0, SHORT | LONG, branch, 0x6400, 0x6400, 0x6400 }
    };

variant bleva[] =
    {
        { Absolute, 0, SHORT | LONG, branch, 0x6f00, 0x6F00, 0x6F00 }
    };

variant blova[] =
    {
        { Absolute, 0, SHORT | LONG, branch, 0x6500, 0x6500, 0x6500 }
    };

variant blsva[] =
    {
        { Absolute, 0, SHORT | LONG, branch, 0x6300, 0x6300, 0x6300 }
    };

variant bltva[] =
    {
        { Absolute, 0, SHORT | LONG, branch, 0x6d00, 0x6D00, 0x6D00 }
    };

variant bmiva[] =
    {
        { Absolute, 0, SHORT | LONG, branch, 0x6b00, 0x6B00, 0x6B00 }
    };

variant bneva[] =
    {
        { Absolute, 0, SHORT | LONG, branch, 0x6600, 0x6600, 0x6600 }
    };

variant bplva[] =
    {
        { Absolute, 0, SHORT | LONG, branch, 0x6a00, 0x6A00, 0x6A00 }
    };

variant brava[] =
    {
        { Absolute, 0, SHORT | LONG, branch, 0x6000, 0x6000, 0x6000 }
    };

variant bsetva[] =
    {
        { DnDirect, MemAlt, BYTE, arithAddr, 0x01C0, 0x01C0, 0 },
        { DnDirect, DnDirect, LONG, arithAddr, 0, 0x01C0, 0x01C0 },
        { Immediate, MemAlt, BYTE, staticBit, 0x08C0, 0x08C0, 0 },
        { Immediate, DnDirect, LONG, staticBit, 0, 0x08C0, 0x08C0 }
    };

variant bsrva[] =
    {
        { Absolute, 0, SHORT | LONG, branch, 0x6100, 0x6100, 0x6100 }
    };

variant btstva[] =
    {
        { DnDirect, Memory, BYTE, arithAddr, 0x0100, 0x0100, 0 },
        { DnDirect, DnDirect, LONG, arithAddr, 0, 0x0100, 0x0100 },
        { Immediate, Memory, BYTE, staticBit, 0x0800, 0x0800, 0 },
        { Immediate, DnDirect, LONG, staticBit, 0, 0x0800, 0x0800 }
    };

variant bvcva[] =
    {
        { Absolute, 0, SHORT | LONG, branch, 0x6800, 0x6800, 0x6800 }
    };

variant bvsva[] =
    {
        { Absolute, 0, SHORT | LONG, branch, 0x6900, 0x6900, 0x6900 }
    };

variant chkva[] =
    {
        { Data, DnDirect, WORD, arithReg, 0, 0x4180, 0 }
    };

variant clrva[] =
    {
        { DataAlt, 0, BWL, oneOp, 0x4200, 0x4240, 0x4280 }
    };

variant cmpva[] =
    {
        { All, DnDirect, BWL, arithReg, 0xB000, 0xB040, 0xB080 },
        { All, AnDirect, WL, arithReg, 0, 0xB0C0, 0xB1C0 },
        { Immediate, DataAlt, BWL, immedInst, 0x0C00, 0x0C40, 0x0C80 }
    };

variant cmpava[] =
    {
        { All, AnDirect, WL, arithReg, 0, 0xB0C0, 0xB1C0 }
    };

variant cmpiva[] =
    {
        { Immediate, DataAlt, BWL, immedInst, 0x0C00, 0x0C40, 0x0C80 }
    };

variant cmpmva[] =
    {
        { AnIndPost, AnIndPost, BWL, twoReg, 0xB108, 0xB148, 0xB188 }
    };

variant dbccva[] =
    {
        { DnDirect, Absolute, WORD, dbcc, 0, 0x54C8, 0 }
    };

variant dbcsva[] =
    {
        { DnDirect, Absolute, WORD, dbcc, 0, 0x55C8, 0 }
    };

variant dbeqva[] =
    {
        { DnDirect, Absolute, WORD, dbcc, 0, 0x57C8, 0 }
    };

variant dbfva[] =
    {
        { DnDirect, Absolute, WORD, dbcc, 0, 0x51C8, 0 }
    };

variant dbgeva[] =
    {
        { DnDirect, Absolute, WORD, dbcc, 0, 0x5CC8, 0 }
    };

variant dbgtva[] =
    {
        { DnDirect, Absolute, WORD, dbcc, 0, 0x5EC8, 0 }
    };

variant dbhiva[] =
    {
        { DnDirect, Absolute, WORD, dbcc, 0, 0x52C8, 0 }
    };

variant dbhsva[] =
    {
        { DnDirect, Absolute, WORD, dbcc, 0, 0x54C8, 0 }
    };

variant dbleva[] =
    {
        { DnDirect, Absolute, WORD, dbcc, 0, 0x5FC8, 0 }
    };

variant dblova[] =
    {
        { DnDirect, Absolute, WORD, dbcc, 0, 0x55C8, 0 }
    };

variant dblsva[] =
    {
        { DnDirect, Absolute, WORD, dbcc, 0, 0x53C8, 0 }
    };

variant dbltva[] =
    {
        { DnDirect, Absolute, WORD, dbcc, 0, 0x5DC8, 0 }
    };

variant dbmiva[] =
    {
        { DnDirect, Absolute, WORD, dbcc, 0, 0x5BC8, 0 }
    };

variant dbneva[] =
    {
        { DnDirect, Absolute, WORD, dbcc, 0, 0x56C8, 0 }
    };

variant dbplva[] =
    {
        { DnDirect, Absolute, WORD, dbcc, 0, 0x5AC8, 0 }
    };

variant dbrava[] =
    {
        { DnDirect, Absolute, WORD, dbcc, 0, 0x51C8, 0 }
    };

variant dbtva[] =
    {
        { DnDirect, Absolute, WORD, dbcc, 0, 0x50C8, 0 }
    };

variant dbvcva[] =
    {
        { DnDirect, Absolute, WORD, dbcc, 0, 0x58C8, 0 }
    };

variant dbvsva[] =
    {
        { DnDirect, Absolute, WORD, dbcc, 0, 0x59C8, 0 }
    };

variant eorva[] =
    {
        { DnDirect, DataAlt, BWL, arithAddr, 0xB100, 0xB140, 0xB180 },
        { Immediate, DataAlt, BWL, immedInst, 0x0A00, 0x0A40, 0x0A80 }
    };

variant eoriva[] =
    {
        { Immediate, DataAlt, BWL, immedInst, 0x0A00, 0x0A40, 0x0A80 },
        { Immediate, CCRDirect, BYTE, immedToCCR, 0x0A3C, 0x0A3C, 0 },
        { Immediate, SRDirect, WORD, immedWord, 0, 0x0A7C, 0 }
    };

variant exgva[] =
    {
        { DnDirect, DnDirect, LONG, exg, 0, 0xC140, 0xC140 },
        { AnDirect, AnDirect, LONG, exg, 0, 0xC148, 0xC148 },
        { GenReg, GenReg, LONG, exg, 0, 0xC188, 0xC188 }
    };

variant extbva[] =
    {
        { DnDirect, 0, LONG, oneReg, 0, 0,      0x49C0 }
    };

variant extva[] =
    {
        { DnDirect, 0, WL,   oneReg, 0, 0x4880, 0x48C0 }
    };

variant illegalva[] =
    {
        { 0, 0, 0, zeroOp, 0, 0x4AFC, 0 }
    };

variant jmpva[] =
    {
        { Control, 0, 0, oneOp, 0, 0x4EC0, 0 }
    };

variant jsrva[] =
    {
        { Control, 0, 0, oneOp, 0, 0x4E80, 0 }
    };

variant leava[] =
    {
        { Control, AnDirect, LONG, arithReg, 0, 0x41C0, 0x41C0 }
    };

variant linkva[] =
    {
        { AnDirect, Immediate, 0, link, 0, 0x4E50, 0 }
    };

variant lslva[] =
    {
        { MemAlt, 0, WORD, oneOp, 0, 0xE3C0, 0 },
        { DnDirect, DnDirect, BWL, shiftReg, 0xE128, 0xE168, 0xE1A8 },
        { Immediate, DnDirect, BWL, shiftReg, 0xE108, 0xE148, 0xE188 }
    };

variant lsrva[] =
    {
        { MemAlt, 0, WORD, oneOp, 0, 0xE2C0, 0 },
        { DnDirect, DnDirect, BWL, shiftReg, 0xE028, 0xE068, 0xE0A8 },
        { Immediate, DnDirect, BWL, shiftReg, 0xE008, 0xE048, 0xE088 }
    };

variant moveva[] =
    {
        { All, DataAlt, BWL, move, 0x1000, 0x3000, 0x2000 },        /* move */
        { All, AnDirect, WL, move, 0, 0x3000, 0x2000 },             /*movea */
        { Data, CCRDirect, WORD, oneOp, 0, 0x44C0, 0 },      /* move to CCR */
        { Data, SRDirect, WORD, oneOp, 0, 0x46C0, 0 },        /* move to SR */
        { CCRDirect, DataAlt, WORD, moveReg, 0, 0x42C0, 0 },
                                                           /* move from CCR */
        { SRDirect, DataAlt, WORD, moveReg, 0, 0x40C0, 0 }, /* move from SR */
        { AnDirect, USPDirect, LONG, moveUSP, 0, 0x4E60, 0x4E60 },
                                                             /* move to USP */
        { USPDirect, AnDirect, LONG, moveUSP, 0, 0x4E68, 0x4E68 }
                                                           /* move from USP */
    };

variant moveava[] =
    {
        { All, AnDirect, WL, move, 0, 0x3000, 0x2000 }             /* movea */
    };

variant movecva[] =
    {
        { SFCDirect | DFCDirect | USPDirect | VBRDirect,
                           GenReg, LONG, movec, 0, 0x4E7A, 0x4E7A },
        { GenReg, SFCDirect | DFCDirect | USPDirect | VBRDirect,
                           LONG, movec, 0, 0x4E7B, 0x4E7B }
    };

variant movepva[] =
    {
        { DnDirect, AnIndDisp, WL, movep, 0, 0x0188, 0x01C8 },
        { AnIndDisp, DnDirect, WL, movep, 0, 0x0108, 0x0148 },
        { DnDirect, AnInd, WL, movep, 0, 0x0188, 0x01C8 },
        { AnInd, DnDirect, WL, movep, 0, 0x0108, 0x0148 }
    };

variant moveqva[] =
    {
        { Immediate, DnDirect, LONG, moveq, 0, 0x7000, 0x7000 }
    };

variant movesva[] =
    {
        { GenReg, MemAlt, BWL, moves, 0x0E00, 0x0E40, 0x0E80 },
        { MemAlt, GenReg, BWL, moves, 0x0E00, 0x0E40, 0x0E80 }
    };

variant mulsva[] =
    {
        { Data, DnDirect, WORD, arithReg, 0, 0xC1C0, 0 }
    };

variant muluva[] =
    {
        { Data, DnDirect, WORD, arithReg, 0, 0xC0C0, 0 }
    };

variant nbcdva[] =
    {
        { DataAlt, 0, BYTE, oneOp, 0x4800, 0x4800, 0 }
    };

variant negva[] =
    {
        { DataAlt, 0, BWL, oneOp, 0x4400, 0x4440, 0x4480 }
    };

variant negxva[] =
    {
        { DataAlt, 0, BWL, oneOp, 0x4000, 0x4040, 0x4080 }
    };

variant nopva[] =
    {
        { 0, 0, 0, zeroOp, 0, 0x4E71, 0 }
    };

variant notva[] =
    {
        { DataAlt, 0, BWL, oneOp, 0x4600, 0x4640, 0x4680 }
    };

variant orva[] =
    {
        { Data, DnDirect, BWL, arithReg, 0x8000, 0x8040, 0x8080 },
        { DnDirect, MemAlt, BWL, arithAddr, 0x8100, 0x8140, 0x8180 },
        { Immediate, DataAlt, BWL, immedInst, 0x0000, 0x0040, 0x0080 }
    };

variant oriva[] =
    {
        { Immediate, DataAlt, BWL, immedInst, 0x0000, 0x0040, 0x0080 },
        { Immediate, CCRDirect, BYTE, immedToCCR, 0x003C, 0x003C, 0 },
        { Immediate, SRDirect, WORD, immedWord, 0, 0x007C, 0 }
    };

variant peava[] =
    {
        { Control, 0, LONG, oneOp, 0, 0x4840, 0x4840 }
    };

variant resetva[] =
    {
        { 0, 0, 0, zeroOp, 0, 0x4E70, 0 }
    };

variant rolva[] =
    {
        { MemAlt, 0, WORD, oneOp, 0, 0xE7C0, 0 },
        { DnDirect, DnDirect, BWL, shiftReg, 0xE138, 0xE178, 0xE1B8 },
        { Immediate, DnDirect, BWL, shiftReg, 0xE118, 0xE158, 0xE198 }
    };

variant rorva[] =
    {
        { MemAlt, 0, WORD, oneOp, 0, 0xE6C0, 0 },
        { DnDirect, DnDirect, BWL, shiftReg, 0xE038, 0xE078, 0xE0B8 },
        { Immediate, DnDirect, BWL, shiftReg, 0xE018, 0xE058, 0xE098 }
    };

variant roxlva[] =
    {
        { MemAlt, 0, WORD, oneOp, 0, 0xE5C0, 0 },
        { DnDirect, DnDirect, BWL, shiftReg, 0xE130, 0xE170, 0xE1B0 },
        { Immediate, DnDirect, BWL, shiftReg, 0xE110, 0xE150, 0xE190 }
    };

variant roxrva[] =
    {
        { MemAlt, 0, WORD, oneOp, 0, 0xE4C0, 0 },
        { DnDirect, DnDirect, BWL, shiftReg, 0xE030, 0xE070, 0xE0B0 },
        { Immediate, DnDirect, BWL, shiftReg, 0xE010, 0xE050, 0xE090 }
    };

variant rtdva[] =
    {
        { Immediate, 0, 0, immedWord, 0, 0x4E74, 0 }
    };

variant rteva[] =
    {
        { 0, 0, 0, zeroOp, 0, 0x4E73, 0 }
    };

variant rtrva[] =
    {
        { 0, 0, 0, zeroOp, 0, 0x4E77, 0 }
    };

variant rtsva[] =
    {
        { 0, 0, 0, zeroOp, 0, 0x4E75, 0 }
    };

variant sbcdva[] =
    {
        { DnDirect, DnDirect, BYTE, twoReg, 0x8100, 0x8100, 0 },
        { AnIndPre, AnIndPre, BYTE, twoReg, 0x8108, 0x8108, 0 }
    };

variant sccva[] =
    {
        { DataAlt, 0, BYTE, scc, 0x54C0, 0x54C0, 0 }
    };

variant scsva[] =
    {
        { DataAlt, 0, BYTE, scc, 0x55C0, 0x55C0, 0 }
    };

variant seqva[] =
    {
        { DataAlt, 0, BYTE, scc, 0x57C0, 0x57C0, 0 }
    };

variant sfva[] =
    {
        { DataAlt, 0, BYTE, scc, 0x51C0, 0x51C0, 0 }
    };

variant sgeva[] =
    {
        { DataAlt, 0, BYTE, scc, 0x5CC0, 0x5CC0, 0 }
    };

variant sgtva[] =
    {
        { DataAlt, 0, BYTE, scc, 0x5EC0, 0x5EC0, 0 }
    };

variant shiva[] =
    {
        { DataAlt, 0, BYTE, scc, 0x52C0, 0x52C0, 0 }
    };

variant shsva[] =
    {
        { DataAlt, 0, BYTE, scc, 0x54C0, 0x54C0, 0 }
    };

variant sleva[] =
    {
        { DataAlt, 0, BYTE, scc, 0x5FC0, 0x5FC0, 0 }
    };

variant slova[] =
    {
        { DataAlt, 0, BYTE, scc, 0x55C0, 0x55C0, 0 }
    };

variant slsva[] =
    {
        { DataAlt, 0, BYTE, scc, 0x53C0, 0x53C0, 0 }
    };

variant sltva[] =
    {
        { DataAlt, 0, BYTE, scc, 0x5DC0, 0x5DC0, 0 }
    };

variant smiva[] =
    {
        { DataAlt, 0, BYTE, scc, 0x5BC0, 0x5BC0, 0 }
    };

variant sneva[] =
    {
        { DataAlt, 0, BYTE, scc, 0x56C0, 0x56C0, 0 }
    };

variant splva[] =
    {
        { DataAlt, 0, BYTE, scc, 0x5AC0, 0x5AC0, 0 }
    };

variant stva[] =
    {
        { DataAlt, 0, BYTE, scc, 0x50C0, 0x50C0, 0 }
    };

variant stopva[] =
    {
        { Immediate, 0, 0, immedWord, 0, 0x4E72, 0 }
    };

variant subva[] =
    {
        { Immediate, DataAlt,  BWL, immedInst, 0x0400, 0x0440, 0x0480 },
        { Immediate, AnDirect, WL,  quickMath, 0,      0x5140, 0x5180 },
        { All,       DnDirect, BWL, arithReg,  0x9000, 0x9040, 0x9080 },
        { DnDirect,  MemAlt,   BWL, arithAddr, 0x9100, 0x9140, 0x9180 },
        { All,       AnDirect, WL,  arithReg,  0,      0x90C0, 0x91C0 },
    };

variant subava[] =
    {
        { All, AnDirect, WL, arithReg, 0, 0x90C0, 0x91C0 }
    };

variant subiva[] =
    {
        { Immediate, DataAlt, BWL, immedInst, 0x0400, 0x0440, 0x0480 }
    };

variant subqva[] =
    {
        { Immediate, DataAlt, BWL, quickMath, 0x5100, 0x5140, 0x5180 },
        { Immediate, AnDirect, WL, quickMath, 0, 0x5140, 0x5180 }
    };

variant subxva[] =
    {
        { DnDirect, DnDirect, BWL, twoReg, 0x9100, 0x9140, 0x9180 },
        { AnIndPre, AnIndPre, BWL, twoReg, 0x9108, 0x9148, 0x9188 }
    };

variant svcva[] =
    {
        { DataAlt, 0, BYTE, scc, 0x58C0, 0x58C0, 0 }
    };

variant svsva[] =
    {
        { DataAlt, 0, BYTE, scc, 0x59C0, 0x59C0, 0 }
    };

variant swapva[] =
    {
        { DnDirect, 0, WORD, oneReg, 0, 0x4840, 0 }
    };

variant tasva[] =
    {
        { DataAlt, 0, BYTE, oneOp, 0x4AC0, 0x4AC0, 0 }
    };

variant trapva[] =
    {
        { Immediate, 0, 0, trap, 0, 0x4E40, 0 }
    };

variant trapccva[] =
    {
        { 0, 0, 0, NULL, 0x54fc, 0x54fa, 0x54fb }
    };

variant trapcsva[] =
    {
        { 0, 0, 0, NULL, 0x55fc, 0x55fa, 0x55fb }
    };

variant trapeqva[] =
    {
        { 0, 0, 0, NULL, 0x57fc, 0x57fa, 0x57fb }
    };

variant trapfva[] =
    {
        { 0, 0, 0, NULL, 0x51fc, 0x51fa, 0x51fb }
    };

variant trapgeva[] =
    {
        { 0, 0, 0, NULL, 0x5cfc, 0x5cfa, 0x5cfb }
    };

variant trapgtva[] =
    {
        { 0, 0, 0, NULL, 0x5efc, 0x5efa, 0x5efb }
    };

variant traphiva[] =
    {
        { 0, 0, 0, NULL, 0x52fc, 0x52fa, 0x52fb }
    };

variant trapleva[] =
    {
        { 0, 0, 0, NULL, 0x5ffc, 0x5ffa, 0x5ffb }
    };

variant traplsva[] =
    {
        { 0, 0, 0, NULL, 0x53fc, 0x53fa, 0x53fb }
    };

variant trapltva[] =
    {
        { 0, 0, 0, NULL, 0x5dfc, 0x5dfa, 0x5dfb }
    };

variant trapmiva[] =
    {
        { 0, 0, 0, NULL, 0x5bfc, 0x5bfa, 0x5bfb }
    };

variant trapneva[] =
    {
        { 0, 0, 0, NULL, 0x56fc, 0x56fa, 0x56fb }
    };

variant trapplva[] =
    {
        { 0, 0, 0, NULL, 0x5afc, 0x5afa, 0x5afb }
    };

variant traptva[] =
    {
        { 0, 0, 0, NULL, 0x50fc, 0x50fa, 0x50fb }
    };

variant trapvva[] =
    {
        { 0, 0, 0, zeroOp, 0, 0x4E76, 0 }
    };

variant trapvcva[] =
    {
        { 0, 0, 0, NULL, 0x58fc, 0x58fa, 0x58fb }
    };

variant trapvsva[] =
    {
        { 0, 0, 0, NULL, 0x59fc, 0x59fa, 0x59fb }
    };

variant tstva[] =
    {
        { DataAlt, 0, BWL, oneOp, 0x4A00, 0x4A40, 0x4A80 }
    };

variant unlkva[] =
    {
        { AnDirect, 0, 0, oneReg, 0, 0x4E58, 0 }
    };


/* Define a macro to compute the length of a variant list */

#define variantCount(variantArray) (sizeof(variantArray)/sizeof(variant))


/* The instruction table itself... */

instruction instTable[] =
    {
        { "ABCD",   abcdva,     variantCount(abcdva),   TRUE,   NULL },
        { "ADD",    addva,      variantCount(addva),    TRUE,   NULL },
        { "ADDA",   addava,     variantCount(addava),   TRUE,   NULL },
        { "ADDI",   addiva,     variantCount(addiva),   TRUE,   NULL },
        { "ADDQ",   addqva,     variantCount(addqva),   TRUE,   NULL },
        { "ADDX",   addxva,     variantCount(addxva),   TRUE,   NULL },
        { "AND",    andva,      variantCount(andva),    TRUE,   NULL },
        { "ANDI",   andiva,     variantCount(andiva),   TRUE,   NULL },
        { "ASL",    aslva,      variantCount(aslva),    TRUE,   NULL },
        { "ASR",    asrva,      variantCount(asrva),    TRUE,   NULL },
        { "BCC",    bccva,      variantCount(bccva),    TRUE,   NULL },
        { "BCHG",   bchgva,     variantCount(bchgva),   TRUE,   NULL },
        { "BCLR",   bclrva,     variantCount(bclrva),   TRUE,   NULL },
        { "BCS",    bcsva,      variantCount(bcsva),    TRUE,   NULL },
        { "BEQ",    beqva,      variantCount(beqva),    TRUE,   NULL },
        { "BFCHG",  bfchgva,    variantCount(bfchgva),  FALSE,  bitField },
        { "BFCLR",  bfclrva,    variantCount(bfclrva),  FALSE,  bitField },
        { "BFEXTS", bfextsva,   variantCount(bfextsva), FALSE,  bitField },
        { "BFEXTU", bfextuva,   variantCount(bfextuva), FALSE,  bitField },
        { "BFFFO",  bfffova,    variantCount(bfffova),  FALSE,  bitField },
        { "BFINS",  bfinsva,    variantCount(bfinsva),  FALSE,  bitField },
        { "BFSET",  bfsetva,    variantCount(bfsetva),  FALSE,  bitField },
        { "BFTST",  bftstva,    variantCount(bftstva),  FALSE,  bitField },
        { "BGE",    bgeva,      variantCount(bgeva),    TRUE,   NULL },
        { "BGT",    bgtva,      variantCount(bgtva),    TRUE,   NULL },
        { "BHI",    bhiva,      variantCount(bhiva),    TRUE,   NULL },
        { "BHS",    bccva,      variantCount(bccva),    TRUE,   NULL },
        { "BKPT",   NULL,       0,                      FALSE,  bkPoint },
        { "BLE",    bleva,      variantCount(bleva),    TRUE,   NULL },
        { "BLO",    bcsva,      variantCount(bcsva),    TRUE,   NULL },
        { "BLS",    blsva,      variantCount(blsva),    TRUE,   NULL },
        { "BLT",    bltva,      variantCount(bltva),    TRUE,   NULL },
        { "BMI",    bmiva,      variantCount(bmiva),    TRUE,   NULL },
        { "BNE",    bneva,      variantCount(bneva),    TRUE,   NULL },
        { "BPL",    bplva,      variantCount(bplva),    TRUE,   NULL },
        { "BRA",    brava,      variantCount(brava),    TRUE,   NULL },
        { "BSET",   bsetva,     variantCount(bsetva),   TRUE,   NULL },
        { "BSR",    bsrva,      variantCount(bsrva),    TRUE,   NULL },
        { "BTST",   btstva,     variantCount(btstva),   TRUE,   NULL },
        { "BVC",    bvcva,      variantCount(bvcva),    TRUE,   NULL },
        { "BVS",    bvsva,      variantCount(bvsva),    TRUE,   NULL },
        { "CALLM",  NULL,       0,                      FALSE,  callModule },
        { "CAS",    NULL,       0,                      FALSE,  compSwap },
        { "CAS2",   NULL,       0,                      FALSE,  compSwap2 },
        { "CHK",    chkva,      variantCount(chkva),    TRUE,   NULL },
        { "CHK2",   NULL,       0,                      FALSE,  checkReg },
        { "CLR",    clrva,      variantCount(clrva),    TRUE,   NULL },
        { "CMP",    cmpva,      variantCount(cmpva),    TRUE,   NULL },
        { "CMP2",   NULL,       0,                      FALSE,  checkReg },
        { "CMPA",   cmpava,     variantCount(cmpava),   TRUE,   NULL },
        { "CMPI",   cmpiva,     variantCount(cmpiva),   TRUE,   NULL },
        { "CMPM",   cmpmva,     variantCount(cmpmva),   TRUE,   NULL },
        { "DBCC",   dbccva,     variantCount(dbccva),   TRUE,   NULL },
        { "DBCS",   dbcsva,     variantCount(dbcsva),   TRUE,   NULL },
        { "DBEQ",   dbeqva,     variantCount(dbeqva),   TRUE,   NULL },
        { "DBF",    dbfva,      variantCount(dbfva),    TRUE,   NULL },
        { "DBGE",   dbgeva,     variantCount(dbgeva),   TRUE,   NULL },
        { "DBGT",   dbgtva,     variantCount(dbgtva),   TRUE,   NULL },
        { "DBHI",   dbhiva,     variantCount(dbhiva),   TRUE,   NULL },
        { "DBHS",   dbccva,     variantCount(dbccva),   TRUE,   NULL },
        { "DBLE",   dbleva,     variantCount(dbleva),   TRUE,   NULL },
        { "DBLO",   dbcsva,     variantCount(dbcsva),   TRUE,   NULL },
        { "DBLS",   dblsva,     variantCount(dblsva),   TRUE,   NULL },
        { "DBLT",   dbltva,     variantCount(dbltva),   TRUE,   NULL },
        { "DBMI",   dbmiva,     variantCount(dbmiva),   TRUE,   NULL },
        { "DBNE",   dbneva,     variantCount(dbneva),   TRUE,   NULL },
        { "DBPL",   dbplva,     variantCount(dbplva),   TRUE,   NULL },
        { "DBRA",   dbrava,     variantCount(dbrava),   TRUE,   NULL },
        { "DBT",    dbtva,      variantCount(dbtva),    TRUE,   NULL },
        { "DBVC",   dbvcva,     variantCount(dbvcva),   TRUE,   NULL },
        { "DBVS",   dbvsva,     variantCount(dbvsva),   TRUE,   NULL },
        { "DC",     NULL,       0,                      FALSE,  dc   },
        { "DCB",    NULL,       0,                      FALSE,  dcb  },
        { "DIVS",   NULL,       0,                      FALSE,  divsop },
        { "DIVSL",  NULL,       0,                      FALSE,  divslop },
        { "DIVU",   NULL,       0,                      FALSE,  divuop },
        { "DIVUL",  NULL,       0,                      FALSE,  divulop },
        { "DS",     NULL,       0,                      FALSE,  ds   },
        { "END",    NULL,       0,                      FALSE,  funct_end },
        { "EOR",    eorva,      variantCount(eorva),    TRUE,   NULL },
        { "EORI",   eoriva,     variantCount(eoriva),   TRUE,   NULL },
        { "EQU",    NULL,       0,                      FALSE,  equ  },
        { "EXG",    exgva,      variantCount(exgva),    TRUE,   NULL },
        { "EXT",    extva,      variantCount(extva),    TRUE,   NULL },
        { "EXTB",   extbva,     variantCount(extbva),   TRUE,   NULL },
        { "ILLEGAL", illegalva, variantCount(illegalva),TRUE,   NULL },
        { "JMP",    jmpva,      variantCount(jmpva),    TRUE,   NULL },
        { "JSR",    jsrva,      variantCount(jsrva),    TRUE,   NULL },
        { "LEA",    leava,      variantCount(leava),    TRUE,   NULL },
        { "LINK",   linkva,     variantCount(linkva),   TRUE,   NULL },
        { "LSL",    lslva,      variantCount(lslva),    TRUE,   NULL },
        { "LSR",    lsrva,      variantCount(lsrva),    TRUE,   NULL },
        { "MOVE",   moveva,     variantCount(moveva),   TRUE,   NULL },
        { "MOVEA",  moveava,    variantCount(moveava),  TRUE,   NULL },
        { "MOVEC",  movecva,    variantCount(movecva),  TRUE,   NULL },
        { "MOVEM",  NULL,       0,                      FALSE,  movem},
        { "MOVEP",  movepva,    variantCount(movepva),  TRUE,   NULL },
        { "MOVEQ",  moveqva,    variantCount(moveqva),  TRUE,   NULL },
        { "MOVES",  movesva,    variantCount(movesva),  TRUE,   NULL },
        { "MULS",   mulsva,     variantCount(mulsva),   TRUE,   NULL },
        { "MULU",   muluva,     variantCount(muluva),   TRUE,   NULL },
        { "NBCD",   nbcdva,     variantCount(nbcdva),   TRUE,   NULL },
        { "NEG",    negva,      variantCount(negva),    TRUE,   NULL },
        { "NEGX",   negxva,     variantCount(negxva),   TRUE,   NULL },
        { "NOP",    nopva,      variantCount(nopva),    TRUE,   NULL },
        { "NOT",    notva,      variantCount(notva),    TRUE,   NULL },
        { "OR",     orva,       variantCount(orva),     TRUE,   NULL },
        { "ORG",    NULL,       0,                      FALSE,  org  },
        { "ORI",    oriva,      variantCount(oriva),    TRUE,   NULL },
        { "PACK",   NULL,       0,                      FALSE,  pack },
        { "PEA",    peava,      variantCount(peava),    TRUE,   NULL },
        { "REG",    NULL,       0,                      FALSE,  reg  },
        { "RESET",  resetva,    variantCount(resetva),  TRUE,   NULL },
        { "ROL",    rolva,      variantCount(rolva),    TRUE,   NULL },
        { "ROR",    rorva,      variantCount(rorva),    TRUE,   NULL },
        { "ROXL",   roxlva,     variantCount(roxlva),   TRUE,   NULL },
        { "ROXR",   roxrva,     variantCount(roxrva),   TRUE,   NULL },
        { "RTD",    rtdva,      variantCount(rtdva),    TRUE,   NULL },
        { "RTE",    rteva,      variantCount(rteva),    TRUE,   NULL },
        { "RTM",    NULL,       0,                      FALSE,  rtm  },
        { "RTR",    rtrva,      variantCount(rtrva),    TRUE,   NULL },
        { "RTS",    rtsva,      variantCount(rtsva),    TRUE,   NULL },
        { "SBCD",   sbcdva,     variantCount(sbcdva),   TRUE,   NULL },
        { "SCC",    sccva,      variantCount(sccva),    TRUE,   NULL },
        { "SCS",    scsva,      variantCount(scsva),    TRUE,   NULL },
        { "SEQ",    seqva,      variantCount(seqva),    TRUE,   NULL },
        { "SET",    NULL,       0,                      FALSE,  set  },
        { "SF",     sfva,       variantCount(sfva),     TRUE,   NULL },
        { "SGE",    sgeva,      variantCount(sgeva),    TRUE,   NULL },
        { "SGT",    sgtva,      variantCount(sgtva),    TRUE,   NULL },
        { "SHI",    shiva,      variantCount(shiva),    TRUE,   NULL },
        { "SHS",    sccva,      variantCount(sccva),    TRUE,   NULL },
        { "SLE",    sleva,      variantCount(sleva),    TRUE,   NULL },
        { "SLO",    scsva,      variantCount(scsva),    TRUE,   NULL },
        { "SLS",    slsva,      variantCount(slsva),    TRUE,   NULL },
        { "SLT",    sltva,      variantCount(sltva),    TRUE,   NULL },
        { "SMI",    smiva,      variantCount(smiva),    TRUE,   NULL },
        { "SNE",    sneva,      variantCount(sneva),    TRUE,   NULL },
        { "SPL",    splva,      variantCount(splva),    TRUE,   NULL },
        { "ST",     stva,       variantCount(stva),     TRUE,   NULL },
        { "STOP",   stopva,     variantCount(stopva),   TRUE,   NULL },
        { "SUB",    subva,      variantCount(subva),    TRUE,   NULL },
        { "SUBA",   subava,     variantCount(subava),   TRUE,   NULL },
        { "SUBI",   subiva,     variantCount(subiva),   TRUE,   NULL },
        { "SUBQ",   subqva,     variantCount(subqva),   TRUE,   NULL },
        { "SUBX",   subxva,     variantCount(subxva),   TRUE,   NULL },
        { "SVC",    svcva,      variantCount(svcva),    TRUE,   NULL },
        { "SVS",    svsva,      variantCount(svsva),    TRUE,   NULL },
        { "SWAP",   swapva,     variantCount(swapva),   TRUE,   NULL },
        { "TAS",    tasva,      variantCount(tasva),    TRUE,   NULL },
        { "TRAP",   trapva,     variantCount(trapva),   TRUE,   NULL },
        { "TRAPCC", trapccva,   variantCount(trapccva), FALSE,  trapcc },
        { "TRAPCS", trapcsva,   variantCount(trapcsva), FALSE,  trapcc },
        { "TRAPEQ", trapeqva,   variantCount(trapeqva), FALSE,  trapcc },
        { "TRAPF",  trapfva,    variantCount(trapfva),  FALSE,  trapcc },
        { "TRAPGE", trapgeva,   variantCount(trapgeva), FALSE,  trapcc },
        { "TRAPGT", trapgtva,   variantCount(trapgtva), FALSE,  trapcc },
        { "TRAPHI", traphiva,   variantCount(traphiva), FALSE,  trapcc },
        { "TRAPHS", trapccva,   variantCount(trapccva), FALSE,  trapcc },
        { "TRAPLE", trapleva,   variantCount(trapleva), FALSE,  trapcc },
        { "TRAPLO", trapcsva,   variantCount(trapcsva), FALSE,  trapcc },
        { "TRAPLS", traplsva,   variantCount(traplsva), FALSE,  trapcc },
        { "TRAPLT", trapltva,   variantCount(trapltva), FALSE,  trapcc },
        { "TRAPMI", trapmiva,   variantCount(trapmiva), FALSE,  trapcc },
        { "TRAPNE", trapneva,   variantCount(trapneva), FALSE,  trapcc },
        { "TRAPPL", trapplva,   variantCount(trapplva), FALSE,  trapcc },
        { "TRAPT",  traptva,    variantCount(traptva),  FALSE,  trapcc },
        { "TRAPV",  trapvva,    variantCount(trapvva),  TRUE,   NULL },
        { "TRAPVC", trapvcva,   variantCount(trapvcva), FALSE,  trapcc },
        { "TRAPVS", trapvsva,   variantCount(trapvsva), FALSE,  trapcc },
        { "TST",    tstva,      variantCount(tstva),    TRUE,   NULL },
        { "UNLK",   unlkva,     variantCount(unlkva),   TRUE,   NULL },
        { "UNPK",   NULL,       0,                      FALSE,  unpack }
    };


/* Declare a global variable containing the size of the instruction table */

short int tableSize = sizeof(instTable)/sizeof(instruction);

