/********************************************************/
/*		 ROFF5, Version 2.00			*/
/*    (c) 1983,4,8,9 by Ernest E. Bergmann		*/
/*	730 Seneca Street, Bethlehem, Pa. 18015		*/
/*							*/
/* Permission is hereby granted for all commercial and	*/
/* non-commercial reproduction and distribution of this	*/
/* material provided this notice is included.		*/
/********************************************************/
#include <roff5.h>
#if (VERSION!=2) || (subVERSION!=00)
#error ***************This is version 2.00******************
#endif
/****************************************/
void puttl3(char *s1,char *s2,char *s3,int pageno, FORMAT fmt)
		/*put out three part title, none
		containing '\n', with
		optional page numbering; aligning
		with page margins,0 & RMVAL;*/
{int size1,gap1,size2,gap2,size3;
	size1 =strln4(s1,FALSE,pageno,fmt);
	OUTTOP=LTOP;OUTBOT=LBOT;
	size2 =strln4(s2,FALSE,pageno,fmt);
	if(LTOP<OUTTOP) OUTTOP=LTOP;
	size3 =strln4(s3,FALSE,pageno,fmt);
	if(LTOP<OUTTOP) OUTTOP=LTOP;
	gap1 = ((RMVAL-INVAL-size2)/2) - size1;
	gap2 = RMVAL-INVAL-size1-size2-size3-gap1;
	blankb(INVAL);
	puttl(s1,pageno,fmt);
	blankb(gap1);
	puttl(s2,pageno,fmt);
	blankb(gap2);
	puttl(s3,pageno,fmt);
	putlinout(FALSE);
}
/****************************************/
void blankb(int i) /*sends i blanks to OUTBUF2, handles negative i*/
{	if(i<0)	for( ; i ;i++) {putout('^');putout('H');}
	else for( ; i ; i--) putout( BLANK );
}
/****************************************/
void gettl3(char *sl,char ttl1[MAXLINE],char **ttl2,char **ttl3)
	/* Gets from source line (s1) three part title that it
	transfers to buffer delineated by ttl1 which has
	capacity for all three strings; none of the strings
	will contain '\n' */
{char c, cc, *dp;
	/*pass over command*/
	for(c=*sl;c!=' '&&c!='\n'&&c!='\t';sl++)c=*sl;
	/*advance to first non-blank or '\n' */
	for(;c==' '||c=='\t';sl++)c=*sl;
	/*advance beyond delim. if present:*/
	if(c!='\n'&&!c) sl++;
	dp=ttl1;
	transfer(&sl,&dp,c);
	*ttl2=dp;
	transfer(&sl,&dp,c);
	*ttl3=dp;
	transfer(&sl,&dp,c);
}
/********************************************************/
/* title3() is the combination of gettl3() and puttl3() */
/* used to implement the .tl command.			*/
/********************************************************/
void title3(char *sl, int pageno, FORMAT fmt)
{char TLBUFF[MAXLINE],*ttl2,*ttl3;
 gettl3(sl,TLBUFF,&ttl2,&ttl3);
 puttl3(TLBUFF,ttl2,ttl3,pageno,fmt);
}
/***************************************************************/
/* Copy string from source to destination.  Original delim.    */
/*can be \0, \n, or char.  The pointer to the source is updated*/
/*to point at the \0, \n, or past char.  In destination, delim.*/
/*is always replaced by \0.  The destination pointer always    */
/*points past this \0.					       */
/***************************************************************/
void transfer(char **s,char **d,char c)
{char a;
	a=**s;
	while(a!=c && a!='\n' && a)
		{**d = a;  (*d)++;
		(*s)++;  a=**s;
		}
	**d='\0';
	(*d)++;
	if(a!='\n' && a!='\0') (*s)++;
}
/**********************************************************
		centers a line of text
**********************************************************/
void center(char *line)
{TIVAL = max(( RMVAL+TIVAL-strln4(line,FALSE,-1,PAGFMT))/2, 0 );
OUTTOP=LTOP;OUTBOT=LBOT;
return;
}
/************************************************************
Transfers next word from in to out.  Scans off leading white
space from in.  If there is no word, returns FALSE.
Otherwise, input is truncated on left of word which is
transfered to out without leading or trailing blanks.
WE_HAVE_A_WORD will be returned.  If the transfered word
terminates a sentence then SENTENCE is set to 1, otherwise it
is reset to FALSE.
**************************************************************/
int getwrd(char *in,char *out )
{char *pin,*pout,c,cm,cp;
 int i;
	for(i=0; (c=in[i])!='\0'; i++)	/*replace tabs with spaces*/
	{if (c==TAB) in[i]=BLANK;
	 else if (c=='^')
		{c=in[i+1];
		 if ((c=='t')||(c=='T')) in[i]=in[i+1]=BLANK;
		}
	}
	pin=in;
	while(*pin==BLANK || *pin==NEWLINE) pin++;
	pout=out;
	c=*pin;
	SENTENCE=FALSE;
	if(c==NEWLINE || c=='\0')
		{*out='\0';
		return(FALSE);
		}
	while(c!=BLANK && c!=NEWLINE && c)
		{*(pout++)=c;
		*pin=BLANK;
		c=*(++pin);
		}
	*pout='\0';	/*terminate out string*/
	cm=*(pout-1);
	cp=*(pin+1);
	switch (cm)
		{case ':' :
		case ';' :
		case '?' :
		case '!' :
			SENTENCE=1;
			break;
		case '.' :
			if(cp==BLANK||cp==NEWLINE||c==NEWLINE)
				SENTENCE=1;
		}
	return(WE_HAVE_A_WORD);
}
/*********************************/
int gwLIN(char *out)
{return(getwrd(LIN,out)); }
/*******************************************************
Truncates white-space characters at the end of a string.
********************************************************/
void trunc_bl (char *string)
{char *ptr;
int k;
k = strlen (string);
ptr = &string[ k-1 ];	/* char before terminating nul */
while (*ptr==BLANK || *ptr==TAB || *ptr==NEWLINE)
	*ptr--  = '\0';
}
/*********************************************
	distribute words evenly across a line
**********************************************/
void spread ( line, nextra, no_words)
char *line;
int nextra;	/* no. extra places left in line */
int no_words;   /* no. words in the line         */
{int i, j, nblanks, nholes, half;
if (nextra <= 0 ) return;
if (DOTJ==0) return;
if ((no_words<=1)&&(DOTJ==1))	return;
DIR = !(DIR);
nholes = no_words - 1;
trunc_bl (line);
i = strlen(line) - 1 ; /* last character of string */
j = min(MAXLINE-2,i+nextra);	/* last  position in output */
line[j+1] = '\0';
switch(DOTJ)
{case 1: /*both margins*/
	 for ( ; i<j ; i--, j-- )
		{ line[j] = line[i];
		  if ( line[i] == BLANK)
		      { if (DIR == 0) nblanks=(nextra-1)/nholes+1;
			else		nblanks = nextra/nholes;
			nextra = nextra - nblanks;
			nholes = nholes - 1;
			for ( ; nblanks > 0;  nblanks-- )
				line[--j] = BLANK;
		      }
		}
	break;
 case 3: /*center*/
	for(half=nextra/2; half; half--)
		line[j--] = BLANK;
	for( ; i>=0; i--,j-- )
		line[j] = line[i];
	for( ; j>=0; j-- )
		line[j] = BLANK;
	break;
 case 5: /*right margin only*/
	for( ; i>=0; i--,j-- )
		line[j] = line[i];
	for( ; j>=0; j-- )
		line[j] = BLANK;
 }
}
/************************************************/
void adjust()	/* handle .ad request */
{int i; ARGs a; char c;
 i = process(&a);
 JUSTIFY = YES;
 if(i==0) return;
 c = toupper(a.arg[1][0]);
 switch(c)
	{case 'L': DOTJ = 0; break;
	 case 'B':
	 case 'N': DOTJ = 1; break;
	 case 'C': DOTJ = 3; break;
	 case 'R': DOTJ = 5; break;
	}
}
/************************************************
place portion of title line with optional page no. in OUTBUF2
*************************************************/
void puttl (char *str, int num, FORMAT fmt)
{char c;
for ( ;'\0'!=(c= *str); str++)
	{if ( c != NUMSIGN ) putout(c);
	else putnum(num,fmt);
	}
}
/*******************************************
	put out num to OUTBUF2 (conversion)
********************************************/
void putnum ( int num, FORMAT fmt )
{int i, nd;
 char chars[MAXLINE];
	nd = itoC( num, chars, fmt );
	for ( i=0;i<nd; i++) putout(chars[i]);
}
/*********************************************/
/* itoC() converts a non-negative num <30000 */
/* into a character representation returned  */
/* in buff[] in the format specified by fmt. */
/* fmt = (fill char) + 128*(min width).  It  */
/* returns the length of the string.         */
/*********************************************/
int  itoC(int num, char buff[], FORMAT fmt)
{int sl;
 if (fmt.typ == 'i') strlwr(itoR(num, buff));
 else if (fmt.typ=='I') itoR(num, buff);
 else if (fmt.typ=='a') strlwr(itoA(num, buff));
 else if (fmt.typ=='A') itoA(num, buff);
 else itoa(num, buff, 10);
 sl = strlen(buff);
 if (sl >= fmt.minwidth) return (sl);
 /* we need to left pad the string */
 do buff[fmt.minwidth--] = buff[sl--];  while (sl >=0);
 do buff[fmt.minwidth--] = fmt.fill;  while (fmt.minwidth >=0);
 return ( strlen(buff) );
}
/*****************************************************************/
/* itoR converts an integer into upper case roman numeral string */
/*  it is similar in spirit to itoa()                            */
/*****************************************************************/
char *itoR(int value, char *string )
{int digit;
string[0]='\0';
if (value==0) {string[0]='0'; string[1]='\0';}
if (value<1)  return(string);
digit = value / 1000 ;
while (digit>0) {strcat(string,"M");digit--;}
value = value % 1000;
digit = value / 100 ;
if (digit<4)
   while (digit>0) {strcat(string,"C");digit--;}
if (digit==4) strcat(string,"CD");
if (digit==9) strcat(string,"CM");
else if (digit>4)
	{strcat(string,"D");
         while (digit>5) {strcat(string,"C"); digit--;}
         }
value = value % 100;
digit = value / 10 ;
if (digit<4)
   while (digit>0) {strcat(string,"X");digit--;}
if (digit==4) strcat(string,"XL");
if (digit==9) strcat(string,"XC");
else if (digit>4)
	{strcat(string,"L");
         while (digit>5) {strcat(string,"X"); digit--;}
         }
digit = value % 10;
if (digit<4)
   while (digit>0) {strcat(string,"I");digit--;}
if (digit==4) strcat(string,"IV");
if (digit==9) strcat(string,"IX");
else if (digit>4)
	{strcat(string,"V");
         while (digit>5) {strcat(string,"I"); digit--;}
         }

return (string);
}
/*****************************************************************/
/* itoA converts an integer into 0, A, B, .. AA, AB,..  strings  */
/*  it is similar in spirit to itoa()                            */
/*****************************************************************/
char *itoA(int value, char *string )
{int digit;
 char c;
 char lbuff[10], *sp;
 string[0]='\0';
 if (value==0) {string[0]='0'; string[1]='\0';}
 if (value<1) return(string);
 sp = &lbuff[9];
 *sp = '\0';
 while (value > 0)
	{digit = value % 26;
	 if (digit==0)  *(--sp)='Z';
	 else *(--sp) = 'A'-1+digit;
	 value -= *sp - 'A'+1 ;
	 value /= 26;
	}
 strcpy(string,sp);
 return ( string );
}
/****************************************/
void putout(char c)	/*places c in OUTBUF2[]*/
{if(c==SCVAL[0]) c= BLANK;
if(c==NEWLINE)c='\0';
OUTBUF2[BPOS++]=c;
OUTBUF2[BPOS]='\0';/*safty net*/
}
