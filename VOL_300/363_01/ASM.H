/***********************************************************************
 *
 *      ASM.H
 *      Global Definitions for 68000 Assembler
 *
 *      Author: Paul McKee
 *      ECE492    North Carolina State University
 *
 *        Date: 12/13/86
 *
 *      Modified: A.E. Romer
 *          April 1991: "Invalid label" error code added
 *          30 April 1991: Upgrade to 68020
 *          Autumn 1991: "Invalid quick constant" error code added
 *
 ************************************************************************/


/* include system header files for prototype checking */
#include <stdio.h>
#include <string.h>
#include <process.h>
#include <stdlib.h>
/* #include <malloc.h>      required for Microsoft, but not for
                                                        Zortech compiler */

#define     CHAR_DELIMITER      '\''
                    /* Character and string delimiter in data constants */
#define     LBUF_SIZE           256
                    /* size of input line buffer */

/* extension word bit values */
#define     XREG_IS_An      0x8000
#define     XREG_NUM(x)     (x) << 12
#define     XSIZE_LONG      0x0800
#define     SCALE_EIGHT     0x0600
#define     SCALE_FOUR      0x0400
#define     SCALE_TWO       0x0200
#define     FULL_FORMAT     0x0100
#define     BASE_SUPPRESS   0x0080
#define     X_SUPPRESS      0x0040
#define     BASE_LONG       0x0030
#define     BASE_WORD       0x0020
#define     BASE_NULL       0x0010
#define     POST_X          0x0004
#define     PRE_X           0x0000               /* included for symmetry */
#define     OUTER_LONG      0x0003
#define     OUTER_WORD      0x0002
#define     OUTER_NULL      0x0001

/* extension word bit masks */
#define     BASE            0x0030
#define     OUTER           0x0003

/* Status values */

/* These status values are 12 bits long with
   a severity code in the upper 4 bits */

#define     OK                  0x00

/* Severe errors */
#define     SEVERE              0x400
#define     SYNTAX              0x401
#define     INV_OPCODE          0x402
#define     INV_ADDR_MODE       0x403
#define     LABEL_REQUIRED      0x404
#define     PHASE_ERROR         0x405
#define     UNIMPLEMENTED       0x406
#define     INV_SHORT_BRANCH    0x407

/* Errors */
#define     ERROR               0x300
#define     UNDEFINED           0x301
#define     DIV_BY_ZERO         0x302
#define     MULTIPLE_DEFS       0x303
#define     REG_MULT_DEFS       0x304
#define     REG_LIST_UNDEF      0x305
#define     INV_FORWARD_REF     0x306
#define     INV_LENGTH          0x307

/* Minor errors */
#define     MINOR               0x200
#define     INV_SIZE_CODE       0x201
#define     INV_VECTOR_NUM      0x202
#define     INV_BRANCH_DISP     0x203
#define     INV_DISP            0x204
#define     INV_ABS_ADDRESS     0x205
#define     INV_3_BIT_DATA      0x206
#define     INV_8_BIT_DATA      0x207
#define     INV_16_BIT_DATA     0x208
#define     ODD_ADDRESS         0x209
#define     NOT_REG_LIST        0x20a
#define     REG_LIST_SPEC       0x20b
#define     INV_SHIFT_COUNT     0x20c
#define     INV_LABEL           0x20d

/* Warnings */
#define     WARNING             0x100
#define     ASCII_TOO_BIG       0x101
#define     NUMBER_TOO_BIG      0x102
#define     INCOMPLETE          0x103
#define     EXCESSIVE_SIZE      0x104
#define     UNSIZED             0x105
#define     IGNORED_SIZE        0x106
#define     CORRECTED_SIZE      0x107
#define     INV_QUICK_CONST     0x108
#define     INV_MOVEQ_CONST     0x109

#define     SEVERITY            0xf00

/* The NEWERROR macro updates the error variable var only if the
   new error code is more severe than all previous errors.  Throughout
   ASM this is the standard means of reporting errors. */

#define     NEWERROR(var, code) if ((code & SEVERITY) > var) var = code

/* Symbol table definitions */

/* Significant length of a symbol */
#define     SIGCHARS            8

/* Structure for operand descriptors */
typedef struct
    {
    long mode;                          /* addressing mode code (see below) */
    long data;        /* Immediate value, displacement, or absolute address */
    char reg;                                 /* Base register number (0-7) */
    char size;        /* Size of absolute address (WORD or LONG, see below) */
    int  xtenWord;             /* first extension word in indexed operation */
    int  bfXtenWord;                            /* bit field extension word */
    long outDisp;                     /* outer displacement, for 68020 only */
    char backRef;                      /* True if data is known on 1st pass */
    char odBackRef;      /* True if outer displacement is known on 1st pass */
    } opDescriptor;


/* Structure for a symbol table entry */
typedef struct symbolEntry
    {
    long int value;                           /* 32-bit value of the symbol */
    struct symbolEntry *next;      /* Pointer to next symbol in linked list */
    char flags;                                        /* Flags (see below) */
    char name[SIGCHARS+1];                                          /* Name */
    } symbolDef;


/* Flag values for the "flags" field of a symbol */

#define     BACKREF             0x01
                          /* Set when the symbol is defined on the 2nd pass
                           * so that the following references know the symbol
                           * has been defined */
#define     REDEFINABLE         0x02
                            /* Set for symbols defined by the SET directive */
#define     REG_LIST_SYM        0x04
                            /* Set for symbols defined by the REG directive */
#define     MULTDEF             0x08
                                    /* set if multiple definition attempted */


/* Instruction table definitions */

/* Structure to describe one "variant" of an instruction */

typedef struct
    {
    long int source;                   /* Bit masks for the legal source... */
    long int dest;                     /*  and destination addressing modes */
    char sizes;                             /* Bit mask for the legal sizes */
    int (*exec)(int, int, opDescriptor *, opDescriptor *, int *);
                             /* Pointer to routine to build the instruction */
    short int bytemask;      /* Skeleton instruction masks for byte size... */
    short int wordmask;                                  /*  word size, ... */
    short int longmask;               /*  and long sizes of the instruction */
    } variant;


/* Structure for the instruction table */
typedef struct INSTR
    {
    char *mnemonic;                                             /* Mnemonic */
    variant *variantPtr;                         /* Pointer to variant list */
    char variantCount;                /* Number of variants in variant list */
    char parseFlag;                /* Should assemble() parse the operands? */
    int (*exec)(struct INSTR *, int, char *, char *, int *);
                              /* Routine to be called if parseFlag is FALSE */
    } instruction;


/* Addressing mode codes/bitmasks */

#define     DnDirect            0x0000001
#define     AnDirect            0x0000002
#define     AnInd               0x0000004
#define     AnIndPost           0x0000008
#define     AnIndPre            0x0000010
#define     AnIndDisp           0x0000020
#define     AnIndIndex          0x0000040
#define     AbsShort            0x0000080
#define     AbsLong             0x0000100
#define     PCDisp              0x0000200
#define     PCIndex             0x0000400
#define     Immediate           0x0000800
#define     SRDirect            0x0001000
#define     CCRDirect           0x0002000
#define     USPDirect           0x0004000
#define     SFCDirect           0x0008000
#define     DFCDirect           0x0010000
#define     VBRDirect           0x0020000

/* Addressing mode codes/bitmasks specific to the 68020 only */

#define     CACRDirect          0x0040000
#define     CAARDirect          0x0080000
#define     MSPDirect           0x0100000
#define     ISPDirect           0x0200000
#define     MemIndPostinx       0x0400000
#define     MemIndPreinx        0x0800000
#define     PCMemIndPostinx     0x1000000
#define     PCMemIndPreinx      0x2000000

/* Definitions of addressing mode masks for various classes of references */

#define Data    (DnDirect | AnInd | AnIndPost | AnIndPre | AnIndDisp \
        | AnIndIndex | AbsShort | AbsLong | PCDisp | PCIndex | Immediate \
\
                /* additional modes specific to the 68020 only */ \
\
        | MemIndPostinx | MemIndPreinx | PCMemIndPostinx | PCMemIndPreinx)

#define Memory  (AnInd | AnIndPost | AnIndPre | AnIndDisp \
        | AnIndIndex | AbsShort | AbsLong | PCDisp | PCIndex | Immediate \
\
                /* additional modes specific to the 68020 only */ \
\
        | MemIndPostinx | MemIndPreinx | PCMemIndPostinx | PCMemIndPreinx)
\

#define Control (AnInd | AnIndDisp | AnIndIndex | AbsShort | AbsLong | PCDisp \
        | PCIndex \
\
                /* additional modes specific to the 68020 only */ \
\
        | MemIndPostinx | MemIndPreinx | PCMemIndPostinx | PCMemIndPreinx)


#define Alter   (DnDirect | AnDirect | AnInd | AnIndPost | AnIndPre | \
        AnIndDisp | AnIndIndex | AbsShort | AbsLong \
\
                /* additional modes specific to the 68020 only */ \
\
        | MemIndPostinx | MemIndPreinx)


#define All         (Data | Memory | Control | Alter)
#define DataAlt     (Data & Alter)
#define CtrAlt      (Control & Alter)
#define MemAlt      (Memory & Alter)
#define Absolute    (AbsLong | AbsShort)
#define GenReg      (DnDirect | AnDirect)


/* Register, absolute address,  and operation size codes/bitmasks */

#define     BYTE                ((int) 1)
#define     WORD                ((int) 2)
#define     LONG                ((int) 4)
#define     SHORT               ((int) 8)


/* added for PC port -- 7/8/1988 */

#define     TRUE                -1
#define     FALSE               0


/* function return codes */

#define     NORMAL              0



/* function prototype definitions */
#include    "proto.h"

/*************** end asm.h *******************/
