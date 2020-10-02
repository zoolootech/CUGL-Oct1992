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
/*assuming REVSCROLL is FALSE*/
/***************************************************************/
/* Output OUTBUF2 with the vertical height of the mainline     */
/*specified by VLINENO,FVLINENO[they must not be changed here].*/
/*Excessive superscripting will be pushed down.                */
/***************************************************************/
void printout(int txtlin) /*txtlin is FALSE for titles*/
{int level,top,bot;	/*"up" is negative;units fractional*/
int lsave,fsave;
fsave=FVLINENO;
lsave=VLINENO;
level=FRVAL*(PLINENO-VLINENO)+FPLINENO-FVLINENO;
if(!OLDBOT) level++;
excurs(&OUTBUF2[0],&top,&bot);
if(NUMBERING&&txtlin)
	{int ofst;
	 ofst=LNI+3+LNST;
	 if(LN>999) ofst++;
	 if(LN>9999) ofst++;
	 offset(OUTBUF2,ofst);
	}
if(top>level) level=top;

if(!REVSCROLL) FVLINENO += level;
padv();
for(level=top;
    level<=bot;
    level++)
	{OCNT=0;
	do {OCNT++;
	    flp(level,FALSE);
	   }
	while(retype());
	if(NUMBERING&&txtlin&&!(LN % LNMOD)&&!level)
		{char buff[10],*bp;
		 int ofst;
		 ofst=dotO+LNI;
		 if(LN<100) ofst++;
		 if(LN<10) ofst++;
		 for( ; ofst ;ofst--) cputc(' ',ofp);
		 itoa(LN,buff,10);
		 bp=buff;
		 while(*bp) cputc(*bp++,ofp);
		}
	if(level<bot)
		{fraction();
		cputc('\n',ofp);
		FPLINENO++;
		}
	}
OCNT=MCNT;
flp(level,TRUE);	/*update UF, XF, MCNT*/
OUTBUF2[0]=BPOS=0;
OLDLN=VLINENO=lsave;
OLDBOT=bot;
FVLINENO=fsave;
if(NUMBERING&&txtlin) LN++;
}
/*********************************************/
/* Moves printer vertically so that its      */
/* position is specified by VLINENO,FVLINENO */
/*********************************************/
void padv()
{int w,f;	/*whole,fractional lines*/
w=VLINENO-PLINENO;
f=FVLINENO-FPLINENO;
while(f<0) {w--; f += FRVAL; }
while(f>=FRVAL)	{w++; f -= FRVAL; }
if(w<0){fprintf(stderr,"padv():VL=%d,PL=%d\n",
			VLINENO,PLINENO);
#ifndef REVSCROLL
	if(REVSCROLL) backup(w*FRVAL+f);
	else
#endif
	     {VLINENO +=w;
	      FVLINENO +=f;
	      while(FVLINENO<0){VLINENO++;FVLINENO+=FRVAL;}
	      while(FVLINENO<FRVAL){VLINENO--;FVLINENO-=FRVAL;}
	      return;
	     }
	}
if(FRQ)	{while(f--){cputc('\n',ofp); FPLINENO++; }
	if(w){whole();while(w--){cputc('\n',ofp);PLINENO++;}}
	}
else	{while(w--){cputc('\n',ofp); PLINENO++; }
	if(f){fraction();while(f--){cputc('\n',ofp);FPLINENO++;}}
	}
while(FPLINENO>=FRVAL) {PLINENO++; FPLINENO -= FRVAL; }
}
/****************************************/
#ifndef REVSCROLL
void backup(i)	/*not yet implemented*/
int i;		/*# of fractional lines(probably negative)*/
{fprintf(stderr,"\nCan't back up yet\n");
}
#endif
/********************************************************/
/*Finds the topmost and bottommost line	positions of str*/
/********************************************************/
void excurs(char *str,int *t,int *b)
{int l;
char c;
*t=*b=l=0; /*current line position */
c=*str;
while(c){if(c==CFVAL[0]){if('\0'!=(c=*(++str)))
			switch(c)
			{case '+':l--; if(l<*t) *t=l;
				c=*(++str); break;
			case '-':l++; if(l>*b) *b=l;
				c=*(++str); break;
			default : c=*(++str); break;
			}
		    }
	else c=*(++str);
	}
}

/*****************************************************/
/*fancy line print(flp) at a given vertical level the*/
/*string in OUTBUF2[] with backspacing,	underlining, */
/*and strikout.  To permit boldface it modifies      */
/*DBUF[],DPOS so that retype can be used to patch    */
/*up OUTBUF2 for resubmittal to	flp()		     */
/*****************************************************/
void flp(level,updat)
int level;	/* current vertical level to print*/
int updat;	/* boolean for update of UF,XF,MCNT*/
{int i;
	BLKCNT=lbc(level,OUTBUF2);
	FIRST=TRUE;
	while((BLKCNT>0)||updat)
		{prpass(level,updat); cputc('\r',ofp);
		updat=FIRST=FALSE;
		}
	if(XCOL>-1){for(i=0;i<=XCOL;i++)
			cputc(XBUF[i],ofp);
			cputc('\r',ofp);
		   }
	if(UCOL>-1){for(i=0;i<=UCOL;i++)
			cputc(UBUF[i],ofp);
			cputc('\r',ofp);
		   }
	if((UCOL>-1)||(XCOL>-1)) initxu();
}

/**************************************************/
int retype()	/*restores characters into OUTBUF2 from DBUF
		that need to be overstruck again*/
{int i;
if(DPOS==-1) return(FALSE);
else	{for(i=0;i<=DPOS;i++)
		{if(DBUF[i])
			{OUTBUF2[i]=DBUF[i];
			DBUF[i]=FALSE;
			}
		}
	DPOS=-1;
	return(TRUE);
	}
}

/**************************************************/
int lbc(lev,str) /*counts printable chars in line level and
		above; parity must be reset*/
int lev; /*=0 main line,=-1 superscripts,=+1 subscripts, etc.*/
char *str;
{char c,n;
int l;
l=n=0;
c=*str;
while(c){if(c==CFVAL[0]){if('\0'!=(c=*(++str)))
			switch(c)
			{case '+':l--;c=*(++str);break;
			case '-':l++;c=*(++str);break;
			default: c=*(++str);break;
		    }	}
	else	{if((c>' ')&&(l<=lev)) if(c!=TFVAL[0]) n++;
		c=*(++str);
		}
	}
return(n);
}
/*******************************************/
/* printer pass initial cr; no lf anywhere */
/*******************************************/
void prpass(lev,updat)
int lev; /*=0 main line,=-1 superscripts,=+1 subscripts, etc.*/
int updat;/*boolean to update UF,XF,MCNT*/
{char ch;
int l;
int xfs,ufs,mcnts;	/*save variables*/
int p1,p2,p3;		/*position holders*/
int cp2;	/*for tabulation calculation*/
xfs=XF; ufs=UF; mcnts=MCNT;
p1=p2=p3=l=BPOS=CP=PP=0;
while('\0'!=(ch=OUTBUF2[BPOS]))
	{switch (class(ch))
	{case	BLACK:/*print it if posssible*/
		if((PP>CP)||(l>lev)){CP++;BPOS++;break;}
		else	{while(CP>PP){cputc(' ',ofp);PP++;}
			if(ch==SCVAL[0])cputc(' ',ofp);
			else cputc(ch,ofp);PP++;
			if(MCNT>OCNT)
				{DBUF[BPOS]=OUTBUF2[BPOS];
				if(BPOS>DPOS) DPOS=BPOS;
				}
			OUTBUF2[BPOS++]=' ';
			if(UF&&FIRST)UBUF[UCOL=CP]=UCHAR;
			if(XF&&FIRST)XBUF[XCOL=CP]=XCHAR;
			BLKCNT--; CP++;
			} break;
	case	WHITE:/*assume blank*/ CP++;BPOS++;break;
	case	TRANSLATE:/*similar to BLACK and WHITE*/
			ch=OUTBUF2[++BPOS];
			if((PP>CP)||(l>lev)||(ch==' '))
				{CP++;BPOS++;break;}
			else
			  {while(CP>PP){cputc(' ',ofp);PP++;}
			trch(ch);PP++;
			if(MCNT>OCNT)
				{DBUF[BPOS]=OUTBUF2[BPOS];
				DBUF[BPOS-1]=OUTBUF2[BPOS-1];
				if(BPOS>DPOS) DPOS=BPOS;
				}
			OUTBUF2[BPOS++]=' ';
			if(UF&&FIRST)UBUF[UCOL=CP]=UCHAR;
			if(XF&&FIRST)XBUF[XCOL=CP]=XCHAR;
			BLKCNT--; CP++;
			} break;
	case	CONTROL:/*decode on following letter*/
			ch=OUTBUF2[++BPOS];
			if(CPTR[ch-' ']) pcont(ch);
			else switch(ch)
			{case 'h':
			case 'H':/*backspace*/
				if(CP)CP--;break;
			case '+': l--; break;
			case '-': l++; break;
			case 'U': UF=TRUE;break;
			case 'u': UF=FALSE;break;
			case 'X': XF=TRUE;break;
			case 'x': XF=FALSE;break;
			case 'B': MCNT *=3;break;
			case 'b': if((MCNT /=3)==0)MCNT=1;
					break;
			case 'D': MCNT *=2;break;
			case 'd': if((MCNT /=2)==0)MCNT=1;
					break;
			case '(': p1=CP;break;
			case ')': CP=p1;break;
			case '[': p2=CP;break;
			case ']': CP=p2;break;
			case '{': p3=CP;break;
			case '}': CP=p3;break;
			default:/*?,ignore*/ break;
		       } BPOS++; break;
	case SENTINEL:
	case SOH:
	case HTAB:
	case OTHERS:
       fprintf(stderr,"\nweird character value: %o\n",ch);
		BPOS++;
		break;
	}}
if(!updat){/*restore original values*/
	XF=xfs;
	UF=ufs;
	MCNT=mcnts;
	}
}
/**************************************************/
int class(char c)
{if(c==TFVAL[0]) return(TRANSLATE);
if (c==CFVAL[0]) return(CONTROL);
if((unsigned char)c>' ') return(BLACK);
if(c==' ') return(WHITE);
if(c=='\n') return(SENTINEL);
if(c=='\r') return(SENTINEL);
if(c==TAB) return(HTAB);
if(c=='\001') return(SOH);
if(!c) return(SENTINEL);
return(OTHERS);
}
/**************************************************/
void fraction()	/*put printer in fractional spcing mode;
			set FRQ*/
{if(!FRQ && FRSTRING && (FRVAL!=1))
	{outstr(FRSTRING);
	FRQ = TRUE;
	}
}
/**************************************************/
void whole()		/*put printer in whole line spacing;
			reset FRQ */
{if(FRQ && WHSTRING)
	{outstr(WHSTRING);
	FRQ = FALSE;
	}
}
/**************************************************/
void trch(char c)	/*output string translation of c*/
{char *p;
	if(c<' ') {cputc(TFVAL[0],ofp);cputc(c,ofp);return;}
	p = TPTR[c-' '];
	if(p) outstr(p);
	else	{/*not initialized*/
		cputc(TFVAL[0],ofp);
		cputc('?',ofp);
		}
}
/****************************************/
void pcont(char c) /*output printer control string for c*/
{char *p;
	if(c<' ') {cputc(CFVAL[0],ofp);cputc(c,ofp);return;}
	p = CPTR[c-' '];
	if(p) outstr(p);
	else	{/*not initialized*/
		cputc(CFVAL[0],ofp);
		cputc('?',ofp);
		}
}
/********************************************************/
void cputc(char c, FILE *fp)
{
	if (! SUPPRESS)
	{ if (c!=CNTLZ) putc(c,fp);
	  else putc(CZSUB,fp);
	}
}
