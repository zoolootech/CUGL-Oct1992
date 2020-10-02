/*
	bbscdef.h

	Global defines and storage definitions for use by
	BBSc system.

	NOTE:	BDS C treats global variable like a FORTRAN COMMON
		block, hence all programs that use these externals
		must define them in the same order and for the same
		length, AND they all must be recompiled if you change
		anything in here!
				Mike Kelly

	07/23/83 v1.1	Added #define of CLOCK, NOCLOCK, LCDAV, LCEMPTY.
	07/05/83 v1.0	Added #define DEBUG to control compilation
			of "if(debug)" code.
	07/03/83 v1.0	Added stop_that (indicates a ctl-K or K received
			deleted).
			from the keyboard in portout routine).
	06/28/83 v1.0	Added msg_delete (indicates if message is
	06/25/83 v1.0	updated
	05/17/83 v1.0	written
*/


#define DEBUG	  1			/* comment this out so the */
					/*  "if(debug)" code does NOT */
					/*  get compiled */


/*
#define CLOCK	  1			/* if you have a clock board then */
					/*  this define wants to be  */
					/*  uncommented and NOCLOCK should */
					/*  be commented */
*/

#define NOCLOCK	  1			/* uncomment this if you do NOT have */
					/*  a clock board */


/* --------------------------------------------------------------------- */

#define CLEAR	  'E'			/* clear screen on z-19 */
#define ESC	  0x1b
#define CR	  0x0d
#define LF	  0x0a
#define BELL	  0x07			/* Avon calling */
#define CRLF	  "\r\n"		/* <cr><lf> */
#define CTL_D	  0x04			/* <ctl-d> */
#define CTL_K	  0x0b			/* <ctl-k> */
#define CTL_X	  0x18			/* <ctl-x> */

#define BREAKCNT  5000			/* max count to allow for    */
					/*   status routines to loop */
#define STATMAX	  2000			/* used by stat debug routine */

#define LCDATA	  0x02			/* local console data port   */
#define LCSTAT	  0x03			/* local console status port */
#define LCDAV	  0x02			/* local console data avail. mask */
#define LCEMPTY	  0x01			/* local console transmit buffer */
					/*  empty mask */

#define GBI4PDATA 0x10			/* data port   */
#define GBI4PSTAT GBI4PDATA+1		/* status port */
#define GBI4PCMD  GBI4PDATA+3		/* command port */
#define GBI4PSEL  GBI4PDATA+7		/* user select port */
#define GBI4PMODE GBI4PDATA+2		/* mode port */

#define SDSREADY  0x80			/* data set ready */
#define SCARRIER  0x40			/* carrier detect */
#define SFRAME	  0x20			/* framing error  */
#define SOVERRUN  0x10			/* overrun error  */
#define SDAV	  0x02			/* character waiting */
#define SEMPTY	  0x01			/* transmit buffer empty */

#define USER5	  5			/* user number 5=not used    */
#define USER6	  6			/* user number 6=chronograph */
#define USER7	  7			/* user number 7=Smart modem */

#define WELCOME	  "WELCOME.BBS"		/* file name */
#define BULLETINS "BULLETIN.BBS"	/* file name */
#define USERS	  "USERS.BBS"		/* file name */
#define USERX	  "USERS.$$$"		/* file name */
#define MESSAGES  "MESSAGES.BBS"	/* file name */
#define MSGX1	  "MESSAGES.$$1"	/* file name */
#define MSGX2	  "MESSAGES.$$2"	/* file name */
#define HELP	  "HELP.BBS"		/* file name */

/*			defines used by the bbscfile.c routine */

#define HEADER	"HEADER.BBS"	/* file name */
#define MSG1MAX 104		/* max. piece of message to put in 128 */
				/*  byte chunk of msg file */
#define MSGSECT 128		/* size of message sector */

/*			end of defines used by the bbscfile.c routine */

			/* global variables */
char    portin(),			/* in routine returns a character */
	*gb4sin(),			/* returns pointer to character */

	ibuf[BUFSIZ],			/* handy input buffer */
	obuf[BUFSIZ],			/* another handy buffer */
	iobuf[BUFSIZ],			/* buffer used to read/write user file */

	w_fname[25],			/* first name on signon */
	w_lname[25],			/* last  name on signon */
	w_password[10],			/* password   on signon */
	*passptr;			/* ptr to password */

			/* define user file fields */
char	u_fname[25],
	u_lname[25],
	u_password[5],
	u_time1[12],			/* first time on system */
	u_date1[9],			/* first date on system */
	u_time2[12],			/* last  time on system */
	u_date2[9],			/* last  date on system */
	u_city[31];			/* city, state */

			/* define message file fields	*/
char	msg_no[11],			/* message number */
	msg_date[9],			/* date of message */
	msg_time[15],			/* time of message */
	msg_to[21],			/* who message is to */
	msg_from[21],			/* who message is from */
	msg_pass[11],			/* password */
	msg_subject[21],		/* what it's all about */
	msg_text[842],			/* text of message 20 lines */
					/*  by 40 wide */
	msg_delete[2];			/* indicates msg status */
					/*  0 = active */
					/*  9 = deleted */


int	debug,				/* switch for debug mode */
	offline,			/* switch for offline mode */
	xpert,				/* switch */
	active,				/* indicates if logged on */
	statcnt,			/* count used for status routines */
	stop_that;			/* switch - true indicates ctl-K */
					/*  or K received in portout routine */
			/* date/time fields - used by bbsclock.c */
char	mm[3],
	dd[3],
	yy[3],
	month[20],			/* ie. January */
	day[3],				/* dd */
	year[5],			/* 19yy */
	date[30],			/* ie. January 31, 1983 */ 
	week[20],			/* day of week Monday, Tuesday... */
	time[12];			/* time of day  HH:MM:SS xM */
					/*          where x = A or P */

/*			variables used by the bbscfile.c routine */

char	h_next_msg[10],		/* next avail. message # */
	h_date[10];		/* last update date of header file */
int	h_next;			/* next avail. message # in integer */

/*			end of variables used by bbscfile.c */

			/* end of globals */

/* end of program */

