/****************************************************************
* UTLX.C		Variant of UTIL.C to convert files	*

Release 5: RBBS 4.1 Edit 00 - Names now one large field

* Functions included in this file are:

* main		calls chk_user, subject, select_subj, and performs
		the main command loop

* chk_user	checks to see if USERS.CCC exists, and calls nu_user
		to create it if it doesn't.

* rite_user	(Case 2) creates USER.$$$, selectively copies each
		user, one at a time from USER.CCC. Then renames
		USER.CCC to USER.BAK and USER.$$$ to USER.BAK

* subject	checks to see if SUBJECTS.CCC exists, and calls
		creat_subj if not.  Then calls displ_subj to
		display the available subjects.

* readsum	special conversion version from RBBSFN.C

****************************************************************/

#include	<bdscio.h>
#include	"rbbs4.h"

main()
{
	char	c[2];
	int	i;
	int	fd, ufd;
	struct	_sum	s;
	struct	_user	u;

#if	DATETIME
	get_date();
	strcpy(logdate,sysdate);
#endif
	upstr("dummy");
	setptr(m,lp);
	chk_user();
#if	!PERSONLY
	subject();
#else
	strcpy(cmsgfile,"PERSONAL");
	sprintf(msgfile,"%s%s%s",DRIVE,capstr(cmsgfile),".MSG");
	msgct = load_cntrl(0,s);
	msg_info(msgct,TRUE);
#endif
	while(TRUE)
	{
outstr("**************************************************************",2);
outstr("<1> Convert USERS.CCC file from 4.0 to 4.1 format",2);
outstr("<2> Convert a Subject file from 4.0 to 4.1 format",1);
		outstr("<9> Quit",2);
		crlf(1);
		outstr("Enter choice: ",4);
		instr("",c,1);
		crlf(1);
		switch(*c-0x30)
		{
			case 1:
				rite_user(u);
				break;

			case 2:
#if	!PERSONLY
				select_subj(0,s);
				msgct = load_cntrl(0,s);
				msg_info(msgct,FALSE);
#endif
				rite_active(msgct,s);
				outstr("Loading new control",2);
				msgct = nload_cntrl(msgct,s);
				msg_info(msgct,TRUE);
				for ( i = 1; i <= msgct; i++)
					prnt_ndx(i);
				break;

			case 9:
				outstr("Finished",2);
				exit();
				break;

			default:
				outstr("Choice Error",2);
				break;
		}
	}
}
/****************************************************************/
chk_user()
{
	int	fd;
	int	bias;
	char	input[10];

	crlf(1);
	if ( (fd = open(DSK(USERS.CCC),2)) == ERROR)
		ioerr("USERS.CCC does not exist");
	else
		close(fd);	
}
/****************************************************************/
rite_user(u)			/* Conversion version		*/
struct	_user	*u;
{
	int	i,j,k;
	int	fdin,fdout;
	int	inct,outct;
	char	nm1[11];
	char	nm2[11];
	
	outct = 1;
	if ( (fdin = open(DSK(USERS.CCC),2)) == ERROR)
		ioerr("Cannot open USERS.CCC");
#if	LOCKEM
	if (lokrec(fdin,-1))
		ioerr("locking USERS.CCC");
#endif
	if ((fdout= creat(DSK(USERS.$$$)))== ERROR)
		ioerr("Creating USERS.$$$ file");
	if ( seek(fdout,1,0) == ERROR)
		ioerr("Seek in USERS.$$$");
	readrec(fdin,0,tmpstr,1);
	sscanf(tmpstr,"MAX REC: %d",&inct);
	for ( i = 1; i <= inct; i++)
	{
		formrec(tmpstr,SECSIZ);
		readrec(fdin,i,tmpstr,1);
		sscanf(tmpstr+0,"%s",nm2);		/* NM2:	*/
		if (*nm2)
		{
			sscanf(tmpstr+16,"%s",nm1);	/* NM1:	*/
			sprintf(u->nm,"%s %s",nm1,nm2);
		}
		else
			sscanf(tmpstr+16,"%s",u->nm);
		sscanf(tmpstr+32,"%s",u->ustat);	/* PRV:	*/
		sscanf(tmpstr+34,"%s",u->pwd);		/* PWD:	*/
		sscanf(tmpstr+41,"%s",u->lastlog);	/* LLG:	*/
		sscanf(tmpstr+65,"%d",&u->lstmsg[0]);	/* LMG:	*/
		sscanf(tmpstr+70,"%d",&u->bellflag);	/* UBL:	*/
		sscanf(tmpstr+75,"%d",&u->xpert);	/* UXP:	*/
		sscanf(tmpstr+95,"%s",u->from);		/* FRM:	*/
		printuser(u);
		crlf(1);
		if (getyn("Write rec"))
		{
			formrec(tmpstr,SECSIZ);
			sprintf(tmpstr+NM,"%s",u->nm);		/* NM:	*/
			sprintf(tmpstr+PRV,"%s",u->ustat);	/* PRV:	*/
			sprintf(tmpstr+UBL,"%1d",u->bellflag);	/* UBL:	*/
			sprintf(tmpstr+UXP,"%1d",u->xpert);	/* UXP:	*/
			sprintf(tmpstr+PWD,"%s",u->pwd);	/* PWD:	*/
			sprintf(tmpstr+LLG,"%s",u->lastlog);	/* LLG:	*/
			sprintf(tmpstr+FRM,"%s",u->from);	/* FRM:	*/
			sprintf(tmpstr+LMG0,"%03d",u->lstmsg[0]);
			sprintf(tmpstr+LMG1,"%03d",0);
			sprintf(tmpstr+LMG2,"%03d",0);
			sprintf(tmpstr+LMG3,"%03d",0);
			sprintf(tmpstr+LMG4,"%03d",0);
			sprintf(tmpstr+LMG5,"%03d",0);
			sprintf(tmpstr+LMG6,"%03d",0);
			Rriterec(fdout,outct++,1,tmpstr);
		}
	}
	formrec(tmpstr,SECSIZ);
	sprintf(tmpstr,"MAX REC: %d",--outct);
	Rriterec(fdout,0,1,tmpstr);
	if ( close(fdout) == ERROR)
		ioerr("Closing USERS.$$$");

#if	LOCKEM
	frerec(fdin,-1);
#endif
	if ( close(fdin) == ERROR)
		ioerr("Closing USERS.CCC");
	unlink(DSK(USERS.BAK));
	if ( rename(DSK(USERS.CCC),DSK(USERS.BAK)) == ERROR)
		ioerr("Renaming USERS.CCC to USERS.BAK");
	if ( rename(DSK(USERS.$$$),DSK(USERS.CCC)) == ERROR)
		ioerr("Renaming USERS.$$$ to USERS.CCC");
}
/***************************************************************/	
#if	!PERSONLY
subject()
{
	int	i,fd,n;

	crlf(1);
	if ( (fd = open(DSK(SUBJECTS.CCC),2)) == ERROR)
		ioerr("SUBJECTS.CCC does not exist");
	if ( readrec(fd,0,tmpstr,1) == ERROR)
		ioerr("reading the subject record");
	sscanf(tmpstr,"%d",&n);
	displ_subj(fd,n,tmpstr);
	close(fd);	
}
#endif
/****************************************************************/
readsum(fd,recno,s)
int	fd,recno;
struct	_sum	*s;
{
	int	blks;
	char	nm1[11];
	char	nm2[11];

	if ( readrec(fd,recno,tmpstr,1) == ERROR)
		return(ERROR);

	/* SSCANF CONSTANT INFORMATION */
	sscanf(tmpstr,"%d ",&s->msgno);		/* MSG:	*/
	sscanf(tmpstr+10,"%d ",&s->lct);	/* LNS:	*/
	sscanf(tmpstr+18,"%d ",&s->mstat);	/* STAT:*/
	sscanf(tmpstr+26,"%s",s->date);		/* DT:	*/
	sscanf(tmpstr+61,"%s",nm2);
	if (*nm2)
	{
		sscanf(tmpstr+51,"%s",nm1);	/* FM:	*/
		sprintf(s->fnm,"%s %s",nm1,nm2);
	}
	else
		sscanf(tmpstr+51,"%s",s->fnm);
	sscanf(tmpstr+82,"%s",nm2);
	if (*nm2)
	{
		sscanf(tmpstr+72,"%s",nm1);	/* TO:	*/
		sprintf(s->tnm,"%s %s",nm1,nm2);
	}
	else
		sscanf(tmpstr+72,"%s",s->tnm);
	sscanf(tmpstr+93,"%s",s->subj);		/* SU:	*/
	blks = (s->lct+3)/2;
	return(recno+blks);
}
/***************************************************************/	
nload_cntrl(msgct,s)
int	msgct;
struct	_sum	*s;
{
	int	msgfd;

	if ( (msgfd = open(msgfile,2)) == ERROR)
	{
		sprintf(tmpstr,"Creating %s",msgfile);
		outstr(tmpstr,1);
		if ( (msgfd = creat(msgfile)) == ERROR)
			ioerr("Creating msgfile");
		mno[1] = mndx[1] = 0;
		if (close(msgfd) == ERROR)
			ioerr("Closing Msg");
		return 0;
	}
	msgct = nloadsum(msgfd,s);
	if (close(msgfd) == ERROR)
		ioerr("Closing Msg");
	return msgct;
}
/***************************************************************/
nloadsum(fd,s)
int	fd;
struct	_sum	*s;
{
	int	ct, i, siz;

	ct = siz = 0;
	for ( i = 1; i< MAXMSGS; i++)
	{
		if ( (ct = nreadsum(fd,ct,s)) == ERROR)
		{
			mndx[i] = mndx[i-1] + siz;
			mno[i] = 0;
			return (i-1);
		}
		if (i)
			mndx[i] = mndx[i-1] + siz;
		else
			mndx[0] = 0;
		siz = (s->lct+3)/2;
		if (s->mstat)
			mno[i] = s->msgno;
		else
			mno[i] = 0;
	}
}
/***************************************************************/
nreadsum(fd,recno,s)
int	fd,recno;
struct	_sum	*s;
{
	int	blks;

	if ( readrec(fd,recno,tmpstr,1) == ERROR)
		return(ERROR);

	/* SSCANF CONSTANT INFORMATION */
	sscanf(tmpstr,"%d ",&s->msgno);		/* MSG:	*/
	sscanf(tmpstr+LNS,"%d ",&s->lct);	/* LNS:	*/
	sscanf(tmpstr+STAT,"%d ",&s->mstat);	/* STAT:*/
	sscanf(tmpstr+DT,"%s",s->date);		/* DT:	*/
	sscanf(tmpstr+FM,"%s",s->fnm);		/* FM:	*/
	sscanf(tmpstr+TO,"%s",s->tnm);		/* TO:	*/
	sscanf(tmpstr+SU,"%s",s->subj);		/* SU:	*/
	blks = (s->lct+3)/2;
	return(recno+blks);
}
/****************************************************************/
nt	ct, i, siz;

	ct = siz = 0;
	for ( i = 1; i< MAXMSGS; i++)
	{
		if ( (ct = nreadsum(fd,ct,s)) == E