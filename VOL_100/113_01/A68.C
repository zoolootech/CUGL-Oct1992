/*
	HEADER:		CUG113;
	TITLE:		6800 Cross-Assembler (BDS C Version);
	FILENAME:	A68.C;
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

File:	a68.c

It all begins here.
*/

/*  Get globals:  */

#include "a68.h"

/*  The assembler starts here.  */

main(argc,argv)
int argc;
char *argv[];
{
    int n, m;

    puts("6800/6801 Cross-Assembler  v. 2.6\n");
    puts("   Copyright (c) 1980,83,84,85 William C. Colley, III\n");

    curdrive = bdos(GETDISK);	/*  Initialize file system.		*/
    setfiles(argc,argv);

    sympoint = symtbl;		/*  Initialize working areas.		*/
    symend = symtbl[SYMBOLS].symname;
    setmem(sympoint,(SYMBOLS * (SYMLEN + 2)),'\0');
    ifstack[ifsp = pc = 0] = pass = 1;
    flshhbf(extend = errcount = hxbytes = 0);
    puts("\nEntering Pass 1.\n");

    while (pass != 3)		/*  The actual assembly starts here.	*/
    {
	errcode = ' ';
	if (!getlin()) {
	    strcpy(linbuf,"\tEND\t\t;You forgot this!\n");
	    linptr = linbuf;
	    markerr('*');
	    ifstack[ifsp] = ON;
	}
	asmline();		/*  Get binary from line.	*/
	if (pass > 1) {
	    lineout();		/*  In pass 2, list line.	*/
	    hexout();		/*  In pass 2, build hex file.	*/
	}
	pc += nbytes;
	if (!pass) {		/*  This indicates end of pass 1.	*/
	    pass = 2;
	    rewind(source);

			/*  Mark symbols as not for direct addressing.	*/

	    for (sympoint = symtbl; sympoint < symend; ++sympoint)
		sympoint -> symname[0] |= 0x80;

	    ifsp = pc = errcount = extend = 0;
	    puts("Entering Pass 2.\n\n");
	}
    }
				/*  List number of errors.		*/
    linptr = linbuf;  *linptr++ = '\n';
    if (!errcount) strcpy(linptr,"No");
    else { putdec(errcount,&linptr);  *linptr = '\0'; }
    strcat(linbuf," error(s).\n");
    puts(linbuf);  putchar('\n');
    if (list != CONO && list != NOFILE) {
	fputs(linbuf,list);  putc('\f',list);
    }
    if (list != NOFILE) {	/*  If needed, sort and list symbols.	*/
	n = sortsym();

	sympoint = symtbl;
	while (n) {
	    linptr = linbuf;
	    for (m = 4; ; ) {
		movmem(sympoint->symname,linptr,SYMLEN);
		linptr += 8;  *linptr++ = *linptr++ = ' ';
		puthex4((sympoint++)->symvalu,&linptr);
		if (--n == 0 || --m == 0) break;
		*linptr++ = *linptr++ = *linptr++ = *linptr++ = ' ';
	    }
	    *linptr++ = '\n';  *linptr = '\0';  fputs(linbuf,list);
	}
	if (list >= LODISK) putc(CPMEOF,list);
	else if (list == LST) putc('\f',list);
    }
    fflush(list);
    fclose(list);

    fclose(source);
    wipeout("\n");
}

/*
Function to set up the file structure.  Routine is called with
the original argc and argv from main().
*/

setfiles(argc,argv)
int argc;
char *argv[];
{
    char c, *tptr, sorfname[16],lstfname[16],hexfname[16];

    if (!--argc) wipeout("\nNo file info supplied.\n");
    source = list = hex = NOFILE;

    sorfname[0] = curdrive + 'A';  sorfname[1] = ':';  tptr = &sorfname[2];
    for (++argv; (c = *(*argv)++) && c != '.'; *tptr++ = c);
    *tptr = '\0';

    strcpy(lstfname,sorfname);
    strcpy(hexfname,lstfname);
    strcat(sorfname,".68H");
    strcat(lstfname,".PRN");
    strcat(hexfname,".HEX");

    if (!--argc) goto defsorf;
    for (++argv; **argv; ) {
	switch (*(*argv)++) {
	    case 'S':	switch (c = *(*argv)++) {
			    case 'A':
			    case 'B':
			    case 'C':
			    case 'D':	sorfname[0] = c;

			    case '-':	if (fopen(sorfname,source = &_source)
					    != ERROR) break;
					wipeout("\nCan't open source.\n");

			    default:	goto badcomnd;
			}
			break;

	    case 'L':	switch (c = *(*argv)++)	{
			    case 'A':
			    case 'B':
			    case 'C':
			    case 'D':	lstfname[0] = c;

			    case '-':	if (fcreat(lstfname,list = &_list)
					    != ERROR) break;
					wipeout("\nCan't open list.\n");

			    case 'X':	list = CONO;
					break;

			    case 'Y':	list = LST;
					break;

			    default:	goto badcomnd;
			}
			break;

	    case 'H':	switch(c = *(*argv)++) {
			    case 'A':
			    case 'B':
			    case 'C':
			    case 'D':	hexfname[0] = c;

			    case '-':	if (fcreat(hexfname,hex = &_hex)
					    != ERROR) break;
					wipeout("\nCan't open hex.\n");

			    case 'X':	hex = CONO;
					break;

			    case 'Y':	hex = LST;
					break;

			    default:	goto badcomnd;
			}
			break;

	    badcomnd:
	    default:	wipeout("\nIllegal command line.\n");
	}
    }
    if (source != NOFILE) return;

    defsorf:

    if (fopen(sorfname,source = &_source) == ERROR)
	wipeout("\nCan't open source.\n");
}
	break;

	    case 'L':	switch (c = *(*argv)++)	{
			    case 'A':