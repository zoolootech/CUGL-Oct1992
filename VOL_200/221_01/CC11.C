/* >>>>> start cc1 <<<<<<  */
/*     */
/* Compiler begins execution here */
/*     */
#ifdef CMD_LINE
main(argc, argv) int argc, *argv[];
{
  argcs=argc;
  argvs=argv;
#else
main()
{
#endif
 swend=(swnext=swq)+SWTABSZ+SWSIZ;
 first_func=1; /* first function */
 declared = -1;
 glbptr=startglb; /* clear global symbols */
 glbptr=startglb+symsiz*5;
 statptr=startstat;
 startcomp=endsearch=locptr=startloc; /* clear local symbols */
 stagelast=stage+stagesize-1; /* set to end of stageing buffer */
 wqptr=wq;  /* clear while queue */
 litptr=  /* clear literal pool */
 stkp =  /* stack ptr (relative) */
 errcnt=  /* no errrpts */
 eof=  /* not eof yet */
 input=  /* no input file */
 input2=  /* or include file */
 output=  /* no open units */
 ncmp=  /* no open compound states */
 lastst=  /* no last statement yet */
 iflevel= /* no ifdef yet */
 skiplevel= /* also no skip of course */
 stagenext=
 quote[1]=
 0;  /*  ...all set to zero.... */
 quote[0]='"';  /* fake a quote literal */
 cmode= /* enable preprocessing */
 macptr=  /* clear the macro pool */
 1;
 /*    */
 /* compiler body  */
 /*    */
 ask();   /* get user options */
#ifndef CMD_LINE
 openout();  /* get an output file */
 openin();  /* and initial input file */
#endif
 header();  /* intro code */
 parse();   /* process ALL input */
 trailer();  /* follow-up code */
 closeout();  /* close the output (if any) */
 errrptsummary();  /* summarize errrpts */
 return; /* then exit to system */
 }
/*     */
/* Process all input text  */
/*     */
/* At this level, only static declarations, */
/* defines, includes, and function */
/* definitions are legal... */
parse()
 {
 int storclass;
 while (eof==0)  /* do until no more input */
  {
  storclass = automatic;
  if(amatch("extern",6)) storclass = EXTERNAL;
  if(amatch("static",6)) storclass = statik;
  if(amatch(_char,4)) {declglb(cchar, storclass);ns();}
  else if(amatch(_int,3)){declglb(cint,storclass);ns();}
  else if(match("#asm"))doasm();
  else if(match("#include"))doinclude();
  else if(match("#define"))addmac();
  else newfunc(storclass);
  blanks(); /* force eof if pending */
  }
 }
/*     */
/* Dump the literal pool  */
/*     */
dumplits(siz) int siz;
 {int j,k;
 k=0;   /* init an index... */
 while (k<litptr) /*  to loop with */
  {defstorage(siz); /* pseudo-op to define byte */
  j=10;  /* max bytes per line */
  while(j--)
   {outdec(getint(litq+k, siz));
    k=k+siz;
    if ((j==0) | (k>=litptr))
    {nl();  /* need <cr> */
    break;
    }
   outbyte(','); /* separate bytes */
   }
  }
  litptr=0; 
 }

/*     */
/* Report errrpts for user  */
/*     */
errrptsummary()
 {
 /* see if anything left hanging... */
 if (ncmp) errrpt("missing closing bracket");
  /* open compound statement ... */
 output=stdout;
 cnl();
 outdec(errcnt); /* total # errrpts */
 outstr(" error(s) in compilation.");
 }
/*
** get run options
*/
#ifdef CMD_LINE
ask() {
  int i;
  char *argptr;
  i=nxtlab=0;
  litlab=getlabel(); /* first label=literal pool */
  kill();   /* erase line buffer */
  output=stdout;
#ifdef OPTIMIZE
  optimize=
#endif
#ifdef PHASE2
  monitor=pause=DEFDEBUG=
#endif
  ctext=NO;
  if(argcs==1) {
    sout("\nusage: cc <infile [>outfile] [-c]",stderr);
#ifdef OPTIMIZE
    sout(" [-o]",stderr);
#endif
#ifdef PHASE2
    sout(" [-m] [-p] [-t]",stderr);
#endif
    sout("\n",stderr);
    exit(0);
    }
  while(--argcs) {  /* process all input */
    argptr=argvs[++i];
    if(*argptr=='<')
      if((input=fopen(++argptr,"r"))==NULL) {
        errrpt("input file error");
        exit(0);
      }
      else continue;
    else if(*argptr=='>')
      if((output=fopen(++argptr,"w"))==NULL) {
        errrpt("output file errrpt");
        exit(0);
      }
      else continue;
    else if(*argptr++ == '-')
      if(upper(*argptr)=='C') ctext=YES;
#ifdef OPTIMIZE
      else if(upper(*argptr)=='O') optimize=YES;
#endif
#ifdef PHASE2
      else if(upper(*argptr)=='M') monitor=YES;
      else if(upper(*argptr)=='P') pause=YES;
      else if(upper(*argptr)=='T') DEFDEBUG=YES;
#endif
  }
  if(input==NULL) {
    sout("\nno input file...\n",stderr);
    exit(0);
    }
}
#else
/*     */
/* Get options from user  */
/*     */
ask()
 {
 int k,num[1];
 output=stdout;  /* init output to stdout */
 kill();   /* clear input line */
#ifndef VMS
 outbyte(CLS);  /* clear the screen */
#endif
 cnl();cnl();cnl(); /* print banner */
 pl("   * * *  small-c compiler  * * *");
 cnl();cnl();
 /* see if user wants to interleave the c-text */
 /* in form of comments (for clarity) */
 pl("Do you wish the c-text to appear? ");
 gets(line);  /* get answer */
 ctext=0;  /* assume no */
 if(upper(ch())=='Y')
  ctext=1; /* user said yes */
 litlab=getlabel(); /* first label=literal pool */
 kill();   /* erase line */
 }
/*     */
/* Get output filename  */
/*     */
openout()
 {
 kill();   /* erase line */
 output=0;  /* start with none */
 pl("Output filename? "); /* ask...*/
 gets(line); /* get a filename */
 if(ch()==0) {
   output=stdout;
   return; /* none given... */
 }
#ifndef VMS
  if((output=fopen(line,"w"))==NULL) /* Changed from UNIX to VMS  */
#else
 if((output=fdopen(creat(line,0,"rat=cr","rfm=var"),"w"))==NULL)
#endif
  {output=0; /* can't open */
  errrpt("Open failure");
  }
 kill();   /* erase line */
}
/*     */
/* Get (next) input file  */
/*     */
openin()
{
 input=0;  /* none to start with */
 while (input==0) /* any above 1 allowed */
  {kill(); /* clear line */
  if(eof)break; /* if user said none */
  pl("Input filename? ");
  gets(line); /* get a name */
  if(ch()==0)
   {eof=1;break;} /* none given... */
  if((input=fopen(line,"r"))==NULL)
   {input=0; /* can't open it */
   pl("Open failure");
   }
  }
 kill();  /* erase line */
 }
#endif
/*     */
/* Open an include file  */
/*     */
doinclude()
{
 blanks(); /* skip over to name */
 if((input2=fopen(line+lptr,"r"))==NULL)
  {input2=0;
  errrpt("Open failure on include file");
  }
 kill();  /* clear rest of line */
   /* so next read will come from */
   /* new file (if open */
}
/*     */
/* Close the output file  */
/*     */
closeout()
{ if(output)fclose(output); /* if open, close it */
 output=0;  /* mark as closed */
}
/*     */
/* Declare a static variable */
/*   (i.e. define for use)  */
/*     */
/* makes an entry in the symbol table so subsequent */
/*  references can call symbol by name */
declglb(typ,class)  /* typ is cchar or cint */
 int typ, class;
{ int k,j;char sname[namesize];
 while(1)
   {if(endst())return; /* do line */
   if(match("*")) { /* pointer ? */
    j=pointer; /* yes */
    k=0;
    }
   else {
    j=variable; /* no */
    k=1;
    }
   if (symname(sname)==0) /* name ok? */
    illname(); /* no... */
   if(findglb(sname)) /* already there? */
    multidef(sname);
   if(match("()")) j=function;
   else if (match("["))  /* array? */
    {k=needsub(); /* get size */
    j=array; /* !0=array */
    }
   if(class==EXTERNAL) declexternal(sname);
   else {
     entry(sname, class);
     j=initials(typ>>2, j, k);
   }
   addglb(sname,j,typ,k,class); /* add symbol */
   if(match(",")==0) return;  /* more? */
  }
}

/*
** declare local variables
** changed for static declaration
** dieter h. flunkert
** 29-oct-86
*/
declloc(typ, class)  int typ, class;  {
  int k,j; char sname[namesize], numstring[namesize];
#ifdef STGOTO
  if(noloc) errrpt("not allowed with goto");
#endif
  if(declared < 0) errrpt("must declare first in block");
  while(1) {
    while(1) {
      if(endst()) return;
      if(match("*")) j=pointer;
      else j=variable;
      if (symname(sname)==0) illname();
      endsearch=startcomp;
      if(findloc(sname) != 0) multidef(sname);
      endsearch=startloc;
      k=2;
      if (match("[")) {
        k=needsub();
        if(k || class == statik) {
         j=array;
         if(typ==cint)k=k+k;
        }
        else {j=pointer;
          k=2;
     }
      }
      else if(match("()")) j=function;
      else if((typ==cchar)&&(j==variable)) k=1;
      if(class == statik) {    /* dhf 29-oct-86 */
   convert(statlab++, numstring);
   entry(numstring, class);
   if(typ==cint) k = k>>1;
   j=initials(typ>>2, j, k);
   addstatic(sname, numstring, j, typ, k);
   addloc(sname, j, typ, 0); /* making it known as local */
      }
      else {
       declared = declared + k;
       addloc(sname, j, typ, stkp - declared);
      }
      break;
      }
    if (match(",")==0) return;
    }
  }

convert(n,s) int n; char *s; {
char *c, ch;

   c=s;
   do {
    *s++ = n % 10 + '0';
   } while((n /= 10) > 0);
   *s-- = '\0';
   while(c < s) {
    ch = *c;
    *c++ = *s;
    *s-- = ch;
   }
}

/*
** initialize global objects
*/
initials(siz, id, dim) int siz, id, dim; {
  int savedim;
  if(dim==0) dim = -1;
  savedim=dim;
  if(match("=")) {
    if(match("{")) {
      while(dim) {
        init(siz, id, &dim);
        if(match(",")==0) break;
        }
      needbrack("}");
      }
    else init(siz, id, &dim);
    }
  if((dim == -1)&&(dim==savedim)) {
     stowlit(0, siz=2);
    id=pointer;
    }
  dumplits(siz);
  dumpzero(siz*dim);
  return id;
  }

/*
** evaluate one initializer
*/
init(siz, id, dim) int siz, id, *dim; {
  int value;
  if(qstr(&value)) {
    if((id==variable)||(siz!=1))
      errrpt("must assign to char pointer or array");
    *dim = - (litptr - value) + *dim;
    if(id==pointer) point();
    }
  else if(constexpr(&value)) {
    if(id==pointer) errrpt("cannot assign to pointer");
    stowlit(value, siz);
    *dim = -1 + *dim;
    }
  }
