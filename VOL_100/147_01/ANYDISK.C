.$$$ file");
	if ( seek(fdout,1,0) == ERROR)
		ioerr("Seek in USERS.$$$");
	readrec(fdin,0,tmpstr,1);
	sscanf(tmpstr,"MAX REC: %d",&inct);
	for ( i = 1; i <= inct; i++)
	{
		formrec(tmpstr,SECSIZ);
		readrec(fdin,i,tmpstr,1);
		sscanf(tmpstr+NM,"%s",u->nm);		/* NM:	*/
		sscanf(tmpstr+LLG,"%s",u->lastlog);	/* LLG:	*/
		sscanf(tmpstr+FRM,"%s",u->from);	/* FRM:	*/
		sscanf(tmpstr+PRV,"%s",u->ustat);	/* PRV: */
		if (auto && (*(u->ustat) != '#'))/* GOODBYE TWITS*/
		{
			printuser(u);
			Rriterec(fdout,outct++,1,tmpstr);
		}
		if (!auto)
		{
			printuser(u);
			crlf(1);
			if ( getyn("Write rec"))
			{
				Rriterec(fdout,outct++,1,tmpstr);
			}
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
/****************************************************************/
#if	!PERSONLY
subject()
{
	int	i,fd,n;

	crlf(1);
	if ( (fd = open(DSK(SUBJECTS.CCC),2)) == ERROR)
		fd = creat_subj(fd);
	if ( readrec(fd,0,tmpstr,1) == ERROR)
		ioerr("reading the subject record");
	sscanf(tmpstr,"%d",&n);
	displ_subj(fd,n,tmpstr);
	close(fd);	
}
/****************************************************************/
creat_subj(fd)
int	fd;
{
	int	i;

	outstr("Creating new SUBJECTS.CCC...",1);
	if ( (fd = creat(DSK(SUBJECTS.CCC))) == ERROR)
		ioerr("creating SUBJECTS.CCC");
	for ( i = 0; i < SECSIZ; i++)
		*(tmpstr+i) = ' ';
	sprintf(tmpstr,"%02d",2);
	sprintf(tmpstr+8,"PERSONAL");
	sprintf(tmpstr+16,"GENERAL ");
	Rriterec(fd,0,1,tmpstr);
	return fd;
}
/****************************************************************/
add_subj()
{
	int	i,fd,n;
	char	input[17];
	char	rec[SECSIZ];

	if ( (fd = open(DSK(SUBJECTS.CCC),2)) == ERROR)
		ioerr("opening subject");
#if	LOCKEM
	if (lokrec(fd,-1))
		ioerr("locking SUBJECTS.CCC");
#endif
	if ( readrec(fd,0,rec,1) == ERROR)
		ioerr("reading the subject record");
	sscanf(rec,"%d",&n);
	displ_subj(fd,n,rec);
        do
        {
		outstr("Enter new subject <Max 8 chars>: ",4);
		instr("",input,8);
		capstr(input);
        } while (!strlen(input));
	strcat(input,"        ");
	sprintf(rec+8*(++n),"%8.8s",input);
	sprintf(rec,"%02d",n);
	Rriterec(fd,0,1,rec);
#if	LOCKEM
	frerec(fd,-1);
#endif
	close(fd);	
}
/****************************************************************/
select_subj(choice,s)
int	choice;
struct	_sum	*s;
{
	int	fd,n;
	char	input[3];
	char	rec[SECSIZ];

	if ( (fd = open(DSK(SUBJECTS.CCC),2)) == ERROR)
		ioerr("opening SUBJECTS.CCC");
	if (readrec(fd,0,rec,1) == ERROR)
		ioerr("reading SUBJECTS.CCC");
	sscanf(rec,"%d",&n);
	if (!choice)
	{
		outstr("Subjects are:",2);
		displ_subj(fd,n,rec);
		outstr("File number: ",4);
		instr("",input,3);
		crlf(1);
		choice = atoi(input);
	}
	setmem(cmsgfile,9,0);
	movmem(rec+8*choice,cmsgfile,8);
	sprintf(msgfile,"%s%s%s",DRIVE,capstr(cmsgfile),".MSG");
	sprintf(tmpstr,"Current SUBJECT File: %s",msgfile);
	outstr(tmpstr,2);
	crlf(1);
	close(fd);	
}
#endif
/****************************************************************/
superuser()
{
	char	name[NAMELGTH+1];
	char	input[5];
	struct	_user	u2;
	int	ufd;
	int	i;

	if ( (ufd = open(DSK(USERS.CCC),2)) == ERROR)
		ioerr("opening USERS");
	crlf(1);
#if	LOCKEM
	if (lokrec(ufd,-1))
		ioerr("locking USERS");
#endif
	while ( (getname(name,"Enter user name: ")) )
		crlf(1);
	if (!checkuser(ufd,name,u2,0))
	{
		crlf(1);
		if (getyn("User not found.  Add this user") )
		{
			belflg = FALSE;
			expert = FALSE;
			newuser(u2,name,ufd);
		}
#if	LOCKEM
		frerec(ufd,-1);
#endif
		close(ufd);
		return;
	}
	else
	{
		printuser(u2);
		for ( i = 2; i <= 8; ++i)
		{
	sprintf(tmpstr,"Last msg (File #%d): %d, change it",i,u2.lstmsg[i-2]);
			while(getyn(tmpstr))
			{
				outstr("Enter new value: ",5);
				instr("",input,4);
				u2.lstmsg[i-2] = atoi(input);
	sprintf(tmpstr,"Last msg (File #%d): %d, change it",i,u2.lstmsg[i-2]);
				crlf(1);
			}
		}
		sprintf(tmpstr,"MF stat: %s, change it",u2.ustat);
		while(getyn(tmpstr))
		{
			outstr("Enter new value: ",5);
			instr("",u2.ustat,1);
			sprintf(tmpstr,"MF stat: %s, change it",u2.ustat);
			crlf(1);
		}
		sprintf(tmpstr,"Password: %s, change it",u2.pwd);
		while(getyn(tmpstr))
		{
			outstr("Enter: ",5);
			instr("",u2.pwd,6);
			sprintf(tmpstr,"Password: %s, change it",u2.pwd);
			crlf(1);
		}
		belflg = u2.bellflag;
		expert = u2.xpert;
		updtuser(u2,ufd);
	}
#if	LOCKEM
	frerec(ufd,-1);
#endif
	close(ufd);
}
/****************************************************************/
printuser(u)
struct	_user	*u;
{
	char	temp[SECSIZ];

	crlf(1);
	sprintf(temp,"USER NAME:  %s",u->nm);
	outstr(temp,3);
	sprintf(temp,"USER SITE:  %s",u->from);
	outstr(temp,3);
	sprintf(temp,"LAST LOGON: %s",u->lastlog);
	outstr(temp,2);
	crlf(1);
}
/****************************************************************/
newuser(u,name,fd)		/*DOUBLE CHECKS NEW USER & GETS	*/
struct	_user	*u;		/*INFO TO ADD NEW USER. ADDS	*/
char	*name;			/*NEW USER TO FILE. DOES NOT	*/
int	fd;			/*CLOSE USER FILE		*/
{
	int	correct;
	char	place[31];
	char	passwd[7];
#if	LOCKEM
	char	temp[SECSIZ];
#endif

	correct = FALSE;
	crlf(1);
	do
	{
		sprintf(tmpstr,"Enter user's %s: ",SITEPROMPT);
		outstr(tmpstr,4);
		instr("",place,30);
		capstr(place);
		crlf(1);
		sprintf(tmpstr,"Is the name, %s, correct",place);
		correct = getyn(tmpstr);
	}while(!correct);
	chkpwd(passwd);

	formrec(tmpstr,SECSIZ);
	sprintf(tmpstr+NM,"%s",name);		/* NM:	*/
	sprintf(tmpstr+PRV,"%c%c",'+',0);	/* PRV:	*/
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
	sprintf(tmpstr,"MAX REC: %3d",u->maxno);
	Rriterec(fd,0,1,tmpstr);
#if	LOCKEM
	frerec(fd,-1);
#endif

	return (TRUE);
}
/****************************************************************/
ntrlong(ufd,u,s)	
int	ufd;
struct	_user	*u;
struct	_sum	*s;
{
	struct	_sum	s2;
	int	msgfd;

	msgfd = openfile(msgfile);
	s->mstat = TRUE;
	strcpy(s->date,sysdate);
	if (!check_to(ufd,s,TRUE))
		return;
	strcpy(s->fnm,u->nm);
	*(s->subj) = 0;
	get_subj(s);
#if	LOCKEM
	if (lokrec(msgfd,-1))
		ioerr("locking .MSG file");
#endif
	msgct = loadsum(msgfd,s2); /* Loads last msg no if updated	*/
	msg_info(msgct,FALSE);
	s->msgno = ++lastmsg;
	++msgct;
	if ( (s->lct = longtxt(msgfd,mndx[msgct])) == ERROR)
	{
#if		LOCKEM
		frerec(msgfd,-1);
#endif
		return;
	}
	mndx[msgct+1] = ritesum(msgfd,mndx[msgct],s);
	mno[msgct] = s->msgno;
	mno[msgct+1] = 0;
#if	LOCKEM
	frerec(msgfd,-1);
#endif
	sprintf(tmpstr,"Message stored as #%d.",s->msgno);
	outstr(tmpstr,1);
	return TRUE;
}
/***************************************************************/
prnt_instr()
{
	int	test;

outstr("To enter text this way, all line should be 60 chars or less",1);
outstr("with no high order bits set.  Otherwise, exit now:  ",0);
	test = getyn("Continue");
	crlf(1);
	return test;
}
/***************************************************************/	
longtxt(fd,start)
int	fd,start;
{
	int	lnct;
	char	file[18];
	char	name[13];
	char	buf[BUFSIZ];
	char	line[SECSIZ];

	lnct = 0;
	outstr("File name to read: ",0);
	instr("",name,13);
	crlf(1);
	capstr(name);
	sprintf(file,"%s%s",DRIVE,name);
	outstr(file,1);
	if (fopen(file,buf) != ERROR)
	{
		while ( (fgets(line,buf)))
		{
			if(strlen(line) > 61)
				*(line + 61) = 0;
			else
				*(line+strlen(line)-1) = 0;
			if ( !((lnct++)%2))
			{
				formrec(tmpstr,SECSIZ);
				movmem(line,tmpstr,SECSIZ/2);
			}
			else
			{
				movmem(line,tmpstr+SECSIZ/2,SECSIZ/2);
				rite_line(fd,++start,tmpstr);
			}
		}
		if (!lnct)
		{
			rite_line(fd,++start,tmpstr);
			fclose(buf);
			return lnct;
		}
		else
		{
			fclose(buf);
			return --lnct;
		}
	}
	return ERROR;
}
/***************************************************************/	
rite_line(fd,line,source)
int	fd,line;
char	*source;
{
	outstr(tmpstr,1);
	outstr(tmpstr+SECSIZ/2,1);
	seek_blk(fd,line);
	if ( write(fd,source,1) == ERROR)
	{
		sprintf(tmpstr,"Rite err in fd: %d, rec: %d",fd,line);
		outstr(tmpstr,1);
		rexit();
	}
}
/***************************************************************/	
);
	crlf(1);
	capstr(name);
	sprintf(file,"%s%s",DRIVE,name);
	outstr