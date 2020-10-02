/************************************************************************/
/*				misc.c					*/
/*									*/
/*	Random functions...						*/
/************************************************************************/

/************************************************************************/
/*				history 				*/
/*									*/
/* 84Dec18 JLS	transmitFile() fixed so mAbort() affects <.rb> cmd.	*/
/* 84Dec17 JLS	Tutorial() fixed so on mabort() is stops instantly.	*/
/* 84Dec16 HAW	Tutorial() fixed so it can't access non-existent drives.*/
/* 84Dec16 HAW&JLS download() & transmitFile() modified for new WC.	*/
/* 84Jul08 HAW	file_RO changed to detect R/O drives.			*/
/* 84Jul03 HAW	file_RO written to check .SYS files for R/O status.	*/
/* 84Jun23 HAW & JLS  patchDebug put under control of conditional comp. */
/* 84Jun16 JLS & HAW  Configure fixed so that ".Enter Con..." works	*/
/* 84Jun10 JLS	Function changedate() installed.			*/
/* 84May01 HAW	Starting 1.50a upgrade. 				*/
/* 83Mar12 CrT	from msg.c						*/
/* 83Mar03 CrT & SB   Various bug fixes...				*/
/* 83Feb27 CrT	Save private mail for sender as well as recipient.	*/
/* 83Feb23	Various.  transmitFile() won't drop first char on WC... */
/* 82Dec06 CrT	2.00 release.						*/
/* 82Nov05 CrT	Stream retrieval.  Handles messages longer than MAXTEXT.*/
/* 82Nov04 CrT	Revised disk format implemented.			*/
/* 82Nov03 CrT	Individual history begun.  General cleanup.		*/
/************************************************************************/

#include <210ctdl.h>

/************************************************************************/
/*				contents				*/
/*									*/
/*	changeDate()		allow changing of date			*/
/*	configure()		sets terminal parameters via dialogue	*/
/*	doCR()			newline on modem and console		*/
/*	download()		menu-level routine for WC-protocol sends*/
/*	file_RO()		checks for file and drive being R/O	*/
/*	patchDebug()		display/patch byte			*/
/*	printDate()		prints out date 			*/
/*	putFlChar()		readFile() -> disk file interface	*/
/*	putWCChar()		filter[]s, readFile() to putMsgChar()	*/
/*	transmitFile()		send a host file, no formatting 	*/
/*	tutorial()		prints a .hlp file			*/
/*	upload()		menu-level read-via-WC-protocol fn	*/
/*	visible()		convert control chars to letters	*/
/************************************************************************/

/************************************************************************/
/*	changedate() gets the date from the aide and remembers it	*/
/************************************************************************/
changeDate()
{
    mPrintf("Current date is: ");
    printdate();
    if (!getYesNo("Enter a new date"))
	return ;
    interpret(pInitDate);
}

/************************************************************************/
/*	configure() sets up terminal width etc via dialogue		*/
/************************************************************************/
configure() {
    termWidth	= getNumber(" Screen width",	      10, 255);
    termNulls	= getNumber(" #Nulls",		       0, 255);
    termUpper	= getYesNo(" Uppercase only"		     )	?  UCMASK  :  0;
    termLF	= getYesNo(" Linefeeds" 		     )	?  LFMASK  :  0;
/*  termTab	= getYesNo(" Tabs"			     )	?  TABMASK :  0;
*/  expert	=!getYesNo(" Helpful hints"		     )	?  EXPERT  :  0;

	/* Now make it (only) semi-permanent	 --JLS&HAW */
    logBuf.lbwidth = termWidth;
    logBuf.lbnulls = termNulls;
    logBuf.lbflags = expert | termUpper | termLF | termTab | aide;
}

/************************************************************************/
/*	doCR() does a newline on modem and console			*/
/************************************************************************/
doCR() {
    int i;

    crtColumn	= 1;
    if (outFlag) return;	/* output is being s(kip)ped	*/

    putChar(NEWLINE);
    if (haveCarrier) {
	if (!usingWCprotocol) {
	    outmod('\r');
	    for (i=termNulls;  i;  i--) outmod(0);
	    if (termLF) outmod('\n');
	} else {
	    sendWCChar('\r');
	    if (termLF) sendWCChar('\n');
	}
    }
#ifdef XYZZY
/* test code only */
    else {
	if (usingWCprotocol) {
	    sendWCChar('\r');
	    if (termLF) sendWCChar('\n');
	}
    }
#endif
    prevChar	= ' ';
}

/************************************************************************/
/*	download() is the menu-level send-message-via-WC-protocol fn	*/
/************************************************************************/
download(whichMess, revOrder)
char whichMess, revOrder;
{
    outFlag	= OUTOK;
    if (!expert)   tutorial("wcdown.blb");

    outFlag	= OUTOK;
    if (!getYesNo("Ready"))  return;

    /* here we go: */
    echo		= NEITHER;
    usingWCprotocol	= TRUE	 ;	/* all points bulletin		*/

    if (doWC(STARTUP))	{	/* Caller wasn't ready, so don't try	*/

	showMessages(whichMess, revOrder);

	doWC(FINISH);		    /* Clean up after the WC downloader     */
    }

    setUp(FALSE);
    usingWCprotocol	= FALSE  ;
}

/************************************************************************/
/*	file_RO() checks the given file for R/O status	--HAW		*/
/************************************************************************/
file_RO(fd)
int fd;
{
#define R_O_MASK	128
	char *fc, *fcbaddr(), drive;
	int  vector, bdos();

	if ((fc = fcbaddr(fd)) == ERROR)
		return TRUE;		/* No such file, return  TRUE */
	if ((fc[9] & R_O_MASK) != 0)
		return TRUE;		/* Bit set, file is R/O */
	/* else */
	if ((drive = fc[0]) == 0)	/* Then get default disk from BDOS */
		drive = bdos(25) + 1;
					/* Wheee!  Bit operations. */
	vector = (bdos(29) << (16 - drive)) & 0x8000;
	return vector;
}

#ifdef XYZZY
/************************************************************************/
/*	patchDebug()							*/
/************************************************************************/
patchDebug() {
    unsigned  i, j, page;
    char      finished, *pc;

    mPrintf("\bpatch\n ");
    page = getNumber("page", 0, 255);
    finished = FALSE;
    do {
	for (i=16, pc= (page*256);  i;	i--)   {
	    mPrintf("%d ", pc);
	    for (j=16;	j;  j--)   {
		mPrintf("%c", visible(*pc++));
	    }
	    mPrintf("\n ");
	}
	switch (toLower(iChar()))   {
	case 'r':
	    pc	= getNumber("adr", 0, 65355);
	    *pc = getNumber("val", 0, 255);
	    break;
	case 'n':	page++; 		break;
	case 'p':	page--; 		break;
	default:	finished = TRUE;	break;
	}
    } while (!finished);
}
#endif
/************************************************************************/
/*	printDate() prints out current date.				*/
/************************************************************************/
printDate()
{
    mPrintf("%d%s%02d ", interpret(pGetYear),
			 monthTab[interpret(pGetMonth)],
			 interpret(pGetDay));
}

/************************************************************************/
/*	putFlChar() is used to upload files				*/
/*	returns: ERROR on problems else TRUE				*/
/*	Globals: msgBuf.mbtext is used as a buffer, to save space	*/
/************************************************************************/
putFlChar(c)
char c;
{
    return  putC(c, msgBuf.mbtext) != ERROR;
}

/************************************************************************/
/*	putWCChar() filter[]s from readFile() to putMsgChar()		*/
/*	Returns: ERROR if problems, else TRUE				*/
/*	Globals: outFlag is set to OUTSKIP when an ascii EOF (^Z) is	*/
/*	    encountered, and no further text is written to disk.	*/
/************************************************************************/
int putWCChar(c)
int c;
{
    if (outFlag)	return TRUE;	/* past ascii EOF.  ignore.	*/

    if (c == ERROR) {
	/* EOF.  Note for overkill:	*/
	outFlag = OUTSKIP;
	return TRUE;
    }

    c	&= 0x7F;	/* cut high stuff off	*/

    if (c == CPMEOF) {
	/* ascii EOF.  Ignore rest of file:	*/
	outFlag = OUTSKIP;
	return TRUE;
    }

    return  putMsgChar(filter[c]);
}

/************************************************************************/
/*	transmitFile() dumps a host file with no formatting		*/
/************************************************************************/
transmitFile(filename)
char *filename;
{
    char mAbort();
    int  getc(), sendWCChar();
    char fbuf[BUFSIZ], fname[NAMESIZE];
    int  c;

    outFlag	= OUTOK;

    unspace(filename, fname);

    if(fopen(fname, fbuf) == ERROR) {
	mPrintf("\n No %s.\n", fname);
	return(ERROR);
    }

    doCR();
    if (usingWCprotocol)   {
	if (!doWC(STARTUP)) {
	    fClose(fbuf);
	    return ERROR;
	}
    }

    while ((c=getc(fbuf)) != ERROR)  {
	if (!usingWCprotocol)
	    putChar(c);
	if (whichIO == MODEM) {
	    if (!usingWCprotocol) outMod(c);
	    else if (!sendWCChar(c)) break;
	}
	if (textDownload      &&  c==CPMEOF)  break;
	if (!usingWCprotocol  &&  mAbort())   break;
    }

    if (usingWCprotocol)   doWC(FINISH);

    fClose(fbuf);
}

/************************************************************************/
/*	tutorial() prints file <filename> on the modem & console	*/
/*	Returns:	TRUE on success else ERROR			*/
/************************************************************************/
#define MAXWORD 256
tutorial(filename)
char *filename;
{
    char fbuf[BUFSIZ];
    int  c;
    int  toReturn;

    toReturn	= TRUE;

    outFlag	= OUTOK;
    if (filename[1] == ':' || fopen(filename, fbuf) == ERROR) {
	mPrintf("\n No %s.\n", filename);
	toReturn	= ERROR;
    } else {
	if (!expert)   mPrintf("\n <J>ump <P>ause <S>top\n");
	mPrintf(" \n");
	while ((c=getc(fbuf)) != ERROR)  {
	    putChar(c);
	    if (whichIO == MODEM)
		outMod(c);
	    if (c==CPMEOF || mAbort())	 break;
	}
	fClose(fbuf);
    }

    return   toReturn;
}

/************************************************************************/
/*	upLoad() enters a file into current directory			*/
/************************************************************************/
upLoad() {
    char fileName[NAMESIZE], *s;
    int putFlChar();

    getNormStr("filename", fileName, NAMESIZE);

    if (fileName[1] != ':')   s = &fileName[0];
    else		      s = &fileName[2];


    setSpace(roomBuf.rbdisk, roomBuf.rbuser);

    if(fOpen(s, msgBuf.mbtext) != ERROR) {
	mPrintf("\n A %s already exists.\n", s);
    } else {
	if (!expert) {
	    setSpace(homeDisk, homeUser);
	    tutorial("wcupload.blb");
	    setSpace(roomBuf.rbdisk, roomBuf.rbuser);
	}
	if (fCreat(s, msgBuf.mbtext) == ERROR) {
	    mPrintf("\n Can't create %s!\n", s);
	} else {

	    readFile(putFlChar);

	    fFlush(msgBuf.mbtext);
	    fClose(msgBuf.mbtext);
	}
    }
    setSpace(homeDisk, homeUser);
}

/************************************************************************/
/*	visible() converts given char to printable form if nonprinting	*/
/************************************************************************/
char visible(c)
char c;
{
    if (c==0xFF)  c = '$'	;   /* start-of-message in message.buf	*/
    c		    = c & 0x7F	;   /* kill high bit otherwise		*/
    if ( c < ' ') c = c + 'A' -1;   /* make all control chars letters	*/
    if (c== 0x7F) c = '~'	;   /* catch DELETE too 		*/
    return(c);
}
 roomBuf.rbuser);

    if(fOpen(s, msgBuf.mbtex