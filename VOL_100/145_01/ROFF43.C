/********************************************************/
/*							*/
/*		    ROFF4, Version 1.60			*/
/*							*/
/*(C) 1983,4 by Ernest E. Bergmann			*/
/*		Physics, Building #16			*/
/*		Lehigh Univerisity			*/
/*		Bethlehem, Pa. 18015			*/
/*							*/
/* Permission is hereby granted for all commercial and	*/
/* non-commercial reproduction and distribution of this	*/
/* material provided this notice is included.		*/
/*							*/
/********************************************************/
/*MAR 10, 1984*/
#include "roff4.h"
/****************************************/
regist(s)	/*checks RLIST; creates new entry if needed;
		returns pointer to integer variable*/
char *s;
{int *pw;
char *pc,*s2;
pw=find2(s,RLINK);
if(pw) return(pw); /*else create new entry*/
pw=TREND;
*pw=RLINK;
pc=pw+1;
s2=s;
transfer(&s2,&pc,'\0');
RLINK=TREND;
pw=pc;
*pw=REGDEF;
TREND=pw+1;
return(pw);
}
/****************************************/
dovar()	/*process .RG request*/
{char typ, wbuf[MAXLINE];
int val, *pw;
getwrd(LINE,wbuf);
skip_blanks(LINE);
val=get_val(LINE,&typ);
getwrd(LINE,wbuf);
if(wbuf[0]==NUMSIGN && wbuf[1]=='\0')
	{set(&NEWPAG,val,typ,NEWPAG,1,9999);
	}
else	{pw=regist(wbuf);
	set(pw,val,typ,REGDEF,1,9999);
	}
}
/****************************************/
dodiv()		/*process .DIversion*/
{char wbuf[MAXLINE],*pc,*pc1,*wb;
struct divfd *pd;
int *pw;
getwrd(LINE,wbuf);
if(getwrd(LINE,wbuf)!=WE_HAVE_A_WORD)
	{strcpy(wbuf,"JUNK.$$$");
	fprintf(stderr,"DIVERSION has no name, %s assumed\n",
				wbuf);
	}
ucstr(wbuf);
if(pd=find2(wbuf,DLINK))	/*if in table*/
	{if(!(pd->bf))		/*if not open*/
#if BDS
	    if(pd->bf = alloc(BUFSIZ)) /*if space for buffer*/
			{if(fcreat(wbuf,pd->bf)!=ERROR)
#endif
#if C86
		if (1)
			{if((pd->bf = fopen(wbuf,"wb"))!=ERROR)
#endif
#if DeSmet
		if (1)
			{if((pd->bf = creat(wbuf,"w"))!=ERROR)
#endif
				fprintf(stderr,
				"<%s> rewritten for .DI\n",
				wbuf);
			else	{fprintf(stderr,
				"<%s> cannot be rewritten\n",
				wbuf); exit();
				}
			}
		else	{fprintf(stderr,
				"\nCan't get space for <%s>\n",
				wbuf); exit();
			}
	}
else	/*not in table*/
	{pw=TREND;
	*pw=DLINK;
	pc1=pc=pw+1;
	wb=wbuf;
	transfer(&wb,&pc,'\0');
	DLINK=TREND;
	pd=pc;
	TREND = pd+1;
	pd->nm = pc1;
	pd->ls = pd->cs = 0;
#if BDS
	if(pd->bf = alloc(BUFSIZ))
		{if(fcreat(wbuf,pd->bf)==ERROR)
#endif
#if C86
	if (1)
		{if((pd->bf = fopen(wbuf,"wb"))==ERROR)
#endif
#if DeSmet
	if (1)
		{if((pd->bf = creat(wbuf,"w"))==ERROR)
#endif
			{fprintf(stderr,
				"Can't create <%s>\n",
				wbuf);
			exit();
			}
		}
	else	{fprintf(stderr,"Can't allocate buf for %s\n",
			wbuf);
		exit();
		}
	}
while(fgets2(LINE,iobuf))
	{if((*LINE==COMMAND)&&(comtyp(LINE)==ED)) break;
	if(pc=macq(LINE))
		{pbstr(pc);
		continue;
		}
	/*else*/
	fputs(LINE,pd->bf);
	(pd->ls)++;
	(pd->cs) += 1+strlen(LINE);/*crlf=2*/
	}
}
/****************************************/
source()
{struct divfd *pd;
char name[MAXLINE];
getwrd(LINE,name);
if(getwrd(LINE,name)!=WE_HAVE_A_WORD)
	{fprintf(stderr,".SO has no name\n");
	return;
	}
ucstr(name);

if(pd=find2(name,DLINK)) dclose(pd);
#if	BDS
if(FPTR<FMAX) FSTACK[FPTR++]=iobuf;
else	{fprintf(stderr,"FSTACK overflow\n");
	exit();
	}
if(iobuf=alloc(BUFSIZ))
	{if(fopen(name,iobuf)!=ERROR) return;
	/*else*/
	fprintf(stderr,"Can't open <%s>\n",name);
	}
else fprintf(stderr,".SO can't buffer <%s>\n",name);
#else	/* More normal C compilers	*/
if(FPTR<FMAX) FSTACK[FPTR++]=iobuf;
else	{fprintf(stderr,"FSTACK overflow\n");
	exit();
	}
#if C86
if((iobuf = fopen(name,"r"))!=ERROR) return;
#endif
#if DeSmet	/* Well, almost normal	*/
if((iobuf = fopen(name,"r"))!=FERR) return;
#endif
/*else*/
fprintf(stderr,"Can't open <%s>\n",name);
#endif
endso();
}
/****************************************/
showr()	/*lists register names and contents*/
{int *pw, *pr;
char *pc;
fprintf(stderr,"REGISTERS and <values>:\n");
pw=RLINK;
while(pw)
	{pc=pw+1;
	fprintf(stderr,"%s ",pc);
	pr=pc+1+strlen(pc);
	fprintf(stderr,"<%d>\n",*pr);
	pw=*pw;
	}
dashes();
}
/****************************************/
showd()	/*shows all diversions and status*/
{int *pw;
char *pc;
struct divfd *pd;

fprintf(stderr, "Diversion files:\n");
pw=DLINK;
while(pw)
	{pc=pw+1;
	fprintf(stderr,"%s: ",pc);
	pd=pc+1+strlen(pc);
	fprintf(stderr,"%d characters, %d lines [",
			pd->cs, pd->ls );
	if(pd->bf) fprintf(stderr,"open]\n");
	else	fprintf(stderr,"closed]\n");
	pw =*pw;
	}
dashes();
}
/****************************************/
dsclose()	/*flushes and closes all open diversions*/
{int *pw;
char *pc;
struct divfd *pd;

pw = DLINK;
while(pw)
	{pc=pw+1;
	pd=pc+1+strlen(pc);
	if(pd->bf) dclose(pd);
	pw=*pw;
	}
}
/****************************************/
dclose(pd)	/*flushes and closes diversion*/
struct divfd *pd;
{if(pd&&(!(pd->bf))) return(FALSE);
fprintf(stderr,"closing %s ",pd->nm);
#if	MSDOS == 0
putc(CPMEOF,pd->bf);
#endif
#if BDS
fflush(pd->bf);
#endif
#if C86
fflush(pd->bf);
#endif
if(fclose(pd->bf)==ERROR)
	fprintf(stderr,"****bad close****\n");
else	{fprintf(stderr,"[O.K.]\n");
#if BDS
	free(pd->bf);
#endif
	pd->bf = FALSE;
	}
return(TRUE);
}
/****************************************/
ucstr(s)	/*convert s to upper case*/
char *s;
{while(*s)
	{*s = toupper(*s);
	s++;
	}
}
/****************************************/
endso()	/*called upon EOF to return to previous input file*/
{if(FPTR)
	{fclose(iobuf);
#if	BDS
	free(iobuf);
#endif
	iobuf=FSTACK[--FPTR];
	if DEBUG fprintf(stderr, "endso: FPTR = %d\n", FPTR);
	}
BINP=0;
}
/****************************************/
dashes()
{fprintf(stderr,"-----------\n");
}
