/***********************************************************************
 *
 *      PROTO.H
 *      Global Definitions for 68000 Assembler
 *
 *      Author: Paul McKee
 *      ECE492    North Carolina State University
 *
 *        Date: 9/5/88
 *
 *      Modified: A.E. Romer, Version 1.0
 *          April 1991: Upgrade to 68020
 *
 ************************************************************************/

/* ANSI C function prototype definitions */

int processFile(void);

int assemble(char *, int *);

int move(int, int, opDescriptor *, opDescriptor *, int *);

int zeroOp(int, int, opDescriptor *, opDescriptor *, int *);

int oneOp(int, int, opDescriptor *, opDescriptor *, int *);

int arithReg(int, int, opDescriptor *, opDescriptor *, int *);

int arithAddr(int, int, opDescriptor *, opDescriptor *, int *);

int immedInst(int, int, opDescriptor *, opDescriptor *, int *);

int quickMath(int, int, opDescriptor *, opDescriptor *, int *);

int movep(int, int, opDescriptor *, opDescriptor *, int *);

int moves(int, int, opDescriptor *, opDescriptor *, int *);

int moveReg(int, int, opDescriptor *, opDescriptor *, int *);

int staticBit(int, int, opDescriptor *, opDescriptor *, int *);

int movec(int, int, opDescriptor *, opDescriptor *, int *);

int trap(int, int, opDescriptor *, opDescriptor *, int *);

int branch(int, int, opDescriptor *, opDescriptor *, int *);

int moveq(int, int, opDescriptor *, opDescriptor *, int *);

int immedToCCR(int, int, opDescriptor *, opDescriptor *, int *);

int immedWord(int, int, opDescriptor *, opDescriptor *, int *);

int dbcc(int, int, opDescriptor *, opDescriptor *, int *);

int scc(int, int, opDescriptor *, opDescriptor *, int *);

int shiftReg(int, int, opDescriptor *, opDescriptor *, int *);

int exg(int, int, opDescriptor *, opDescriptor *, int *);

int twoReg(int, int, opDescriptor *, opDescriptor *, int *);

int oneReg(int, int, opDescriptor *, opDescriptor *, int *);

int moveUSP(int, int, opDescriptor *, opDescriptor *, int *);

int link(int, int, opDescriptor *, opDescriptor *, int *);

int output(long, int);

int effAddr(opDescriptor *);

int extWords(opDescriptor *, int, int *);

int org(instruction *, int, char *, char *, int *);

int funct_end(instruction *, int, char *, char *, int *);

int equ(instruction *, int, char *, char *, int *);

int set(instruction *, int, char *, char *, int *);

int dc(instruction *, int, char *, char *, int *);

int dcb(instruction *, int, char *, char *, int *);

int ds(instruction *,int, char *, char *, int *);

int printError(FILE *, int, int);

char *eval(char *, long *, char *, int *);

char *instLookup(char *, instruction *(*), char *, int *);

int initList(char *);

int listLine(void);

int listLoc(void);

int listObj(long, int);

int main(int, char *[]);

int movem(instruction *, int, char *, char *, int *);

int reg(instruction *, int, char *, char *, int *);

int initObj(char *);

int outputObj(long, long, int);

long checkValue(long);

int finishObj(void);

char *opParse(char *, opDescriptor *, int *);

symbolDef *create(char *, long, int *);

symbolDef *lookup(char *, int *);

int writeObj(void);

void hashInit(void);

int noOp(int, int, opDescriptor *, opDescriptor *, int *);

int bitField(instruction *, int, char *, char *, int *);

int bkPoint(instruction *, int, char *, char *, int *);

int callModule(instruction *, int, char *, char *, int *);

int trapcc(instruction *, int, char *, char *, int *);

int compSwap(instruction *, int, char *, char *, int *);

int compSwap2(instruction *, int, char *, char *, int *);

int checkReg(instruction *, int, char *, char *, int *);

int divsop(instruction *, int, char *, char *, int *);

int divslop(instruction *, int, char *, char *, int *);

int divuop(instruction *, int, char *, char *, int *);

int divulop(instruction *, int, char *, char *, int *);

int pack(instruction *, int, char *, char *, int *);

int rtm(instruction *, int, char *, char *, int *);

int unpack(instruction *, int, char *, char *, int *);

/*************** end proto.h *******************/
