/*
	bbsc.c

	BBS (Bulletin Board System) written in BDS "C".

		Support files needed:
			bdscio.h
			bbscdef.h
			bbscport.crl or bbscport.c
			bbsclock.crl or bbsclock.c
			bbscfile.crl or bbscfile.c
			bbscmisc.crl or bbscmisc.c

		To compile/link do:

			>cc1 bbsc.c
			>cc1 bbscport  <----- optional if .crl file not present
			>cc1 bbsclock  <----- optional if .crl file not present
			>cc1 bbscfile  <----- optional if .crl file not present
			>cc1 bbscmisc  <----- optional if .crl file not present
			>clink bbsc -s -f bbscport -f bbsclock -f bbscfile -f bbscmisc -e 800

				Mike Kelly

	03/09/83 v1.0	written
	07/07/83 v1.0	updated
*/
#include "bdscio.h"
#include "bbscdef.h"


#define LASTDATE  " 07/07/83 "

#define PGMNAME "BBSc    "
#define VERSION " 1.0 "


main(argc,argv)

char	**argv;

int	argc;

{
	int	i;

/*
*		init global variables
*/
	debug 	= FALSE;
	offline = FALSE;
	statcnt = 0;
	xpert	= FALSE;
	active	= TRUE;

	w_fname[0] = "\0";
	w_lname[0] = "\0";
	strcpy(w_password,"MPK0");

	u_fname[0] = "\0";
	u_lname[0] = "\0";
	strcpy(u_password,"MPK0");
	u_time1[0] = "\0";
	u_date1[0] = "\0";
	u_time2[0] = "\0";
	u_date2[0] = "\0";
	u_city[0]  = "\0";

	mm[0]    = "\0";
	dd[0]    = "\0";
	yy[0]    = "\0";
	month[0] = "\0";
	day[0]   = "\0";
	year[0]  = "\0";
	date[0]  = "\0";
	week[0]  = "\0";
	time[0]  = "\0";

	strcpy(msg_delete,"0\0");
	msg_no[0]   = "\0";
	msg_date[0] = "\0";
	msg_date[0] = "\0";
	msg_time[0] = "\0";
	msg_to[0]   = "\0";
	msg_from[0] = "\0";
	msg_pass[0] = "\0";
	msg_subject[0] = "\0";
	msg_text[0] = "\0";

	stop_that = FALSE;

/*
*		end global init
*/

/*
*		check for DEBUG/OFFLINE mode on command line
*/
	if (argc > 1)
	{
		printf("\nargc=%04u  argc hex=%04x\n",argc,argc);
		for (i = 1; i < argc; i++)
		{
        		printf("argv[%02u]=%s \n",i,*++argv);

			if (strcmp(*argv,"DEBUG") == OK)
			{
        			printf("\nDebug mode on!\n\n");
	        		debug = TRUE;
			}

			if (strcmp(*argv,"OFFLINE") == OK)
			{
        			printf("\nOffline mode on!\n\n");
	        		offline = TRUE;
			}
		}
	}


/*
*			get date and time from the clock
*/
	gettime(time);
	getdate(mm,dd,yy,month,day,year,date,week);

/*	unlink(MSGX1);			/* delete temporary file */
	unlink(MSGX2);			/* delete temporary file */
*/

	portinit(USER7);	/* init modem port */
	signon();
	driver(argc,argv);
	signoff();
}
/*	end of mainline routine 	*/


driver(argc,argv)

char	**argv;
int	argc;

{
char	linebuf[MAXLINE],
	buf[128],
	*bufptr,		/* ptr to buf */
	usercd[100],		/* user record */
	*userptr,		/* ptr to usercd */
	cmd[2];

int	cnt,
	ok_sw,
	case_sw,
	i,
	num0,
	num1,
	num2;

	cnt	= 0;
	ok_sw	= TRUE;
	bufptr	= &buf;
	xpert	= FALSE;
	cmd[0]  = '\0';

	while (active)
	{
		stop_that = 0;		/* reset switch */
		if (xpert)
		{
			portsout(CRLF);
			portsout("===>");
		}
		else
		{
			portsout(CRLF);
			portsout("Commands: B,C,E,G,K,Q,R,S,X or ? ===>");
		}

/*		while (portinstat());
		byte = portin();
		while (portoutstat());
		portout(byte);
*/
		portsin(cmd,1);
		portsout(CRLF);

		switch (cmd[0])
		{
			case ('B'):		/* goodby */
				cmd_b();
				break;
			case ('C'):		/* cp/m */
				cmd_c();
				break;
			case ('E'):		/* enter msg */
				cmd_e();
				break;
			case ('G'):		/* goodby */
				cmd_g();
				break;
			case ('K'):		/* kill msg */
				cmd_k();
				break;
			case ('Q'):		/* quick scan */
				cmd_q();
				break;
			case ('R'):		/* read msg */
				cmd_r();
				break;
			case ('S'):		/* scan msg */
				cmd_s();
				break;
			case ('X'):		/* xpert toggle */
				cmd_x();
				break;
			case ('?'):		/* help */
				cmd_h();
				break;
			default:
				break;
		}

	}
}
/*	end of function		*/

cmd_b()			/* good-by */
{
	active = FALSE;
	portsout("\r\nThanks for calling ");
	portsout(w_fname);
	portsout(CRLF);
	sleep(25);			/* for the Smartmodem */
	portsout("+");			/* get the modems attention */
	sleep(1);			/* for the Smartmodem */
	portsout("+");			/* get the modems attention */
	sleep(1);			/* for the Smartmodem */
	portsout("+");			/* get the modems attention */
	sleep(25);			/* for the Smartmodem */
	gobble();
	portsout("ATH\r\n");		/* hang up the phone */
}
/*	end of function		*/

cmd_c()			/* go to cp/m */
{
	portsout("Going to CP/M...\r\n");
	active = FALSE;
}
/*	end of function		*/

cmd_e()			/* enter a message */
{
	int	entering,
		editing,
		cnt1;
	char	msg[50],
		l_cnt[3],
		ans[2];

	entering = editing = TRUE;
	cnt1 = 0;
	l_cnt[0] = '0';			/* init line count */
	msg_text[0] = '\0';		/* init message text area */

#ifdef DEBUG
	if (debug)
	{
		printf("text s.b. empty!  text = ");
		prthex(msg_text);
		printf("\n");
	}
#endif
	portsout(CRLF);
	portsout("This will be message # ");
	portsout(h_next_msg);
	portsout(CRLF);
	portsout("			    ____.____1____.____2\r\n");
	portsout("Who is the message for  ===>");
	portsin(msg_to,20);
	portsout(CRLF);
	portsout("What is the subject     ===>");
	portsin(msg_subject,20);
	portsout(CRLF);

#ifdef DEBUG
	if  (debug)
	{
		printf("msg_to=%s\n",msg_to);
		printf("msg_subject=\n",msg_subject);
		prthex(msg_subject);
	}
#endif
	portsout(CRLF);
	portsout(CRLF);
	portsout("A line can contain 40 characters, with a maximum of 20 lines.\r\n");
	portsout("To end, enter a carriage return on an empty line.");
	portsout(CRLF);
	portsout(CRLF);
	portsout("   .___.____1____.____2____.____3____.____4");
	portsout(CRLF);

	while (entering)		/* get the text of the message */
	{
		linecnt(l_cnt);		/* calc line count */

		portsout(l_cnt);	/* show line count */
		portsout(">");		/*  and prompt */

		portsin(msg,40);
		portsout(CRLF);

		strcat(msg,"\r");	/* put a <cr> on each line */

#ifdef DEBUG
		if (debug)
		{
			printf("msg=%s\n",msg);
			printf("loop cnt = %d\n",cnt1);
		}
#endif
		strcat(msg_text,msg);		/* append each line to text */
		if (strlen(msg) < 2  ||		/* count cr too */
		       (++cnt1) > 19)		/* only allow 20 lines */
		{
			entering = FALSE;	/* all done */
		}
	}

	while (editing)		/* save it, fix it, list it, or get out */
	{
		portsout(CRLF);
		portsout("\r\n(S)ave, (E)dit, (L)ist, (Q)uit   ==>");

		portsin(ans,1);			/* get a byte */ 
		portsout(CRLF);

		switch (ans[0])			/* get 1 byte */
		{
			case ('S'):		/* save message */
				savemsg();
				editing = FALSE; /* get out of while */
				break;
			case ('E'):		/* edit message */
				editmsg();
				break;
			case ('L'):		/* list message */
				listmsg();
				break;
			case ('Q'):		/* get out w/out saving */
				editing = FALSE; /* get out of while */
				break;
			default:
				portsout("\n\r");
				portsout("Shall we try that one more time?\r\n");
				break;
		}	
	}

}
/*	end of function		*/

linecnt(lcnt)			/* 2 byte string of line numbers */
char	*lcnt;
{
	int	icnt;
	char	xcnt[4];

	icnt = atoi(lcnt);
	if (++icnt < 10)
	{
		strcpy(lcnt,"0");
		itoa(xcnt,icnt);
		strcat(lcnt,xcnt);
	}
	else
	{
		itoa(xcnt,icnt);
		strcpy(lcnt,xcnt);
	}
	return;
}
/*	end of function		*/

savemsg()				/* save a message */
{
	int	fd;

	portsout("Saving...\r\n");

	strcpy(msg_delete,"0\0");	/* delete byte */
	strcpy(msg_date,"\0");		/* move the date in */
	strcat(msg_date,mm);
	strcat(msg_date,"/");
	strcat(msg_date,dd);
	strcat(msg_date,"/");
	strcat(msg_date,yy);
	strcat(msg_date,"\0");

	strcpy(msg_time,time);		/* and the time */

	strcpy(msg_from,w_fname);	/* and the from name */
	strcat(msg_from," ");
	strcat(msg_from,w_lname);

#ifdef DEBUG
	if (debug)
	{
		printf("msg_time=");
		prthex(msg_time);
		printf("\n");
		printf("msg_date=%s\n",msg_date);
		printf("msg_from=%s\n",msg_from);
	}
#endif
	portsout("Updating the message file...\r\n");
	if ((fd = msgopen(2)) == ERROR)		/* open i/o */
	{
		return(ERROR);
	}
	msgwrt(fd);		/* write the message */
	msgclose(fd);

	strcpy(h_date,msg_date);
	portsout("Updating the header  file...\r\n");
	hdrwrt();		/* update the header file */
}
/*	end of function		*/


listmsg()
{
	char	*ptr,
		byte,
		lcnt[3];
	int	i;

	ptr = &msg_text;		/* ptr points to start of text */
	lcnt[0] = '0';
	
	linecnt(lcnt);
	portsout(lcnt);			/* put out line number */
	portsout(">");

	while (byte = *ptr++)
	{
		while(portoutstat());
		portout(byte);
		if (byte == '\r')	/* we dont save <lf>, so add */
		{			/*  it here when find a <cr> */
			while(portoutstat());
			portout('\n');
			linecnt(lcnt);
			portsout(lcnt);	/* put out line number */
			portsout(">");
		}
	}
#ifdef DEBUG
	if (debug)
	{
		printf("ending listmsg\n");
	}
#endif
}
/*	end of function		*/

editmsg()
{
	char	ans[3];
	int	lnum;		/* line number to edit */

	while(1)
	{
		portsout(CRLF);
		portsout("Enter line number to edit - RETURN to exit ==>");
		portsin(ans,2);		/* only 20 lines possible */
		portsout(CRLF);
		lnum = atoi(ans);
		if (lnum == 0)
		{
			break;		/* out of while and return */
		}
		if (lnum <= 20)
		{
			editline(lnum);
		}
		else			/* else slap on hand and loop around */
		{
			portsout("Aw come on, there can only be 20 lines!");
			portsout(CRLF);
			portsout("Shall we try that again?");
			portsout(CRLF);
		}
	}		
}
/*	end of function		*/

editline(line_no)
int	line_no;
{
	char	work_msg[(20*40)+40],	/* work message area */
		*work_ptr,		/* pointers */
		*msg_ptr,
		*new_ptr,
		new_line[40+1],		/* new line to replace in text */
		byte;
	int	i;		/* a counter */

	work_msg[0] = '\0';	/* init strings */
	new_line[0] = '\0';
	i = 0;

	work_ptr = &work_msg;	/* put the addr of work_msg in the pointer */
	msg_ptr  = &msg_text;	/* put the addr of the global message in ptr */
	new_ptr  = &new_line;	/* the new line too */

	if (line_no != 1)
	{
		while(byte = *msg_ptr++)	/* move msg to work area */
		{				/*  until end of msg */
			*work_ptr++ = byte;
			if (byte == '\r')		/* new line? */
			{
				if (++i >= (line_no - 1)) /* at the line to edit? */
				{
					break;		/* then stop moving */
				}
			}
		}
	}
#ifdef DEBUG
	if (debug)
	{
		printf("work_msg after move of 1st part = ");
		prthex(work_msg);
		printf("\n");
	}
#endif
	if (i < (line_no - 1))	/* if true then there ain't no such beast */
	{
		portsout("Hmmm, can't seem to find that line");
		portsout(CRLF);
		portsout("   give me another chance, ok?");
		portsout(CRLF);
		return;		/* here we go round the mulberry bush */
	}
	portsout(CRLF);
	portsout("Line to edit is:");
	portsout(CRLF);
	portsout(">");

#ifdef DEBUG
	if (debug)
	{
		byte = *msg_ptr;
		printf("1st byte of old line = ");
		prthex(byte);
	}
#endif
	while((byte = *msg_ptr++) != '\r')	/* show the line to edit */
	{
		while(portoutstat());
		portout(byte);
	}
	portsout(CRLF);

	portsout("Enter corrected line:");
	portsout(CRLF);
	portsout(CRLF);
	portsout(">");
	portsin(new_line,40);
	portsout(CRLF);
	strcat(new_line,"\r\0");		/* put cr on end of new line */

	while (*work_ptr++ = *new_ptr++)	/* tag on new line to */
	{					/*  work area */
		;
	}
	*work_ptr--;				/* back up one */

#ifdef DEBUG
	if (debug)
	{
		printf("after move of new line to msg, msg_text =");
		prthex(msg_text);
	}
#endif
	while (*work_ptr++ = *msg_ptr++)	/* now move in rest of */
	{					/*  message to work area */
		;
	}
	*work_ptr++ = '\0';		/* for luck */

	strcpy(msg_text,work_msg);	/* replace the message with the */
					/*  new line inserted */
}
/*	end of function		*/

cmd_g()
{
	cmd_b();		/* same thing */
}
/*	end of function		*/

cmd_k()
{
	char	ans[11];
	int	kill_msg,
		fd,
		rc;

	portsout(CRLF);
	portsout("Enter message number to delete ==>");
	portsin(ans,10);
	portsout(CRLF);
	kill_msg = atoi(ans);
	fd = msgopen(2);		/* open i/o */
	if ((rc = msgread(fd,kill_msg)) == ERROR || rc == 0)
	{
		portsout("Can't delete that message");
		portsout(CRLF);
		return;
	}
	portsout("Deleting message...");
	portsout(CRLF);
	strcpy(msg_delete,"9");		/* mark for deletion */
	if (msgrewrt(fd,kill_msg) == ERROR) /* re-write the record just read */
	{
		portsout("Sorry, can't delete that message");
		portsout(CRLF);
		return;
	}
	msgclose(fd);
	portsout("Message ");
	portsout(ans);
	portsout(" has been deleted");
	portsout(CRLF);
	return;
}
/*	end of function		*/

cmd_q()
{
	char	ans[11];
	int	q_msg,
		fd,
		rc;

	if ((fd = msgopen(0)) == ERROR)		/* open input */
	{
		return(ERROR);
	}
	portsout(CRLF);
	portsout("Enter starting message number -- RETURN to exit  ==>");
	portsin(ans,10);
	q_msg = atoi(ans);
	if (q_msg == 0)			/* = 0 all done */
	{
		msgclose(fd);
		return;
	}
	portsout(CRLF);
	portsout(CRLF);
	portsout("No.\t\tDate\t\tSubject");
	portsout(CRLF);
	portsout("---------------------------------------------------");
	portsout(CRLF);

	while ((rc = msgread(fd,q_msg++)) != ERROR)
	{				/* read until eof or error */
#ifdef DEBUG
		if (debug)
		{
			printf("in quick scan loop  rc from read = %d\n",rc);
		}
#endif
		if (rc == 0)
		{
			continue;	/* not a valid msg */
		}
		portsout(msg_no);
		portsout("\t\t");
		portsout(msg_date);
		portsout("\t");
		portsout(msg_subject);
		portsout(CRLF);
		if (stop_that)		/* ctl-K ? */
		{
			stop_that = FALSE;
			break;
		}
	}
	portsout(CRLF);
	msgclose(fd);
}
/*	end of function		*/

cmd_r()
{
	char	msgno[10],
		*text,
		byte0;
	int	msg,
		fd,
		rc;

	if ((fd = msgopen(0)) == ERROR)		/* open input */
	{
		return(ERROR);
	}
	portsout("\n\n\r");
	portsout("Enter the message number to read ");
	portsout(" - RETURN to exit   ==>");

	while (!stop_that)
	{
		portsin(msgno,9);
		portsout(CRLF);
		msg = atoi(msgno);	/* make the no. integer */
#ifdef DEBUG
		if (debug)
		{
			printf("in cmd_r, after atoi, msg=%d\r",msg);
			printf("                      msgno=%s\n",msgno);
		}
#endif
		if (msg == 0)
		{
			break;
		}
		if ((rc = msgread(fd,msg)) == 0 || rc == ERROR)
		{
			portsout("Message ");
			portsout(msgno);
			portsout(" not found\r\n");
		}
		else
		{
#ifdef DEBUG
			if (debug)
			{
				printf("msg no. returned from msgread=%d\n",
					rc);
			}
#endif
			portsout(CRLF);
			portsout("No. ");
			portsout(msg_no);
			portsout("  ");
			portsout(msg_date);
			portsout("  ");
			portsout(msg_time);
			portsout("\n\r");

			portsout("From: ");
			portsout(msg_from);
			portsout("   To: ");
			portsout(msg_to);
			portsout(CRLF);

			portsout("Subject: ");
			portsout(msg_subject);
			portsout(CRLF);
			portsout(CRLF);

			text = &msg_text;
#ifdef DEBUG
			if (debug)
			{
				printf("text pointer = ");
				prthex(text);
				printf("\n");
			}
#endif
			while (byte0 = (*text++))
			{
				while(portoutstat());
				portout(byte0);
				if (byte0 == '\r')
				{
					while(portoutstat());
					portout('\n');
				}
				if (stop_that)
				{
					stop_that = FALSE;
					msgclose(fd);
					return;
				}
			}
		}
		portsout(CRLF);
		portsout("# or RETURN to exit  ==>");
	}
	msgclose(fd);
}
/*	end of function		*/

cmd_s()
{
	char	ans[11];
	int	q_msg,
		fd,
		rc;

	if ((fd = msgopen(0)) == ERROR)		/* open input */
	{
		return(ERROR);
	}
	portsout(CRLF);
	portsout("Enter starting message number -- RETURN to exit ) ==>");
	portsin(ans,10);
	q_msg = atoi(ans);
	if (q_msg == 0)
	{
		return;
	}
	portsout(CRLF);
	portsout(CRLF);
	portsout("No.\tDate\t\tFrom\t\tTo");
	portsout(CRLF);

	portsout("  Subject");
	portsout(CRLF);

	portsout("------------------------------------------");
	portsout(CRLF);

	while ((rc = msgread(fd,q_msg++)) != ERROR)
	{				/* read until eof or error */
		if (rc == 0)
		{
			continue;	/* not a valid msg */
		}
		portsout(msg_no);
		portsout("\t");
		portsout(msg_date);
		portsout("\t");
		portsout(msg_from);
		portsout("\t");
		portsout(msg_to);
		portsout(CRLF);

		portsout("  ");
		portsout(msg_subject);
		portsout(CRLF);
		portsout(CRLF);
		if (stop_that)
		{
			stop_that = FALSE;
			break;
		}
	}
	portsout(CRLF);
	msgclose(fd);
}
/*	end of function		*/

cmd_x()
{
	xpert = !xpert;		/* flip the expert mode */
}
/*	end of function		*/

cmd_h()		/* type help file */
{

	if (fopen(HELP,ibuf) == ERROR)
	{
		portsout("\r\nSorry, Help not available\r\n");
	}
	else
	{
		porttype(ibuf);	/* type a file to port */
		fclose(ibuf);	/* close it to free up fcb */
	}
}
/*	end of function		*/


signon()	
{
	char	byte0;

	int	i;

	portsout(PGMNAME);
	portsout("Version");
	portsout(VERSION);
	portsout(LASTDATE);
	portsout("\r\r\n");
/*
*			type welcome file
*/
	if (fopen(WELCOME,ibuf) == ERROR)
	{
		portsout("\r\nWelcome file not present, welcome anyhow\r\n");
	}
	else
	{
		porttype(ibuf);	/* type a file to port */
		fclose(ibuf);
	}


	portsout("On at ");	/* give date and time of signon */
	portsout(time);
	portsout("  ");
	portsout(week);
	portsout("  ");
	portsout(date);
	portsout(CRLF);
/*
*			get name
*/
	while (1)
	{
		portsout("\r\n\n");
		portsout("Enter your first name   ==>");
		portsin(w_fname,20);
		portsout(CRLF);
		portsout("Enter your last name    ==>");
		portsin(w_lname,20);
		portsout("\r\r\n");
		portsout("Hello ");
		portsout(w_fname);
		portsout(" ");
		portsout(w_lname);
		portsout(CRLF);
		portsout("Did I get your name right?  (Y/N) ==>");
		while(portinstat());
		byte0 = portin();	/* get answer */
		portout(byte0);		/* send it back */
		if (byte0 == 'Y')
		{
			portsout(CRLF);
			if (checkuser())
			{
#ifdef DEBUG
				if (debug)
				{
					printf("found valid user\n");
				}
#endif
				break;	/* got a valid user */
			}
#ifdef DEBUG
			if (debug)
			{
				printf("no valid user found\n");
			}
#endif
		}
	}
/*
*			type bulletins file
*/
	if (fopen(BULLETINS,ibuf) == ERROR)
	{
		portsout("\r\nNo Bulletins today\r\n");
	}
	else
	{
		porttype(ibuf);	/* type a file to port */
		fclose(ibuf);
	}

	hdrread();		/* read msgs header file into memory */
/*
*			end of signon
*/

}
/*	end of function		*/
		
checkuser()			/* returns 1 when find a match */
{
	char	name[50];

	if (fopen(USERS,ibuf) == ERROR)
	{
		portsout("\r\nUser file not present, will log you on as");
		portsout(" a GUEST\r\n");
		strcpy(u_fname,"GUEST");
		strcpy(u_lname," ");
		strcpy(u_password,"MPK0");
		return(TRUE);
	}

	portsout("\nChecking user file...\r\n");

	while (readuser(ibuf) != 0)	/* look until eof on users file */
	{
#ifdef DEBUG
		if (debug)
		{
			printf("    u_fname=%s\n",u_fname);
			printf("    w_fname=%s\n",w_fname);
			printf("<cr>\n");
			getchar();
		}
#endif
		if ((strcmp(u_fname,w_fname) == OK) &&
		    (strcmp(u_lname,w_lname) == OK))
		{
#ifdef DEBUG
			if (debug)
			{
				printf("match on names\n");
				printf("u_fname=%s\n",u_fname);
				printf("u_lname=%s\n",u_lname);
				printf("w_fname=%s\n",w_fname);
				printf("w_lname=%s\n",w_lname);
				printf("<cr>\n");
				getchar();
			}
#endif
			if (checkpass() == OK)
			{
				fclose(USERS,ibuf);
				return(TRUE);	/* passwords match */
			}
			else
			{
				fclose(USERS,ibuf);
				return(FALSE);	/* passwords dont match */
			}
		}
	}
	fclose(USERS,ibuf);
	newuser();		/* not on file, so add 'em */
#ifdef DEBUG
	if (debug)
	{
		printf("end of while in checkuser\n");
	}
#endif
	return(TRUE);
}
/*	end of function		*/

checkpass()			/* returns TRUE (1) when equal passwords */
{
	char	*passptr;
	int	j,
		i;

	passptr = w_password;	/* give passptr the addr of w_password */

	for (i = 0; i < 3; i++)		/* give 'em 3 tries to get it right */
	{
		portsout("\r\nEnter your password  ==>");
		for (j=0; j < 4; j++)	/* password is 4 long */
		{
			while(portinstat());
/*			*passptr++ = portin();
*/
			w_password[j] = portin();
			portout('_');
		}
/*		*passptr++ = '\0';
*/
		w_password[j] = '\0';

		portsout(CRLF);
#ifdef DEBUG
		if (debug)
		{
			printf("u_password=%s\n",u_password);
			printf("w_password=%s\n",w_password);
			printf("passptr=%s\n",passptr);
			printf("<cr>\n");
			getchar();
		}
#endif
		if ((strcmp(w_password,u_password)) == OK)
		{
#ifdef DEBUG
			if (debug)
			{
				printf("passwords match\n");
			}
#endif
			return(OK);	/* passwords match */
		}
	}
	if (i >= 3)
	{
		portsout("\r\nSorry, but three tries is all you get\r\n");
		portsout("  Goodby....\r\n\r\n");
		return(ERROR);		/* bad try on password */
	}
}
/*	end of function		*/

newuser()
{
	char	pword[5];
	int	j;

#ifdef DEBUG
	if (debug)
	{
		printf("in newuser\n");
	}
#endif
	portsout("Loading new user file...\r\n");
	filecpy(USERS,USERX);	/* copy file */

	strcpy(u_fname,w_fname);
	strcpy(u_lname,w_lname);

	strcpy(u_time1,time);
	strcpy(u_time2,time);

	strcpy(u_date1,"\0");
	strcat(u_date1,mm);
	strcat(u_date1,"/");
	strcat(u_date1,dd);
	strcat(u_date1,"/");
	strcat(u_date1,yy);

	strcpy(u_date2,"\0");
	strcat(u_date2,mm);
	strcat(u_date2,"/");
	strcat(u_date2,dd);
	strcat(u_date2,"/");
	strcat(u_date2,yy);

	portsout(CRLF);
	portsout("Welcome, as a new user I need a few pieces");
	portsout(" of information\r\n\r\n");

	portsout("Please enter the City, State you are from ===>");
	portsin(u_city,30);
	portsout(CRLF);

	while (1)
	{
		portsout(CRLF);
		portsout("Ok, now I need a 4 character password ");
		portsout("                ===>");
		for (j=0; j < 4; j++)	/* password is 4 long */
		{
			while(portinstat());
			u_password[j] = portin();
			portout('_');
		}
		u_password[j] = '\0';
		portsout("\r\n\n");

		portsout("Just to make sure we both remember it,");
		portsout(" enter it again ===>");
		for (j=0; j < 4; j++)	/* password is 4 long */
		{
			while(portinstat());
			w_password[j] = portin();
			portout('_');
		}
		w_password[j] = '\0';
		portsout("\r\n\n");

#ifdef DEBUG
		if (debug)
		{
			printf("u_password=%s\n",u_password);
			printf("w_password=%s\n",w_password);
			printf("<cr>\n");
			getchar();
		}
#endif
		if (strcmp(u_password,w_password) == OK)
		{
			break;	/* get it right twice, then get out */
		}
		portsout(CRLF);
		portsout("hmmmm, one of us forgot it already\r\n");
		portsout("	let's try it again!!\r\n\n");
	}


	wrtuser(iobuf);		/* write a user record */

	putc(CPMEOF,iobuf);	/* put hex 1a on end of file */
	fflush(iobuf);		/* ok leor, just for you */
	fclose(iobuf);		/* close the file after new user added */
	unlink(USERS);		/* delete current file */
	rename(USERX,USERS);	/* rename temp file to current file */

#ifdef DEBUG
	if (debug)
	{
		printf("ending newuser()\n");
	}
#endif
	
}
/*	end of function		*/

readuser(buf)	/* read a record from the user file */
		/*  returns 0 on eof, 1 on good read */
char	*buf;
{
	int	code;

	code = fscanf(buf,"%s~%s~%s~%s~%s~%s~%s~%s~",
			u_fname,
			u_lname,
			u_password,
			u_time1,
			u_date1,
			u_time2,
			u_date2,
			u_city);

#ifdef DEBUG
	if (debug)
	{
		printf("in readuser	u_fname   =%s\n",u_fname);
		printf("		u_lname   =%s\n",u_lname);
		printf("		u_password=%s\n",u_password);
		printf("		u_date1   =%s\n",u_date1);
		printf("		u_time1   =%s\n",u_time1);
		printf("		u_date2   =%s\n",u_date2);
		printf("		u_time2   =%s\n",u_time2);
		printf("		u_city    =%s\n",u_city);
		printf("		field count =%d\n",code);
	}
#endif
	if (code == -1 || code == 0)
	{
		return(0);	/* all done, hit eof */
	}
	else
	{
		return(1);	/* good read */
	}
}
/*	end of function		*/

wrtuser(buf)
char	*buf;
{
	int	code;

#ifdef DEBUG
	if (debug)
	{
		printf("in wrtuser	u_fname   =%s\n",u_fname);
		printf("		u_lname   =%s\n",u_lname);
		printf("		u_password=%s\n",u_password);
		printf("		u_date1   =%s\n",u_date1);
		printf("		u_time1   =%s\n",u_time1);
		printf("		u_date2   =%s\n",u_date2);
		printf("		u_time2   =%s\n",u_time2);
		printf("		u_city    =%s\n",u_city);
	}
#endif

	code = fprintf(buf,"%s~%s~%s~%s~%s~%s~%s~%s~",
			u_fname,
			u_lname,
			u_password,
			u_time1,
			u_date1,
			u_time2,
			u_date2,
			u_city);
	if (code == -1)
	{
		printf("User file has problem writing\n");
		printf("  write failed...\n");
	}
	else
	{
		fputs("\n",buf);	/* put cr,lf on end of each record */
	}

#ifdef DEBUG
	if (debug)
	{
		printf("leaving wrtuser\n");
	}
#endif
}
/*	end of function		*/

filecpy(from,to)	/* copy file "from" to file "to" */
char	*from,		/* this function leaves the "to" file open */
	*to;		/*  as output, because BDS has no append mode */
{
	char	rcd[300];

#ifdef DEBUG
	if (debug)
	{
		printf("in filecpy\n");
	}
#endif

	if (fopen(from,ibuf) == ERROR)
	{
		portsout("\r\nOpen failed for ");
		portsout(from);
		portsout(CRLF);
		return(ERROR);
	}
	if (fcreat(to,iobuf) == ERROR)
	{
		portsout("\r\nCreate failed for ");
		portsout(to);
		portsout(CRLF);
		return(ERROR);
	}

	while (fgets(rcd,ibuf))
	{
		fputs(rcd,iobuf);
	}

	fclose(ibuf);

#ifdef DEBUG
	if (debug)
	{
		printf("leaving filecpy\n");
	}
#endif
	return(OK);
}
/*	end of function */

fileapp(file1,file2)		/* append file2 to file1 */
char	*file1,
	*file2;
{
	char	rcd[300];

#ifdef DEBUG
	if (debug)
	{
		printf("in fileapp\n");
	}
#endif

	if (fopen(file1,ibuf) == ERROR)			/* open file1 input */
	{
		printf("\r\nOpen failed for %s\r\n",file1);
		return(ERROR);
	}
	if (fcreat("TEMPAPP.$$$",iobuf) == ERROR)	/* open temp output */
	{
		printf("\r\nCreate failed for TEMPAPP.$$$\r\n");
		return(ERROR);
	}

	while (fgets(rcd,ibuf))		/* copy file1 to temp */
	{
		fputs(rcd,iobuf);
	}
	fclose(ibuf);			/* close file1 */
	if (fopen(file2,ibuf) == ERROR)	/* open  file2 input */
	{
		printf("\r\nOpen failed for %s\r\n",file2);
		return(ERROR);
	}
	while (fgets(rcd,ibuf))		/* append file2 to temp */
	{
		fputs(rcd,iobuf);
	}
	fclose(ibuf);			/* close file2 */
	putc(CPMEOF,iobuf);		/* put hex 1a on end of file */
	fflush(iobuf);			/* close temp */
	fclose(iobuf);
	unlink(file1);			/* delete file1 */
	rename("TEMPAPP.$$$",file1);	/* rename temp to file1 */

#ifdef DEBUG
	if (debug)
	{
		printf("leaving fileapp\n");
	}
#endif
	return(OK);
}
/*	end of function */

signoff()
{
	;
}
/*	end of function		*/

/*	end of program  	*/
