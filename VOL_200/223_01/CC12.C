

/*
** open an include file
*/
doinclude()  {
  char c, *fname, buff[15];
  int i;

  blanks();       /* skip over to name */
  /*
   * added code to handle include filename in quotes or brackets
   * 4/5/83 br
   */
  if ((*lptr == '"') | (*lptr == '<')) {
    i = 0;
    fname = buff;
    while (i<14) {
      c = *++lptr;
      if ((c == '"') | (c == '>'))
        i = 14; /* force exit from loop */
      else
        *fname++ = c;
      }
    *fname = '\0';
    fname = buff;
    }
  else
    fname = lptr;       /* no '"' or '<' (original convention) */

   if(inclevel <= 5){					/* fas 2.7 */
     if((input2[++inclevel]=fopen(fname,"r"))==NULL) {	/* fas 2.7 */
       input2[inclevel--]= EOF;				/* fas 2.7 */
       error("open failure on include file");		/* fas 2.7 */
       }						/* fas 2.7 */
   }else{						/* fas 2.7 */
      error("maximum include nesting reached");		/* fas 2.7 */
   }							/* fas 2.7 */

  kill();         /* clear rest of line */
      /* so next read will come from */
      /* new file (if open) */
  }


/*
** test for global declarations
*/
dodeclare(class) int class; {
  if(amatch("char",4)) {
    declglb(CCHAR, class);
    ns();
    return 1;
    }
  else if((amatch("int",3))|(class==EXTERNAL)) {
    declglb(CINT, class);
    ns();
    return 1;
    }
  return 0;
  }

/*
** delcare a static variable
*/
declglb(type, class)  int type, class; {
  int k, j, ll, aindex, i, ii, jj, temp[8];
						/* fas 2.3, 2.6 */
  char arrindex;				/* fas 2.6 */
  itype = type;					/* fas 2.3 */
  while(1) {
    arrindex = 0;				/* fas 2.6 */
    ii = jj = level = i = 0;			/* fas 2.3, 2.6 */
    if(endst()) return;     /* do line */
    if(match("(*")|match("*")) {                        /*03*/
      j=POINTER;
      k=0;
      while(match("*"))level++;			/* fas 2.3 */
      if(level) type = CINT;			/* fas 2.3 */
      }
    else {
      j=VARIABLE;
      k=1;
      }
    if (symname(ssname, YES)==0) illname();
    if(findglb(ssname)) multidef(ssname);
    if(match(")")) ;               /*03*/
    if(match("()")) j=FUNCTION;
    else if (match("[")) {
      ll = level;				/* fas 2.5 */
      k=needsub();    /* get size  */
      level = ll;				/* fas 2.5 */
      if(j != POINTER){				/* fas 2.5 */
        j=ARRAY;				/* fas 2.5 */
        while(match("[")){			/* fas 2.6 */
          temp[i] = needsub();			/* fas 2.6 */
          if(++i >= MAXELMNTS){			/* fas 2.6 */
             error("max md elements exceeded");	/* fas 2.6 */
             k = 0;				/* fas 2.6 */
             }					/* fas 2.6 */
          k *= temp[i-1];			/* fas 2.6 */
          }
        if(k>0 && i>0){				/* fas 2.6 */
          if(arrcount <= MAXARRAYS){		/* fas 2.6 */
            arrptr += ARRINDEX;			/* fas 2.6 */
            arrindex = arrcount++;		/* fas 2.6 */
          }else{				/* fas 2.6 */
            error("max md arrays exceeded");	/* fas 2.6 */
            k = 0;				/* fas 2.6 */
          }
          for(jj = 0; jj < i; jj++){		/* fas 2.6 */
             arrptr[jj] = 1;			/* fas 2.6 */
             for(ii = i-1; ii >= jj; ii--)	/* fas 2.6 */
                arrptr[jj] *= temp[ii];		/* fas 2.6 */
          } 
        }
        }else{					/* fas 2.5 */
           type = CINT;				/* fas 2.5 */
           ++level;				/* fas 2.5 */
        }					/* fas 2.5 */
      }
    if(class==EXTERNAL) external(ssname);
    else if(j!=FUNCTION) j=initials(type>>2, j, k);     /*16*/
    addsym(ssname, j, type, level, itype, arrindex, k, &glbptr, class);
						/* fas 2.3 2.6 */
    if (match(",")==0) return; /* more? */
    }
  }

/*
** declare local variables
*/
declloc(typ)  int typ;  {
  int k, j, l;						/* fas 2.5 */
  itype = typ;						/* fas 2.3 */
  if(swactive) error("not allowed in switch");         /*08*/
#ifdef STGOTO
  if(noloc) error("not allowed with goto");
#endif
  if(declared < 0) error("must declare first in block");
  while(1) {
    level = 0;						/* fas 2.3 */
    while(1) {
      if(endst()) return;
      if(match("*")){
        j=POINTER;
        while(match("*")) level++;			/* fas 2.3 */
        if(level) typ = CINT;				/* fas 2.3 */
      }
      else           j=VARIABLE;
      if (symname(ssname, YES)==0) illname();
      /* no multidef check, block-locals are together */
      k=BPW;
      if (match("[")) {
        l = level;					/* fas 2.5 */
        if(k=needsub()) {          /*25*/
          if(j != POINTER){				/* fas 2.5 */
            j=ARRAY;					/* fas 2.5 */
          }else{					/* fas 2.5 */
            typ = CINT;					/* fas 2.5 */
            level = l;					/* fas 2.5 */
            }						/* fas 2.5 */
         
          if(typ==CINT)k=k<<LBPW;
          }
        else {j=POINTER; k=BPW;}   /*25*/
        }
                                   /*14*/

      else if((typ==CCHAR)&(j==VARIABLE)) k=SBPC;
      declared = declared + k;
      addsym(ssname, j, typ, level, itype, 0,csp-declared, &locptr, AUTOMATIC);
						 /* fas 2.3 2.6 */
      break;
      }
    if (match(",")==0) return;
    }
  }

/*
** initialize global objects
*/
initials(size, ident, dim) int size, ident, dim; {
  int savedim;
  litptr=0;
  if(dim==0) dim = -1;
  savedim=dim;
  entry();
  if(match("=")) {
    if(match("{")) {
      while(dim) {
        init(size, ident, &dim);
        if(match(",")==0) break;
        }
      needtoken("}");
      }
    else init(size, ident, &dim);
    }
  if((dim == -1)&(dim==savedim)) {
     stowlit(0, size=BPW);
    ident=POINTER;
    }
  dumplits(size);
  dumpzero(size, dim);
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
    *dim = *dim - (litptr - value);
    if(ident==POINTER) point();
    }
  else if(constexpr(&value)) {
    if(ident==POINTER) error("cannot assign to pointer");
    stowlit(value, size);
    *dim = *dim - 1;
    }
  }

/*
** get required array size
*/
needsub()  {
  int val;
  if(match("]")) return 0; /* null size */
  if (constexpr(&val)==0) val=1;
  if (val<0) {
    error("negative size illegal");
    val = -val;
    }
  needtoken("]");      /* force single dimension */
  return val;          /* and return size */
  }

/*
** begin a function
**
** called from "parse" and tries to make a function
** out of the following text
**
** Patched per P.L. Woods (DDJ #52)
*/
newfunc()  {
  char *ptr;
#ifdef STGOTO
  nogo  =             /* enable goto statements */
  noloc = 0;          /* enable block-local declarations */
#endif
  lastst=             /* no statement yet */
  litptr=0;           /* clear lit pool */
  arrcount = 0;       					/* fas 2.6 */
  litlab=getlabel();  /* label next lit pool */
  locptr=STARTLOC;    /* clear local variables */
  if(monitor) lout(line, stderr);
  if (symname(ssname, YES)==0) {
    error("illegal function or declaration");
    kill(); /* invalidate line */
    return;
    }
  if(func1) {
    postlabel(beglab);
    func1=0;
    }
  if(ptr=findglb(ssname)) {      /* already in symbol table ? */
    if(ptr[IDENT]!=FUNCTION)       multidef(ssname);
    else if(ptr[OFFSET]==FUNCTION) multidef(ssname);
    else {                    /*37*/
      /* earlier assumed to be a function */
      ptr[OFFSET]=FUNCTION;
      ptr[CLASS]=STATIC;      /*37*/
      }                       /*37*/
    }
  else
    addsym(ssname, FUNCTION, CINT, 0, 0, 0, FUNCTION, &glbptr, STATIC);
						/* fas 2.3 2.6 */
  if(match("(")==0) error("no open paren");
  entry();
  locptr=STARTLOC;
  argstk=0;               /* init arg count */
  while(match(")")==0) {  /* then count args */
    /* any legal name bumps arg count */
    if(symname(ssname, YES)) {
      if(findloc(ssname)) multidef(ssname);
      else {
        addsym(ssname, 0, 0, 0, 0, 0,  argstk, &locptr, AUTOMATIC);
						/* fas 2.3 2.6 */
        argstk=argstk+BPW;
        }
      }
    else {error("illegal argument name");junk();}
    blanks();
    /* if not closing paren, should be comma */
    if(streq(lptr,")")==0) {
      if(match(",")==0) error("no comma");
      }
    if(endst()) break;
    }
  csp=0;        /* preset stack ptr */
  argtop=argstk;
  while(argstk) {
    /* now let user declare what types of things */
    /*      those arguments were */
    if(amatch("char",4))     {doargs(CCHAR);ns();}
    else if(amatch("int",3)) {doargs(CINT);ns();}
    else {error("wrong number of arguments");break;}
    }
/*55*/
  statement();
#ifdef STGOTO
  if(lastst != STRETURN && lastst != STGOTO) ffret();
#else
  if(lastst != STRETURN) ffret();
#endif
/*55*/
  if(litptr) {
    printlabel(litlab);
    col();
    dumplits(1); /* dump literals */
    }
  }

/*
** declare argument types
**
** called from "newfunc" this routine adds an entry in the
** local symbol table for each named argument
**
** rewritten per P.L. Woods (DDJ #52)
*/
doargs(t) char t; {					/* fas 2.4 */
  int legalname;					/* fas 2.4 */
  char c, j, *argptr;					/* fas 2.4 */
  itype = t;						/* fas 2.3 */
  while(1) {
    level = 0;						/* fas 2.3 */
    if(argstk==0) return; /* no arguments */
    if(match("(*")|match("*")){	  /*03*/
      j=POINTER;                  /*03*/
      while(match("*")) level++;			/* fas 2.3 */
      if(level) t = CINT;				/* fas 2.3 */
    }else{                        /*03*/
      j=VARIABLE;                 /*03*/
    }
    if((legalname=symname(ssname, YES))==0) illname();
    if(match(")")) ;              /*03*/
    if(match("()")) ;             /*03*/
    if(match("[")) {
      while(inbyte()!=']') if(endst()) break;/* skip "[...]" */
      if(j != POINTER){					/* fas 2.5 */
        j=POINTER; /* add entry as pointer */
      }else{						/* fas 2.5 */
        t = CINT;					/* fas 2.5 */
        ++level;					/* fas 2.5 */
        }						/* fas 2.5 */
     }
    if(legalname) {
      if(argptr=findloc(ssname)) {
        /* add details of type and address */
        argptr[IDENT]=j;
        argptr[TYPE]=t;
        argptr[LEVEL]=level;				/* fas 2.4 */
        argptr[ITYPE]=itype;				/* fas 2.4 */
        argptr[AINDEX]=0;				/* fas 2.6 */
        putint(argtop-getint(argptr+OFFSET, OFFSIZE), argptr+OFFSET, OFFSIZE);
        }
      else error("not an argument");
      }
    argstk=argstk-BPW;        /* cnt down */
    if(endst())return;
    if(match(",")==0) error("no comma");
    }
  }

