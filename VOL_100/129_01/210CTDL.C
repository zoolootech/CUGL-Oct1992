/************************************************************************/
/*				ctdl.c					*/
/*		Command-interpreter code for Citadel			*/
/************************************************************************/
#include <210ctdl.h>
/************************************************************************/
/*				history 				*/
/*									*/
/* 84Dec28 HAW	doAide modified to list deleted rooms.			*/
/* 84Dec21 HAW	doHelp fixed so that LONG filenames don't bomb system.	*/
/* 84Dec12 HAW	Fixed kill acct. bug that made killed acct. the user.	*/
/* 84Aug23 HAW & JLS  JLS's Forget scheme implemented.			*/
/* 84JuL12 HAW & JLS  <S>kip implemented.				*/
/* 84JuL06 HAW	ALL FILES: make putChar the bottleneck! 		*/
/* 84Jun26 JLS & HAW  Commented out extra params to putRoom & getRoom.	*/
/* 84Jun21 JLS	Fix ".Read" so can't have endless "reverse forward..."	*/
/* 84Jun10 JLS	Implement "user must log in to create room."		*/
/* 84Jun10 JLS	Mod for changing date from other than system console.	*/
/* 84May18 JLS/HAW Greeting modified for coherency.			*/
/* 84Apr04 HAW	Upgrade to BDS 1.50a begun.				*/
/* 83Mar08 CrT	Aide-special functions installed & tested...		*/
/* 83Feb24 CrT/SB Menus rearranged.					*/
/* 82Dec06 CrT	2.00 release.						*/
/* 82Nov05 CrT	removed main() from room2.c and split into sub-fn()s	*/
/************************************************************************/

/************************************************************************/
/*				Contents				*/
/*									*/
/*	doAide()		handles Aide-only commands		*/
/*	doChat()		handles C(hat)		command 	*/
/*	doEnter()		handles E(nter) 	command 	*/
/*	doForget()		handles Z(Forget room)	command 	*/
/*	doGoto()		handles G(oto)		command 	*/
/*	doHelp()		handles H(elp)		command 	*/
/*	doKnown()		handles K(nown rooms)	command 	*/
/*	doLogin()		handles L(ogin) 	command 	*/
/*	doLogout()		handles T(erminate)	command 	*/
/*	doRead()		handles R(ead)		command 	*/
/*	doRegular()		fanout for above commands		*/
/*	doSkip()		handles S(kip)		command 	*/
/*	doSysop()		handles sysop-only commands		*/
/*	getCommand()		prints prompt and gets command char	*/
/*	greeting()		System-entry blurb etc			*/
/*	main()			has the central menu code		*/
/************************************************************************/

/************************************************************************/
/*	doAide() handles the aide-only menu				*/
/*	    return FALSE to fall invisibly into default error msg	*/
/************************************************************************/
char doAide(moreYet, first)
char moreYet;
char first;	/* first parameter if TRUE		*/
{
    char oldName[NAMESIZE];
    int  rm;

    if (!aide)	 return FALSE;

    if (moreYet)   first = '\0';

    mprintf("ide special fn: ");

    if (first)	   oChar(first);

    switch (toUpper(   first ? first : iChar()	  )) {
    case 'D':
	sprintf(msgBuf.mbtext,
	   "The following empty rooms deleted by %s: ", logBuf.lbname);
	mPrintf("elete empty rooms\n ");
	strCpy(oldName, roomBuf.rbname);
	indexRooms();

	if ((rm=roomExist(oldName)) != ERROR)	getRoom(rm);
	else					getRoom(LOBBY);

	aideMessage( /* noteDeletedMessage== */ FALSE );
	break;
    case 'E':
	mprintf("dit room\n  \n");
	strCpy(oldName, roomBuf.rbname);
	if (!renameRoom())   break;
	sPrintf(
	    msgBuf.mbtext,
	    "%s> edited to %s> by %s",
	    oldName,
	    roomBuf.rbname,
	    logBuf.lbname
	);
	aideMessage( /* noteDeletedMessage == */ FALSE);
	break;
    case 'S':
	mprintf("et Date\n ");
	changedate();
	break;
    case 'I':
	mPrintf("nsert message\n ");
	if (
	    thisRoom   == AIDEROOM
	    ||
	    pulledMLoc == ERROR
	)   {
	    mPrintf("nope!");
	    break;
	}
	note2Message(pulledMId, pulledMLoc);
	putRoom(thisRoom);
	noteRoom();
	sPrintf(
	    msgBuf.mbtext,
	    "Following message inserted in %s> by %s",
	    roomBuf.rbname,
	    logBuf.lbname
	);
	aideMessage( /* noteDeletedMessage == */ TRUE);
	break;
    case 'K':
	mPrintf("ill room\n ");
	if (
	    thisRoom == LOBBY
	    ||
	    thisRoom == MAILROOM
	    ||
	    thisRoom == AIDEROOM
	) {
	    mPrintf(" not here!");
	    break;
	}
	if (!getYesNo("confirm"))   break;

	sPrintf(
	    msgBuf.mbtext,
	    "%s> killed by %s",
	    roomBuf.rbname,
	    logBuf.lbname
	);
	aideMessage( /* noteDeletedMessage == */ FALSE);

	roomBuf.rbflags ^= INUSE;
	putRoom(thisRoom);
	noteRoom();
	getRoom(LOBBY);
	break;
    case '?':
	tutorial("aide.mnu");
	break;
    default:
	if (!expert)	mprintf(" ?(Type '?' for menu)\n "	);
	else		mprintf(" ?\n " 			);
	break;
    }
    return TRUE;
}

/************************************************************************/
/*	doChat()							*/
/************************************************************************/
doChat(moreYet, first)
char moreYet;	/* TRUE to accept folliwng parameters	*/
char first;	/* first parameter if TRUE		*/
{
    if (moreYet)   first = '\0';

    if (first)	   oChar(first);

    mPrintf("hat ");

    if (noChat)   {
	tutorial("nochat.blb");
	return;
    }

    if (whichIO == MODEM)	ringSysop();
    else			interact() ;
}

/************************************************************************/
/*	doEnter() handles E(nter) command				*/
/************************************************************************/
doEnter(moreYet, first)
char moreYet;	/* TRUE to accept following parameters	*/
char first;	/* first parameter if TRUE		*/
{
#define CONFIGURATION	0
#define MESSAGE 	1
#define PASSWORD	2
#define ROOM		3
#define FILE		4
    char what;			/* one of above five */
    char abort, done, WC;
    char iChar(), toUpper();


    if (moreYet)   first = '\0';

    abort	= FALSE  ;
    done	= FALSE  ;
    WC		= FALSE  ;
    what	= MESSAGE;

    mprintf("nter ");

    if (!loggedIn  &&  !unlogLoginOk  &&  thisRoom!=MAILROOM)	{
	mprintf("--Must log in to enter (except in Mail>)\n ");
	return;
    }

    if (first)	   oChar(first);

    do	{
	outFlag = OUTOK;

	switch (toUpper(   first ? first : iChar()   )) {
	case '\r':
	case '\n':
	    moreYet	= FALSE;
	    break;
	case 'F':
	    if (roomBuf.rbflags & CPMDIR) {
		mprintf("ile upload ");
		what	= FILE;
		done	= TRUE;
		break;
	    }
	default:
	    mprintf("? ");
	    abort	= TRUE;
	    if (expert)  break;
	case '?':
	    tutorial("entopt.mnu");
	    abort	= TRUE;
	    break;
	case 'C':
	    mprintf("onfiguration ");
	    what	= CONFIGURATION;
	    done	= TRUE;
	    break;
	case 'F':
	    mprintf("ile upload "    );
	    WC		= TRUE;
	    done	= TRUE;
	    break;
	case 'M':
	    mprintf("essage "	   );
	    what	= MESSAGE;
	    done	= TRUE;
	    break;
	case 'P':
	    mprintf("assword "	   );
	    what	= PASSWORD     ;
	    done	= TRUE;
	    break;
	case 'R':
	    mprintf("oom "	   );
	    if (!nonAideRoomOk && !aide)   {
		mPrintf(" ?-- must be aide to create room\n ");
		abort	= TRUE;
		break;
	    }
	    if (!loggedIn) {
		mprintf("!Must log in to create a new room\n");
		abort = TRUE;
		break;
	    }
	    what	= ROOM;
	    done	= TRUE;
	    break;
	}
	first = '\0';
    } while (!done && moreYet && !abort);

    doCR();

    if (!abort) {
	if (whichIO != CONSOLE && thisRoom == MAILROOM) echo = CALLER;
	switch (what) {
	case CONFIGURATION  :	configure()	   ;	   break;
	case MESSAGE	    :	makeMessage(WC, 0) ;	   break;
	case PASSWORD	    :	newPW() 	   ;	   break;
	case ROOM	    :	makeRoom()	   ;	   break;
	case FILE	    :	upLoad()	   ;	   break;
	}
	echo = BOTH;
    }
}

/************************************************************************/
/*	doForget() handles (Forget room) command			*/
/************************************************************************/
doForget(expand, first)
char expand, first;
{
    int i;

    if (!expand) {
	mPrintf("\bForget %s \n ", roomBuf.rbname);
	if (thisRoom == LOBBY	 ||
	    thisRoom == MAILROOM ||
	    thisRoom == AIDEROOM) {
	    mPrintf("!Can't forget this room! \n ");
	    return ;
	}
	if (!getYesNo("confirm"))   return;
	i = (roomBuf.rbgen + FORGET_OFFSET) % MAXGEN;	    /* Set up offset */
	logBuf.lbgen[thisRoom] = i << GENSHIFT; 	    /* Save it	     */
	gotoRoom("Lobby", 'S');
    }
    else {
	mPrintf("\b\b");
	listRooms(/* doDull == */ FORGOT);
    }
}

/************************************************************************/
/*	doGoto() handles G(oto) command 				*/
/************************************************************************/
doGoto(expand, first)
char expand;	/* TRUE to accept following parameters	*/
char first;	/* first parameter if TRUE		*/
{
    char roomName[NAMESIZE];

    mprintf("oto ");

    if (!expand) {
	gotoRoom("", 'R');
	return;
    }

    getNormStr("", roomName, NAMESIZE);

    if (roomName[0] == '?')	listRooms(/* doDull== */ ALMOST_ALL);
    else			gotoRoom(roomName, 'R');
}

/************************************************************************/
/*	doHelp() handles H(elp) command 				*/
/************************************************************************/
doHelp(expand, first)
char expand;	/* TRUE to accept following parameters	*/
char first;	/* first parameter if TRUE		*/
{
    char fileName[NAMESIZE];

    mprintf("elp ");
    if (!expand) {
	mprintf("\n\n");
	tutorial("dohelp.hlp");
	return;
    }

    getNormStr("", fileName, 9);	/* CP/M filename size is 8	*/
    if (strlen(fileName) == 0)
	strcpy(fileName, "dohelp");
    if (fileName[0] == '?')	tutorial("helpopt.hlp");
    else {
	/* adding the extention makes things look simpler for		*/
	/* the user... and restricts the files which can be read	*/
	strcat(fileName, ".hlp");
	tutorial(fileName);
    }
}

/************************************************************************/
/*	doKnown() handles K(nown rooms) command.			*/
/************************************************************************/
doKnown(expand, first)
char expand;	/* TRUE to accept following parameters	*/
char first;	/* first parameter if TRUE		*/
{
    mprintf("nown rooms\n ");
    listRooms(/* doDull== */ ALMOST_ALL);
}

/************************************************************************/
/*	doLogin() handles L(ogin) command				*/
/************************************************************************/
doLogin(moreYet, first)
char moreYet;	/* TRUE to accept following parameters	*/
char first;	/* first parameter if TRUE		*/
{
    char passWord[NAMESIZE];

    mprintf("ogin ");
    if (!moreYet)   mPrintf("\n");
    if (loggedIn)   {
	mprintf("\n ?Already logged in!\n ");
	return;
    }
    echo	= CALLER;
    getNormStr(moreYet ? "" : " password", passWord, NAMESIZE);
    echo	= BOTH;
    login(passWord);
}

/************************************************************************/
/*	doLogout() handles T(erminate) command				*/
/************************************************************************/
doLogout(expand, first)
char expand;	/* TRUE to accept following parameters	*/
char first;	/* first parameter if TRUE		*/
{
    char iChar(), toUpper();

    if (expand)   first = '\0';

    mprintf("erminate ");

    if (first)	 oChar(first);

    switch (toUpper(	first ? first : iChar()    )) {
    case '?':
	mprintf("\n Logout options:\n \n ");

	mprintf("Quit-also\n "		);
	mprintf("Stay\n "		);
	break;
    case 'Q':
	mprintf("uit-also\n ");
	if (!expand)   {
	   if (!getYesNo("confirm"))   break;
	}
	terminate( /* hangUp == */ TRUE);
	break;
    case 'S':
	mprintf("tay\n ");
	terminate( /* hangUp == */ FALSE);
	break;
    }
}

/************************************************************************/
/*	doRead() handles R(ead) command 				*/
/************************************************************************/
doRead(moreYet, first)
char moreYet;	/* TRUE to accept following parameters	*/
char first;	/* first parameter if TRUE		*/
{
    char iChar(), toUpper();
    int  fileDir(), transmitFile();
    char abort, doDir, done, hostFile, whichMess, revOrder, status, WC;
    char fileName[NAMESIZE];


    if (moreYet)   first = '\0';

    mprintf("\bread ");

    if (!loggedIn  &&  !unlogReadOk)   {
	mPrintf("Must log in to read\n ");
	return;
    }

    if (first)	   oChar(first);

    abort	= FALSE;
    doDir	= FALSE;
    hostFile	= FALSE;
    revOrder	= FALSE;
    status	= FALSE;
    WC		= FALSE;
    whichMess	= NEWoNLY;

    do {
	outFlag = OUTOK;
	done = TRUE;

	switch (toUpper(   first ? first : iChar()   )) {
	case '\n':
	case '\r':
	    moreYet	= FALSE;
	    break;
	case 'A':
	    mprintf("ll");
	    whichMess	= OLDaNDnEW;
	    break;
	case 'F':
	    mprintf("orward");
	    revOrder	= FALSE;
	    whichMess	= OLDaNDnEW;
	    break;
	case 'G':
	    mPrintf("lobal new-messages");
	    whichMess	= GLOBALnEW;
	    break;
	case 'N':
	    mprintf("ew ");
	    whichMess	= NEWoNLY;
	    break;
	case 'O':
	    mPrintf("ld Reverse");
	    revOrder	= TRUE;
	    whichMess	= OLDoNLY;
	    break;
	case 'R':
	    mprintf("everse");
	    revOrder	= TRUE;
	    whichMess	= OLDaNDnEW;
	    break;
	case 'S':
	    mprintf("tatus\n ");
	    status	= TRUE;
	    break;
	case 'W':
	    mprintf("C protocol ");
	    WC		= TRUE;
	    done	= FALSE;
	    break;
	case 'B':
	    if (roomBuf.rbflags & CPMDIR)   {
		mprintf("inary file(s) ");
		hostFile	= TRUE ;
		textDownload	= FALSE;
		break;
	    }
	case 'D':
	    if (roomBuf.rbflags & CPMDIR)   {
		mprintf("irectory ");
		doDir	= TRUE;
		break;
	    }
	case 'T':
	    if (roomBuf.rbflags & CPMDIR)   {
		mprintf("extfile(s) ");
		hostFile	= TRUE;
		textDownload	= TRUE;
		break;
	    }
	default:
	    mprintf("? ");
	    abort	= TRUE;
	    setUp(FALSE);
	    if (expert)   break;
	case '?':
	    tutorial("readopt.mnu");
	    abort	= TRUE;
	    break;
	}
	first = '\0';
    } while (!done && moreYet && !abort);

    if (abort) return;

    if (status) {
	systat();
	return;
    }

    if (doDir) {
	FDSectCount	= 0;	/* global fileDir() totals sectors in	*/

	getNormStr("", fileName, NAMESIZE);

	if (strLen(fileName))	wildCard(fileDir, fileName);
	else			wildCard(fileDir, "*.*"   );

	mprintf("\n %d sectors total\n ", FDSectCount);
	return;
    }

    if (hostFile) {
	getNormStr("afn", fileName, NAMESIZE);
	usingWCprotocol = WC;
	wildCard(transmitFile, fileName);
	usingWCprotocol = FALSE;
	return;
    }

    doCR();
    if (WC) {
	download(whichMess, revOrder);
	return;
    }

    if (whichMess != GLOBALnEW)   {
	showMessages(whichMess, revOrder);
    } else {
	while (outFlag != OUTSKIP   &&	 gotoRoom("", 'R'))   {
	    givePrompt();
	    mPrintf("read new\n ");
	    showMessages(NEWoNLY, revOrder);
	}
    }
}

/************************************************************************/
/*	doRegular()							*/
/************************************************************************/
char doRegular(x, c)
char x, c;
{
    char toReturn;

    toReturn = FALSE;

    switch (c) {

    case 'C': doChat(  x, '\0');		    break;
    case 'E': doEnter( x, 'm' );		    break;
    case 'F': doRead(  x, 'f' );		    break;
    case 'G': doGoto(  x, '\0');		    break;
    case 'H': doHelp(  x, '\0');		    break;
    case 'K': doKnown( x, '\0');		    break;
    case 'L': doLogin( x, '\0');		    break;
    case 'N': doRead(  x, 'n' );		    break;
    case 'O': doRead(  x, 'o' );		    break;
    case 'R': doRead(  x, 'r' );		    break;
    case 'S': doSkip(  x, '\0');		    break;
    case 'T': doLogout(x, 'q' );		    break;

    case 0:
	if (newCarrier)   {
	    greeting();
	    newCarrier	= FALSE;
	}
	if (justLostCarrier) {
	    justLostCarrier = FALSE;
	    if (loggedIn) terminate(FALSE);
	}
	break;	/* irrelevant value */
    case '?':
	tutorial("mainopt.mnu");
	if (whichIO == CONSOLE)   mprintf(" ^p: privileged fns\n ");
	break;

    case 'A': if (!doAide(x, 'E'))  toReturn=TRUE;    break;
    case 'Z': doForget(x, '\0');		      break;
    default:
	toReturn=TRUE;
	break;
    }
    return  toReturn;
}

/************************************************************************/
/*	doSkip() handles the <S>kip a room command			*/
/************************************************************************/
char doSkip(expand, first)
char first, expand;
{
    char roomName[NAMESIZE];		/* In case of ".Skip" */

    mPrintf("kip %s> goto ", roomTab[thisRoom].rtname);
    if (expand)
	getString("", roomName, NAMESIZE);
    else
	roomName[0] = '\0';
    if (roomName[0] == '?')
	tutorial("skip.hlp");
    else {
	roomTab[thisRoom].rtflags |= SKIP;	/* Set bit */
	gotoRoom(roomName, 'S');
    }
}

/************************************************************************/
/*	doSysop() handles the sysop-only menu				*/
/*	    return FALSE to fall invisibly into default error msg	*/
/************************************************************************/
char doSysop(c, first)
char c;
char first;	/* first parameter if TRUE		*/
{
    char		who[NAMESIZE];
    struct logBuffer	lBuf;
    int 		logNo, ltabSlot;

    if (!onConsole)   return TRUE;

    while (TRUE)   {

	mprintf("\n privileged fn: ");

	switch (toUpper(   first ? first : iChar()    )) {
	case 'A':
	    mPrintf("bort\n ");
	    return FALSE;
	case 'C':
	    mprintf("hat mode %sabled\n ",
		(noChat = !noChat)
		?
		"dis"
		:
		"en"
	    );
	    break;
	case 'D':
	    debug = !debug;
	    mprintf("ebug switch=%d\n \n", debug);
	    break;
	case 'K':
	    mPrintf("ill account\n ");
	    if (!getYesNo("Confirm"))	break;
	    getNormStr("who", who, NAMESIZE);
	    logNo   = findPerson(who, &lBuf);
	    if (logNo == ERROR)   {
		mPrintf("No such person\n ");
		break;
	    }
	    mPrintf(
		"%s deleted\n ",
		who
	    );
	    ltabSlot = PWSlot(lBuf.lbpw, /* load == */ FALSE);
	    lBuf.lbname[0] = '\0';
	    lBuf.lbpw[0  ] = '\0';

	    putLog(&lBuf, logNo);

	    logTab[ltabSlot].ltpwhash	    = 0;
	    logTab[ltabSlot].ltnmhash	    = 0;

	    break;
	case 'M':
	    mprintf("\bSystem now on MODEM\n ");
	    whichIO = MODEM;
	    setUp(FALSE);
	    mprintf("Chat mode %sabled\n ", noChat  ?  "dis"  :  "en");
	    if (debug)	     mPrintf("Debug mode on\n "  );
	    if (visibleMode) mPrintf("Visible mode on\n ");
	    return FALSE;
	case 'P':
	    mPrintf("\baide privilege set/clear\n ");
	    getNormStr("who", who, NAMESIZE);
	    logNo   = findPerson(who, &lBuf);
	    if (logNo == ERROR)   {
		mPrintf("No such person\n ");
		break;
	    }

	    lBuf.lbflags ^= AIDE;
	    mPrintf(
		"%s %s aide privileges\n ",
		who,
		(lBuf.lbflags & AIDE)  ?  "gets"  :  "loses"
	    );
	    if (!getYesNo("Confirm"))	break;

	    putLog(&lBuf, logNo);

	    /* see if it is us: */
	    if (loggedIn  &&  strCmpU(logBuf.lbname, who)==SAMESTRING)	 {
		aide = lBuf.lbflags & AIDE;
		logBuf.lbflags ^= AIDE;
	    }
	    break;
#ifdef XYZZY
	case 'R':
	    patchDebug();
	    break;
#endif
	case 'S':
	    changeDate();
	    break;
	case 'V':
	    mprintf(" VisibleMode==%d\n ",  visibleMode = !visibleMode);
	    break;
	case 'X':
	    mprintf("\bexit to CP/M\n \n");
	    exitToCpm = TRUE;
	    return FALSE;
	case '?':
	    tutorial("ctdlopt.mnu");
	    break;
	default:
	    if (!expert)    mprintf(" ?(Type '?' for menu)\n "	    );
	    else	    mprintf(" ?\n "			    );
	    break;
	}
    }
}

/************************************************************************/
/*	getCommand() prints menu prompt and gets command char		*/
/*	Returns: char via parameter and expand flag as value  --	*/
/*		 i.e., TRUE if parameters follow else FALSE.		*/
/************************************************************************/
char getCommand(c)
char *c;
{
    char BBSCharReady(), iChar(), toUpper();
    char expand;

    outFlag = OUTOK;

  /*if (*c)*/ givePrompt();
    while (BBSCharReady())   iChar();	      /* eat type-ahead       */

    *c = toUpper(iChar());

    expand  = (
	*c == ' '
	||
	*c == '.'
	||
	*c == ','
	||
	*c == '/'
    );
    if (expand) *c = toUpper(iChar());

    if (justLostCarrier) {
	justLostCarrier = FALSE;
	if (loggedIn) terminate(FALSE);
    }
    return expand;
}

/************************************************************************/
/*	greeting() gives system-entry blurb etc 			*/
/************************************************************************/
greeting()
{
    if (loggedIn) terminate(FALSE);

    setUp(TRUE);     pause(10);

    mprintf("\n Welcome to %s", nodeTitle);
    mprintf("\n Running: Citadel (V2.4) \n \n");
    printDate();
    mprintf("\n H for Help\n ");

    printf("Chat mode %sabled\n", noChat ? "dis" : "en");
    printf("\n 'MODEM' mode.\n "		       );
    printf("(<ESC> for CONSOLE mode.)\n "	       );

    gotoRoom("Lobby", 'R');
}

/************************************************************************/
/*	main() contains the central menu code				*/
/************************************************************************/
main() {
    char c, x;
    char getCommand(), init();
    int  putChar();		/* Force load from modem.c */
    int  getNumber();		/* Force load */
    int  doWC();
    
    /* don't put any code above init()... readSysTab() will defeat it.	*/
    init();
    initCitadel();
    weAre	= CITADEL;
    greeting();

    while (!exitToCpm) {
	x	= getCommand(&c);

	outFlag = OUTOK;

	if ((c==CNTRLp)  ?  doSysop(0, '\0')  :  doRegular(x, c))  {
	    if (!expert)    mprintf(" ? (Type '?' for menu)\n \n"   );
	    else	    mprintf(" ?\n \n"			    );
	}
    }
    if (loggedIn) terminate( /* hangUp == */ TRUE);
    writeSysTab(); 
}
sabled\n", noChat ? "dis" : "en");
    