/************************************************************************/
/*				roomb.c 				*/
/*		room code for Citadel bulletin board system		*/
/************************************************************************/

/************************************************************************/
/*				History 				*/
/*									*/
/* 84Dec28 HAW	indexRooms modified to list deleted rooms.		*/
/* 84Dec20 JLS	renameRoom changed aides can decide to cause amnesia.	*/
/* 84Dec20 JLS & HAW "vp" union eliminated from the system.		*/
/* 84Jun28 JLS	Enhancement: Creator of a room is listed in Aide>.	*/
/* 84Jun23 HAW & JLS  Local unused variables are zapped.		*/
/* 84Jun10 JLS	Room cannot have a name of length 0.			*/
/* 84May07 JLS & HAW Fix bug in makeRoom to ensure correct g. # inc.ed. */
/* 84Apr04 HAW	Start upgrade to BDS 1.50a.				*/
/* 83Feb26 CrT	bug in makeRoom when out of rooms fixed.		*/
/* 83Feb26 CrT	matchString made caseless, normalizeString()		*/
/* 83Feb26 CrT	"]" directory prompt, user name before prompts		*/
/* 82Dec06 CrT	2.00 release.						*/
/* 82Nov02 CrT	Cleanup prior to V1.2 mods.				*/
/* 82Nov01 CrT	Proofread for CUG distribution. 			*/
/* 82Mar27 dvm	conversion to v. 1.4 begun				*/
/* 82Mar25 dvm	conversion for TRS-80/Omikron test started		*/
/* 81Dec21 CrT	Log file...						*/
/* 81Dec20 CrT	Messages...						*/
/* 81Dec19 CrT	Rooms seem to be working...				*/
/* 81Dec12 CrT	Started.						*/
/************************************************************************/

#include <210ctdl.h>

/************************************************************************/
/*				Contents				*/
/*									*/
/*	editText()		handles the end-of-message-entry menu	*/
/*	findRoom()		find a free room			*/
/*	getNumber()		prompt user for a number, limited range */
/*	getRoom()		load given room into RAM		*/
/*	getString()		read a string in from user		*/
/*	getText()		reads a message in from user		*/
/*	getYesNo()		prompts for a yes/no response		*/
/*	givePrompt()		gives usual "THISROOM>" prompt		*/
/*	indexRooms()		build RAM index to ctdlroom.sys 	*/
/*	makeRoom()		make new room via user dialogue 	*/
/*	matchString()		search for given string 		*/
/*	noteRoom()		enter room into RAM index		*/
/*	putRoom()		store room to given disk slot		*/
/*	renameRoom()		sysop special to rename rooms		*/
/*	replaceString() 	string-substitute for message entry	*/
/*	zapRoomFile()		erase & re-initialize ctdlroom.sys	*/
/************************************************************************/

/************************************************************************/
/*	editText() handles the end-of-message-entry menu.		*/
/*	return TRUE  to save message to disk,				*/
/*	       FALSE to abort message, and				*/
/*	       ERROR if user decides to continue			*/
/************************************************************************/
int editText(buf, lim)
char *buf;
int lim;
{
    char iChar(), toUpper()/*, buff[BUFSIZ]*/;
/*
int count;

    count = 25; */
    do {
	outFlag = OUTOK;
	mPrintf("\n entry cmd: ");
	switch (toUpper(iChar())) {
	case 'A':
	    mPrintf("bort\n ");
	    if (getYesNo(" confirm")) return FALSE;
	    break;
	case 'C':
	    mPrintf("ontinue\n ");
	    return ERROR;
	case 'P':
	    mPrintf("rint formatted\n ");
	    doCR();
	    mPrintf(   "   ");
	    printDate();
	    if (loggedIn)   mPrintf(" from %s", msgBuf.mbauth);
	    doCR();
	    mFormat(buf);
	    break;
	case 'R':
	    mPrintf("eplace string\n ");
	    replaceString(buf, lim);
	    break;
	case 'S':
	    mPrintf("ave buffer\n ");
	    return TRUE;
	default:
	    tutorial("edit.mnu");
/*count--;*/
	    break;
	}
    } while ((haveCarrier  ||  onConsole)/* && count*/);
/*if (count == 0) {
    fcreat("blasted", buff);
    fprintf(buff, "%s\nhC=%d, oC=%d, jLC=%d, Ooops=%d\nnC=%d, wIO=%d\032",
logBuf.lbname,
		haveCarrier, onConsole, justLostCarrier, Ooops, newCarrier,
    whichIO);
    fclose(buff);
    onConsole = FALSE;
    haveCarrier = interpret(pCarrDetect);
    justLostCarrier = !haveCarrier;
}*/
    return FALSE;
}

/************************************************************************/
/*	findRoom() returns # of free room if possible, else ERROR	*/
/************************************************************************/
int findRoom()
{
    int roomRover;

    for (roomRover=0;  roomRover<MAXROOMS;  roomRover++) {
	if (!(roomTab[roomRover].rtflags & INUSE)) return roomRover;
    }
    return ERROR;
}

/************************************************************************/
/*	getNumber() prompts for a number in (bottom, top) range.	*/
/************************************************************************/
int getNumber(prompt, bottom, top)
char   *prompt;
unsigned bottom;
unsigned top;
{
    unsigned try;
    char numstring[NAMESIZE];

    do {
	outFlag = OUTOK;
	getString(prompt, numstring, NAMESIZE);
	try	= atoi(numstring);
	if (try < bottom)  mPrintf("Sorry, must be at least %d\n", bottom);
	if (try > top	)  mPrintf("Sorry, must be no more than %d\n", top);
    } while ((try < bottom ||  try > top)
	  && (haveCarrier  ||  onConsole)
    );
    return  try;
}

/************************************************************************/
/*	getRoom()							*/
/************************************************************************/
getRoom(rm)
int rm;
{
    int      read();
    unsigned val;

    /* load room #rm into memory starting at buf */
    thisRoom	= rm;
    seek(roomfl, rm*SECSPERROOM, 0);
    if ((val = read(roomfl, &roomBuf, SECSPERROOM)) >= 1000)   {
	printf(" ?getRoom(): read failed, val=%d\n", val);
    }
    crypte(&roomBuf, (SECSPERROOM * SECTSIZE), rm);
}

/************************************************************************/
/*	getString() gets a string from the user.			*/
/************************************************************************/
getString(prompt, buf, lim)
char *prompt;
char *buf;
int  lim;	/* max # chars to read */
{
    char iChar();
    char c;
    int  i;

    outFlag = OUTOK;

    if(strLen(prompt) > 0) {
	doCR();
	mPrintf("Enter %s\n : ", prompt, lim);
    }

    i	= 0;
    while (
	 c = iChar(),

	 c	  != NEWLINE
	 && i	  <  lim
	 && (haveCarrier || onConsole)
    ) {
	outFlag = OUTOK;

	/* handle delete chars: */
	if (c == BACKSPACE) {
	    oChar(' ');
	    oChar(BACKSPACE);
	    if (i > 0) i--;
	    else  {
		oChar(' ');
		oChar(BELL);
	    }
	} else	 buf[i++] = c;

	if (i >= lim) {
	    oChar(BELL);
	    oChar(BACKSPACE); i--;
	}

	/* kludge to return immediately on single '?': */
	if (*buf == '?')   {
	    doCR();
	    break;
	}
    }
    buf[i]  = '\0';
}

/************************************************************************/
/*	getText() reads a message from the user 			*/
/*	Returns TRUE if user decides to save it, else FALSE		*/
/************************************************************************/
char getText(prompt, buf, lim, recipient)
char *prompt;
char *buf;
char *recipient;	/* Meaningful iff thisRoom == MAILROOM */
int  lim;	/* max # chars to read */
{
    char iChar(), visible();
    char c, sysopAbort;
    int  i, toReturn;

    outFlag = OUTOK;
    if (!expert)    tutorial("entry.blb");

    outFlag = OUTOK;
    if (!expert)    mPrintf(   "Enter %s (end with empty line)", prompt);

    outFlag = OUTOK;
    doCR();

    mPrintf(   "   ");
    printDate();

    if (loggedIn)  mPrintf("from %s", msgBuf.mbauth);
    if (thisRoom == MAILROOM) mPrintf(" to %s", recipient);
    doCR();
    lim--;
    i  = 0;
    toReturn	= TRUE;
    sysopAbort	= FALSE;
    do {
	if (whichIO == MODEM)	{
	    fastIn(toReturn == ERROR);
	    if (whichIO != MODEM)   sysopAbort	= TRUE;
	} else {
	   /* this code would handle the modem as well...	*/
	   /* fastIn() is a later addition to handle people	*/
	   /* who like to upload fast without handshaking	*/
	   while (
		!(  (c=iChar()) == NEWLINE   &&   buf[i-1] == NEWLINE )
		&& i < lim
		&& (haveCarrier || onConsole)
	   ) {
	       if (debug) putChar(visible(c));

	       if (c != BACKSPACE)  buf[i++]   = c;
	       else {
		   /* handle delete chars: */
		   oChar(' ');
		   oChar(BACKSPACE);
		   if (i>0  &&	buf[i-1] != NEWLINE)   i--;
		   else 			       oChar(BELL);
	       }
	   }

	   buf[i] = 0x00;	       /* null to terminate message	*/

	   if (i == lim)   mPrintf(" buffer overflow\n ");
	}
	toReturn    =	sysopAbort  ?  FALSE  :  editText(buf, lim);
    } while ((toReturn == ERROR)  &&  (haveCarrier || onConsole));
    return  toReturn;
}

/************************************************************************/
/*	getYesNo() prompts for a yes/no response			*/
/************************************************************************/
char getYesNo(prompt)
char *prompt;
{
    char iChar(), toUpper();
    int  toReturn;

    for (
	doCR(),
	toReturn = ERROR;

	toReturn == ERROR   &&	 (haveCarrier || onConsole);
    ) {
	outFlag = OUTOK;
	mPrintf("%s? (Y/N): ", prompt);

	switch (toUpper(iChar())) {
	case 'Y': toReturn	= TRUE ;		break;
	case 'N': toReturn	= FALSE;		break;
	}
	doCR();
    }
    return   toReturn;
}

/************************************************************************/
/*	givePrompt() prints the usual "CURRENTROOM>" prompt.		*/
/************************************************************************/
givePrompt() {
    doCR();

    if (loggedIn)   printf("(%s)\n", logBuf.lbname);

    if (roomBuf.rbflags & CPMDIR)	mPrintf("%s] ", roomBuf.rbname);
    else				mPrintf("%s> ", roomBuf.rbname);
}

/************************************************************************/
/*	indexRooms() -- build RAM index to room.buf			*/
/************************************************************************/
indexRooms()
{
    int goodRoom, m, roomCount, slot;

    roomCount	= 0;
    for (slot=0;  slot<MAXROOMS;  slot++) {
	getRoom(slot);
	if (roomBuf.rbflags & INUSE) {
	    roomBuf.rbflags ^=	INUSE;	    /* clear "inUse" flag */
	    for (m = 0, goodRoom = FALSE; m < MSGSPERRM && !goodRoom; m++)
	    {
		/* comparison done with 64K wraparound in mind: */
		if (roomBuf.msg[m].rbmsgNo - oldestLo < 0x800) {
		    goodRoom	= TRUE;
		}
	    }
	    if (goodRoom   ||	(roomBuf.rbflags & PERMROOM))	{
		roomBuf.rbflags |= INUSE;
	    }

	    if (roomBuf.rbflags & INUSE) roomCount++;
	    else {
		if (weAre == CITADEL) {
		    strCat(msgBuf.mbtext, roomBuf.rbname);
		    strCat(msgBuf.mbtext, "> ");
		}
		roomBuf.rbflags = 0;
		putRoom(slot);
	    }
	}
	noteRoom();
    }
#ifdef XYZZY
    printf(" %d of %d rooms in use\n", roomCount, MAXROOMS);
#endif
}

/************************************************************************/
/*	makeRoom() constructs a new room via dialogue with user.	*/
/************************************************************************/
makeRoom() {
    char getYesNo();
    char *nm[NAMESIZE];
    char *oldName[NAMESIZE];
    int  i;

    /* update lastMessage for current room: */
    logBuf.lbgen[thisRoom]	= roomBuf.rbgen << GENSHIFT;

    strcpy(oldName, roomBuf.rbname);
    if ((thisRoom=findRoom()) == ERROR) {
	indexRooms();	/* try and reclaim an empty room	*/
	if ((thisRoom=findRoom()) == ERROR) {
	    mPrintf(" ?no room");
	    /* may have reclaimed old room, so: */
	    if (roomExists(oldName) == ERROR)	strcpy(oldName, "Lobby");
	    getRoom(roomExists(oldName));
	    return;
	}
    }

    getNormStr("name for new room", nm, NAMESIZE);
    if (strlen(nm) == 0) return ;
    if (roomExists(nm) >= 0) {
	mPrintf(" A '%s' already exists.\n", nm);
	/* may have reclaimed old room, so: */
	if (roomExists(oldName) == ERROR)   strcpy(oldName, "Lobby");
	getRoom(roomExists(oldName));
	return;
    }
    if (!expert)   tutorial("newroom.blb");

    roomBuf.rbflags = INUSE;
    if (getYesNo(" Make room public"))	 roomBuf.rbflags |= PUBLIC;

    mPrintf("'%s', a %s room",
	nm,
	roomBuf.rbflags & PUBLIC  ?  "public"  :  "private"
    );

    if(!getYesNo("Install it")) {
	/* may have reclaimed old room, so: */
	if (roomExists(oldName) == ERROR)   strcpy(oldName, "Lobby");
	getRoom(roomExists(oldName));
	return;
    }

    strcpy(roomBuf.rbname, nm);
    for (i=0;  i<MSGSPERRM;  i++) {
	roomBuf.msg[i].rbmsgNo	 = 0;	 /* mark all slots empty */
	roomBuf.msg[i].rbmsgLoc  = ERROR;
    }
    roomBuf.rbgen = (roomTab[thisRoom].rtgen + 1) % MAXGEN;

    noteRoom(); 			/* index new room	*/
    putRoom(thisRoom);

    /* update logBuf: */
    logBuf.lbgen[thisRoom]	= roomBuf.rbgen << GENSHIFT;
    sprintf(msgBuf.mbtext, "%s> created by %s", nm, logBuf.lbname);
    aideMessage(FALSE);
}

/************************************************************************/
/*	matchString() searches for match to given string.  Runs backward*/
/*	through buffer so we get most recent error first.		*/
/*	Returns loc of match, else ERROR				*/
/************************************************************************/
char *matchString(buf, pattern, bufEnd)
char *buf, *pattern, *bufEnd;
{
    char *loc, *pc1, *pc2;
    char foundIt;

    for (loc = bufEnd, foundIt = FALSE;  !foundIt && --loc >= buf;) {
	for (pc1 = pattern, pc2 = loc,	foundIt = TRUE ;  *pc1 && foundIt;) {
	    if (! (toLower(*pc1++) == toLower(*pc2++)))   foundIt = FALSE;
	}
    }

    return   foundIt  ?  loc  :  ERROR;
}

/************************************************************************/
/*	getNormStr() gets a string and deletes leading			*/
/*					  & trailing blanks etc.	*/
/************************************************************************/
getNormStr(prompt, s, size)
char *s, *prompt;
int  size;
{
    char *pc;

    getString(prompt, s, size);
    pc = s;

    /* find end of string   */
    while (*pc)   {
	if (*pc < ' ')	 *pc = ' ';   /* zap tabs etc... */
	pc++;
    }

    /* no trailing spaces: */
    while (pc>s  &&  isSpace(*(pc-1))) pc--;
    *pc = '\0';

    /* no leading spaces: */
    while (*s == ' ') {
	for (pc=s;  *pc;  pc++)    *pc = *(pc+1);
    }

    /* no double blanks */
    for (;  *s;  s++)	{
	if (*s == ' '	&&   *(s+1) == ' ')   {
	    for (pc=s;	*pc;  pc++)    *pc = *(pc+1);
	}
    }
}

/************************************************************************/
/*	noteRoom() -- enter room into RAM index array.			*/
/************************************************************************/
noteRoom()
{
    int i, last;

    last = 0;
    for (i=0;  i<MSGSPERROOM;  i++)  {
	if (roomBuf.msg[i].rbmsgNo > last) {
	    last = roomBuf.msg[i].rbmsgNo;
	}
    }
    roomTab[thisRoom].rtlastMessage = last	     ;
    strcpy(roomTab[thisRoom].rtname, roomBuf.rbname) ;
    roomTab[thisRoom].rtgen	    = roomBuf.rbgen  ;
    roomTab[thisRoom].rtflags	    = roomBuf.rbflags;
}

/************************************************************************/
/*	putRoom() stores room in buf into slot rm in ctdlroom.sys	*/
/************************************************************************/
putRoom(rm)
int rm;
{
    int      write();
    unsigned val;

    crypte(&roomBuf, (SECSPERROMM * SECTSIZE), rm);

    seek(roomfl, rm*SECSPERROOM, 0);
    if ((val = write(roomfl, &roomBuf, SECSPERROOM)) != SECSPERROOM) {
	printf("?putRoom()%d", val);
    }

    crypte(&roomBuf, (SECSPERROMM * SECTSIZE), rm);
}

/************************************************************************/
/*	renameRoom() is sysop special fn				*/
/*	Returns:	TRUE on success else FALSE			*/
/************************************************************************/
renameRoom() {
    char getYesNo(), toUpper();
    char nm[NAMESIZE];
    char c, goodOne, wasDirectory;
    int  r;

    if (				/* clearer than "thisRoom <= AIDEROOM"*/
	thisRoom == LOBBY
	||
	thisRoom == MAILROOM
	||
	thisRoom == AIDEROOM
    ) {
	mPrintf("? -- may not edit this room.\n ");
	return FALSE;
    }

    if (!getYesNo("confirm"))	return FALSE;

    if (getYesNo("Change name"))   {
	getNormStr("new room name", nm, NAMESIZE);
	r = roomExists(nm);
	if (r>=0  &&  r!=thisRoom) {
	     mPrintf("A %s exists already!\n", nm);
	} else {
	    strcpy(roomBuf.rbname, nm);   /* also in room itself  */
	}
    }
    mPrintf("%s, ", roomBuf.rbflags & PUBLIC ? "public" : "private");
    mPrintf(
	"%s, ",
	(
	    (roomBuf.rbflags & PERMROOM)
	    ?
	    " permanent"
	    :
	    " temporary"
	)
    );
    wasDirectory = roomBuf.rbflags & CPMDIR;
    mPrintf("%sdirectory room\n ", wasDirectory  ?  "" : "non");

    roomBuf.rbflags = INUSE;

    if (getYesNo("Public room"))    {
	roomBuf.rbflags |= PUBLIC;
    } else {
	if (getYesNo("Cause past non-aide users to forget this room"))
	    roomBuf.rbgen    = (roomBuf.rbgen +1) % MAXGEN;
    }

    if (!onConsole)   roomBuf.rbflags |= wasDirectory;
    else if (getYesNo("Directory room")) {
	roomBuf.rbflags    |= CPMDIR;

	printf(" now space %c%c\n", 'A'+roomBuf.rbdisk, '0'+roomBuf.rbuser);

	for (goodOne = FALSE;  !goodOne;  )   {
	    getString("disk", nm, NAMESIZE);
	    c	    = toUpper(nm[0]);
	    if (c>='A'	&& c<='P') {
		roomBuf.rbdisk	    = c - 'A';
		goodOne 	    = TRUE;
	    } else mPrintf("?");
	}

	roomBuf.rbuser = getNumber("user", 0, 31);
	printf(" space %c%c\n", 'A'+roomBuf.rbdisk, '0'+roomBuf.rbuser);
    }


    if (
	roomBuf.rbflags & CPMDIR
	||
	getYesNo("permanent")
    ) {
	roomBuf.rbflags    |= PERMROOM;
    }

    noteRoom();
    putRoom(thisRoom);

    return TRUE;
}

/************************************************************************/
/*	replaceString() corrects typos in message entry 		*/
/************************************************************************/
replaceString(buf, lim)
char *buf;
int  lim;
{
    char oldString[2*SECTSIZE];
    char newString[2*SECTSIZE];
    char *loc, *textEnd;
    char *pc;
    int  incr;

    for (textEnd=buf;  *textEnd;  textEnd++);	/* find terminal null	*/

    getString("string",      oldString, (2*SECTSIZE));
    if ((loc=matchString(buf, oldString, textEnd)) == ERROR) {
	mPrintf("?not found.\n ");
	return;
    }

    getString("replacement", newString, (2*SECTSIZE));
    if ( (strLen(newString)-strLen(oldString))	>=  (&buf[lim]-textEnd) ) {
	mPrintf("?Overflow!\n ");
	return;
    }

    /* delete old string: */
    for (pc=loc, incr=strLen(oldString);  *pc=*(pc+incr);  pc++);
    textEnd -= incr;

    /* make room for new string: */
    for (pc=textEnd, incr=strLen(newString);  pc>=loc;	pc--) {
	*(pc+incr) = *pc;
    }

    /* insert new string: */
    for (pc=newString;	*pc;  *loc++ = *pc++);
}

/************************************************************************/
/*	zapRoomFile() erases and re-initilizes room.buf 		*/
/************************************************************************/
zapRoomFile()
{
    char getCh(), toUpper();
    int i;

    printf("\nWipe room file? ");
    if (toUpper(getCh()) != 'Y') return;

    roomBuf.rbflags	= 0;
    roomBuf.rbgen	= 0;
    roomBuf.rbdisk	= 0;
    roomBuf.rbuser	= 0;
    roomBuf.rbname[0]	= 0;	/* unnecessary -- but I like it...	*/
    for (i=0;  i<MSGSPERRM;  i++) {
	roomBuf.msg[i].rbmsgNo = roomBuf.msg[i].rbmsgLoc = 0;
    }

    printf("maxrooms=%d\n", MAXROOMS);

    for (i=0;  i<MAXROOMS;  i++) {
	printf("clearing room %d\n", i);
	putRoom(i);
    }

    /* Lobby> always exists -- guarantees us a place to stand! */
    thisRoom		= 0		;
    strcpy(roomBuf.rbname, "Lobby")	;
    roomBuf.rbflags	= (PERMROOM | PUBLIC | INUSE);
    putRoom(LOBBY);

    /* Mail> is also permanent...	*/
    thisRoom		= MAILROOM	;
    strcpy(roomBuf.rbname, "Mail")	;
    roomBuf.rbflags	= (PERMROOM | PUBLIC | INUSE);
    putRoom(MAILROOM);

    /* Aide> also...			*/
    thisRoom		= AIDEROOM	;
    strcpy(roomBuf.rbname, "Aide")	;
    roomBuf.rbflags	= (PERMROOM | INUSE);
    putRoom(AIDEROOM);
}
tf("\nWipe room file? ");
    if (toUpper(getCh()) != 'Y'