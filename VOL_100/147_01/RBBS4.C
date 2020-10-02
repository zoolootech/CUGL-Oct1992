/****************************************************************
* RBBS4.C	 						*

Release 7: RBBS 4.1 Edit 02 - Added test for % MFLAG to disallow
			      E and K commands for such users.
Release 6: RBBS 4.1 Edit 01 - Allow "personal" messages everywhere
			    - Added non-display of summary if already
			      seen a msgfile once
			    - Bypass choice list if typeahead
			    - Added N command to simulate entry of "R;#+"
Release 5: RBBS 4.1 Edit 00 - Names now one large field
			    - Activated lstmsg for 1st 7 non-PERSONALs
			    - Inserted PERSONLY conditional
Release 4: RBBS 4.0 Edit 21 - if no PERSONAL msgs waiting, selects GENERAL
Release 1: RBBS 4.0 Edit 18

First C version of RBBS, inspired by RBBS31

Originally designed and written by John C. Gilbert.
Additional coding (mainly RBBSCIO.C) by Frank J. Wancho.

Principal Beta testing and numerous suggestions by Sigi Kluger,
with additional testing and help from David Schmidt and Ron
Fowler and others.

This program was originally designed to support a
restricted-access, multi-user TurboDOS RCP/M system.

This source is herewith released into the public domain with the
understanding that bug reports and revisions be submitted to the
LAZARUS RCP/M (915-544-1432) or via DDN to WANCHO@SIMTEL20.

(See any DRI license agreement for any other disclaimers that may apply
to this program.)

* Functions included in this file:

* main		calls logon, loguser, select_MSG, and maindispatch

* compuser	called by checkmsgs to check if current msg is
		for this user

* maindispatch	main command dispatcher

* select_MSG	called by main and by maindispatch if F command.
		Offers subject files for selection and calls
                checkmsgs

* checkmsgs	loops thru current message file, calling compuser
                on each entry.

****************************************************************/
#include	<bdscio.h>
#include	"rbbs4.h"

	int	privct;
	int	fflags[15];

main()
{
	int	userfd,msgfd;
	char	rflg;
	struct	_sum	s;
	struct	_user	u;

	setmem(fflags,15,FALSE);
	outstr(VERSION,1);
	belflg = FALSE;
	expert = FALSE;
	rtnok = FALSE;
	poke(0,0xcd);		/* Change JMP at 0 to CALL		*/
	rflg = peek(0x5d);	/* Get first char of command line	*/
	poke(0x5d,0x20);	/* Clear that first char		*/
	if ( (rflg == 'P') && (peek(0x5b) == 'x') )
		rtnok = TRUE;	/* Set flag if a rerun			*/
	poke(0x5b,0x78);	/* Set flag loc for possible return	*/
	if (!rtnok)
		bulletins();
	userfd = openfile(DSK(USERS.CCC));
#if	!DATETIME
	get_date();
	strcpy(logdate,sysdate);
#endif
	logon(userfd,u);
	loguser(u);
	crlf(1);
	setptr(m,lp);
	fileflag = FALSE;
	msgfd = select_MSG(msgfd,1,u,s);	/* Select PERSONAL	*/
#if	!PERSONLY
	if (!privct)				/* if none,		*/
	{
		fileflag = TRUE;
		msgfd = select_MSG(msgfd,2,u,s);/* select GENERAL	*/
	}
#endif
	maindispatch(msgfd,userfd,u,s);
}
/***************************************************************/
compuser(ct,u,s)
int	ct;
struct	_user	*u;
struct	_sum	*s;
{
	capstr(s->tnm);
	if ( !strcmp(u->nm,s->tnm) || (sysop  && !strcmp("SYSOP",s->tnm)) )
	{
		if(!ct)
		{
			outstr("YOU HAVE MAIL:",1);
			crlf(1);
		}
		dispsum(s);
		return ++ct;
	}
	else	return ct;
}
/***************************************************************/
maindispatch(fd,ufd,u,s)
int	fd,ufd;
struct	_user	*u;
struct	_sum	*s;
{
	int	cmd;
	char	cmd2[2];

	cmd = FALSE;
	while(TRUE)
	{
		while(cmd)
		{
			switch(cmd)
			{
				case 'E':
					if (*u->ustat == '%')
						break;
					entermsg(fd,ufd,u,s,FALSE);
					break;

				case 'R':
					readmsgs(fd,u,s);
					break;

				case 'S':
					summsgs(fd,u,s);
					break;

				case 'K':
					if (*u->ustat == '%')
						break;
					killmsg(fd,u,s);
					break;

				case 'G':
					fd = goodbye(fd,ufd,u,s);
					break;

				case 'W':
					welcome();
					break;

				case 'C':
					fd = gocpm(fd,ufd,u,s);
					break;

#if	!PERSONLY
				case 'F':
					if (close(fd) == ERROR)
						ioerr("Closing MSG");
					fileflag = TRUE;
					fd = select_MSG(fd,0,u,s);
					break;
#endif

				case 'U':
#if	NOUCMD
					if (!sysop)
						break;
#endif
					crlf(1);
					showuser(ufd,u);
					break;

				case 'T':
					belflg = !belflg;
					break;

				case 'X':
					expert = !expert;
					break;

				case 'B':
					bulletins();
					break;

				case 'P':
					chgpasswd(u,ufd);
					break;

				case 'M':
					msg_info(msgct,TRUE);
#if	!PERSONLY
					himsg(u);
#endif
#if	DATETIME
					crlf(1);
					tos();
#endif
					break;

				case 'N':
					strcpy(sav,"#+");
					readmsgs(fd,u,s);
					break;

				case '?':
					help();
					break;

				default:
					if(sysop)
					{
						crlf(1);
						lstuser();
					}
					break;
			}
			cmd = FALSE;
		}
		strcpy(tmpstr,"COMMAND: ");
		if (!expert)
		{
			strcat(tmpstr,"C,E,");
#if	!PERSONLY
			strcat(tmpstr,"F,");
#endif
			strcat(tmpstr,"G,K,R,S,N,b,m,p,t,");
#if	NOUCMD
			strcat(tmpstr,"w,x (or ? for help): ");
#else
			strcat(tmpstr,"u,w,x (or ? for help): ");
#endif
		}
		outstr(tmpstr,4);
		instr("",cmd2,1);
		cmd = toupper(*cmd2);
		crlf(1);
	}
}
/****************************************************************/
select_MSG(msgfd,choice,u,s)
int	msgfd,choice;
struct	_user	*u;
struct	_sum	*s;
{
	int	fd,n;
	char	input[3];
	char	rec[SECSIZ];
	int	pflag;

#if	!PERSONLY
	fd = openfile(DSK(SUBJECTS.CCC));
	if (readrec(fd,0,rec,1) == ERROR)
		ioerr("reading subject");
	sscanf(rec,"%d",&n);
	while ( (!choice) || (choice > n) )
	{
		if (!*sav)
		{
			outstr("Subjects are:",2);
			displ_subj(fd,n,rec);
		}
		outstr("File number: ",4);
		instr("",input,3);
		crlf(1);
		choice = atoi(input);
	}
	close(fd);	
	if ( choice > 8)
		cchoice = 0;
	else
		cchoice = choice - 1;
	setmem(cmsgfile,9,0);
	movmem(rec+8*choice,cmsgfile,8);
#else
	cchoice = 0;
	strcpy(cmsgfile,"PERSONAL");
#endif
	sprintf(msgfile,"%s%s%s",DRIVE,capstr(cmsgfile),".MSG");
	msgct = load_cntrl(msgct,s);
	pflag = TRUE;
	if ( fflags[choice])
		pflag = FALSE;
	msg_info(msgct,pflag);
	msgfd = openfile(msgfile);
	personal = !strcmp(cmsgfile,"PERSONAL");
	if ( actmsg && !fflags[choice])
	{
		sprintf(tmpstr,"Checking the %s Messages File...",cmsgfile);
		outstr(tmpstr,3);
		checkmsgs(msgfd,u,s);
	}
	fflags[choice] = TRUE;
#if	!PERSONLY
	himsg(u);
#endif
	return msgfd;
}
/***************************************************************/
#if	!PERSONLY
himsg(u)
struct	_user	*u;
{
	if (cchoice)
	{
sprintf(tmpstr,"Highest message number seen in this file is %d.",u->lstmsg[cchoice-1]);
		outstr(tmpstr,2);
	}
}
#endif
/***************************************************************/
checkmsgs(fd,u,s)
int	fd;
struct	_user	*u;
struct 	_sum	*s;
{
	int	i;

	privct = 0;
	for ( i = 1; i <= msgct; i++)
	{
		if (mno[i])
		{
			readsum(fd,mndx[i],s);
			privct = compuser(privct,u,s);
		}
	}
	if (privct)
	{
		outstr("PLEASE READ AND KILL ",3);
		sprintf(tmpstr,"TH%s MESSAGE%s",(privct==1)?"IS":"ESE",(privct==1)?".":"S.");
		outstr(tmpstr,1);
	}
	else
		outstr("No new mail for you.",1);
}
/***************************************************************/
ice])
	{
		sprintf(tmpstr,"Checking the %s Messages File...",cmsgfile);
		outstr(tm