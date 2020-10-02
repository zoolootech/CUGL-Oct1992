/* >>>>>> start of cc7 <<<<<< */
heir11(lval)
 int *lval;
{ int k;char *ptr;
 k=primary(lval);
 ptr=lval[0];
 blanks();
 if((ch()=='[')||(ch()=='('))
 while(1)
  {if(match("["))
   {if(ptr==0)
    {errrpt("can't subscript");
    junk();
    needbrack("]");
    return (0);
    }
   else if(ptr[ident]==pointer)rvalue(lval);
   else if(ptr[ident]!=array)
    {errrpt("can't subscript");
    k=0;
    }
   push();
   expression();
   needbrack("]");
   if(ptr[type]==cint)doublereg();
/*   pop();  */
   add();
   lval[1]=ptr[type];
   k=1;
   }
  else if(match("("))
   {if(ptr==0)
    {callfunction(0);
    }
   else if(ptr[ident]!=function)
    {rvalue(lval);
    callfunction(0);
    }
   else callfunction(ptr);
   k=lval[0]=0;
   }
  else return (k);
  }
 if(ptr==0)return (k);
 if(ptr[ident]==function)
  {immed();
  outstr(ptr);
  nl();
  return (0);
  }
 return (k);
}
/*
**  change for fully implementation of 
**  komma operator DDJ bound volume 8 page 345
*/
primary(lval)
 int *lval;
{ char *ptr, *savptr, sname[namesize];int num[1];
 int k;
 if(match("(")) {
  while(1) {
    k=heir1(lval);
    if(match(",")==0) break;
    }
  needbrack(")");
  return (k);
  }
 if(symname(sname)) {
   if(savptr=findloc(sname)) {
     if(ptr=findstat(sname)) {  /* 29-oct-86 */
       lval[0]=ptr;
       lval[1]=0;
       if(ptr[ident]!=array)return (1);
       immed();
       outstr(ptr+symsiz);outbyte('B');nl();
       lval[1]=ptr[type];
       return (0);
     }
     else {
       ptr=savptr;
       getloc(ptr);
       lval[0]=ptr;
       lval[1]=ptr[type];
       if(ptr[ident]==pointer)lval[1]=cint;
       if(ptr[ident]==array)return (0);
       else return (1);
     }
   }
   if(ptr=findglb(sname))
     if(ptr[ident]!=function) {
      lval[0]=ptr;
      lval[1]=0;
      if(ptr[ident]!=array)return (1);
      immed();
      outstr(ptr);nl();
      lval[1]=ptr[type];
      return (0);
     }
     ptr=addglb(sname,function,cint,0,statik);
     lval[0]=ptr;
     lval[1]=0;
     return (0);
   }
 if(constant(num)) {
  constval[0]=1;
  constval[1]=num[0];
  return(lval[0]=lval[1]=0);
  }
 else
  {errrpt("invalid expression");
  immed();outdec(0);nl();
  junk();
  return (0);
  }
 }

store(lval)
 int *lval;
{ if (lval[1]==0)putmem(lval[0]);
 else putstk(lval[1]);
}

rvalue(lval)
 int *lval;
{ if ((lval[0])==0)return;
 if (lval[1]==0)getmem(lval[0]);
 else indirect(lval[1]);
}

test(label, bracket) int label, bracket; {
 char *before, *start;
 int lval[8];
 if(bracket) needbrack("(");
 while(1) {
  setstage(&before, &start);
  if(heir1(lval)) rvalue(lval);
  if(match(",")) clearstage(before, start);
  else break;
 }
 if(bracket) needbrack(")");
 testjump(label);
 clearstage(before, start);
}

constexpr(val) int *val; {
  int const;
  char *before, *start;
  setstage(&before, &start);
  constval[0]=constval[1]=0;
  expression();
  const=constval[0];
  *val=constval[1];
  clearstage(before, 0);  /* scratch generated code */
  if(const==0) errrpt("must be constant expression");
  return(const);
  }

constant(lval)
 int lval[];
{ if (number(lval))
  immed();
 else if (pstr(lval))
  immed();
 else if (qstr(lval))
  {immed();printlabel(litlab);outbyte('+');}
 else return (0); 
 outdec(lval[0]);
 nl();
 return (1);
}

number(val)
 int val[];
{ int k,minus;char c;
 k=minus=1;
 while(k)
  {k=0;
  if (match("+")) k=1;
  if (match("-")) {minus=(-minus);k=1;}
  }
 if(numeric(ch())==0)return (0);
 if(match("0"))   /* octal or hex number */
   if(match("x") | match("X"))    /* hex */
     while(numeric(c=upper(ch())) || (c>='A' && c<='F')) {
       if(numeric(c)) c=c-'0';
       else c=c-55;
     k=(k<<4)+c;
     inbyte();
     }
   else  /* octal */
     while(ch()>='0' && ch()<='7') {
       c=inbyte();
       k=(k<<3)+c-'0';
     }
 else
   while (numeric(ch()))
     {c=inbyte();
     k=k*10+(c-'0');
 }
 if (minus<0) k=(-k);
 val[0]=k;
 return (1);
}

upper(c) char c; {
  if(c>='a' && c<='z') return(c-32);
  return c;
}

pstr(val)
 int val[];
{ int k;char c;
 k=0;
 if (match("'")==0) return (0);
 while((ch())!=39)
  k=(k&255)*256 + (litchar()&255);
 val[0]=k;
 gch();
 return (1);
}

qstr(val)
 int val[];
{ char c;
 if (match(quote)==0) return (0);
 val[0]=litptr;
 while (ch()!='"')
  {if(ch()==0)break;
  if(litptr>=litmax)
   {errrpt("string space exhausted");
   while(match(quote)==0)
    if(gch()==0)break;
   return (1);
   }
  litq[litptr++]=litchar();
  }
 gch();
 litq[litptr++]=0;
 return (1);
}

stowlit(value, siz) int value, siz; {
  if((litptr+siz) >= litmax) {
    errrpt("literal que overflow"); exit(ERRCODE);
    }
  putint(value, litq+litptr, siz);
  litptr=litptr+siz;
  }

/*
** return current literal char & bump lptr
*/
litchar() {
  int i, oct;
  if((ch()!=92)|(nch()==0)) return gch();
  gch();
  if(ch()=='n') {gch(); return 13;} /* NL(LF) same as CR for FLEX*/
  if(ch()=='t') {gch(); return  9;} /* HT */
  if(ch()=='b') {gch(); return  8;} /* BS */
  if(ch()=='f') {gch(); return 12;} /* FF */
  if(ch()==92)  {gch(); return 92;} /* \  */
  if(ch()==39)  {gch(); return 39;} /* '  */
  if(ch()=='r') {gch(); return 13;} /* CR */
  i=3; oct=0;
  while(((i--)>0)&&(ch()>='0')&&(ch()<='7')) oct=(oct<<3)+gch()-'0';
  if(i==2) return gch(); else return oct;
  }
