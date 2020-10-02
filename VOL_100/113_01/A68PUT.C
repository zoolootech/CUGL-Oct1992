/*
	HEADER:		CUG113;
	TITLE:		6800 Cross-Assembler (BDS C Version);
	FILENAME:	A68PUT.C;
	VERSION:	2.6;
	DATE:		07/22/1985;

	DESCRIPTION:	"This program lets you use your CP/M-80-based computer
			to assemble code for the Motorola 6800, 6801, 6802,
			6803, 6808, and 68701 microprocessors.  The program is
			written in BDS C for the best possible performance on
			8-bit machines.  All assembler features are supported
			except relocation, linkage, listing control, and
			macros.";

	KEYWORDS:	Software Development, Assemblers, Cross-Assemblers,
			Motorola, MC6800, MC6801;

	SEE-ALSO:	CUG149, 6801 Cross-Assembler (Portable);

	SYSTEM:		CP/M-80;
	COMPILERS:	BDS C;

	WARNINGS:	"This package is specifically tailored to CP/M-80
			machines and the rather non-standard, but high-
			performance BDS C compiler.  For other environments,
			use the portable version of this package on CUG149.";

	AUTHORS:	William C. Colley III;
*/

/*
	6800/6801 Cross-Assembler  v. 2.6

	May, 1980

	July, 1980 -- Rev. 2.2 consisting of fixing the M errors that
		come from forward references in FDB and FCB pseudo-ops.

	October, 1980 -- Rev. 2.3 consisting of updating the assembly
		language file and I/O routines to match and take
		advantage of BDS C V1.4.

	October, 1983 -- Rev. 2.4 consisting of adding the CPU pseudo-op,
		adding the 6801 CPU's extra opcodes, and speeding up the
		code a bit.

	September, 1984 -- Rev. 2.5 consisting of fixing bugs with the symbol
		table sort, the writing of files to specified drives, and the
		handling of blank input lines.

	June, 1985 -- Rev. 2.6 consisting of fixing a bug in the IF block
		nesting mechanism.

	Copyright (c) 1980,83,84,85 William C. Colley, III.

File:	a68put.c

List and hex output routines.
*/

/*  Get globals:  */

#include "a68.h"

/*
Function to form the list output line and put it to
the list device.  Routine also puts the line to the
console in the event of an error.
*/

lineout()
{
    char tbuf[25], *tptr, *bptr, count;

    if (list == NOFILE && errcode == ' ') return;
    setmem(tbuf,24,' ');  tbuf[24] = '\0';  tptr = tbuf;
    *tptr++ = errcode;  ++tptr;
    if (hexflg != NOCODE) { puthex4(address,&tptr);  tptr += 3; }
    else tptr += 7;
    count = 0;  bptr = binbuf;
    for (;;) {
	if (count == nbytes || (count && count % 4 == 0)) {
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
	if (count++ == nbytes) return;
	++address;  puthex2(*bptr++,&tptr);  tptr++;
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

	case NOMORE:	flshhbf(0);  fputs(":0000000000\n\032",hex);
			fflush(hex);  fclose(hex);  return;
    }
}

/*
Function to put a line of intel hex to the appropriate
device and get a new line started.
*/

flshhbf(loadaddr)
unsigned loadaddr;
{
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
    if (!number) return;
    putdec(number/10,lineptr);
    *(*lineptr)++ = number % 10 + '0';
}
,&hxlnptr);  *hxlnptr+