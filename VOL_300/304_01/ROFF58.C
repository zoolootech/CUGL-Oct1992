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
/********************************************************/
char *fgets3()
{	int count,c,i,*pw;
	char *cptr,*pc, *pc2;
	ENTRY *pe;
	char wbuf[LSZ],*fnd;
	count = MAXLINE;
	cptr = &LINE[0];
	if ( (c=ngetc(ifp))==EOF) return (LIN=NULL);
	do {	if(c==ICVAL[0])/*need macro substitution*/
		  {for(i=0;ICVAL[0]!=(wbuf[i]=ngetc(ifp));i++)
			{if(wbuf[i]=='\n')
				{if(i) putback('\n');
				break;
				}
			}
		  if(i)
		   {wbuf[i]='\0';
		    if (*wbuf=='\"')	/*comment rest of line*/
			{*cptr='\n';
			 *(++cptr)='\0';
			 do c=ngetc(ifp);
			 while((c!='\n')&&(c!=EOF));
			 return( LIN=LINE );
			}
		    else if(readonly(wbuf)) ; /* handled as predefined */
		    else if(NULL!=(pw=(int *)find2(wbuf,&RLINK)))/*register?*/
			{itoC(*pw,wbuf,*(FORMAT *)(pw+1));
			pbstr(wbuf);
			}
		    else if(NULL!=(fnd=find2(wbuf,&SLINK))) pbstr(fnd);
		    else{
			char buff[128];
			unsigned siz;
			ENTRY *where;

		    	pc=wbuf;
			pe=(ENTRY *)buff;
			pe->link=SLINK.link;
			pc2=(char *)&(pe->ident);
			transfer(&pc,&pc2,'\0');
			fprintf(stderr,"%cPlease define <%s>:",
						BELL,wbuf);
			gets(wbuf);
			pc=wbuf;
			transfer(&pc,&pc2,'\0');
			siz = pc2 - buff;
			if (NULL==(where=(ENTRY *)malloc(siz)))
				exitm("fgets3(): cannot malloc\n");
			SLINK.link = (ENTRY *)memcpy(where,buff,siz);
			pbstr(wbuf);
			}
		    continue;
		    }
		   else if(*wbuf!='\n') putback(ICVAL[0]);
		   c=ngetc(ifp);
		  }
		if ((*cptr++ = c) == '\n') break;

	 } while (count--&&(c=ngetc(ifp))!=EOF);
       *cptr = '\0';
	return (LIN=LINE);
}
/**************************************************************
performs the formatting command returned by comtyp1 -sets global
  variables ( indenting, underlining, etc. )
"line" starts just after the ".".
**************************************************************/
void comand()
{CMD c_type;	/* command type  */
int arg_val;	/* argument value, if any */
char arg_typ;	/* relative (+ or -) or absolute or conditional? */
char wbuf[LSZ], *lptr;
int i;
c_type = comtyp1(LIN);
switch (c_type)
	{case UNKNOWN :
		fprintf(stderr, "UNKNOWN COMMAND: <%s>\n", LINE);
		return;
	case DOTDOT : break;/* ignore remark */
	case IG : ignore(); break;

	case FI :	/* filled lines  */
		Brk();
		FILL = YES;
		break;
	case NF :	/* non-filled lines */
		Brk();
		FILL = NO;
		break;
	case NA :	/* Not Adjust lines */
		JUSTIFY = NO;
		break;
	case AD :	/* ADjust lines  */
		adjust();
		break;
	case BR :	/* just cause a break */
		Brk();
		break;
	case LS :	/* set line spacing value */
		arg_val = get_val2(LIN, &arg_typ, &lptr );
		setS(LSVAL, arg_val, arg_typ, LS_DEF, 1, HUGE );
		break;
	case TI :	/* set temporary left indent */
		Brk();
		arg_val = get_val2( LIN, &arg_typ, &lptr );
		set ( &TIVAL, arg_val, arg_typ, TI_DEF, 0, RMVAL );
		break;
	case M1:	/* set topmost margin */
		arg_val = get_val2( LIN, &arg_typ, &lptr );
		setS( M1VAL, arg_val, arg_typ, M1_DEF,0,HUGE);
		break;
	case M2:	/* set second top margin */
		arg_val = get_val2( LIN, &arg_typ, &lptr );
		setS( M2VAL, arg_val, arg_typ, M2_DEF,0,HUGE);
		break;
	case M3:	/* set first bottom margin */
		arg_val = get_val2( LIN, &arg_typ, &lptr );
		setS( M3VAL, arg_val, arg_typ, M3_DEF,0,HUGE);
		setTRAP();
		break;
	case M4:	/* set bottom-most margin */
		arg_val = get_val2( LIN, &arg_typ, &lptr );
		setS(M4VAL, arg_val, arg_typ, M4_DEF,0,HUGE);
		setTRAP();
		break;
	case CE :	/* center next arg_val lines */
		Brk();
		arg_val = get_val2( LIN, &arg_typ, &lptr );
		set ( &CEVAL, arg_val, arg_typ,CE_DEF,0, HUGE);
		break;
	case HE :	/* get header title for pages */
		gettl3 ( LIN, EHEAD, &EH2, &EH3 );
		gettl3 ( LIN, OHEAD, &OH2, &OH3 );
		break;
	case OH :	/*get odd header title*/
		gettl3 ( LIN, OHEAD, &OH2, &OH3 );
		break;
	case EH :	/*get even header title*/
		gettl3 ( LIN, EHEAD, &EH2, &EH3 );
		break;
	case FO :	/* get footer title for pages */
		gettl3 ( LIN, EFOOT, &EF2, &EF3 );
		gettl3 ( LIN, OFOOT, &OF2, &OF3 );
		break;
	case OF :	/* get odd page footer title*/
		gettl3 ( LIN, OFOOT, &OF2, &OF3 );
		break;
	case EF :	/* get even page footer title*/
		gettl3 ( LIN, EFOOT, &EF2, &EF3 );
		break;
	case SP :	/* space down arg_val blank lines */
		arg_val = get_val2( LIN, &arg_typ, &lptr );
		set (&SPVAL, arg_val, arg_typ, 1, 0, HUGE);
		*(--LIN)='\0';
		space ( SPVAL );
		break;
	case GO :	/*advance printer to absolute location*/
		arg_val = get_val2( LIN, &arg_typ, &lptr );
		if (arg_val>=VLINENO) break; /*already there*/
		set (&SPVAL, arg_val, arg_typ, 1, 0, HUGE);
		if ((SPVAL>PLINENO)&&!DIVERTING)
		if (PLINENO) space ( SPVAL - VLINENO );
		else space(SPVAL-M1VAL[0]-M2VAL[0]);
		break;

	case ST :	/* stop(pause) at each page?*/
		arg_val = get_val2( LIN, &arg_typ, &lptr );
		set(&PAGESTOP,arg_val,'0',YES,NO,YES);
		break;
	case BP :	/* set pageno arg_val - begin page */
		Brk();
		arg_val = get_val2( LIN, &arg_typ, &lptr );
		if(((VLINENO<=0)||(VLINENO>=FOOTLOC))&&
			(arg_val==NO_VAL)) break;
		if ( VLINENO > 0 )	space (HUGE);
		set(&CURPAG,arg_val,arg_typ,CURPAG+1,0,9999);
		NEWPAG = CURPAG;
		break;
	case NE :	/*"need"*/
		arg_val = get_val2( LIN, &arg_typ, &lptr );
		if (arg_val==NO_VAL) arg_val=2;/*default*/
		need(arg_val);
		break;
	case PL :	/* set page length */
		arg_val = get_val2( LIN, &arg_typ, &lptr );
		setS(PLVAL, arg_val, arg_typ, PL_DEF,
		  M1VAL[0]+M2VAL[0]+M3VAL[0]+M4VAL[0]+1,HUGE);
		setTRAP();
		break;
	case FF :	/*formfeed*/
		arg_val = get_val2( LIN, &arg_typ, &lptr );
		set(&FFEED,arg_val,'0',FF_DEF,NO,YES);
		break;
	case SC :	/*space character*/
		setchar(SCVAL,TRUE,SC_INI,BLANK,127);
		break;
	case AB :	/*abort*/
		gwLIN(wbuf);	/*skip command*/
		skip_blanks();
		trunc_bl(LIN);
		if(strlen(LIN)) fprintf(stderr,"\n%s\n",LIN);
		else fprintf(stderr,"\n***USER ABORT***\n");
		exit();
	case TF :	/*translate flag */
		setchar(TFVAL,TRUE,TF_DEF,BLANK+1,127);
		break;
	case CF :	/*character flag*/
		setchar(CFVAL,TRUE,CF_DEF,BLANK+1,127);
		break;
	case IC :	/*insert character for macro replace*/
		setchar(ICVAL,TRUE,IC_DEF,BLANK+1,127);
		break;
	case OU :	/*output code string*/
		ocode(); break;
	case FR :	/*define fractional or whole line spacing code*/
		getfr(); break;
	case WH : when();
		 setTRAP();
		 break;
	case CH : changetrap();
		 setTRAP();
		 break;
	case IT : itset();
		 break;
	case EM : EMset();
		 break;
	case DT : dtset();
		 break;
	case TL :title3(LIN,CURPAG,PAGFMT); break;
	case DS :	/*define string*/
		insert(); break;
	case DE :	/*define macro*/
	case AM :	/*append macro*/
		mappend(c_type);
		break;
	case NR :	/*number register */
		dovar(); break;
	case DI :	/*diversion to file*/
	case DA :
		dappend(c_type);
		break;
	case SO :	/*source from file*/
		source(); break;
	case OT :	/*Output Transaltion*/
		getot(); break;
	case TM :	/*msg to terminal*/
		gwLIN(wbuf);	/*skip command*/
		skip_blanks();
		trunc_bl(LIN);
		fprintf(stderr,"<%s>\n",LIN);
		break;
	case BJ :	/*break with right justification*/
		if(FILL)/*not applicable otherwise*/
		{spread(OUTBUF,
			min(RMVAL-TIVAL,MAXLINE-1)-OUTW+1,
			OUTWRDS);
		Brk();
		}
		break;
	case AF :	/*assign format to register variable*/
		assignfmt();
		break;
	case CZ :	/*substitute ^Z character in decimal*/
		arg_val = get_val2( LIN, &arg_typ, &lptr );
		set(&CZSUB,arg_val,'0',(int)CZSUB_DEF,0,255);
		break;
	case RL :	/* Ruler Line similar to Wordstar for tabs*/
		gwLIN(wbuf);	/*skip command*/
		skip_blanks();
		trunc_bl(LIN);
		if(LIN[0]=='?') {TIVAL=0; put(RULER); return;}
		else if (LIN[0]=='L') gwLIN(wbuf);
		else if (LIN[0]=='\"') strcpy(wbuf,strtok(LIN,"\""));
		else	{redoRR();
	 		 TIVAL=INVAL;
			 return;
			}
		if (verifyRR(wbuf)==FALSE)
			fprintf(stderr,"Bad RULER; request ignored.\n");
		TIVAL=INVAL;
		break;
	case EV :	/* change enviroment */
		{int temp;
		temp=EVVAL[0];
		arg_val = get_val2( LIN, &arg_typ, &lptr );
		setS(EVVAL,arg_val,arg_typ,EV_INI,0,NUMENV-1);
		if (temp!=EVVAL[0]) chgenv(temp,EVVAL[0]);
		break;
		}
	case RM :	/* remove definition */
		rmdef();
		break;
	case RN :	/* rename definition */
		rndef();
		break;
	case PM : showm(get_val2(LIN,&arg_typ,&lptr));
	/*print list of macros*/
		  break;
	case MC : exitm(".mc not implemented yet!\n");
	case NM : NNVAL = 0;
		  arg_val=get_val2(LIN,&arg_typ,&lptr);
		  if(arg_val==NO_VAL) NUMBERING=FALSE;
		  else	{NUMBERING=TRUE;
			 set(&LN,arg_val,arg_typ,1,1,HUGE);
			 setnxtval(&lptr,&LNMOD,1,1,HUGE);
			 setnxtval(&lptr,&LNST,1,0,HUGE);
			 setnxtval(&lptr,&LNI,0,0,HUGE);
			}
		  break;
	case NN : arg_val = get_val2( LIN, &arg_typ, &lptr );
		  set(&NNVAL,arg_val,arg_typ,1,1,HUGE);
		  break;
	case PC : exitm(".pc not implemented yet!\n");
	case PO : arg_val = get_val2(LIN,&arg_typ,&lptr);
		  set(&dotO,arg_val,arg_typ,0,0,70);
		  break;
	case LC :	/*Leader Expansion Character*/
		setchar(&LCVAL,FALSE,BLANK,BLANK,127);
		break;
	case TC :	/*Tab expansion Character*/
		setchar(&TCVAL,FALSE,BLANK,BLANK,127);
		break;
	case IE :
	case EL :
	case RIGHT :
	case LEFT :
	case IF :	/* unexpected since filtered by action */
		conderr("conditional by command()");
		break;
      }
}
/*********************************************/
void dolns()
{LIN=LINE;
 while(LIN || FPTR)
   {if(LIN) action();
    else endso();
    if(LIN[0]==COMMAND)
      {if(LIN[1]=='}') conderr(".}");
       else if((LIN[1]=='e')&&(LIN[2]=='l')) conderr(".el");
      }
   }
}
/*******************/
void action()
{if(LIN)	/*not EOF*/
  {if(!(*LIN)) fgets3();
   if(!LIN) return;	/*EOF*/
   if(*LIN != COMMAND) {text(); LIN=LINE; LINE[0]='\0';}
   else{CMD c_type;
	int arg_val;
	char arg_typ, *lptr;
	LIN++;
	c_type=comtyp1(LIN);
	if(c_type<UNKNOWN)	/*conditional grammar*/
	  switch(c_type)
	   {case IF :	/*"if" conditional*/
		arg_val = get_val2( LIN, &arg_typ, &lptr );
		if (arg_val==NO_VAL) arg_val=FALSE;
		else if (arg_typ=='!') arg_val= !arg_val;
		LIN = lptr;
		while((*LIN==' ')||(*LIN=='\t')||(*LIN=='\n')) LIN++;
		if(arg_val) action();
		else inaction();
		break;
	    case IE :	/*"if-else" conditional*/
		arg_val = get_val2( LIN, &arg_typ, &lptr );
		if (arg_val==NO_VAL) arg_val=FALSE;
		else if (arg_typ=='!') arg_val= !arg_val;
		LIN = lptr;
		while((*LIN==' ')||(*LIN=='\t')||(*LIN=='\n')) LIN++;
		if(arg_val)
			{action();
			 act2(FALSE);
			}
		else	{inaction();
			 act2(TRUE);
			}
		break;
	    case LEFT: /*.{ encountered */
		{char dummy[LSZ];
		 gwLIN(dummy);
		 skip_blanks();
		 while ((LIN[0]!='.')||(LIN[1]!='}'))
			{if(!LIN) {conderr("EOF"); return;}
			 action();
			}
		 gwLIN(dummy);
		 skip_blanks();
		} break;
	    case RIGHT: LIN--; break;
	    case EL:	LIN--; break;
	   }
	else
	   {char *pc;
	    pc=macq1(LIN);
	    if(pc)  {ARGs a;
		     process(&a);
		     pbmac(pc,a);
		    }
	    else comand();
	    LIN=LINE;
	    LINE[0]='\0';
	   }
       }
  }
}
void inaction()
{if(LIN)	/*not EOF*/
  {if(!(*LIN)) fgets3();
   if(!LIN) return;	/*EOF*/
   if(*LIN != COMMAND) { /*text()*/ LIN=LINE; LINE[0]='\0';}
   else{CMD c_type;
	char arg_typ, *lptr;
	LIN++;
	c_type=comtyp1(LIN);
	if(c_type<UNKNOWN)	/*conditional grammar*/
	switch(c_type)
	   {case IF :	/*"if" conditional*/
		get_val2( LIN, &arg_typ, &lptr );
		LIN = lptr;
		while((*LIN==' ')||(*LIN=='\t')||(*LIN=='\n')) LIN++;
		inaction();
		break;
	    case IE :	/*"if-else" conditional*/
		get_val2( LIN, &arg_typ, &lptr );
		LIN = lptr;
		while((*LIN==' ')||(*LIN=='\t')||(*LIN=='\n')) LIN++;
		inaction();
		act2(FALSE);
		break;
	    case LEFT: /*.{ encountered */
		{char dummy[LSZ];
		 gwLIN(dummy);
		 skip_blanks();
		 while ((LIN[0]!='.')||(LIN[1]!='}'))
			{if(!LIN) {conderr("EOF"); return;}
			 inaction();
			}
		 gwLIN(dummy);
		 skip_blanks();
		} break;
	    case RIGHT: LIN--; break;
	    case EL:	LIN--; break;
	   }
	else
	   {LIN=LINE;
	    LINE[0]='\0';
	   }
       }
  }
}
int act2(int act) /*returns true if no error; act for .el action*/
{char dummy[LSZ];
 if(!LIN[0]) fgets3();
 if(!LIN)
	{fprintf(stderr,"Unexpected EOF encountered\n");
	 return(FALSE);
	}
 if((LIN[0]==COMMAND)&&(LIN[1]=='e')&&(LIN[2]=='l')) /*ok*/
	{gwLIN(dummy);
	 skip_blanks();
	 if(act) action();
	 else inaction();
	 return(TRUE);
	}
 else return(FALSE);
}
void conderr(char *err)/*non-fatal error reporting for conditional grammar*/
{if(LIN) fprintf(stderr,"Unexpected %s encountered;",err);
 else {fprintf(stderr, "Unexpected EOF encountered.\n"); return;}
 fprintf(stderr,"  ignoring <%s>.\n",(char *)LIN);
 LIN=LINE;
 LINE[0]='\0';
}
