/****************************************************************
* RBBSFN.C							*

Release 7: RBBS 4.1 Edit 02 - getname takes a prompt argument
Release 5: RBBS 4.1 Edit 00 - Names now one large field
			    - If output killed by user in bufinmsg,
			      the stacked commands are flushed.
			    - Inserted PERSONLY in dispsum
Release 4: RBBS 4.0 Edit 21 - changed displ_subj to list across
Release 1: RBBS 4.0 Edit 18

* This file contains the functions:

* get_date	gets the current date into the global sysdate

* displ_subj	displays the contents of SUBJECTS.CCC

* load_cntrl	creates the .MSG if none exists, else calls loadsum

* loadsum	calls readsum and fills the mndx and mno arrays

* dispsum	displays the one-liner summaries

* msg_info	gets the stats on the current .MSG file and
                optionally displays them

* prnt_txt	displays the text of a message optionally with
                line numbers

* setptr	sets up the line number pointers

* readtxt	reads the text of a message and prints it

* readsum	reads the header record and fills the summary
                structure for that message

* prnt_sum	displays the header of a message

* ritemsg	writes the header (via ritesum) and text to .MSG

* ritesum	writes the header to .MSG

* readrec	calls seek_blk and read(s a record)

* openfile	opens requested file and displays error msg on failure

* getname	prompts for and returns name

* seek_blk	calls seek and displays error msg on failure

* formrec	fills character array with blanks ending with CRLF

* Rriterec	writes a "random" record

* bufinmsg	fgets and outstr contents of file.CCC with pause

* ioerr		displays error messages

* rexit		chains to AUTO.COM

* hexit		chains to HANGUP.COM

****************************************************************/
#include	<bdscio.h>
#include	"rbbs4.h"

/****************************************************************/
get_date()		/* gets a date in format mm/dd/yy	*/
{
#if	DATETIME
	sprintf(sysdate,"%s %s",daytim(),ZONE);
#else
	int	mo,da,yr;
	int	error;
	int	leap;

	do
	{
		error = FALSE;
		outstr("Date <MM/DD/YY>: ",5);
		instr("",sysdate,9);
		crlf(1);
		if(*(sysdate+2) != '/' || *(sysdate+5) != '/')
			error = TRUE;
		*(sysdate+2) = *(sysdate+5) = 0;
		mo = atoi(sysdate);
		da = atoi(sysdate+3);
		yr = atoi(sysdate+6);
		leap = yr%4;
		if ( (mo < 1) || (mo > 12) )
			error = TRUE;
		else
		{
			switch(mo)
			{
				case 4:
				case 6:
				case 9:
				case 11:		
					if ( (da < 1) || (da > 30) )
						error = TRUE;
					break;
				case 2:
					if ( (da < 1) || (da > 28+leap) )
						error = TRUE;
					break;
				default:
					if ( (da < 1) || (da > 31) )
						error = TRUE;
					break;
			}
		}
	} while(error);
	*(sysdate+2) = *(sysdate+5) = '/';
#endif
}
/***************************************************************/
displ_subj(fd,n,rec)
int	fd,n;
char	*rec;
{
	int	i;

	crlf(1);
	for ( i = 1; i <= n; i++)
	{
		sprintf(tmpstr,"<%2d>: %8.8s",i,rec+8*i);
		outstr(tmpstr,0);
		if ( !(i % 4))
			crlf(1);
		else
			outstr("    ",0);
	}
	crlf(1);
}
/****************************************************************/
load_cntrl(msgct,s)
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
	msgct = loadsum(msgfd,s);
	if (close(msgfd) == ERROR)
		ioerr("Closing Msg");
	return msgct;
}
/***************************************************************/
loadsum(fd,s)
int	fd;
struct	_sum	*s;
{
	int	ct, i, siz;

	ct = siz = 0;
	for ( i = 1; i< MAXMSGS; i++)
	{
		if ( (ct = readsum(fd,ct,s)) == ERROR)
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
dispsum(s)
struct	_sum	*s;
{
	char	temp[NAMELGTH+1];
	char	temp1[NAMELGTH+1];

	sprintf(tmpstr,"%4d %2d  ",s->msgno,s->lct+5);
	outstr(tmpstr,0);
	sprintf(tmpstr,"%.9s  ",s->date);
	outstr(tmpstr,0);
	if (!strcmp(s->fnm,"SYSOP"))
		outstr("SYSOP     ",0);
	else
	{
		sscanf(s->fnm,"%s %s",temp1,temp);
		sprintf(tmpstr,"%-10s",temp);
		outstr(tmpstr,0);
	}
	outstr(" -> ",0);
	if (!strcmp(s->tnm,"SYSOP"))
		outstr("SYSOP       ",0);
	else
#if	!PERSONLY
		if (!strcmp(s->tnm,"ALL"))
			outstr("ALL         ",0);
		else
#endif
		{
			sscanf(s->tnm,"%s %s",temp1,temp);
			sprintf(tmpstr,"%-10s  ",temp);
			outstr(tmpstr,0);
		}
	sprintf(tmpstr,"%.30s",s->subj);
	outstr(tmpstr,1);
}
/****************************************************************/
msg_info(msgct,print)
int	msgct,print;
{
	int	i;

	sprintf(tmpstr,"Current Message file: %s",cmsgfile);
	outstr(tmpstr,2);
	for ( i = 1; (i <= msgct) && (!mno[i]); i++);
	frstmsg = mno[i];
	for ( i = msgct; (i >= 1) && (!mno[i]); i--);
	lastmsg = mno[i];
	for ( i = 1, actmsg = 0; i <= msgct; i++)
		if(mno[i])
			++actmsg;
	if (print)
	{
sprintf(tmpstr,"First Active Msg #: %3d   No. of Active Msgs: %3d",frstmsg,actmsg);
		outstr(tmpstr,2);
sprintf(tmpstr," Last Active Msg #: %3d    Total No. of Msgs: %3d",lastmsg,msgct);
		outstr(tmpstr,1);
	}
}
/***************************************************************/
prnt_txt(s,linenum)	/* print the output of the text buffer */
struct	_sum	*s;	/* the structure s		       */
int	linenum;	/* the line number or 0 to suppress    */
{
	int	i;

	crlf(1);
	for ( i = 1; i <= s->lct; i++)
	{
		*(lp[i]+MLINESIZ-1) = 0;
		*tmpstr = 0;
		if (linenum)
			sprintf(tmpstr,"%2d>: ",i);
		strcat(tmpstr,lp[i]);
		outstr(tmpstr,1);
	}
}
/****************************************************************/
setptr(m,p)		/* Set ptrs to lines of message		*/
char	*m,*p[];
{
	int	i;

	p[0] = m;
	for ( i = 1; i <= MLINELIM; i++)
		p[i] = p[i-1]+MLINESIZ;
}
/***************************************************************/
readtxt(fd,recno,s)
int	fd, recno;
struct	_sum	*s;
{
	int	i,j,blks;

	blks = (s->lct + 1)/2;
	for ( i = 1, j = 1; i <= blks; i++)
	{
		readrec(fd,recno+i,tmpstr,1);
		outstr(tmpstr,1);
		if(++j <= s->lct)
		{
			outstr(tmpstr+SECSIZ/2,1);
			++j;
		}
	}
}
/***************************************************************/
readsum(fd,recno,s)
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
prnt_sum(s)
struct	_sum	*s;
{
	sprintf(tmpstr,"Msg#: %d  (%d lines)",s->msgno,s->lct+5);
	outstr(tmpstr,1);
	sprintf(tmpstr,"Date: %s",s->date);
	outstr(tmpstr,1);
	sprintf(tmpstr,"From: %s",s->fnm);
	outstr(tmpstr,1);
	sprintf(tmpstr,"To:   %s",s->tnm);
	outstr(tmpstr,1);
	sprintf(tmpstr,"Re:   %s",s->subj);
	outstr(tmpstr,1);
}
/****************************************************************/
ritemsg(fd,recno,s,source)
int	fd,recno;
struct	_sum	*s;
char	*source;
{
	int	blks,nextrec;

	nextrec = ritesum(fd,recno,s);
	blks = (s->lct + 1)/2;
	seek_blk(fd,recno+1);
	if ( write(fd,source,blks) == ERROR)
	{
		sprintf(tmpstr,"Rite err in fd: %d,  rec: %d",fd,recno);
		outstr(tmpstr,1);
		rexit();
	}
	return (nextrec);
}
/***************************************************************/
ritesum(fd,recno,s)		/* WRITE A ONE BLOCK SUMMARY 	*/
int	fd,recno;		/* RECORD TO THE LOC'N 'RECNO'	*/
struct	_sum	*s;	
{
	int	blks;

	formrec(tmpstr,SECSIZ);
	sprintf(tmpstr,"%04d ",s->msgno);	/* MSG:	*/
	sprintf(tmpstr+LNS,"%02d ",s->lct);	/* LNS:	*/
	sprintf(tmpstr+STAT,"%1d ",s->mstat);	/* STAT:*/
	sprintf(tmpstr+DT,"%s",s->date);	/* DT:	*/
	sprintf(tmpstr+FM,"%s",s->fnm);		/* FM:	*/
	sprintf(tmpstr+TO,"%s",s->tnm);		/* TO:	*/
	sprintf(tmpstr+SU,"%s",s->subj);	/* SU:	*/
	seek_blk(fd,recno);
	if ( write(fd,tmpstr,1) == ERROR)
		ioerr("Writing to Summary");
	blks = (s->lct+3)/2;
	return(recno+blks);
}
/****************************************************************/
readrec(fd,recno,dest,blks)
int	fd,recno;
char	*dest;
int	blks;
{
	int	err;

	seek_blk(fd,recno);
	err = read(fd,dest,blks);
	if ( (!err) || err == ERROR)
		return(ERROR);
	else
		return(TRUE);
}
/***************************************************************/
openfile(fn)		/* OPENS A FILE FOR READING & WRITING	*/
char	*fn;		
{
	int	d;

	if ( (d = open(fn,2)) == ERROR)
	{
		sprintf(tmpstr,"Opening %s Error: %s  Quit",fn,errmsg(errno()));
		if (getyn(tmpstr))
			rexit();
	}
	return(d);
}
/****************************************************************/
getname(name,prompt)		/*GET WHOLE NAME, CONVERT	*/
char	*name;			/*TO CAPS 			*/
char	*prompt;
{
	outstr(prompt,5);
	instr("",name,NAMELGTH);
	capstr(name);
	if (!strlen(name) || !strcmp(name,"ALL"))
		return(TRUE);
	return(FALSE);
}
/****************************************************************/
seek_blk(fd,blk)	/*SEEKS TO THE FILE AND BLOCK & PRINTS	*/
int	fd,blk;		/*ERROR MESSAGE IF SEEK IS BAD		*/
{
	if ( seek(fd,blk,0) == ERROR)
	{
		sprintf(tmpstr,"Seek error in fd %d, rec: %d",fd,blk);
		outstr(tmpstr,1);
		rexit();
	}
}
/***************************************************************/
Rriterec(fd,blk,nbl,source)
int	fd,blk,nbl;
char	*source;
{
	if ( seek(fd,blk,0) == ERROR)
		ioerr("Seek error in random rite");
	if ( write(fd,source,nbl) == ERROR)
		ioerr("Rite error in random rite");
}
/***************************************************************/
formrec(ptr,siz)	/* FORMS A BLANK RECORD WITH STRING TERM*/
char	*ptr;
int	siz;
{
	setmem(ptr,siz,' ');
	*(ptr+siz-2) = 13;		/* CR */
	*(ptr+siz-1) = 10;		/* LF */
}
/***************************************************************/
bufinmsg(f)
char	*f;
{
	char	buf[BUFSIZ];
	char	file[18];
	int	i;

	i = 0;
	sprintf(file,"%s%s%s",DRIVE,f,".CCC");
	if (fopen(file,buf) != ERROR)
	{
		if (!expert)
outstr("Hit CTL-S or s to Pause; any key to Resume.  Hit CTL-K or k to Abort.",2);
		crlf(1);
		while ( (fgets(tmpstr,buf)) ) 
		{
			*(tmpstr+strlen(tmpstr)-1) = 0;
			if (outstr(tmpstr,1))
			{
				*sav = 0;
				break;
			}
			if (!(++i % 22))
				if (!getyn("More"))
					break;
		}
		fclose(buf);
		crlf(1);
		return TRUE;
	}
	return FALSE;
}
/***************************************************************/
ioerr(s)
char	*s;
{
	sprintf(tmpstr,"%s. Error: %s.  Quit",s,errmsg(errno()));
	if (getyn(tmpstr))
		rexit();
}
/****************************************************************/
rexit()
{
	poke(0,0xc3);

#if	CHAINEXIT
	exec(CHAIN);
#endif
	exit();
}
/****************************************************************/
hexit()
{
	poke(0,0xc3);
	poke(0x5b,0);

#if	HANGUP
	exec(HANG);
#endif
	exit();
}
/****************************************************************/
sprint