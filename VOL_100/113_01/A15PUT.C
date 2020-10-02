/*
	HEADER:		CUG113;
	TITLE:		1802 Cross-Assembler (BDS C Version);
	FILENAME:	A15PUT.C;
	VERSION:	1.2;
	DATE:		07/22/1985;

	DESCRIPTION:	"This program lets you use your CP/M-80-based computer
			to assemble code for the RCA 1802, 1804, 1805, 1805A,
			1806, AND 1806A microprocessors.  The program is
			written in BDS C for the best possible performance on
			8-bit machines.  All assembler features are supported
			except relocation, linkage, listing control, and
			macros.";

	KEYWORDS:	Software Development, Assemblers, Cross-Assemblers,
			RCA, CDP1802, CDP1805A;

	SEE-ALSO:	CUG149, 1805A Cross-Assembler (Portable);

	SYSTEM:		CP/M-80;
	COMPILERS:	BDS C;

	WARNINGS:	"This package is specifically tailored to CP/M-80
			machines and the rather non-standard, but high-
			performance BDS C compiler.  For other environments,
			use the portable version of this package on CUG149.";

	AUTHORS:	William C. Colley III;
*/

/*
	1805A Cross-Assembler  v 1.2

	Copyright (c) 1980, 82, 83, 85 William C. Colley, III.

	July 1982 -- Adapted from my 1802 cross-assembler.  WCC3.

	Vers 1.0 -- March 1983 -- Added 1805A opcodes to the 1805 set.  WCC3.

	Vers 1.1 -- March 1983 -- Added CPU pseudo-op to combine 1802 and 1805A
			cross-assemblers into a single program.  WCC3.

	Vers 1.2 -- June 1985 -- Fixed IF block nesting mechanism bug and bug
			in 1805A SCAL opcode.  WCC3.

File:	a15put.c

List and hex output routines.
*/

/*  Get globals:  */

#include "a15.h"

/*
Function to form the list output line and put it to the list device.  Routine
also puts the line to the console in the event of an error.
*/

lineout()
{
    char tbuf[25], *tptr, *bptr, count;

    if (list == NOFILE && errcode == ' ') return;
    setmem(tbuf,24,' ');  tbuf[24] = '\0';
    tptr = tbuf;  *tptr++ = errcode;  tptr++;
    if (hexflg != NOCODE) puthex4(address,&tptr);
    else tptr += 4;
    tptr += 3;  count = 0;  bptr = binbuf;

    for (;;) {
	if (count == nbytes || count != 0 && count % 4 == 0) {
	    if (list != NOFILE) {
		fputs(tbuf,list);
		if (count > 4) fputs("\n",list);
		else fputs(linbuf,list);
	    }
	    if (list != CONO && errcode != ' ') {
		puts(tbuf);
		if (count >= 4) putchar('\n');
		else puts(linbuf);
	    }
	    tptr = tbuf + 2;  puthex4(address,&tptr);
	    setmem(tptr,14,' ');  tptr += 3;
	}
	if (count == nbytes) return;
	count++;  address++;  puthex2(*bptr++,&tptr);  tptr++;
    }
}

/*
Function to form the hex output line and put it to
the hex output device.
*/

hexout()
{
    char count, *bptr;

    if (hex == NOFILE) return;

    switch (hexflg) {
	case PUTCODE:	bptr =binbuf;
			for (count = 1; count <= nbytes; count++) {
				puthex2(*bptr,&hxlnptr);  chksum += *bptr++;
				if (++hxbytes == 16) flshhbf(pc+count);
			}

	case NOCODE:	return;

	case FLUSH:	flshhbf(pc);  return;

	case NOMORE:	flshhbf(0);  fputs(":0000000000\n",hex);
			if (hex >= LODISK) putc(CPMEOF,hex);
			else if (hex == LST) putc('\f',hex);
			fflush(hex);  fclose(hex);
    }
}

/*
Function to put a line of intel hex to the appropriate
device and get a new line started.
*/

flshhbf(loadaddr)
unsigned loadaddr;
{
    if (hex == NOFILE) return;
    if (hxbytes != 0) {
	puthex2(-(chksum+hxbytes),&hxlnptr);  *hxlnptr++ = '\n';
	*hxlnptr++ = '\0';  hxlnptr = hxlnbuf + 1;
	puthex2(hxbytes,&hxlnptr);  fputs(hxlnbuf,hex);
    }
    hxbytes = 0;  hxlnptr = hxlnbuf;  *hxlnptr++ = ':';
    hxlnptr += 2;  puthex4(loadaddr,&hxlnptr);  puthex2(0,&hxlnptr);
    chksum = (loadaddr >> 8) + (loadaddr & 0xff);
}

/*
Function to put a 4-digit hex number into an output line.
*/

puthex4(number,lineptr)
unsigned number;
char **lineptr;
{
    puthex2(number>>8,lineptr);  puthex2(number,lineptr);
}

/*
Function to put a 2-digit hex number into an output line.
*/

puthex2(number,lineptr)
char number, **lineptr;
{
    if ((**lineptr = (number >> 4) + '0') > '9') **lineptr += 7;
    if ((*++*lineptr = (number & 0xf) + '0') > '9') **lineptr += 7;
    ++(*lineptr);
}

/*
Function to put a decimal number into an output line.
*/

putdec(number,lineptr)
unsigned number;
char **lineptr;
{
    if (number == 0) return;
    putdec(number/10,lineptr);  *(*lineptr)++ = number % 10 + '0';
}
,&hxlnptr);  *hxlnptr++ = '\n';
	*hxlnptr++ = '\0';  hxlnptr = hxlnbuf + 1;
	puthex2(hxbytes,&hxlnptr);  fputs(hxlnbuf,