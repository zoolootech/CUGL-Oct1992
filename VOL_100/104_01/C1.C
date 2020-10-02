/*	>>>>> start ccl <<<<<<		*/
/*					*/
/*	Compiler begins execution here  */
/*					*/
#ifndef TRUE	/*see if need to use include file */
#include <c.def>
#endif
main()
	{
	glbptr=startglb;	/* clear global symbols */
	locptr=startloc;	/* clear local symbols */
	wqptr=wq;		/* clear while queue */
	macptr=			/* clear the macro pool */
	litptr=			/* clear literal pool */
	nxtlab= 		/* next label to assign */
	sp =			/* stackptr (relative) */
	errcnt=			/* no errors */
	eof=			/* not eof yet */
	input=			/* no input file */
	input2=			/* or include file */
	output=			/* no open units */
	ncmp=			/* no open compound states */
	ctext=			/* do not print text */
	lastst=0;		/* no last statement yet */
	cif=			/* state of #ifxxxx */
	cmode=1;		/* enable preprocessing */
	litlab=getlabel();	/* label number for literal poll */
	/*				*/
	/*	compiler body		*/
	/*				*/
	ask();			/* get user options */
	openout();		/* get an output file */
	openin();		/* and initial input file */
	header();		/* intro code */
	parse();		/* process ALL input */
	dumplits();		/* then dump literal pool */
	dumpglbs();		/* and all static memory */
	dumpextrn();		/* make all function defined extrn */
	errorsummary();		/* summarize errors */
	trailer();		/* follow-up code */
	closeout();		/* close the output */
	if (errcnt) unlink("a:$$$.sub"); /* if errors erase submit file */
	return;			/* then exit to system */
	}
/*					*/
/*	Process all input text		*/
/*					*/
/* At this level, only static declarations,	*/
/*	defines, includes, and function		*/
/*	definitions are legal...		*/
parse()
{
	while (!eof) {	/* do until no more input */
		if(amatch("char",4)) {
			declglb(cchar);
			ns();
			}
		else if(amatch("int",3)) {
			declglb(cint);
			ns();
			}
		else if(match("#asm")) doasm();
		else if(match("#include")) doinclude();
		else if(match("#define")) addmac(); 
		else if(match("#ifndef")) doifndef();
		else if(match("#ifdef")) doifdef(); 
		else newfunc();
		blanks();	/* force eof if pending */
		}
	}
/*					*/
/*	Dump the literal pool		*/
/*					*/
dumplits()
{
	int j,k;

	if (litptr==0) return;  /* if nothing there, exit...*/
	printlabel(litlab);
	col();			/* print literal label */
	k=0;			 /* init an index... */
	while (k<litptr) {	 /*      to loop with */
		defbyte();	 /* pseudo-op to define byte */
		j=10;	   /* max bytes per line */
		while(j--) {
			outdec(litq[k++]);
			if (j==0 || k>=litptr) {
				nl();	  /* need <cr> */
				break;
				}
			outbyte(',');   /* separate bytes */
			}
		}
	litptr=0;
	}
/*					*/
/*	Dump all static variables	*/
/*					*/
dumpglbs()
{
	int j;
	dsect("@globals");
	cptr=startglb;
	while(cptr<glbptr) {
		if (cptr[ident] != function) {
			/* do if anything but function */
			outstr(cptr);col();
				/* output name as label... */
			defstorage();   /*define storage */
			j=((cptr[offset]&255)+
				((cptr[offset+1]&255)<<8));
					/* calc # bytes */
			outdec(j);      /* need that many */
			nl();
			}
		cptr=cptr+symsiz;
		}
	}
/*					*/
/*	dump extrn function		*/
/*					*/
dumpextrn()
{
	char *ptr;

	ptr=startglb;
	while(ptr!=glbptr) {
		if (!ptr[offset]) extrn(ptr);
		ptr=ptr+symsiz;
		}
	}
/*					*/
/*	Report errors for user		*/
/*					*/
errorsummary()
{
	/* see if anything left hanging... */
	if (ncmp) error("missing closing bracket");
		/* open compund statment ... */
	if (!cif) error("missing closing #endif");
	nl();
	comment();
	outdec(errcnt); /* total # errors */
	outstr(" errors in compilation.");
	nl();
	}
/*					*/
/*	Get options from user		*/
/*					*/
ask()
{
	int k;
	kill();	 /* clear input line */
	pl("  * * *  small-c  version 1.1 compiler  * * *\n");
	/* see if user wants to interleasve the c-text	*/
	/*      inform of comments (for clarity) 	*/
	pl("Do you wish to c-text to appear? ");
	gets(line);			/* get answer */
	if(toupper(ch())=='Y') ctext=1;	/* user said yes */
	}
/*					*/
/*	Get output filename		*/
/*					*/
openout()
{
	kill();	 /* erase line */
	output=0;	       /* start with none */
	pl("Output filename? "); /* ask...*/
	gets(line);     /* get a filename */
	if(ch()==0)return;      /* none given... */
	if ((output=fcreat(line,fout)) == ERROR) {  
		output=0;      /* can't open */
		error("Open failure");
		}
	kill();		 /* erase line */
}
/*					*/
/*	Get (next) input file		*/
/*					*/
openin()
{
	input=0;		/* none to start with */
	while (!input)		/* any above 1 allowed */
		{kill();	/* clear line */
		if(eof)break;	/* if user said none */
		pl("Input filename? ");
		gets(line);	/* get a name */
		if(ch()==0) {
			eof=1;
			break;	/* none given... */
			}
		line1=0;
		if ((input=fopen(line,finp)) == ERROR) {
			input=0;	/* can't open it */
			pl("Open failure");
			}
		}
	kill();	 /* erase line */
	}
/*					*/
/*	Open an include file		*/
/*					*/
doinclude()
{
	char fname[31];	/* place to open file name */
	char fend;	/* ending character for file name */
	int  lenght;	/* size of file name */
	while(isspace(ch())) gch(); 	/* skip over to name */
	fend=' ';
	if (ch()=='<') {
		fend='>';
		gch();
		}
	if (ch()=='\"') {
		fend='\"';
		gch();
		}
	lenght=0;
	while(lenght<30 && ch() && ch()!=fend) {
		fname[lenght++]=ch();
		gch();
		}
	fname[lenght]=0;
	if (!(fend == ch() || (fend ==' ' &&  !ch()))) {
		error("invaild file name for include");
		kill();
		return;
		}
	if (!lenght) {
		error("missing file name for include");
		kill();
		return;
		}
	line2=0;
	if (input2) {
		error("nested include not aloud");
		kill();
		return;
		}
	if ((input2=fopen(fname,finp2)) == ERROR) { 
		error("Open failure on include file");
		input2=0;
		}
	kill();
	}
/*					*/
/*	#ifndef 			*/
/*					*/
doifndef()
{
	char sname[namesize];
	int lval[2];

	/* check to if define as constant */
	blanks();
	if (constant(lval)) {
		cif=FALSE;
		return;
		}

	/* check to see if vaild sysmbol */
	if (!symname(sname)) {
		error("invaild identifier");
		kill();
		return;
		}

	/* check to see if symbol is define */
	if (findloc(sname) || findglb(sname))
		cif=FALSE; else cif=TRUE;
	}
/*			*/
/*	#ifdef		*/
/*			*/
doifdef()
{
	char sname[namesize];
	int  lval[2];

	/* check to see if constant */
	blanks();
	if (!constant(lval)) {
		cif=FALSE;
		return;
		}

	/* check to see if vaild symbol	*/
	if (!symname(sname)) {
		error("invaild identifier");
		kill();
		}

	/* check to see if symbol is define */
	if (findloc(sname) || findglb(sname)) 
		cif=TRUE; else cif=FALSE;
	}
/*					*/
/*	Close the output file		*/
/*					*/
closeout()
{
	if (output) {	/* if open mark end, close it */
		putc(26,fout);
		fflush(fout);
		fclose(output);
		}
	output=0;	      /* mark as closed */
	}
/*					*/
/*	Declare a static variable	*/
/*	(i.e. define for use)		*/
/*					*/
/* makes an entry in the symbol table so subsequent */
/*  references can call symbol by name  */
declglb(typ)	    /* typ is cchar or cint */
	int typ;
{
	int j;
	int count;
	char sname[namesize];

	while(1) {
		count=0;	
		if (endst()) return;
		if(match("*")) {
			j=pointer;
			++count; 
			while(ch()=='*') {
				++count;
				gch();
				}
			}		
		else j=variable;
		if (!symname(sname)) illname();
		if(findglb(sname)) multidef(sname);
		data_parse(sname,typ,statik,j,count);
		if (match(",")==0) return; /* more? */
		}
	}
/*					*/
/*      Declare local variables		*/
/*      (i.e. define for use)		*/
/*					*/
/* works just like "declglb" but modifies machine stack */
/*	and adds symbol table entry with appropriate	*/
/*	stack offset to find it again			*/
declloc(typ)	    /* typ is cchar or cint */
int typ;
{
	int j;
	int count;
	char sname[namesize];
	while(1) {
		count=0;
		if (endst()) return;
		if (match("*")) { 
			j=pointer;
			++count;
			while(ch()=='*') {
				++count;
				gch();
				}
			}
		else j=variable;
		if (!symname(sname)) illname();
		if (findloc(sname)) multidef(sname);
		data_parse(sname,typ,stkloc,j,count);
		if (!match(",")) return;
		}
	}
/*						*/
/*	parse out data item and add to symbol	*/
/*	symbol table.				*/
/*						*/
/*	written April 8, 1981 By Mike Bernson	*/
/*						*/
data_parse(sname,typ,class_id,ident_typ,count)
char *sname;		/* pointer to name of symbol */
char typ;		/* type of data char or int */
char class_id;		/* typ of storage static automic */
char ident_typ;		/* type of varble pointer or varble */
int  count;		/* number of level of pointers */
{
	int size;	/* amount of memory needed for varble */
	int value;	/* used to hold value for offset */
	char *ptr;	/* pointer to free slot in symbol table */

	/* check to see if subscript */
	if (match("[")) {

		/* add 1 to inderict count */
		++count;

		/* check to see null subscript */
		if (size=needsub()) {

			/* subscript size not null */
			if (ident_typ==pointer) {

				/* array of pointer *temp[size] */
				size=size+size;
				}
			else {

				/* array temp[size] */
				ident_typ=array;
				if (typ==cint) size=size+size;
				}
			}
		else {

			/* null size subscript */
			ident_typ=pointer;
			size=2;
			}
		}
	else {
		/* no subscript */
		if (ident_typ==pointer) size=2;
		else if (typ == cint) size=2; else size=1;
		}
	/* add data element to symbol table */
	switch(class_id) {

		/* global symbol */
		case statik :
			if (glbptr>=endglb) {
				error("global symnol table overflow");
				return 0;
				}
			ptr=glbptr;
			glbptr+=symsiz;
			value=size;
			break;
	
		/* stack local varble */ 
		case stkloc :
			sp=modstk(sp-size);
			value=sp;
			if (locptr>=endloc) {
				error("local symbol table overflow");
				return 0;
				}
			ptr=locptr;
			locptr+=symsiz;
			break;

		/* arg stack */
		case stkarg :
			value=argstk;
			argstk-=2;
			if (locptr>=endloc) {
				error("local symbol table overflow");
				return 0;
				}
			ptr=locptr;
			locptr+=symsiz;
			break;
		}
	/* file in symbol table entry */
	strcpy(ptr+name,sname);
	ptr[ident]=ident_typ;
	ptr[type]=typ;
	ptr[storage]=class_id;
	ptr[offset]=value;
	ptr[offset+1]=value>>8;
	ptr[indcnt]=count;
	return TRUE;
	}
	/* a