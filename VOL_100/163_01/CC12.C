/*
** open an include file
*/
doinclude() {
  if(monitor) lout(line, stderr);
  blanks();         /* skip over to name                            */
  if(match("<")) ;  /* ignore <> */
  else match("\42"); /* ignore "" */
  if((input2=fopen(lptr,"r"))==NULL) {
    input2=EOF;
    error("open failure on include file");
    }
  kill();           /* clear rest of line                           */
      /* so next read will come from                                */
      /* new file (if open)                                         */
  }
 
/*
**  test for global declarations
*/
dodeclare(class) int class; {
  if(amatch("char",4)) {
    declglb(CCHAR,class);
    return 1;
    }
  else if((amatch("int",3))|(class==EXTERNAL)|(class==STATIC)) {
    declglb(CINT, class);
    return 1;
    }
  return 0;
  }
 
/*
** declare a static variable
*/
declglb(type, class) int type, class; {
int k, j, dim2, ndim;
char *ste; /* symbol table entry */
  dim2 = 1;
  ndim = 0;
  while(1) {
    if(match(";")) return;
    if(match("*")) {
      j=POINTER;
      k=0;
      }
    else {
      j=VARIABLE;
      k=1;
      }
    if(symname(ssname, YES)==0) illname();
    blanks();
    if(streq(lptr, "(")) {
      if(j==POINTER) error("pointer functions not allowed");
      if(type==CCHAR) error("character functions not allowed");
      if((class==STATIC)|(class==PUBLIC)) {
        newfunc(class);
        return;
        }
      else {
        j=FUNCTION;
        if(match("()") == 0) error("missing closing paren");
        }
      }
    if(findglb(ssname)) multidef(ssname);
    if(j!=FUNCTION) {
      if(match("[")) {
        k=needsub();  /* get size                                     */
	j=ARRAY;      /* !0=array                                     */
	ndim = 1;
	if(match("[")) {
	  dim2 = needsub();
	  ndim = 2;
	  if(dim2 == 0) {
	     error("need array size");
	     k = dim2 = 1;
	  } /* if dim2 */
	} /* if match */
      } /* if match */
    } /* if j!= */
    if(class!=EXTERNAL) j=initials(type>>2, j, k, class, dim2);
    ste = addsym(ssname, j, type, k, &glbptr, class, dim2, ndim);
    if(ste) ste[STATUS] |= DECLARED;
    if(match(",")==0) {ns(); return;}  /* more? */
    }
  }
 
/*
** declare local variables
*/
declloc(typ) int typ; {
  int k, j, dim2, ndim;
#ifdef STGOTO
  if(noloc) error("not allowed with goto");
#endif
  if(declared < 0)  error("must declare first in block");
  while(1) {
    while(1) {
      if(endst()) return;
      if(match("*")) j=POINTER;
      else j=VARIABLE;
      if(symname(ssname, YES)==0) illname();
      /* no multidef check, block-locals are together               */
      k=BPW;
      if (match("[")) {
	ndim = 1;
        k=needsub();
        if(k) {
          j=ARRAY;
	  if(typ == CINT) k = k << LBPW;
	  if(match("[")) {
	      dim2 = needsub();
	      k = k * dim2;
	      ndim = 2;
	  }
	}
        else {
          j = POINTER;
          k = BPW;
	  if(match("[")) {
	      dim2 = needsub();
	      while(inbyte() != ']') if(endst()) break;
	      ndim = 2;
	   }
	  }
        }
      else if(match("()")) j=FUNCTION;
      else if((typ==CCHAR)&(j==VARIABLE)) k=SBPC;
      declared = declared + k;
      addsym(ssname, j, typ, csp - declared, &locptr, AUTOMATIC, dim2, ndim);
      break;
      }
    if (match(",")==0) return;
    }
  }
 
/*
** inialize global objects
*/
initials(size, ident, dim, class, dim2) int size, ident, dim, class, dim2; {
  int savedim, savectxt, dimsz, sflag, otemp;
  savectxt = ctext;
  sflag = -1;
  ctext = 0; /* turn off interleaved source - problem with multi-line dcls */
  litptr=0;
  if(dim==0) dim = -1;
  dimsz = dim * dim2;
  savedim=dim;
  if(class==PUBLIC) entry();
  startglob();
  if(match("=")) {
    if(match("{")) {
      while(dimsz) {
	otemp = dimsz;
	init(size, ident, &dimsz);
	if(match(",")==0) break;
	if(otemp != dimsz) sflag = 1;
        }
      needtoken("}");
      }
    else {
	  otemp = dimsz;
	  init(size, ident, &dimsz);
	  if (otemp != dimsz) sflag = 1;
      }
    }
  if((savedim == -1) & (sflag == -1)) {
    stowlit(0, size=BPW);
    ident=POINTER;
    }
  dumplits(size);
  dumpzero(size, dimsz);
  ctext = savectxt; /* restore source code interleave mode */
  return ident;
  }
 
/*
** evaluate one initializer
*/
init(size, ident, dim) int size, ident, *dim; {
  int value;
  if(qstr(&value)) {
    if((ident==VARIABLE)|(size!=1))
      error("must assign to char pointer or array");
    *dim = *dim- (litptr-value);
    if(ident==POINTER) point();
    }
  else if(constexpr(&value)) {
    if(ident==POINTER) error("cannot assign to pointer");
    stowlit(value, size);
    *dim = *dim-1;
    }
  }
 
/*
** get required array size
*/
needsub() {
  int val;
  if (match("]")) return 0;  /* null size                           */
  if (constexpr(&val) == 0) val = 1;
  if (val<0) {
    error("negative size illegal");
    val = -val;
    }
  needtoken("]");            /* force single dimension             */
  return val;                /* and return size                    */
  }
 
/*
** begin a function
**
** called from "parse" and tries to make a function
** out of the following text
**
** Patched per P. L. Woods (DDJ #52)
*/
newfunc(class) int class; {
  char *ptr, *source, *dest, funname[NAMESIZE];
#ifdef STGOTO
  nogo  =              /* enable goto statements */
  noloc = 0;           /* enable block-local declarations */
#endif
  lastst=              /* no statement yet */
  litptr=0;            /* clear lit pool */
  litlab=getlabel();   /* label next literal pool */
  locptr=STARTLOC;     /* clear local variables */
  if(monitor) lout(line,stderr);
  if(class==PUBLIC) {  /* skip symname if STATIC -- already done */
    if(symname(ssname, YES)==0) {
      error("illegal function or declaration");
      kill();          /* invalidate line */
      return;
      }
    }
  if(match("(")==0) error("no open paren");
  if(ptr=findglb(ssname)) {        /* already in symbol table ?    */
    if(ptr[IDENT]!=FUNCTION)       multidef(ssname);
    else if(ptr[STATUS] & DECLARED) multidef(ssname);
    else ptr[STATUS] |= DECLARED;
    /* earlier assumed to be a function */
    }
  else {
    ptr = addsym(ssname, FUNCTION, CINT, FUNCTION, &glbptr, STATIC, 0, 0);
    if(ptr) ptr[STATUS] |= DECLARED;
    }
  if(class==PUBLIC) entry(); /* gen the PUBLIC declaration */
  source=ssname;
  dest=funname;
  while(*dest++=*source++); /* save function name (for ENDP) */
  startfun(funname); /* gen PROC and BP manipulation */
  locptr=STARTLOC;
  argstk=0;         /* init arg count                              */
  while(match(")")==0) {  /* then count args
    /* any legal name bumps arg count                              */
    if(symname(ssname, YES)) {
      if(findloc(ssname)) multidef(ssname);
      else {
        ++argstk;
	ptr = addsym(ssname, 0, 0, argstk, &locptr, AUTOMATIC, 0, 0);
        if(ptr) ptr[STATUS] |= DECLARED;
        }
      }
    else {error("illegal argument name"); junk();}
    blanks();
    /* if not closing paren, should be comma                       */
    if(streq(lptr,")")==0) {
      if(match(",")==0) error("no comma");
      }
    if(endst()) break;
    }
  csp=0;            /* preset stack pointer                        */
  argtop=argstk;
  while(argstk) {
    /* now let user declare what types of things                   */
    /*              those arguments were                           */
    if(amatch("char",4))     {doargs(CCHAR);ns();}
    else if(amatch("int",3)) {doargs(CINT);ns();}
    else {error("argument(s) not declared"); break;}
    }
  if(statement()!=STRETURN) ret();
  endfun(funname); /* gen the ENDP */
  if(litptr) {
    startlit(); /* set to DATASEG */
    printlabel(litlab);
    dumplits(1);   /* dump literals                                */
    }
  }
 
/*
** declare argument types
**
** called from "newfunc" this routine adds an entry in the
** local symbol table for each named argument
**
** rewritten per P. L. Woods (DDJ #52)
*/
doargs(t) int t; {
  int j, legalname, dim2, ndim;
  char c, *argptr;
  while(1) {
    if(argstk==0) return;      /* no arguments                     */
    if(match("*")) j=POINTER; else j=VARIABLE;
    if((legalname=symname(ssname, YES))==0) illname();
    if(match("[")) {   /* is it a pointer ?                        */
      ndim = 1;
      /* yes, so skip stuff between "[...]"                        */
      while(inbyte()!=']') if(endst()) break;
			   else if(streq(lptr,"[")) break; /* check this logic */
      j=POINTER;      /* add entry as pointer                      */
      if(match("[")) {
	dim2 = needsub();
	ndim = 2;
      }
    }
    if(legalname) {
      if(argptr=findloc(ssname)) {
        if(argptr[TYPE]==NULL) {
          /* add details of type and address                       */
          argptr[IDENT]=j;
          argptr[TYPE]=t;
          putint((((argtop-getint(argptr+OFFSET, OFFSIZE))*BPW)+STKFRSZ),
	      argptr+OFFSET, OFFSIZE);
	  argptr[NDIM] = ndim;
	  putint(dim2, argptr + CDIM, 2);
          }
        else error("duplicate argument declaration");
        }
      else error("not an argument");
      }
    --argstk; /* count down */
    if(endst())return;
    if(match(",")==0) error(" no comma");
    }
  }
 

