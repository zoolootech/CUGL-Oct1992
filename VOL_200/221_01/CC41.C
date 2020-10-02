/* >>>>>> start of cc4 <<<<<<< */

keepch(c)
 char c;
{ mline[mptr]=c;
 if(mptr<mpmax)mptr++;
 return (c);
}
preprocess()
{ int k;
 char c,sname[namesize];
 if(cmode==0)return;
 mptr=lptr=0;
 while(ch())
  {if((ch()==' ')||(ch()==9))
   {keepch(' ');
   while((ch()==' ')|
    (ch()==9))
    gch();
   }
  else if(ch()=='"')
   {keepch(ch());
   gch();
   while(ch()!='"')
    {if(ch()==0)
      {errrpt("missing quote");
      break;
      }
    keepch(gch());
    }
   gch();
   keepch('"');
   }
  else if(ch()==39)
   {keepch(39);
   gch();
   while(ch()!=39)
    {if(ch()==0)
      {errrpt("missing apostrophe");
      break;
      }
    keepch(gch());
    }
   gch();
   keepch(39);
   }
  else if((ch()=='/')&&(nch()=='*'))
   {inchar();inchar();
   while(((ch()=='*')&
    (nch()=='/'))==0)
    {if(ch()) inchar();
    else {
     inline();
     if(eof)break;
     }
    }
   inchar();inchar();
   }
  else if(an(ch()))
   {k=0;
   while(an(ch()))
    {if(k<namemax)sname[k++]=ch();
    gch();
    }
   sname[k]=0;
   if(k=findmac(sname))
    while(c=macq[k++])
     keepch(c);
   else
    {k=0;
    while(c=sname[k++])
     keepch(c);
    }
   }
  else keepch(gch());
  }
 keepch(0);
 if(mptr>=mpmax)errrpt("line too long");
 lptr=mptr=0;
 while(line[lptr++]=mline[mptr++]);
 lptr=0;
 }
addmac()
{ char sname[namesize];
 if(symname(sname)==0)
  {illname();
  kill();
  return;
  }
 addglb(sname,MACRO,0,macptr,MACRO);
 while(ch()==' ' || ch()==9) gch();
 while(putmac(gch()));
 if(macptr>=macmax)errrpt("macro table full");
 }
putmac(c)
 char c;
{ macq[macptr]=c;
 if(macptr<=macmax)++macptr;
 return (c);
}
findmac(sname) char *sname; {
  if((findglb(sname)!=0)&&(cptr[ident]==MACRO))
    return(getint(cptr+offset,offsize));
  return (0);
}

outbyte(c) char c; {
  if(c==0) return(0);
  if(stagenext) {
    if(stagenext==stagelast) {
      errrpt("staging buffer overflow");
      return 0;
      }
    else *stagenext++ = c;
    }
  else cout(c,output);
  return c;
  }

cout(c, fd) char c; int fd; {
  if(putc(c, fd)==EOF) xout();
  }

sout(string, fd) char *string; int fd; {
 if(fputs(string, fd)==EOF) xout();
 }

xout() {
 fputs("output error\n", stderr);
 exit(ERRCODE);
 }

fputs(string, fp) char *string; int fp; {
 char c;
 while(c = *string++) if((c=putc(c, fp)) <= NULL) return(c);
 }

outstr(ptr)
 char ptr[];
 {
 int k;
 k=0;
 while(outbyte(ptr[k++]));
 }
cnl() /* print CR to the console */
#ifdef VMS
 {putchar(EOL);}
#else
 {putchar(CR);}
#endif

nl()
 {outbyte(EOL);}

tab()
#ifdef TAB
 {outbyte(TAB);}
#else
 {outbyte(' ');}
#endif

errrpt(ptr)
 char ptr[];
 {
 int k;
/*
**  output to console and stop on request implemented
**  Dieter H. Flunkert   04-may-86
*/
 if(stdout!=output) {
   putc('\n',stderr);  
   fputs(line,stderr);
   fputs("\n*****  ",stderr);
   fputs(ptr,stderr);
   fputs("  *****\n",stderr);
 }
 if(pause) {
    fputs("\n? ",stderr);
    if(getc(stderr)==BREAK) exit(0);
 }
 comment();outstr(line);nl();comment();
 k=0;
 while(k<lptr)
  {if(line[k]==9) tab();
   else outbyte(' ');
  ++k;
  }
 outbyte('^');
 nl();comment();outstr("******  ");
 outstr(ptr);
 outstr("  ******");
 nl();
 ++errcnt;
 }
ol(ptr)
 char ptr[];
{
 ot(ptr);
 nl();
}
ot(ptr)
 char ptr[];
{
 tab();
 outstr(ptr);
}
streq(str1,str2)
 char str1[],str2[];
 {
 int k;
 k=0;
 while (*str2)
  {if ((*str1)!=(*str2)) return (0);
  ++k; ++str1; ++str2;
  }
 return (k);
 }
astreq(str1,str2,len)
 char str1[],str2[];int len;
 {
 int k;
 k=0;
 while (k<len)
  {if ((*str1)!=(*str2))break;
  if(*str1==0)break;
  if(*str2==0)break;
  ++k; ++str1; ++str2;
  }
 if (an(*str1))return (0);
 if (an(*str2))return (0);
 return (k);
 }
match(lit)
 char *lit;
 {
 int k;
 blanks();
 if (k=streq(line+lptr,lit))
  {lptr=lptr+k;
  return (1);
  }
  return (0);
}
amatch(lit,len)
 char *lit;int len;
 {
 int k;
 blanks();
 if (k=astreq(line+lptr,lit,len))
  {lptr=lptr+k;
  while(an(ch())) inbyte();
  return (1);
  }
 return (0);
 }
blanks()
 {while(1)
  {while(ch()==0)
   {inline();
   preprocess();
   if(eof)break;
   }
  if(ch()==' ')gch();
  else if(ch()==9)gch();
  else return;
  }
 }
PDBody( item)
int item;
    {
    char chtr;
    if ( item != 0)
 {
 chtr = ( item % 10) + '0'; /*Calculate rightmost character*/
 PDBody( item / 10);
 outbyte(chtr);  
 }
    }

setstage(before, start) int *before, *start; {
  if((*before=stagenext)==0) stagenext=stage;
  *start=stagenext;
  }

clearstage(before, start) char *before, *start; {
  *stagenext=0;
  if(stagenext=before) return;
  if(start) {
#ifdef OPTIMIZE
    peephole(start);
#else
    sout(start,output);
#endif
    }
  }

outdec(item)
int item;
{
    if (item == 0)
 outbyte( '0');
    else if (item == -32768)
 outstr( "-32768");
    else if (item < 0)
 {
 outbyte( '-');
 PDBody( -item);
 }
    else
 PDBody( item);
}
