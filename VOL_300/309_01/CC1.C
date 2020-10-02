/*
HEADER:		Small-C compiler source part 1;
VERSION:	2.3;
FILENAME:	CC1.C;
AUTHORS:	Dieter H. Flunkert;
COMPILERS:Turbo C V2.0 Medium Memory Model;
29-oct-86  static declaration added. dhf

Modified to MSDOS supporting ASxxx assemblers by Brian Brown Nov 1989

*/
#include <stdio.h>
#include <string.h>
#include "ccdef.c"

/*  Type definitions */
char _char[] = "char";
char _int[] = "int";

/* Now reserve some storage words  */
char stattab[stattbsize]; /* static symbol table */
char symtab[symtbsz]; /* symbol table */
char *glbptr,*locptr,*statptr;  /* ptrs to next entries */
char *endsearch;  /* compound start for searching locals */
char *startcomp;  /* start of pointer in current compound */
char swq[SWTABSZ];
int argcs;
char **argvs;
int wq[wqtabsz];  /* while queue */
int *wqptr;   /* ptr to next entry */
char litq[litabsz];  /* literal pool */
int litptr;   /* ptr to next entry */
char macq[macqsize];  /* macro string buffer */
int macptr;   /* and its index */
int stdecl; /* greater 1 if static variable was declared */
char stage[stagesize]; /* staging buffer */
char *stagenext;  /* next adr in stage */
char *stagelast;  /* last adr in stage */
char line[linesize];  /* parsing buffer */
char mline[linesize]; /* temp macro buffer */
int lptr,mptr;  /* ptrs into each */
/* Misc storage */
int nxtlab,  /* next avail label # */
 statlab,    /* next static label number */
 stlab,      /* internal static label */
 litlab,  /* label # assigned to literal pool */
 constval[2], /* [0] = constant identifier */
              /* [1] = constant value */
 monitor,   /* monitor compile */
 pause,   /* pause on error */
 DEFDEBUG,  /* debug feature enabled */
 first_func, /* 1 first function else 0 */
 declared, /* local symbol decl counter */
 stkp,  /* compiler relative stk ptr */
 argstk,  /* function arg sp */
#ifdef STGOTO
 nogo,  /* > 0 disables goto statements */
 noloc, /* > 0 disables block locals */
#endif
 ncmp,  /* # open compound statements */
 swactive, /* true inside a switch */
 swdefault, /* default label # else 0 */
 *swnext, /* address of next entry */
 *swend, /* address of last table entry */
 errcnt,  /* # errors in compilation */
 eof,  /* set non-zero on final input eof */
 iflevel, /* depth of #ifdef */
 skiplevel, /* zero, if no skip */
 optimize, /* non-zero if user wants optimization */
 ctext,  /* non-zero to intermix c-source */
 cmode,  /* non-zero while parsing c-code */
   /* zero when passing assembly code */
 lastst;  /* last executed statement type */
char quote[2]; /* literal string for '"' */
char *cptr,  /* work ptr to any char buffer */
 *cptr2;
int *iptr;  /* work ptr to any int buffer */

/* following changes for MSDOS, Brian Brown */
FILE *input,  /* iob # for input file */
     *output,  /* iob # for output file (if any) */
     *input2;  /* iob # for "include" file */

/* following change for supporting different assemblers, Brian Brown */
int asmtype;

/*  external functions used */
extern int addglb(), addloc(), addmac(), addstatic(), amatch(),
  blanks(), cnl(), ch(), constexpr(),
  defstorage(), doasm(), dumpzero(),
  entry(), endst(), errrpt(), declexternal(),
  findglb(), findloc(),
  getint(), getlabel(),
  header(), illname(),
  kill(),
  match(), multidef(),
  needbrack(),needsub(), newfunc(), nl(), number(), ns(),
  outbyte(), outdec(), outstr(),
  pl(), point(), printlab(),
  qstr(),
  sout(), stowlit(), symname(),
  trailer(),
  upper();

/* Compiler begins execution here */
main( argc, argv)
int argc;
char **argv;
{
  argcs = argc;
  argvs = argv;
  swnext = swq;
  swend=swnext+SWTABSZ+SWSIZ;
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
  quote[1]= 0;  /*  ...all set to zero.... */
  quote[0]='"';  /* fake a quote literal */
  cmode= /* enable preprocessing */
  macptr=  /* clear the macro pool */
  1;

  asmtype = AS6809;  /*asxxx assembler output is default*/

  /* compiler body  */
  ask();   /* get user options */
  header();  /* intro code */
  parse();   /* process ALL input */
  trailer();  /* follow-up code */
  closeout();  /* close the output (if any) */
  errrptsummary();  /* summarize errrpts */
  return; /* then exit to system */
}

/* Process all input text  At this level, only static declarations, */
/* defines, includes, and function definitions are legal... */
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

  /* these added by Brian Brown to support ASxxx .area directives */
  else if(match("#data")) dodata();
  else if(match("#const")) doconst();
  else if(match("#code")) docode();

  else if(match("#asm")) doasm();
  else if(match("#include")) doinclude();
  else if(match("#define")) addmac();
  else newfunc(storclass);
  blanks(); /* force eof if pending */
  }
}

/* Dump the literal pool  */
dumplits(siz) int siz;
{
 int j,k;
 k=0;   /* init an index... */
 while (k<litptr) /*  to loop with */
  {
    defstorage(siz); /* pseudo-op to define byte */
    j=10;  /* max bytes per line */
    while(j--)
    {
      outdec(getint(litq+k, siz));
      k=k+siz;
      if ((j==0) | (k>=litptr))
      {
        nl();  /* need <cr> */
        break;
      }
      outbyte(','); /* separate bytes */
     }
  }
  litptr=0;
 }

/* Report errrpts for user  */
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

myexit()
{
    printf("\nusage: cc infile.c outfile [-options]\n",stderr);
    exit(1);
}

ask()
{
  int i;
  int tempcount;
  char *argptr;
  i=0;
  nxtlab=0;
  litlab=getlabel(); /* first label=literal pool */
  kill();   /* erase line buffer */
  clrscr();
  printf(" Small-6809 C compiler for MSDOS systems V2.02\n");
  printf(" Adapted to MSDOS and AS6809 by Brian Brown, Nov 1989\n\n");
  output=stdout;
  optimize=
  monitor=pause=DEFDEBUG=
  ctext=NO;
  if(argcs == 1) {
    myexit();
  }
  else
  {
     if((input=fopen(argvs[1],"r"))==NULL) {
	 errrpt("input file error");
	 myexit();
     }
     printf("Compiling Source file: %s\n", argvs[1] );
     if((output=fopen(argvs[2],"w"))==NULL) {
	   output = stdout;
	   printf("No output file specified. Using console screen.\n");
     }
     else printf("Generating asm file: %s\n", argvs[2] );
     tempcount = argcs;
     tempcount = tempcount - 2; /* skip past main, source and dest args */
     while( tempcount-- )
     {
		argptr = argvs[argcs - tempcount];
		if(*argptr++ == '-')
	         if(upper(*argptr)=='C') {
			   ctext=YES;
			   printf("Inserting C text as comments in output file.\n");
	         }
	         else if(upper(*argptr)=='O') {
			   optimize=YES;
			   printf("Optimization is ON\n");
	         }
	         else if(upper(*argptr)=='M') {
			   monitor=YES;
			   printf("Monitor function is ON\n");
	         }
			 else if(upper(*argptr) == 'Z' ) {
				asmtype = AS9;
				printf("Assembler file is AS9 compatible\n");
	         }
	         else printf("Invalid option: %c\n", *argptr );
     }
  }
  if(input==NULL) {
    sout("\nno input file...\n",stderr);
    myexit();
  }
}

/* Open an include file  */
doinclude()
{
  blanks(); /* skip over to name */
  printf("\n\n#include %s", line+lptr);
  if((input2=fopen(line+lptr,"r"))==NULL)
  {  input2=0;
     errrpt("Open failure on include file");
  }
  kill();  /* clear rest of line */
   /* so next read will come from */
   /* new file (if open */
}

/* Close the output file  */
closeout()
{
   if( output == stdout )
	return;
   if(output)fclose(output); /* if open, close it */
   output=0;  /* mark as closed */
}

/* Declare a static variable (i.e. define for use)  makes an entry in the
   symbol table so subsequent references can call symbol by name */

declglb(typ,class)  /* type is cchar or cint */
int typ, class;
{
  int k,j;
  char sname[namesize];
  while(1)
  {
     if( endst() )  return; /* do line */
     if(match("*")) { /* pointer ? */
       j=pointer;
       k=0;
     }
     else {
      j=variable;
      k=1;
    }
    if (symname(sname)==0) /* name ok? */
       illname(); /* no... */
    if( findglb(sname) ) /* already there? */
      multidef(sname);
    if(match("()"))  j = function;
    else if (match("["))  /* array? */
    {  k=needsub(); /* get size */
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

/* declare local variables ** changed for static declaration
** dieter h. flunkert 29-oct-86 */
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
      else j = variable;
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
	j=initials(typ>>2, j, k); /*not*/
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

convert(n,s)
int n; char *s;
{
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

/* initialize global objects */
initials(siz, id, dim)
int siz, id, dim;
{
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

/* evaluate one initializer */
init(siz, id, dim)
int siz, id, *dim;
{
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
