/********************************************************/
/*		 ROFF5, Version 2.00			*/
/*    (c) 1983,4,8,9 by Ernest E. Bergmann		*/
/*	730 Seneca Street, Bethlehem, Pa. 18015		*/
/*							*/
/* Permission is hereby granted for all commercial and	*/
/* non-commercial reproduction and distribution of this	*/
/* material provided this notice is included.		*/
/********************************************************/
#include <conio.h>
#include <roff5.h>
#if (VERSION!=2) || (subVERSION!=00)
#error ***************This is version 2.00******************
#endif
/**************************************/
void doTI(char *line)  /*removes leading space; updates TIVAL*/
{int i, j, white;
 char c;
for (white=TIVAL,i=0; c=line[i],(c==' ')||(c=='\t')||(c=='^'); i++ )
	{if(line[i]==' ') white++;
	 else if (c=='^')
		{c=line[i+1];
		 if ((c=='T')||(c=='t')) {i++; c='\t';}
		 else break;
		}
	 if (c=='\t')
		{j=white+1;
		 while (RULER[j]=='-') j++;
		 if (strchr("lcr.,",RULER[j])) white=j;
		}
	}
if ( line[i] != NEWLINE ) TIVAL = white;
/* move line to left */
for(j=0;(line[j]=line[i])!='\0';j++,i++);
}
/**************************************************
handles case of leading blanks or tabs; empty lines
***************************************************/
void leadbl(char *line)
{if (JUSTIFY && (DOTJ>1))
	{int nextra;
	 nextra=min(RMVAL-TIVAL,MAXLINE-1)-OUTW+1;
	 spread(OUTBUF,nextra,1);
	}
Brk();
doTI(line);
}
/*****************************************
		puts out page header
******************************************/
void phead()
{VLINENO=-1;
setTRAP();
chgenv(0,1);
UF=XF=FALSE;
MCNT=1;
CURPAG = NEWPAG++;
if((CURPAG<FIRSTPAGE)||(CURPAG>LASTPAGE)) SUPPRESS=TRUE;
else SUPPRESS=FALSE;
OLDLN=-1;
if(PAGESTOP)
	{putch(BELL);	/*ring bell*/
	getch();	/*wait til char typed at console*/
	}
VLINENO=FVLINENO=PLINENO=FPLINENO=0;
if (M1VAL[0] > 0 )
      { VLINENO = M1VAL[0]-1;
	if (CURPAG % 2) puttl3 ( OHEAD, OH2, OH3, CURPAG, PAGFMT );
	else puttl3 ( EHEAD, EH2, EH3, CURPAG, PAGFMT );
      }
chgenv(1,0);
VLINENO = M1VAL[0]+M2VAL[0];
}
/**************************************
		puts out page footer
***************************************/
void pfoot()
{chgenv(0,1);
UF=XF=FALSE;
MCNT=1;
VLINENO = FOOTLOC+M3VAL[0]; FVLINENO=0;
if ( M4VAL[0] > 0 )
      { if(CURPAG % 2)puttl3 ( OFOOT,OF2,OF3, CURPAG, PAGFMT );
	else puttl3(EFOOT,EF2,EF3,CURPAG,PAGFMT);
      }
VLINENO = PLVAL[0];FVLINENO=0;
if (FFEED) cputc(FORMF,ofp);
else padv();
whole();
OLDBOT=PLINENO=FPLINENO=0;/*printer at start of newpage*/
OLDLN=-1;
chgenv(1,0);
}
void pbcmd(char *xx,int num) /*pbstr(".xx <num>\n") */
{char cbuf[10];	/*if num<0, don't do <num> */
 pbstr("\n");
 if(num>=0)
	{pbstr((char *)itoa(num,cbuf,9));
	 putback(' ');
	}
 pbstr(xx);
 putback('.');
}
/*******************************************
undoes stuff in progress during a page trap
********************************************/
void retract()
{int ti;
 ti=TIVAL;
 if (*(LIN-1)==COMMAND) LIN--;
 else if(FILL) doTI(LIN);
 if (GPWBUF[0])
	{pbstr(LIN);
	 LIN=LINE;
	 LINE[0]='\0';
	 OUTPOS=0;
	 Brk();
	 putback(' ');
	 if(SENTENCE) putback(' ');
	 pbstr(GPWBUF);
	 GPWBUF[0]='\0';
	}
 else if((FILL)&&!CEVAL)
	{pbstr(LIN);
	 LIN=LINE;
	 LINE[0]='\0';
	 OUTPOS=0;
	}
 pbcmd("ti",ti);
}
/**********************************************
	space n lines or to bottom of the page or to trap
***********************************************/
void space(int n)
{int destination;
 if(DIVERTING)
    {for(;n>0;n--)
	{divert("\n");
	 if((++DLINES)==Dstats->dt)
	    {char *pc;
		 pc=macq1(Dstats->dtname);
		 if(pc)
		    {retract();
		     pbstr(pc);
		     return;
		    }
	    }
	}
     return;
    }
/* else not DIVERTING:*/
 if (VLINENO >= FOOTLOC) return;
 if (VLINENO<0) phead();	/* top of page */
 VLINENO+=n;
 if (VLINENO>FOOTLOC) destination=VLINENO=FOOTLOC;
 else destination=VLINENO;
 if ((VLINENO >= TRAPLOC)&&(TRAPLOC>(M1VAL[0]+M2VAL[0]))) VLINENO=TRAPLOC;
 if (VLINENO>=FOOTLOC) pfoot(); /* print footer if bottom */
 else if (VLINENO >= TRAPLOC)
	{char *pc;
	 /*execute TRAP:*/
	 pc=macq1(TRAPNAM);
	 if(pc)
		{retract();
		 pbcmd("go",destination);
		 pbstr(pc);
		}
	 setTRAP();
	}
}
/*******************************************************/
    char GPWBUF[LSZ];
void text()
{int i, j, k;
char *p1, *p2;
if(INTRAP) if(--INTRAP==0)
		{p1=macq1(ITstring);
		 if(p1) pbstr(p1);
		}
if ((FILL) && (LIN[0] == BLANK || LIN[0]==NEWLINE || LIN[0] == TAB
	||((LIN[0]=='^')&&((LIN[1]=='T')||(LIN[1]=='t'))) ))
	leadbl (LIN);
if (CEVAL > 0)
      { center(LIN);
	put(LIN);
	CEVAL--;
      }
else if(LIN[0]==NEWLINE)
	{
	  Brk(); space(LSVAL[0]);
	}
else if(!FILL) put(LIN);
else while (WE_HAVE_A_WORD == gwLIN(GPWBUF))
	putwrd (GPWBUF);
}
/******************************************************
	put out a line of text with correct indentation
	underlining if specified;  starts new page if
	line does not fit on current page.  if DIVERTING
	it diverts line and advances the line spacing val.
*******************************************************/
void put(char *line)
{int i,fs,minfs;
 int top,bot;
 if(!DIVERTING)
   {if ((VLINENO < 0)||(VLINENO >= FOOTLOC)) phead();
    fs=(VLINENO-OLDLN)*FRVAL;
    minfs=OLDBOT-OUTTOP; if(!(OLDBOT&&OUTTOP)) minfs++;
    while(fs<minfs) {fs+=FRVAL; VLINENO++;}
    excurs(line,&top,&bot);
    if ((VLINENO + (bot-top)/FRVAL)>=FOOTLOC) {pfoot(); phead();}
   }
putline(line);
TIVAL = INVAL;
space(LSVAL[0]);
}
/***********************************************************
concatenates GPWBUF onto the end of OUTBUF for filled text
************************************************************/
void putwrd()
{int i, j, k, trapped;
char s[MAXLINE], ch;
int line_len, new_out_width, wid;
int nextra;
trapped=FALSE;
trunc_bl (GPWBUF);
wid =SENTENCE+strln4(GPWBUF,TRUE,-1,PAGFMT);/*sets WTOP,WBOT*/
line_len = RMVAL - TIVAL;
new_out_width = OUTW+wid;
if (new_out_width > min (line_len, MAXLINE-1))
      { nextra = min(line_len, MAXLINE-1)-OUTW+1;
	if(OUTBUF[OUTPOS-2]==BLANK) nextra++;
	if(JUSTIFY) spread(OUTBUF,nextra,OUTWRDS);
	Brk();
	if(GPWBUF[0]=='\0') trapped=TRUE;
      }
if(trapped)
   {OUTTOP=0;
    OUTBOT=0;
    OUTWRDS=0;
   }
else
   {OUTW += wid+1;
    OUTcat(GPWBUF);
    *GPWBUF= '\0'; /*marked empty after transfer*/
    if(*OUTBUF) {OUTSP(); if(SENTENCE) OUTSP();}
    OUTWRDS++;
    if(WTOP<OUTTOP) OUTTOP=WTOP;
    if(WBOT>OUTBOT) OUTBOT=WBOT;
   }
}
/**********************************************************
 offset() will insert leading blanks into a string
***********************************************************/
void offset(char *s, int i)
{char *p,*q;
 if (i)
 {for(p=s; *p ; p++);
  for(q=p+i; p>=s; *q--=*p--);
  for( ; q>=s; *q--=BLANK);
 }
}
/**********************************************************
  a new putline routine; sends line OF TEXT to  OUTPUT2 or diverts
***********************************************************/
void putline(char *line)
{char c;
blankb(TIVAL);
for(;'\0'!=(c=*line);line++) putout(c);
putout('\0');
detab();
putlinout(TRUE);
}
/****************************************/
void OUTcat(char *str)	/*appends string to OUTBUF*/
{while('\0'!=(OUTBUF[OUTPOS]=*(str++)))
	OUTPOS++;
}
/****************************************/
void OUTSP()		/*appends BLANK to OUTBUF*/
{OUTBUF[OUTPOS++]=BLANK;
OUTBUF[OUTPOS]='\0';
}
/****************************************/
void gloss()	/*prints on STDOUT a glossary of .tr chars*/
{int i;
char line[20],tcs,TCS;
put("GLOSSARY:");
put("USE     <GET>");
tcs=TFVAL[0];
TCS = 128 | tcs; /*set most significant bit*/
TFVAL[0] = TCS; /*temporary change of character for class()*/
for(i=1;i<19;i++) line[i]=' ';
line[0]=tcs;
line[8]='<';
line[9]=TCS;
line[11]='>';
line[12]='\0';
for(i=' ';i<127;i++)
	{if(TPTR[i-' '])
		{line[1]=line[10]=i;
		put(line);
		}
	}
TFVAL[0]=tcs; /*restore original*/
Brk();
space(HUGE);
}
/******** ENVIROMENT SWITCHING: ********/
int UFs[NUMENV];
int XFs[NUMENV];
int MCNTs[NUMENV];
int FILLs[NUMENV];
int JUSTIFYs[NUMENV];
int TIVALs[NUMENV];
int CEVALs[NUMENV];
char RULERs[NUMENV][LSZ];
int SENTENCEs[NUMENV];
int LSVALs[NUMENV][STKSIZ+1];
int INVALs[NUMENV];
int RMVALs[NUMENV];

int OUTPOSs[NUMENV];
char OUTBUFs[NUMENV][LSZ];
char GPWBUFs[NUMENV][LSZ];
int OUTWs[NUMENV];
int OUTWRDSs[NUMENV];
int OUTTOPs[NUMENV];
int OUTBOTs[NUMENV];
char OUTBUF2s[NUMENV][LSZ];
int BPOSs[NUMENV];
/***************************************************/
/* Saves current values of state variables to old. */
/* Restores values of state variables from new.    */
/* If old==-1 does restores only.		   */
/* If new==-1 does save only.			   */
/***************************************************/
void chgenv(int old, int new)
{
 if ((old<-1)||(old>=NUMENV)) return /*error*/;
 if ((new<-1)||(new>=NUMENV)) return /*error*/;
 if (old!=-1) /*saves*/
    {int i;
     MCNTs[old]=MCNT;
     XFs[old]=XF;
     UFs[old]=UF;
     FILLs[old]=FILL;
     JUSTIFYs[old]=JUSTIFY;
     TIVALs[old]=TIVAL;
     CEVALs[old]=CEVAL;
     for(i=0;i<LSZ;i++)
	{RULERs[old][i]=RULER[i];
	 OUTBUFs[old][i]=OUTBUF[i];
	 GPWBUFs[old][i]=GPWBUF[i];
	 OUTBUF2s[old][i]=OUTBUF2[i];
	}
     OUTPOSs[old]=OUTPOS;
     OUTWs[old]=OUTW;
     OUTWRDSs[old]=OUTWRDS;
     OUTTOPs[old]=OUTTOP;
     OUTBOTs[old]=OUTBOT;
     BPOSs[old]=BPOS;
     SENTENCEs[old]=SENTENCE;
     for(i=0;i<=STKSIZ;i++) LSVALs[old][i]=LSVAL[i];
     INVALs[old]=INVAL;
     RMVALs[old]=RMVAL;
    }
 if (new!=-1) /*restores*/
    {int i;
     MCNT=MCNTs[new];
     XF=XFs[new];
     UF=UFs[new];
     FILL=FILLs[new];
     JUSTIFY=JUSTIFYs[new];
     TIVAL=TIVALs[new];
     CEVAL=CEVALs[new];
     for(i=0;i<LSZ;i++)
	{RULER[i]=RULERs[new][i];
	 OUTBUF[i]=OUTBUFs[new][i];
	 GPWBUF[i]=GPWBUFs[new][i];
	 OUTBUF2[i]=OUTBUF2s[new][i];
	}
     OUTPOS=OUTPOSs[new];
     OUTW=OUTWs[new];
     OUTWRDS=OUTWRDSs[new];
     OUTTOP=OUTTOPs[new];
     OUTBOT=OUTBOTs[new];
     BPOS=BPOSs[new];
     SENTENCE=SENTENCEs[new];
     for(i=0;i<=STKSIZ;i++) LSVAL[i]=LSVALs[new][i];
     INVAL=INVALs[new];
     RMVAL=RMVALs[new];
    }
}
/********************************/
/* selects printer or diversion */
/********************************/
void putlinout(int txtline)
{if (DIVERTING)
	{divert(OUTBUF2);
	 OUTBUF2[0]=BPOS=0;
	}
 else 	{offset(OUTBUF2, dotO);
	 printout(txtline);
	}
}
