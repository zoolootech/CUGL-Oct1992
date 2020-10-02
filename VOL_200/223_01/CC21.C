junk() {
  if(an(inbyte())) while(an(ch)) gch();
  else while(an(ch)==0) {
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
  

multidef(sname)  char *sname; {
  error("already defined");
  }

needtoken(str)  char *str; {
  if (match(str)==0) error("missing token");
  }

needlval() {
  error("must be lvalue");
  }

findglb(sname)  char *sname; {
  if(search(sname, STARTGLB, SYMMAX, ENDGLB, NUMGLBS, NAME))
    return cptr;
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

addsym(sname, id, typ, level, itype, arrindex, value, lgptrptr, class)
  char *sname, id, typ, level, itype, arrindex; int value, *lgptrptr, class;{
						/* fas 2.3 2.6 */
  if(lgptrptr == &glbptr) {
    if(cptr2=findglb(sname)) return cptr2;
    if(cptr==0) {
      error("global symbol table overflow");
      return 0;
      }
    }
  else {
    if(locptr > (ENDLOC-SYMMAX)) {
      error("local symbol table overflow");
      abort(ERRCODE);
      }
    cptr = *lgptrptr;
    }
  cptr[IDENT]=id;
  cptr[TYPE]=typ;
  cptr[CLASS]=class;
  cptr[LEVEL]=level;					/* fas 2.3 */
  cptr[ITYPE]=itype;					/* fas 2.3 */
  cptr[AINDEX]=arrindex;				/* fas 2.6 */
  putint(value, cptr+OFFSET, OFFSIZE);
  cptr3 = cptr2 = cptr + NAME;
  while(an(*sname)) *cptr2++ = *sname++;
  if(lgptrptr == &locptr) {
    *cptr2 = cptr2 - cptr3;         /* set length */
    *lgptrptr = ++cptr2;
    }
  return cptr;
  }

nextsym(entry) char *entry; {
  entry = entry + NAME;
  while(*entry++ >= ' '); /* find length byte */
  return entry;
  }

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
  if(alpha(ch)==0) return (*sname=0);   /*19*/
  k=0;
  while(an(ch)) {
#ifdef UPPER
    if(ucase)
      sname[k]=toupper(gch());
    else
#endif
      sname[k]=gch();
    if(k<NAMEMAX) ++k;
    }
  sname[k]=0;
  return 1;
  }

/*
** return next avail internal label number
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
** test if c is alphabetic
*/
alpha(c)  char c; {
  return (isalpha(c) || c=='_');
  }

/*
** test if given character is alphanumeric
*/
an(c)  char c; {
  return (alpha(c) || isdigit(c));
  }

addwhile(ptr)  int ptr[]; {
  int k;
  ptr[WQSP]=csp;           /* and stk ptr */
  ptr[WQLOOP]=getlabel();  /* and looping label */
  ptr[WQEXIT]=getlabel();   /* and exit label */
  if (wqptr==WQMAX) {
    error("too many active loops");
    abort(ERRCODE);
    }
  k=0;
  while (k<WQSIZ) *wqptr++ = ptr[k++];
  }

delwhile() {
  if (wqptr > wq) wqptr=wqptr-WQSIZ;                 /*01*/
  }

readwhile(ptr) int *ptr; {                           /*01*/
  if (ptr <= wq) {                                   /*01*/
    error("out of context");                         /*01*/
    return 0;
    }
  else return (ptr-WQSIZ);                           /*01*/
 }

white() {
#ifdef DYNAMIC
  /* test for stack/prog overlap at deepest nesting */
  /* primary -> symname -> blanks -> white */
  avail(YES);  /* abort on stack overflow */   /*31*/
#endif
  return (*lptr<= ' ' && *lptr!=NULL);         /*19*/
  }

gch() {
  int c;
  if(c=ch) bump(1);
  return c;
  }

bump(n) int n; {
  if(n) lptr=lptr+n;
  else  lptr=line;
  if(ch=nch = *lptr) nch = *(lptr+1);
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

  poll(1);  /* allow program interruption */

  while(1) {
    if (input == EOF) openin();
    if(eof) return;

    if(inclevel){				/* fas 2.7 */
       if((unit=input2[inclevel]) == EOF)	/* fas 2.7 */
         unit=input2[--inclevel];		/* fas 2.7 */
    }else{ 					/* fas 2.7 */
       unit=input;				/* fas 2.7 */
    }						/* fas 2.7 */

    if(xgets(line, LINEMAX, unit)==NULL) {
      fclose(unit);
      if(inclevel){				/* fas 2.7 */
         if(input2[inclevel] != EOF)		/* fas 2.7 */
            input2[inclevel--] = EOF;		/* fas 2.7 */
         }else  input = EOF;
      }
    else {
      bump(0);
      return;
      }
    }
  }


/*
 * special version of 'fgets' that deletes trailing '\n'
 */
xgets(string, len, fd) char *string; int len, fd; {

  char c, *strptr;
  strptr = string;
  while ((((c = getc(fd)) & 127) != '\n') && (--len)) {
    if (c == EOF) return NULL;
    if (c == 8){				/* fas 2.2 */
      len++;
      string--;
    }else{
      *string++ = c & 127;  /* mask parity off */
      }
    }
  *string = NULL;
  return strptr;
  }

