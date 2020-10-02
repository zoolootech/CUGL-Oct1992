/****************************************************************
* LOGON.C

Release 7: RBBS 4.1 Edit 02 - Changed getname calls to add prompts.
Release 6: RBBS 4.1 Edit 01 - if RBBS P used, keeps original logtime.
 			    - also added <cr> to LASTCALR file.
Release 5: RBBS 4.1 Edit 00 - Names now one large field
			    - Activated lstmsg for 1st 7 non-PERSONALs
			    - Inserted NOSITE into newuser
			    - Inserted PERSONLY in closing
			    - Fixed duplicate user entry problem
			      by closing and reopening USERS.CCC
			      in newuser - now immune to lost carrier
			      which caused the problem!
Release 4: RBBS 4.0 Edit 21 - changed sysop flag to "!" in logon
			    - forced-truncated names in getlast
			    - added bias in loguser
Release 3: RBBS 4.0 Edit 20 - fixed LASTREAD bug in getlast
Release 1: RBBS 4.0 Edit 18

* This file contains the functions:

* logon		calls getname, checkuser, testpwd, updtuser, and
                maybe newuser ... in general, logs the user into
                the program and validates the user.

* checkuser	checks the entry in USERS.CCC

* chkpwd	prompts for a new password

* testpwd	prompts user for login password and checks it

* newuser	registers a new user

* updtuser	updates the current user entry in USERS.CCC

* loguser	makes an entry in CALLERS.CCC

* lstuser	sysop only - lists CALLERS.CCC in reverse order

* welcome	calls bufinmsg to display WELCOME.CCC

* closing	closes session.  Called by gocpm and goodbye.
                Warns if only PERSONAL examined, offers others,
                and offers to leave msg for sysop.  Closes .MSG
                and USERS.CCC

****************************************************************/
#include	<bdscio.h>
#include	"rbbs4.h"

/****************************************************************/
logon(fd,u)		/* Logon gets the callers name		*/
			/* pwd and checks the users file	*/
			/* If the user is a new user, it gets	*/
			/* add'l info & adds to the user file.	*/
			/* It sets the user struct.		*/
int	fd;
struct	_user	*u;
{
	int	valid_user, i;
	int	*p;
	char	name[NAMELGTH+1];

	valid_user = FALSE;
	i = 0;
	p = 0x59;
#if	DATETIME
	get_date();
	strcpy(logdate,sysdate);
#endif
	do
	{
		u->recno = 0;
#if	LASTREAD
		rtnok = getlast(u,fd,name);
#endif
		if (!rtnok)
			while ( (getname(name,"Enter your name: ")) )
				crlf(1);
		else
			u->recno = *p;
		if (checkuser(fd,name,u,u->recno))
		{
			if (*(u->ustat) == '#')
			{
				bufinmsg("TWITMSG");
				hexit();
			}
			belflg = u->bellflag;
			expert = u->xpert;
			if (!rtnok)
				testpwd(u);
			if ( (!strcmp(u->nm,"SYSOP")) || (*(u->ustat) == '!'))
				sysop = TRUE;
			sprintf(tmpstr,"Hello again, %s!!",u->nm);
			outstr(tmpstr,2);
			sprintf(tmpstr,"Last logon: %s",u->lastlog);
			outstr(tmpstr,1);
			sprintf(tmpstr,"This logon: %s",logdate);
			outstr(tmpstr,1);
			strcpy(u->lastlog,logdate);
			if (!rtnok)
				strcpy(u->lastlog,logdate);
			else
				strcpy(logdate,u->lastlog);
			updtuser(u,fd);
			valid_user = TRUE;
		}
		else
		{
#if	!PREREG
			valid_user = newuser(u,name,fd);
#endif
			if (++i > 2)
				flushcaller();
			else
				crlf(1);
		}
	} while (!valid_user);
}
/****************************************************************/
checkuser(fd,name,u,n)		/*Checks the user file & exits */
int	fd,n;			/* TRUE if a valid user after	*/
char	*name;			/* loading a user structure	*/
struct	_user	*u;
{
	int	ndx,m;

	formrec(tmpstr,SECSIZ);
	readrec(fd,0,tmpstr,1);
	sscanf(tmpstr,"MAX REC: %d",&u->maxno);
	if ( !(m = n) )
		++m;
	for (ndx = m; ndx <= u->maxno; ndx++)
	{
		formrec(tmpstr,SECSIZ);
		readrec(fd,ndx,tmpstr,1);
		sscanf(tmpstr+NM,"%s",u->nm);			/* NM:	*/
		capstr(u->nm);
		if( n || !strcmp(name,u->nm) )
		{
			u->recno = ndx;
			sscanf(tmpstr+PRV,"%s",u->ustat);	/* PRV:	*/
			sscanf(tmpstr+UBL,"%d",&u->bellflag);	/* UBL:	*/
			sscanf(tmpstr+UXP,"%d",&u->xpert);	/* UXP:	*/
			sscanf(tmpstr+PWD,"%s",u->pwd);		/* PWD:	*/
			sscanf(tmpstr+LLG,"%s",u->lastlog);	/* LLG:	*/
			sscanf(tmpstr+FRM,"%s",u->from);	/* FRM:	*/
			sscanf(tmpstr+LMG0,"%d",&u->lstmsg[0]);	/* LMG0:*/
			sscanf(tmpstr+LMG1,"%d",&u->lstmsg[1]);	/* LMG1:*/
			sscanf(tmpstr+LMG2,"%d",&u->lstmsg[2]);	/* LMG2:*/
			sscanf(tmpstr+LMG3,"%d",&u->lstmsg[3]);	/* LMG3:*/
			sscanf(tmpstr+LMG4,"%d",&u->lstmsg[4]);	/* LMG4:*/
			sscanf(tmpstr+LMG5,"%d",&u->lstmsg[5]);	/* LMG5:*/
			sscanf(tmpstr+LMG6,"%d",&u->lstmsg[6]);	/* LMG6:*/
			return TRUE;
		}
	}
	return FALSE;
}
/****************************************************************/
chkpwd(n)		/* Gets a new password and returns it	*/
char	*n;
{
	int	flag;
	char	check[7];

#if	PWCASE
	outstr("WARNING: Case of password characters is significant!",2);
#endif
	do
	{
		outstr("Enter new password <6 chars>:  ",4);
		do
		{
			cflg = 2;
			instr("",n,6);
		} while (!*n);
		outstr("Enter the new password again:  ",3);
		do
		{
			cflg = 2;
			instr("",check,6);
		} while (!*check);
#if	!PWCASE
		upstr(n);
		upstr(check);
#endif
		if (flag = strcmp(n,check))
			outstr("No match.  Try again.",3);
		crlf(1);
	} while (flag);
}
/****************************************************************/
testpwd(u)		/* Permits up to 3 tries at entering 	*/
struct	_user	*u;	/* password, then exits			*/
{
	int	i;
	char	input[7];

	setmem(sav,SECSIZ,0);
	for ( i = 0; i < 3; i++)
	{
		outstr("Password:  ",4);
		cflg = 2;
		instr("",input,6);
#if	!PWCASE
		upstr(input);
#endif
		if (!strcmp(input,u->pwd))
		{
			outstr(" [OK]",1);
			return;
		}
		sprintf(tmpstr," WRONG!, %d tr%s left",2-i,(i==1)?"y":"ies");
		outstr(tmpstr,0);
	}
	flushcaller();
}
/****************************************************************/
flushcaller()
{
	crlf(1);
	outstr("SORRY, TOO MANY FAILURES!  GOODBYE",2);
	hexit();	/* Hangup	*/
}
/****************************************************************/
#if	!PREREG
newuser(u,name,fd)		/*DOUBLE CHECKS NEW USER & GETS	*/
struct	_user	*u;		/*INFO TO ADD NEW USER. ADDS	*/
char	*name;			/*NEW USER TO FILE. DOES NOT	*/
int	fd;			/*CLOSE USER FILE		*/
{
	int	userstatus;
	int	correct;
	char	place[SITELGTH];
	char	passwd[7];
#if	LOCKEM
	char	temp[SECSIZ];
#endif

	correct = FALSE;
#if	!LASTREAD
	sprintf(tmpstr,"!!WELCOME %s!!",name);
	outstr(tmpstr,2);
	crlf(1);
	userstatus = getyn("Is the name correct");
	if (!userstatus)
		return(FALSE);	
#endif
#if	NOSITE && LASTREAD
	strcpy(place," ");
#else
	do
	{
		sprintf(tmpstr,"Enter your %s: ",SITEPROMPT);
		outstr(tmpstr,4);
		instr("",place,SITELGTH);
		capstr(place);
		crlf(1);
		sprintf(tmpstr,"Is the name, %s, correct",place);
		correct = getyn(tmpstr);
	}while(!correct);
#endif
#if	LASTREAD
	strcpy(passwd,"123456");
#else
	chkpwd(passwd);
#endif
	welcome();

	formrec(tmpstr,SECSIZ);
	sprintf(tmpstr+NM,"%s",name);		/* NM:	*/
	sprintf(tmpstr+PRV,"%c%c",NEWFLAG,0);	/* PRV:	*/
	sprintf(tmpstr+UBL,"%1d",belflg);	/* UBL:	*/
	sprintf(tmpstr+UXP,"%1d",expert);	/* UXP:	*/
	sprintf(tmpstr+PWD,"%s",passwd);	/* PWD:	*/
	sprintf(tmpstr+LLG,"%s",logdate);	/* LLG:	*/
	sprintf(tmpstr+FRM,"%s",place);		/* FRM:	*/
	sprintf(tmpstr+LMG0,"%03d",0);		/* LMG0:*/
	sprintf(tmpstr+LMG1,"%03d",0);		/* LMG1:*/
	sprintf(tmpstr+LMG2,"%03d",0);		/* LMG2:*/
	sprintf(tmpstr+LMG3,"%03d",0);		/* LMG3:*/
	sprintf(tmpstr+LMG4,"%03d",0);		/* LMG4:*/
	sprintf(tmpstr+LMG5,"%03d",0);		/* LMG5:*/
	sprintf(tmpstr+LMG6,"%03d",0);		/* LMG6:*/
#if	LOCKEM
	formrec(temp,SECSIZ);
	if (lokrec(fd,-1))
		ioerr("locking USERS.CCC");
	readrec(fd,0,temp,1);
	sscanf(temp,"MAX REC: %d",&u->maxno);
#endif
	Rriterec(fd,++u->maxno,1,tmpstr);

	formrec(tmpstr,SECSIZ);
	sprintf(tmpstr,"MAX REC: %d",u->maxno);
	Rriterec(fd,0,1,tmpstr);
#if	LOCKEM
	frerec(fd,-1);
#endif
	close(fd);
	fd = openfile(DSK(USERS.CCC));
	/* RELOAD THE CURRENT USER TO BE THE NEW USER */
	checkuser(fd,name,u,u->maxno);
	return (TRUE);
}
#endif
/****************************************************************/
updtuser(u,fd)
struct	_user	*u;
int	fd;
{
	formrec(tmpstr,SECSIZ);
	sprintf(tmpstr+NM,"%s",u->nm);			/* NM:	*/
	sprintf(tmpstr+PRV,"%s",u->ustat);		/* PRV:	*/
	sprintf(tmpstr+UBL,"%1d",belflg);		/* UBL:	*/
	sprintf(tmpstr+UXP,"%1d",expert);		/* UXP:	*/
	sprintf(tmpstr+PWD,"%s",u->pwd);		/* PWD:	*/
	sprintf(tmpstr+LLG,"%s",u->lastlog);		/* LLG:	*/
	sprintf(tmpstr+FRM,"%s",u->from);		/* FRM:	*/
	sprintf(tmpstr+LMG0,"%03d",u->lstmsg[0]);	/* LMG0:*/
	sprintf(tmpstr+LMG1,"%03d",u->lstmsg[1]);	/* LMG1:*/
	sprintf(tmpstr+LMG2,"%03d",u->lstmsg[2]);	/* LMG2:*/
	sprintf(tmpstr+LMG3,"%03d",u->lstmsg[3]);	/* LMG3:*/
	sprintf(tmpstr+LMG4,"%03d",u->lstmsg[4]);	/* LMG4:*/
	sprintf(tmpstr+LMG5,"%03d",u->lstmsg[5]);	/* LMG5:*/
	sprintf(tmpstr+LMG6,"%03d",u->lstmsg[6]);	/* LMG6:*/
	Rriterec(fd,u->recno,1,tmpstr);
}
/****************************************************************/
loguser(u)		/* LOG LAST CALLER TO CALLER FILE	*/
struct	_user	*u;	
{
	int	fd,ct,maxct,bias;
	int	*p;
	char	fname[NAMELGTH+1];
	char	lname[NAMELGTH+1];

	p = 0x59;
	fd = openfile(DSK(CALLERS.CCC));
#if	LOCKEM
	if (lokrec(fd,-1))
		ioerr("locking USERS.CCC");
#endif
	readrec(fd,0,tmpstr,1);
	sscanf(tmpstr,"MAX CALR: %d %d",&maxct,&bias);
	ct = (maxct%2)*(SECSIZ/2);
	readrec(fd,(maxct/2)+1,tmpstr,1);
	formrec(tmpstr+ct,SECSIZ/2);
	sprintf(tmpstr+ct," %s",sysdate);
	sprintf(tmpstr+ct+32,":%s",u->nm);
	/* If 1st rec in blk, clear the balance of block */
	if (!ct)
		formrec(tmpstr+SECSIZ/2,SECSIZ/2);
	Rriterec(fd,(maxct/2)+1,1,tmpstr);
	formrec(tmpstr,SECSIZ);
	sprintf(tmpstr,"MAX CALR: %d %d",++maxct,bias);
	Rriterec(fd,0,1,tmpstr);
#if	LOCKEM
	frerec(fd,-1);
#endif
	close(fd);
	*p = u->recno;
	sprintf(tmpstr,"You are caller #%d",maxct+bias);
	outstr(tmpstr,3);
#if	LASTWRITE
	if ( (fd = open(LASTW,2)) == ERROR)
		if ( (fd = creat(LASTW)) == ERROR)
			ioerr("Cannot create LASTCALR");
	formrec(tmpstr,SECSIZ);
	if ( (index(u->nm," ")) != ERROR)
		sscanf(u->nm,"%s %s",fname,lname);
	else
	{
		strcpy(fname,u->nm);
		*lname = 0;
	}
	sprintf(tmpstr,"%s,%s,%s\r\n%c",fname,lname,logdate,CPMEOF);
	Rriterec(fd,0,1,tmpstr);
	close(fd);
#endif
}
/****************************************************************/
lstuser()		/* LIST CALLER FILE IN REVERSE SEQ	*/
{
	int	i,fd,maxct;
	char	rec[SECSIZ];

	fd = openfile(DSK(CALLERS.CCC));/* Opening for read only (0)	*/
	readrec(fd,0,rec,1);
	sscanf(rec,"MAX CALR: %d",&maxct);
	sprintf(tmpstr,"Max Count: %d",maxct);
	outstr(tmpstr,1);
	readrec(fd,(maxct/2)+1,rec,1);
	if (*(rec+64+1) != ' ')
		sprintf(tmpstr,"%s   %s\r\n%s   %s",rec+64,rec+96,rec,rec+32);
	else
		sprintf(tmpstr,"%s   %s",rec,rec+32);
	if (*(rec+SECSIZ-2) == 0x0d)
		outstr(tmpstr,1);
	for ( i = maxct-2; i > 0; i -= 2)
	{
		readrec(fd,(i/2)+1,rec,1);
		sprintf(tmpstr,"%s   %s\r\n%s   %s",rec+64,rec+96,rec,rec+32);
		outstr(tmpstr,1);
		if(!(i%11))
			if(!getyn("More"))
				i = 0;
	}	
	close(fd);
}
/****************************************************************/
welcome()
{
	bufinmsg("WELCOME");
}
/***************************************************************/
closing(fd,ufd,u,s)
int	fd,ufd;
struct	_user	*u;
struct	_sum	*s;
{
	crlf(1);
#if	!PERSONLY
	if ( (!fileflag) && (!expert) )
	{
		if (!getyn("Just checking PERSONAL mail"))
			return fd;
	}
#endif
	if ( (!expert) && (!sysop) )
	{
		if(!getyn("SKIP leaving a msg for the SYSOP"))
		{
			if (close(fd) == ERROR)
				ioerr("Closing MSG file");
			fd = select_MSG(fd,1,u,s);
			entermsg(fd,ufd,u,s,TRUE);
		}
		crlf(1);
	}
	updtuser(u,ufd);
	if (close(fd) == ERROR)
		ioerr("Closing MSG file");
	if (close(ufd) == ERROR)
		ioerr("Closing USER file");
	return FALSE;
}
/***************************************************************/
#if	LASTREAD
getlast(u,fd,name)
struct	_user	*u;
int	fd;
char	name[NAMELGTH+1];
{
	int	lfd;
	int	*p;
	char	fname[NAMELGTH+1];
	char	lname[NAMELGTH+1];
	char	dummy[2*NAMELGTH+1];

	p = 0x59;
	if ( (lfd = open(LASTR,0)) == ERROR)
		return FALSE;
	readrec(lfd,0,tmpstr,1);
	if (*tmpstr == '~')
		return FALSE;
	if ( (index(tmpstr,",")) != ERROR)
	{
		sscanf(tmpstr,"%s,%s,",fname,lname);
		sprintf(dummy,"%s %s",fname,lname);
		*(dummy+NAMELGTH+1) = 0;
		strcpy(name,dummy);
		capstr(name);
	}
	else
		return FALSE;
	*p = 0;
	if (checkuser(fd,name,u,0))
		*p = u->recno;
	return TRUE;
}
#endif	
/***************************************************************/
ioerr("Closing MSG file");
			fd = select_MSG(fd,1,u,s);
			entermsg(fd,ufd,u,s,TRUE);
		}
		cr