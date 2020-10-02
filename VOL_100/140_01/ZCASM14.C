/*

ZCASM.C	-- a modification of Leor Zolman's CASM.C

Copyright (c) 1983 Brian Waldron, Xerox Corporation

Altered to accept standard ZILOG Z80 mnemonics and output
short labels for consumption by Microsoft's M80/L80

	See the CASM Appendix in the BDSC User's Guide for
	complete information.

	Compile and link with:

			cc zcasm.c -o -e4300

			l2 zcasm
		(or)	clink zcasm

	ver. 1.0	7 Oct 83 blw

	ver. 1.1	11 Oct 83 blw
			changed DB, DW, DS to DEFB, DEFW, DEFS
			and modified putdir() accordingly

	ver. 1.2	17 Oct 83 blw
			cleaned up numerous typos

	ver 1.2 released to PD January 1984

	ver. 1.3	21 Feb 84 blw
			Allowed underscore(_) in function names

	ver. 1.4	27 Mar 85        Lindsay Haisley - Austin, TX.
			Fixed numerous lethal bugs associated with the
			EXTERNAL ZCASM pseudo op.  The list of needed
			functions (*nflist) will now include the name
			of the current function itself, thus avoiding
			multiple lables when EXTERNAL functions are
			referenced. Several initializers in "for" loops 
			were changed to accomodate this modification.
			Two "JMP" statements changed to "JP" statements
			for Z80.  One fprintf2 statement fixed so that 
			the number of arguments agrees with the number 
			of conversion characters.
			  
ZCASM is hereby placed in the public domain with the restriction
that it may not be distributed for profit.

*/

#include <bdscio.h>

#define TITLE "BDS C CRL-format M80/L80 Preprocessor ver. 1.4\n"


/*
 *	Customizable definitions:
 */

#define DEFUSER	"9/"		/* default user area for include files	 */
				/* make this a null string for "current" */

#define DEFDISK "A:"		/* default disk for include files	 */
#define ZCASMEXT	".CZM"	/* extension on input files 		 */
#define ASMEXT	".MAC"		/* extension on output files		 */
#define SUBFILE "A:ZZZ.FOO"	/* Submit file to erase on error. To not */
				/* erase any, use a null string ("")	 */

#define CONTROL_C 3		/* Abort character */
#define EQUMAX	500		/* maximum number of EQU ops		 */
#define FUNCMAX	100		/* maximum number of functions		 */
#define NFMAX	100		/* maximum number of external functions
				   in one function			 */
#define LABMAX	150		/* max number of local labels in 1 func	 */
#define TXTBUFSIZE 2000		/* max # of chars for labels and needed
				   function names for a single function	 */

		/* the following stuff implements a symbol table for
		   the purpose of shortening labels to accommidate M80	*/

#define MAXSYMS 150		/* max # of entries in label		*/
				/* innumeration table			*/
#define MAXCHARS 1500		/* max # of chars in lable		*/
				/* innumeration table			*/

char *str_name[MAXSYMS];	/* array of symbol table entries	*/
char sym_buff[MAXCHARS];	/* symbol table				*/
char *next_sym;			/* pointer to next empty slot		*/
int  current_syms;		/* # of currently defined entries	*/

/*
 *	End of customizable section
 */

#define DIRSIZE	512		/* max # of byte in CRL directory 	*/
#define TPALOC	0x100		/* base of TPA in your system		*/

		/* Global data used throughout processing
		   of the input file:			*/

char	fbuf[BUFSIZ];		/* I/O buffer for main input file	*/
char	incbuf[BUFSIZ];		/* I/O buffer for included file 	*/
char	obuf[BUFSIZ];		/* I/O buffer for output file		*/
char	*cbufp;			/* ptr to currently active inp buf	*/
char	*cfilnam;		/* ptr to name of current inp file	*/
char	nambuf[30],		/* filenames for current intput		*/
	nambuf2[30],		/* and output files.			*/
	onambuf[30];

char	*equtab[EQUMAX];	/* table of absolute symbols		*/
int	equcount;		/* # of entries in equtab		*/

char	*fnames[FUNCMAX];	/* list of functions in the file	*/
int	fcount;			/* # of entries in fnames		*/

int	lino,savlino;		/* line number values used for
				   error reporting.			*/

char	doingfunc;		/* true if now processing a function	*/

char	errf;			/* true if an error has been detected	*/
char	verbose;		/* true to output wordy comments	*/
char	blankflag;		/* true if last line processed was null	*/

		/* Global data used during the processing of a
		   single function in the source file:			*/

char	*nflist[NFMAX];		/* list of needed functions		*/
int	nfcount;		/* number of entries in nflist		*/

struct {
	char *labnam;		/* name of function label		*/
	char defined;		/* whether it has been defined yet	*/
} lablist[LABMAX];

int	labcount;		/* # of local labels in a function	*/

char	txtbuf[TXTBUFSIZE],	/* where text of needed function 	*/
	*txtbufp;		/* names and function labels go		*/

char 	linbuf[150],		/* text line buffers	*/
	linsav[150],
	workbuf[150],
	pbuf[150], *pbufp;

char	*cfunam;	/* pointer to name of current function		*/
int	relblc;		/* relocation object count for a function	*/

char	pastnfs;		/* true if past all needed functions	*/
				/* declarations ("external" pseudo ops)	*/

int	argcnt;			/* values set by "parse_line" function	*/
char	*label,
	*op,
	*argsp,
	*args[40];

char 	*gpcptr;		/* general-purpose text pointer	*/
char    temp_string[20];	/* scratch text buffer		*/

/*
 * Open main input file, open output file, initialize needed globals
 * and process the file:
 */

main(aarghc,aarghv)
char **aarghv;
{
	int i,j,k;
	char c, *inpnam, *outnam;

	puts(TITLE);

	initequ();		/* init EQU table with reserved words	*/
	fcount = 0;		/* haven't seen any functions yet	*/
	doingfunc = FALSE;	/* not currently processing a function	*/
	errf = FALSE;		/* no errors yet */
	verbose = FALSE;
	inpnam = outnam = NULL;	/* haven't seen any names yet		*/
	blankflag = FALSE;	/* haven't just processed a null line	*/

	while (--aarghc) 
	{
		++aarghv;	/* bump to next arg text */
		if (**aarghv == '-')
		{
		    switch(c = aarghv[0][1])
		    {
			case 'C':
				verbose = 1;
				break;

			case 'O':
				if (aarghv[0][2])
					outnam = &aarghv[0][2];
				else if (--aarghc)
					outnam = *++aarghv;
				else goto usage;
				break;

			default: goto usage;
		    }
		}
		else
			inpnam = *aarghv;
	}

	if (!inpnam) {
  usage:	puts("Usage:\tzcasm [-c] [-o <name>] <filename>\n");
		puts("-C: don't strip comments from input and output\n");
		puts("-O <name>: Call the output file <name>.MAC\n");
		if(*SUBFILE) unlink(SUBFILE);
		exit();
	}

	/* set up filenames with proper extensions: */
	for (i = 0; (c = inpnam[i]) && c != '.'; i++)
		nambuf[i] = c;
	nambuf[i] = '\0';

	strcpy(onambuf, outnam ? outnam : nambuf);
	strcat(nambuf,ZCASMEXT);	/* input filename */
	cbufp = fbuf;			/* buffer pointer */
	cfilnam = nambuf;		/* current filename pointer */
	if (fopen(cfilnam,cbufp) == ERROR){
		if (*SUBFILE) unlink(SUBFILE);
		exit(printf("Can't open %s\n",cfilnam));
	}	
	if (!hasdot(onambuf))
		strcat(onambuf,ASMEXT);		/* output filename */
	if (fcreat(onambuf,obuf) == ERROR){
		if(*SUBFILE) unlink(SUBFILE);
		exit(printf("Can't create %s\n",onambuf));
	}
					/* begin writing output file */
	fprintf2(obuf,"\nTPALOC\tEQU\t%04xH\n",TPALOC);

	lino = 1;			/* initialize line count */

	while (get_line()) {		/* main loop */
		if (kbhit() && getchar() == CONTROL_C)
			abort("Aborted by ^C\n");
		process_line();		/* process lines till EOF */
		lino++;
	}

	if (doingfunc)		/* if ends inside a function, error */
		abort("File ends, but last function is unterminated\n");

	if (errf)
	{
		puts("Fix those errors and try again...");
		unlink(onambuf);
		if (*SUBFILE) 
			unlink(SUBFILE);
	}
	else
	{
					/* end of functions	*/
		fputs2("\nEND$CRL\tEQU\t$-TPALOC\n",obuf);
		putdir();		/* now put out CRL dir.	*/
		fputs2("\tEND\n",obuf);	/* end of ASM file	*/
		putc(CPMEOF,obuf);	/* CP/M EOF char.	*/
		fclose(cbufp);		/* close input file	*/
		fclose(obuf);		/* close output file	*/
		printf("%s is ready to be assembled.\n",onambuf);
	}
}

/*
 * Get a line of text from input stream, and process
 * "include" ops on the fly:
 */

int get_line()
{
	int i;

top:	if (!fgets(linbuf,cbufp)) {		/* on EOF: */
		if (cbufp == incbuf) {		/* in an "include" file?*/
			fabort(cbufp->_fd);	/* close the file	*/
			cbufp = fbuf;	/* go back to mainline file	*/
			cfilnam = nambuf;
			lino = savlino + 1;
			return get_line();
		}
		else return NULL;
	}

	if (!verbose)			/* strip commments, default */
	{
		for (i = 0; linbuf[i]; i++)
		{
			if (linbuf[i] == ';')
			{
				while (i && isspace(linbuf[i-1]))
					i--;
				if (!i && blankflag)
				{
					lino++;
					goto top;
				}
				strcpy(&linbuf[i], "\n");
				blankflag = TRUE;
				break;
			}
			if (linbuf[i] == '\'' || linbuf[i] == '"')
				break;
		}
		if (!linbuf[i])
			blankflag = FALSE;		
	}

	parse_line();				/* not EOF. Parse line */
	if (streq(op,"INCLUDE")  ||		/* check for file inclusion */
	    streq(op,"MACLIB")) {
		if (cbufp == incbuf)		/* if already in an include, */
		 abort("Only one level of inclusion is supported"); /* error */
		if (!argsp)
		 abort("No filename specified");
		cbufp = incbuf;			/* set up for inclusion */
		savlino = lino;

		for (i = 0; !isspace(argsp[i]); i++)	/* put null after */
			;				/* filename	  */
		argsp[i] = '\0';

		*nambuf2 = '\0';

		if (*argsp == '<') {		/* look for magic delimiters */
			strcpy(nambuf2,DEFUSER);
			if (argsp[2] != ':')	/* if no explicit disk given */
				strcat(nambuf2,DEFDISK); /* then use default */
			strcat(nambuf2,argsp+1);
			if (nambuf2[i = strlen(nambuf2) - 1] == '>')
				nambuf2[i] = '\0';
		} else if (*argsp == '"') {
			strcpy(nambuf2,argsp+1);
			if (nambuf2[i = strlen(nambuf2) - 1] == '"')
				nambuf2[i] = '\0';
		} else
			strcpy(nambuf2,argsp);

		if (fopen(nambuf2,cbufp) == ERROR) {
			if (!hasdot(nambuf2)) {
				strcat(nambuf2,".LIB");
				if (fopen(nambuf2,cbufp) != ERROR)
					goto ok;
			}			    
			printf("Can't open %s\n",nambuf2);
			abort("Missing include file");
		}

	ok:	lino = 1;
		cfilnam = nambuf2;
		return get_line();
	}
	return 1;
}

parse_line()
{
	int i;
	char c;

	label = op = argsp = NULL;
	argcnt = 0;

	strcpy2(pbuf,linbuf);
	strcpy2(linsav,linbuf);
	pbufp = pbuf;

	if (!*pbufp) return;		/* ignore null lines */
	if (!isspace(c = *pbufp)) {
		if (c == ';')
			return;		/* totally ignore comment lines */
		label = pbufp;		/* set pointer to label	*/
		while (isidchr(*pbufp))	/* pass over the label identifier */
			pbufp++;
		*pbufp++ = '\0';	/* place null after the identifier */
	}

	skip_wsp(&pbufp);
	if (!*pbufp || *pbufp == ';')
		return;
	op = pbufp;			/* set pointer to operation mnemonic */
	while (isalpha(*pbufp))
		pbufp++;  		/* skip over the op 		*/
	if (*pbufp) *pbufp++ = '\0';	/* place null after the op	*/


					/* now process arguments	*/
	skip_wsp(&pbufp);
	if (!*pbufp || *pbufp == ';')
		return;
	argsp = linsav + (pbufp - pbuf);/* set pointer to arg list */

					/* create vector of ptrs to all args
					   that are possibly relocatable */
	for (argcnt = 0; argcnt < 40;) {
		while (!isidstrt(c = *pbufp))
			if (!c || c == ';')
				return;
			else
				pbufp++;

		if (isidchr(*(pbufp - 1))) {
			pbufp++;
			continue;
		}

		args[argcnt++] = pbufp;			
		while (isidchr(*pbufp)) pbufp++;
		if (*pbufp) *pbufp++ = '\0';
	}
	error("Too many operands in this instruction for me to handle\n");
}

process_line()
{
	char *cptr, c;
	int i,j;

	if (op) {
			/* check for definitions of global data that will be
			   exempt from relocation when encountered in the
			   argument field of assembly instructions: 	   */

	   if (streq(op,"EQU") || streq(op,"SET") ||
		(!doingfunc &&
		    (streq(op,"DEFS") || streq(op,"DEFB") || streq(op,"DEFW")
		 || streq(op,"DS") || streq(op,"DB") || streq(op,"DW"))))
	   {
		fputs2(linbuf,obuf);
		cptr = sbrk2(strlen(label) + 1);
		strcpy(cptr,label);
		equtab[equcount++] = cptr;
		if (equcount >= EQUMAX)
			abort(
		  "Too many EQU lines..increase 'EQUMAX' and recompile ZCASM");
		return;
	   }

	   if (streq(op,"EXTERNAL")) {
		if (!doingfunc) abort(
		 "'External's for a function must appear inside the function");
		if (pastnfs) error(
		 "Externals must all be together at start of function\n");
		for (i = 0; i < argcnt; i++) {
			nflist[nfcount++] = txtbufp;
			strcpy(txtbufp,args[i]);
			bumptxtp(args[i]);
		}
		if (nfcount >= NFMAX) {
		  printf("Too many external functions in function \"%s\"\n",
					cfunam);
		  abort("Change the NFMAX constant and recompile ZCASM");
		}
		return;
	   }

	   if (streq(op,"FUNCTION")) {
		if (!fcount) {
			if (verbose)
			fputs2("\n; dummy external data information:\n",obuf);

			fputs2("\tORG\tTPALOC+200H\n",obuf);
			fputs2("\tDEFB\t0,0,0,0,0\n",obuf);
		}

		if (doingfunc) {
			printf("'Function' op encountered in a function.\n");
			abort("Did you forget an 'endfunc' op?");
		}
		if (!argcnt)
			abort("A name is required for the 'function' op");

		cfunam = sbrk2(strlen(args[0]) + 1);
		fnames[fcount++] = cfunam;
		init_innum();
		strcpy(cfunam,args[0]);

		printf("Processing the %s function...          \n",cfunam);

		doingfunc = 1;
		txtbufp = txtbuf;
		labcount = 0;
		nfcount = 0;
		pastnfs = 0;

		nflist[nfcount++] = txtbufp;  /* Use function name as first */
		strcpy(txtbufp,cfunam);       /* needed func. */
		bumptxtp(cfunam);



		if (verbose)
			fprintf2(obuf,"\n\n; The \"%s\" function:\n",cfunam);

		fprintf2(obuf,"BEG$%02d\tEQU\t$-TPALOC\n",fcount-1);
		return;
	   }

	   if (streq(op,"ENDFUNC") || streq(op,"ENDFUNCTION")) {
		if (!doingfunc)
		  abort("'Endfunc' op encountered while not in a function");

		if (!pastnfs) flushnfs();    /* flush needed function list */
		fprintf2(obuf,"END$%02D\tEQU\t$\n",fcount-1);
		doreloc();		     /* flush relocation parameters */

		for (i = 0; i < labcount; i++)	/* detect undefined labels */
		  if (!lablist[i].defined) {
			printf("The label %s in function %s is undefined\n",
					lablist[i].labnam,cfunam);
			errf = 1;
		  }
		doingfunc = 0;
		return;
	   }
	}
				/* No special pseudo ops, so now process
				   the line as a line of assemby code: 	*/

	if (streq(op,"END")) return;		/* don't allow "end" yet     */

	if (!doingfunc || (!label && !op))	/* if nothing interesting on */
		return fputs2(linbuf,obuf);	/* line, ignore it	*/

	if (!pastnfs)				/* if haven't flushed needed */
		flushnfs();			/* function list yet, do it  */

						/* check for possible label  */
	if (label) {
		fprintf2(obuf,"L%02d$%02d\tEQU\t$-STR$%02d\n",
		   fcount-1, innumerate(label), fcount-1);
		for (i=0; linbuf[i]; i++)
			if (isspace(linbuf[i]) || linbuf[i] == ':')
				break;
			else
				linbuf[i] = ' ';
		if (linbuf[i] == ':') linbuf[i] = ' ';
		for (i = 0; i < labcount; i++)	 /* check if in label table  */
		  if (streq(label,lablist[i].labnam)) {	       /* if found,  */
			if (lablist[i].defined) {  /* check for redefinition */
				error("Re-defined label:");
				printf("%s, in function %s\n",
						lablist[i].labnam,cfunam);
			}
			 else
				lablist[i].defined = 1;
			goto out;
		  }
		lablist[i].labnam = txtbufp;	/* add new entry to	*/
		lablist[i].defined = 1;		/* label list		*/
		strcpy(txtbufp,label);
		bumptxtp(label);
		labcount++;
	}
out:
	if (!op) return fputs2(linbuf,obuf);	/* if label only, all done   */

						/* if a non-relocatable op,  */
	if (norelop(op)) return fputs2(linbuf,obuf);	/* then we're done   */

	if (argcnt && doingfunc)
	  for (i = 0; i < argcnt; i++) {
		if (gpcptr = isef(args[i]))
		   sprintf(workbuf,"F%02d$%02d-STR$%02d",fcount-1,
			innumerate(gpcptr),fcount-1);
		else if (norel(args[i])) continue;
		else {
			sprintf(workbuf,"L%02d$%02d",fcount-1,
				innumerate(args[i]));
			for (j = 0; j < labcount; j++)
				if (streq(args[i],lablist[j].labnam))
					goto out2;
			lablist[j].labnam = txtbufp;	/* add new entry to */
			lablist[j].defined = 0;		/* label list	*/
			strcpy(txtbufp,args[i]);
			bumptxtp(txtbufp);
			labcount++;
		}		   

	out2:
		if((streq(op,"JR")) || (streq(op,"DJNZ"))){/* odd z80 case */
			sprintf(temp_string,"+STR$%02d",fcount-1);
			strcat(workbuf,temp_string);
		}
		replstr(linbuf, workbuf, args[i] - pbuf, strlen(args[i]));

		if (streq(op,"DEFW") || streq(op,"DW")) {
		  fprintf2(obuf,"R%02d$%02d\tEQU\t$-STR$%02d\n",
		    fcount-1,relblc++,
		      fcount-1);
		    if (argcnt > 1)
		      error("Only one relocatable value allowed per DEFW\n");
		}
		else
			{
			fputs2(linbuf,obuf);
			if ((!streq(op,"JR")) && (!streq(op,"DJNZ")))/* z80  */
			     fprintf2(obuf,"R%02d$%02d\tEQU\t$-2-STR$%02d\n",
				    fcount-1,relblc++,
					fcount-1);
			return;
			}
		break;
	  }
	fputs2(linbuf,obuf);
}


/*
	Test for ops in which there may be a need
	for generation of relocation parameters. 
	Note that this is somewhat different from the 
	original CASM in that the list contains ops
	that may need attention rather than those
	gauraunteed not to.
*/

norelop(op)
char *op;
{
	if (streq(op,"LD")) return 0;
	if (streq(op,"CALL")) return 0;
	if (streq(op,"JP")) return 0;
	if (streq(op,"JR")) return 0;
	if (streq(op,"DJNZ")) return 0;
	return 1;
}


flushnfs()
{
	int i,j, length;

	pastnfs = 1;
	relblc = 0;

	if (verbose)
		fputs2("\n\n; List of needed functions:\n",obuf);

	for (i=1; i < nfcount; i++) {
		strcpy(workbuf,"\tDEFB\t'");
		length = strlen(nflist[i]);
		length = length < 8 ? length : 8;
		for (j = 0; j < length - 1; j++)
			workbuf[7+j] = nflist[i][j];
		workbuf[7+j] = '\0';
		fprintf2(obuf,"%s','%c'+80H\n",workbuf,nflist[i][j]);
	}

	fputs2("\tDEFB\t0\n",obuf);

	if (verbose)
		fputs2("\n; Length of body:\n",obuf);

	fprintf2(obuf,"\tDEFW\tEND$%02d-$-2\n",fcount-1);

	if (verbose)
		fputs2("\n; Body:\n",obuf);

	fprintf2(obuf,"STR$%02d\tEQU\t$\n",fcount-1);
	if (nfcount > 1) {
		fprintf2(obuf,"R%02d$%02d\tEQU\t$+1-STR$%02d\n",
			fcount-1,relblc++,fcount-1);
		fprintf2(obuf,"\tJP\tSTC$%02d-STR$%02d\n",
			fcount-1,fcount-1);
	}
	fprintf2(obuf,"F%02d$%02d\tEQU\tSTR$%02d\n",fcount-1,
		innumerate(cfunam),fcount-1);
	for (i=1; i < nfcount; i++)
		fprintf2(obuf,"F%02d$%02d:\tJP\t0\n",fcount-1,
			innumerate(nflist[i]));
	fprintf2(obuf,"\nSTC$%02d\tEQU\t$\n",fcount-1);
}


doreloc()
{
	int i;

	if(verbose)
		fputs2("\n; Relocation parameters:\n",obuf);

	fprintf2(obuf,"\tDEFW\t%d\n",relblc);
	for(i = 0; i < relblc; i++)
		fprintf2(obuf,"\tDEFW\tR%02d$%02d\n",fcount-1,i);
	fputs2("\n",obuf);
}


putdir()
{
	int i,j, length;
	int bytecount;

	bytecount = 0;

	fputs2("\n\tORG\tTPALOC\n\n; Directory:\n",obuf);
	for (i = 0; i < fcount; i++) {
		strcpy(workbuf,"\tDEFB\t'");
		length = strlen(fnames[i]);
		length = length < 8 ? length : 8;
		for (j = 0; j < length - 1; j++)
			workbuf[7+j] = fnames[i][j];
		workbuf[7+j] = '\0';
		fprintf2(obuf,"%s','%c'+80H\n",workbuf,fnames[i][j]);
		fprintf2(obuf,"\tDEFW\tBEG$%02d\n",i);
		bytecount += (length + 2);
	}
	fputs2("\tDEFB\t80H\n\tDEFW\tEND$CRL\n",obuf);

	bytecount += 3;
	if (bytecount > DIRSIZE) {
		printf("CRL Directory size will exceed 512 bytes;\n");
		printf("Break the file up into smaller chunks, please!\n");
		exit(-1);
	}
}


initequ()		/* modified to reflect the z80 mnemonics */
{
	equtab[0] = "A";
	equtab[1] = "B";
	equtab[2] = "C";
	equtab[3] = "D";
	equtab[4] = "E";
	equtab[5] = "H";
	equtab[6] = "L";
	equtab[7] = "SP";
	equtab[8] = "AF";
	equtab[9] = "AND";
	equtab[10]= "OR";
	equtab[11]= "MOD";
	equtab[12]= "NOT";
	equtab[13]= "XOR";
	equtab[14]= "SHL";
	equtab[15]= "SHR";
	equtab[16]= "HL";
	equtab[17]= "BC";
	equtab[18]= "DE";
	equtab[19]= "IX";
	equtab[20]= "IY";
	equtab[21]= "I";
	equtab[22]= "R";

	equtab[23]= "Z";
	equtab[24]= "NZ";
	equtab[25]= "P";
	equtab[26]= "M";
	equtab[27]= "PO";
	equtab[28]= "PE";
	equtab[29]= "C";
	equtab[30]= "NC";

	equcount = 31;
}


int isidchr(c)	/* return true if c is legal character in identifier */
char c;
{	
	return isalpha(c) || c == '$' || isdigit(c) || c == '.' || c == '_';
}


int isidstrt(c)	/* return true if c is legal as first char of identifier */
char c;
{
	return isalpha(c) || c == '_';
}


int streq(s1, s2)	/* return true if the two strings are equal */
char *s1, *s2;
{
	if (*s1 != *s2) return 0;	/* special case for speed */
	while (*s1) if (*s1++ != *s2++) return 0;
	return (*s2) ? 0 : 1;
}


skip_wsp(strptr)	/* skip white space at *strptr and modify the ptr */
char **strptr;
{
	while (isspace(**strptr)) (*strptr)++;
}


strcpy2(s1,s2)	/* copy s2 to s1, converting to upper case as we go */
char *s1, *s2;
{
	while (*s2)
	     *s1++ = toupper(*s2++);
	*s1 = '\0';
}


/*
	General-purpose string-replacement function:
		'string'	is pointer to entire string,
		'insstr'	is pointer to string to be inserted,
		'pos'		is the position in 'string' where 'insstr'
				is to be inserted
		'lenold'	is the length of the substring in 'string'
				that is being replaced.
*/

replstr(string, insstr, pos, lenold)
char *string, *insstr;
{
	int length, i, j, k, x;

	length = strlen(string);
	x = strlen(insstr);
	k = x - lenold;
	i = string + pos + lenold;
	if (k) movmem(i, i+k, length - (pos + lenold) + 1);
	for (i = 0, j = pos; i < x; i++, j++)
		string[j] = insstr[i];
}


error(msg,arg1,arg2)
char *msg;
{
	printf("\n\7%s: %d: ",cfilnam,lino);
	printf(msg,arg1,arg2);
	errf = 1;
}


abort(msg,arg1,arg2)
char *msg;
{
	error(msg,arg1,arg2);
	putchar('\n');
	if (cbufp == incbuf) fclose(incbuf);
	fclose(fbuf);
	if (*SUBFILE)
		unlink(SUBFILE);
	exit(-1);
}


sbrk2(n)	/* allocate storage and check for out of space condition */
{
	int i;
	if ((i = sbrk(n)) == ERROR)
		abort("Out of storage allocation space\n");
	return i;
}

bumptxtp(str)	/* bump txtbufp by size of given string + 1 */
char *str;
{
	txtbufp += strlen(str) + 1;
	if (txtbufp >= txtbuf + (TXTBUFSIZE - 8))
	 abort("Out of text space. Increase TXTBUFSIZE and recompile ZCASM");
}


int norel(id)	/* return true if identifier is exempt from relocatetion */
char *id;
{
	if (isequ(id)) return 1;
	return 0;
}


int isequ(str)	/* return true if given string is in the EQU table */
char *str;
{
	int i;
	for (i = 0; i < equcount; i++)
		if (streq(str,equtab[i]))
			return 1;
	return 0;
}


char *isef(str)	/* return nflist entry if given string is an external */
char *str;	/* function name */
{
	int i;
	for (i = 0; i < nfcount; i++)
		if (streq(str,nflist[i]))
			return nflist[i];
	return 0;
}

int hasdot(str)	/* return true if given string has a dot in it */
char *str;
{
	while (*str)
		if (*str++ == '.')
			return TRUE;
	return FALSE;
}

fputs2(arg1,arg2)
{
	if (fputs(arg1,arg2) == ERROR)
		abort("Out of disk space for output file.");
}

fprintf2(arg1,arg2,arg3,arg4,arg5)
{
	if (fprintf(arg1,arg2,arg3,arg4,arg5) == ERROR)
		abort("Out of disk space for output file.");
}

	/* the following two functions manipulate a simple linear
	lookup symbol table to provide SHORT unique identifiers
	for consumption by assemblers that would not allow the long
	labels produced by the original CASM.
	*/

init_innum()		/* initializes the symbol table to empty */
{
	current_syms = 0;
	next_sym = sym_buff;
}

innumerate(string)   /* Assigns a unique # to label to allow assemblers	*/
{		     /* with only 6 significant label characters to work */
	int i;
	/* look for symbol */
	for(i=0; (i<current_syms) && (i<MAXSYMS); i++)
	{
		if (!strcmp(string,str_name[i])) return i;		
	}
	/* enter symbol */
	if (!((next_sym - sym_buff) < (MAXCHARS-strlen(string)))){
		puts("Out of space in symbol shortening table...");
		unlink(onambuf);
		if (*SUBFILE) unlink(SUBFILE);
		exit();
		}
	if (current_syms == MAXSYMS){
		puts("Too many entries in symbol shortening table...");
		unlink(onambuf);
		if (*SUBFILE) unlink(SUBFILE);
		exit();
		}
	current_syms++;
	str_name[i] = next_sym;
	strcpy(str_name[i],string);
	next_sym += strlen(string) +1;
	return i;
}
