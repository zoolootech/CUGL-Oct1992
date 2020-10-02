/****************************************************************
* UTIL.C							*

Release 5: RBBS 4.1 Edit 00 - Names now one large field
			    - Activated lstmsg for 1st 7 non-PERSONALs
			    - Inserted PERSONLY in main
Release 4: RBBS 4.0 Edit 21 - added confirmation request in creating
			      a new USERS.CCC file
			    - added settable bias value in creating
			      a newer CALLERS.CCC file
			    - repaired superuser
Release 1: RBBS 4.0 Edit 18

* Function included in this file is:

* main		calls chk_user, subject, select_subj, and performs
		the main command loop

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
	select_subj(1,s);
#else
	strcpy(cmsgfile,"PERSONAL");
	sprintf(msgfile,"%s%s%s",DRIVE,capstr(cmsgfile),".MSG");
#endif
	msgct = load_cntrl(0,s);
	msg_info(msgct,TRUE);
	while(TRUE)
	{
outstr("**************************************************************",2);
#if	!PERSONLY
outstr("<1> Write active messages          <5> Add a new subject",2);
#else
outstr("<1> Write active messages",2);
#endif
outstr("<2> Read msgs <Random selection>   <6> Manipulate User Entries",1);
outstr("<3> Setup a new USERS.CCC file     <7> Cleanup USERS.CCC file",1);
#if	!PERSONLY
outstr("<4> Select a Subject file          <8> Enter a long message",1);
#else
outstr("                                   <8> Enter a long message",1);
#endif
		outstr("<9> Quit",2);
		crlf(1);
		outstr("Enter choice: ",4);
		instr("",c,1);
		crlf(1);
		switch(*c-0x30)
		{
			case 1:
				rite_active(msgct,s);
				outstr("Loading new control",2);
				msgct = load_cntrl(msgct,s);
				msg_info(msgct,TRUE);
				for ( i = 1; i <= msgct; i++)
					prnt_ndx(i);
				break;

			case 2:
				crlf(1);
				read_msgs(msgct,s);
				break;	

			case 3:
				if (getyn("Are you sure"))
					nu_user();
				break;

#if	!PERSONLY
			case 4:
				select_subj(0,s);
				msgct = load_cntrl(0,s);
				msg_info(msgct,FALSE);
				break;

			case 5:
				add_subj();
				break;
#endif

			case 6:
				superuser();
				break;

			case 7:
				rite_user(u);
				break;

			case 8:
				if ( (ufd = open(DSK(USERS.CCC),2)) == ERROR)
					ioerr("Cannot open USERS.CCC");
				logon(ufd,u);
#if	!PERSONLY
				select_subj(FALSE,s);
#endif
				close(ufd);
				if(!prnt_instr())
					break;
				ntrlong(ufd,u,s);
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
r("Loading new control",2);
				msgct = load_cntrl(msgct,s);
				ms