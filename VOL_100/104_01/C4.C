/*      >>>>>> start of cc4 <<<<<<<     */
#ifndef TRUE	/* check to see if need include file */
#include <C.DEF>
#endif
keepch(c)
char c;
{
	mline[mptr]=c;
	if (mptr<mpmax) mptr++;
	return c;
	}
preprocess()
{
	int k;
	char c,sname[namesize];
	if (!cmode) return;
	mptr=lptr=0;
	while(!cif && !eof) {
		if (match("#endif")) cif=TRUE;
		else inline();
		}
	if (ch() == '#') match("#endif");
	while(ch()) {
		if (isspace(ch())) {
			keepch(' ');
			while(isspace(ch())) gch();
			}
		else if (ch() == '\\') {
			keepch(ch());
			gch();
			if (!ch()) {
				error("missing escape character");
				break;
				}
			keepch(ch());
			gch();
			}
		else if(ch()=='"') {
			keepch(ch());
			gch();
			while(ch()!='"') {
				if(!ch()) {
				  error("missing quote");
				  break;
				  }
				if (ch() == '\\') keepch(gch());
				keepch(gch());
				}
			gch();
			keepch('"');
			}
		else if(ch()=='\'') {
			keepch('\'');
			gch();
			while(ch()!='\'') {
				if(!ch()) {
				  error("missing apostrophe");
				  break;
				  }
				if (ch()== '\\') keepch(gch());
				keepch(gch());
				}
			gch();
			keepch('\'');
			}
		else if((ch()=='/') && (nch()=='*')) {
			inchar();
			inchar();
			while(!((ch()=='*') && (nch()=='/'))) {
				if(ch()==0)inline();
				else inchar();
				if(eof)break;
				}
			inchar();
			inchar();
			}
		else if(an(ch())) {
			k=0;
			while(an(ch())) {
				if(k<namemax)sname[k++]=ch();
				gch();
				}
			sname[k]=0;
			if(k=findmac(sname))
				while(c=macq[k++])
					keepch(c);
			else {
				k=0;
				while(c=sname[k++])
					keepch(c);
				}
			}
		else keepch(gch());
		}
	keepch(0);
	if(mptr>=mpmax) error("line too long");
	lptr=mptr=0;
	while(line[lptr++]=mline[mptr++]);
	lptr=0;
	}
addmac()
{
	char sname[namesize];
	int k;
	if (symname(sname) == 0) {
		illname();
		kill();
		return;
		}
	k=0;
	while(putmac(sname[k++]));
	while(isspace(ch())) gch();
	while(putmac(gch()));
	if (macptr >= macmax) error("marco table full");
	}
putmac(c)
char c;
{
	macq[macptr]=c;
	if(macptr < macmax) macptr++;
	return c;
	}
findmac(sname)
char *sname;
{
	int k;
	k=0;
	while(k<macptr) {
		if(astreq(sname,macq+k,namemax)) {
			while(macq[k++]);
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
	if (!c) return 0;	/* if null return */
	if (output) {
		if (putc(c,fout) == ERROR) {
			closeout();
			error("Output file error");
			}
		}
	else putch(c);
	return c;
	}
outstr(ptr)
char *ptr;
{
	while(outbyte(*ptr++));
	}
nl()
{
	outbyte('\r');
	outbyte('\n');
	}
tab()
{
	outbyte('\t');
	}
col()
{
	outbyte(':');
	}
error(ptr)
char *ptr;
{
	char buff[80];
	int  j,k;
	strcpy(buff,ptr);
	strcat(buff," \"");
	j=lptr-10;
	if (j<0) j=0;
	k=strlen(buff);
	while(j-15<lptr && line[j]) buff[k++]=line[j++];
	buff[k]=0;
	strcat(buff,"\"");
	error_print(buff);
	return;
	}
error_print(ptr)
char ptr[];
{
	char buff[80];
	int k,j;
	comment();outstr(line);nl();comment();
	k=0;
	while(k<lptr) {
		if(line[k]==9) tab();
			else outbyte(' ');
		++k;
		}
	outbyte('^');
	nl();comment();outstr("******  ");
	outstr(ptr);
	outstr("  ******");
	nl();
	if (input2) {
		strcpy(buff,"Error in include file at line ");
		sdec(buff+strlen(buff),line2);
		}
	else {
		strcpy(buff,"Error at line ");
		sdec(buff+strlen(buff),line1);
		}
	strcat(buff," ");
	strcat(buff,ptr);
	strcat(buff,"\n");
	pl(buff);
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
	while(str2[k]) {
		if (str1[k]!=str2[k]) return 0;
		k++;
		}
	return k;
	}
astreq(str1,str2,len)
char str1[],str2[];int len;
{
	int k;
	k=0;
	while (k<len) {
		if ((str1[k])!=(str2[k]))break;
		if(!str1[k])break;
		if(!str2[k])break;
		k++;
		}
	if (an(str1[k]))return 0;
	if (an(str2[k]))return 0;
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
char *lit;int len;
 {
	int k;
	blanks();
	if (k=astreq(line+lptr,lit,len)) {
		lptr=lptr+k;
		while(an(ch())) inbyte();
		return 1;
		}
	return 0;
	}
blanks()
{
	while(1) {
		while(ch()==0) {
			inline();
			preprocess();
			if (eof) break;
			}
		if(isspace(ch())) gch();
		else return;
		}
	}
outdec(number)
int number;
{
	char buff[10];
	outstr(sdec(buff,number));
	}
sdec(string,number)
char string[];
int number;
{
	int k,zs,j;
	char c;
	j=zs=0;
	k=10000;
	if (number<0) {
		number=(-number);
		string[j++]='-';
		}
	while (k>=1) {
		c=number/k + '0';
		if (c != '0' || k == 1 || zs) {
			zs=1;
			string[j++]=c; 
			}
		number=number%k;
		k=k/10;
		}
	string[j]=0;
	return string;
	}
/*						*/
/*	to output number as a hex value		*/
/*						*/
/*	written june 28, 1981 by Mike Bernson	*/
/*						*/
outhex(value)
int value;
{
	int  postion;
	char *hex;

	hex="0123456789abcdef";
	for(postion=16; postion >= 0; postion -=4) 
		outbyte(hex[(value>>postion) & 0x0f]);
	outbyte('h');
	} 0x0f]);
	outbyte('h');
	}/* number of case statement in switch */
int label;	/* label for switch t