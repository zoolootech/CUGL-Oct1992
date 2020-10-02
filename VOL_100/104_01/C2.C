

#ifndef TRUE	/* see if need include file */
#include <c.def>
#endif

/*					*/
/*	Get required array size		*/
/*					*/
/* invoked when declared variable is follwed by "[" */
/*      this routine makes subscript the absolute */
/*      size of the array. */
needsub()
{
	int num[1];
	if (match("]")) return 0;	/* null size */
	if (number(num)==0) {		/* go after a number */
		error("must be constant");     /* it isn't */
		while(ch() != ',' && ch() != ';' && ch()) gch();
		return 1; 
		}
	if (num[0]<0) {
		error("negative size illegal");
		num[0]=(-num[0]);
		}
	needbrack("]");		/* force single dimension */
	return num[0];		/* and return size */
	}
/*				*/
/*      Begin a function	*/
/*				*/
/* Called from "parse" this routine tries to make a function */
/*      out of what follows.    */
newfunc()
{
	char n[namesize],*ptr;
	if (symname(n) == 0) {
		error("illegal function or declaration");
		kill();
		return;
		}
	if (!match("(")) {
		error("illegal function or declaration");
		kill();
		return;
		}
	if(ptr=findglb(n)) {      /* already in symbol table ? */
		if (ptr[ident] != function) multidef(n);
			/* already variable by that name */
		else if(ptr[offset] == function) multidef(n);
			/* already function by that name */
		else {
			ptr[offset]=function;
			csect(n);
			}
			/* otherwise we have what was earlier*/
			/*  assumed to be a function */
		}
	/* if not in table, define as a function now */
	else {
		/* fill in gobal symbol table */
		if (glbptr>=endglb) 
			error("gobal symbol table overflow");
		else {
			ptr=glbptr;
			glbptr+=symsiz;
			strcpy(ptr+name,n);
			ptr[ident]=function;
		]=cint;
			ptr[offset]=function;
			ptr[offset1]=
			ptr[indcnt]=
			ptr[storage]=0;
			}
		csect(n);
		}

	argstk=0;	       /* init arg count */
	while(match(")") == 0) {    /* then count args */
		/* any legal name bumps arg count */
		if (symname(n)) argstk=argstk+2;
		else{
			error("illegal argument name");
			junk();
			}
		blanks();
		/* if not closing paren, should be a comma */
		if( streq(line+lptr,")") == 0) {
			if(match(",")==0)
			error("expected comma");
			}
		if(endst())break;
		}
	locptr=startloc;	/* "clear" local symbol table*/
	sp=0;		   /* preset stack ptr */
	while(argstk) {
		/* now let user declare what types of things */
		/*      those arguments were */
		if (amatch("char",4)) {
			getarg(cchar);
			ns();
			}
		else if (amatch("int",3)) {
			getarg(cint);
			ns();
			}
		else {
			error("wrong number args");
			break;
			}
		}
	if(statement()!=streturn)  {
			/* do a statement, but if */
			/* it's a return, skip */
			/* cleaning up the stack */
		modstk(0);
		ret();
		}
	sp=0;
	locptr=startloc;
	dumplits();
	litlab=getlabel();
	}
/*				      */
/*      Declare argument types	  */
/*				      */
/* called from "newfunc" this routine adds a entry in the */
/*      local symbol table for each named argument */
getarg(t)	       /* t = cchar or cint */
int t;
{
	char n[namesize],c;int j,count;
	while(1) {
		count=0;
		if (argstk == 0) return;   /* no more args */
		if (match("*")) {
			j=pointer;
			++count;
			while(ch()=='*') {
				++count;
				gch();
				}
			}
		else j=variable;
		if (symname(n) == 0) illname();
		if (findloc(n)) multidef(n);
		data_parse(n,t,stkarg,j,count);
		if(endst())return;
		if(match(",")==0)error("expected comma");
		}
	}
/*				      */
/*      Statement parser		*/
/*				      */
/* called whenever syntax requires      */
/*      a statement.		     */
/*  this routine performs that statement */
/*  and returns a number telling which one */
statement()
 {      if (!ch() && eof) return;
	else if(amatch("char",4)) {
		declloc(cchar);
		ns();
		}
	else if(amatch("int",3)) {
		declloc(cint);
		ns();
		}
	else if(match("{")) compound();
	else if(amatch("if",2)) {
		doif();
		lastst=stif;
		}
	else if(amatch("while",5)) {
		dowhile();
		lastst=stwhile;	
		}
	else if (amatch("for",3)) {
		dofor();
		lastst=stfor;
		}
	else if (amatch("switch",6)) {
		doswitch();
		lastst=stswitch;
		}
	else if(amatch("return",6)) {
		doreturn();
		ns();
		lastst=streturn;
		}
	else if(amatch("break",5)) {
		dobreak();
		ns();
		lastst=stbreak;
		}
	else if(amatch("continue",8)) {
		docont();
		ns();
		lastst=stcont;
		}
	else if(match(";"));
	else if(match("#asm")) {
		doasm();
		ns();
		lastst=stasm;
		}
	/* if nothing else, assume it's an expression */
	else {
		expression();
		ns();
		lastst=stexp;
		}
	return lastst;
	}
/*				      */
/*      Semicolon enforcer	      */
/*				      */
/* called whenever syntax requires a semicolon */
ns()    {if(match(";")==0)error("missing semicolon");}
/*					*/
/*	need semicolon			*/
/*	written	by Mike	Bernson	1/81	*/
/*					*/
needsem()
{
	if (match(";"))	return FALSE;
	error("Missing semicolon");
	junk();
	return TRUE;
	}
/*					*/
/*	need opening parn		*/
/*	written	by Mike	Bernson	1/81	*/
needoparn()
{
	if (match("("))	return FALSE;
	error("Missing left parnthis");
	junk();
	return TRUE;
	}
/*					*/
/*	need closing parn		*/
/*	written	by Mike	Bernson	1/81	*/
needcparn()
{
	if (match(")"))	return FALSE;
	error("Missing right parnthis");
	junk();
	return TRUE;
	}
/*					*/
/*	need opening brace		*/
/*	written	by Mike	Bernson	1/81	*/
needobrace()
{
	if (match("{"))	return FALSE;
	error("Missing left brace");
	junk();
	return TRUE;
	}
/*					*/
/*	need closing brace		*/
/*	written	by Mike	Bernson	1/81	*/
needcbrace()
{
	if (match("}"))	return FALSE;
	error("Missing right brace");
	junk();
	return TRUE;
	}
/*				      */
/*      Compound statement	      */
/*				      */
/* allow any number of statements to fall between "{}" */
compound()
	{
	++ncmp;	 /* new level open */
	while(match("}")==0)
		if(eof) return;
		else statement();
	--ncmp;	 /* close current level */
	}
/*				      */
/*	      "if" statement	  */
/*				      */
doif()
	{
	int flev,fsp,flab1,flab2;
	flev=locptr;    /* record current local level */
	fsp=sp;	 /* record current stk ptr */
	flab1= getlabel(); /* get label for false branch */
	test(flab1);    /*get expression, and branch false */
	statement();    /* if true, do a statement */
	sp=modstk(fsp); /* then clean up the stack */
	locptr=flev;    /* and deallocate any locals */
	if (amatch("else",4)==0)	/* if...else ? */
		/* simple "if"...print false label */
		{sprintlabel(flab1);
		return; /* and exit */
		}
	/* an "if...else" statement. */
	jump(flab2=getlabel()); /* jump around the false code */
	sprintlabel(flab1);    /* print true label */
	statement();		/* and do else clause */
	sp=modstk(fsp);		/* then clean up stack ptr */
	locptr=flev;		/* dellocate locals */
	sprintlabel(flab2);	/* print true label */
	}
/*				      */
/*      "while" statement	       */
/*				      */
dowhile()
	{
	int que[wqsiz];	    		/* allocate local queue */

	que[wqsym]=locptr;      	/* record local level */
	que[wqsp]=sp;	   		/* and stk ptr */
	que[wqloop]=			/* and looping label */
	que[wqend]=getlabel();		/* continue label */
	que[wqlab]=getlabel();		/* and exit label */
	addwhile(que);			/* add to looping stack	*/
	sprintlabel(que[wqloop]);	/* loop label */
	test(que[wqlab]);		/* see if true */
	statement();			/* if so, do a statement */
	jump(que[wqloop]);		/* loop	to label */
	sprintlabel(que[wqlab]);	/* exit label */
	locptr=que[wqsym];		/* deallocate locals */
	sp=modstk(que[wqsp]);		/* clean up stk	ptr */
	delwhile();			/* delete queue	entry */
	}
/*					*/
/*	"Switch" statement		*/
/*					*/
/*	written by Mike Bernson 1/81	*/
/*					*/
doswitch()
{
	int value[SWITCH_MAX];  /* value for case statemant */
	int label[SWITCH_MAX];  /* value for each label */
	int count,tenp; 	/* number of switches */
	int end_label;  	/* label for default */
	int label_switch;	/* used for switch label */
	int temp,val[2];
	int que[wqsiz];		/* local que area */
	count=0;		/* number of case statements */
	que[wqsym]=locptr;	/* local vable pointer */
	que[wqsp]=sp;		/* save current stack pointer */
	end_label=		/* default exit label */
	que[wqloop]=		/* looping label */
	que[wqlab]=		/* loop exit varble */
	que[wqend]=getlabel();	/* continue label */
	addwhile(que);		/* add to while stack */
	if (needoparn()) {	/* check to see	if"(" exits */
		delwhile();	/* no delete switch entry and */
		return;		/* return out of switch	*/
		}
	expression();		/* expression for switch */
	push();
	if (needcparn()) {	/* check for ")" */
		delwhile();	/* not fould detele que	entry */
		return;		/* and exit switch statemant */
		}
	if (needobrace()) {
		delwhile();
		return;
		}
	jump(label_switch=getlabel());
	sp=sp+2;

	while(1) {
 	if (amatch("case",4)) {
			if (const_exp(val) == 0 ) {
				error("Bad constant");
				continue;
				}
			if (count<SWITCH_MAX-2) {
				value[count]=val[0];
				sprintlabel(label[count++]=getlabel());
				}
			else error("Too many case statments");
			if (!match(":")) error("Missing colon");
			}
	else if (amatch("default",7)) {
		end_label=getlabel();
		sprintlabel(end_label);
		if (!match(":")) error("Missing colon");
		}
	else if (match("}")) {
		jump(que[wqlab]);	/* jump past switch data */
		temp=0;
		sprintlabel(label_switch);
		exec_switch(count,label_switch=getlabel(),end_label);
		sprintlabel(label_switch);
		while(temp<count) {
			defword();
			outdec(value[temp]);
			outstr(",");
			printlabel(label[temp++]);
			nl();
			}
		delwhile();
		sprintlabel(que[wqlab]);
		locptr=que[wqsym];
		sp=modstk(que[wqsp]);
		return;
		}
	else statement();
	}}
/*					*/
/*	"for" statement			*/
/*					*/
/*	written by Mike Bernson 1/81	*/
/*					*/
dofor()
{
	int que[wqsiz];		/* local que area */
	int status;		/* machine status after expession */

	que[wqsym]=locptr;    /* save locaL LEVEL */
	que[wqsp]=sp;		/* and stack pointer */
	que[wqloop]=getlabel();	/* looping label */
	que[wqlab]=getlabel();	/* loop exit varble */
	que[wqend]=getlabel();	/* loop end label */
	que[wqbody]=getlabel();	/* body for cody */
	addwhile(que);		/* add while to loop que */
	if (needoparn()) {	/* check for open parn */
		delwhile();	/* delete for entry from que */
		return;
		}
	expression();		/* init	express	*/
	if (needsem()) {	/* check for semcol */
		delwhile();	/* delete que entry */
		return;
		}
	sprintlabel(que[wqloop]);	/* control loop	label */
	status=expression();		/* loop control express */
	testjump(que[wqlab],status);	/* see if exit time */
	jump(que[wqbody]);	/* not time to exit do body */
	if (needsem()) {	/* check for semcol */
		delwhile();	/* delete 1 que	entry */
		return;
		}
	sprintlabel(que[wqend]);	/* print end of loop */
	expression();		/* end loop expression */
	jump(que[wqloop]);	/* do loop control expression */
	if (needcparn()) {
		delwhile();
		return;
		}
	sprintlabel(que[wqbody]);
	statement();
	jump(que[wqend]);
	sprintlabel(que[wqlab]);
	locptr=que[wqsym];
	sp=modstk(que[wqsp]);
	delwhile();
	}
/*				       */
/*      "return" statement	      */
/*				      */
doreturn()
	{
	/* if not end of statement, get an expression */
	if(endst()==0)expression();
	modstk(0);      /* clean up stk */
	ret();	  /* and exit function */
	}
/*				      */
/*      "break" statement	       */
/*				      */
dobreak()
	{
	int *ptr;
	/* see if any "whiles" are open */
	if ((ptr=readwhile())==0) return;       /* no */
	modstk(ptr[wqsp]);    /* else clean up stk ptr */
	jump(ptr[wqlab]);       /* jump to exit label */
	}
/*				      */
/*      "continue" statement	    */
/*				      */
docont()
	{
	int *ptr;
	/* see if any "whiles" are open */
	if ((ptr=readwhile())==0) return;       /* no */
	modstk((ptr[wqsp]));    /* else clean up stk ptr */
	jump(ptr[wqend]);      /* jump to end label */
	}
/*				      */
/*      "asm" pseudo-statement	  */
/*				      */
/* enters mode where assembly language statements are */
/*      passed intact through parser    */
doasm()
	{
	cmode=0;	/* mark mode as "asm" */
	while(1)
		{inline();      /* get and print lines */
		if (match("#endasm")) break;    /* until... */
		if(eof)break;
		outstr(line);
		nl();
		}
	kill();	 /* invalidate line */
	cmode=1;
	}



