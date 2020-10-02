/* >>>>> start of cc3 <<<<<<<<< */
/*     */
/* Perform a function call  */
/*     */
/* called from heir11, this routine will either call */
/* the named function, or if the supplied ptr is */
/* zero, will call the contents of HL  */
callfunction(ptr)
 char *ptr; /* symbol table entry (or 0) */
{ int nargs;
 nargs=0;
 blanks(); /* already saw open paren */
 if(ptr==0)push(); /* calling HL */
 while(streq(line+lptr,")")==0)
  {if(endst())break;
  expression(); /* get an argument */
  if(ptr==0)swapstk(); /* don't push addr */
  push(); /* push argument */
  nargs=nargs+2; /* count args*2 */
  if (match(",")==0) break;
  }
 needbrack(")");
 if(streq(ptr,"printf")) {
   immed();     /* argument counter in primary register */
   outdec(nargs>>1);     /*  for printf etc. */
   nl();                 /*  Dieter H. Flunkert     20-jul-86  */
 }
 if(ptr)call(ptr);
 else callstk();
 stkp=modstk(stkp+nargs); /* clean up arguments */
}
junk()
{ if(an(inbyte()))
  while(an(ch()))gch();
 else while(an(ch())==0)
  {if(ch()==0)break;
  gch();
  }
 blanks();
}
endst()
{ blanks();
 return ((streq(line+lptr,";")|(ch()==0)));
}
illname()
{ errrpt("illegal symbol name");junk();}
multidef(sname)
 char *sname;
{ errrpt("already defined");
 comment();
 outstr(sname);nl();
}
needbrack(str)
 char *str;
{ if (match(str)==0)
  {errrpt("missing token");
  comment();outstr(str);nl();
  }
}
needlval()
{ errrpt("must be lvalue");
}

hash(sname) char *sname; {
  int h, c;
  h = *sname;
  while(c = *(++sname)) h=(h<<1)+c;
  return h;
}

/*
** findstatic variable
** dhf 29-oct-86
*/
findstat(sname) char *sname; {
char *ptr;
  ptr=statptr;
  while(ptr >= startstat) {
   ptr = ptr - statsize;
   if(astreq(sname, ptr, namemax)) return ptr;
  }
  return 0;
}

findglb(sname) char *sname; {
 int h;
 h=hash(sname)&MASKGLBS;
 cptr=startglb+h*symsiz;
 while(astreq(sname,cptr,namemax)==0) {
   if(*cptr==0) return 0;
   cptr=cptr+symsiz;
   if(cptr>=endglb) cptr=startglb;
 }
 return cptr;
}
/*
** find local changed dhf 4-nov-86
** search is now from end to start
** of local pointer storage area
*/
findloc(sname)
 char *sname;
{ char *ptr;
 ptr=locptr;
 while(ptr > endsearch)
  {
   ptr=ptr-symsiz;
   if(astreq(sname,ptr,namemax))return (ptr);
  }
 return (0);
}

/*
** addstatic variable
** dhf 29-oct-86
*/
addstatic(sname, numlab, id, typ, value)
char *sname, *numlab, id, typ;
int value;
{
  char *ptr;

  endsearch=startcomp;
  if(findloc(sname)) return statptr;
  endsearch=startloc;
  if(statptr >= endstat) {
   errrpt("static symbol table overflow");
   return 0;
  }
  ptr=statptr;
  while(an(*ptr++ = *sname++)) ; /* copy name */
  ptr=statptr+symsiz;
  while(an(*ptr++ = *numlab++)) ;  /* copy local label */
  statptr[ident] = id;
  statptr[type] = typ;
  statptr[storage] = statik;
  putint(value, statptr+offset, offsize);
  statptr = statptr + statsize;
  return statptr;
}

addglb(sname,id,typ,value,class)
 char *sname,id,typ;
 int value,class;
{ char *ptr;
 if(findglb(sname))return (cptr);
 if(glbptr>=endglb)
  {errrpt("global symbol table overflow");
  return (0);
  }
 ptr=cptr;
 while(an(*ptr++ = *sname++)); /* copy name */
 cptr[ident]=id;
 cptr[type]=typ;
 cptr[storage]=class;
 putint(value, cptr+offset, offsize);
 glbptr=glbptr+symsiz;
 return (cptr);
}

addloc(sname,id,typ,value)
 char *sname,id,typ;
 int value;
{ char *ptr;
  endsearch=startcomp;
  if(findloc(sname)) return statptr;
  endsearch=startloc;
 if(locptr>=endloc)
  {errrpt("local symbol table overflow");
  return (0);
  }
 cptr=ptr=locptr;
 while(an(*ptr++ = *sname++)); /* copy name */
/* next lines added for 6809 from dieter flunkert 22 jan 1986 */
/* value has to be adjusted for cchar type varibles. 6809 handles */
/* the values on the stack high/low not /low/high like the 8080 and z80 */
 if((id==variable)&&(typ==cchar)) --value;
/* end of modification 22 jan 1986 */
 cptr[ident]=id;
 cptr[type]=typ;
 cptr[storage]=stkloc;
 putint(value, cptr+offset, offsize);
 locptr=locptr+symsiz;
 return (cptr);
}
#ifdef STGOTO
#ifndef HASH
nextsym(entry) char *entry; {
  entry = entry + name;
  while(*entry++ >= ' ');  /* find length byte */
  return entry;
}
#endif
#endif

/*
**  get integer of length len from address addr
**  (byte sequence set by "putint"
*/
getint(addr,len) char *addr; int len; {
  int i;
  i = *(addr + --len);  /* high order byte sign extend */
  while(len--) i = (i << 8) | *(addr+len)&255;
  return(i);
}

/*
**  put integer i of length len into address addr
**  (low byte first)
*/
putint(i, addr, len) char *addr; int i, len; {
  while(len--) {
    *addr++ = i;
    i = i>>8;
    }
  }


/* Test if next input string is legal symbol name */
symname(sname)
 char *sname;
{
 blanks();
 if(alpha(ch())==0) return (0);
 while(an(ch()))*sname++=gch();
 *sname=0;
 return (1);
 }
/* Return next avail internal label number */
getlabel()
{ return(++nxtlab);
}

/*
**  print label with newline
*/
postlabel(label) int label; {
  printlabel(label);
  nl();
}

/* Print specified number as label */
printlabel(label)
 int label;
{ outstr("cc");
 outdec(label);
}
/* Test if given character is alpha */
alpha(c)
 char c;
{ c=c&127;
 if(c>='a') return(c<='z');
 if(c<='Z') return(c>='A');
 return(c=='_');
}
/* Test if given character is numeric */
numeric(c)
 char c;
{ c=c&127;
 if(c>='0') return(c<='9');
 return 0;
}
/* Test if given character is alphanumeric */
an(c)
 char c;
{ if(alpha(c)) return 1;
  return(numeric(c));
}
/* Print a new line and a string only to console */
pl(str)
 char *str;
{
 cnl();
 while(*str)putchar(*str++);
}
addwhile(ptr)
 int ptr[];
 {
 int k;
 ptr[wqsp]=stkp;  /* and stk ptr */
 ptr[wqloop]=getlabel(); /* and looping label */
 ptr[wqlab]=getlabel(); /* and exit label */
 if (wqptr==wqmax)
  {errrpt("too many active whiles");exit(ERRCODE);}
 k=0;
 while (k<wqsiz)
  {*wqptr++ = ptr[k++];}
}
delwhile()
 {if(readwhile()) wqptr=wqptr-wqsiz;
 }
readwhile()
 {
 if (wqptr==wq){errrpt("no active whiles");return (0);}
 else return (wqptr-wqsiz);
 }
ch()
{ return(line[lptr]&127);
}
nch()
{ if(ch())return(line[lptr+1]&127);
  return 0;
}
gch()
{ if(ch()) return(line[lptr++]&127);
  return 0;
}
kill()
{ lptr=0;
 line[lptr]=0;
}
inbyte()
{
 while(ch()==0)
  {inline();
  if (eof) return (0);
  preprocess();
  }
 return (gch());
}
inchar()
{
 if(ch()==0)inline();
 if(eof)return (0);
 return(gch());
}
inline()
 {
 int k,unit;
 while(1)
  {if (input==0)
#ifdef CMD_LINE
    eof=YES;
#else
    openin();
#endif
  if(eof)return;
  if(CCPOLL())  /* allow program interruption D.H.Flunkert 04-may-86 */
   if(getc(stderr)==3) exit(0);
  if((unit=input2)==0)unit=input;
  kill();
  while((k=getc(unit))>0)
   {if((k==EOL)||(lptr>=linemax))break;
   line[lptr++]=k;
   }
  line[lptr]=0; /* append null */
  if(k<=0)
   {fclose(unit);
   if(input2)input2=0;
    else input=0;
   }
  if(lptr)
   {if((ctext)&&(cmode))
    {cout(';',output);
    sout(line,output);
    cout('\n',output);
    }
   lptr=0;
   if(match("#ifdef")) {
    ++iflevel;
    if(skiplevel) continue;
    blanks();
    if(findmac(&line[lptr])==0)
     skiplevel=iflevel;
    continue;
    }
   if(match("#ifndef")) {
    ++iflevel;
    if(skiplevel) continue;
    blanks();
    if(findmac(&line[lptr]))
     skiplevel=iflevel;
    continue;
    }
   if(match("#else")) {
    if(iflevel) {
     if(skiplevel==iflevel) skiplevel=0;
     else if(skiplevel==0) skiplevel=iflevel;
     }
    else errrpt("no matching #if...");
    continue;
    }
   if(match("#endif")) {
    if(iflevel) {
     if(skiplevel==iflevel) skiplevel=0;
     --iflevel;
     }
    else errrpt("no matching #if...");
    continue;
    }
   if(skiplevel) continue;
   break;
   }
  }
}
