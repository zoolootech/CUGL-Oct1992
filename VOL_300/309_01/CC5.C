/*
HEADER:		Small-C source part 5;
FILENAME:	CC5.C;
AUTHORS:	Dieter H. Flunkert;
COMPILERS:Turbo C V2.0 Medium Memory Model;
SYSTEM: MSDOS (Brian Brown, Nov 1989)
*/

#include <stdio.h>
#include "ccdef.c"

/* external function declaration */
extern int add(), and(), asl(), asr(), blanks(), ch(), clearstage(),
  div(), doublereg(), eq(), ge(), getlabel(), gt(), heir7(), immed(),
  inbyte(), jump(), le(), loadsec(), lt(), match(), mod(), mult(),
  needbrack(), needlval(), ne(), nl(), or(), outdec(), outstr(),
  postlabel(), prelabel(), crs8label(), pop(), push(), pushsec(),
  rvalue(), setstage(), store(), streq(), sub(), testjump(), testnoopt(), testtruejump(),
  uge(), ugt(), ule(), ult(), xor();

/* external value declaration */
extern int lptr, asmtype;

extern char *cptr, line[linesize];

doexpression()
{
 char *before, *start;
 while(1) {
  setstage(&before, &start);
  expression();
  clearstage(before, start);
  if(ch() != ',') break;
  inbyte();
 }
}

expression()
{
 int lval[8];
 if(heir1(lval)) rvalue(lval);
}

/* implemented +=, -=, *= etc. operator 13-jul-86  Dieter H. Flunkert */
heir1(lval)
int lval[];
{
 char *off_set;
 int k, lval2[8];
 lval[2]=lval[3]=1;
 k=heir1a(lval);
 blanks();
 off_set = line + lptr;
 if(streq(off_set, "+=") == 0)
  if(streq(off_set, "-=") == 0)
   if(streq(off_set, "*=") == 0)
    if(streq(off_set, "/=") == 0)
     if(streq(off_set, "%=") == 0)
      if(streq(off_set, "<<=") == 0)
       if(streq(off_set, ">>=") == 0)
        if(streq(off_set,"&=") == 0)
         if(streq(off_set, "|=") == 0)
          if(streq(off_set, "^=") == 0)
           if(ch() != '=') return k;
 if(k==0) {
   needlval();
   return 0;
 }
 if(lval[1]) push();
 else if(ch() != '=') {
   immed();
   outstr(*lval+name);
   nl();
   push();
 }
 if (match("=")) {
  if(heir1(lval2)) rvalue(lval2);
 }
 else if(match("+=")) {
  if(heir1(lval2)) rvalue(lval2);
  loadsec();
  pushsec();
  if(cptr=lval[0])
   if((cptr[ident] == pointer) && (cptr[type] == cint) && lval[2]) doublereg();
  add();
 }
 else if(match("-=")) {
  if(heir1(lval2)) rvalue(lval2);
  loadsec();
  pushsec();
  if(cptr=lval[0])
   if((cptr[ident] == pointer) && (cptr[type] == cint) && lval[2]) doublereg();
  sub();
 }
 else if(match("*=")) {
  if(heir1(lval2)) rvalue(lval2);
  loadsec();
  mult();
 }
 else if(match("/=")) {
  if(heir1(lval2)) rvalue(lval2);
  loadsec();
  div();
 }
 else if(match("%=")) {
  if(heir1(lval2)) rvalue(lval2);
  loadsec();
  mod();
 }
 else if(match(">>=")) {
  if(heir1(lval2)) rvalue(lval2);
  loadsec();
  pushsec();
  asr();
 }
 else if(match("<<=")) {
  if(heir1(lval2)) rvalue(lval2);
  loadsec();
  pushsec();
  asl();
 }
 else if(match("&=")) {
  if(heir1(lval2)) rvalue(lval2);
  loadsec();
  pushsec();
  and();
 }
 else if(match("^=")) {
  if(heir1(lval2)) rvalue(lval2);
  loadsec();
  pushsec();
  xor();
 }
 else if(match("|=")) {
  if(heir1(lval2)) rvalue(lval2);
  loadsec();
  pushsec();
  or();
 }
 store(lval);
 return 0;
}

/* '?:', '&&' and '||' operator implemented Dieter H. Flunkert 18-jul-86 */
heir1a(lval)
   int lval[];
{
   int k, lab, endlab;
   k=heir1b(lval);
   if(match("?")) {
   if(k) rvalue(lval);
    testjump(lab=getlabel());
    endlab=getlabel();
    if(heir1a(lval)) rvalue(lval);
    jump(endlab);
    needbrack(":");
    prelabel(lab);
    if(heir1a(lval)) rvalue(lval);
    prelabel(endlab);
    return 0;
   }
   else return k;
}

heir1b(lval)
   int lval[];
{
   int k, lval2[8], lab, endlab;
   k=heir1c(lval);
   blanks();
   if(streq(line+lptr,"||") == 0) return k;
   if(k) rvalue(lval);
   while(1) {
    if(match("||")) {
     testtruejump(endlab=getlabel());
     if(heir1c(lval2)) rvalue(lval2);
     testtruejump(endlab);
     immed();
     outdec(0);nl();
     jump(lab=getlabel());
     prelabel(endlab);
     immed();
     outdec(1);nl();
     prelabel(lab);
    }
    else return 0;
   }
}

heir1c(lval)
   int lval[];
{
   int k, lval2[8], endlab;
   k=heir2(lval);
   blanks();
   if(streq(line+lptr,"&&") == 0) return k;
   if(k) rvalue(lval);
   while(1) {
    if(match("&&")) {
     testnoopt(endlab=getlabel());
     if(heir2(lval2)) rvalue(lval2);
     testnoopt(endlab);
     immed();
     outdec(1);
     nl();
     prelabel(endlab);
    }
    else return 0;
   }
}

heir2(lval)
 int lval[];
{ int k,lval2[8];
 k=heir3(lval);
 blanks();
 if(ch()!='|')return (k);
 if(streq(line+lptr,"|=")) return k;
 if(k)rvalue(lval);
 while(1)
  {if(streq(line+lptr,"||")) return 0;
  if (match("|"))
   {push();
   if(heir3(lval2)) rvalue(lval2);
   or();
   }
  else return (0);
  }
}

heir3(lval)
 int lval[];
{ int k,lval2[8];
 k=heir4(lval);
 blanks();
 if(ch()!='^')return (k);
 if(streq(line+lptr,"^=")) return k;
 if(k)rvalue(lval);
 while(1)
  {if (match("^"))
   {push();
   if(heir4(lval2))rvalue(lval2);
   xor();
   }
  else return (0);
  }
}

heir4(lval)
 int lval[];
{ int k,lval2[8];
 k=heir5(lval);
 blanks();
 if(ch()!='&')return (k);
 if(streq(line+lptr,"&=")) return k;
 if(k)rvalue(lval);
 while(1)
  {if(streq(line+lptr,"&&")) return 0;
  if (match("&"))
   {push();
   if(heir5(lval2))rvalue(lval2);
   and();
   }
  else return (0);
  }
}

heir5(lval)
 int lval[];
{
 int k,lval2[8];
 char *off_set;
 k=heir6(lval);
 off_set=line+lptr;
 blanks();
 if((streq(off_set,"==")==0)&&
  (streq(off_set,"!=")==0))return (k);
 if(k)rvalue(lval);
 while(1)
  {if (match("=="))
   {push();
   if(heir6(lval2))rvalue(lval2);
   eq();
   }
  else if (match("!="))
   {push();
   if(heir6(lval2))rvalue(lval2);
   ne();
   }
  else return (0);
  }
}

heir6(lval)
 int lval[];
{
 int k,lval2[8];
 char *off_set;
 k=heir7(lval);
 blanks();
 off_set=line+lptr;
 if((ch()!='<') & (ch() != '>') &
 (streq(off_set,"<=")==0) & (streq(off_set,">=")==0)) return (k);
 if(streq(off_set,">>"))return (k);
 if(streq(off_set,"<<"))return (k);
 if(streq(off_set,">>=")) return k;
 if(streq(off_set,"<<=")) return k;
 if(k)rvalue(lval);
 while(1)
  {if (match("<="))
   {push();
   if(heir7(lval2))rvalue(lval2);
   if(cptr=lval[0])
    if((cptr[ident]==pointer)&&lval[2])
    {
    ule();
    continue;
    }
   if(cptr=lval2[0])
    if((cptr[ident]==pointer)&&lval[2])
    {
    ule();
    continue;
    }
   le();
   }
  else if (match(">="))
   {push();
   if(heir7(lval2))rvalue(lval2);
   if(cptr=lval[0])
    if((cptr[ident]==pointer)&&lval[2])
    {
    uge();
    continue;
    }
   if(cptr=lval2[0])
    if((cptr[ident]==pointer)&&lval[2])
    {
    uge();
    continue;
    }
   ge();
   }
  else if(ch() == '<')
   {inbyte();
   push();
   if(heir7(lval2))rvalue(lval2);
   if(cptr=lval[0])
    if((cptr[ident]==pointer)&&lval[2])
    {
    ult();
    continue;
    }
   if(cptr=lval2[0])
    if((cptr[ident]==pointer)&&lval[2])
    {
    ult();
    continue;
    }
   lt();
   }
  else if(ch() == '>')
   {inbyte();
   push();
   if(heir7(lval2))rvalue(lval2);
   if(cptr=lval[0])
    if((cptr[ident]==pointer)&&lval[2])
    {
    ugt();
    continue;
    }
   if(cptr=lval2[0])
    if((cptr[ident]==pointer)&&lval[2])
    {
    ugt();
    continue;
    }
   gt();
   }
  else return (0);
  }
}
