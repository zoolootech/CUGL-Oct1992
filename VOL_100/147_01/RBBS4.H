/************************************************************************
* RBBS4.H

Release 7: RBBS 4.1 Edit 02  27 May 84
Release 6: RBBS 4.1 Edit 01  30 Apr 84
Release 5: RBBS 4.1 Edit 00  03 Mar 84

Release 4: RBBS 4.0 Edit 21  26 Feb 84
Release 3: RBBS 4.0 Edit 20  04 Feb 84
Release 2: RBBS 4.0 Edit 19  30 Jan 84
Release 1: RBBS 4.0 Edit 18  29 Jan 84

*/

#define	VERSION	"RBBS 4.1 Edit 02 (JCG/FJW - 27 May 84)"

/************************************************************************
* Set PERSONLY to TRUE if generating a system to use PERSONAL only
*/

#define	PERSONLY	FALSE	/* TRUE: PERSONAL only			*/

/************************************************************************
* Set NOSITE to TRUE if you have LASTREAD set and don't want your users
* asked for a site name (nor display it with the U Command).
*/

#define	NOSITE		FALSE	/* TRUE: no site prompt when LASTREAD is
				   set to TRUE				*/

/************************************************************************
* Set NEWFLAG to '*' if you want new users denied access to CP/M, else
* set to '+'.
*/

#define	NEWFLAG		'*'	/* '*' = CP/M Access DENIED to new users
				   '+' = CP/M Access OK
				   '%' = Same as *, but no E or K	*/

/************************************************************************
* Set RULER TRUE if you want the non-expert to see a ruler line at the
* beginning of message text entry.
*/

#define	RULER		TRUE	/* TRUE: ruler line displayed		*/

/************************************************************************
* Set PCREND FALSE if message text input is to be ended by a CR on a new
* line.  (Edit the MSGHELP.CCC file to show this.)  Otherwise, if set
* TRUE, then only a PERIOD (".") followed by a CR on a new line ends
* input.
*/

#define	PCREND		FALSE	/* TRUE: only .<cr> ends msg text input	*/

/************************************************************************
* Set SENDOK TRUE to allow sending messages to non-users
*/

#define	SENDOK		TRUE	/* TRUE: allow sending to any name	*/

/************************************************************************
* If the case of the user password is to be significant, set PWCASE to
* TRUE:
*/

#define	PWCASE		TRUE	/* TRUE: case of password is significant*/

/************************************************************************
* Set NOUCMD to TRUE if you wish to make the U Command only for the
* SYSOP:
*/

#define	NOUCMD		FALSE	/* TRUE: U Command SYSOP only		*/

/************************************************************************
* Set PREREG to TRUE if you intend to allow only preregistered callers
* access this program.  You can preregister users (or modify their
* entries) with UTIL.
*/

#define	PREREG		FALSE	/* TRUE: only preregistered users	*/

/************************************************************************
* SITEPROMPT will appear as the site prompt for the new user, normally,
* "City, State"
*/

#define	SITEPROMPT	"City, State"

/************************************************************************
* Set CHAINEXIT to FALSE if you do not wish to chain to some program,
* like MENU, renamed to AUTO.COM upon exit with the C Command
*/

#define	CHAINEXIT	FALSE	/* TRUE: C command chains to AUTO.COM	*/
#define	CHAIN		"AUTO"	/* Default name of the chained program	*/

/************************************************************************
* Set HANGUP to FALSE if you do not wish to chain to some program that
* will hang up the call.  The G Command will exit to this program, as
* well as failed password attempts.
*/

#define	HANGUP		TRUE	/* TRUE: G command chains to HANGUP.COM	*/
#define	HANG		"HANGUP"/* Default name of the HANGUP program	*/

/************************************************************************
* LOCKEM is only useful in a multi-user environment, such as TurboDOS,
* to avoid conflicts while updating USER.CCC or the current .MSG file.
* If this DEFINE is present, both a lokrec and a frerec function must be
* provided, similar to the sample described in TDOSFN.CSM
*/

#define	LOCKEM		FALSE	/* TRUE: lock/free record used		*/

/************************************************************************
* If you are not providing a daytim function similar to the sample in
* DAYTIM.CSM, set DATETIME to FALSE.  The daytim function must return
* the address of a null-terminated string in HL, which is exactly 19
* characters long, including the trailing null.
*/

#define	DATETIME	FALSE	/* TRUE: daytim function used		*/

/************************************************************************
* Whether or not you are using daytim above, set ZONE to your
* three-character time zone.
*/

#define	ZONE		"MST"	/* System Time Zone			*/

/************************************************************************
* If RBBS4 is not being used as a front-end program, and some other
* program creates a LASTCALR file containing the first and last name of
* the caller, set LASTREAD to TRUE and to the location of the LASTCALR
* file.  RBBS4 will use that name and assume the caller has already been
* validated.  This DEFINE should NOT be used un a multi-user environment
* unless each LASTCALR file is a different name for each "user".
*/

#define	LASTREAD	FALSE		/* TRUE:  LASTCALR file is read	*/
#define	LASTR		"B:LASTCALR"	/* Default name of file		*/

/************************************************************************
* If RBBS4 is being used as a front-end program, and other programs,
* such as XMODEM, expect the contents of LASTCALR to be valid, set this
* DEFINE to TRUE, and RBBS4 will correctly write to LASTCALR.
*/

#define LASTWRITE	TRUE		/* TRUE: LASTCALR file is written*/
#define LASTW		"15/A:LASTCALR"	/* Default name of file		*/

/************************************************************************
* The following two DEFINEs determine where RBBS4 expects to find the
* files it needs.  By BDS-C convention, you may use user/drive here, as
* in "15/B:", if that's where the files are.
*/

#define	DRIVE		"15/A:"	/* Drive used to find ALL RBBS4		*/
#define	DSK(fn)		"15/A:fn"/* support files			*/

/************************************************************************
* This last customizable DEFINE may be a string of up to 30 characters,
* and is used only by UTIL to set up the initial entry in USERS.CCC,
* that of the SYSOP.
*/

#define	SYSITE		"El Paso, TX"	/* Home site of SYSOP		*/

/************************************************************************/
/*		     END OF CUSTOMIZATION DEFINES			*/
/************************************************************************/

#define	SITELGTH	29
#define	SUMSIZE		30
#define	NAMELGTH	30
#define	MLINESIZ	64
#define	MLINELIM	24
#define	MAXMSGS		200
#define	MSIZE		(MLINELIM+1)*MLINESIZ

#define	YES		TRUE
#define	NO		FALSE

#ifdef	DATETIME
	char	sysdate[23];	/* Storage for current DATE/TIME	*/
	char	logdate[23];	/* Logon DATE/TIME			*/
#else
	char	sysdate[9];	/* Storage for current DATE		*/
	char	logdate[9];	/* Logon DATE				*/
#endif


	char	cmsgfile[9];	/* Current .MSG file name		*/
	char	msgfile[18];	/* Storage for active .MSG file name	*/
	char	ndxfile[18];	/* Storage for Index file name		*/
	int	fileflag;	/* Tells which files have been seen	*/
	int	cchoice;	/* Current .MSG file number -1		*/

	int	sysop;		/* set if sysop logged in		*/
	int	expert;		/* Expert Flag				*/
	int	rtnok;		/* Return OK Flag			*/
	int	personal;	/* Set TRUE if current msgfile is	*/
				/* PERSONAL.MSG				*/

	int	mptr;		/* Pointer to Current Message		*/

				/* Info loaded from .MSG file	 	*/
	int	actmsg,frstmsg,lastmsg,msgct,lstseen;
	int	mno[MAXMSGS],mndx[MAXMSGS];

				/* Storage for Current Message  	*/
	char	m[MSIZE];
	char	*lp[MLINELIM+1];

	char	tmpstr[SECSIZ];	/* Handy temporary string space		*/


struct	_sum
	{
	int	msgno;
	int	lct;
	int	mstat;
#ifdef	DATETIME
	char	date[23];
#else
	char	date[9];
#endif
	char	fnm[NAMELGTH+1];
	char	tnm[NAMELGTH+1];
	char	subj[SUMSIZE];
	};

/* Offsets for summary record items					*/

#define	LNS	5
#define	STAT	8
#define	DT	11
#define	FM	34
#define	TO	65
#define	SU	96


struct	_user
	{
	int	recno;			/* Record Number of Current User*/
	int	maxno;			/* Record Number of Max User	*/
	int	lstmsg[7];		/* Last message seen		*/
	int	bellflag;		/* BELL Flag			*/
	int	xpert;			/* EXPERT Flag			*/
	char	ustat[2];		/* MF Status			*/
	char	nm[NAMELGTH+1];		/* Name of Current User		*/
	char	from[SITELGTH+1];	/* City, State of Current User	*/
	char	pwd[7];			/* PASSWORD 			*/
#ifdef	DATETIME
	char	lastlog[23];		/* Last LOGON DATE/TIME		*/
#else
	char	lastlog[9];		/* Last LOGON DATE		*/
#endif
	};

/* Offsets for user record items					*/

#define	NM	0		/* Name					*/
#define	PRV	31		/* User Status				*/
#define	UBL	33		/* Bell Flag				*/
#define	UXP	35		/* Expert Flag				*/
#define	PWD	37		/* Password				*/
#define	LLG	44		/* Last login				*/
#define	FRM	67		/* From					*/
#define	LMG0	97		/* Last msg seen (GENERAL)		*/
#define	LMG1	101		/* Last msg seen			*/
#define	LMG2	105		/* Last msg seen			*/
#define	LMG3	109		/* Last msg seen			*/
#define	LMG4	113		/* Last msg seen			*/
#define	LMG5	117		/* Last msg seen			*/
#define	LMG6	121		/* Last msg seen			*/


/*  The following are globals for use with RBBSCIO			*/

	char	pp[SECSIZ];	/* Previous Prompt string		*/
	int	ocnt;		/* Output character count		*/
	int	belflg;		/* BELL Flag				*/
	int	cflg;		/* Character Flag:
					2 = use Mask Characters
					3 = output CRLF after input	*/
	char	sav[SECSIZ];	/* Typeahead buffer			*/
	int	icnt;		/* Input character count		*/
	int	colcnt;		/* Horizontal curpos			*/

/*********************	 END OF RBBS4.H   *******************************/
0		/* Name					*/
#define	PRV	31		/* User Status				*/
#define	UBL