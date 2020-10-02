/*
HEADER:		Small-C source part 2;
FILENAME:	CC2.C;
AUTHORS:	Dieter H. Flunkert;
COMPILERS:Turbo C V2.0 Medium Memory Model;
SYSTEM: MSDOS, modified by Brian Brown, Nov 1989
*/
#include <stdio.h>
#include "ccdef.c"

/* define external functions */
extern int addglb(), addloc(), addwhile(), amatch(), blanks(), ch(),
  comment(), constexpr(), cout(), debug(), declloc(), defname(),
  defstorage(), delwhile(), doexpression(), dumplits(), endst(), entry(),
  errrpt(), findglb(), findloc(), gch(), getlabel(), getint(), illname(),
  inbyte(), inchar(), inline(), junk(), jump(), kill(), match(), modstk(),
  multidef(), needbrack(), nextsym(), nl(), number(), outbyte(), outdec(),
  outstr(), postlabel(), prelabel(), printlabel(), putint(),
  readwhile(), ret(), sout(), streq(), sw(), symname(), tab(), test();

/*  external variables */
extern int argstk, ctext, cmode, DEFDEBUG, declared, declstat, eof,
  first_func,lastst, litlab, litptr, lptr, monitor,
  nogo, noloc, ncmp, *startcomp, statlab, stdecl, stkp,
  stlab, swactive, swdefault, *swnext, *swend, asmtype;

extern char *cptr, *cptr2, line[linesize], *locptr, *glbptr, *statptr,symtab[symtbsz];

/* Get required array size, invoked when declared variable is followed
   by "[",  this routine makes subscript the absolute size of the array. */
needsub()
{
 int num[1];
 if(match("]"))return (0); /* null size */
 if (number(num)==0) /* go after a number */
  {errrpt("must be constant"); /* it isn't */
  num[0]=1;  /* so force one */
  }
 if (num[0]<0)
  {errrpt("negative size illegal");
  num[0]=(-num[0]);
  }
 needbrack("]");  /* force single dimension */
 return (num[0]);  /* and return size */
 }

/* Begin a function  */
/* Called from "parse" this routine tries to make a function out of what follows. */
newfunc(class)
int class;
{  /* next lines added by dieter flunkert 23 jan 1986 */
  int argtop;  /* top of argument stack */
  char n[namesize],*ptr;
#ifdef STGOTO
  nogo=noloc=0;
#endif
  if(monitor)
  {
    cout('\n',stderr);
    sout(line,stderr);
  }
  if (symname(n)==0)
  {
    errrpt("illegal function or declaration");
    kill(); /* invalidate line */
    return;
  }
 if(ptr=findglb(n)) /* already in symbol table ? */
  {
    if(ptr[ident]!=function)  multidef(n);
    /* already variable by that name */
    else if(ptr[offset]==function)multidef(n);
    /* already function by that name */
   else ptr[offset]=function;
    /* otherwise we have what was earlier*/
    /*  assumed to be a function */
  }
 /* if not in table, define as a function now */
 else addglb(n,function,cint,function);
 /* we had better see open paren for args... */
 if(match("(")==0)errrpt("missing open paren");
/* next lines added by dieter flunkert 23 jan 1986 */
 if(first_func) {
   defname(n);  /* define name of asm module */
   first_func = 0;
 }
 entry(n, class);
 if(DEFDEBUG) debug(n);
     /* ** Clear local stack pointer rev. 1P */
 locptr = startloc; /* ** 2 lines inserted  rev. 1P */
 argstk=0;  /* init arg count */
 while(match(")")==0) /* then count args */
  /* any legal name bumps arg count */
  {if(symname(n)) /* **+ Modification  rev. 1P */
   { if (findloc(n)) multidef(n);
   else { addloc(n, 0, 0, argstk);
    argstk = argstk + 2;
   }
  }  /* **- End of modification rev. 1P */
  else{errrpt("illegal argument name");junk();}
  blanks();
  /* if not closing paren, should be comma */
  if(streq(line+lptr,")")==0)
   {if(match(",")==0)
   errrpt("expected comma");
   }
  if(endst())break;
  }
/* ** Next line deleted      rev. 1P */
/* ** locptr=startloc; ** */ /* "clear" local symbol table*/
 stkp=0;   /* preset stack ptr */
/* next line added by dieter flunkert 23 jan 1986 */
 argtop=argstk; /* save top of argument stack */
 while(argstk)
  /* now let user declare what types of things */
  /* those arguments were */
  {if(amatch("char",4)){getarg(argtop,cchar);ns();}
  else if(amatch("int",3)){getarg(argtop,cint);ns();}
  else{errrpt("wrong number args");break;}
  }
 if(statement()!=streturn) /* do a statement, but if */
    /* it's a return, skip */
    /* cleaning up the stack */
  {modstk(0);
  ret();
  }
 stkp=0;   /* reset stack ptr again */
 locptr=startloc; /* deallocate all locals */
 if(litptr) {     /* dump literal pool */
	prelabel(litlab);
	dumplits(1);
	litlab=getlabel();
  }
}

/* Declare argument types, called from "newfunc" this routine adds an entry
   in the local symbol table for each named argument. argtop is not allowed
   to change value. This was the case if a function had a declaration like:
   int i; char c; */

getarg(argtop,t)  /* t = cchar or cint */
int argtop,t;
{
 int j, legalname, address;
 char n[namesize], c, *argptr;
 while(1)
  {if(argstk==0)return; /* no more args */
  if(match("*"))j=pointer;
   else j=variable;
  if((legalname = symname(n)) == 0) illname();
  if(match("[")) /* pointer ? */
  {
     while(inbyte()!=']')     /* it is a pointer, so skip all stuff between "[]" */
     if(endst())break;
    j=pointer;  /* add entry as pointer */
  }
  if (legalname) {
   if (argptr = findloc(n)) {
   /*  Add in details of the type and address of the name  */
   argptr[ident] = j;
   argptr[type] = t;
   address = argtop - (argptr[offset] + (argptr[offset+1]<<8));
   putint(address, argptr+offset, offsize);
   }
  else errrpt("Expected argument name");
  }
  argstk=argstk-2; /* cnt down */
  if (endst()) return;
  if (match(",") == 0) errrpt("expected comma");
  }
}

/* statement parser, called whenever syntax requires a statement this
   routine performs that statement and returns a number telling which one */
statement()
{
  int class;
  if ((ch==0) && (eof)) return;
  if(amatch("static",6)) {
   class=statik;
   if(++stdecl == 1)  /* there are static variables */
       jump(stlab=getlabel()); /* skip static declarations */
  }
  else class=stkloc;
  if(amatch("char",4))  {declloc(cchar,class);ns();}
  else if(amatch("int",3))   {declloc(cint,class);ns();}
  else {
    if(stdecl > 0) {
      postlabel(stlab);
      stdecl = 0;
    }
    if(declared >= 0) {
#ifdef STGOTO
      if(ncmp > 1) nogo=declared; /* disable goto if any */
#endif
      stkp=modstk(stkp - declared);
      declared = -1;
      }
    if(match("{"))               compound();
    else if(amatch("if",2))      {doif();lastst=stif;}
    else if(amatch("while",5))   {dowhile();lastst=stwhile;}
    else if(amatch("do",2))      {dodo();lastst=STDO;}
    else if(amatch("for",3))     {dofor();lastst=STFOR;}
    else if(amatch("switch",6))  {doswitch();lastst=STSWITCH;}
    else if(amatch("case",4))    {docase();lastst=STCASE;}
    else if(amatch("default",7)) {dodefault();lastst=STDEF;}
#ifdef STGOTO
    else if(amatch("goto", 4))   {dogoto(); lastst=STGOTO;}
    else if(dolabel())           lastst=STLABEL;
#endif
    else if(amatch("return",6))  {doreturn();ns();lastst=streturn;}
    else if(amatch("break",5))   {dobreak();ns();lastst=stbreak;}
    else if(amatch("continue",8)){docont();ns();lastst=stcont;}
    else if(match(";"))          ;
    else if(match("#asm"))       {doasm();lastst=stasm;}
    else if(match("#data"))      {dodata(); lastst=stdata;}
    else if(match("#const"))     {doconst(); lastst=stconst;}
    else if(match("#code"))      {docode(); lastst=stcode;}
    else                         {doexpression();ns();lastst=stexp;}
    }
  return lastst;
  }

dodo() {
  int wq[4], top;
  addwhile(wq);
  prelabel(top=getlabel());
  statement();
  needbrack("while");
  prelabel(wq[wqloop]);
  test(wq[wqlab],YES);
  jump(top);
  prelabel(wq[wqlab]);
  delwhile();
  ns();
}

dofor() {
  int wq[4], lab1, lab2;
  addwhile(wq);
  lab1=getlabel();
  lab2=getlabel();
  needbrack("(");
  if(match(";")==0) {
    doexpression();            /* expr 1 */
    ns();
    }
  prelabel(lab1);
  if(match(";")==0) {
    test(wq[wqlab],NO); /* expr 2 */
    ns();
    }
  jump(lab2);
  prelabel(wq[wqloop]);
  if(match(")")==0) {
    doexpression();            /* expr 3 */
    needbrack(")");
    }
  jump(lab1);
  prelabel(lab2);
  statement();
  jump(wq[wqloop]);
  prelabel(wq[wqlab]);
  delwhile();
}

doswitch()
{
  int wq[4], endlab, swact, swdef, *swnex, *swptr;
  swact=swactive;
  swdef=swdefault;
  swnex=swptr=swnext;
  addwhile(wq);
  needbrack("(");
  doexpression();      /* evaluate switch expression */
  needbrack(")");
  swdefault=0;
  swactive=1;
  jump(endlab=getlabel());
  statement();   /* cases, etc. */
  jump(wq[wqlab]);
  prelabel(endlab);
  sw();          /* match cases */
  while(swptr < swnext) {
    defstorage(cint>>2);
    printlabel(*swptr++);  /* case label */
    outbyte(',');
    outdec(*swptr++);      /* case value */
    nl();
    }
  defstorage(cint>>2);
  outdec(0);
  nl();
  if(swdefault) jump(swdefault);
  prelabel(wq[wqlab]);
  delwhile();
  swnext=swnex;
  swdefault=swdef;
  swactive=swact;
}

docase() {
  if(swactive==0) errrpt("not in switch");
  if(swnext > swend) {
    errrpt("too many cases");
    return;
  }
  prelabel(*swnext++ = getlabel());
  constexpr(swnext++);
  needbrack(":");
}

dodefault() {
  if(swactive) {
    if(swdefault) errrpt("multiple defaults");
    }
  else errrpt("not in switch");
  needbrack(":");
  prelabel(swdefault=getlabel());
}

#ifdef STGOTO
dogoto() {
  char ssname[namesize];
  if(nogo > 0) errrpt("not allowed with block-locals");
  else noloc = 1;
  if(symname(ssname)) jump(addlabel(ssname));
  else errrpt("bad label");
  ns();
}

dolabel() {
  int i;
  char ssname[namesize];
  char *savelptr;
  blanks();
  savelptr=lptr;
  if(symname(ssname)) {
    if(gch()==':') {
	  prelabel(addlabel(ssname));
      return 1;
	}
    else {
     i=savelptr-lptr+1;
     while(--i) inchar();
     }
    }
  return 0;
}

addlabel(ssname) char *ssname; {
  if(cptr=findloc(ssname)) {
    if(cptr[ident]!=LABEL) errrpt("not a label");
    }
  else cptr=addloc(ssname, LABEL, LABEL, getlabel(), LABEL);
  return (getint(cptr+offset, offsize));
  }
#endif

/* Semicolon enforcer, called whenever syntax requires a semicolon */
ns()
{
  if(match(";")==0)errrpt("missing semicolon");
}

/* Compound statement, allow any number of statements to fall between "{}" */
compound()
{
 int savcsp;
 char *savloc, *savstat;
 savcsp=stkp;
 startcomp=savloc=locptr;
 savstat=statptr;
 declared = stdecl = statlab = 0;
 ++ncmp;  /* new level open */
 while (match("}")==0)
  if(eof) {
    errrpt("no final }");
    break;
  }
  else statement(); /* do one */
 --ncmp;  /* close current level */
 if(lastst != streturn)
   modstk(savcsp);  /* delete local variable space */
 stkp=savcsp;
#ifdef STGOTO
  cptr=savloc;            /* retain labels */
  while(cptr < locptr) {
    cptr2=nextsym(cptr);
    if(cptr[ident] == LABEL) {
      while(cptr < cptr2) *savloc++ = *cptr++;
      }
    else cptr=cptr2;
    }
#endif
  declared = -1;
  locptr=savloc;
  statptr=savstat;
}

/*  "if" statement  */
doif()
 {
 int flev,fsp,flab1,flab2;
 flev=locptr; /* record current local level */
 fsp=stkp;  /* record current stk ptr */
 flab1=getlabel(); /* get label for false branch */
 test(flab1,YES); /* get expression, and branch false */
 statement(); /* if true, do a statement */
 stkp=modstk(fsp); /* then clean up the stack */
 locptr=flev; /* and deallocate any locals */
 if (amatch("else",4)==0) /* if...else ? */
  /* simple "if"...print false label */
  {
     prelabel(flab1);
     return;  /* and exit */
  }
 /* an "if...else" statement. */
 flab2=getlabel();
#ifdef STGOTO
 if((lastst != streturn)&&(lastst != STGOTO)) jump(flab2);
#else
 if(lastst != streturn) jump(flab2); /* jump around false code */
#endif
 prelabel(flab1);
 /* print false label */
 statement();  /* and do "else" clause */
 stkp=modstk(fsp);  /* then clean up stk ptr */
 locptr=flev;  /* and deallocate locals */
 prelabel(flab2);
 /* print true label */
}

/* "while" statement  */
dowhile()
{
 int lwq[4];  /* allocate local queue */
 addwhile(lwq);  /* add entry to queue */
    /* (for "break" statement) */
 prelabel(lwq[wqloop]); /* loop label */
 test(lwq[wqlab],YES); /* see if true */
 statement();  /* if so, do a statement */
 jump(lwq[wqloop]); /* loop to label */
 prelabel(lwq[wqlab]); /* exit label */
 delwhile();  /* delete queue entry */
}

/* "return" statement  */
doreturn()
{
  /* if not end of statement, get an expression */
  if(endst()==0)doexpression();
  modstk(0); /* clean up stk */
  ret();  /* and exit function */
}

/* "break" statement  */
dobreak()
{
 int *ptr;
 /* see if any "whiles" are open */
 if ((ptr=readwhile())==0) return; /* no */
 modstk((ptr[wqsp])); /* else clean up stk ptr */
 jump(ptr[wqlab]); /* jump to exit label */
}

/* "continue" statement  */
docont()
{
 int *ptr;
 /* see if any "whiles" are open */
 if ((ptr=readwhile())==0) return; /* no */
 modstk((ptr[wqsp])); /* else clean up stk ptr */
 jump(ptr[wqloop]); /* jump to loop label */
}

/* "asm" pseudo-statement, enters mode where assembly language statement are
   passed intact through parser */
doasm()
{
 cmode=0;  /* mark mode as "asm" */
 while (1)
  {inline(); /* get and print lines */
  if(eof) break;
  if (match("#endasm")){
   if (ctext){
    comment();
    outstr(line); nl();
   }
   break; /* until... */
  }
  outstr(line);
  nl();
  }
 kill();  /* invalidate line */
 cmode=1;  /* then back to parse level */
}

/* following added by Brian Brown to support ASxxx Assembler */
dodata()
{
 nl();
 if( asmtype == AS6809 ) {
   ot(".area  RAMDATA  (REL,CON)"); nl();
 }
 else if( asmtype == AS9 ){
  outstr("* .area  RAMDATA  (REL,CON)"); nl();
 }
 kill();  /* invalidate line */
}

doconst()
{
 nl();
 if( asmtype == AS6809 ) {
	ot(".area  CONST  (REL,CON)"); nl();
 }
 else if( asmtype == AS9 ) {
	outstr("*  .area  CONST  (REL,CON)"); nl();
 }
 kill();  /* invalidate line */
}

docode()
{
 nl();
 if( asmtype == AS6809 ) {
   ot(".area  ROMCODE  (REL,CON)"); nl();
 }
 else if( asmtype == AS9 )
 {
   outstr("*  .area  ROMCODE  (REL,CON)"); nl();
 }
 kill();  /* invalidate line */
}

