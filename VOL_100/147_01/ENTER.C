/****************************************************************
* ENTER.C							*

Release 7: RBBS 4.1 Edit 02 - Better prompt in entermsg
Release 6: RBBS 4.1 Edit 01 - Fixed edit problem in edit_txt
			    - Added L command option to edit_txt
			    - Allow "personal" messages everywhere
			    - Cannot send to self anymore
Release 5: RBBS 4.1 Edit 00 - Names now one large field
			    - Inserted PERSONLY in check_to
			    - Fixed potential problem in save_draft
Release 1: RBBS 4.0 Edit 18

* This file contains the functions:

* entermsg	calls check_to, get_subj, get_text, and edit_draft

* check_to	prompts for To:, calls checkuser

* get_subj	prompts for Subject string

* get_text	prompts for up to 24 text lines

* edit_draft	command dispatch

* save_draft	appends the message to current .MSG file

* edit_sum	offers to change the To: and Subject: lines

* edit_txt	prompts for line number to change and offers the
                line for editting

****************************************************************/
#include	<bdscio.h>
#include	"rbbs4.h"

/***************************************************************/
entermsg(fd,ufd,u,s,tosys)	/* ROUTINE TO CONTROL MSG ENTRY*/
int	fd,ufd;
struct	_user	*u;
struct	_sum	*s;
int	tosys;
{
	s->msgno = ++lastmsg;;
	s->mstat = TRUE;
#if	DATETIME
	get_date();
#endif
	strcpy(s->date,sysdate);
	if (tosys)
		strcpy(s->tnm,"SYSOP");
	else
		if (!check_to(ufd,s,u,TRUE))
			return;
	strcpy(s->fnm,u->nm);
	if (tosys)
		strcpy(s->subj,"MSG FOR SYSOP");
	else
	{
		*(s->subj) = 0;
		get_subj(s);
	}
	s->lct = get_text(s,1);
	edit_draft(fd,ufd,u,s);
}
/***************************************************************/
check_to(fd,s,u,flag)	/* GETS THE NAME OF THE MSG RECIPIENT	*/
int	fd,flag;
struct	_user	*u;
struct	_sum	*s;	/* CHECKS FOR VALID USER - PERMITS 	*/
{			/* OVERRIDE - PERMITS ALL		*/

	struct	_user	x;
	char	name[NAMELGTH+1];

#if	!PERSONLY
	if (!expert && flag && personal)
	{
outstr("The current message file, 'PERSONAL' is for private mail only.",2);
outstr("If you wish to send to a specific individual and you don't care",2);
outstr("who can read it (or send to 'ALL'), use the F Command to select",1);
outstr("the 'GENERAL' or some other message file at the COMMAND prompt.",1);
		crlf(1);
		outstr("To abort, hit RETURN.",1);
		outstr("Enter To:",3);
	}
#endif
	do
	{
		crlf(1);
		if ( getname(name,"To:        "))
		{
			if (personal)
			{
				crlf(1);
				return FALSE;
			}
			else
			{
				outstr("ALL",0);
				strcpy(s->tnm,"ALL");
				return(TRUE);
			}

		}
		if ( !strcmp(name,u->nm))
		{
			outstr("Can't send to yourself!",3);
			crlf(1);
			*sav = 0;
			break;
		}
		if ( !checkuser(fd,name,x,0) )
		{
			crlf(1);
			sprintf(tmpstr," %s",name);
			strcat(tmpstr," is NOT a user of this system.");
#if	SENDOK
			strcat(tmpstr,"  Use anyway");
			if ( getyn(tmpstr))
			{
				strcpy(s->tnm,name);
				return TRUE;
			}
#else
			strcat(tmpstr,"  Please try again.");
			outstr(tmpstr,2);
#endif
		}
		else
		{
			strcpy(s->tnm,name);
			return TRUE;
		}
	} while (TRUE);
}
/***************************************************************/
get_subj(s)		/* GETS SUBJECT - VARIABLE LENGTH	*/
struct	_sum	*s;
{
	do
	{
		outstr("Subject:   ",4);
		instr(s->subj,s->subj,SUMSIZE-1);
	}	while (!strlen(s->subj));
	crlf(1);
}
/***************************************************************/
get_text(s,i)		/* GETS TEXT FOR MESSAGE - UP TO 24 	*/
struct	_sum	*s;	/* LINES				*/
int	i;
{
	int	j;

	setptr(m,lp);
	strcpy(tmpstr,"Enter Text");
	if (!expert)
		strcat(tmpstr," (?<cr> on a new line for help)");
	strcat(tmpstr,":");
	outstr(tmpstr,2);
#if	RULER
	if (!expert)
		outstr("     ---------------------------------------------------------------",1);
#endif
	setmem(sav,SECSIZ,0);
	for( j = i; j <= MLINELIM; j++)
	{
		sprintf(tmpstr,"%2d>: ",j);
		outstr(tmpstr,4);
		cflg = 1;
		instr("",lp[j],MLINESIZ-1);
#if	PCREND
		if ( (strlen(lp[j]) == 1) && (*lp[j] == '.') )
#else
		if ( !*(lp[j]) )
#endif
		{
			crlf(1);
			return --j;
		}
		if ( (strlen(lp[j]) == 1) && (*lp[j] == '?') )
		{
			crlf(1);
			bufinmsg("MSGHELP");
			outstr("Hit RETURN to resume input: ",0);
			cflg = 3;
			instr("",tmpstr,1);
			s->lct = --j;
			prnt_txt(s,TRUE);
		}
		if (j >= MLINELIM-3)
		{
			if (belflg)
				bputs("\07");
			*tmpstr = 0;
			if (j == MLINELIM)
				strcpy(tmpstr,"No ");
			else
				sprintf(tmpstr,"%d ",MLINELIM-j);
			strcat(tmpstr,"line");
			if (MLINELIM - j - 1)
				strcat(tmpstr,"s");
			strcat(tmpstr," left!");
			outstr(tmpstr,3);
		}
	}
	crlf(1);
	return(MLINELIM);
}
/***************************************************************/
edit_draft(fd,ufd,u,s)	/* EDIT DRAFT MESSAGE	*/
int	fd,ufd;
struct	_user	*u;
struct	_sum	*s;
{
	int	cmd;
	char	cmd2[2];

	cmd = FALSE;
	while (TRUE)
	{
		switch(cmd)
		{
			case 'H':
				edit_sum(ufd,s,u);
				break;

			case 'L':
				crlf(1);
				prnt_sum(s);
				prnt_txt(s,TRUE);
				break;

			case 'S':
				if (save_draft(fd,s))
					return;
				break;

			case 'R':
				if (!personal)
					break;
				if (!save_draft(fd,s))
					break;
				if (!expert)
outstr("Use 'A' to Abort, or 'H' to edit 'To:', then 'S' or 'R' to Store again.",2);
				s->msgno = ++lastmsg;
				break;

			case 'C':
				s->lct = get_text(s,s->lct+1);
				break;

			case 'E':
				edit_txt(s);
				break;

			case 'A':
				if (getyn("Abort EnterMsg"))
					return;
				else
					break;

			case 'X':
				expert = !expert;
				break;

			case '?':
				crlf(1);
				bufinmsg("SELHELP");
				break;

			default:
				break;
		}
	strcpy(tmpstr,"SELECT: ");
	if (!expert)
		strcat(tmpstr,"H, L, E, C, A, R, S, X, or ?: ");
	outstr(tmpstr,4);
	instr("",cmd2,1);
	crlf(1);
	cmd = *cmd2 = toupper(*cmd2);
	}
}
/***************************************************************/
save_draft(fd,s)	/* WRITES NEW MSG TO MESSAGE.CCC & 	*/
int	fd;
struct	_sum	*s;
{		
	struct	_sum	s2;

	if (!s->lct)
	{
		outstr("Cannot Store an empty message!",1);
		outstr("Use either A)bort to quit, or",1);
		outstr("C)continue to add text.",1);
		return FALSE;
	}
#if	LOCKEM
	if (lokrec(fd,-1))
		ioerr("locking .MSG file");
#endif
	msgct = loadsum(fd,s2); /* Loads last msg no if updated	*/
	msg_info(msgct,FALSE);
	s->msgno = ++lastmsg;
	++msgct;
	mndx[msgct+1] = ritemsg(fd,mndx[msgct],s,lp[1]);
	mno[msgct] = s->msgno;
	mno[msgct+1] = 0;
#if	LOCKEM
	frerec(fd,-1);
#endif
	close(fd);
	fd = open(msgfile,2);
	sprintf(tmpstr,"Message stored as #%d.",s->msgno);
	outstr(tmpstr,1);
	return TRUE;
}
/***************************************************************/
edit_sum(ufd,s,u)		/* EDITS THE HEADER PORTION OF THE MSG */
int	ufd;
struct	_user	*u;
struct _sum	*s;
{
	crlf(1);
	sprintf(tmpstr,"To: %s OK",s->tnm);
	if (!getyn(tmpstr))
	{
		check_to(ufd,s,u,FALSE);
		crlf(1);
	}
	sprintf(tmpstr,"Re: %s OK",s->subj);
	if (!getyn(tmpstr))
		get_subj(s);
}
/***************************************************************/
edit_txt(s)		/* EDITS THE TEXT PORTION OF MSG	*/
struct	_sum	*s;
{
	int	i;
	int	j;
	char	input[4];

	do
	{
		if (!expert)
			strcpy(tmpstr,"Enter l");
		else
			strcpy(tmpstr,"L");
		strcat(tmpstr,"ine number");
		if (!expert)
			strcat(tmpstr," to edit, or 0 to exit");
		strcat(tmpstr,": ");
		outstr(tmpstr,4);
		instr("",input,3);
		if ( toupper(*input) == 'L')
		{
			crlf(1);
			prnt_txt(s,TRUE);
		}
		else
		{
			i = atoi(input);
			setmem(sav,SECSIZ,0);
			if (!i)
			{
				crlf(1);
				break;
			}
			if ( abs(i) > 0 && abs(i) <= s->lct)
			{
				if ( i < 0)
				{
					i = abs(i);
					if ( (s->lct)++ < MLINELIM)
						strcpy(lp[s->lct],lp[(s->lct)-1]);
					for (j = (s->lct)-1; j > i; j--)
						strcpy(lp[j],lp[j-1]);
					if ( (s->lct) > MLINELIM )
						s->lct = MLINELIM;
					*lp[i] = 0;
				}
				sprintf(tmpstr,"%2d>: ",i);
				outstr(tmpstr,4);
				cflg = 1;
				instr(lp[i],lp[i],MLINESIZ-1);
#if	PCREND
				if ( (strlen(lp[i]) == 1) && (*lp[i] == '.') )
#else
				if ( !(*lp[i]) )
#endif
				{
					outstr("\bLine deleted",0);
					for (j = i; j < s->lct; j++)
						strcpy(lp[j],lp[j+1]);
					--(s->lct);
				}
			}
			else
				outstr(" is not a valid line number",1);
			crlf(1);
		}
	} while (TRUE);
}
/***************************************************************/
,input,3);
		if ( toupper(*input) == 'L')
		{
			crlf(1);
			prnt_txt(s,TRUE);
		}
		else
		{