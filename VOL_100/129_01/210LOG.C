/************************************************************************/
/*				log.c					*/
/*									*/
/*	   userlog code for  Citadel bulletin board system		*/
/************************************************************************/

/************************************************************************/
/*				history 				*/
/*									*/
/* 84Dec28 HAW	findPerson() moved from MSG.C to LOG.C. 		*/
/* 84Dec12 HAW	Fixed kill acct. bug so killed acct doesn't become login*/
/* 84Jun23 HAW&JLS  Eliminating unused local variables using CRF.	*/
/* 84Jun19 JLS	Fixed terminate so that Mail> doesn't screw up SYSOP.	*/
/* 84Apr04 HAW	Started upgrade to BDS C 1.50a. 			*/
/* 83Feb27 CrT	Fixed login-in-Mail> bug.				*/
/* 83Feb26 CrT	Limited # new messages for new users.			*/
/* 83Feb18 CrT	Null pw problem fixed.					*/
/* 82Dec06 CrT	2.00 release.						*/
/* 82Nov03 CrT	Began local history file & general V1.2 cleanup 	*/
/************************************************************************/

#include <210ctdl.h>

/************************************************************************/
/*				contents				*/
/*									*/
/*	crypte()		encrypts/decrypts data blocks		*/
/*	findPerson()		load log record for named person	*/
/*	getLog()		loads requested userlog record		*/
/*	hash()			hashes a string to an integer		*/
/*	login() 		is menu-level routine to log caller in	*/
/*	logInit()		builds the RAM index to userlog.buf	*/
/*	newPW() 		is menu-level routine to change a PW	*/
/*	newUser()		menu-level routine to log a new caller	*/
/*	noteLog()		enters a userlog record into RAM index	*/
/*	putLog()		stores a logBuffer into citadel.log	*/
/*	PWSlot()		returns userlog.buf slot password is in */
/*	slideLTab()		support routine for sorting logTab	*/
/*	sortLog()		sort userlog by time since last call	*/
/*	storeLog()		store data in log			*/
/*	strCmpU()		strcmp(), but ignoring case distinctions*/
/*	terminate()		menu-level routine to exit system	*/
/*	zapLogFile()		erases & re-initializes userlog.buf	*/
/************************************************************************/

/************************************************************************/
/*	crypte() encrypts/decrypts data blocks				*/
/*									*/
/*  This was at first using a full multiply/add pseudo-random sequence	*/
/*  generator, but 8080s don't like to multiply.  Slowed down I/O	*/
/*  noticably.	Rewrote for speed.					*/
/************************************************************************/
#define b	fpc1
#define c	fi1
#define s	fi2
crypte(buf, len, seed)
char	  *buf;
unsigned  len, seed;
{
    seed	= (seed + cryptSeed) & 0xFF;
    b		= buf;
    c		= len;
    s		= seed;
    for (;  c;	c--) {
	*b++   ^= s;
	s	= (s + CRYPTADD)  &  0xFF;
    }
}

/************************************************************************/
/*	findPerson() loads log record for named person. 		*/
/*	RETURNS: ERROR if not found, else log record #			*/
/************************************************************************/
int findPerson(name, lBuf)
char		    *name;
struct logBuffer    *lBuf;
{
    int  h, i, foundIt, logNo;

    h	= hash(name);
    for (foundIt=i=0;  i<MAXLOGTAB && !foundIt;  i++) {
	if (logTab[i].ltnmhash == h) {
	    getLog(lBuf, logNo = logTab[i].ltlogSlot);
	    if (strCmpU(name, lBuf->lbname) == SAMESTRING) {
		foundIt = TRUE;
	    }
	}
    }
    if (!foundIt)    return ERROR;
    else	     return logNo;
}

/************************************************************************/
/*	getLog() loads requested log record into RAM buffer		*/
/************************************************************************/
getLog(lBuf, n)
struct logBuffer	*lBuf;
int			n;
{
    if (lBuf == &logBuf)   thisLog	= n;

    n	*= SECSPERLOG;

    seek(logfl, n, 0);
    if (read(logfl, lBuf, SECSPERLOG) == -1) {
	printf("?getLog-read fail");
    }

    crypte(lBuf, (SECSPERLOG*SECTSIZE), n);	/* decode buffer	*/
}

/************************************************************************/
/*	hash() hashes a string to an integer				*/
/************************************************************************/
int hash(str)
char *str;
{
    char toUpper();
    int  h, i, shift;

    for (h=shift=0;  *str;  shift=(shift+1)&7, str++) {
	h ^= (i=toUpper(*str)) << shift;
    }
    return h;
}

/************************************************************************/
/*	login() is the menu-level routine to log someone in		*/
/************************************************************************/
login(password)
char *password;    /* TRUE if parameters follow    */
{
    char getYesNo();
    int  foundIt, ltentry;

    foundIt =	 ((ltentry = PWSlot(password, /* load == */ TRUE)) != ERROR);

    if (foundIt && *password) {

	/* recite caller's name, etc:	 */
	mPrintf(" %s\n", logBuf.lbname);

	/* update userlog entries: */

	loggedIn     = TRUE;
	setUp(TRUE);

	showMessages(NEWoNLY, FALSE);

	listRooms(/* doDull== */ !expert);

	outFlag = OUTOK;
	if (
	    (
		logBuf.lbId[MAILSLOTS-1]
		-
		(logBuf.lbvisit[   logBuf.lbgen[MAILROOM] & CALLMASK   ]+1)
		< 0x8000
	    )
	    &&
	    logBuf.lbId[MAILSLOTS-1] - oldestLo 	  < 0x8000
	    &&
	    thisRoom != MAILROOM
	)   {
	    mPrintf("\n  * You have private mail in Mail> *\n ");
	}

    } else {
	/* discourage password-guessing: */
	if (strLen(password) > 1 && whichIO == MODEM)	pause(2000);
	if (!unlogLoginOk  &&  whichIO == MODEM)  {
	    mPrintf(" No record -- leave message to 'sysop' in Mail>\n ");
	} else if (getYesNo(" No record: Enter as new user"))	newUser();
    }
}

/************************************************************************/
/*	logInit() indexes userlog.buf					*/
/************************************************************************/
logInit()
{
    int i;
    int count;

    count = 0;

    /* clear logTab */
    for (i=0;  i<MAXLOGTAB;  i++) logTab[i].ltnewest	= ERROR;

    /* load logTab: */
    for (thisLog=0;  thisLog<MAXLOGTAB;  thisLog++) {
	printf("log#%d", thisLog);
	getLog(&logBuf, thisLog);

	/* count valid entries: 	    */
	if (logBuf.lbvisit[0] != ERROR) {
	    count++;
	    printf("  %s", logBuf.lbname);
	}
	putChar('\n');

	/* copy relevant info into index:   */
	logTab[thisLog].ltnewest = logBuf.lbvisit[0];
	logTab[thisLog].ltlogSlot= thisLog;
	logTab[thisLog].ltnmhash = hash(logBuf.lbname);
	logTab[thisLog].ltpwhash = hash(logBuf.lbpw  );
    }
    printf(" logInit--%d valid log entries\n", count);
    sortLog();
}

/************************************************************************/
/*	newPW() is menu-level routine to change one's password		*/
/*	since some Citadel nodes run in public locations, we avoid	*/
/*	displaying passwords on the console.				*/
/************************************************************************/
newPW()
{
    char oldPw[NAMESIZE];
    char pw[NAMESIZE];
    char *s;
    int  goodPW;

    /* save password so we can find current user again: */
    if (!loggedIn) {
	mPrintf("?Can't change pwd when not logged in!\n ");
	return ;
    }
    strcpy(oldPw, logBuf.lbpw);
    storeLog();
    do {
	echo	= CALLER;
	getNormStr(" new password", pw, NAMESIZE);
	echo	= BOTH;

	/* check that PW isn't already claimed: */
	goodPW = (PWSlot(pw, /* load == */ TRUE) == ERROR  &&  strlen(pw) >= 2);

	if (!goodPW) mPrintf("\n Poor password\n ");

    } while (!goodPW && (haveCarrier || whichIO==CONSOLE));

    doCR();
    PWSlot(oldPw, /* load == */TRUE);	/* reload old log entry 	    */
    pw[NAMESIZE-1] = 0x00;		/* insure against loss of carrier:  */

    if (goodPW	&&  strlen(pw) > 1) {	/* accept new PW:		    */
	strcpy(logBuf.lbpw, pw);
	logTab[0].ltpwhash	= hash(pw);
    }

    mPrintf("\n %s\n pw: ", logBuf.lbname);
    echo = CALLER;
    mPrintf("%s\n ", logBuf.lbpw);
    echo = BOTH;
}

/************************************************************************/
/*	newUser() prompts for name and password 			*/
/************************************************************************/
newUser() {
    char	getYesNo();
    char	fullnm[NAMESIZE];
    char	pw[NAMESIZE];
    int 	good, g, h, i, ok, ourSlot;
    unsigned	low;


    configure();	/* make sure new users configure reasonably	*/

    if (!expert)   tutorial("password.blb");

    do {
	/* get name and check for uniqueness... */
	do {
	    getNormStr(" Name", fullnm, NAMESIZE);
	    h	 = hash(fullnm);
	    for (i=0, good=TRUE;   i<MAXLOGTAB && good;   i++) {
		if (h == logTab[i].ltnmhash) good = FALSE;
	    }
	    if (
		!h
		||
		h==hash("Citadel")
		||
		h==hash("Sysop")
	    ) {
		good = FALSE;
	    }
	    /* lie sometimes -- hash collision !=> name collision */
	    if (!good) mPrintf("We already have a %s\n", fullnm);
	} while (!good	&&  (haveCarrier || whichIO==CONSOLE));

	/* get password and check for uniqueness...	*/
	do {
	    echo	= CALLER;
	    getNormStr(" password",  pw, NAMESIZE);
	    echo	= BOTH	;

	    h	 = hash(pw);
	    for (i=0, good=strLen(pw) > 1;  i<MAXLOGTAB  &&  good;  i++) {
		if (h == logTab[i].ltpwhash) good = FALSE;
	    }
	    if (!h)   good = FALSE;
	    if (!good) {
		mPrintf("\n Poor password\n ");
	    }
	} while( !good	&&  (haveCarrier || whichIO==CONSOLE));

	mPrintf("\n nm: %s", fullnm);
	mPrintf("\n pw: ");
	echo = CALLER;
	mPrintf("%s\n ", pw);
	echo = BOTH;
    } while (
	!getYesNo("OK")
	&&
	(haveCarrier || whichIO==CONSOLE)
    );

    if (ok && (haveCarrier || whichIO==CONSOLE)) {

	/* kick least recent caller out of userlog and claim entry:	*/
	ourSlot 	    = logTab[MAXLOGTAB-1].ltlogSlot;
	slideLTab(0, MAXLOGTAB-1);
	logTab[0].ltlogSlot = ourSlot;
	getLog(&logBuf, ourSlot);

	/* copy info into record:	*/
	strcpy(logBuf.lbname, fullnm);
	strcpy(logBuf.lbpw, pw);

	low = newestLo-50;
	if (oldestLo-low < 0x8000)   low = oldestLo;
	for (i=1;  i<MAXVISIT;	i++)   logBuf.lbvisit[i]= low;
	logBuf.lbvisit[ 			      0]= newestLo;
	logBuf.lbvisit[ 		   (MAXVISIT-1)]= oldestLo;

	/* initialize rest of record:	*/
	for (i=0;  i<MAXROOMS;	i++) {
	    if (roomTab[i].rtflags & PUBLIC) {
		g = (roomTab[i].rtgen);
		logBuf.lbgen[i] = (g << GENSHIFT) + (MAXVISIT-1);
	    } else {
		/* set to one less */
		g = (roomTab[i].rtgen + (MAXGEN-1)) % MAXGEN;
		logBuf.lbgen[i] = (g << GENSHIFT) + (MAXVISIT-1);
	    }
	}
	for (i=0;  i<MAILSLOTS;  i++)  {
	    logBuf.lbslot[i]	= 0;
	    logBuf.lbId[  i]	= oldestLo -1;
	}

	/* fill in logTab entries	*/
	logTab[0].ltpwhash	= hash(pw)	   ;
	logTab[0].ltnmhash	= hash(fullnm)	   ;
	logTab[0].ltlogSlot	= thisLog	   ;
	logTab[0].ltnewest	= logBuf.lbvisit[0];

	/* special kludge for Mail> room, to signal no new mail:   */
	roomTab[MAILROOM].rtlastMessage = logBuf.lbId[MAILSLOTS-1];

	loggedIn = TRUE;

	storeLog();

	listRooms(/* doDull== */ !expert);
    }
}

/************************************************************************/
/*	noteLog() notes logTab entry in RAM buffer in master index	*/
/************************************************************************/
noteLog()
{
    int i, slot;

    /* figure out who it belongs between:	*/
    for (i=0;  logTab[i].ltnewest > logBuf.lbvisit[0];	i++);

    /* note location and open it up:		*/
    slot = i;
    slideltab(slot, MAXLOGTAB-1);

    /* insert new record */
    logTab[slot].ltnewest	= logBuf.lbvisit[0]  ;
    logTab[slot].ltlogSlot	= thisLog	     ;
    logTab[slot].ltpwhash	= hash(logBuf.lbpw)  ;
    logTab[slot].ltnmhash	= hash(logBuf.lbname);
}

/************************************************************************/
/*	putLog() stores givend log record into ctdllog.sys		*/
/************************************************************************/
putLog(lBuf, n)
struct logBuffer	*lBuf;
int			n;
{
    n	*= SECSPERLOG;

    crypte(lBuf, (SECSPERLOG*SECTSIZE), n);	/* encode buffer	*/

    seek(logfl, n, 0);
    if (write(logfl, lBuf, SECSPERLOG) != SECSPERLOG) {
	printf("?putLog-write fail");
    }

    crypte(lBuf, (SECSPERLOG*SECTSIZE), n);	/* decode buffer	*/
}

/************************************************************************/
/*	PWSlot() returns userlog.buf slot password is in, else ERROR	*/
/*	NB: we also leave the record for the user in logBuf, if asked.	*/
/************************************************************************/
int PWSlot(pw, load)
char pw[NAMESIZE];
char load;
{
    int 	     h, i;
    int 	     foundIt, ourSlot;
    struct logBuffer lbuf;

    if (strLen(pw) == 0) return ERROR;		/* Yes, this is needed. */
    h = hash(pw);

    /* Check all passwords in memory: */
    for(i = 0, foundIt = FALSE;  !foundIt && i < MAXLOGTAB;  i++) {
	/* check for password match here */

	/* If password matches, check full password			*/
	/* with current newUser code, password hash collisions should	*/
	/* not be possible... but this is upward compatible & cheap	*/
	if (logTab[i].ltpwhash == h) {
	    ourSlot	= logTab[i].ltlogSlot;
	    getLog(&lbuf, ourSlot);

	    if (strCmpU(pw, lbuf.lbpw) == SAMESTRING) {
		/* found a complete match */
		thisSlot = i   ;
		foundIt  = TRUE;
	    }
	}
    }
    if (foundIt) {
	if (load == TRUE) {
	    movmem(&lbuf, &logBuf, sizeof logBuf);
	    thisLog = ourSlot;
	}
	return thisSlot;
    }
    else
	return ERROR   ;
}

/************************************************************************/
/*	slideLTab() slides bottom N lots in logTab down.  For sorting.	*/
/************************************************************************/
slideLTab(slot, last)
int slot;
int last;
{
    int i;

    /* open slot up: (movmem isn't guaranteed on overlaps) */
    for (i=last-1;  i>=slot;  i--)  {
	movmem(&logTab[i], &logTab[i+1], sizeLTentry);
    }
}

/************************************************************************/
/* sortLog ShellSorts userlog by time of last call			*/
/************************************************************************/
sortLog()
{
#define TSIZE 10
    char *temp[TSIZE];
    int finis, i, intCount, step;

    printf("sortLog...\n");
    if(sizeLTentry > TSIZE) {
	printf("!!!increase TSIZE in sortLog to %>d\n", sizeLTentry);
    }

    intCount = 0;
    for(finis=FALSE, step=MAXLOGTAB >> 1;  !finis || step>1;  ) {
	if (finis) {
	    step = step/3 + 1;
	    finis = FALSE;
	}

	finis	= TRUE;

	printf("stepsize=%d\n", step);

	for(i=step;  i<MAXLOGTAB;  i++) {
	    if(logTab[i-step].ltnewest < logTab[i].ltnewest) {
		intCount++;
		finis	= FALSE;

		/* interchange two entries */
		movmem(&logTab[i-step], temp, sizeLTentry);
		movmem(&logTab[i], &logTab[i-step], sizeLTentry);
		movmem(temp, &logTab[i], sizeLTentry);
	    }
	}
    }
    printf("sortLog: %d interchanges\n", intCount);
}

/************************************************************************/
/*	storeLog() stores the current log record.			*/
/************************************************************************/
storeLog()  {

    logTab[0].ltnewest	    = newestLo;

    logBuf.lbvisit[0]	    = newestLo;
    logBuf.lbwidth	    = termWidth;
    logBuf.lbnulls	    = termNulls;
    logBuf.lbflags	    = expert | termUpper | termLF | termTab | aide;

    putLog(&logBuf, thisLog);
}

/************************************************************************/
/*	strCmpU() is strcmp(), but ignoring case distinctions		*/
/************************************************************************/
int strCmpU(s, t)
char s[], t[];
{
    char toUpper();
    int  i;

    i	= 0;

    while (toUpper(s[i]) == toUpper(t[i])) {
	if (s[i++] == '\0')  return 0;
    }
    return  toUpper(s[i]) - toUpper(t[i]);
}

/************************************************************************/
/*	terminate() is menu-level routine to exit system		*/
/************************************************************************/
terminate(discon)
char discon;
	/* 1.  parameter <discon> is TRUE or FALSE.		*/
	/* 2.  if <discon> is TRUE, breaks modem connection	*/
	/*     or switches whichIO from CONSOLE to MODEM,	*/
	/*     as appropriate.					*/
	/* 3.  modifies externs: struct logBuf, 		*/
	/*			 struct logTab			*/
	/* 4.  returns no values				*/
	/*	      modified	dvm 9-82			*/
{
    int i;

    if (loggedIn) {
	mPrintf(" %s logged out\n ", logBuf.lbname);

	logBuf.lbgen[thisRoom]	= roomBuf.rbgen << GENSHIFT;
	if (haveCarrier || onConsole)	storeLog();
	loggedIn = FALSE;

	setUp(TRUE);
    }

    if (discon)  {
	switch (whichIO) {
	case MODEM:
	    if (rcpm)	exitToCpm = TRUE;
	    else	interpret(pHangUp);

	    break;
	case CONSOLE:
	    whichIO =  MODEM;
	    printf("\n'MODEM' mode.\n ");
	    break;
	}
    }
    for (i = 0; i < MAXROOMS; i++)	/* Clear skip bits */
	roomTab[i].rtflags &= ((BYTE - 1) - SKIP);
}

/************************************************************************/
/*	zapLogFile() erases & re-initializes userlog.buf		*/
/************************************************************************/
zapLogFile()
{
    char getCh(), toUpper();
    int  i;

    printf("\nWipe out log file? ");
    if (toUpper(getCh()) != 'Y')   return;

    /* clear RAM buffer out:			*/
    logBuf.lbvisit[0]	= ERROR;
    for (i=0;  i<MAILSLOTS;  i++) {
	logBuf.lbslot[i]= ERROR;
	logBuf.lbId[i]	= ERROR;
    }
    for (i=0;  i<NAMESIZE;  i++) {
	logBuf.lbname[i]= 0;
	logBuf.lbpw[i]	= 0;
    }

    /* write empty buffer all over file;	*/
    for (i=0;  i<MAXLOGTAB;  i++) {
	printf("Clearing log #%d\n", i);
	putLog(&logBuf, i);
    }
}
m = TRUE;
	    els