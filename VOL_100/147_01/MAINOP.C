/***************************************************************
* MAINOP.C

Release 7: RBBS 4.1 Edit 02 - Added check to allow display only
			      if message is not deleted for
			      continuous display with "+" or "*".
			    - Skips Confirm message if any typeahead
			      after G command.
			    - Added test for % in gocpm.
Release 6: RBBS 4.1 Edit 01 - Fixed msgno display in killmsg
			    - Relaxed permissions for non-PERSONAL mail
Release 5: RBBS 4.1 Edit 00 - Names now one large field
			    - Activated lstmsg for 1st 7 non-PERSONALs
			    - Inserted NOSITE into showuser
			    - Inserted PERSONLY in readmsgs
Release 4: RBBS 4.0 Edit 21 - fixed restricted-user bug in gocpm
Release 1: RBBS 4.0 Edit 18

* This file includes the functions:

* readmsgs	calls tstnum, readmsg, tstaddr, prnt_sum, and
                readtxt if OK.

* summsgs	calls tstnum, readsum, tstaddr, and dispsum if OK

* killmsg	calls tstnum, readsum, tstaddr, dispsum, and
                ritesum if OK to delete

* goodbye	calls closing and hexit to hangup

* gocpm		calls closing and rexit

* tos		time in RBBS

* chgpasswd	calls chkpwd and updtuser

* help		calls bufinmsg to display HELP.CCC

* bulletins	calls bufinmsg to display BULLETINS.CCC

* showuser	called by maindispatch U command

* tstnum	prompts for msgno, checks for pause/quit

* tstaddr	checks to see if user is "authorized" to see the
                particular msg selected.

*****************************************************************/

#include	<bdscio.h>
#include	"rbbs4.h"

int	skipquit;
int	plus;
/***************************************************************/
readmsgs(fd,u,s)
int	fd;
struct	_user	*u;
struct	_sum	*s;
{
	int	i;
	int	flag;

	if ( !(i = tstnum("read",u)) )
		return;
	do
	{
		readsum(fd,mndx[i++],s);
		flag = TRUE;
		if ( personal )
			flag = tstaddr(u,s,FALSE);
		if ( flag && mno[i-1])
		{
			crlf(1);
#if	!PERSONLY
			if ( cchoice && (mno[i-1] > u->lstmsg[cchoice-1]) )
				u->lstmsg[cchoice-1] = mno[i-1];
#endif
			prnt_sum(s);
			crlf(1);
			readtxt(fd,mndx[i-1],s);
			if (!skipquit)
			    if ( (i <= msgct) && plus)
				if (!getyn("More"))
					i = msgct + 2;
		}
	} while ( (i <= msgct) && plus);
	plus = FALSE;
}
/***************************************************************/
summsgs(fd,u,s)
int	fd;
struct	_user	*u;
struct	_sum	*s;
{
	int	i;
	int	fftflg;
	int	flag;

	if ( !(i = tstnum("summarize",u)))
		return;
	fftflg = TRUE;
	do
	{
		readsum(fd,mndx[i++],s);
		flag = TRUE;
		if ( personal )
			flag = tstaddr(u,s,FALSE);
		if ( flag && mno[i-1])
		{
			if (fftflg && !expert)
			{
outstr(" No. Ct.    Date    From       -> To          Subject",2);
				crlf(1);
				fftflg = FALSE;
			}
			dispsum(s);
			if (!(i%22))
			{
				if (!skipquit)
					if ( (i <= msgct) && plus)
						if (!getyn("More"))
							return FALSE;
			}
		}
	} while ( (i <= msgct) && plus);
	plus = FALSE;
}
/***************************************************************/
killmsg(fd,u,s)
int	fd;
struct	_user	*u;
struct	_sum	*s;
{
	int	i;

	if ( !(i = tstnum("kill",u)))
		return;
	do
	{
		readsum(fd,mndx[i],s);
		if (tstaddr(u,s,TRUE) && mno[i])
		{
			dispsum(s);
			crlf(1);
			if (getyn("Delete it"))
			{
				s->mstat = FALSE;
				ritesum(fd,mndx[i],s);
				mno[i] = FALSE;
			}
		}
		else
		{
			outstr("You are not authorized to kill message #",3);
			sprintf(tmpstr,"%d",mno[i]);
			outstr(tmpstr,1);
			crlf(1);
		}
		++i;
	} while ( (i <= msgct) && plus);
	plus = FALSE;
}
/***************************************************************/
goodbye(fd,ufd,u,s)
int	fd,ufd;
struct	_user	*u;
struct	_sum	*s;
{
	if (!*sav)
		if ( !getyn("Confirm") )
			return fd;
	if (fd = closing(fd,ufd,u,s))
		return fd;
#if	DATETIME
	tos();
#endif
#if	HANGUP
	sprintf(tmpstr,"Goodbye %s.  Please call again!",u->nm);
	outstr(tmpstr,2);
#else
	outstr("Returning to CP/M...",2);
#endif
	hexit();		/* Hangup	*/
}
/***************************************************************/
gocpm(fd,ufd,u,s)
int	fd,ufd;
struct	_user	*u;
struct	_sum	*s;
{
	if ( (*(u->ustat) == '*') || (*(u->ustat) == '%') )
	{
		outstr("Access to CP/M has been denied.",2);
		return fd;
	}
	if (fd = closing(fd,ufd,u,s))
		return fd;
#if	DATETIME
	tos();
#endif
	if (!expert)
		bufinmsg("EXIT2CPM");
	rexit();
}
/***************************************************************/
#if	DATETIME
tos()
{
	char	dif[9];

	strcpy(dif,"00:00:00");
	get_date();
	sscanf(logdate+10,"%s ",tmpstr);
	sscanf(sysdate+10,"%s ",tmpstr+10);
	timdif(tmpstr,tmpstr+10,dif);
	sprintf(tmpstr,"Time in RBBS: %s",dif);
	outstr(tmpstr,1);
}
#endif
/***************************************************************/
chgpasswd(u,fd)
struct	_user	*u;
int	fd;
{
	chkpwd(u->pwd);
	updtuser(u,fd);
}
/***************************************************************/
help() 
{
	bufinmsg("HELP");
}
/***************************************************************/
bulletins()
{
	if (bufinmsg("BULLETINS"));
	else
		outstr("NO BULLETINS",2);
}
/****************************************************************/
showuser(fd,u)		/*CHECKS THE USER FILE & EXITS	*/
int	fd;			
struct	_user	*u;
{
	int	ndx;
	struct	_user	u2;
	char	temp[SECSIZ];

	for ( ndx = 1; ndx <= u->maxno; ndx++)
	{
		/* SPRINT CONSTANT INFORMATION */
		formrec(tmpstr,SECSIZ);
		readrec(fd,ndx,tmpstr,1);
		sscanf(tmpstr+NM,"%s",u2.nm);		/* NM:	*/
#if	(NOSITE && LASTREAD)
#else
		sscanf(tmpstr+FRM,"%s",u2.from);	/* FRM:	*/
#endif
		sscanf(tmpstr+LLG,"%s",u2.lastlog);	/* LLG:	*/
		sprintf(temp,"%-16s  Last Logon: %s",u2.nm,u2.lastlog);
#if	(NOSITE && LASTREAD)
#else
#if	DATETIME
		sprintf(tmpstr,"  from %-20s",u2.from);
#else
		sprintf(tmpstr,"  from %s",u2.from);
#endif
		strcat(temp,tmpstr);
#endif
		outstr(temp,1);
		if (!(ndx%22))
			if(!getyn("More"))
				ndx = u->maxno+1;
	}
}
/****************************************************************/
tstnum(msg,u)
char	*msg;
struct	_user	*u;
{
	char		input[6];
	int		i,n;
	unsigned	*addr;

	outstr("Message Number: ",5);
	instr("",input,6);
	crlf(2);
	addr = input + strlen(input)-1;
	plus = FALSE;
	if (*addr == '+' || *addr == '*')
	{
		skipquit = FALSE;
		if(*addr == '*')
			skipquit = TRUE;
		plus = TRUE;	
		*addr = 0;
	}
	if (!strcmp("summarize",msg))
		plus = TRUE;
	if ( cchoice && (*input == '#') )
		n = u->lstmsg[cchoice-1] + 1;
	else
		n = atoi(input);
	if (n > lastmsg)
	{
		outstr("Message no. out of range",1);
		return FALSE;
	}
    	i = 1;
	while (i <= msgct)
	{
		if (mno[i++] == n)
			return --i;
		if (i > msgct)
		{
			if (plus)
			{
				++n;
				i = 1;
				if (n > lastmsg)
					return FALSE;
			}
			else
				return FALSE;
		}
	}
}
/****************************************************************/
/* Checks to see if the user is authorized 			*/
/* to see the message and returns FALSE if not			*/

tstaddr(u,s,type)
struct	_user	*u;
struct	_sum	*s;
int	type;
{
	capstr(s->tnm);
	capstr(s->fnm);
	if (!s->mstat)
		return FALSE;
	if ( (sysop) || (!strcmp("ALL",s->tnm) && !type) )
		return TRUE;
	if ( !strcmp(u->nm,s->tnm) )
		return TRUE;
	if ( !strcmp(u->nm,s->fnm) )
		return TRUE;
	return FALSE;
}
/****************************************************************/
	}
	if (!strcmp("summarize",msg))
		plus = TRUE;
	if ( cch