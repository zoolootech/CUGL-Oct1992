/*	Starting module of Tiny c compiler by Ron Cain	*/
/* Modified Feb. 10,1981 per P.L. Woods article in DDJ #52 */
/* 	Functions newfunc and getarg(t)	affected	*/
/*
	DEC 25,1981	repaired break function, module ca.c
 
	May 5,1982	Modified declloc(typ) not to update the
	the stack until all allocations have been performed . See
	Doctor Dobb's #67 (May, 1982) in the letters column.
 
	July 10,1982	Re-repaired break function. Use of "==" instead
	of "=" in "ptr = readwhile() " if statement was problem.
 
	July 13,1982	Changed stack offset in getarg(t) for character
	variable from 0 to +1 due to byte swap between 8080 and 6809
	machines.
 
*/
/* Other modules are 	CDEF.C	Definitions		*/
/*			CB.C	Midsection of compiler	*/
/*			CD.C	End section of compiler */
#INCLUDE CDEF.H
/* */
/*							*/
/*	Start CC1					*/
/*							*/
/* Compiler begins execution here			*/
/*							*/
main()
	{
	glbptr	=startglb;	/* clear global symbols */
	locptr=startloc;	/* clear local symbols	*/
	wqptr=wq;		/* clear while queue	*/
	macptr=			/* clear the macro pool	*/
	litptr=			/* clear the literal pool*/
	sp =			/* stack ptr (relative)	*/
	errcnt=			/* no errors (yet)	*/
	eof =			/* not end of file	*/
	input =			/* no input file	*/
	input2 =		/* or include file	*/
	output =		/* no open units	*/
	ncmp =			/* no open compound stmt*/
	lastst =		/* no last statement	*/
	quote[1]=
		0;	/*   all set = to zero		*/
	quote[0]='"';		/* fake a quote literal	*/
	cmode=1;		/* enable preprocessing	*/
/*							*/
/*	Compile body					*/
/*							*/
	ask();			/* get user options	*/
	openout();		/* get an output file	*/
	openin();		/* and initial input file */
	header();		/* intro code		*/
	parse();		/* process all input	*/
	dumplits();		/* dump literal pool	*/
	dumpglbs();		/* and all static memory*/
	inclrn();		/* Bring in the run time package */
	errorsummary();		/* summarize errors	*/
	trailer();		/* follow-up code	*/
	closeout();		/* close the output file (if any) */
	return ;		/* exit to system		*/
}
/* 	Process all input text		*/
/* */
/* At this level, only static declarations,defines,includes,	*/
/* and function definitions are legal...			*/
parse()
	{
	while	(eof==0)		/* do until no more input */
		{ 
		if(amatch("char",4)){declglb(cchar);ns();}
		else	if(amatch("int",3)){declglb(cint);ns();}
		else	if(match("#ASM"))doasm();
		else	if(match("#INCLUDE")) doinclude();
		else	if(match("#DEFINE"))addmac();
		else	newfunc();
		blanks();		/*force eof if pending */
		}
	}
/*					*/
/*	Dump the literal pool		*/
/*					*/
dumplits()
	{
	int	j,k;
	if (litptr==0) return;		/* if nothing there, exit. */
	printlabel(litlab);col();	/* print literal label	*/
	k=0;				/* init an index ...	*/
	while (k<litptr)		/* to loop with		*/
		{
		defbyte();		/* pseudo-op to define byte */
		j=10;			/* max bytes per line	*/
		while(j--)
		{
		outdec((litq[k++]&127));
		if ((j==0) || (k>=litptr))
			{
			nl();		/* need carriage return	*/
			break;
			}
		outbyte (',');		/* separate bytes	*/
		}
	}
}
/*	Dump all static variables	*/
/*					*/
/*					*/
dumpglbs()
	{
	int	j;
	if (glbflag==0) return;		/* no dump if user said no */
	cptr=startglb;
	while(cptr<glbptr) {
		if (cptr[ident]!=function)
			/* do if anything but function */
			{ outstr(cptr);col(); /* output name as label..*/
			defstorage();	/* define storage */
			j=((cptr[offset]&255)+((cptr[offset+1]&255)<<8)); /* calculate number of bytes */
			if ((cptr[type]==cint)||(cptr[ident]==pointer))
				j=j+j;
			outdec(j);	/* neeed that many bytes */
			nl();
			}
		cptr=cptr+symsiz;
		}
	}
/* */
/*	Report errors to user	*/
/* */
errorsummary()
	{			/* see if anything left hanging */
	if (ncmp) error ("missing closing bracket");
		/* Open compound statment */
	nl();
	comment();
	outdec(errcnt);		/* total number of errors */
	outstr (" errors in compilation.");
	nl();
	}
/* */
/*	Get options from user	*/
/* */
ask()
	{ 
	int	k,num[1];
	kill();			/* clear input line */
	outbyte(12);		/* clear the screen */
	nl();nl();nl();		/* print banner		*/
	pl(" small c compiler by Ron Cain ");
	nl();
	/* see if the user wants to interleave the c text */
	/* in the form of comments for clarity.	 	  */
	pl("Do you want the c-text to appear?");
	gets(line);		/*retreive answere */
	ctext=0;		/* assume no	   */
	if ((ch()=='Y')||(ch()=='y'))
		ctext =1;	/* user said yes   */
	/* see if the user wants to allocate static variables by name in this module */
		pl(" Do you want the globals to be defined? ");
		gets(line);
		glbflag=0;
		if((ch()=='Y'||ch()=='y'))
			glbflag = 1;
	/* get the first allowable number for compiler-generated labels */
	while(1)
		{pl(" Starting number for labels?" );
		gets(line);
		if(ch()==0) {num[0]=0; break;}
		if(k=number(num)) break;
		}
	nxtlab=num[0];
	litlab=getlabel();	/* first label = literal pool */
	kill();			/* erase line		      */
	}
/* */
/*	Get output file name		*/
/* */
openout()
	{
	char	*ioptr;
	kill();		/* erase line	*/
	output = 0;		/* start with none	*/
	pl("Output filename?"); /* ask  ....		*/
	gets(line);		/* get filename		*/
	if(ch()==0)return;	/* none given		*/
	ioptr=line;		/* copy of line pointer	*/
	if((output=fcreat(line,iobuf0))==EOF)	/* if given, open it up */
		{output =0;		/* cannot open file */
		error(" Open failure ");
		}
	kill();				/* erase line	   */
	}
/* */
/*	Get (next) input file	*/
/* */
openin()
	{
	char *testptr;
	input=0;		/* none to start with */
	while (input==0)	/* any above 1 allowed */
		{kill();	/* clear any line	*/
		if(eof)break;	/* if user said none	*/
		pl("Input filename?");
		gets(line);
		testptr=line;
		if(ch()==0){eof=1;break;} /* none given ... */
		if((input=fopen(line,iobuf1))==EOF)
			{input=0;	/* cannot open it */
			pl ("Open failure");
			}
		}
	kill();			/* erase line		*/
	}
/* */
/*	Open an include file		*/
/* */
doinclude()
	{blanks();		/* skip over to name	*/
	if((input2=fopen(line+lptr,iobuf2))==EOF) {
		input2=0;
		error("Open failure on include file");
		}
	kill();			/* clear rest of line	*/
				/* so next read will come from new file (if open)	*/
	}
/*	Close the output file	*/
closeout()
	{if(output){fflush(iobuf0);fclose(iobuf0);};	/* if open,close it 	*/
	output=0;			/* and mark it closed	*/
	}
/* */
/*	Declare a static variable (ie, define it for use)	*/
/*  Makes an entry in the symbol table so subsequent references can call symbol by name */
/* */
declglb(typ)
	int	typ;
	{
	int	k,j;
	char	sname[namesize];
	while(1)
		{while(1)
			{if(endst())return;	/* do line	*/
			k=1;			/* assume one element */
			if(match("*"))		/* pointer ?	*/
				j=pointer;	/* yes		*/
			else	j=variable;	/* not a pointer*/
			if(symname(sname)==0)	/* name ok ?	*/
				illname();	/* illegal name */
			if(findglb(sname))	/* already there? */
				multidef(sname);/* multiple def.*/
			if(match("["))		/* array ?	*/
				{ k=needsub();	/* get size	*/
				if(k) j=array;	/* 10 = array	*/
				else j=pointer;	/* 0 = pointer	*/
				}
			addglb(sname,j,typ,k);	/* add symbol	*/
			break;
			}
		if (match(",")==0) return;	/* more ?	*/
		}
	}
/* */
/*	Declare local variables (i.e.,define for use)		*/
/* Works just like "declglb" but modifies machine stack and adds sumbol table entry with appropriate */
/* stack offset to find it again				*/
/* */
/*
	Modified to not update the stack on declaration until the
   entire argument list has been parsed. See DDJ #67 in letters column.
*/
declloc(typ)				/* type is cchar or cint */
	int typ;
	{
	int	k,j,newsp;
	char	sname[namesize];
/*	Init the temp stack pointer */
	newsp = sp;
	while(1)
		{
			if(endst())break;
			if(match("*"))
				j=pointer;
			else	j=variable;
			if(symname(sname)==0)
				illname();
			if(findloc(sname))
				multidef(sname);
			if(match("[")) {
				k=needsub();
				if(k)
					{j=array;
					if(typ==cint) k=k+k;
					}
				else {
					j=pointer;
					k=2;
				     }
				}
			else
				if((typ==cchar)&&(j!=pointer))
					k=1;
				else	k=2;
			/* change machine temp stack	*/
			newsp = newsp - k;
			addloc(sname,j,typ,newsp);
			if(match(",")==0) break;
			}
		/* change the machine stack for real */
		sp = modstk(newsp);
	}
/* */
/*	>>>> start of cc2 <<<<<		*/
/* Get required array size		*/
/* invoked when declared variable is followed by "["	*/
/* this routine makes subscript the absolute size of the array */
/* */
needsub()
	{
	int	num[1];
	if (match("]")) return 0;	/* null size	*/
	if (number(num)==0)		/* go after a number	*/
		{ error("must be a constant");	/* if it isn't	*/
		num[0]=1;		/* so force one	*/
		}
	if(num[0]<0)
		{error("negative size illegal");
		num[0]=(-num[0]);
		}
	needbrack("]");			/* force single dimension */
	return	num[0];			/* and return size	*/
	}
/* */
/* Begin a function */
/* Called from "parse", this routine tries to make a function out of what follows */
/* */
newfunc()
	{
	char	n[namesize],*ptr;
	if (symname(n)==0)
		{error("illegal function or declaration");
		kill();		/* invalidate line	*/
		return;
		}
	if(ptr=findglb(n))	/* already in symbol table ?	*/
		{
		if(ptr[ident]!=function) multidef(n); /* already variable by that name */
	else if(ptr[offset]==function) multidef(n); /* already function by that name   */
	else ptr[offset]=function; /* otherwise, we have what was earlier assumed to be a function */
	}
/* If not in table, define as function now	*/
else addglb(n,function,cint,function);
/* look for open paren for arguments	*/
if(match("(")==0)error("missing open paren");
outstr(n);col();nl();	/* print function name*/
locptr=startloc;	/* Added per P.L. Woods fix,DDJ #52 */
argstk=0;		/* init arg count	*/
while(match(")")==0)	/* then coun the arguments */
	/* any legal name bumps the argument count */
	{if(symname(n))		/* More Woods fixes, DDJ #52 */
		{ if(findloc(n)) multidef(n);
		  else
			{ addloc (n,0,0,argstk);
			  argstk=argstk + 2;
			}
		}
	else {error("illegal argument name");junk();}
	blanks();
	/* if not closing paren, should be comma   */
	if(streq(line+lptr,")")==0)
		{if(match(",")==0)
		error("expected a comma");
		}
	if(endst())break;
	}
/* Code to reset locptr to startloc deleted per P.L. Woods fix */
sp=0;				/* preset stack pointer	      */
while(argstk) /* user must now declare types of arguments	*/
	{if(amatch("char",4)){getarg(cchar);ns();}
	else if(amatch("int",3)){getarg(cint);ns();}
	else {error("wrong number or arguments");break;}
	}
if (statement()!=streturn)	/* doa statement, but if it'S a return,skip cleaning up the stack */
	{modstk(0);
	ret();
	}
sp=0;				/* reset stack pointer again	*/
locptr=startloc;		/* deallocate all locals	*/
}
/* */
/* Declare argument types	*/
/* Called from "newfunc" this routine adds an entry in the local symbol*/
/* table for each named argument	*/
/* Totally rewritten by P.L. Woods	*/
getarg(t)		/* t= cchar or cint	*/
	int t;
	{
	char n[namesize],c,*argptr;
	int j,argtop,legalname,address;
	argtop = argstk;
	while(1)
		{if(argstk==0)return;	/* no more arguments	*/
		if(match("*")) j=pointer;
			else j=variable;
		if((legalname = symname(n))==0) illname();
		if(match("["))		/* Is it a pointer ???*/
			/* It is a pointer, so skip all between [...] */
			{ while(inbyte() !=']')
				if(endst()) break;
			j=pointer;		/* add entry as pointer */
			}
		if(legalname)
			{if(argptr=findloc(n))
				/* add in details of the type and address of the name */
				{argptr[ident] =j;
				 argptr[type]  =t;
				 address=argtop - ((argptr[offset]&255)+((argptr[offset+1]&255)<<8));
/*
	If variable is a character, the address must be incremented 
	to access the low byte in the stack.
*/
			if(j == variable)
			  if(t == cchar)
				address++;
				argptr[offset]=address;
				argptr[offset+1]=address >> 8;
				}
			 else
				error("expecting argument name");
			}
		argstk=argstk - 2;
		if(endst()) return;
		if(match(",")==0) error ("expected comma");
		}
	}
/* */
/* 	STATEMENT PARSER	*/
/* Called whenever syntax requires a statement.	*/
/* Routine performs that statement and returnsa number telling which one*/
statement()
	{if((ch()==0) && (eof)) return;
	else if(amatch("char",4))
		{declloc(cchar);ns();}
	else if(amatch("int",3))
		{
		declloc(cint);
		ns();
		}
	else if(match("{")) compound();
	else if(amatch("if",2))
		{doif();lastst=stif;}
	else if(amatch("while",5))
		{dowhile();lastst=stwhile;}
	else if(amatch("return",6))
		{doreturn();ns();lastst=streturn;}
	else if(amatch("break",5))
		{dobreak();ns();lastst=stbreak;}
	else if(amatch("continue",8))
		{docont();ns();lastst=stcont;}
	else if(match(";"));				/* huh ? */
	else if(match("#ASM"))
		{doasm();lastst=stasm;}
	/* if nothing else, assume it's an expression	*/
	else {expression();ns();lastst=stexp;}
	return lastst;
	}
/* */
/*	Semicolon enforcer	*/
/* Big Tony weilds a large bludgeon to enforce those tricky little semicolons */
/* */
ns()
	{ if(match(";")==0) error("missing semicolon");
	}
/* */
/*	Compound statement	*/
/* allow any number or statements to fall between "()"	*/
/* */
compound()
	{
	++ncmp;		/* new level open	*/
	while(match("}")==0)
		if(eof) return;
		else statement();
	--ncmp;		/* close current level	*/
	}
/* */
/*	"if" statement				*/
/* */
doif()
	{
	int	flev,fsp,flab1,flab2;
	flev = locptr;			/* record current local stack 	*/
	fsp=sp;				/* record current stack pointer */
	flab1=getlabel();	/* get label fro false branch	*/
	test(flab1);		/* get expression, and branch if false */
	statement();		/* if true, do a statement	*/
	sp=modstk(fsp);		/* then clean up the stack	*/
	locptr=flev;		/* and deallocate any locals	*/
	if(amatch("else",4)==0)	/* if .... else ?		*/
		/* simple if... print false label		*/
		{printlabel(flab1);col();nl();
		return;		/*  and exit			*/
		}
	/* an "if .... else" statement	*/
	jump(flab2=getlabel());	/* jump around false code	*/
	printlabel(flab1);col();nl();	/* print false label	*/
	statement();			/* and do else clause	*/
	sp=modstk(fsp);			/* then clean up stk ptr*/
	locptr=flev;			/* deallocate locals	*/
	printlabel(flab2);col();nl();	/* and print true label */
	}
/* */
/*	While statement	*/
/* */
dowhile()
	{
	int	wq[4];		/* allocate local que	*/
	wq[wqsym]=locptr;	/* recordlocal level	*/
	wq[wqsp] = sp;		/* and stak ptr		*/
	wq[wqloop]=getlabel();	/* and looping label	*/
	wq[wqlab]=getlabel();	/* and exit label	*/
	addwhile(wq);		/* add entry to que	*/
				/* for break statement	*/
	printlabel(wq[wqloop]);col();nl(); /*loop break */
	test(wq[wqlab]);	/* see if true		*/
	statement();		/* if so,do a statement */
	jump(wq[wqloop]);	/* loop to label	*/
	printlabel(wq[wqlab]);col();nl(); /* exit label */
	locptr=wq[wqsym];	/* deallocate locals	*/
	sp=modstk(wq[wqsp]);	/* clean up stack pointer */
	delwhile();		/* delete queue entry	*/
	}
/* */
/*	"return" statement	*/
/* */
doreturn()
	{
	/* if not end of statement, get an expression	*/
	if(endst()==0) expression();
	modstk(0);		/* clean up stack	*/
	ret();			/* exit function	*/
	}
/* */
/*	"break" statement	*/
/* */
dobreak()
	{
	int	*ptr;
	/* see if any "whiles" are open	*/
	if((ptr=readwhile())==0) return;
	modstk((ptr[wqsp]));		/* clean up the stack	*/
	jump(ptr[wqlab]);	/* jump to exit label	*/
	}
/* */
/*	"continue" statement	*/
/* */
docont()
	{
	int	*ptr;
	/* see if any "whiles" are open	*/
	if ((ptr=readwhile())==0) return;	/* none open */
	modstk((ptr[wqsp]));		/* else clean up stack pointer */
	jump(ptr[wqloop]);		/* jump to loop label	*/
	}
/* */
/*	"ASM" pseudo statement	*/
/* enters mode where assembly language statements are passed intact throug parser */
/* */
doasm()
	{
	cmode=0;		/* mark mode as "asm" */
	while(1)
		{inline();	/* get and print line	*/
		if (match("#ENDASM")) break;	/* until... */
		if (eof) break;
		outstr(line);
		nl();
		}
	kill();			/* invalidate line	*/
	cmode=1;		/* set to parse mode	*/
	}
/*	>>>>>>>>> start of cc3 <<<<<<<<	*/
/* */
/* Perform a function call		*/
/* called from heir11, this routine will either call the named function,*/
/* or if the supplied ptr is zero, will call the contents of HL	*/
/* */
callfunction(ptr)
	char	*ptr;		/* symbol table entry (or zero) */
	{
	int	nargs;
	nargs=0;
	blanks();		/* already saw open paren	*/
	if(ptr==0)push();	/* calling HL	*/
	while(streq(line+lptr,")")==0)
	{if(endst()) break;
	expression();		/* get an argument	*/
	if(ptr==0)swapstk();	/* don't push address	*/
	push();			/* push argument	*/
	nargs=nargs+2;		/* count arguments	*/
	if (match(",")==0) break;
	}
	needbrack(")");
	if(ptr)call(ptr);
	else callstk();
	sp=modstk(sp+nargs);	/* clean up arguments	*/
	}
/* */
junk()
	{if(an(inbyte()))
		while(an(ch()))gch();
	else while(an(ch())==0)
		{if(ch()==0) break;
		gch();
		}
	blanks();
	}
/* */
endst()
	{blanks();
	return ((streq(line+lptr,";")||(ch()==0)));
	}
/* */
illname()
	{
	error("illegal symbol name");junk();
	}
/* */
multidef(sname)
	char *sname;
	{error("already defined");
	comment();
	outstr(sname);nl();
	}
/* */
/* */
needbrack(str)
	char *str;
	{if (match(str)==0)
		{error("missing bracket");
		comment();outstr(str);nl();
		}
	}
/* */
needlval()
{	error("must be lvalue");
}
findglb(sname)
	char	*sname;
	{char *ptr;
	ptr=startglb;
	while(ptr!=glbptr)
		{if(astreq(sname,ptr,namemax)) return ptr;
		ptr=ptr+symsiz;
		}
	return 0;
	}
findloc(sname)
	char *sname;
{
	char *ptr;
	ptr=startloc;
	while(ptr!=locptr)
		{if(astreq(sname,ptr,namemax))return ptr;
		ptr=ptr+symsiz;
		}
	return 0;
}
/* */
addglb(sname,id,typ,value)
	char *sname,id,typ;
	int  value;
	{
	char *ptr;
	if (cptr=findglb(sname))return cptr;
	if (glbptr >= endglb)
		{error("global symbol table overflow");
		return 0;
		}
	cptr=ptr=glbptr;
	while(an(*ptr++ = *sname++));	/* copy name	*/
	cptr[ident]=id;
	cptr[type]=typ;
	cptr[storage]=statik;
	cptr[offset]=value;
	cptr[offset+1]=value>>8;
	glbptr=glbptr+symsiz;
	return cptr;
	}
/* */
addloc(sname,id,typ,value)
	char *sname,id,typ;
	int value;
	{
	char *ptr;
	if(cptr=findloc(sname)) return cptr;
	if(locptr >= endloc)
		{error("local symbol table overflow");
		return 0;
		}
	cptr=ptr=locptr;
	while(an(*ptr++ = *sname++));	/* copy name */
	cptr[ident]=id;
	cptr[type]=typ;
	cptr[storage]=stkloc;
	cptr[offset]=value;
	cptr[offset+1]=value>>8;
	locptr=locptr+symsiz;
	return cptr;
	}
/* */
/*	test if input string is legal symbol name	*/
symname(sname)
	char *sname;	
	{
	int k;
	char c;
	blanks();
	if(alpha(ch())==0) return 0;
	k=0;
	while(an(ch())) sname[k++]=gch();
	sname[k]=0;
	return 1;
	}
/*	Return next available internal label number	*/
getlabel()
	{return(++nxtlab);}
/* 	Print specified number as label	*/
printlabel(label)
	int label;
{	outstr("cc");
	outdec(label);
}
/*	Test if given character is alpha	*/
alpha(c)
	char c;
{	c=c&127;
	return(((c>='a')&&(c<='z'))||((c>='A')&&(c<='Z'))||(c=='-'));
/* +++++ NOTE: editor would not allow under score above. Used - */
}
/*	Test if given character is numeric	*/
numeric(c)
	char c;
{	c=c&127;
	return((c>='0')&&(c<='9'));
}
/* Test if given character is alphanumeric	*/
an(c)
	char c;
{
	return((alpha(c))||(numeric(c)));
}
/* Print a carriage return and line feed only to console	*/
pl(str)
	char *str;
{	int k;
	k=0;
	putchar(eol);
	putchar(lf);		/* output carriage return and linefeed */
	while(str[k])
		putchar(str[k++]);
}
/* */
addwhile(ptr)
	int ptr[];
{	int k;
	if (wqptr==wqmax)
		{error("too many active whiles");return;}
	k=0;
	while (k<wqsiz)
		{*wqptr++ = ptr[k++];}
}
/* */
delwhile()
	{
	if(readwhile()) wqptr=wqptr-wqsiz;
	}
/* */
readwhile()
{
	if(wqptr==wq) 
		{error("no active whiles"); return 0;}
		else return (wqptr-wqsiz);
}
/* */
ch()
{
	return(line[lptr]&127);
}
nch()
{
	if(ch()==0) return 0;
		else return(line[lptr+1]&127);
}
gch()
{
	if(ch()==0) return 0;
		else return(line[lptr++]&127);
}
kill()
{
	lptr=0;
	line[lptr]=0;
}
inbyte()
{
	while(ch()==0)
		{if (eof) return 0;
		inline();
		preprocess();
		}
	return gch();
}
inchar()
{
	if(ch()==0)inline();
	if(eof) return 0;
	return(gch());
}
inline()
{
	char *ioptr;
	int k,unit;
	while(1)
		{
		if (input==0) openin();
		if(eof) return;
		ioptr=iobuf2;		/* init to include buffer first */
		if((unit=input2)==0) {unit=input;ioptr=iobuf1;}
		kill();
		while(((k=getc(ioptr)) !=26)&&(k != EOF))
			{
			if(k==eol) break;
			if(k!=lf) line[lptr++]=k;
			}
		line[lptr]=0;		/* append null */
		if((k==26)||(k==EOF))
			{fclose(ioptr);
			if(input2)input2=0;
				else {
					input=0;
					eof=1;	 /* set end of main file*/
					}
			}
		if(lptr)
			{if((ctext)&&(cmode))
				{comment();
				outstr(line);
				nl();
				}
			lptr=0;
			return;
			}
		}
	}
ееееееееееееееееееее