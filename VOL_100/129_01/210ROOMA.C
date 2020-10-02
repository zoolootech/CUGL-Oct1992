/************************************************************************/
/*				rooma.c 				*/
/*		room code for Citadel bulletin board system		*/
/************************************************************************/

/************************************************************************/
/*				history 				*/
/*									*/
/* 84Dec20 HAW&JLS The vp union eliminated from the system.		*/
/* 84Sep16 HAW	#if put in to handle CP/M-85 bug.			*/
/* 84Aug23 JLS & HAW  JLS's <F>orget Room Scheme implemented.		*/
/* 84Jul12 JLS & HAW  gotoRoom() and dumpRoom() modified for <S>kip.	*/
/* 84Jun23 HAW	Files now checked for CP/M R/O status.			*/
/* 84Jun23 HAW & JLS  Unused locals are zapped				*/
/* 84Apr04 HAW	Start 1.50a update					*/
/* 83Feb24	Insert check for insufficient RAM, externs too low.	*/
/* 82Dec06 CrT	2.00 release.						*/
/* 82Nov05 CrT	main() splits off to become citadel.c			*/
/************************************************************************/

#include <210ctdl.h>

/************************************************************************/
/*				Contents				*/
/*									*/
/*	dumpRoom()		tells us # new messages etc		*/
/*	fileDir()		prints out a filename for a dir listing */
/*	fillMailRoom()		set up Mail> from log record		*/
/*	gotoRoom()		handles "g(oto)" command for menu	*/
/*	init()			system startup initialization		*/
/*	initCitadel()							*/
/*	initSysop()							*/
/*	listRooms()		lists known rooms			*/
/*	openFile()		opens a .sys file			*/
/*	readSysTab()		restores system state from citadel.tab	*/
/*	roomExists()		returns slot# of named room else ERROR	*/
/*	setSpace()		set default disk and user#		*/
/*	setUp() 							*/
/*	systat()		shows current system status		*/
/*	wildCard()		expands ambiguous filenames		*/
/*	writeSysTab()		saves state of system in citadel.tab	*/
/*									*/
/************************************************************************/


/************************************************************************/
/*	dumpRoom() tells us # new messages etc				*/
/************************************************************************/
dumpRoom()
{
    char hasSkipped, str[NAMESIZE];
    int  fileDir();
    int  i, count, loc, newCount, no;

    for (newCount=0, count=0, i=0;   i<MSGSPERRM;   i++) {

	loc	= roomBuf.msg[i].rbmsgLoc;
	no	= roomBuf.msg[i].rbmsgNo ;
	if (loc != ERROR) {
	    /* comparisons which take 64K wraparound into account:	*/
	    if (no - oldestLo
		< 0x8000
	    ) {
		count++;
	    }

	    /* don't boggle -- just checking against newestLo as of */
	    /* the last time we were  in this room:		    */
	    if (no -
	       (logBuf.lbvisit[ logBuf.lbgen[thisRoom] & CALLMASK ]+1)
		< 0x8000
	    ) {
		newCount++;
	    }
	}
    }
    /* lazy patch: */
    if (newCount > count)   newCount = count;

    mPrintf(" %d messages\n ", count);
    if (loggedIn && newCount > 0)   mPrintf(" %d new\n", newCount);
    if ((roomBuf.rbflags & CPMDIR) && !expert) {
	/* list directory also: */
	doCR();
	wildCard(fileDir, "*.*");
    }

    for (i = LOBBY, hasSkipped = FALSE; i < MAXROOMS; i++)
	if (roomTab[i].rtflags & INUSE &&
		   roomTab[i].rtgen == (logBuf.lbgen[i] >> GENSHIFT)) {
	    if (roomTab[i].rtflags & SKIP)
		hasSkipped = TRUE;
	    else
		if (roomTab[i].rtlastMessage - (
			logBuf.lbvisit[logBuf.lbgen[i] & CALLMASK] + 1)
					< 0x8000)
		    break;
	}

    if (i == MAXROOMS && hasSkipped) {
	mPrintf("\n Skipped rooms: \n ");
	for (i = LOBBY; i < MAXROOMS; i++)	/* No need to match gen #s. */
	    if (roomTab[i].rtflags & SKIP &&
		       roomTab[i].rtflags & INUSE) {
		roomTab[i].rtflags &= ((BYTE - 1) - SKIP);   /* Clear. */
		strcpy(str, roomTab[i].rtname);
		if (roomTab[i].rtflags & CPMDIR)    strcat(str, ":");
		else				    strcat(str, ">");
		if (!(roomTab[i].rtflags & PUBLIC)) strcat(str, "*");
		mPrintf(" %s ", str);
	    }
    }
}


/************************************************************************/
/*	fileDir() prints out one filename and size, for a dir listing	*/
/************************************************************************/
fileDir(fileName)
char *fileName;
{
    struct fcb	block;

    char tempName[NAMESIZE], tBuf[2*NAMESIZE];

    outFlag = OUTOK;

    unspace(fileName, tempName);
    setFcb(&block,    tempName);
    bdos(GETfILEsIZE, &block);	    /* very slow on MMS BIOS! */

    FDSectCount    += block.fcbRecn;

    sprintf(tBuf, "%s%5d  ", fileName, block.fcbRecn);
    putWord(tBuf);
    mAbort();	    /* chance to next(!)/pause/skip */
}

/************************************************************************/
/*	fillMailRoom()							*/
/************************************************************************/
fillMailRoom()
{
    int i;

    /* mail room -- copy messages in logBuf to room: */
    for (i=0;  i<MSGSPERRM;  i++) {
	roomBuf.msg[i].rbmsgLoc  = ERROR;
	roomBuf.msg[i].rbmsgNo	 = ERROR;
    }
    for (i=0;  i<MAILSLOTS;  i++) {
	if (i==MSGSPERRM)   break;  /* for MSGSPRRM < MAILSLOTS */
	roomBuf.msg[i].rbmsgLoc  = logBuf.lbslot[i];
	roomBuf.msg[i].rbmsgNo	 = logBuf.lbId[i]  ;
    }
    noteRoom();
}

/************************************************************************/
/*	gotoRoom() is the menu fn to travel to a new room		*/
/*	returns TRUE if room is Lobby>, else FALSE			*/
/************************************************************************/
char gotoRoom(nam, mode)
char *nam, mode;
{
    int  i, foundit, newStuff, nWest, roomNo;

    if (strLen(nam) == 0) {

	/* update log entry for current room:	*/
	if (mode != 'S') {
	    logBuf.lbgen[thisRoom] = roomBuf.rbgen << GENSHIFT;
			     /* Clear SKIP bit */
	    roomTab[thisRoom].rtflags &= ((BYTE - 1) - SKIP);
	}
	foundit = FALSE;	/* leaves us in Lobby> if nothing found */
	newStuff= FALSE;

	for (i	= 0
	     ;
	     i<MAXROOMS  &&  !foundit
	     ;
	     i++
	) {
	    if (
		(roomTab[i].rtflags & INUSE)
		&&
		(roomTab[i].rtgen == (logBuf.lbgen[i] >> GENSHIFT) || aide)
		&&
		(roomTab[i].rtflags & SKIP) == 0
	    ) {
		nWest = logBuf.lbvisit[logBuf.lbgen[i] & CALLMASK]+1;

		if (roomTab[i].rtlastMessage - nWest  < 0x8000) {
		    if (i != thisRoom	&&   (i != AIDEROOM   ||   aide))   {
			foundit = i;
			newStuff= TRUE;
		    }
		}
	    }
	}

	getRoom(foundit);
	mPrintf("%s\n ", roomBuf.rbname);
    } else {

	/* non-empty room name, so now we look for it: */
	if (
	    (roomNo = roomExists(nam)) == ERROR
	    ||
	    (roomNo==AIDEROOM  &&  !aide)
	) {
	    mPrintf(" ?no %s room\n", nam);
	} else {
	    /* update log entry for current room:   */
	    if (mode != 'S') {
		if (loggedIn)
		    logBuf.lbgen[thisRoom] = roomBuf.rbgen << GENSHIFT;
		roomTab[thisRoom].rtflags &= ((BYTE - 1) - SKIP);
	    }
	    getRoom(roomNo);

	    /* if may have been unknown... if so, note it:	*/
	    if ((logBuf.lbgen[thisRoom] >> GENSHIFT) != roomBuf.rbgen) {
		logBuf.lbgen[thisRoom] = (
		    (roomBuf.rbgen << GENSHIFT) +
		    (MAXVISIT -1)
		);
	    }
	}
    }
    setUp(FALSE);
    dumpRoom();
    return  newStuff;	/* was thisRoom */
}

/************************************************************************/
/*	init() -- master system initialization				*/
/************************************************************************/
init() {
    char     getCh(), readSysTab(), toUpper();
    char     *msgFile;
    unsigned codend(), endExt(), externs(), topOfMem();
    int      getText(); 	/* Forces load in BDS C 1.50a */

    whichIO = CONSOLE;
    if (!readSysTab()) exit();
    setSpace(homeDisk, homeUser);
	      /* 1K RAM might not actually be sufficient... */
    if (topOfMem() - codEnd() < 1024)	{
	printf("Not enough RAM!");
	exit();
    }
    if (codEnd() > externs())	{
	printf("Externs too low!");
	exit();
    }

#ifdef XYZZY
    printf("code ends ....at  %u\n", codend()  );
    printf("externs start at  %u\n", externs() );
    printf("externs end   at  %u\n", endext()  );
    printf("free RAM ends at  %u\n", topofmem());
#endif

    exitToCpm	= FALSE;		/* not time to quit yet!	*/
    sizeLTentry = sizeOf(logTab[0]);	/* just had to try that feature */
    outFlag	= OUTOK;		/* not p(ausing)		*/

    pullMessage = FALSE;		/* not pulling a message	*/
    pulledMLoc	= ERROR;		/* haven't pulled one either	*/
    pulledMId	= ERROR;

    debug	= FALSE;
    loggedIn	= FALSE;
    thisRoom	= LOBBY;

    whichIO	= CONSOLE;
    loggedIn	= FALSE;
    setUp(TRUE);

    /* we initmodem at beginning & end both...*/
    modemInit();

    monthTab[1] = "Jan";
    monthTab[2] = "Feb";
    monthTab[3] = "Mar";
    monthTab[4] = "Apr";
    monthTab[5] = "May";
    monthTab[6] = "Jun";
    monthTab[7] = "Jul";
    monthTab[8] = "Aug";
    monthTab[9] = "Sep";
    monthTab[10]= "Oct";
    monthTab[11]= "Nov";
    monthTab[12]= "Dec";

    if (!clock)   interpret(pInitDate);

    /* open message files: */
    msgFile	= "a:ctdlmsg.sys";
    *msgFile   += msgDisk;
    openFile(msgFile,	     &msgfl );
    openFile("ctdlroom.sys", &roomfl);
    openFile("ctdllog.sys",  &logfl );

    getRoom(0);       /* load Lobby>  */
    modemInit();
}

/************************************************************************/
/*	initCitadel() does not reformat data files			*/
/************************************************************************/
initCitadel() {
    whichIO = MODEM;
    setUp(FALSE);
}

/************************************************************************/
/*	listRooms() lists known rooms					*/
/************************************************************************/
listRooms(doDull)
char doDull;	/* ALMOST_ALL to list unchanged rooms only    */
{
    char str[NAMESIZE+3];
    char boringRooms, doBoringRooms, hasUnseenStuff, shownHidden, equal;
    int  i;

    mPrintf((doDull == FORGOT) ? "\n Forgotten public rooms:\n " :
				 "\n Rooms with unread messages:\n ");
    equal	= TRUE;
    shownHidden = FALSE;
    boringRooms = FALSE;
    doBoringRooms = (doDull == FORGOT) ? FORGOT : UNREAD;
    for (; doBoringRooms <= doDull;  doBoringRooms++) {
	for (i = 0; i < MAXROOMS;  i++) {
	    if ((roomTab[i].rtflags & INUSE) && (debug || aide ||
	/* This line checks to see if gen. #s match, saves result in equal */
		(equal=(roomTab[i].rtgen == (logBuf.lbgen[i] >> GENSHIFT))))) {
		/* do only rooms with unseen messages first pass,  */
		/* only rooms without unseen messages second pass: */
		hasUnseenStuff	=  (
		    roomTab[i].rtlastMessage - (
			logBuf.lbvisit[ logBuf.lbgen[i] & CALLMASK ]+1
		    )  <  0x8000
		);
		if (!hasUnseenStuff)   boringRooms  = TRUE;

		if (doDull != FORGOT			 &&
		    (!doBoringRooms &&	hasUnseenStuff)  ||
		    ( doBoringRooms && !hasUnseenStuff)
		) {
		    strcpy(str, roomTab[i].rtname);

		    if (roomTab[i].rtflags & CPMDIR)	strcat(str, ":");
		    else				strcat(str, ">");

		    if (! (roomTab[i].rtflags & PUBLIC)) {
			strcat(str, "*");
			shownHidden	= TRUE;
		    }
		    mPrintf(" %s ", str);
		}
	    }
	    else {
		if (!equal && doDull == FORGOT && roomTab[i].rtflags & PUBLIC)
		    mPrintf(" %s%c ", roomTab[i].rtname,
				     (roomTab[i].rtflags & CPMDIR) ? ':' : '>');
		equal = TRUE;
	    }
	}
	if (boringRooms && !doBoringRooms  && doDull == ALMOST_ALL)  {
	    mPrintf("\n No unseen msgs in:\n ");
	}
    }
    if (!expert && shownHidden) mPrintf("\n \n * => hidden room\n ");
}

/************************************************************************/
/*	openFile() opens one of the .sys files. 			*/
/************************************************************************/
openFile(filename, fd)
char *filename;
int  *fd;
{
    /* open message file */
    if ((*fd = open(filename, 2)) == ERROR) {
	printf("?no %s", filename);
	exit();
    }
    if (file_RO(*fd)) {
	printf("?File %s is R/O!", filename);
	writeSysTab();
	exit();
    }
}

/************************************************************************/
/*	readSysTab() restores state of system from SYSTEM.TAB		*/
/*	returns:	TRUE on success, else FALSE			*/
/*	    destroys SYSTEM.TAB after read, to prevent erroneous re-use */
/*	    in the event of a crash.					*/
/************************************************************************/
readSysTab() {
    char getc();
    char fBuf[BUFSIZ];
    char *c;
    int  fd;		/* For checking for R/O status */

    if ((fd = fopen("ctdlTabl.sys", fBuf)) == ERROR) {
	printf("?no ctdlTabl.sys!");
	return(FALSE);
    }

    if (file_RO(fd)) {
	printf("?File ctdltabl.sys is R/O!");
	return(FALSE);
    }

    if (
	getw(fBuf) !=  &firstExtern
	||
	getw(fBuf) !=  &lastExtern
    ) {
	printf("?old ctdlTabl.sys!");
	return(FALSE);
    }
    c	= &firstExtern;
    while (c < &lastExtern)   *c++ = getc(fBuf);

    unlink("ctdlTabl.sys");
    return(TRUE);
}

/************************************************************************/
/*	roomExists() returns slot# of named room else ERROR		*/
/************************************************************************/
int roomExists(room)
char *room;
{
    int i;

    for (i=0;  i<MAXROOMS;  i++) {
	if (
	    roomTab[i].rtflags & INUSE	 &&
	    strCmpU(room, roomTab[i].rtname) == SAMESTRING
	) {
	    return(i);
	}
    }
    return(ERROR);
}

/************************************************************************/
/*	setSpace() moves us to a disk and user# 			*/
/************************************************************************/
setSpace(disk, user)
char disk, user;
{
    bdos(SETdISK, disk);
    ourDisk	= disk;

    bdos(SETuSER, user);
    ourUser	= user;
}

/************************************************************************/
/*	setUp() 							*/
/************************************************************************/
setUp(justIn)
char justIn;
{
    int g, i, j, ourSlot;

    echo		= BOTH; 	/* just in case 		*/

    if (!loggedIn)   {
	prevChar    = ' ';
	termWidth   = 32;
	termLF	    = LFMASK;
	termUpper   = TRUE;
	termNulls   = 5;
	expert	    = FALSE;
	aide	    = FALSE;

	if (justIn)   {
	    /* set up logBuf so everything is new...	    */
	    for (i=0;  i<MAXVISIT;  i++)  logBuf.lbvisit[i] = oldestLo;

	    /* no mail for anonymous folks: */
	    roomTab[MAILROOM].rtlastMessage = newestLo;
	    for (i=0;  i<MAILSLOTs;  i++)   logBuf.lbId[i]  = 0;

	    logBuf.lbname[0]	=0;

	    for (i=0;  i<MAXROOMS;  i++) {
		if (roomTab[i].rbflags & PUBLIC) {
		    /* make public rooms known: */
		    g			= roomTab[i].rtgen;
		    logBuf.lbgen[i]	= (g << GENSHIFT) + (MAXVISIT-1);
		} else {
		    /* make private rooms unknown: */
		    g		    = (roomTab[i].rtgen + (MAXGEN-1)) % MAXGEN;
		    logBuf.lbgen[i] = (g << GENSHIFT) + (MAXVISIT-1);
		}
	    }
	}
    } else {
	/* loggedIn: */
	termWidth   = logBuf.lbwidth;
	termNulls   = logBuf.lbnulls;
	termLF	    = logBuf.lbflags & LFMASK ;
	termUpper   = logBuf.lbflags & UCMASK ;
	expert	    = logBuf.lbflags & EXPERT ;
	termTab     = logBuf.lbflags & TABMASK;
	aide	    = logBuf.lbflags & AIDE   ;

	if (justIn)   {
	    /* set gen on all unknown rooms  --  INUSE or no: */
	    for (i=0;  i<MAXROOMS;  i++) {
		if (!(roomTab[i].rtflags & PUBLIC)) {
		    /* it is private -- is it unknown? */
		    if ((logBuf.lbgen[i] >> GENSHIFT)	!=   roomTab[i].rtgen) {
			/* yes -- set	gen = (realgen-1) % MAXGEN */
			j = (roomTab[i].rtgen + (MAXGEN-1)) % MAXGEN;
			logBuf.lbgen[ i ] =  (j << GENSHIFT) + (MAXVISIT-1);
		    }
		}
		else if ((logBuf.lbgen[i] >> GENSHIFT)	!=  roomTab[i].rtgen)  {
		    /* newly created public room -- remember to visit it; */
		    if
		     (abs(roomTab[i].rtgen - (logBuf.lbgen[i] >> GENSHIFT))
				       != FORGET_OFFSET)
		    logBuf.lbgen[i] = (roomTab[i].rtgen << GENSHIFT) +1;
		}
	    }
	    /* special kludge for Mail> room, to signal new mail:   */
	    roomTab[MAILROOM].rtlastMessage = logBuf.lbId[MAILSLOTS-1];

	    /* slide lbvisit array down and change lbgen entries to match: */
	    for (i=(MAXVISIT-2);  i;  i--) {
		logBuf.lbvisit[i] = logBuf.lbvisit[i-1];
	    }
	    logBuf.lbvisit[(MAXVISIT-1)]    = oldestLo;
	    for (i=0;  i<MAXROOMS;  i++) {
		if ((logBuf.lbgen[i] & CALLMASK)  <  (MAXVISIT-2)) {
		    logBuf.lbgen[i]++;
		}
	    }

	    /* Slide entry to top of log table: */
	    ourSlot = logTab[thisSlot].ltlogSlot;
	    slideltab(0, thisSlot);

	    logTab[0].ltpwhash	    = hash(logBuf.lbpw);
	    logTab[0].ltnmhash	    = hash(logBuf.lbname);
	    logTab[0].ltlogSlot     = ourSlot;
	    logTab[0].ltnewest	    = newestLo;
	}
    }
    logBuf.lbvisit[0]	= newestLo;

    onConsole	= (whichIO == CONSOLE);
    if (thisRoom == MAILROOM)	fillMailRoom();
}

/************************************************************************/
/*	systat() prints out current system status			*/
/************************************************************************/
systat() {
    int i;

    printDate();

#ifdef XYZZY
    if (debug) mPrintf(" *catChar=%d catSector=%d\n*", catChar, catSector);
#endif

    mPrintf("\n Width %d, %s%slinefeeds, %d nulls\n",
	termWidth,
	termUpper  ?  "UPPERCASE ONLY, " : "",
	termLF	   ?  ""		 : "no ",
	termNulls
    );

#ifdef XYZZY
    if (debug) mPrintf(" *lowId=%u %u, highId=%u %u\n*",
	oldestHi, oldestLo,
	newestHi, newestLo
    );
#endif

    if (loggedIn) {
	mPrintf(" Logged in as %s\n", logBuf.lbname);
#ifdef XYZZY
	if (debug) {
	    for (i=0; i<MAXVISIT; i++) {
		printf("lbvisit[%d]=%u\n", i, logBuf.lbvisit[i]);
	    }
	    printf("lbgen>>GENSHIFT==%d; roomTab.rtgen==%d; roomBuf.rbgen==%d\n"
		, logBuf.lbgen[thisRoom]>>GENSHIFT, roomTab[thisRoom].rtgen,
		roomBuf.rbgen
	    );
	    printf("lbgen&CALLMASK==%d\n", logBuf.lbgen[thisRoom]&CALLMASK);
	}
#endif
    }

    mPrintf(" %d messages,",			newestLo-oldestLo +1);
    mPrintf(" last is %u %u,\n",		newestHi, newestLo  );
    mPrintf(" %dK message space,\n",		maxMSector >> 3     );
    mPrintf(" %d-entry log\n",			MAXLOGTAB	    );
}


/************************************************************************/
/*	unspace() copies a filename, removing blanks			*/
/************************************************************************/
unspace(from, to)
char *from, *to;
{
    while (*to =  *from++)   if (*to != ' ')   to++;
}

/************************************************************************/
/*	writeSysTab() saves state of system in SYSTEM.TAB		*/
/*	returns:	TRUE on success, else ERROR			*/
/************************************************************************/
writeSysTab() {
    /*	extern int opsLogChar, opsLogSector, catChar, catSector;	*/
    /*	extern int lowId, highId, nextRoom;				*/
    /*	extern struct logTab, struct roomTab;				*/

    char *c;
    char fBuf[BUFSIZ];

    if(fcreat("ctdlTabl.sys", fBuf) == ERROR) {
	printf("?can't make ctdlTabl.sys");
	return(ERROR);
    }

    /* write loc & len so we can detect bad ctdlTabl.sys files: */
    putw(&firstExtern, fBuf);
    putw(&lastExtern,  fBuf);
    c	= &firstExtern;
    while (c < &lastExtern)
	putc(*c++, fBuf);

    fclose(fBuf);
    return(TRUE);
}

/************************************************************************/
/*	wildCard() Taken from Aug. '84 BYTE Japan report.		*/
/************************************************************************/
#define UFNsIZE 	13	/* size of "filename.ext" plus a null.	*/
#define DEFAULT_DMA	0x80	/* Use default DMA			*/

wildCard(fn, filename)
int (*fn)();
char *filename; 	/* may be ambiguous.  No drive or user numbers. */
{
#ifndef CPM_85_IS_BUGGY
    char fcb[36], *p, *q;
    int  i, s, strCmp(), bdosfn, fileCount;
    struct {
	char unambig[UFNsIZE];	/* filename */
    } *fp;

    bdosfn = FINDfIRST;
    setSpace(roomBuf.rbdisk, roomBuf.rbuser);
    bdos(SETdMA, DEFAULT_DMA);

    if (filename[1] == ':' || setfcb(fcb, filename) == 255 ||
	(i = bdos(FINDfIRST, fcb)) == 255) {
	/* no such file */
	mPrintf("no %s\n ", filename);
	setSpace(homeDisk, homeUser);
	return;
    }

    fp	      = msgBuf.mbtext;
    fileCount = 0;

    while ((i = bdos(bdosfn, fcb)) != 255) {
	if (fp > (&msgBuf.mbtext[MAXTEXT-100])) {
	    printf("too many files!!\n");
	    setSpace(homeDisk, homeUser);
	    return;
	}
	bdosfn = FINDnEXT;
	if (unpack(i * 32 + 0x81, fp)) {
	    fileCount++;
	    fp++;
	}
    }

    if (fileCount == 0) 	    /* no such file */
	mPrintf("no %s\n ", filename);
    else {
	qSort(msgBuf.mbtext, fileCount, UFNsIZE, strCmp);
	outFlag     = OUTOK;
	for (fp = msgBuf.mbtext;  fileCount-- && outFlag != OUTSKIP;  fp++)
	    (*fn)(fp);
    }
    setSpace(homeDisk, homeUser);
#else
    char buf[BUFSIZ];

    if (index(filename, "*") != ERROR ||
	index(filename, "?") != ERROR) {
	/* no such file */
	mPrintf("Sorry, CP/M-85 won't let us process wildcards\n ");
	return;
    }
    setSpace(roomBuf.rbdisk, roomBuf.rbuser);
    if (filename[1] == ':' || fopen(filename, buf) == ERROR) {
	mPrintf("no %s\n ", filename);
    }
    else {
	fclose(buf);
	(*fn)(filename);
    }
    setSpace(homeDisk, homeUser);
#endif
}

unpack(pdir, name)
char *pdir, *name;
{
    int i, j;
    char *p;

    if (pdir[9] > 128) return FALSE;	/* SYS file. */
    p = name;
    for (i = j = 0; i < 12; i++) {
	if (i == 8) *p++ = '.';
	*p++ = pdir[j++] & '\177';
    }
    *p = '\0';
    return TRUE;
}
nt == 0) 	    /* no such file */
	mPrintf("no %s\n ", filename);
    else {
	qSort(msgBuf.mbtext, f