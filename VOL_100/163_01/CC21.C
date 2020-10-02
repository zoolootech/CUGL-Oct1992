/*
** Purpose is kill characters until a transition between alphanumeric and
** non-alphanumeric occurs.  Fixed to not gobble primes and quotes.  DRH.
*/
junk() {
  if (an(inbyte())) while(an(ch)) gch();
  else while((an(ch)==0)&&(ch!='\'')&&(ch!='"')) { /* don't swallow quote */
    if(ch==0) break;
    gch();
    }
  blanks();
  }
 
endst() {
  blanks();
  return ((streq(lptr,";")|(ch==0)));
  }
 
illname() {
  error("illegal symbol");
  junk();
  }
 
 
multidef(sname) char *sname; {
  error("already defined");
  }
 
needtoken(str) char *str; {
  if (match(str)==0) error("missing token");
  }
 
needlval() {
  error("must be lvalue");
  }
 
findglb(sname) char *sname; {
#ifdef HASH
  if(search(sname, STARTGLB, SYMMAX, ENDGLB, NUMGLBS, NAME))
    return cptr;
#else
  cptr= STARTGLB;
  while(cptr < glbptr) {
    if(astreq(sname, cptr+NAME, NAMEMAX)) return cptr;
    cptr=nextsym(cptr);
    }
#endif
  return 0;
  }
 
findloc(sname)  char *sname;  {
  cptr = locptr - 1;  /* search backward for block locals */
  while(cptr > STARTLOC) {
    cptr = cptr - *cptr;
    if(astreq(sname, cptr, NAMEMAX)) return (cptr - NAME);
    cptr = cptr - NAME - 1;
    }
  return 0;
  }
 
addsym(sname, id, typ, value, lgptrptr, class, cdim, ndim)
  char *sname, id, typ; int value, *lgptrptr, class, cdim, ndim; {
  if(lgptrptr == &glbptr) {
    if(cptr2=findglb(sname)) return cptr2;
#ifdef HASH
    if(cptr==0) {
      error("global symbol table overflow");
      return 0;
      }
#else
#ifndef DYNAMIC
    if(glbptr >= ENDGLB) {
      error("global symbol table overflow");
      return 0;
      }
#endif
    cptr=*lgptrptr;
#endif
    }
  else {
    if(locptr > (ENDLOC-SYMMAX)) {
      error("local symbol table overflow");
      exit(EUSER);
      }
    cptr=*lgptrptr;
    }
  cptr[IDENT]=id;
  cptr[TYPE] =typ;
  cptr[CLASS]=class;
  cptr[STATUS]=0;
  putint(value, cptr+OFFSET, OFFSIZE);
  cptr[NDIM] = ndim;
  putint(cdim, cptr + CDIM, 2);
  cptr3 = cptr2 = cptr + NAME;
  while(an(*sname)) *cptr2++ = *sname++;
#ifdef HASH
  if(lgptrptr == &locptr) {
    *cptr2 = cptr2 - cptr3; /* set length */
    *lgptrptr = ++cptr2;
    }
#else
  *cptr2 = cptr2 - cptr3;   /* set length */
  *lgptrptr = ++cptr2;
#ifdef DYNAMIC
  if(lgptrptr == &glbptr) malloc(cptr2 - cptr);
  /* gets allocation error if no more memory */
#endif
#endif
  return cptr;
  }
 
/*
** declare externals to the assembler and warn of undefined variables
*/
externs() {
  errflag = 0;
#ifdef HASH
  this section not yet defined;
#else
  cptr= STARTGLB;
  while(cptr < glbptr) {
    if((cptr[IDENT]==FUNCTION) &
       (!(cptr[STATUS] & DECLARED))) {
      if(errflag==0) {
        errflag = 1;
        ++errcnt;
        lout("**** undefined symbols:", stderr);
        }
      errstr(cptr+NAME);
      lout(" ",stderr);
      external(cptr+NAME, cptr[TYPE], cptr[IDENT]);
      }
    if((cptr[CLASS] == EXTERNAL) &&
        (cptr[STATUS] & USED))
      external(cptr+NAME, cptr[TYPE], cptr[IDENT]);
    cptr=nextsym(cptr);
    }
#endif
  }
 
#ifndef HASH
nextsym(entry) char *entry; {
  entry = entry + NAME;
  while(*entry++ >= ' ');  /* find length byte */
  return entry;
  }
#endif
 
/*
** get integer of length len from address addr
** (byte sequence set by "putint")
*/
getint(addr, len) char *addr; int len; {
  int i;
  i = *(addr + --len);  /* high order byte sign extended */
  while(len--) i = (i << 8) | *(addr+len)&255;
  return i;
  }
 
/*
** put integer i of length len into address addr
** (low byte first)
*/
putint(i, addr, len) char *addr; int i, len; {
  while(len--) {
    *addr++ = i;
    i = i>>8;
    }
  }
 
/*
** test if next input string is legal symbol name
*/
symname(sname, ucase) char *sname; int ucase; {
  int k;char c;
  blanks();
  if(alpha(ch)==0) return 0;
  k=0;
  while(an(ch)) {
#ifdef UPPER
    if(ucase)
      sname[k]=upper(gch());
    else
#endif
      sname[k]=gch();
    if(k<NAMEMAX) ++k;
    }
  sname[k]=0;
  return 1;
  }
 
#ifdef UPPER
/*
** force upper case alphabetics
*/
upper(c)  char c; {
  if((c >= 'a') & (c <= 'z')) return (c - 32);
  else return c;
  }
#endif
 
/*
** return next avail intern label number
*/
getlabel() {
  return(++nxtlab);
  }
 
/*
** post a label in the program
*/
postlabel(label) int label; {
  printlabel(label);
  col();
  nl();
  }
 
/*
** print specified number as a label
*/
printlabel(label)  int label; {
  outstr("CC");
  outdec(label);
  }
 
/*
** test if given character is alphabetic
*/
alpha(c)  char c; {
  return (((c>='a')&(c<='z'))|((c>='A')&(c<='Z'))|(c=='_'));
  }
 
/*
** test if given character is numeric
*/
numeric(c)  char c; {
  return((c>='0')&(c<='9'));
  }
 
/*
** test if given character is alphanumeric
*/
an(c) char c; {
  return((alpha(c))|(numeric(c)));
  }
 
addwhile(ptr)  int ptr[]; {
  int k;
  ptr[WQSP]  =csp;         /* and stack pointer */
  ptr[WQLOOP]=getlabel();  /* and looping label */
  ptr[WQEXIT]=getlabel();   /* and exit label    */
  if (wqptr==WQMAX) {
    error("too many active loops");
    exit(EUSER);
    }
  k=0;
  while (k<WQSIZ) *wqptr++ = ptr[k++];
  }
 
/*
** fixed per J. E. Hendrix letter, DDJ #77
*/
delwhile() {
  if(wqptr>wq) wqptr=wqptr-WQSIZ;
  }
 
/*
** fixed per J. E. Hendrix letter, DDJ #77
*/
readwhile(ptr) int *ptr; {
  if (ptr<=wq) {
    error("out of context");
    return 0;
    }
  else return (ptr-WQSIZ);
  }
 
white() {
  /* test for stack/program overlap          */
  /* primary -> symname -> blanks -> white   */
#ifdef DYNAMIC
  CCAVAIL();  /* abort on stack/symbol table overflow */
#endif
  if(*lptr==' ') return 1; /* blank */
  if(*lptr==9)   return 1; /* tab */
  if(*lptr==10)  return 1; /* new line (line feed) */
  if(*lptr==13)  return 1; /* carriage return */
  return 0;
  }
 
gch() {
  int c;
  if(c=ch) bump(1);
  return c;
  }
 
bump(n) int n; {
  if(n) lptr=lptr+n;
  else  lptr=line;
  if(ch=nch=*lptr) nch=*(lptr+1);
  }
 
kill() {
  *line=0;
  bump(0);
  }
 
inbyte()  {
  while(ch==0) {
    if (eof) return 0;
    preprocess();
    }
  return gch();
  }
 
inline() {
  int k,unit;
  char *ptr;
  while(1) {
    if (input==EOF) {eof=1; return;}
    if((unit=input2)==EOF) unit=input;
    if(fgets(line, LINEMAX, unit)==NULL) {
      fclose(unit);
      if(input2!=EOF) input2=EOF;
      else input=EOF;
      }
    else {
      bump(0);
      /* if line ends with '\n', get rid of it */
      ptr = line;
      while(*ptr++);
      ptr -= 2;
      if(*ptr == '\n') *ptr = '\0';
      else error("line too long");
      return;
      }
    }
  }
 

