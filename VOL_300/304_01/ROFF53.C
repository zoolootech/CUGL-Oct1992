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
#include <string.h>
#if (VERSION!=2) || (subVERSION!=00)
#error ***************This is version 2.00******************
#endif
/**************************/
void exitm(char *msg)
{fprintf(stderr,msg);
 exit(1);
}
/***************************************************/
/* regist() returns pointer to integer variable.   */
/* It searches RLIST; creates new entry if needed; */
/***************************************************/
int *regist(char *s)
{ENTRY *pe;
int *pw;
FORMAT *pf;
char *pc,*s2;
unsigned siz; ENTRY *where;
char buff[64];
pc=find2(s,&RLINK);
if(pc != NULL) return((int *)pc); /*else create new entry*/
pe=(ENTRY *)buff;
pe->link=RLINK.link;
pc=(char *)&(pe->ident);
s2=s;
transfer(&s2,&pc,'\0');
pw=(int *)pc;
*pw=REGDEF;
siz = (int)pw - (int)buff + sizeof(int) + sizeof(FORMAT);
if (NULL==(where=(ENTRY *)malloc(siz))) exitm("regist(): cannot malloc\n");
RLINK.link = (ENTRY *)memcpy(where,buff,siz);
pc = (char *)RLINK.link + siz;
pf = (FORMAT *)pc - 1;
pw = (int *)pf - 1;
/* writing default FORMAT */
pf->fill = ' ';
pf->typ  = ' ';
pf->minwidth=1;
return(pw);
}
/****************************************/
void dovar()	/*process .NR request*/
{char typ, wbuf[MAXLINE], *lptr;
int val, *pw;
gwLIN(wbuf);
skip_blanks();
val=get_val2(LIN,&typ,&lptr);
gwLIN(wbuf);
if(wbuf[0]==NUMSIGN && wbuf[1]=='\0')
	{set(&NEWPAG,val,typ,NEWPAG,1,30000);
	}
else	{pw=regist(wbuf);
	set(pw,val,typ,REGDEF,0,30000);
	}
}
/****************************************/
void source()
{char name[FNAMSIZ];
gwLIN(name);
if(gwLIN(name)!=WE_HAVE_A_WORD)
	{fprintf(stderr,".SO has no name\n");
	return;
	}
strupr(name);
if(FPTR<FMAX)
	{FSTACK[FPTR].IFP=ifp;
	 FSTACK[FPTR].nlines=INPUTlns;
	 INPUTlns=0;
	 /* need to save pbbuf ptr*/
	 FSTACK[FPTR].binp=BINP;
	 FSTACK[FPTR].backbuf=BACKBUF;
	 FSTACK[FPTR].bbsiz=BBSIZ;
	 BBSIZ-=BINP;
	 BACKBUF+=BINP;
	 BINP=0;
	 strcpy(FSTACK[++FPTR].FNAME,name);
	}
else exitm("source(): FSTACK overflow\n");
if( (ifp=fopen(name,"r")) != ERROR ) return;
	/*else*/
	fprintf(stderr,"Can't open <%s>\n",name);
endso();
}
/****************************************/
void showr()	/*lists register names and contents*/
{int *pr;
FORMAT *pf;
ENTRY *pe;
char *pc;
fprintf(stderr,"REGISTERS and <values> and [format]:\n");
pf = &PAGFMT;
fprintf(stderr,"#   <%d>  [%c%c %d]\n",
		NEWPAG,pf->fill,pf->typ,pf->minwidth);
pe=RLINK.link;
while(pe)
	{pc=(char *)&(pe->ident);
	fprintf(stderr,"%s ",pc);
	pr=(int *)(pc+1+strlen(pc));
	pf=(FORMAT *)(pr+1);
	fprintf(stderr,"<%d>  [%c%c %d]\n",
			*pr,pf->fill,pf->typ,pf->minwidth);
	pe = pe->link;
	}
dashes();
}
/****************************************/
void assignfmt()
{char *pc, wbuf[MAXLINE];
 int *pw;
 gwLIN(wbuf);
 skip_blanks();
 gwLIN(wbuf);
 skip_blanks();
 if(LIN[0]=='"') pc = strtok(&LIN[1],"\"");
 else pc = strtok(&LIN[0]," \t\n");
 if (wbuf[0]==NUMSIGN && wbuf[1]=='\0') picfmt(pc,&PAGFMT);
 else	{pw = regist(wbuf);
 	 picfmt(pc,(FORMAT *)(pw+1));
 	}
}
/********************************************/
/* picfmt() converts the string it receives */
/* into a FORMAT describing the min width   */
/* and format type and fill character;  For */
/* example, ".......1" would be in arabic   */
/* with width at least 8, left filled with  */
/* '.';  "i" whould be lowercase roman at   */
/* least one character wide; "   A" would   */
/* be lettered: 0,A,B..,Z,AA,AB,..AZ,BA,    */
/* right justified in field of width 4      */
/********************************************/
FORMAT *picfmt(char *string, FORMAT *pf)
{if ((*string) < ' ') return (NULL);
 pf->minwidth = strlen(string);
 pf->typ = *strpbrk(string,"AaIi");
 if (pf->typ == '\0') pf->typ = '1';
 pf->fill = *string;
 return ( pf ) ;
}
/****************************************/
void endso()	/*called upon EOF to return to previous input file*/
{if(FPTR)
	{close(ifp);
	ifp=FSTACK[--FPTR].IFP;
	INPUTlns=FSTACK[FPTR].nlines;
	/*need to handle pbbuf*/
	BBSIZ=FSTACK[FPTR].bbsiz;
	BACKBUF=FSTACK[FPTR].backbuf;
	BINP=FSTACK[FPTR].binp;
	LIN=fgets3();
	}
else BINP=0;
}
/****************************************/
void dashes()
{fprintf(stderr,"-----------\n");
}
/****************************************/
void ignore()	/*handle .ig request*/
{int i; char C1, C2;  ARGs a;
 i = process(&a);
 if (i==0) return; /*no arg so ignore current line*/
 C1 = a.arg[1][0]; C2 = a.arg[1][1];
 while(fgets3())	/*until EOF or MSDOSEOF*/
	if(*LIN==COMMAND)
	if(LIN[1]==C1)
	if(!C2 || (LIN[2]==C2) )  return; /*found match to 1st 2 letters*/
}
/************************************/
/* detab() checks OUTBUF2 for tabs, */
/* soh; it not found does nothing.  */
/* else it replaces tabs with .tc   */
/* and it replaces ^a,^A with .lc   */
/************************************/
void detab()
{char ch;
 int p1,p2,p3;
 int BP3;
 char OUTBUF3[LSZ]; /* temp needed to expand tabs, etc.*/
 /*prescan for tabs, soh to save time, effort*/
 int tabs;
 tabs=CP=BPOS=0;
 while('\0'!=(ch=OUTBUF2[BPOS]))
  {if(ch=='\t')   tabs=YES;
   if(ch=='\001') tabs=YES;
   if(ch==CFVAL[0])
	{ch=OUTBUF2[BPOS+1];
	 if((ch=='t')||(ch=='T'))
		{tabs=YES; ch='\t'; BPOS++;}
	 else if((ch=='a')||(ch=='A'))
		{tabs=YES; ch='\001'; BPOS++;}
	 else ch=OUTBUF2[BPOS];
	}
   OUTBUF2[CP++]=ch;
   BPOS++;
  }
 OUTBUF2[CP]='\0';
 if(!tabs) return;
 /* below is the processing needed to expand tabs, soh */
 BP3=CP=BPOS=0;
 while('\0'!=(ch=OUTBUF2[BPOS]))
  {switch(class(ch))
   {case TRANSLATE: OUTBUF3[BP3++]=ch;OUTBUF3[BP3++]=OUTBUF2[++BPOS];
		    CP++; break;
    case BLACK:
    case WHITE:     OUTBUF3[BP3++]=ch;
		    CP++; break;
    case CONTROL:   OUTBUF3[BP3++]=ch;ch=OUTBUF2[++BPOS];
		    switch(ch)
		    {case 'h':
		     case 'H': if (CP) CP--;OUTBUF3[BP3++]=ch;break;
		     case '(': p1=CP;OUTBUF3[BP3++]=ch;break;
		     case ')': CP=p1;OUTBUF3[BP3++]=ch;break;
		     case '[': p2=CP;OUTBUF3[BP3++]=ch;break;
		     case ']': CP=p2;OUTBUF3[BP3++]=ch;break;
		     case '{': p3=CP;OUTBUF3[BP3++]=ch;break;
		     case '}': CP=p3;OUTBUF3[BP3++]=ch;break;
		     default:/*?,ignore*/;break;
		    } break;
    case HTAB:	   {int cp2,newCP;
		    cp2=CP;
		    do cp2++; while(RULER[cp2]=='-');
		    if (RULER[cp2]=='l') newCP=cp2;
		    else   {char tc,bc,bc2; int W,D;
			    tc=RULER[cp2];
			    D=cp2-CP;W=0;
			    if((tc=='.')||(tc==','))
				while((bc=OUTBUF2[BPOS+1+W])!=0)
				  {if (bc==tc) break;
				   if (bc=='\t') break;
				   if (bc=='\001') break;
				   W++;
				  }
			    else while((bc=OUTBUF2[BPOS+1+W])!=0)
				  {if (bc=='\t') break;
				   if (bc=='\001') break;
				   W++;
				  }
			    /* now W,D are known */
			    if((tc=='r')||(tc=='R')) newCP=CP+D-W+1;
			    else if((tc=='.')||(tc==',')) newCP=CP+D-W ;
			    else if (tc=='c') newCP=CP+D-(W/2);
			   }
		    while(CP<newCP)
			   {OUTBUF3[BP3++]=TCVAL;
			    CP++;
			   }
		   } break;
    case SOH:	   {int cp2,newCP;
		    cp2=CP;
		    do cp2++; while(RULER[cp2]=='-');
		    if (RULER[cp2]=='l') newCP=cp2;
		    else   {char tc,bc,bc2; int W,D;
			    tc=RULER[cp2];
			    D=cp2-CP;W=0;
			    if((tc=='.')||(tc==','))
				while((bc=OUTBUF2[BPOS+1+W])!=0)
				  {if (bc==tc) break;
				   if (bc=='\t') break;
				   if (bc=='\001') break;
				   W++;
				  }
			    else while((bc=OUTBUF2[BPOS+1+W])!=0)
				  {if (bc=='\t') break;
				   if (bc=='\001') break;
				   W++;
				  }
			    /* now W,D are known */
			    if((tc=='r')||(tc=='R')) newCP=CP+D-W+1;
			    else if((tc=='.')||(tc==',')) newCP=CP+D-W ;
			    else if (tc=='c') newCP=CP+D-(W/2);
			   }
		    while(CP<newCP)
			   {OUTBUF3[BP3++]=LCVAL;
			    CP++;
			   }
		   } break;
    case OTHERS:   fprintf(stderr,"\nweird character value: %o\n",ch);
		   break;
   }
   BPOS++;
  }
 OUTBUF3[BP3]='\0';
 strcpy(OUTBUF2,OUTBUF3);
}
/**********************************************/
/* to handle predefined (read only) registers */
/**********************************************/
int readonly(char *wbuf) /*used by fgets3();returns TRUE if predefined*/
{if(wbuf[1]=='\0')	/*length 1*/
	{int pnum;
	 if(*wbuf!=NUMSIGN) return(FALSE);
		{if((VLINENO>=PLVAL[0])
			||(VLINENO<0)) pnum=NEWPAG;
		 else pnum=CURPAG;
		 itoC(pnum,wbuf,PAGFMT);
		 pbstr(wbuf);
		 return(TRUE);
		}
	 }
 else if(wbuf[2]=='\0')	/*length 2*/
	{int reg;
	 if(*wbuf=='d')
		{if(wbuf[1]=='l') reg=DWIDTH;
		 else if(wbuf[1]=='n') reg=DLINES;
		 else return(FALSE);
		}
	 else if(*wbuf=='n')
		{if(wbuf[1]=='l') reg=PLINENO;
		 else return(FALSE);
		}
	 else if(*wbuf=='l')
		{if(wbuf[1]=='n') reg=LN;
		 else return(FALSE);
		}
	 else if(*wbuf=='.')
		switch(wbuf[1])
		{case 'c': reg=INPUTlns; break;
		 case 'd': if(DIVERTING) reg=DLINES;
			   else reg=PLINENO;
			   break;
		 case 'i': reg=INVAL;	break;
		 case 'j': if(JUSTIFY)reg=DOTJ;
			   else reg=0; break;
		 case 'l': reg=RMVAL-INVAL; break;
		 case 'n': exitm("/.n/not yet implemented");
		 case 'o': reg=dotO;	break;
		 case 'p': reg=PLVAL[0]; break;
		 case 't': if(DIVERTING)
			   {reg=-1;
			    if((Dstats->dtname[0]) && (Dstats->dt>0))
				reg=(Dstats->dt)-DLINES;
			    if(reg<0) reg=HUGE;
			   }
			   else reg=dotT(VLINENO);
			   break;
		 case 'u': reg=FILL;	break;
		 case 'x': reg=VERSION; break;
		 case 'y': reg=subVERSION; break;
		 case 'z': pbstr(DNAME); return(TRUE);
		 case 'F': pbstr(FSTACK[FPTR].FNAME); return(TRUE);
		 case 'L': reg=LSVAL[0]; break;
		 default : return(FALSE);
		}
	 else return(FALSE);
	 itoa(reg,wbuf,10);
	 pbstr(wbuf);
	 return(TRUE);
	}
 else if(wbuf[3]=='\0') /*length 3*/
	{int reg;
	 if((wbuf[0]=='.')&&(wbuf[1]=='m'))
	   switch(wbuf[2])
	   {case '1': reg=M1VAL[0]; break;
	    case '2': reg=M2VAL[0]; break;
	    case '3': reg=M3VAL[0]; break;
	    case '4': reg=M4VAL[0]; break;
	    default:  return(FALSE);
	   }
	 else return(FALSE);
	 itoa(reg,wbuf,10);
	 pbstr(wbuf);
	 return(TRUE);
	}
 return(FALSE);
}
