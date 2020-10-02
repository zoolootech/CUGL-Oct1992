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
/**********************************************************
Advances LIN in LINE over white-space characters.
"^t","^T" are considered tabs, hence white space.
***********************************************************/
void skip_blanks ( )
{while (*LIN==BLANK||*LIN==TAB||*LIN==NEWLINE
	||((*LIN=='^')&&(LIN[1]=='t'))
	||((*LIN=='^')&&(LIN[1]=='T'))
	)
	{if (*LIN=='^') LIN++;
	 LIN++;
	}
}
/*************************************************************/
CMD comtyp1(char *line)
{char let1, let2;
let1 = line[0] ;
let2 = line[1] ;

if ( let1==COMMAND )	return( DOTDOT );
if ( let1=='{' )	return( LEFT );
if ( let1=='}' )	return( RIGHT );
if ( let1=='i' && let2=='g')	return( IG );
if ( let1=='f' && let2=='i')	return( FI );
if ( let1=='f' && let2=='o')	return( FO );
if ( let1=='t' && let2=='i')	return( TI );
if ( let1=='b' && let2=='p')	return( BP );
if ( let1=='b' && let2=='r')	return( BR );
if ( let1=='c' && let2=='e')	return( CE );
if ( let1=='h' && let2=='e')	return( HE );
if ( let1=='l' && let2=='s')	return( LS );
if ( let1=='n' && let2=='f')	return( NF );
if ( let1=='p' && let2=='l')	return( PL );
if ( let1=='s' && let2=='p')	return( SP );
if ( let1=='s' && let2=='t')	return( ST );
if ( let1=='n' && let2=='e')	return( NE );
if ( let1=='f' && let2=='f')	return( FF );
if ( let1=='s' && let2=='c')	return( SC );
if ( let1=='o' && let2=='h')	return( OH );
if ( let1=='o' && let2=='f')	return( OF );
if ( let1=='e' && let2=='h')	return( EH );
if ( let1=='e' && let2=='f')	return( EF );
if ( let1=='a' && let2=='b')	return( AB );
if ( let1=='t' && let2=='f')	return( TF );
if ( let1=='c' && let2=='f')	return( CF );
if ( let1=='i' && let2=='c')	return( IC );
if ( let1=='o' && let2=='u')	return( OU );
if ( let1=='a' && let2=='d')	return( AD );
if ( let1=='n' && let2=='a')	return( NA );
if ( let1=='f' && let2=='r')	return( FR );
if ( let1=='d' && let2=='e')	return( DE );
if ( let1=='a' && let2=='m')	return( AM );
if ( let1=='d' && let2=='s')	return( DS );
if ( let1=='n' && let2=='r')	return( NR );
if ( let1=='d' && let2=='i')	return( DI );
if ( let1=='d' && let2=='a')	return( DA );
if ( let1=='s' && let2=='o')	return( SO );
if ( let1=='o' && let2=='t')	return( OT );
if ( let1=='t' && let2=='m')	return( TM );
if ( let1=='b' && let2=='j')	return( BJ );
if ( let1=='a' && let2=='f')	return( AF );
if ( let1=='i' && let2=='f')	return( IF );
if ( let1=='i' && let2=='e')	return( IE );
if ( let1=='e' && let2=='l')	return( EL );
if ( let1=='^' && let2=='z')	return( CZ );
if ( let1=='r' && let2=='l')	return( RL );
if ( let1=='e' && let2=='v')	return( EV );
if ( let1=='r' && let2=='m')	return( RM );
if ( let1=='r' && let2=='n')	return( RN );
if ( let1=='p' && let2=='m')	return( PM );
if ( let1=='p' && let2=='m')	return( PM );
if ( let1=='m' && let2=='c')	return( MC );
if ( let1=='n' && let2=='m')	return( NM );
if ( let1=='n' && let2=='n')	return( NN );
if ( let1=='p' && let2=='c')	return( PC );
if ( let1=='l' && let2=='c')	return( LC );
if ( let1=='t' && let2=='c')	return( TC );
if ( let1=='p' && let2=='o')	return( PO );
if ( let1=='w' && let2=='h')	return( WH );
if ( let1=='c' && let2=='h')	return( CH );
if ( let1=='i' && let2=='t')	return( IT );
if ( let1=='e' && let2=='m')    return( EM );
if ( let1=='d' && let2=='t')	return( DT );
if ( let1=='t' && let2=='l')	return( TL );
if ( let1=='g' && let2=='o')	return( GO );

if ( let1=='m')
      { if (let2=='1')		return( M1 );
	if (let2=='2')		return( M2 );
	if (let2=='3')		return( M3 );
	if (let2=='4')		return( M4 );
      }
return( UNKNOWN );		/* no match */
}
/*************************************************************
 sets a global parameter like SPVAL, PAGESTOP, etc.
 Also checks that the new value is within the range of that 
 parameter.  Assigns the default for that parameter if no value
  is specified.
*************************************************************/
void set ( int *param, int val, char arg_typ,
		int defval, int minval, int maxval )
{if(val==NO_VAL) *param = defval;	/* defaulted */
else if(arg_typ == '+') *param += val;	/* relative + */
else if(arg_typ == '-')	*param -= val;	/* relative - */
else	*param = val;			/* absolute */
*param = min (*param,maxval);
*param = max (*param, minval);
}
/*************************************************************
	end current filled line
**************************************************************/
void Brk()
{int l;
if (OUTPOS) put(OUTBUF);
OUTW=OUTPOS=OUTTOP=OUTBOT=OUTWRDS = 0;
OUTBUF[0] = '\0';
}

/**************************************************/
void initxu()	/*initialize underline,overstrike variables*/
{	XCOL=UCOL=-1;
	setmem(&XBUF,LSZ,' ');
	setmem(&UBUF,LSZ,' ');
}
/****************************************/
/*test for n line of space before footer*/
/****************************************/
void need(int n)
{if(DIVERTING)
	{if(((DLINES<(Dstats->dt)) && ((DLINES+n)>=(Dstats->dt))))
		{Brk();
		 space((Dstats->dt)-DLINES);
		 return;
		}
	}
 else if (( VLINENO>=(TRAPLOC-n) ) && (TRAPLOC>=VLINENO) )
		{Brk();
		 space(HUGE);
		 NEWPAG= ++CURPAG;
		}
}
/****************************************/
int min(int i,int j) {return( (i<j)?i:j);}

int max(int i, int j) {return( (i>j)?i:j);}
/*******************************************/
/* process() parses args from LIN in LINE[]*/
/* into entries of the arglist a for macro */
/* processing by dolns(); returns # args.  */
/*******************************************/
int process(ARGs *a)
{int i;
char *pc, c;
strcpy(a->tokens,LIN);
pc=a->tokens;
for(i=0;i<ARGLIM;i++)
	{c=*pc;	/*skip whitespace*/
	 while((c==' ')||(c=='\t')||(c=='\n')) c=*(++pc);
	 if (c=='\0') a->arg[i]=NULL;
	 else if (c=='\"')	/*quoted string*/
		{a->arg[i] = ++pc;
		 c=*pc;
		 while((c!='\0')&&(c!='\n')&&(c!='\"')) c=*(++pc);
		 if (c!='\0') *(pc++)='\0';	/*mark end*/
		}
	 else	{a->arg[i]=pc;	/*simple token*/
		 while((c!='\0')&&(c!=' ')&&(c!='\t')&&(c!='\n'))
			c = *(++pc);
		 if (c!='\0') *(pc++)='\0';	/*mark end*/
		}
	}
for (i=1;(a->arg[i])&&(i<ARGLIM);i++);
i--;
a->dollar = i;
return (i) ;
}
/************************************/
/* pbmac() is more generalized than */
/* pbstr() in that it will expand   */
/* "$n" and "$" found in macro      */
/* expansion string m from a        */
/************************************/
void pbmac(char *m, ARGs a)
{int i;
char c, c2;
if ((i=strlen(m))==0) return;
c = m[--i];
if (c=='$') putback('0'+a.dollar);
else putback(c);
while (i>0)
	{c=m[--i];
	 if (c=='$')
		{c = BACKBUF[BINP--];
		 if ((c>'0')&&(c<='9')) pbstr(a.arg[c-'0']);
		 else	{putback(c); putback('0'+a.dollar);}
		}
	 else putback(c);
	}
}
/**************************************/
#include "\turboc\include\setjmp.h"
#define TOKSIZ LSZ
jmp_buf jumper;
char *xptr,*saveptr;
/*****************************************************/
/* get_val2() is more general than the old get_val() */
/* in that it can do arithmetic and a greater range  */
/* of typ (any nonwhite character); and it now       */
/* indicates where on line it has finished parsing.  */
/*****************************************************/
int get_val2(char *line, char *typ, char **end)
{char c; int val;
 saveptr=xptr=line; c=*xptr;
 while ((c!=' ')&&(c!='\t')&&(c!='\n')) c=*(++xptr); /*skip command*/
 while ((c==' ')||(c=='\t')||(c=='\n')) c=*(++xptr); /*skip white*/
 *typ=c;
 if ((c=='+')||(c=='-')||(c=='!')) c=*(++xptr);
 while ((c==' ')||(c=='\t')||(c=='\n')) c=*(++xptr); /*skip white*/
 if ((c=='\0')||(c=='\r')||(c==';'))
	{*end=xptr;
	 return (NO_VAL);
	}
 val=expr();
 *end=xptr;
 return ( val );
}
/****************************************************/
/* setnxtval can do arithmetic and a greater range  */
/* of typ (any nonwhite character); and it updates  */
/*  line to where on line it has finished parsing.  */
/****************************************************/
void setnxtval(char **line,int *param,int deflt,int minparam,int maxparam)
{char c,typ; int val;
 saveptr=xptr=*line; c=*xptr;
 if(!c) return;
 while ((c!=' ')&&(c!='\t')&&(c!='\n')) c=*(++xptr); /*skip command*/
 while ((c==' ')||(c=='\t')&&(c=='\n')) c=*(++xptr); /*skip white*/
 typ=c;
 if ((c=='+')||(c=='-')||(c=='!')) c=*(++xptr);
 while ((c==' ')||(c=='\t')||(c=='\n')) c=*(++xptr); /*skip white*/
 if ((c=='\0')||(c=='\r')||(c==';'))
	{*line=xptr;
	 return;
	}
 val=expr();
 *line=xptr;
 set(param,val,typ,deflt,minparam,maxparam);
}
int expr()
{if (setjmp(jumper)!=0)
	{fprintf(stderr,"Ignoring expression (with error):\n");
	 amat();
	 return(NO_VAL);
	}
 return (expr0());
}

int expr0()
{int i,j;
 char c;
 j=expr1();
 c=*xptr;
 if (c=='>')
 	{c=*(++xptr);
 	 if (c=='=') {xptr++; if (j>=expr1()) return(TRUE);else return(0);}
 	 else {if (j>expr1()) return(TRUE);else return(0);}
 	}
 else if (c=='<')
 	{c=*(++xptr);
 	 if (c=='=') {xptr++; if (j<=expr1()) return(TRUE);else return(0);}
 	 else if (c=='>') {xptr++; if (j!=expr1()) return(TRUE);else return(0);}
 	 else {if (j<expr1()) return(TRUE);else return(0);}
 	}
 else if (c=='&') {xptr++; if (j & expr1()) return(TRUE);else return(0);}
 else if (c==':') {xptr++; if (j | expr1()) return(TRUE);else return(0);}
 else if (c=='=') {xptr++; if (j == expr1()) return(TRUE);else return(0);}
 else return(j);
}
int expr1()
{int i;
 if (*xptr=='-')
 	{xptr++;
 	 i = -expr2();
 	}
 else if (*xptr=='+')
 	{xptr++;
 	 i = expr2();
 	}
 else i = expr2();
 /* now have taken care of optional unary operator and first term */
 /* now for any possible additional terms: */
 while ((*xptr=='-')||(*xptr=='+'))
 	{if (*xptr=='-')
 		{xptr++; i -= expr2();
 		}
 	 if (*xptr=='+')
 		{xptr++; i += expr2();
 		}
	}
 return (i);
}
int expr2()
{int val;
 val = expr3();
 /* first factor done; process optionally more: */
 while((*xptr=='*')||(*xptr=='/')||(*xptr=='%'))
 	{ if (*xptr=='*')
 		{xptr++; val *= expr3();
 		}
          if (*xptr=='/')
          	{xptr++; val = val/expr3();
          	}
          if (*xptr=='%')
          	{xptr++; val = val % expr3();
          	}
	}
 return (val);
}
int expr3()
{int val;
 if (isdigit(*xptr))
 	{val=0;
 	 while (isdigit(*xptr))
 		{val=10*val + (*xptr - '0');
 		 xptr++;
 		}
	 return(val);
 	}
 else if (*xptr=='(')
 	{xptr++;
 	 val = expr0();
 	 /* amat(); showstk(); */
 	 if(*xptr==')') xptr++;
 	 else	longjmp(jumper,1); /*error return*/
	 return (val);
	}
 else if((*xptr=='\'')||(*xptr=='"'))
	{char tbuf1[TOKSIZ],tbuf2[TOKSIZ],*ptr1,*ptr2,term;
	 term=*xptr++;
	 ptr1=xptr;
	 ptr2=tbuf1;
	 while(*ptr1 && (*ptr1!=term)) *(ptr2++)=*(ptr1++);
	 *ptr2='\0';
	 if(*ptr1) ptr1++;
	 ptr2=tbuf2;
	 while(*ptr1 && (*ptr1!=term)) *(ptr2++)=*(ptr1++);
	 *ptr2='\0';
	 if(*ptr1) ptr1++;
	 xptr=ptr1;
	 if(strcmp(tbuf1,tbuf2)) return(FALSE);
	 else return(TRUE);
	}
 else	{char tbuf[TOKSIZ],c, *ptr1, *ptr2; int *pw;
	 ptr1=xptr;
	 ptr2=tbuf;
	 while (isalnum(*ptr1)||(*ptr1=='#')) *(ptr2++)=*(ptr1++);
	 *ptr2='\0';
	 if ((tbuf[0]=='#')&&(tbuf[1]=='\0'))
		{xptr= ptr1;
		 return(CURPAG);
		}
	else if (NULL!=(pw=(int *)find2(tbuf,&RLINK)))
		{xptr = ptr1;
		 return (*pw);
		}
	}
 longjmp(jumper,1); /*error return*/
return(val);/*never reach here but eliminates compiler warning*/
}
void amat()
{char *pc;
 fprintf(stderr,"%s\n",saveptr);
 pc=saveptr;
 while (pc<xptr)
 	{printf(" ");
 	 pc++;
 	}
 fprintf(stderr,"^\n");
}
