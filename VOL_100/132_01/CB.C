/*	CB.C small c compiler section	*/
/*	This section is preceeded by CA.C and followed by CD.C	*/
/* */
#INCLUDE CDEF.H
/*	Start of cc4	*/
/*     >>>>>>>>>>>>>><<<<<<<<<<<<<<<<<< */
keepch(c)
	char c;
{	mline[mptr]=c;
	if(mptr<mpmax) mptr++;
	return c;
}
preprocess()
{
	int k;
	char c,sname[namesize];
	if(cmode==0)return;
	mptr=lptr=0;
	while(ch())
		{
		if((ch()==' ') || (ch()==9))
			{
			keepch(' ');
			while((ch()==' ')||(ch()==9))
				gch();
			}
		else if (ch()=='"')
			{
			keepch(ch());
			gch();
			while(ch()!='"')
				{if(ch()==0)
					{error("missing quote");
				break;
				}
			keepch(gch());
			}
		gch();
		keepch('"');
		}
	else if (ch()==39)
			{
		keepch(39);
		gch();
		while(ch()!=39)
			{if(ch()==0)
				{error("missing apostrophe");
				break;
				}
			keepch(gch());
			}
		gch();
		keepch(39);
		}
	else if ((ch()=='/')&&(nch()=='*'))
			{
		while(!((ch()=='*')&&(nch()=='/')))
			{if(ch()==0)inline();
				else inchar();
			if (eof) break;
			}
		inchar (); inchar();
		}
	else if(an(ch()))
			{
		k=0;
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
			while (c=sname[k++])
				keepch(c);
			}
		}
		else {
				keepch(gch());
			}
	}
	keepch(0);
	if(mptr>=mpmax)error("line too long");
	lptr=mptr=0;
	while(line[lptr++]=mline[mptr++]);
	lptr=0;
	}
addmac()
{	char sname[namesize];
	int k;
	if(symname(sname)==0)
		{illname();
		kill();
		return;
		}
	k=0;
	while(putmac(sname[k++]));
	while((ch()==' ') || (ch()==9)) gch();
	while(putmac(gch()));
	if(macptr>=macmax)error("macro table full");
	}
putmac(c)
	char c;
{
	macq[macptr]=c;
	if(macptr<macmax)macptr++;
	return c;
}
findmac(sname)
	char *sname;
{	
	int k;
	k=0;
	while(k<macptr)
		{if(astreq(sname,macq+k,namemax))
			{while(macq[k++]);
			return k;
			}
		while(macq[k++]);
		while(macq[k++]);
		}
	return 0;
}
outbyte(c)
	char c;
{
	if(c==0) return 0;
	if(output)
		{if((putc(c,iobuf0))==EOF)
			{closeout();
			error("output file error");
			}
		}
	else putchar(c);
	return c;
}
outstr(ptr)
	char ptr[];
{
	int k;
	k=0;
	while (outbyte(ptr[k++]));
}
nl()
{	outbyte(eol);outbyte(lf);}
tab()
{	outbyte(9);}
col()
{	outbyte(58);}
error(ptr)
	char ptr[];
{
	int k;
	comment();outstr(line);nl();comment();
	printf("#error line ... %s\n",line);
	k=0;
	while(k<lptr)
		{if(line[k]==9) tab();
			else outbyte(' ');
		++k;
		}
	outbyte('^');
	nl();comment();outstr("***** ");
	outstr(ptr);
	outstr(" *****");
	nl();
	++errcnt;
}
ol(ptr)
	char ptr[];
{	ot(ptr);
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
	while (str2[k])
		{if((str1[k])!=(str2[k])) return 0;
		k++;
		}
	return k;
}
astreq(str1,str2,len)
	char str1[],str2[];
	int len;
{
	int k;
	k=0;
	while(k<len)
		{if((str1[k])!=(str2[k])) break;
		if(str1[k]==0)break;
		if(str2[k]==0)break;
		k++;
		}
	if (an(str1[k])) return 0;
	if (an(str2[k])) return 0;
	return k;
}
match(lit)
	char *lit;
{
	int k;
	blanks();
	if (k=streq(line+lptr,lit))
		{lptr=lptr+k;
		return 1;
		}
	return 0;
}
amatch(lit,len)
	char *lit;
	int len;
{
	int k;
	blanks();
	if (k=astreq(line+lptr,lit,len))
		{lptr=lptr+k;
		while(an(ch())) inbyte();
		return 1;
		}
	return 0;
}
blanks()
{
	while(1)
		{while(ch()==0)
			{inline();
			preprocess();
			if(eof) break;
			}
		if(ch()==' ')gch();
		else if(ch()==9)gch();
		else return;
		}
}
outdec(number)
	int number;
{
	int k,zs;
	char c;
	zs=0;
	k=10000;
	if(number<0)
		{number=-(number);
		outbyte('-');
		}
	while (k>=1)
		{c=(number/k) + '0';
		if((c!='0')||(k==1)||(zs))
			{zs=1,outbyte(c);}
		number=number%k;
		k=k/10;
		}
}
;
		}
}
;
		k=k/10;
