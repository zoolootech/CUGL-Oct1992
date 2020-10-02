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
#include <mem.h>
#include <ctype.h>
#if (VERSION!=2) || (subVERSION!=00)
#error ***************This is version 2.00******************
#endif
ENTRY	SLINK, /* string substitution table */ /*insert(), showit()*/
	MLINK, /* macro and diversion substitution table */
		/*minsert(),showm(),macq1()*/
		/*SLINK,MLINK table entries are following ident string
			with second string used as replacement*/
	RLINK; /* register substitution table;
		   ident string followed by int*/ /*showr(),regist()*/
/* typical addition of new entries consists of:
	entry *pe;
	char  *pc;
	char buff[?];
	unsigned siz;
	ENTRY *where;
	pe = (ENTRY *)buff;
	pe->link = ?LINK.link;
	pc = &pe.ident;
	pc++ = stpcpy(pc, NAME);
        Now pc points to where info should be placed;
	Compute actual siz;
	if (NULL==(where=(ENTRY *)malloc(siz)))
		exitm("ERROR MESSAGE cannot malloc\n");
	?LINK.link = (ENTRY *)memcpy(where,buff,siz);
*/
int INPUTlns, INTRAP;
char ITstring[LSZ];
char EMstring[LSZ];
int PAGESTOP=FALSE;
int JUSTIFY=JU_INI;
int DOTJ = 1; /*both margins*/
int FFEED=FF_INI;
int FIRSTPAGE=1,LASTPAGE=30000;/*for selectively printing output*/
int SUPPRESS;/*if true,no output is passed by putchar()*/

int FILL=YES;
int LSVAL[STKSIZ+1];	/* line spacing value -> default will be 1 */
int TIVAL=TI_DEF;	/* temporary indent -> default  0 */
int INVAL=0;	/* left indent -> default  0 */
int RMVAL=65;	/* right margin -> default  PAGEWIDTH */
int CEVAL=0;	/* set equal to number of lines to be centered	*/
int TFVAL[STKSIZ+1]; /*translation flag char*/
		/* default is 0                                   */
int SPVAL;	/* blank lines to be spaced down */
int CURPAG=0;	/* current output page number */
int NEWPAG=1;	/* next output page number */
FORMAT PAGFMT = {' '/*typ*/, ' '/*fill*/, 1/*minwidth*/};
int VLINENO=-1;	/* virtual (intended) line advances on page,
			see vadv()*/
int FVLINENO=0;	/* + line fraction */
int PLINENO=0;	/* printer's actual line advances on page,
			see padv()*/
int FPLINENO=0;	/* + line fraction */
int PLVAL[STKSIZ+1]; /* page length in lines */
int M1VAL[STKSIZ+1]; /* margin before& including header in lines*/
int M2VAL[STKSIZ+1]; /* margin after header in lines*/
int M3VAL[STKSIZ+1]; /* margin after last text line in lines*/
int M4VAL[STKSIZ+1]; /* bottom margin, including footer in lines*/
int TRAPLOC;	/* location of next page trap in lines */
int FOOTLOC;	/* location of page footer */
int SCVAL[STKSIZ+1]; /* space character*/
int SENTENCE;	/* Nov 20*/
int CFVAL[STKSIZ+1]; /* control flag character value*/
int ICVAL[STKSIZ+1]; /*insert character*/
int EVVAL[STKSIZ+1]; /*environments*/

char LINE[ LSZ ];	/*input line buffer */
char *LIN = &LINE[0];	/*variable start of LINE*/
char EHEAD[ LSZ ],*EH2=EHEAD,*EH3=EHEAD;	/* even header title */
char OHEAD[ LSZ ],*OH2=OHEAD,*OH3=OHEAD;	/* odd header title */
char EFOOT[ LSZ ],*EF2=EFOOT,*EF3=EFOOT;	/* even footer title */
char OFOOT[ LSZ ],*OF2=OFOOT,*OF3=OFOOT;	/* even footer title */
int CZSUB=CZSUB_DEF;				/* ^Z substitute char */
char RULER[ LSZ ];				/* cf WS to store tabs*/
int LCVAL='.';
int TCVAL=BLANK;
FILE *ifp, *ofp;
/*following added for buffered and formatted output:  */
char OUTBUF2[LSZ];	/*for line to be output in fancy fmt*/
int BPOS=0,CP,PP;		/*buffer,column,printer positions*/
char XBUF[LSZ];		/*strikout buffer*/
int XF=FALSE,XCOL;		/* " flag and column */
char UBUF[LSZ];		/*underline buffer*/
int UF=FALSE,UCOL;		/* " flag and column */
int FIRST;			/*flag for first pass*/
char DBUF[LSZ];		/*double strike buffer*/
int dotO= 0;		/* .o page offset */
int DPOS=-1;
int OCNT;
int MCNT=1;
int BLKCNT;
int DIR=0;			/* for "spreading" of lines    */
int OUTWRDS=0;			/* no. words in OUTBUF */
char OUTBUF[ LSZ ];		/*lines to be filled collected here */
int OUTW=0;			/*current display width of OUTBUF*/
int OUTPOS=0;			/* =strlen(OUTBUF) */
int WTOP,LTOP,OUTTOP=0;		/*zero|negative;levels of subscripts*/
int WBOT,LBOT,OUTBOT=0;		/*zero|positive;levels of subscripts*/
int OLDLN=0;				/*position of previous main line*/
int OLDBOT=0;				/*OUTBOT for previous line;reset
							each page*/
int FRQ=0,FRVAL=1;		/* fractional line?, what fraction size*/
char *FRSTRING = 0; 		/* ^ to code for fractional spacing*/
char *WHSTRING = 0;			/* ^ to code for whole line spacing*/
char *CPTR[128-' '];	/*pointer table for print control*/
char *TPTR[128-' '];	/*pointer table for translation strings
			  				for char; initialize to null */
char BACKBUF0[BBSIZ0];
int BBSIZ=BBSIZ0;
char *BACKBUF=BACKBUF0;
int BINP;		/*position in above;init to 0*/
char KEYBD=FALSE;	/*boolean & prompt for keyboard input*/
char KLINE[MAXLINE];	/*keyboard line input buffer*/
char *KPTR=KLINE;	/*pointer for above*/

int	FPTR=0;
FILEF FSTACK[FMAX];
int XF2,UF2,MCNT2;
int DLINES=0;
int DIVERTING=FALSE;
int NUMBERING=FALSE;
int NNVAL=0;
int LN=0;
int LNMOD=1;
int LNI=0;
int LNST=1;

/******************/
/*  MAIN - ROFF5  */
/******************/
void main(int argc,char **argv)
{char option,*pc;
char filename[FNAMSIZ];
int i;

fprintf(stderr,"ROFF5 v2.00; MS-DOS, TurboC %s\n",__DATE__);
fprintf(stderr,"         (c) 1983,4,8,9 by\n");
fprintf(stderr,"E. E. Bergmann, 730 Seneca Street\n");
fprintf(stderr,"      Bethlehem, PA 18015\n\n");
ofp=stdout;
SUPPRESS=FALSE;
if (argc == 1) exitm("USAGE:  ROFF5 file1\n  more than one file OK\n");
for(i=1;i<argc;i++){
	strcpy (filename,argv[i]);
	if(filename[0]==')') strcpy(filename,"}prn");
     if(filename[0]=='}'){
     	fprintf(stderr,"Opening output file <%s>\n",&filename[1]);
		if(( ofp = fopen(&filename[1],"wb")) == ERROR){
		  fprintf(stderr,"Error on opening <%s>\n",&filename[1]);
                  exit();
          	}
	break;
     }
}

argv++;
init_defaults();
INTRAP=0;
EMstring[0]=ITstring[0]='\0';
while ( --argc > 0 )
      { strcpy (filename, *argv++);
	if(*filename=='>')goto SKIPIT;
	if(*filename=='}')goto SKIPIT;
	if(*filename==')')goto SKIPIT;
	fprintf(stderr,"Now processing <%s>\n",filename );
	if(filename[0]=='-')
		{option=toupper(filename[1]);
		if(option=='M') showm(0);
		else if(option=='F') putc(FORMF,ofp);
		else if(option=='G') gloss();
		else if(option=='I') showit();
		else if(option=='R') showr();
		else if(option=='S') PAGESTOP=TRUE;
		else if(option=='O') range(&filename[2]);
		else if(option=='N')
		{Brk();
		 NEWPAG=atoi(&filename[2]);
		 if (NEWPAG<1) NEWPAG=1;
		 if (NEWPAG>9999) NEWPAG=9999;
		 CURPAG=NEWPAG-1;
		}
		else	{KEYBD=option;
			dolns();
			fprintf(stderr,"End of keyboard input <%c>\n",
				KEYBD);
			KEYBD=FALSE;
			}
SKIPIT: 		continue;
		}
	strcpy(FSTACK[0].FNAME,filename);
	if (ERROR == (ifp=fopen(filename,"r")))
		{fprintf(stderr,"can't open <%s>\n",filename );
		continue;
		}
	else dolns();
	if(VLINENO>0||OUTBUF[0])
		{ if (FILL && JUSTIFY && (DOTJ>1))
			{int nextra;
			nextra=min(RMVAL-TIVAL,MAXLINE-1)-OUTW+1;
			if (OUTBUF[strlen(OUTBUF)-2]==BLANK) nextra++;
			spread(OUTBUF,nextra,1);
			}
		Brk();
		}
	fprintf(stderr,"Done processing <%s>\n", FSTACK[0].FNAME );
	close(ifp);
}
if(EMstring[0])
	{pc=macq1(EMstring);
	 if(pc) pbstr(pc);
	 dolns();
	}
while(BINP)
	dolns();
while ((BINP)||(VLINENO<FOOTLOC))
	{Brk();
	 dolns();
	 space(HUGE);
	}
close(ofp);
}/* end main()	       */

/**************************************************************
initializes the global variables governing the execution of the
 format commands.
**************************************************************/
void init_defaults()
{int i;
initsk(LSVAL,LS_DEF);	/* line spacing = 1 */
initsk(PLVAL,PL_DEF);
initsk(M1VAL,M1_DEF);
initsk(M2VAL,M2_DEF);
initsk(M3VAL,M3_DEF);
initsk(M4VAL,M4_DEF);
initsk(SCVAL,SC_INI);
initsk(TFVAL,TF_DEF);
initsk(CFVAL,CF_DEF);
initsk(ICVAL,IC_DEF);
initsk(EVVAL,EV_INI);
initxu();
redoRR();
for(i=0;i<NUMENV;i++) chgenv(i,-1);	/* all environments */
setTRAP();
OUTBUF [0] = '\0';
*EHEAD = *OHEAD = '\0' ;
*EFOOT = *OFOOT = '\0' ;
setmem(CPTR,2*(128-' '),0);
setmem(TPTR,2*(128-' '),0);
OUTBUF2[0]=0;
setmem(&DBUF,LSZ,FALSE);
*KLINE=0;
SLINK.link=NULL;
MLINK.link=NULL;
RLINK.link=NULL;
DIVERTING=FALSE;
DLINES=0;
}
/*****************************/
void range(char *s)
{int num;
num=0;
while(isdigit(*s)) num=num*10+(*(s++)-'0');
if(num) FIRSTPAGE=num;
if(*s=='-')
	{s++; num=0;
	while(isdigit(*s)) num=num*10+(*(s++)-'0');
	if(num) LASTPAGE=num;
	}
else	LASTPAGE=FIRSTPAGE;
}
/************************************/
void redoRR()
{int i=0,rm=65,ts=5;
 RULER[i++] = 'L';
 for(;i<rm;i++) if (i % ts) RULER[i]='-'; else RULER[i]='l';
 RULER[i++] = 'R';
 for ( ; i<LSZ; RULER[i++]='\0');
 verifyRR(RULER);
}
/************************************/
int verifyRR(char *rr)
{int s,i,l,t;
 char c;
 s=strlen(rr)-1;
 if (rr[s]!='R') return(FALSE);
 for(l=0;rr[l]==' ';l++);
 if (rr[l]!='L') return(FALSE);
 t=l;
 for(i=l+1; i<s; i++)
	{c=rr[i];
	 if (c=='-') continue;
	 if (strchr("lcr.,",c))
		{if (t==l) t=i;
		 continue;
		}
	 else break;
	}
 if (i!=s) return(FALSE);
/* o.k. so update  */
 INVAL=l;
 RMVAL=s+1;
 strcpy(RULER, rr);
 for (i=RMVAL; i<LSZ; RULER[i++]='\0');
 return(TRUE);
}
