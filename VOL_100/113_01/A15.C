/*
	HEADER:		CUG113;
	TITLE:		1802 Cross-Assembler (BDS C Version);
	FILENAME:	A15.C;
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

File:	a15.c

It all begins here.
*/

/*  Get globals:  */

#include "a15.h"

/*  The assembler starts here.  */

main(argc,argv)
int argc;
char *argv[];
{
    int n, m;

    puts("1805A/1802 Cross-Assembler Version 1.2\n");
    puts("Copyright (c) 1980,82,83,85  William C. Colley III\n");
    curdrive = bdos(GETDISK);  setfiles(argc,argv);
    sympoint = symtbl;			/*  Initialize symbol table.	*/
    symend = symtbl[SYMBOLS].symname;
    setmem(sympoint,(SYMBOLS * (SYMLEN + 2)),'\0');
    setmem(linbuf,SYMLEN,' ');		/*  Install initial symbols.	*/
    linbuf[0] = 'R';  linbuf[1] = '0';
    for (n = 0; n < 16; n++) {
	addsym(linbuf);  sympoint->symvalu = n;
	if ((linbuf[1] += 1) == ('9' + 1)) linbuf[1] = 'A';
    }
    ifstack[ifsp = 0] = ON;		/*  Initialize if stack.	*/
    hxbytes = 0;  flshhbf(0);		/*  Initialize hex generator.	*/
    pc = errcount = 0;  pass = 1;  extend = DEFEXT;
    puts("\nEntering Pass 1.\n");

    while (pass != 3) {		/*  The actual assembly starts here.	*/
	errcode = ' ';
	if (!getlin()) {
	    strcpy(linbuf,"\tEND\t\t;You forgot this!\n");
	    linptr = linbuf;  markerr('*');  ifstack[ifsp] = ON;
	}
	asmline();			/*  Get binary from line.	*/
	if (pass > 1) {
	    lineout();			/*  In pass 2, list line.	*/
	    hexout();			/*  In pass 2, build hex file.	*/
	}
	pc += nbytes;
	if (pass == 0) {	/*  This indicates end of pass 1.	*/
	    pass = 2;  rewind(source);

			/*  Mark symbols as not for direct addressing.	*/

	    for (sympoint = symtbl; sympoint < symend; sympoint++)
		sympoint->symname[0] |= 0x80;

	    ifsp = pc = errcount = 0;  extend = DEFEXT;

					/*  Unmark pre-defined symbols.	*/

	    setmem(linbuf,SYMLEN,' ');  linbuf[0] = 'R';  linbuf[1] = '0';
	    for (n = 0; n < 16; n++) {
		slookup(linbuf);  sympoint->symname[0] &= 0x7f;
		if ((linbuf[1] += 1) == ('9' + 1)) linbuf[1] = 'A';
	    }

	    puts("Entering Pass 2.\n\n");
	}
    }

    linptr = linbuf;  *linptr++ = '\n';	/*  List number of errors.	*/
    if (errcount == 0) strcpy(linptr,"No");
    else { putdec(errcount,&linptr);  *linptr = '\0'; }
    strcat(linbuf," error(s).\n");  puts(linbuf);  putchar('\n');
    if (list != CONO && list != NOFILE) {
	fputs(linbuf,list);  putc('\f',list);
    }
    if (list != NOFILE) {	/*  If needed, sort and list symbols.	*/
	n = sortsym();  sympoint = symtbl;
	while (n > 0) {
	    linptr = linbuf;
	    for (m = 0; m < 4; m++) {
		movmem(sympoint->symname,linptr,SYMLEN);
		linptr += 8;  *linptr++ = ' ';  *linptr++ = ' ';
		puthex4(sympoint->symvalu,&linptr);  *linptr++ = ' ';
		*linptr++ = ' ';  *linptr++ = ' ';  *linptr++ = ' ';
		sympoint++;
		if (--n <= 0) break;
	    }
	    linptr -= 4;  *linptr++ = '\n';  *linptr = '\0';
	    fputs(linbuf,list);
	}
	if (list >= LODISK) putc(CPMEOF,list);
	else if (list == LST) putc('\f',list);
    }
    fflush(list);  fclose(list);

    fclose(source);  wipeout("\n");
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

    strcpy(lstfname,sorfname);  strcpy(hexfname,lstfname);
    strcat(sorfname,".RCA");  strcat(lstfname,".PRN");
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

	    case 'L':	switch (c = *(*argv)++) {
			    case 'A':
			    case 'B':
			    case 'C':
			    case 'D':	lstfname[0] = c;

			    case '-':	if (fcreat(lstfname,list = &_list)
					    != ERROR) break;
					wipeout("\nCan't open list.\n");

			    case 'X':	list = CONO;  break;

			    case 'Y':	list = LST;  break;

			    default:	goto badcomnd;
			}
			break;

	    case 'H':	switch(c = *(*argv)++) {
			    case 'A':
			    case 'B':
			    case 'C':
			    case 'D':    hexfname[0] = c;

			    case '-':	if (fcreat(hexfname,hex = &_hex)
					    != ERROR) break;
					wipeout("\nCan't open hex.\n");

			    case 'X':	hex = CONO;  break;

			    case 'Y':	hex = LST;  break;

			    default:	goto badcomnd;
			}
			break;

	    badcomnd:
	    default:	wipeout("\nIllegal command line.\n");
	}
    }
    if (source != NOFILE) return;

    defsorf:

    if (fopen(sorfname,&_source) == -1) wipeout("\nCan't open source.\n");
    source = &_source;
}
		}
			break;

	    case 'L':	switch (c = *(*argv)++) {
			    case 'A':
			    ca