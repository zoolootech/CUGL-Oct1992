/************************************************************************/
/*				modem.c 				*/
/*									*/
/*		modem code for Citadel bulletin board system		*/
/*	NB: this code is rather machine-dependent:  it will typically	*/
/*	need some twiddling for each new installation.			*/
/*			      82Nov05 CrT				*/
/************************************************************************/

/************************************************************************/
/*				history 				*/
/*									*/
/* 85Jan12 HAW	SECONDSFACTOR == secondsfactor now, for portability.	*/
/* 84Dec28 HAW	Ensure execution of HANGUP when carrier is lost.	*/
/* 84Dec16 JLS&HAW Start installation of new WC protocol downloader.	*/
/* 84Aug22 HAW	Compilation directive for 8085 chips inserted.		*/
/* 84Jul08 JLS & HAW ReadFile() fixed for the 255 rollover.		*/
/* 84Jul03 JLS & HAW All references to putCh changed to putChar.	*/
/* 84Jun23 HAW & JLS Local unused variables zapped.			*/
/* 84Mar07 HAW	Upgrade to BDS 1.50a begun.				*/
/* 83Mar01 CrT	FastIn() ignores LFs etc -- CRLF folks won't be trapped.*/
/* 83Feb25 CrT	Possible fix for backspace-in-message-entry problem.	*/
/* 83Feb18 CrT	fastIn() upload mode cutting in on people.  Fixed.	*/
/* 82Dec16 dvm	modemInit revised for FDC-1, with kludge for use with	*/
/*		Big Board development system				*/
/* 82Dec06 CrT	2.00 release.						*/
/* 82Nov15 CrT	readfile() & sendfile() borrowed from TelEdit.c 	*/
/* 82Nov05 CrT	Individual history file established			*/
/************************************************************************/

#include <210ctdl.h>

/************************************************************************/
/*				Contents				*/
/*									*/
/*	BBSCharReady()		returns true if user input is ready	*/
/*	doWC()			startup routine for the WC downloader.	*/
/*  #	fastIn()		kludge code compiling other stuff inline*/
/*	getCh() 		bottom-level console-input filter	*/
/*  #	getMod()		bottom-level modem-input   filter	*/
/*	iChar() 		top-level user-input function		*/
/*	interact()		chat mode				*/
/*	interpret()		interprets a configuration routine	*/
/*	KBReady()		returns TRUE if a console char is ready */
/*	mOReady()		returns true if modem can accept a char */
/*	modIn() 		returns a user char			*/
/*	modemInit()		top-level initialize-all-modem-stuff	*/
/*	oChar() 		top-level user-output function		*/
/*  #	outMod()		bottom-level modem output		*/
/*	pause() 		pauses for N/100 seconds		*/
/*  #	putChar()							*/
/*	readFile()		accept a file using WC protocol 	*/
/*	receive()		read modem char or time out		*/
/*	ringSysop()		signal chat-mode request		*/
/*	sendWCChar()		Helps chuck output via WC protocol.	*/
/*									*/
/*	# == routines you should certainly check when porting system	*/
/************************************************************************/

/************************************************************************/
/*		The principal dependencies:				*/
/*									*/
/*  iChar   modIn				    outMod		*/
/*	    modIn   getMod  getCh   mIReady kBReady outMod  carrDetect	*/
/*		    getMod						*/
/*			    getCh					*/
/*				    mIReady				*/
/*					    kBReady			*/
/*							    carrDetect	*/
/*									*/
/*  oChar					    outMod		*/
/*						    outMod  mOReady	*/
/************************************************************************/


/************************************************************************/
/*	BBSCharReady() returns TRUE if char is available from user	*/
/*	NB: user may be on modem, or may be sysop in CONSOLE mode	*/
/************************************************************************/
char BBSCharReady()
{
    char KBReady();

    return ((haveCarrier       &&   interpret(pMIReady))
	 || (whichIO==CONSOLE  &&   KBReady()	       )
    );
}

/************************************************************************/
/*	doWC() is the initialization routine for WC downloading.  If	*/
/*	called with mode == STARTUP, it sets up globals and gets ready	*/
/*	for the initial NAK.  If mode == anything else, then it cleans	*/
/*	up after the downloader.					*/
/************************************************************************/
doWC(mode)
char mode;		/* See above */
{
    int i, m;

    if (mode == STARTUP) {	/* Setup globals for the coming fun.	*/
	WCError  = FALSE;
	WCSecNum = 1;
	i	 = 0;
	WCChar	 = 0;
	while (1) {		/* Get that darn initial NAK		*/
	    m = receive(MINUTE);
	    if (m == CAN || m == ERROR) /* Didn't get initial		*/
		return FALSE;		/* NAK, so time out ..		*/
	    if (m == NAK)		/* There it is! 		*/
		return TRUE;
	    if (++i == ERRORMAX)	/* 10 chars, no NAKs?		*/
		return FALSE;		/* Then ferget it.		*/
	}
    }
    else {	/* Cleanup after downloader				*/
	if (WCError) return;
	for (; WCChar != 0 && sendWCChar(' ');)
	    ;					/* Do final sector	*/
	if (WCError) return;
	for (i = 0; i < ERRORMAX; i++) {
	    outMod(EOT);
	    if (receive(MINUTE) == ACK || !haveCarrier) break;
	}
    }
}

/************************************************************************/
/*	fastIn() is  a special kludge to read in text from the modem	*/
/*	as quickly as possible, to allow message upload without 	*/
/*	handshaking.  Hence we hand-compile some other routines inline	*/
/*	Called only from getText(), when whichIO==MODEM.		*/
/*	Externals:    see below 					*/
/*									*/
/*	This code is probably overkill for 300 baud, but it may handle	*/
/*	1200 baud as well.						*/
/*									*/
/*	code being speed-optimized would normally be:			*/
/*									*/
/*	while ( 							*/
/*	    !(	(c=iChar()) == NEWLINE	 &&   buf[i-1] == NEWLINE )	*/
/*	    &&	i < lim 						*/
/*	    &&	(haveCarrier || whichIO == CONSOLE)			*/
/*	) {								*/
/*	    if (c != BACKSPACE)  buf[i++] = c;				*/
/*	    else {							*/
/*		/  handle deletes:	 /				*/
/*		oChar(' ');						*/
/*		oChar(BACKSPACE);					*/
/*		if (i>0  &&  buf[i-1] != NEWLINE)  i--; 		*/
/*		else				   oChar(BELL); 	*/
/*	    }								*/
/*	}								*/
/************************************************************************/
#define cursor		fpc1		/* external char *		*/
#define BUFEND		fpc2		/* external char *		*/
#define tryCount	fi1		/* external int 		*/
#define isFast		fi2		/* external int 		*/
#define ch		fc1		/* external char		*/
#define lastWasNL	fc2		/* external char		*/
#define slow		fc3		/* external char		*/
fastIn(continuing)
char continuing;	/* TRUE if we are continuing a message	*/
{
    char cache, notFinished;
    int  tryStart, shortTime;

    isFast	= 0;

    tryStart	= 500*megaHz;
    shortTime	= 480*megaHz;
    cursor	= &msgBuf.mbtext[0	  ];
    BUFEND	= &msgBuf.mbtext[MAXTEXT-1];

    if (continuing)   while (*cursor) ++cursor; /* find where we were	*/

    notFinished = TRUE;
    lastWasNL	= FALSE;

    /* put newline at start of buffer to simplify BACKSPACE check:	*/
    cache		  = msgBuf.mbtext[-1];
    msgBuf.mbtext[-1]	  = NEWLINE;

    while (notFinished	 &&   cursor < BUFEND) {
	tryCount = tryStart;	/* try to be waiting for each char	*/
	while (--tryCount  &&  !interpret(pMIReady));

	if (tryCount>shortTime) isFast++;
	else			isFast--;

	if (!tryCount) {
	    /* no modem char -- take break to check other stuff */
	    if (KBReady()) {
		if (getCh() == SPECIAL) {
		    mPrintf("\n system is now in CONSOLE mode.\n ");
		    whichIO	= CONSOLE;
		    notFinished = FALSE;
		    setUp(FALSE);
		}
	    }
	    if (!interpret(pCarrDetect)) {
		modIn();	/* let modIn() announce it etc	*/
		notFinished	= FALSE;
	    }
	} else {
	    /* time to read modem char: */
	    switch (ch =  filter[ inp(mData) & 0x7F ]) {
	    case NEWLINE:
		if (lastWasNL)	{
		    notFinished = FALSE;
		} else {
		    lastWasNL	= TRUE;

		    *cursor++	= ch;

		    if (isFast>0  ||  !termLF)	 ch = '\r';
		    else {
			oChar('\r');
			while (!interpret(pMOReady));	/* for outp()	*/
		    }
		}
		isFast	= 0;	/* figure speed of each line independently */
		break;
	    case BACKSPACE:
		/* people don't upload backspaces -- one hopes! --	*/
		/* so we don't worry about speed so much here:		*/
		if (*--cursor == NEWLINE) {
		    /* trying to erase to previous line -- disallow	*/
		    /* because we have no upline:			*/
		    ch = BELL;
		    cursor++;
		} else {
		    oChar(BACKSPACE);
		    oChar(' ');     /* erase last char			*/
		}
		while (!interpret(pMOReady));		/* for outp()	*/
		break;
	    case '\0':
		/* ignore unwanted chars completely: */
		break;
	    default:
		lastWasNL	= FALSE;
		*cursor++	= ch;
		break;
	    }
	    /* echo to console--expendable but nice: */
	    putChar(ch);   /* Let putChar decide if the console should see it */
	    while (!interpret(pMOReady));	    /* for outp()   */
	    outp(mData, ch);	/* (was:)assume port is ready by now	      */
	}
    }
    *cursor		= '\0'; 	/* tie off message		*/
    msgBuf.mbtext[-1]	= cache;	/* return borrowed space	*/

    if (cursor == BUFEND)
	mPrintf("\n \7BUFFER OVERFLOW\n ");
}


/************************************************************************/
/*	getCh() reads a console char					*/
/*	    In CONSOLE mode, CRs are changed to newlines		*/
/*	    Rubouts are changed to backspaces				*/
/*	Returns:	resulting char					*/
/************************************************************************/
char getCh()
{
    char bios();

    return bios(3);
}

/************************************************************************/
/*	getMod() is bottom-level modem-input routine			*/
/*	  kills any parity bit						*/
/*	  rubout			-> backspace			*/
/*	  CR				-> newline			*/
/*	  other nonprinting chars	-> blank			*/
/*	Returns: result 						*/
/************************************************************************/
char getMod() {
	char inp();

	return inp(mData) & 0x7F;
}

/************************************************************************/
/*	iChar() is the top-level user-input function -- this is the	*/
/*	function the rest of Citadel uses to obtain user input		*/
/************************************************************************/
char iChar()
{
    char modIn();
    char c;

    if (justLostCarrier)   return 0;	/* ugly patch	*/

    c = filter[modIn()];

    switch (echo) {
    case BOTH:
	if (haveCarrier) {
	    if (c == '\n')	doCR();
	    else		outMod(c);
	}
	putChar(c);	/* Let putChar decide if it should go on console */
	break;
    case CALLER:
	if (whichIO == MODEM) {
	    if (c == '\n')	doCR();
	    else		outMod(c);
	} else {
	    putChar(c);
	}
	break;
    }
    return(c);
}

/************************************************************************/
/*	interact() allows the sysop to interact directly with		*/
/*	whatever is on the modem.			       dvm 9-82 */
/************************************************************************/
interact() {
    char c, lineEcho, lineFeeds, localEcho;
    char getMod(), getCh(), interpret();

    printf(" Direct modem-interaction mode\n");
    lineEcho	= getYesNo("Echo to modem"     );
    localEcho	= getYesNo("Echo keyboard"     );
    lineFeeds	= getYesNo("Echo CR as CRLF"   );
    printf("<ESC> to exit\n");

    /* incredibly ugly code.  Rethink sometime: */
    while (c!=SPECIAL) {
	c = 0;

	if (c=interpret(pMIReady) ) {
	    c  = inp(mData) & 0x7F;
	    if (c != '\r') c = filter[c];
	    if (c != '\r') {
		if (lineEcho)	outMod(c);
		putChar(c);
	    } else {
		if (!lineFeeds) {
		    if (lineEcho) outMod('\r');
		    putChar('\r');
		} else {
		    if (lineEcho) {
			outMod('\r');
			outMod('\n');
		    }
		    putChar('\r');
		    putChar('\n');
		}
	    }
	}
	if (KBReady()) {
	    c  = filter[getCh()];
	    if (c != NEWLINE) {
		if (localEcho)	putChar(c);
		outMod(c);
	    } else {
		if (!lineFeeds) {
		    if (localEcho) putChar('\r');
		    outMod('\r');
		} else {
		    if (lineEcho) {
			putChar('\r');
			putChar('\n');
		    }
		    outMod('\r');
		    outMod('\n');
		}
	    }
	}
    }
}

/************************************************************************/
/*	interpret() interprets a configuration routine			*/
/*	Returns byte value computed					*/
/************************************************************************/
char interpret(instr)
union {
    char **pp;
    int  *pi;
    char *pc;
} instr;
{
    char inp();
    char accum; 	/* our sole accumulator */
    char *prompt;
    int  lowLim, topLim;

    while (TRUE) {
	switch (*instr.pc++) {
	case RET:	return accum;				break;
	case RET0:	if (!accum) return 0;			break;
	case RET1:	if (accum) return 1;			break;
case BOUT:	printf("\n%c %d\n", accum, accum); break;
	case ANDI:	accum	       &= *instr.pc++;		break;
	case INP:	accum		= inp(*instr.pc++);	break;
	case XORI:	accum	       ^= *instr.pc++;		break;

	case LOAD:	accum		= *(*instr.pp++);	break;
	case LOADI:	accum		= *instr.pc++;		break;
	case LOADX:	accum		= scratch[*instr.pc++]; break;
	case ORI:	accum	       |= *instr.pc++;		break;
	case OUTP:	outp(*instr.pc++, accum);		break;
	case PAUSEI:	pause(*instr.pc++);			break;
	case STORE:	*(*instr.pp++)	= accum;		break;
	case STOREX:	scratch[*instr.pc++]	= accum;	break;
	case OPRNUMBER:
	    prompt	= instr.pc;
	    while(*instr.pc++); 	/* step over prompt	*/
	    lowLim	= *instr.pc++;
	    topLim	= *instr.pc++;
	    accum	= getNumber(prompt, lowLim, topLim);
	    break;
	case OUTSTRING:
	    while(*instr.pc) {
		pause(5);	/* SmartModem can't handle 300 baud	*/
		outMod(*instr.pc++);	/* output string */
	    }
	    instr.pc++; 				/* skip null	 */
	    break;
	default:
	    printf("intrp-no opcod%d", *(instr.pc-1));
	    break;
	}
    }
}

/************************************************************************/
/*	KBReady() returns TRUE if a console char is ready		*/
/************************************************************************/
char KBReady()
{
    char bios();

    return bios(2);
}

/************************************************************************/
/*	modemInit() is responsible for all modem-related initialization */
/*	at system startup						*/
/*	Globals modified:	haveCarrier	visibleMode		*/
/*				whichIO 	modStat 		*/
/*				exitToCpm	justLostCarrier 	*/
/*	modified 82Dec10 to set FDC-1 SIO-B clock speed at		*/
/*	300 baud	 -dvm						*/
/************************************************************************/
modemInit()
{
    newCarrier		= FALSE;
    Ooops		= '\n';
    visibleMode 	= FALSE;
    exitToCpm		= FALSE;
    justLostCarrier	= FALSE;

    whichIO		= CONSOLE;

#ifdef FDC-1
#define MONBASE 0xF800
#define STSSP	MONBASE+0x045
    call(STSSP, 0, 0, 0x0B, 0x05);	/* 300 baud on SIO-B */
#endif

#ifdef VFC-2
    /* dummy call (to CONSTAT) to make code size equal for both systems */
    call(0xF006, 0, 0, 0x0B, 0x05);
#endif

    if (!rcpm) {
	interpret(pInitPort);
	interpret(pHangUp);
    }
    haveCarrier = modStat = interpret(pCarrDetect);
}

/************************************************************************/
/* modIn() toplevel modem-input function				*/
/*   If DCD status has changed since the last access, reports		*/
/*   carrier present or absent and sets flags as appropriate.		*/
/*   In case of a carrier loss, waits 20 ticks and rechecks		*/
/*   carrier to make sure it was not a temporary glitch.		*/
/*   If carrier is newly received, returns newCarrier = TRUE;  if	*/
/*   carrier lost returns 0.  If carrier is present and state		*/
/*   has not changed, gets a character if present and			*/
/*   returns it.  If a character is typed at the console,		*/
/*   checks to see if it is keyboard interrupt character.  If		*/
/*   so, prints short-form console menu and awaits next 		*/
/*   keyboard character.						*/
/* Globals modified:	carrierDetect	modStat 	haveCarrier	*/
/*			justLostCarrier whichIO 	exitToCpm	*/
/*			visibleMode					*/
/* Returns:	modem or console input character,			*/
/*		or above special values 				*/
/************************************************************************/
char modIn()
{
    char	getMod(), getCh(), interpret(), KBReady();
    char	c;
    unsigned	hi, lo;

    hi	= (HITIMEOUT * megaHz);
    lo	= 0xFF;
    while (TRUE) {
	if ((whichIO==MODEM) && (c=interpret(pCarrDetect)) != modStat) {
	    /* carrier changed	 */
	    if (c)  {	   /* carrier present	*/
		printf("Carr-detect\n");
		haveCarrier	= TRUE;
		pause(200);
		modStat 	= c;
		newCarrier	= TRUE;
		justLostCarrier = FALSE;
		return(0);
	    } else {
		pause(200);		    /* confirm it's not a glitch */
		if (!interpret(pCarrDetect)) {	  /* check again */
		    printf("Carr-loss\n");

		    interpret(pHangUp);
		    haveCarrier     = FALSE;
		    modStat	    = FALSE;
		    justLostCarrier = TRUE;

/*		    while(interpret(pMIReady)) getMod();    eat garbage */

		    return(0);
		}
	    }
	}

	if (interpret(pMIReady)) {
	    if (haveCarrier) {
		c = getMod();
		if (whichIO == MODEM)	return c;
	    }
	}

	if (KBReady()) {
	    c = getCh();
	    if (whichIO == CONSOLE) return(c);
	    else {
		if (c == SPECIAL) {
		    printf("CONSOLE mode\n ");
		    whichIO = CONSOLE;
		    setUp(FALSE);
		    return 0;
		}
	    }
	}

	/* check for no input.	(Short-circuit evaluation, remember!) */
	if (whichIO==MODEM  &&	haveCarrier  &&  !--lo	&&  !--hi) {
	    mPrintf("Sleeping? Call again :-)");
	    interpret(pHangUp);
	}
    }
}

/************************************************************************/
/*	oChar() is the top-level user-output function			*/
/*	  sends to modem port and console both				*/
/*	  does conversion to upper-case etc as necessary		*/
/*	  in "debug" mode, converts control chars to uppercase letters	*/
/*	Globals modified:	prevChar				*/
/************************************************************************/
oChar(c)
char c;
{
    prevChar = c;			/* for end-of-paragraph code	*/
    if (outFlag) return;		/* s(kip) mode			*/

    if (termUpper)	c = toupper(c);
    if (debug)		c = visible(c);
    if (c == NEWLINE)	c = ' ';	/* doCR() handles real newlines */

    /* show on console		    */
    putChar(c);

    if (usingWCprotocol)
	sendWCChar(c);
    else if (haveCarrier)
	outMod(c);
}


/************************************************************************/
/*	outMod stuffs a char out the modem port 			*/
/************************************************************************/
outMod(c)
char c;
{
	while(!interpret(pMOReady));
	outp(mData, c);
}

/************************************************************************/
/*	pause() busy-waits N/100 seconds				*/
/************************************************************************/
pause(i)
int i;
{
    int j;

    for (;  i;	i--) {
	for (j=(secondsfactor*megaHz);	j;  j--);
    }
}

/************************************************************************/
/*	putChar()							*/
/************************************************************************/
putChar(c)
char c;
{
    if (c != ESC && (echo == BOTH || (whichIO == CONSOLE && echo != NEITHER))) {
	bios(4, c);
	if (c == '\n')
	    putChar('\r');
    }
}

/************************************************************************/
/*	receive() gets a modem character, or times out ...		*/
/*	Returns:	char on success else ERROR			*/
/************************************************************************/
int receive(seconds)
int  seconds;
{
    unsigned  count;

    count = seconds * 100;
    while (!interpret(pMIReady)  &&  --count)  pause(1);
    if (count)	return inp(mData);

    return(ERROR);
}

/************************************************************************/
/*	readFile() accepts a file from modem using Ward Christensen's	*/
/*	protocol.  (ie, compatable with xModem, modem7, yam, modem2...) */
/*	Returns:	TRUE on successful transfer, else FALSE 	*/
/************************************************************************/
char readFile(pc)
int  (*pc)();	/* pc will accept the file one character at a time.	*/
		/* returns ERROR on any problem, and closes the file	*/
		/* when handed ERROR as an argument.			*/
{
    int  i, firstchar, lastSector, thisSector, thisComplement, tries;
    int  toterr, checksum;
    char badSector, writeError;
    char sectBuf[SECTSIZE];
    char *nextChar;

    if (!getYesNo("Ready for WC transfer"))   return FALSE;

    lastSector	= 0;
    tries	= 0;
    toterr	= 0;
    writeError	= FALSE;

    while (interpret(pMIReady))   inp(mData);	/* clear garbage	*/

    printf("Awaiting #0 (Try=0, Errs=0)  \r");

    do {
	badSector = FALSE;

	/* get synchronized: */
	do {
	    firstchar = receive(10);
	} while (
	    firstchar != SOH &&
	    firstchar != EOT &&
	    firstchar != ERROR
	);

	if (firstchar == ERROR)   badSector = TRUE;

	if (firstchar == SOH)  {
	    /* found StartOfHeader -- read sector# in: */
	    thisSector		= receive (1);
	    thisComplement	= receive (1);	/* 1's comp of thisSector */

	    if ((thisSector + thisComplement) != 0xFF)	badSector = TRUE;
	    else {
		if (thisSector == (lastSector + 1) % 256) {
		    /* right sector... let's read it in */
		    checksum	= 0;
		    nextChar	= sectBuf;
		    for (i=SECTSIZE;  i;  i--) {
			*nextChar	= receive (1);
			checksum	= (checksum + *nextChar++) & 0xFF;
		    }

		    if (checksum != receive (1))  badSector = TRUE;
		    else {
			tries		= 0;
			lastSector	= thisSector % 256;

			printf("Awaiting #%d (Try=0, Errs=%d)  \r",
			    thisSector, toterr
			);

			if (tries && toterr) putchar('\n');

			/* write sector to where-ever: */
			nextChar = sectBuf;
			for (i=SECTSIZE;  i;  i--) {
			    writeError	 &= (*pc)(*nextChar++) == ERROR;
			}
			if (!writeError)  outMod(ACK);
		    }
		} else	{
		    /* not expected sector... */
		    if (thisSector != lastSector)  badSector = TRUE;
		    else {
			/* aha -- sender missed an ACK and resent last: */
			do;  while (receive(1) != ERROR);   /* eat it */
			outMod(ACK);	/* back in synch! */
		    }
		}
	    }	/* end of "if (thisSector + thisComplement == 255"	*/
	}	/* end of "if (firstChar == SOH)"			*/

	if (badSector)	{
	    tries++;
	    if (lastSector != 0)  toterr++;

	    while (receive (1) != ERROR);
	    printf("Awaiting #%d (Try=%d, Errs=%d)  \r",
		lastSector, tries, toterr
	    );
	    if (tries && toterr) putchar('\n');
	    outMod(NAK);
	}
    } while (
	firstchar != EOT       &&
	tries	  <  ERRORMAX  &&
	!writeError
    );

    if (firstchar != EOT   ||	tries >= ERRORMAX) {
	printf("\naborting\n");
	return FALSE;
    } else {
	outMod(ACK);
	printf("\nfile reception complete.\n");
	return TRUE;
    }
}

/************************************************************************/
/*	ringSysop() signals a chat mode request.  Exits on input from	*/
/*  modem or keyboard.							*/
/************************************************************************/
ringSysop()
{
    char BBSCharReady();
    int  i;

    whichIO = CONSOLE;
    mPrintf("\n Ringing sysop.\n ");

    for (i=0; !BBSCharReady() && !KBReady() && interpret(pCarrDetect);
							   i = ++i % 7)  {
	/* play shave-and-a-haircut/two bits... as best we can: */
	oChar(BELL);
	pause(shave[i]);
    }

    if (!interpret(pCarrDetect))
	modIn();			/* Let modIn() announce it	*/

    if (KBReady())   {
	getCh();
	interact();
    }
    whichIO = MODEM;
}

/************************************************************************/
/*	SendWCChar() sends data via WC protocol.			*/
/************************************************************************/
sendWCChar(c)
char c;
{
    int  i, j, m;
    char ck;

    if (WCError)
	return FALSE;
    WCBuf[WCChar++] = c;		/* Store in buffer		*/
    if (WCChar != SECTSIZE)
	return TRUE;
    for (i = 0; i < ERRORMAX; i++) {	/* Time to transmit		*/
	outMod(SOH);
	outMod(WCSecNum);
	outMod(~WCSecNum);
	for (j = ck = 0; j < SECTSIZE; j++) {
	    outMod(WCBuf[j]);
	    ck += WCBuf[j];
	}
	outMod(ck);
	m = receive(MINUTE);
	if (m == ACK || m == CAN || !haveCarrier) break;
    }
    WCChar = 0;
    WCSecNum++;
    if (m == ACK) return TRUE;
    /* else */
    WCError = TRUE;
    return FALSE;		/* Indicates receiver is allergic to us */
}
 if (!interpre