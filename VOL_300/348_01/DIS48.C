/*
**	Disassembler for 8048, 8041 microcode.
**
**	Copyright (c) 1990, Michael G. Panas
**
*/

#include <stdio.h>
#include <fcntl.h>

#include "dis48.h"

int	infd, pc, lincnt, pass, eoflg;
char	opcode, op2;
char	infile[80], outfile[80], labelbuf[10];
char	*lp, *label, *text, *operand, *tabzone, *hexdata;
FILE	*outfd;

char	*prog = "8048/8041 Disassembler\n";
char	*copy = "Copyright (c) 1990, Michael G. Panas\n";
char	*tab = "\t";
char	*nolabel = "";

struct	symbol	sym[SYMS];	/* the symbol table */
int	symcnt = 0;		/* number of symbols used */

#include "dis48tbl.c"

int findlabel(), dolabel(), addlabel();

main(argc, argv)
char **argv;
int argc;
{
	if (argc == 1) { /* no args */
		printf("%s%s", prog, copy);
		printf("Input filename: ");
		if (gets(infile) == NULL) {
			perror("Line error:");
			exit(1);
		}
		if (infile[0] == '\0')
			exit(0);
		printf("Output filename: ");
		if (gets(outfile) == NULL) {
			perror("Line error:");
			exit(1);
		}
		if (outfile[0] == '\0')
			exit(0);
	}
	else {
		strcpy(infile, argv[1]);
		strcpy(outfile, argv[2]);
	}
	
#ifdef MSC
	if ((infd = open(infile, O_RDONLY|O_BINARY)) == -1) {
#else
	if ((infd = open(infile, O_RDONLY)) == -1) {
#endif
		perror("Can't open input file:");
		exit(1);
	}
	if ((outfd = fopen(outfile, "w")) == NULL) {
		perror("Can't open output file:");
		exit(1);
	}
	
	prolog();			/* assembly startup stuff */

	pass = 0;
	while (pass++ < 2) {		/* do 2 passes */
		lseek(infd, 0, 0);	/* reset pointer */
		eoflg = 0;
		pc = 0;
		lincnt = 0;
		disasm();
	}
	
	cleansym();

	epilog();			/* "END" statement */

	printf("Disassembly done, %d lines\n", lincnt);
}

disasm()
{
	int address, len, origpc, nl;
	unsigned char index, oper;
	struct table *tb;
	char buf[40], operbuf[10];

	do {
		origpc = pc;
		nl = 0;

		index = (unsigned char) getbyte();
		if (eoflg) {
			break;
		}

		tb = &dis48[index];
		
		/* set up defaults */
		label = nolabel;
		text = tb->text;
		operand = tab;
		tabzone = tab;
		sprintf(&buf[0], ";%04X %02X", pc-1, index);
		hexdata = (char *) &buf[0];
		
		switch (tb->type) {

			case RET:
				nl = 1;
			case NOP:
			case DBC:
				break;

			case IMM:
				oper = (unsigned char) getbyte();
				sprintf(&operbuf[0], "0%02XH\t", oper);
				operand = &operbuf[0];
				sprintf(&buf[strlen(&buf[0])], " %02X", oper);
				if ((tb->width + 4) >= 8)
					tabzone = nolabel;
				break;

			case JMP:
				nl = 1;
			case CAL:
				oper = (unsigned char) getbyte();
				sprintf(&buf[strlen(&buf[0])], " %02X", oper);
				address = ((index&0xe0) <<3) | oper;
				sprintf(&operbuf[0], "L%04X\t", address);
				operand = &operbuf[0];
				addlabel(address);
				break;

			case BRA:
				oper = (unsigned char) getbyte();
				sprintf(&buf[strlen(&buf[0])], " %02X", oper);
				address = ((pc-2) & 0xff00) | oper;
				sprintf(&operbuf[0], "L%04X\t", address);
				operand = &operbuf[0];
				addlabel(address);
				if ((tb->width + 5) >= 8)
					tabzone = nolabel;
				break;

		}


		if (pass == 2) {
			if (findlabel(origpc))
				dolabel(origpc);
			putline();
			if (nl)
				fprintf(outfd, "\n");
		}
	}
	while (eoflg == 0);
}

/*
** read the next byte from the input file
*/
getbyte()
{
	char byte;
	
	if(read(infd, &byte, 1) == 0) {
		eoflg = 1;
	}
	pc++;
	return(byte);
}

putline()
{
	fprintf(outfd, "%s%s%s%s%s\n", label, text, operand, tabzone, hexdata);
	
	lincnt++;
}

addlabel(address)
int address;
{
	int i;

	if (findlabel(address))
		return;

	if ((i = symcnt++) == SYMS) {
		printf("Symbol table overflow!\n");
		exit(1);
	}
	
	sym[i].flags |= USED;
	sym[i].address = address;
}

doref(address)
int address;
{
	int i;

	i = findlabel(address) - 1;

	fprintf(outfd, "L%04X\tEQU\t%04XH\n", address, address);
	sym[i].flags |= PRINT;
}

dolabel(address)
int address;
{
	int i;

	i = findlabel(address) - 1;

	sprintf(&labelbuf[0], "L%04X", address);
	label = &labelbuf[0];
	sym[i].flags |= PRINT;
}

int
findlabel(address)
int address;
{
	int i;
	for (i = 0; i <= symcnt; i++) {
		if (address == sym[i].address) {
			return(i+1);
		}
	}
	return(0);
}

cleansym()
{
	int i, k;


	for (i = 0, k = 0; i <= symcnt; i++) {
		if ((sym[i].flags & PRINT) == 0) {
			k++;
		}
	}

	if (k == 0)
		return;

	fprintf(outfd, "\n");
	fprintf(outfd, ";	The following %d symbols are not within\n", k);
	fprintf(outfd, ";	the address range of this file or are not\n");
	fprintf(outfd, ";	on an instruction boundary.\n");

	for (i = 0; i <= symcnt; i++) {
		if ((sym[i].flags & PRINT) == 0) {
			doref(sym[i].address);
		}
	}
}

prolog()
{
	fprintf(outfd, "\tORG\t0\n");
	lincnt++;
}

epilog()
{
	fprintf(outfd, "\tEND\n");
	lincnt++;
}

