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
/**************************************************/
int value(base,string)	/*unsigned conversion*/
int base;		/*radix for conversion*/
char *string;		/*no leading blanks please!*/
			/*trailing whitespace or '\0'*/
{int val,d;
char c;
	val=0;
	for(d=digit(*string);d>=0 && d<base ; d=digit(*string))
		{val = val*base + d;
		string++;
		}
	c=*string;
	if(!c || c==' ' || c==TAB || c=='\n') return(val);
	else return(-1);	/*error return is -1*/
}
/**************************************************/
int digit(d)
char d;
{	d=toupper(d);
	if(d<='9') return(d-'0');
	if(d<'A') return(-1); /*error return is negative val*/
	if(d<='Z') return(10-'A'+d);
	return(-1);	/*error*/
}
/**************************************************/
int strln4(s,word,num,fmt)
 /*returns printed string length; checks legality of
		word function;keeps track of vertical
		excursions; records them in globals*/
char *s;
int word;	/* boolean, if true, check is made for none
		black characters in the string*/
int num;	/* for expansion of NUMSIGN;set negative to ignore*/
FORMAT fmt;	/* format for NUMSIGN */
{int i,i2,p1,p2,p3;
int t,b,h;	/*vertical vars*/
char c, *ss;
ss=s;
t=b=h=0;
p3=p2=p1=-LSZ;
for(c=*s,i2=i=0;c;c=*(++s))
	{if(c==NUMSIGN)
		if (num<0) i++;
		else
		{char temp[LSZ];
		 i += itoC(num, temp, fmt);
		}
	else if((c!=TFVAL[0])&&(c!=CFVAL[0]))
		{if((c<=' ')&&(word)) goto error;
		else i++;
		}
	else if(c==CFVAL[0])
		{c=*(++s);
		if(c==TFVAL[0]) goto error;/*both CFVAL,TFVAL*/
		switch(c)
		{case 'h':
		case 'H':if(i>i2) i2=i;
			 if(i) i--;
			else goto error;/*before start*/
			break;
		case '+': h--; if(h<t) t=h; break;
		case '-': h++; if(h>b) b=h; break;
		case 'B':
		case 'b':
		case 'D':
		case 'd':
		case 'u':
		case 'U':
		case 'X':
		case 'x': break;
		case '(': p1=i; break;
		case '[': p2=i; break;
		case '{': p3=i; break;
		case ')': if(i>i2) i2=i; i=p1; break;
		case ']': if(i>i2) i2=i; i=p2; break;
		case '}': if(i>i2) i2=i; i=p3; break;
		default: if(CPTR[c-' ']) break;
		goto error;	/*undecipherable*/
		}}
	else/*c==TFVAL[0]*/
		{if(class(*(s+1))!=BLACK)
		goto error;	/*illegal translation*/
		}
	}
if(h) goto error;
if(word){WTOP=t;
	WBOT=b;
	}
else	{LTOP=t;
	LBOT=b;
	}
if(i>=i2)return(i);
/* else prints beyond last character: */
error:
	/*should be fprint -> stderr*/
	fprintf(stderr,"STRLN4:<%s> is illegally formed\n",
				ss);
	return(strlen(ss));
}
/* A properly formed token string has its first printable
character indicating the lefthand edge and the last printable
character at the right hand edge.  Only legal control pairs
accepted.  It must consist of printable symbols. */
/************************************
set stack like set() sets a variable
*************************************/
void setS(param,val,arg_typ,defval,minval,maxval)
int param[STKSIZ+1],val,defval,minval,maxval;
char arg_typ;
{int i;
	if(val==NO_VAL)
		{for(i=0;i<STKSIZ;i++)	/*pop*/
		param[i]=param[i+1];
		param[STKSIZ]=defval;
		}
	else	{for(i=STKSIZ;i;i--)	/*push*/
		param[i]=param[i-1];
		if (arg_typ=='+') *param+=val;
		else if (arg_typ=='-') *param-=val;
		else *param=val;
		}
	*param=min(max(*param,minval),maxval);
}
/******************************************
initialize stack type variable, st, with v
*******************************************/
void initsk(int st[STKSIZ+1],int v)
{int i;
	for(i=STKSIZ+1;i;st[--i]=v);
}
/*********************************************
sets the value of a character in .sc,.tc, etc.
**********************************************/
void setchar(int *param,int stack,int defval,int minval,int maxval)
{char *lin,c;
 int val;
 lin=LIN;
 c=*lin;
 while((c!=' ')&&(c!='\t')&&(c!='\n')) c=*(++lin);/*skip command*/
 while((c==' ')||(c=='\t')||(c=='\n')) c=*(++lin);/*skip white*/
 if(c) val=c;
 else val=NO_VAL;
 if(stack) setS(param,val,'0',defval,minval,maxval);
 else set(param,val,'0',defval,minval,maxval);
}
/**************************************************/
void getot()	/*process .ot, output translation*/
{char typ,chr,*pchr;
char wrdbuf[MAXLINE], buff[128];
	gwLIN(wrdbuf);	/*remove .pc*/
	if(gwLIN(wrdbuf)==WE_HAVE_A_WORD)
		{typ=*wrdbuf;
		 if ((typ!=TFVAL[0])&&(typ!=CFVAL[0])) return;
		 chr=wrdbuf[1];
		 if (chr<=' ') return;
		}
	else return;	/*error: missing args*/
	if('.'==getcode(buff) )	/*record pointer*/
		{if (NULL==(pchr=(char *)malloc(1+buff[0])))
			exitm("getot(): cannot malloc\n");
		 if (typ==TFVAL[0])
			TPTR[chr-' ']=(char *)memcpy(pchr,buff,1+buff[0]);
		 else	CPTR[chr-' ']=(char *)memcpy(pchr,buff,1+buff[0]);
		}
	else fprintf(stderr,"\n.OT: error in line: %s\n",
						LINE);
}
/************************************************/
/* getcode(char buff[128]) returns '.' if o.k.  */
/* resultant collected codes will be placed in  */
/* buff[128] with buff[0] containing the count. */
/* LINE must contain the radix as the first     */
/* token and it and the following lines then    */
/* contain code values finally delimited by a   */
/* token that starts with a '.' ; comments can  */
/* be at the end of any of these lines,         */
/* set off by " ;"				*/
/************************************************/
char getcode(char *buff)	
{int base,code;	/*conversion radix, value*/
char *pcode,ncode;
char wrdbuf[MAXLINE];
	if(gwLIN(wrdbuf)==WE_HAVE_A_WORD)
	   {switch(toupper(*wrdbuf))
		{case 'B': base=2;break;
		case 'O':
		case 'Q': base=8;break;
		case 'D': base=10;break;
		case 'H': base=16;break;
		default: return(FALSE);	/*error*/
		}
	   }
	else return(FALSE);	/*error: missing arg*/
	pcode = buff++;
	*pcode=ncode = 0;
	while(ncode<127)
	{while(gwLIN(wrdbuf)!=WE_HAVE_A_WORD)
		fgets3();
	 if(';'==*wrdbuf) fgets3();/*comment*/
	 else if('.'==*wrdbuf)
		{*pcode = ncode;		/*save #*/
		return(*wrdbuf);
		}
	 else	{
		if((code=value(base,wrdbuf)) > -1)
			{*buff = code;
			 buff++;
			 ncode++ ;
			}
		else return(*wrdbuf);	/*conversion error*/
		}
	}
	fprintf(stderr,"\nGETCODE: code sequence too long");
	return(FALSE);
}
/**************************************************/
void ocode()	/*process .ou*/
{char wrdbuf[MAXLINE], buff[128];
	gwLIN(wrdbuf);	/*remove .ou*/
	if('.'==getcode(buff)) outstr(buff);
	else fprintf(stderr,"\nOCODE: error in:\n%s",LINE);
}
/**************************************************/
void outstr(char *p)	/*print string whose bytecount is *p */
{int i;
for(i=*(p++); i; i--) cputc(*(p++),ofp);
}
/**************************************************/
void getfr()	/*process .FR ;cf. ocode() */
{char *pchr,wrdbuf[MAXLINE], buff[128];
	gwLIN(wrdbuf);
	if(gwLIN(wrdbuf)==WE_HAVE_A_WORD)
		FRVAL = atoi(wrdbuf);
	else return;
	FRVAL=max(1,FRVAL); FRVAL=min(FRVAL,4);
	if('.'==getcode(buff) )	/*record pointer*/
		{if (NULL==(pchr=(char *)malloc(1+buff[0])))
			exitm("getfr(): cannot malloc\n");
		 if (FRVAL!=1)
			FRSTRING=(char *)memcpy(pchr,buff,1+buff[0]);
		 else
			WHSTRING=(char *)memcpy(pchr,buff,1+buff[0]);
		}
	else fprintf(stderr,"\n.FR: error in line:\n%s",
						LINE);
}
/******************* page traps! *************/
#define MAXINT	32767
#define NTRAPS	10
#define NMSIZ	10
typedef struct
	{int loc;
	 char nam[NMSIZ];
	} tentry;
tentry Ttable[NTRAPS];
char *TRAPNAM;
void when()
{char c,*lptr;
 int i,j,save;
 i=get_val2(LIN,&c,&lptr);
 if(i==NO_VAL) return;
 if(c=='-') i=-i;
 for(j=0;j<NTRAPS;j++)
   if(Ttable[j].loc==i) goto storename;
 /* else not an occupied position */
 for(j=0;j<NTRAPS;j++)
   if(Ttable[j].nam[0]=='\0')
	{Ttable[j].loc=i;
storename:
	 save=SENTENCE;
	 if(!getwrd(lptr,Ttable[j].nam))
		Ttable[j].nam[0]='\0';
	 SENTENCE=save;
	 return;
	}
/* here only if no place to put trap */
 exitm(".wh: used up entire trap table\n");
}
/******************************/
void changetrap()
{char c,wbuf[NMSIZ],*lptr;
 int i,j,save;
 save=SENTENCE;
 gwLIN(wbuf); /*remove command*/
 if(!getwrd(LIN,wbuf))
	{SENTENCE=save; return;} /*if no name*/
 i=get_val2(LIN,&c,&lptr);
 SENTENCE=save;
 for(j=0;j<NTRAPS;j++)
   if(strcmp(wbuf,Ttable[j].nam)==0)
	{if(i==NO_VAL)
		{Ttable[j].nam[0]='\0';
		 return;
		}
	 if(c=='-') i=-i;
	 Ttable[j].loc=i;
	 return;
	}
/* reaches here if no name match */
}
/****************************************/
void itset()
{char c,*lptr;
 int save;
 INTRAP=get_val2(LIN,&c,&lptr);
 if((INTRAP==NO_VAL)||(c=='-'))
	{INTRAP=0;
	 ITstring[0]='\0';
	 return;
	}
 save=SENTENCE;
 if(!getwrd(lptr,ITstring))
	ITstring[0]='\0';
 SENTENCE=save;
 return;
}
/****************************************/
void EMset()
{char *lptr,c;
 int save;
 for(lptr=LIN;(*lptr!=' ') && (*lptr!='\t') && *lptr ; lptr++) ;
 save=SENTENCE;
 if(!getwrd(lptr,EMstring))
	EMstring[0]='\0';
 SENTENCE=save;
}
/****************************************/
/* dotT() returns distance to the next  */
/* page trap from ln or BIG#; as a side */
/* effect, it updates TRAPNAM.          */
/****************************************/
int dotT(int ln)
{int i,j,dist;
 dist=MAXINT-1000;
 for(i=0;i<NTRAPS;i++)	/*search list of traps*/
  if(Ttable[i].nam[0]!='\0')
	{if((j=Ttable[i].loc)<0) j+=PLVAL[0];
	 if( ((j-=ln)>0) && (j<dist) )
		{dist=j;
		 TRAPNAM=Ttable[i].nam;
		}
	}
  return(dist);
}
/************************************/
void setTRAP() /*used to recalculate FOOTLOC,TRAPLOC*/
{ FOOTLOC = PLVAL[0] - M3VAL[0] - M4VAL[0];
  TRAPLOC = VLINENO + dotT(VLINENO);
  if (TRAPLOC>FOOTLOC) TRAPLOC=FOOTLOC;
}
