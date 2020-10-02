/*
	************************************************************
			 ACRL 8080 Code Generator
	************************************************************

	W. Lemiszki					  9 Jan 1982

	Filename: a8080.c				 BDS C v1.50
*/

#include "acrl.h"

#define iZOP	1		/* MACHINE types */
#define iREG	2
#define iREGL	3
#define iREGP	4
#define iIMM	5
#define iMVI	6
#define iLXI	7
#define iADR	8
#define iMOV	9
#define iXRP	10


/*
 *	Parse an 8080 instruction
 *	-------------------------
 */
instruction()
{
	byte ty;
	byte opcode;

	ty = type;				/* instruction type (above) */
	opcode = value;				/* unmodified opcode */
	scan();					/* 1st operand token */

	switch (ty)
	    {
	    case (iZOP):			/* No operand instructions */
		code(opcode);
		break;

	    case (iREG):			/* Register in bits 0-2 */
		code(opcode + getreg(0));
		break;

	    case (iREGL):			/* Register in bits 3-5 */
		code(opcode + getreg(1));
		break;

	    case (iREGP):			/* Register pair operand */
		code(opcode + getreg(2));
		break;



	    case (iIMM):			/* 8 bit Immediate operand */
		code(opcode);
		code(expr8());
		break;

	    case (iMVI):			/* MVI instruction */
		code(opcode + getreg(1));
		eatcomma();
		code(expr8());
		break;

	    case (iLXI):			/* LXI instruction */
		code(opcode + getreg(2));
		eatcomma();
		codew(expr16());
		break;

	    case (iADR):			/* Address operand */
		code(opcode);
		codew(expr16());
		break;

	    case (iMOV):			/* MOV instruction */
		opcode += getreg(1);
		eatcomma();
		code(opcode += getreg(0));
		if (opcode == 0x76)			/* hlt */
			error ('R');
		break;

	    case (iXRP):			/* LDAX or STAX instruction */
		code(opcode + getreg(3));
		break;
	    }
}



/* Eat a comma */
eatcomma()
{
	if (type != ',')
		error('Q');
	scan();					/* and scan next token */
}




/*
 *	Scan a register
 *	---------------
 *	One of four actions based an arg:
 *	0:  return reg in bits 0-2
 *	1:  return reg in bits 3-5
 *	2:  return even reg in bits 3-5	   (b,d,h,sp)
 *	3:  return b or d in bits 3-5
 */
byte getreg(arg)
byte arg;
{
	byte reg;

	if ((reg = expr8()) >= 8)
		error('R');

	switch (arg)
	    {
	    case 3:
		if (reg & ~2)
			error('R');
	    case 2:
		if (reg & 1)
			error('R');
	    case 1:
		return (reg << 3);

	    case 0:
		return (reg);
	    }
}




/*
 *	Define the Mnemonics
 *	--------------------
 */
init8080()
{
	install(RESVD,	"XCHG",	MACHINE, iZOP,	0xEB);
	install(RESVD,	"DAA",	MACHINE, iZOP,	0x27);
	install(RESVD,	"CMA",	MACHINE, iZOP,	0x2F);
	install(RESVD,	"STC",	MACHINE, iZOP,	0x37);
	install(RESVD,	"CMC",	MACHINE, iZOP,	0x3F);
	install(RESVD,	"RLC",	MACHINE, iZOP,	0x07);
	install(RESVD,	"RRC",	MACHINE, iZOP,	0x0F);
	install(RESVD,	"RAL",	MACHINE, iZOP,	0x17);
	install(RESVD,	"RAR",	MACHINE, iZOP,	0x1F);
	install(RESVD,	"RET",	MACHINE, iZOP,	0xC9);
	install(RESVD,	"RNZ",	MACHINE, iZOP,	0xC0);
	install(RESVD,	"RZ",	MACHINE, iZOP,	0xC8);
	install(RESVD,	"RNC",	MACHINE, iZOP,	0xD0);
	install(RESVD,	"RC",	MACHINE, iZOP,	0xD8);
	install(RESVD,	"RPO",	MACHINE, iZOP,	0xE0);
	install(RESVD,	"RPE",	MACHINE, iZOP,	0xE8);
	install(RESVD,	"RP",	MACHINE, iZOP,	0xF0);
	install(RESVD,	"RM",	MACHINE, iZOP,	0xF8);
	install(RESVD,	"XTHL",	MACHINE, iZOP,	0xE3);
	install(RESVD,	"SPHL",	MACHINE, iZOP,	0xF9);
	install(RESVD,	"PCHL",	MACHINE, iZOP,	0xE9);
	install(RESVD,	"EI",	MACHINE, iZOP,	0xFB);
	install(RESVD,	"DI",	MACHINE, iZOP,	0xF3);
	install(RESVD,	"NOP",	MACHINE, iZOP,	0x00);
	install(RESVD,	"HLT",	MACHINE, iZOP,	0x76);
	install(RESVD,	"RIM",	MACHINE, iZOP,	0x20);
	install(RESVD,	"SIM",	MACHINE, iZOP,	0x30);

	install(RESVD,	"ADD",	MACHINE, iREG,	0x80);
	install(RESVD,	"ADC",	MACHINE, iREG,	0x88);
	install(RESVD,	"SUB",	MACHINE, iREG,	0x90);
	install(RESVD,	"SBB",	MACHINE, iREG,	0x98);
	install(RESVD,	"ANA",	MACHINE, iREG,	0xA0);
	install(RESVD,	"XRA",	MACHINE, iREG,	0xA8);
	install(RESVD,	"ORA",	MACHINE, iREG,	0xB0);
	install(RESVD,	"CMP",	MACHINE, iREG,	0xB8);

	install(RESVD,	"INR",	MACHINE, iREGL,	0x04);
	install(RESVD,	"DCR",	MACHINE, iREGL,	0x05);
	install(RESVD,	"RST",	MACHINE, iREGL,	0xC7);

	install(RESVD,	"DAD",	MACHINE, iREGP,	0x09);
	install(RESVD,	"INX",	MACHINE, iREGP,	0x03);
	install(RESVD,	"DCX",	MACHINE, iREGP,	0x0B);
	install(RESVD,	"PUSH",	MACHINE, iREGP,	0xC5);
	install(RESVD,	"POP",	MACHINE, iREGP,	0xC1);


	install(RESVD,	"ADI",	MACHINE, iIMM,	0xC6);
	install(RESVD,	"ACI",	MACHINE, iIMM,	0xCE);
	install(RESVD,	"SUI",	MACHINE, iIMM,	0xD6);
	install(RESVD,	"SBI",	MACHINE, iIMM,	0xDE);
	install(RESVD,	"ANI",	MACHINE, iIMM,	0xE6);
	install(RESVD,	"XRI",	MACHINE, iIMM,	0xEE);
	install(RESVD,	"ORI",	MACHINE, iIMM,	0xF6);
	install(RESVD,	"CPI",	MACHINE, iIMM,	0xFE);
	install(RESVD,	"IN",	MACHINE, iIMM,	0xDB);
	install(RESVD,	"OUT",	MACHINE, iIMM,	0xD3);

	install(RESVD,	"MVI",	MACHINE, iMVI,	0x06);

	install(RESVD,	"LXI",	MACHINE, iLXI,	0x01);

	install(RESVD,	"JMP",	MACHINE, iADR,	0xC3);
	install(RESVD,	"JNZ",	MACHINE, iADR,	0xC2);
	install(RESVD,	"JZ",	MACHINE, iADR,	0xCA);
	install(RESVD,	"JNC",	MACHINE, iADR,	0xD2);
	install(RESVD,	"JC",	MACHINE, iADR,	0xDA);
	install(RESVD,	"JPO",	MACHINE, iADR,	0xE2);
	install(RESVD,	"JPE",	MACHINE, iADR,	0xEA);
	install(RESVD,	"JP",	MACHINE, iADR,	0xF2);
	install(RESVD,	"JM",	MACHINE, iADR,	0xFA);
	install(RESVD,	"CALL",	MACHINE, iADR,	0xCD);
	install(RESVD,	"CNZ",	MACHINE, iADR,	0xC4);
	install(RESVD,	"CZ",	MACHINE, iADR,	0xCC);
	install(RESVD,	"CNC",	MACHINE, iADR,	0xD4);
	install(RESVD,	"CC",	MACHINE, iADR,	0xDC);
	install(RESVD,	"CPO",	MACHINE, iADR,	0xE4);
	install(RESVD,	"CPE",	MACHINE, iADR,	0xEC);
	install(RESVD,	"CP",	MACHINE, iADR,	0xF4);
	install(RESVD,	"CM",	MACHINE, iADR,	0xFC);
	install(RESVD,	"LDA",	MACHINE, iADR,	0x3A);
	install(RESVD,	"STA",	MACHINE, iADR,	0x32);
	install(RESVD,	"LHLD",	MACHINE, iADR,	0x2A);
	install(RESVD,	"SHLD",	MACHINE, iADR,	0x22);

	install(RESVD,	"MOV",	MACHINE, iMOV,	0x40);

	install(RESVD,	"LDAX",	MACHINE, iXRP,	0x0A);
	install(RESVD,	"STAX",	MACHINE, iXRP,	0x02);

	install(RESVD,	"A",	IDENT,   idEQU,	7);
	install(RESVD,	"B",	IDENT,   idEQU,	0);
	install(RESVD,	"C",	IDENT,   idEQU,	1);
	install(RESVD,	"D",	IDENT,   idEQU,	2);
	install(RESVD,	"E",	IDENT,   idEQU,	3);
	install(RESVD,	"H",	IDENT,   idEQU,	4);
	install(RESVD,	"L",	IDENT,   idEQU,	5);
	install(RESVD,	"M",	IDENT,   idEQU,	6);
	install(RESVD,	"SP",	IDENT,   idEQU,	6);
	install(RESVD,	"PSW",	IDENT,   idEQU,	6);
}

/*EOF*/
