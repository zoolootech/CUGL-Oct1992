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
/************************************************/
/* insert() takes the .DS command line in LINE	*/
/* and uses malloc() to make an ENTRY in SLINK	*/
/************************************************/
void insert()
{ARGs a; int i;
ENTRY *pe;
char *pc,*pc2,*fnd;
unsigned siz; ENTRY *where;
char buff[128];
pe=(ENTRY *)buff;
pe->link=SLINK.link;
pc=(char *)&(pe->ident);
i = process(&a);
if (i<1) {fprintf(stderr,".ds has no name\n");
	  LIN=LINE; LINE[0]='\0';
	  return;
	 }

pc2 = a.arg[1];
transfer(&pc2,&pc,'\0');
pc2 = a.arg[2];
transfer(&pc2,&pc,'\0');
fnd=find2(pe->ident,&SLINK);
if(fnd)
  {fprintf(stderr,"%cWarning: <%s> was defined to be <%s>\n",
		BELL, pe->ident ,fnd);
   fprintf(stderr,"...now it is defined to be <%s>\n",a.arg[2]);
  }
siz = pc - buff;
if (NULL==(where=(ENTRY *)malloc(siz))) exitm("insert(): cannot malloc\n");
SLINK.link = (ENTRY *)memcpy(where,buff,siz);
}
/****************************************/
void showit()	/* displays the list of entries in the string
		substitution table pointed to by SLINK. */
{ENTRY *pe;
char *pc;
fprintf(stderr,"ENTRIES:\n");
pe=SLINK.link;
while(pe)
	{fprintf(stderr,"%u: ",(unsigned)pe);
	pc=(char *)&(pe->ident);
	fprintf(stderr,"<%s>",pc);
	pc += strlen(pc); pc++;
	if(*pc) fprintf(stderr," <%s>\n",pc);
	else fprintf(stderr," <>\n"); /*'C' bug*/
	pe=pe->link;
	}
dashes();
}
/****************************************/
void putback(char c)	/*cf K & P, p256*/
{if(++BINP >= BBSIZ) exitm("Too many characters pushed back\n");
BACKBUF[BINP]=c;
}
/*****************************/
/*filters \r followed by \n; */
/* msb if kgetc(),getc()     */
/*****************************/
int ngetc(FILE *iobuf)	/*cf K & P p256*/
{int c;
if(BINP) c=BACKBUF[BINP];
else {if(KEYBD)	 c=kgetc();
      else       c=getc(iobuf);
      if (c=='\n') INPUTlns++;
      if (c== EOF) BACKBUF[BINP=1]=CPMEOF;
      else { c&=0x7f; BACKBUF[BINP=1]=c; }
      }
BINP--;
if (c==CPMEOF) return(EOF);
if (c==EOF)    return(EOF);
if(c=='\r')
	{c=ngetc(iobuf);
	 if(c!='\n') {putback(c); return('\r');	}
	}
return(c);
}
/****************************************/
int kgetc()	/*like getc(),from keyboard, line-buffered*/
{int i;
if(!*KPTR)
	{fprintf(stderr,"%c",KEYBD);
	if((gets(KLINE)) == NULL) return EOF;
	i=strlen(KLINE);
	KLINE[i++]='\n';
	KLINE[i]='\0';
	KPTR=KLINE;
	}
return(*(KPTR++));
}
/****************************************/
void pbstr(char s[LSZ])	/*put back string on input;cf K&P,p257*/
{int i;
for(i=strlen(s);i>0;) putback(s[--i]);
}
/*************************************/
unsigned char PatchName[5]=".\376\376\n";
unsigned char PN[3]="\376\376";
/************************************/
void apatch(char *s) /* appends PatchName-- to string s */
{strcat(s,PatchName);
 PN[0]=PatchName[1];
 PN[1]=PatchName[2];
 PatchName[2]--;
 if (PatchName[2]< (unsigned char)'\200')
	{PatchName[2]='\376';PatchName[1]--;}
}
/****************************************/
void showm(int mode)	/*lists macro definitions*/
/*mode 0 is most complete; mode 1 line counts;*/
{ENTRY *pe;
char *pc,c;
if((mode<0)||(mode>1)) mode=1;
fprintf(stderr,"MACROS DEFINED:\n");
pe=MLINK.link;
while(pe)
	{pc = (char *)&(pe->ident);
	if(*pc=='\177')
	 {DENTRY *pd=(DENTRY *)pe;
	  fprintf(stderr,"[%p diversion]\ dl=%d, dn=%d",(unsigned)pd,/*???*/
				   pd->dl,pd->dn);
	  if((pd->dt)>0) fprintf(stderr,", .dt %d %s",
					pd->dt,pd->dtname);
	  fprintf(stderr,"\n\n");
	 }
	else
	 {fprintf(stderr,"%p  .%s\n",(unsigned)pe,pc);
	  pc +=strlen(pc); pc++;
	  if(mode)
		{int cnt=0;
		 ENTRY *pe2;
		 while(NULL!=(c=*(pc++))) if (c=='\n') cnt++;
		 while(pe2=pe->link,pc=(char *)pe2->ident,
			(unsigned) *pc >'\177')
			{pe=pe2; if(!pe) break;
			 cnt--; pc +=strlen(pc)+1;
			 while(NULL!=(c=*(pc++))) if (c=='\n') cnt++;
			}
		 fprintf(stderr,"%d lines\n",cnt);
		}
	  else fprintf(stderr,"%s\n",pc);
	 }
	  pe = pe->link;
	}
dashes();
}
/****************************************/
char *macq1(char *line)	/*looks up name to see if it is a macro
			definition.  If it is, returns the
			corresponding string, else returns
			FALSE.
			*/
{char c,*pc,wb[LSZ];
pc=wb;
while(class(c=*(line++))==BLACK) *(pc++)=c;
*pc='\0';
return(find2(wb,&MLINK));
}
/****************************************/
char *find2(char *s,ENTRY *LINK)	/*finds or doesn't find s in table
			of substitutions pointed to by link*/
{char *pc;
if(*s=='\0') return(NULL);
while(LINK)
	{if(!strcmp(s,(char *)&(LINK->ident)))
		{pc=(char *)&(LINK->ident);
		pc += strlen(pc);
		pc++;
		return(pc);
		}
	LINK = LINK->link;
	}
return(FALSE);
}
/**********************************************/
ENTRY *find(char *s, ENTRY *LINK)
{if(*s=='\0') return(NULL);
 while (LINK)
	{if(!strcmp(s,(char *)&(LINK->ident))) return(LINK);
	 LINK=LINK->link;
	}
 return(NULL);
}
/****************************************/
ENTRY *mpatch(char *name,ENTRY *LINK) /*returns FALSE or ->patched name*/
{ENTRY *found,*foundmore;
char c,*pc;
found=find(name,LINK);
if(!found) return(FALSE);
for(foundmore=found;foundmore;)
	{foundmore = found->link;
	 c = *(foundmore->ident);
	 if(foundmore &&((unsigned char)c > '\177')) found=found->link;
	 else   foundmore = FALSE;
	}
pc = found->ident;
pc += strlen(pc)+1;
apatch(pc);
return(found);
}
/**********************************************/
/* mappend() takes a .DE or .AM and following */
/* lines and places the information in MLIST. */
/* If .de (or .am with no prior definition),  */
/* then new entry at heast of list.  If .am,  */
/* then places entry after prior defintion.   */
/* Now uses malloc() to get space for entries.*/
/* Modified to have space for append and ends */
/* with .;;				      */
/**********************************************/
#define MBUF 1000	/*used to fix the max size of mappend()'s buffer*/
/*************************************/
void mappend(CMD cmd)
{char c, *pc,*pc2;
ENTRY *pe;
unsigned siz; ENTRY *where, *prev, *next, *found;
char C1=';';
char C2=';';
char buff[MBUF+5];
/*pass over command and following white space*/
for(pc=LIN,c=*pc; (c!=' ')&&(c!='\n')&&(c!='\t'); pc++)
	c=*pc;
for(; (c==' ')||(c=='\t'); pc++) c=*pc;
if(c=='\n') {fprintf(stderr,".de or .am is UNnamed\n");
	     LIN=LINE; LINE[0]='\0';
	     return;
	    }
else	{pe=(ENTRY *)buff;
	pe->link = MLINK.link;
	pc2=(char *)&(pe->ident);
	while(class(c)==BLACK)
		{*(pc2++)=c;
		c=*(pc++);
		}
	}
*(pc2++)='\0';
if (cmd==DE) found=FALSE;
else found=mpatch(pe->ident,&MLINK);
if (found) /*then should use patched name, location*/
   {prev = found;
    pe->link = found->link;
    pc2 = (char *)&(pe->ident);
    *(pc2++)=PN[0]; *(pc2++)=PN[1]; *(pc2++)='\0';
    siz = pc2-buff+4;
   }
else prev=&MLINK;
while(fgets3())	/*until EOF or MSDOSEOF*/
	{pc=LIN;
	 if((*LIN==COMMAND)&&(LIN[1]==C1)&&(LIN[2]==C2)) break;
	 siz += strlen(LIN);
	 if (siz>=MBUF) /* then need to start new append */
		{*(pc2++)='\0';
		 siz = pc2 - buff + 4;
		 apatch(pc2-1);
		 if (NULL==(where=(ENTRY *)malloc(siz)))
			exitm("mappend(): cannot malloc\n");
		 next=prev->link;
		 prev->link = (ENTRY *)memcpy(where,buff,siz);
		 prev = prev->link;
		 siz = sizeof(ENTRY)+3+strlen(LIN);
		 pe->link = next;
		 pc2 = (char *)&(pe->ident);
		 *(pc2++)=PN[0]; *(pc2++)=PN[1]; *(pc2++)='\0';
		}
	transfer(&pc,&pc2,0);
	*(pc2-1)='\n';
	}
*(pc2++)='\0';
siz = pc2 - buff + 4;
if (NULL==(where=(ENTRY *)malloc(siz))) exitm("mappend(): cannot malloc\n");
prev->link = (ENTRY *)memcpy(where,buff,siz);
}
/**********************************************/
ENTRY *findany(char *s)
{ENTRY *fnd;
 fnd=find(s,&MLINK);if(fnd) return(fnd);
 fnd=find(s,&SLINK);if(fnd) return(fnd);
 return(find(s,&RLINK));
}
/**********************************************/
void rndef()
{ARGs a; ENTRY *pe;
 int identsiz, newsiz, i;
 i = process(&a);
 if(i<2)
	{fprintf(stderr,"2 args needed for .RN\n");
	 LIN=LINE; LINE[0]='\0';
	 return;
	}
 identsiz=strlen(a.arg[1]);
 newsiz = strlen(a.arg[2]);
 if(identsiz!=newsiz)
	{fprintf(stderr,"replacement name should be same length for .RN\n");
	 LIN=LINE; LINE[0]='\0';
	 return;
	}
 pe=findany(a.arg[1]);
 if (pe) strcpy( (char *)&(pe->ident), a.arg[2] );
 else fprintf(stderr,"<%s> not found for .RN\n",a.arg[1]);
}
/**********************************/
/* returns NULL if s not found,   */
/* otherwise returns ->ENTRY found*/
/**********************************/
ENTRY *find0(char *s, ENTRY *LINK)
{ENTRY * LINK2;
 if(!LINK) return(NULL);
 if(*s=='\0') return(NULL);
 while(LINK->link)
	{LINK2=LINK->link;
	 if(!strcmp(s,(char *)&(LINK2->ident))) return(LINK);
	 LINK = LINK2;
	}
 return(NULL);
}
/**********************************************/
ENTRY *find0any(char *s)
{ENTRY *fnd;
 fnd=find0(s,&MLINK);if(fnd) return(fnd);
 fnd=find0(s,&SLINK);if(fnd) return(fnd);
 return(find0(s,&RLINK));
}
/**********************************************/
int RMRPT;	/*global for rm1(),remdef()*/
/****************/
int rm1(char *id) /* removes one definition; returns NULL if not found */
{ENTRY *prev, *curr, *next;
 char c;
 if((*id=='?')&&(id[1]=='\0')) return(RMRPT=TRUE);
 prev = find0any(id);
 if(!prev) return(NULL);
 do {curr = prev->link;
     next = curr->link;
     prev->link=next;
     free(curr);
     c = *(next->ident);
    }
 while(next&&((unsigned char)c >= '\177'));
 return(-1);
}
/**********************************************/
void rmdef()
{int num,i;
 ARGs a;
 RMRPT=FALSE;
 num = process(&a);
 if(num==0)
	{fprintf(stderr,".RM given no args.\n");
	 LIN=LINE; LINE[0]='\0';
	 return;
	}
 for(i=1;i<=num;i++)
	if(!rm1(a.arg[i]))
	    if(RMRPT)
		fprintf(stderr,"<%s> not found for .rm\n",
					a.arg[i]);
}
/**********************************************/
/* dappend() with divert() and enddiv() takes */
/* a .DI or .DA and following processed lines */
/* and places the information in MLIST.	      */
/* If .di (or .da with no prior definition),  */
/* then new entry at head of list.  If .da,   */
/* then places entry after prior definition.  */
/* Now uses malloc() to get space for entries.*/
/* Modified to have space for append and ends */
/* with .;;				      */
/**********************************************/
int DWIDTH=0;	/* max OUTW during current or last diversion */
char DNAME[FNAMSIZ]=""; /*name of current or last diversion*/
char Dbuff[MBUF+5];
unsigned Dsiz;
char *Dpc2;
ENTRY *Dprev,*Dpe;
DENTRY *Dstats; /*init by dappend; updated by enddiv() */
int Di,Di2,Dp1,Dp2,Dp3; /*used to update DWIDTH; cf. strlen4()*/
/**********************************************/
void dappend(CMD cmd)
{char c, *pc;
 ENTRY *found;
 /* similar to start of mappend() */
/*pass over command and following white space*/
for(pc=LIN,c=*pc; (c!=' ')&&(c!='\n')&&(c!='\t'); pc++)
	c=*pc;
for(; (c==' ')||(c=='\t'); pc++) c=*pc;
if(c=='\n')
	if(DIVERTING) {enddiv(); return;}
	else {fprintf(stderr,".de or .am is UNnamed\n");
	      LIN=LINE; LINE[0]='\0';
	      return;
	     }
else   {if(DIVERTING)
		{fprintf(stderr,"Cannot nest diversions; <%s> ignored.\n",
			LIN);
		 LIN=LINE; LINE[0]='\0';
		 return;
		}
	Dpe=(ENTRY *)Dbuff;
	Dpc2=(char *)&(Dpe->ident);
	while(class(c)==BLACK)
		{*(Dpc2++)=c;
		c=*(pc++);
		}
	}
*(Dpc2++)='\0';
strcpy(DNAME,Dpe->ident);
if (cmd==DI) found=FALSE;
else found=mpatch(Dpe->ident,&MLINK);
if (found) /*then should use patched name, location*/
   {Dprev = found;
    Dpe->link = found->link;
    Dpc2 = (char *)&(Dpe->ident);
    *(Dpc2++)=PN[0]; *(Dpc2++)=PN[1]; *(Dpc2++)='\0';
    Dsiz = Dpc2-Dbuff+4;
    Dstats=found->link;
    DLINES=Dstats->dn;
    DWIDTH=Dstats->dl;
   }
else
   {DENTRY *dentry;
    if (NULL==(dentry=(DENTRY *)malloc(sizeof(DENTRY))))
	exitm("dappend(): cannot malloc\n");
    dentry->ident[0]='\177';dentry->ident[1]='\0';
    dentry->dl=0;
    dentry->dn=0;
    dentry->dt=NO_VAL;
    dentry->dtname[0]='\0';
    dentry->link=MLINK.link;
    MLINK.link=Dstats=dentry;
    Dpe->link = MLINK.link;
    Dprev=&MLINK;
    DLINES=0;
    DWIDTH=0;
    Di=Di2=Dp3=Dp2=Dp1=0;
   }
DIVERTING=TRUE;
}
/**********************************************/
void divert(char *line)
{char *pc,c;
 ENTRY *where,*next;
 pc=line;
 Dsiz += strlen(LIN);
 if (Dsiz>=MBUF) /* then need to start new append */
	{*(Dpc2++)='\0';
	 Dsiz = Dpc2 - Dbuff + 4;
	 apatch(Dpc2-1);
	 if (NULL==(where=(ENTRY *)malloc(Dsiz)))
		exitm("divert(): cannot malloc\n");
	 next = Dprev->link;
	 Dprev->link = (ENTRY *)memcpy(where,Dbuff,Dsiz);
	 Dprev = Dprev->link;
	 Dsiz = sizeof(ENTRY)+4+strlen(line);
	 Dpe->link = next;
	 Dpc2 = (char *)&(Dpe->ident);
	 *(Dpc2++)=PN[0]; *(Dpc2++)=PN[1]; *(Dpc2++)='\0';
	 Di=Di2=Dp3=Dp2=Dp1=0;
	}
while(*pc)
	{c = *(Dpc2++) = *(pc++);
	 if(c=='\n')
		{if(Di2>Di) Di=Di2;
		 if(Di>DWIDTH) DWIDTH = Di;
		 Di=Di2=Dp3=Dp2=Dp1=0;
		}
	 else if((c!=TFVAL[0])&&(c!=CFVAL[0])) Di++;
	 else if(c==CFVAL[0])
	       {c = *(Dpc2++) = *(pc++);
		if(!c) break;
		switch(c)
		{case 'h':
		case 'H':if(Di>Di2) Di2=Di;
			 if(Di) Di--;
			break;
		case '(': Dp1=Di; break;
		case '[': Dp2=Di; break;
		case '{': Dp3=Di; break;
		case ')': if(Di>Di2) Di2=Di; Di=Dp1; break;
		case ']': if(Di>Di2) Di2=Di; Di=Dp2; break;
		case '}': if(Di>Di2) Di2=Di; Di=Dp3; break;
		default : break;
		}}
	}
}
/**********************************************/
void enddiv()
{ENTRY *where;
 *(Dpc2++)='\0';
 Dsiz = Dpc2 - Dbuff + 4;
 if (NULL==(where=(ENTRY *)malloc(Dsiz))) exitm("enddiv(): cannot malloc\n");
 Dprev->link = (ENTRY *)memcpy(where,Dbuff,Dsiz);
 if(Di2>Di) Di=Di2;
 if(Di>DWIDTH) DWIDTH = Di;
 Dstats->dl=DWIDTH;
 Dstats->dn=DLINES;
 DIVERTING=FALSE;
}
/****************************************/
void dtset()
{char c,*lptr;
 int *ip,save;
 if(!DIVERTING)
	{fprintf(stderr,"No current diversion; .dt ignored.\n");
	 LIN=LINE; LINE[0]='\0';
	 return;
	}
 ip=&(Dstats->dt);
 *ip=get_val2(LIN,&c,&lptr);
 if((*ip==NO_VAL)||(c=='-'))
	{*ip=0;
	 Dstats->dtname[0]='\0';
	 return;
	}
 save=SENTENCE;
 if(!getwrd(lptr,Dstats->dtname))
	{Dstats->dtname[0]='\0';
	 *ip=0;
	}
 SENTENCE=save;
 return;
}
