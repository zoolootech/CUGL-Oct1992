/*
HEADER:		Small-C source part 6;
FILENAME:	CC6.C;
AUTHORS:	Dieter H. Flunkert;
COMPILERS:Turbo C V2.0 Medium Memory Model;
SYSTEM: MSDOS (Brian Brown, Nov 1989)
*/

#include <stdio.h>
#include "ccdef.c"

/* external function declaration */
extern int add(), asl(), asr(), blanks(), ch(), com(), dec(), div(),
  doublereg(), errrpt(), heir11(), immed(), inc(), lognot(), match(),
  mod(), mult(),needlval(), neg(), nl(), outstr(), pop(), push(),
  rvalue(), store(), streq(), sub();

/* external variables declaration */
extern int lptr;

extern char *cptr, line[linesize];

heir7(lval)
 int lval[];
{
 int k,lval2[8];
 char *off_set;
 k=heir8(lval);
 blanks();
 off_set=line+lptr;
 if(streq(off_set,">>=")) return k;
 if(streq(off_set,"<<=")) return k;
 if((streq(off_set,">>")==0)&&
  (streq(off_set,"<<")==0))return (k);
 if(k)rvalue(lval);
 while(1)
  {if (match(">>"))
   {push();
   if(heir8(lval2))rvalue(lval2);
   asr();
   }
  else if (match("<<"))
   {push();
   if(heir8(lval2))rvalue(lval2);
   asl();
   }
  else return (0);
  }
}

heir8(lval)
 int lval[];
{
 int k,lval2[8];
 char *off_set;
 k=heir9(lval);
 blanks();
 off_set=line+lptr;
 if((ch()!='+')&&(ch()!='-'))return (k);
 if(streq(off_set,"+=")) return k;
 if(streq(off_set,"-=")) return k;
 if(k)rvalue(lval);
 while(1)
  {if (match("+"))
   {push();
   if(heir9(lval2))rvalue(lval2);
   if(cptr=lval[0])
    if((cptr[ident]==pointer)&&(cptr[type]==cint)&&lval[2])
      doublereg();
   add();
   }
  else if (match("-"))
   {push();
   if(heir9(lval2))rvalue(lval2);
   if(cptr=lval[0])
    if((cptr[ident]==pointer)&&(cptr[type]==cint)&&lval[2])
      doublereg();
   sub();
   }
  else return (0);
  }
}

heir9(lval)
 int lval[];
{
 int k,lval2[8];
 char *off_set;
 k=heir10(lval);
 blanks();
 off_set=line+lptr;
 if((ch()!='*')&&(ch()!='/')&&
  (ch()!='%'))return (k);
 if(streq(off_set,"*=")) return k;
 if(streq(off_set,"/=")) return k;
 if(streq(off_set,"%=")) return k;
 if(k)rvalue(lval);
 while(1)
  {if (match("*"))
   {push();
   if(heir9(lval2))rvalue(lval2);
   pop();
   mult();
   }
  else if (match("/"))
   {push();
   if(heir10(lval2))rvalue(lval2);
   pop();
   div();
   }
  else if (match("%"))
   {push();
   if(heir10(lval2))rvalue(lval2);
   pop();
   mod();
   }
  else return (0);
  }
}

/* ! and ~ operators implemented  13-jul-86 Dieter H. Flunkert */
heir10(lval)
int lval[];
{
 int k;
 char *ptr;
 if(match("!")) {
  k=heir10(lval);
  if(k) rvalue(lval);
  lognot();
  return 0;
 }
 else if(match("~")) {
  k=heir10(lval);
  if(k) rvalue(lval);
  com();
  return 0;
 }
 else if(match("++"))
  {if((k=heir10(lval))==0)
   {needlval();
   return (0);
   }
  if(lval[1])push();
  rvalue(lval);
  inc();
  ptr=lval[0];
  if((ptr[ident]==pointer)&&(ptr[type]==cint)&&lval[2])
    inc();
  store(lval);
  return (0);
  }
 else if(match("--"))
  {if((k=heir10(lval))==0)
   {needlval();
   return (0);
   }
  if(lval[1])push();
  rvalue(lval);
  dec();
  ptr=lval[0];
  if((ptr[ident]==pointer)&&(ptr[type]==cint)&&lval[2])
    dec();
  store(lval);
  return (0);
  }
 else if(streq(line+lptr,"-=")) return k;
 else if (match("-"))
  {k=heir10(lval);
  if (k) rvalue(lval);
  neg();
  return (0);
  }
 else if(streq(line+lptr,"*=")) return k;
 else if(match("*"))
  {k=heir10(lval);
  if(k)rvalue(lval);
  lval[1]=cint;
  if(ptr=lval[0])lval[1]=ptr[type];
  lval[2]=0;  /* flag as no pointer or array */
  lval[3]=0;  /* and no constant */
  return (1);
  }
 else if(streq(line+lptr,"&=")) return k;
 else if(match("&"))
  {k=heir10(lval);
  if(k==0)
   {errrpt("illegal address");
   return (0);
   }
  else if(lval[1])return (0);
  else
   {immed();
   outstr(ptr=lval[0]);
   nl();
   lval[1]=ptr[type];
   return (0);
   }
  }
 else 
  {k=heir11(lval);
  if(match("++"))
   {if(k==0)
    {needlval();
    return (0);
    }
   if(lval[1])push();
   rvalue(lval);
   inc();
   ptr=lval[0];
   if((ptr[ident]==pointer)&&(ptr[type]==cint)&&lval[2])
     inc();
   store(lval);
   dec();
   if((ptr[ident]==pointer)&&(ptr[type]==cint)&&lval[2])
    dec();
   return (0);
   }
  else if(match("--"))
   {if(k==0)
    {needlval();
    return (0);
    }
   if(lval[1])push();
   rvalue(lval);
   dec();
   ptr=lval[0];
   if((ptr[ident]==pointer)&&(ptr[type]==cint)&&lval[2])
     dec();
   store(lval);
   inc();
   if((ptr[ident]==pointer)&&(ptr[type]==cint)&&lval[2])
    inc();
   return (0);
   }
  else return (k);
  }
}
