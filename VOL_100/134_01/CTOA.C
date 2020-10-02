/*	CTOA -- BDS `C' CRL-to-ASM postprocessor- part 1.	*/

/*
	Kevin B. Kenny
	729-A E. Cochise Dr.
	Phoenix, AZ   85020
*/

/*	Copyright (c) 1983 by Kevin B. Kenny
	Released to the BDS `C' Users' Group for non-commercial distribution
	only. */

/*	This program is a utility that generates assembly language (.CSM)
	source files for BDS `C' programs.  It accepts the .CRL file for the
	object instructions, the .CDB file for the symbols, and the .C source
	file for commentary.  From these, it produces a .CSM file that generate
	the same object code.

	Syntax:
		ctoa source_file [>output_file]
*/

#include <bdscio.h>
#include <dio.h>
#include <cmdutil.h>
#include "ctoatbls.h"
#include "ctoa.h"

#define TITLE "CTOA version 83-11-11 copyright (c) 1983 by Kevin Kenny.\n"

/*	Main program	*/

main (argc, argv)
    int argc;
    char * * argv;
    {
	/* Check command syntax */

	dioinit (&argc, argv);
	if (argc != 2) {
		fprintf (STD_ERR, "Usage: ctoa filename [>outfile]\n");
		quit ();
		}
	fprintf (STD_ERR, TITLE);

	/* Pick up the source file name, and open the source */

	strcpy (srcfnam, argv [1]);
	if (fopen (srcfnam, srcfile) == ERROR) {
		fprintf (STD_ERR, "Can't open %s: %s\n", srcfnam, 
					errmsg (errno ()));
		quit ();
		}

	/* Acquire the opcode table */

	opc_tabl = opctabl ();

	/* Go get the .CRL  and .CDB files */

	initcdb ();
	initcrl ();

	/* Process the content of the .CRL file */

	proccrl ();

	/* Flush any remaining text from the source file, and close it. */

	while (!seof) sscan ();
	printf ("\n\n\tend\n");
	fclose (srcfile);
	if (cdbopen) cclose (cdbfile);

	/* Flush out directed I/O */

	dioflush ();

	}

/*	Set up processing of the .CRL file for object text 	*/

initcrl() {

	/* Get the .CRL file name */

	strcpy (crlfnam, srcfnam);
	makeext (crlfnam, "CRL");

	/* Open the .CRL file, and read in the directory from it */

	tcopen (crlfile, crlfnam);
	tcseek (crlfile, 0, CABS, crlfnam);
	tcread (crlfile, crldir, sizeof crldir, crlfnam);

	/* Print a heading on the CSM file indicating the file name and
	   compilation options. */

	printf (";\t%s\tBDS `C' object code of %s\n", crlfnam, srcfnam);
	printf (";\t\t\tCompilation options:");
	if (havecdb) printf (" -k");
	if (crldir.crleflag == 0xBD)
		printf (" -e %04x", crldir.crleloc);
	printf ("\n");
	printf (";\t\t\tExternals use %d (0x%04x) bytes.\n", 
			crldir.crlelen, crldir.crlelen);
	printf ("\n\tMACLIB\t<BDS.LIB>\n\n");
	if (crldir.crleflag == 0xBD)
		printf ("SYS$EXTFLAG\tSET\t0BDH\nSYS$EXTADDR\tSET\t0%04xH\n",
							     crldir.crleloc);
	printf ("SYS$EXTSIZE\tSET\t0%04xH\n\n", crldir.crlelen);

	/*	Print non-library-defined CCC symbols on the .CSM file. */

	initccc ();
	}

/*	Process the functions on a .CRL file	*/

proccrl () {
	union {			/* .CRL directory pointer */
		char * c;
		int * i;
		} dirp;

	/* Point dirp to the start of the directory */

	dirp.c = & (crldir.crldtext);

	/* Walk through the directory, doing functions one by one */

	for (;;) {
		strcpy70 (fname, dirp.c);	/* Get function name */
		if (!*fname) break;		/* Return if end of file */
		dirp.c += strlen (fname);	/* Advance directory pointer */
		faddr = *dirp.i++;		/* Get function address */
		procfunc ();			/* Process the function */
		}
	cclose (crlfile);		/* All done; close file */
	}

/*	Do one function from a .CRL file	*/

procfunc () {

	/* Announce ourselves */

	fprintf (STD_ERR, "; Processing the %s function:\n", fname);

	/* Find the function on the source and .CDB files. */

	sfunct (fname);
	cdbfunct (fname);

	/* Put the FUNCTION statement on the .CSM file */

	printf ("\n\tFUNCTION\t%s\n\n", fname);

	/* Process the external references from the function */

	procexts ();

	/* Output the stack frame layout */

	doframe ();

	/* Read in the function text and relocation */

	if (!readfunc ()) {
		fprintf (STD_ERR, ";*** Function is too large to analyze.\n");
		fprintf (STD_ERR, ";*** Break it up and try again.\n");
		}

	else {
		/* Construct the label table */

		bldlbtab ();

		/* Output the code */

		asmcode ();

		/* Get rid of allocated memory */

		freefunc ();
		}

	/* Find the end of the function on the source */

	sendfn ();

	/* Put the ENDFUNC statement on the .CSM file */

	printf (";\n\tENDFUNC\t\t%s\n;\n", fname);
	}

/*	Process the external reference directory from a function on the
	.CRL file */

procexts () {
	int extdlen;		/* Length of the external directory */
	char syname [NAMLEN];	/* Name of an external symbol */
	char * xnamp;		/* Pointer to current char in name */

	/* Position to the start of the function's external directory */

	extdlen = 0;
	tcseek (crlfile, faddr, CABS, crlfnam);

	nexts = 0;		/* No externals yet. */

	/* Read externals from the file */

	for (;;) {
		xnamp = & syname;
		tcread (crlfile, xnamp, 1, crlfnam); ++extdlen;
					/* Get first byte of external name */
		if (syname [0] == '\0') break;
					/* If it's zero, we're done. */
		while ((*xnamp & 0x80) == 0) {
			tcread (crlfile, ++xnamp, 1, crlfnam); ++extdlen;
			}		/* Read the rest of the symbol */
		*xnamp &= 0x7F;		/* Strip the 7th bit from last char */
		*++xnamp = 0;		/* Add null terminator */
		printf ("\tEXTERNAL\t%s\n", syname);
					/* Put EXTERNAL statement on .CSM */
		strcpy (xname [nexts++], syname);
					/* Install name in table */
		}
	
	/* End of external directory */

	printf (";\n");
	ftaddr = faddr + extdlen + 2;	/* Find function text */
	}	

/*	Read the text and relocation of a function	*/

readfunc () {

	/* Read in the length word for the function text */

	tcseek (crlfile, ftaddr-2, CABS, crlfnam);
	tcread (crlfile, &ftlen, 2, crlfnam);

	/* Get space for the function text, and read it in. */

	if ((ftext = alloc (ftlen)) == NULL) return (FALSE);
	tcread (crlfile, ftext, ftlen, crlfnam);

	/* Read in the size of the relocation info */

	tcread (crlfile, &frlen, 2, crlfnam);

	/* Read in the relocation data itself */

	if ((freloc = alloc (2 * frlen)) == NULL) {
		free (ftext);
		return (FALSE);
		}
	tcread (crlfile, freloc, 2 * frlen, crlfnam);

	/* Tell the user what happened */

	fprintf (STD_ERR, ";\t%d (0x%04x) bytes of text, ", ftlen, ftlen);
	fprintf (STD_ERR, "%d relocation directives,\n", frlen);
	fprintf (STD_ERR, ";\t%d external functions", nexts);
	if (havecdb) {
	 fprintf (STD_ERR, ", %d external variables,\n", nextvs);
	 fprintf (STD_ERR, ";\t%d auto variables, %d formal parameters, ",
	 		     nautvs,		nparvs);
	 fprintf(STD_ERR, "frame size is %d (0x%04x)\n", framesize, framesize);
	 }
	else fprintf (STD_ERR, "\n");
	return (TRUE);
	}

/*	Make the label table for a function	*/

bldlbtab () {

	nlabs = 0;			/* Clear out the label table */

	pass1 = TRUE;			/* We're doing the label pass */

	dotext ();			/* Analyze text for line #s. */

	doreloc ();			/* Get other labels from relocation */

	}

/*	Make the assembly code for a function	*/

asmcode () {
	
	pass1 = FALSE;			/* We're doing the code pass */

	dotext ();			/* Output the instructions */

	dospool ();			/* Output the string pool */
	}

/*	Walk through the instructions for a function	*/

dotext () {
	char * label;			/* Label on the current word */
	struct opc_entry * opcent;

	/* Set up the initial location counter past the external header */

	if (nexts == 0) locctr = 0;
	else locctr = 3 * nexts + 3;
	retaddr = ftlen;		/* Dummy up a return address */
	highcode = 0;			/* Dummy up a high transfer address */

	/* Walk through the instruction sequence */

	while (locctr < retaddr) {

		/* Look up the opcode */

		opcode = ftext [locctr];
		for (	opcent = opc_tabl;
			(opcode & opcent -> opc_mask) != opcent -> opc_val;
			++opcent) ;

		/* In code pass, output the opcode, and any label that
		   may be needed. */

		if (!pass1) {
			if ((label = findlab (locctr)) != NULL)
				printf ("\n%s:", label);
			if (opcent -> opc_type != OP_RST6)
				printf ("\t%s", opcent -> opc_name);
			}

		/* Process the opcode and flag the return address */

		switch (opcent -> opc_type) {
		  	case OP_SIMPLE: dosimple (); break;
			case OP_LXI:	dolxi ();	break;
			case OP_RPSP:	dorpsp ();	break;
			case OP_REG53:	doreg53 ();	break;
			case OP_MVI:	domvi ();	break;
			case OP_DATA1:	dodata1 ();	break;
			case OP_DATA2:	dodata2 ();	break;
			case OP_MOV:	domov ();	break;
			case OP_ARITH:	doarith ();	break;
			case OP_CONRET:	doconret ();	break;
			case OP_RPPSW:	dorppsw ();	break;
			case OP_CONTRA:	docontra ();	break;
			case OP_TRANS:	dotrans ();	break;
			case OP_IARITH:	doiarith ();	break;
			case OP_RST:	dorst ();	break;
			case OP_IO:	doio ();	break;
			case OP_RST6:	dorst6 ();	break;
			case OP_WHAT:	dowhat ();	break;
			default:	
				fprintf (STD_ERR, "in dotext: can't happen\n");
				quit ();
			}

		if (!pass1) printf ("\n");	/* End assembly line */
		}
	}

/*	*******	Processing routines for the various opcodes	******* */

/*	Simple operations	*/

dosimple () {
	if (opcode == RET && locctr >= highcode) 
		retaddr = locctr + 1;
				/* Set the return address from function */
	++locctr;
	}

/*	LXI	*/

dolxi () {
	if (!pass1) {
		tab ();				/* LXI x, addr */
		do_pairsp ((opcode >> 4) & 0x03);
		comma ();
		do_imm2 ();
		}
	locctr += 3;
	}

/*	Register pair (11 = SP) 	*/

dorpsp () {
	if (!pass1) {
		tab ();				/* INX x */
		do_pairsp ((opcode >> 4) & 0x03);
		}
	++locctr;
	}

/*	Register number in bits 5-3	*/

doreg53 () {
	if (!pass1) {
		tab ();				/* INR x */
		do_regno ((opcode >> 3) & 0x07);
		}
	++locctr;
	}

/*	MVI	*/

domvi () {
	if (!pass1) {			
		tab ();				/* MVI x, opd */
		do_regno ((opcode >> 3) & 0x07);
		comma ();
		do_byte (ftext [locctr+1]);
		}
	locctr += 2;
	}

/*	LHLD or SHLD	*/

dodata2 () {
	if (!pass1) {
		tab ();				/* LHLD addr */
		do_addr ();
		}
	locctr += 3;
	}

/*	LDA or STA	*/

dodata1 () {
	if (!pass1) {
		tab ();				/* LDA addr */
		do_addr ();
		}
	locctr += 3;
	}

/*	MOV	*/

domov () {
	if (!pass1) {
		tab ();				/* MOV x, y */
		do_regno ((opcode >> 3) & 0x07);
		comma ();
		do_regno (opcode & 0x07);
		}
	++locctr;
	}

/*	Arithmetic operations	*/

doarith () {
	if (!pass1) {
		tab ();				/* ADD x */
		do_regno (opcode & 0x07);
		}
	++locctr;
	}

/*	Conditional returns	*/

doconret () {
	if (!pass1) {	
		do_ccode ();			/* Rcc */
		}
	++locctr;
	}

/*	Register pairs (11 = PSW) 	*/

dorppsw () {
	if (!pass1) {
		tab ();
		do_pairpsw ((opcode >> 4) & 0x03);	/* PUSH xx */
		}
	++locctr;
	}

/*	Conditional transfers	*/

docontra () {
	if (!pass1) {
		do_ccode ();			/* Jcc addr */
		tab ();
		}
	do_xaddr ();
	locctr += 3;
	}

/*	Unconditional transfers	*/

dotrans () {
	struct ccc_entry * ccc;
	if (!pass1) {
		tab ();				/* JMP addr */
		}
	do_xaddr ();
	if ((!isreloc (locctr+1))		/* Funny CCC operator? */
	 && (ccc = scanccc (textword (locctr+1), CCC_CODE))) {
		if (!pass1) {
			switch (ccc -> ccc_flags & 0x07) {
				case 1: printf ("\n\t     db\t");
					do_extoff (ftext [locctr+3]);
					break;
				case 2: printf ("\n\t     dw\t");
					do_extoff (textword (locctr+3));
					break;
				case 5: printf ("\n\t     db\t");
					do_autoff (ftext [locctr+3]);
					break;
				case 6: printf ("\n\t     dw\t");
					do_autoff (textword (locctr+3));
					break;
				}
			}
		locctr += ccc -> ccc_flags & 0x03;	/* Handle funny ops */
		}
	locctr += 3;
	}

/*	Immediate arithmetic	*/

doiarith () {
	if (!pass1) {
		tab ();				/* ADI xx */
		do_byte (ftext [locctr+1]);
		}
	locctr += 2;
	}

/*	Restarts other than 6	*/

dorst () {
	if (!pass1) {
		tab ();				/* RST n */
		printf ("%d", (opcode >> 3) & 0x07);
		}
	++locctr;
	}

/*	I/O operations	*/

doio () {
	if (!pass1) {
		tab ();				/* IN 0nnH */
		printf ("0%02xh", ftext [locctr+1]);
		}
	locctr += 2;
	}

/*	Restart 6 (line number indication) 	*/

dorst6 () {

	char label[8];		/* Storage for labels in pass 1 */

	/*	If this is the label-making pass, assign a label to the
		line for the disassembly pass to use.	*/

	if (pass1) {
		sprintf (label, "line%d", textword (locctr+1) & 0xFFF);
		makelab (label, locctr);
		}

	/* This is code pass.  Copy the source line to the .CSM file */

	else {
		if (!(textword (locctr+1) & 0xF000)) {
			printf ("\n");
			slineno (textword (locctr+1) & 0xFFF);
			}
		}
	locctr += 3;
	}

/*	Unknown operation	*/

dowhat () {
	fprintf (STD_ERR, "; Warning: unknown opcode at location 0x%04x\n",
								locctr);
	fprintf (STD_ERR, ";\tHex value is %02x.\n");
	if (!pass1) {
		do_byte (opcode);
		}
	++locctr;
	}

/*	Put a register number on the .CSM file	*/

do_regno (n)
    int n;				/* Register number */
    {
	char * regnos;

	regnos = "bcdehlma";
	printf ("%c", regnos [n]);
	}

/*	Put a register pair (11=SP) on the .CSM file	*/

do_pairsp (n)
    int n;
    {
	switch (n) {
		case 0:	printf ("b");	break;
		case 1: printf ("d");	break;
		case 2:	printf ("h");	break;
		case 3: printf ("sp");	break;
		}
	}

/*	Put a register pair (11=PSW) on the .CSM file	*/

do_pairpsw (n)
    int n;
    {
	switch (n) {
		case 0: printf ("b");	break;
		case 1:	printf ("d");	break;
		case 2: printf ("h");	break;
		case 3:	printf ("psw");	break;
		}
	}

/*	Put a one-byte quantity on the .CSM file	*/

do_byte (n)
    char n;
    {
	printf ("0%02xh\t\t; %3d", n, n);
	if (n >= ' ' && n <= '~')
		printf (" ('%c')", n);
	}

/*	Put a condition code on the .CSM file	*/

do_ccode () {
	switch (opcode >> 3 & 0x07) {
		case 0:	printf ("nz");	break;
		case 1: printf ("z");	break;
		case 2:	printf ("nc");	break;
		case 3:	printf ("c");	break;
		case 4: printf ("po");	break;
		case 5:	printf ("pe");	break;
		case 6:	printf ("p");	break;
		case 7:	printf ("m");	break;
		}
	}

/*	Put a two-byte immediate operand on the .CSM file	*/

do_imm2 () {
	if (ftext [locctr+3] == DADSP)
		do_frsiz (textword (locctr+1));
				/* Appears to be a frame size */

	else if (ftext [locctr+3] == DADB)
		do_autoff (textword (locctr+1));
				/* Appears to be an AUTO offset */

	else if (textword (locctr-2) == EXTRNS && ftext [locctr+3] == DADD)
		do_extoff (textword (locctr+1));
				/* Appears to be an external variable offset */

	else			/* Don't know what it is */
		do_laddr ();		/* Treat like address for now */
	}

/*	Put a transfer address on the .CSM file			*/

do_xaddr () {
	struct ccc_entry * ccc;

	/* Remember highest transfer address so we can find the last RET */

	if (isreloc (locctr+1))	
		if (textword (locctr+1) > highcode)
			highcode = textword (locctr+1);
	if (highcode > retaddr) retaddr = ftlen + 1;
	if (pass1) return;

	/* Handle addresses in C.CCC */

	if (isreloc (locctr+1)
	 || !(ccc = scanccc (textword (locctr+1), CCC_CODE)))
				/* If not in C.CCC */
		do_laddr ();	/* Treat like data address for now */
	else
		putccc (ccc);
	}

/*	Put a data address on the .CSM file	*/

do_addr () {
	struct ccc_entry * ccc;

	if (isreloc (locctr+1)
	 || !(ccc = scanccc (textword (locctr+1), CCC_DATA)))
					/* If not in C.CCC ... */
		do_laddr ();		/* treat as local for now */
	else
		putccc (ccc);
	}

/*	Put a local address on the .CSM file	*/

do_laddr () {
	unsigned addr;		/* Address to print */

	addr = textword (locctr+1);	/* Get the address */

	if (isreloc (locctr+1)) {	/* Relative address? */
		if (addr < 3 * nexts + 3) {	/* External? */
			printf ("%s", xname [addr / 3 - 1]);
			if (addr % 3) printf ("+%d", addr % 3);
			}
		else printf ("%s", findlab (addr));
		}
	else if (crldir.crleflag	/* Constant external address? */
	      && textword (locctr+1) >= crldir.crleloc
	      && textword (locctr+1) < crldir.crleloc + crldir.crlelen) {
		printf ("SYS$EXTADDR+");
		do_extoff (textword (locctr+1) - crldir.crleloc);
		}
	else  {
		printf ("0%04xh\t\t; %d", addr);	/* Absolute address */
		if (addr >= ' ' && addr <= '~')
			printf (" ('%c')", addr);
		}
	}

/*	Print a tab 	*/

tab () {
	printf ("\t");
	}

/*	Print a comma	*/

comma () {
	printf (", ");
	}

/*	Generate dummy labels for the targets of any relocated addresses */

doreloc () {
	int xlabno;			/* Count of transfer points */
	int strno;			/* Count of strings */
	int i;				/* Loop index */
	int addr;			/* Relocated target address */
	char label [8];			/* Label to assign to it */

	xlabno = strno = 0;

	for (i=(nexts > 0); i<frlen; ++i) {	/* Walk the relocation list */
		addr = textword (freloc [i]);	/* Get target address */
		if (addr >= 3 * nexts + 3	/* Internal address? */
		 && findlab (addr) == NULL) {	/* Labeled yet? */
			if (addr >= retaddr)	/* String pool? */
				sprintf (label, "str%d", ++strno);
			else
				sprintf (label, "x%d", ++xlabno);
			makelab (label, addr);
			}
		}
	}

/*	Determine if a particular address is relocatable	*/

int isreloc (offset)
    unsigned offset;
    {
	int i;
	for (i=frlen-1; i>=0; --i)
		if (freloc [i] == offset) return (TRUE);
	return (FALSE);
	}

/*	Output the content of the string pool	*/

dospool () {
	
	char spopen;			/* Flag: between quotes on DB */
	char comma;			/* Flag: need comma on DB */
	char * label;			/* Label to be printed */
	char byte;			/* Byte we're working on */

	printf ("\n;\tString pool for function %s\n", fname);
	spopen = comma = FALSE;		/* Haven't opened a quote yet */

	while ((locctr < ftlen) && (!findlab (locctr))) ++locctr;
					/* Skip any unreacahable epilogue */

	while (locctr < ftlen) {	/* Walk through string pool */
		if (label = findlab (locctr)) {	/* Put out any labels */
			if (spopen) {
				printf ("'");
				spopen = FALSE;
				}
			comma = FALSE;
			printf ("\n%s:\tdb\t", label);
			}
		
		byte = ftext [locctr];	/* Get a byte from string */
		if (byte < ' ' || byte > '~') {
					/* Printable? */
			if (spopen) {		/* No; close out the string */
				printf ("'");
				spopen = FALSE;
				comma = TRUE;
				}
			if (comma) {		/* Put out any needed comma */
				printf (", ");
				comma = FALSE;
				}
			printf ("0%02xH", byte);	/* Put the byte */
			comma = TRUE;		/* Need comma next time */
			}

		else {
			if (!spopen) {		/* Printable; open a quote */
				if (comma) {	/* First putting out comma */
					printf (", ");
					comma = FALSE;
					}
				printf ("'");
				spopen = TRUE;
				}
			printf ("%c", byte);	/* add byte to quoted string */
			if (byte == '\'') printf ("'");
			}
		++locctr;	/* advance to next byte in pool */
		}
	if (spopen) printf ("'");
	printf ("\n\n");
	}

/*	Free the storage allocated for a function's text and relocation	*/

freefunc () {

	free (freloc); freloc = NULL;
	free (ftext); ftext = NULL;

	}

/*	Add a label to the list of relocatable labels	*/

makelab (str, value)
    char * str;				/* Name of the label */
    unsigned value;			/* Relocation offset of the label */
    {

	/* Never add a second label to a word */

	if (findlab (value) != NULL) return;

	/* Never define the same label twice */

	if (findaddr (str) != ERROR) return;

	/* Insert the new definition */

	lab_tab [nlabs] . lab_val = value;
	strcpy (lab_tab [nlabs++] . lab_name, str);
	}

/*	Find a label, by address, in the label list	*/

char * findlab (value)
    unsigned value;
    {
	int i;
	for (i=nlabs-1; i>=0; --i)
		if (lab_tab [i] . lab_val == value) 
			return (&(lab_tab [i] . lab_name));
	return (NULL);
	}

/*	Find the address of a label, given its name	*/

unsigned findaddr (str)
    char * str;
    {
	int i;
	for (i=nlabs-1; i>=0; --i)
		if (!strcmp (lab_tab [i] . lab_name, str)) 
			return (lab_tab [i] . lab_val);
	return (ERROR);
	}

/*	Extract a word from the function text	*/

unsigned textword (offset)
    unsigned offset;			/* Location of the word */
    {
	return ((ftext [offset+1] << 8) + ftext [offset]);
	}

/*	Source continues in CTOA2.C	*/
  {