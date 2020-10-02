/*
$title ('yam2.c: file transmission protocol handlers')
$date (4 nov 85)
*/
/*
 * Ward Christensen Protocol handler for sending and receiving
 * ascii and binary files.  Modified for choice of checksum or crc.
 */

#include "yam.h"
#define WCEOT (-10)

#ifdef DEFBYTL
long Modtime;		/* Unix style mod time for incoming file */
int Filemode;		/* Unix style mode for incoming file */
#endif

/****************************************************************************
FUNCTION:
	Ward Christensen modem 7 protocol file tranmission handler

CALLING PARAMETERS:
	argc:
		count of command line arguments.
	argp:
		pointer to string of command line arguments.
===========================================================================*/
wcsend(argc, argp)
char **argp;
{
	int wcs();

#ifdef IBMPC
		/* enbable raw mode */
	enblcm(FALSE);
#endif

	Crcflg=FALSE;
	firstsec=TRUE;

	if (Batch)
	{
		printf("Sending in Batch Mode\n");

			/* send files, batch mode */
		if (expand(wcs, argc, argp)==ERROR)
			goto fubar;
			/* transmit null path name for end of data */
		if (wctxpn("")==ERROR)
			goto fubar;
	}
		/* send file, not batch mode */
	else
	{
		for (; --argc>=0;)
		{
				/* open file to transmit. open will compute file size */
			if (opentx(1,NULL,*argp++)==ERROR)
				goto fubar;
				/* Ward Christensen transmit */
			if (wctx()==ERROR)
				goto fubar;
		}
	}
	return OK;

	/* error, close files and send CAN to other end */
fubar:
	closetx(TRUE);
	canit();
	return ERROR;
} /* wcsend */


/****************************************************************************
FUNCTION:
	Ward Christensen modem 7 protocol file transmission handler, batch
	mode.

CALLING PARAMETERS:
	f_buf:
		pointer to a structure containing information about file
	*pathname:
		pointer to directory path name
===========================================================================*/
wcs(f_buf,pathname)
struct find_buf *f_buf;
char *pathname;
{

		/* open file, open uses f_buf for file size */
	if (opentx(2,f_buf,pathname)==ERROR)
		return OK;		/* skip over inaccessible files */
	if (wctxpn(f_buf->pname)== ERROR)
		return ERROR;
	if (wctx()==ERROR)
		return ERROR;
	printf("\n");
	return OK;
} /* wcs */


/****************************************************************************
FUNCTION:
	Ward Christensen modem 7 protocol file reception handler

CALLING PARAMETERS:
	argc:
		count of command line arguments.
	argp:
		pointer to string of command line arguments.
===========================================================================*/
wcreceive(argc, argp)
char **argp;
{
	int baslen;

#ifdef IBMPC
		/* enbable raw mode */
	enblcm(FALSE);
#endif

	if (Batch)
	{
		printf("Receiving in Batch Mode\n");
		for (;;)
		{
				/* create the basename of pathname if one was specified */
			if (argc > 0)
			{
				strcpy(Utility.ubuf,*argp);
				baslen=strlen(Utility.ubuf);
			}
			else
				baslen = 0;

				/* get file name for batch mode */
			if (wcrxpn(&Utility.ubuf[baslen])== ERROR)
				goto fubar;
			if (Utility.ubuf[baslen]==0)
				return OK;

#ifdef DEFBYTL
			procheader(Utility.ubuf);
#endif
				/* receive file */
			if (wcrx(Utility.ubuf)==ERROR)
				goto fubar;
		}
	}
	else
	for (; --argc>=0;)
	{
#ifdef DEFBYTL
		procheader(0);
#endif
#ifdef XMODEM
		printf("Receive:'%s' FILE OPEN\n", *argp);
#endif
		if (wcrx(*argp++)==ERROR)
		goto fubar;
	}
	return OK;
fubar:
	canit();
	closerx(TRUE);
	return ERROR;
} /* wcreceive */


/****************************************************************************
FUNCTION:
	Fetch a pathname from the other end as a C ctyle ASCII string.
	Length is indeterminate as long as less than blklen
	a null string represents no more files

CALLING PARAMETERS:
===========================================================================*/
wcrxpn(rpn)
char *rpn;	/* receive a pathname */
{
	purgeline();
	firstsec=TRUE;
#ifdef STATLINE
	lpstat("Fetching pathname");
#else
	printf("\nFetching pathname ");
#endif
		/* slight pause to allow slow systems to get ready */
	sleep(5);
	totsecs = -1;
	if (wcgetsec(rpn, 100, Crcflg?WANTCRC:NAK) != 0)
		return ERROR;
	sendbyte(ACK);
		/* clear line of status message above */
#ifndef STATLINE
	printf("\r                   \r");
#endif
	return OK;
} /* wcrxpn */


/****************************************************************************
FUNCTION:
	transmit path name for batch mode transmission.

CALLING PARAMETERS:
	name:
		pointer to file name to transmit.
===========================================================================*/
wctxpn(name)
char *name;
{
	char *p;

		/*??????*/
	totsecs = -1;

#ifdef STATLINE
	pstat("Awaiting pathname NAK");
#else
	printf("Awaiting pathname NAK ");
#endif
	if ((firstch=readbyte(400))==TIMEOUT)
		return ERROR;
	if (firstch==WANTCRC)
		Crcflg=TRUE;

		/* don't send drive specification */
	if (p=index(':', name))
		name = ++p;

		/* sector number 0 for pathname */
	if (wcputsec(name, 0, SECSIZ)==ERROR)
	{
		printf("Can't send pathname %s\n", name);
		return ERROR;
	}
#ifndef STATLINE
	printf("\r                               \r");
#endif
	return OK;
} /* wctxpn */


/****************************************************************************
FUNCTION:
	Adapted from CMODEM13.C, written by	Jack M. Wierda and Roderick W. Hart

CALLING PARAMETERS:
===========================================================================*/
wcrx(name)
char *name;
{
	int sendchar, sectnum, sectcurr;
#ifdef DEFBYTL
	int cblklen;			/* bytes to dump this block */
#endif

	if (openrx(name)==ERROR)
		return ERROR;
	firstsec=TRUE;
	totsecs=sectnum=0;
	sendchar=Crcflg?WANTCRC:NAK;

	for (;;)
	{
#ifdef STATLINE
		if (!Quiet)
			pstat("block %3d %2dk", totsecs, totsecs/8 );
#else
		if (!Quiet && !View)
			printf("\rblock %3d %2dk ", totsecs, totsecs/8 );
#endif
		purgeline();
		sectcurr=wcgetsec(Utility.ubuf, (sectnum&0177)?70:130,
		  sendchar);
		if (sectcurr==(sectnum+1 &0xff))
		{
			sectnum++;
/*
 * if the compiler supports longs && the o/s blocks the
 *  exact length of files then and only then use the file length
 *  info (if transmitted).
 */
#ifdef DEFBYTL
#ifdef BYTEFLENGTH
			wcj = cblklen = Bytesleft>blklen ? blklen:Bytesleft;
#else
			wcj = cblklen = blklen;
#endif
#else
			wcj = blklen;
#endif
			for (cp=Utility.ubuf; --wcj>=0; )
				if (fputc(*cp++, fout)==ERROR)
				{
					printf("\nDisk Full\n");
					return ERROR;
				}
#ifndef XMODEM
			if (View)
			{
#ifdef DEFBYTL
				wcj = cblklen;
#else
				wcj = blklen;
#endif
				for (cp=Utility.ubuf;--wcj>=0;)
					putchar(*cp++);
			}
#endif
#ifdef DEFBYTL
			if ((Bytesleft -= cblklen) < 0)
				Bytesleft = 0;
#endif
			totsecs += blklen/128;
			sendchar=ACK;
		}
		else if (sectcurr==sectnum)
		{
			wcperr("received dup block\n");
			sendchar=ACK;
		}
		else if (sectcurr==WCEOT)
		{
			sendbyte(ACK);
			/* don't pad the file any more than it already is */
			closerx(FALSE);
			return OK;
		}
		else if (sectcurr==ERROR)
			return ERROR;
		else
		{
			printf(" Sync Error: got %d\n", sectcurr);
			return ERROR;
		}
	}
} /* wcrx */


/****************************************************************************
FUNCTION:
	wcgetsec fetches a Ward Christensen type sector.
	Returns sector number encountered or ERROR if valid sector not received,
	or CAN CAN received	or WCEOT if eot sector.
	time is timeout for first char, set to 4 seconds thereafter
	*************** NO ACK IS SENT IF SECTOR IS RECEIVED OK **************
	   (Caller must do that when he is good and ready to get next sector)

CALLING PARAMETERS:
	rxbuf:
		pointer to buffer to place received data from modem
	time:
	sendchar:
===========================================================================*/
wcgetsec(rxbuf, time, sendchar)
char *rxbuf;
int time;
{
	int sectcurr;

	for (Lastrx=errors=0; errors<RETRYMAX; ++errors, ++toterrs)
	{
			/* stop on user entered ^C */
		if(CIREADY && getcty()==CAN)
			return ERROR;

		sendbyte(sendchar);	/* send it now, we're ready! */
			/* ingnore echo */
		purgeline();

			/* special char for larger block size */
		if ((firstch=readbyte(time))==STX)
		{
			blklen=KSIZE;
			goto get2;
		}
			/* normal wc start char */
		if (firstch==SOH)
		{
			blklen=SECSIZ;
get2:
				/* verify sector number sent properly */
			sectcurr=readbyte(50);
			if ((sectcurr+readbyte(50))==0xff)
			{
					/* rx block, compute checksum */
				oldcrc=checksum=0;
				for (cp=rxbuf,wcj=blklen; --wcj>=0; )
				{
					if ((firstch=readbyte(50)) < 0)
						goto bilge;
					oldcrc=updcrc(firstch, oldcrc);
					checksum += (*cp++ = firstch);
				}
					/* get first byte of check sum */
				if ((firstch=readbyte(50)) < 0)
					goto bilge;

					/* calc crc if crc mode requested */
				if (Crcflg)
				{
					oldcrc=updcrc(firstch, oldcrc);
					if ((firstch=readbyte(50)) < 0)
						goto bilge;
					oldcrc=updcrc(firstch, oldcrc);
					if (oldcrc)
					{
						wcperr(NULL);
						printf("Bad CRC=0%o\n", oldcrc);
					}
					else
					{
						firstsec=FALSE;
							/* normal return (crc mode) with current block # */
						return sectcurr;
					}
				}
					/* not-crc. verify calcualted checksum to actual */
				else if (((checksum-firstch)&0xff)==0)
				{
					firstsec=FALSE;
						/* normal return with current block # */
					return sectcurr;
				}
				else
				{
					wcperr(NULL);
					printf("Checksum Bad rx=0%o cx=0%o\n",firstch, checksum);
				}
			}
			else
				wcperr("block number garbled\n");
		}
			/* make sure eot really is eot and not just mixmash */
		else if (firstch==EOT && readbyte(2)==TIMEOUT)
			return WCEOT;

			/* CANcel */
		else if (firstch==CAN)
		{
			if (Lastrx==CAN)
			{
				wcperr("Sender CANcelled\n");
				return ERROR;
			}
			else
			{
				Lastrx=CAN;
				continue;
			}
		}
		else if (firstch==TIMEOUT)
		{
bilge:
			wcperr("Timeout\n");
		}
			/* display modem status register if supported in readbyte */
		else if (firstch==ERROR)
		{
			wcperr(NULL);
			printf("Modem SR=0%o\n", Mstatus);
		}
		else
		{
			wcperr(NULL);
			printf("Got 0%o block header\n", firstch);
		}
		Lastrx=0;
		while (readbyte(2)!=TIMEOUT)
			;
		if (firstsec)
			sendchar = Crcflg?WANTCRC:NAK;
		else
		{
			time=40;
			sendchar = NAK;
		}
	}
	/* try to stop the bubble machine. */
	canit(); return ERROR;
} /* wcgetsec */


/****************************************************************************
FUNCTION:
	print error message to console for block transmission or reception
	error.

CALLING PARAMETERS:
	*s:
		pointers to string to display after error message
===========================================================================*/
wcperr(s)
char *s;
{
		/* block number is already displayed in normal mode */
	if (View)
		printf("block %3d ", totsecs);
	printf("errors %d: ", toterrs);
	printf(s);
} /* wcperr */


/****************************************************************************
FUNCTION:
	Ward Christensen modem 7 protocol file tranmission handler, pt 2

CALLING PARAMETERS:
	none.
===========================================================================*/
wctx()
{
	int sectnum;

	firstsec=TRUE;
	totsecs=0;
#ifdef STATLINE
	pstat("Awaiting initial NAK");
#else
	printf("Awaiting NAK");
#endif

		/* get the first char, it must be legal */
	while ( ((firstch=readbyte(400))!=TIMEOUT) && (firstch!=WANTCRC)
	  && (firstch!=NAK) && (firstch!=CAN) )
		printf("%c", firstch);	/* let user see it if strange char */

		/* CAN = abort */
	if (firstch==CAN)
		return ERROR;

		/* file transmission in CRC mode */
	if (firstch==WANTCRC)
		Crcflg=TRUE;

		/* NAK or WANTCRC rx */
	sectnum=1;
		/* fill buffer from input file */
	while (filbuf(Utility.ubuf, blklen))
	{
		totsecs += (blklen/128);
		if (!Quiet
#ifdef STATLINE
		)
			pstat("block %3d %2dk", totsecs, totsecs/8 );
#else
		&& !View)
			printf("\rblock %3d %2dk ", totsecs, totsecs/8 );
#endif
			/* send buffered block to modem */
		if (wcputsec(Utility.ubuf, sectnum, blklen)==ERROR)
		{
			return ERROR;
		}
			/* successful transfer */
		else
		{
				/* View can't be set in xyam, otherwise display transfer */
			if (View)
				for (cp=Utility.ubuf,wcj=blklen;--wcj>=0;)
					putchar(*cp++);
				/* next sector(block) */
			sectnum++;
		}
	}
		/* finished, close files */
	closetx(FALSE);

	for (errors=0; ++errors<RETRYMAX; )
	{
		sendbyte(EOT);
		purgeline();
		if(readbyte(100) == ACK)
			return OK;
	}
	wcperr("No ACK on EOT\n");
	return ERROR;
} /* wctx */


/****************************************************************************
FUNCTION:
	wcputsec outputs a Ward Christensen type sector.
	Returns sector number encountered or ERROR if valid sector not received,
	or CAN CAN received	or WCEOT if eot sector.
	time is timeout for first char, set to 4 seconds thereafter
	*************** NO ACK IS SENT IF SECTOR IS RECEIVED OK **************
	   (Caller must do that when he is good and ready to get next sector)

CALLING PARAMETERS:
	txbuf:
		pointer to buffer to send to modem
	sectnum:
		number of block to send per WC Xmodem protocol
	reclen:
		length of block.  Must = 128 to be xmodem compatible
===========================================================================*/
wcputsec(txbuf, sectnum, reclen)
char *txbuf;
int sectnum;
int reclen;	/* data length of this sector to send */

{
	firstch=0;	/* part of logic to detect CAN CAN */

	for (errors=0; errors <= RETRYMAX; ++errors, ++toterrs)
	{
		Lastrx = firstch;
			/* modified so if block length is <> 128 (default for modem7) STX
			   will be transmitted */
		sendbyte(reclen==KSIZE?STX:SOH);
		sendbyte(sectnum);
		sendbyte(-sectnum-1);
		oldcrc=checksum=0;

			/* send block */
		for (wcj=reclen,cp=txbuf; --wcj>=0; )
		{
			sendbyte(*cp);
			oldcrc=updcrc(*cp, oldcrc);
			checksum += *cp++;
		}
			/* crc mode */
		if (Crcflg)
		{
			oldcrc=updcrc(0,updcrc(0,oldcrc));
			sendbyte(oldcrc>>8);
			sendbyte(oldcrc);
		}
			/* checksum only transfer */
		else
			sendbyte(checksum);

			/* allow abort on ^c */
		if(CIREADY && getcty()==CAN)
			goto cancan;
		purgeline();

		/* ignore line noise, esp. braces from 212's */
		switch(firstch=readbyte(100))
		{
		case CAN:
			/* CAN CAN - cancel */
			if(Lastrx==CAN)
			{
cancan:
				printf("\nReceiver CANcelled\n");
				return ERROR;
			}
			break;
		case ACK|0200: 
		case ACK: 
			firstsec=FALSE;
			return OK;
		case TIMEOUT:
			wcperr("Timeout on block ACK\n");
			break;
		case WANTCRC:
			if (firstsec)
				Crcflg=TRUE;
		case NAK:
			wcperr("NAK on block\n");
			break;
		default:
			wcperr(NULL);
			printf("Got 0%o for block ACK\n", firstch);
			break;
		}
		for (;;)
		{
			Lastrx=firstch;
			if ((firstch=readbyte(3))==TIMEOUT)
				break;
			if (firstch==CAN && Lastrx==CAN)
				goto cancan;
			/* let user see it if strange char */
			printf("%c", firstch);
		}
	}
	wcperr("No ACK on block; Abort\n");
	return ERROR;
} /* wcputsec */


/****************************************************************************
FUNCTION:
	send 10 CAN's to try to get the other end to shut up

CALLING PARAMETERS:
===========================================================================*/
canit()
{
	for (wcj=10; --wcj>=0; )
		sendbyte(CAN);
} /* canit */


#ifdef DEFBYTL
/****************************************************************************
FUNCTION:
	process incoming header for additional file info

CALLING PARAMETERS:
===========================================================================*/
procheader(name)
char *name;
{
	char *p;

		/* set default parameters */
	Bytesleft = DEFBYTL;
	Filemode = 0666;
	Modtime = 0L;

	if (name)
	{
		p = name + 1 + strlen(name);
			/* file coming from Unix type system */
		if (*p)
		{
				/* place additional info into file name string */
			sscanf(p, "%ld%lo%o", &Bytesleft, &Modtime, &Filemode);
			printf("%s ", p);
		}
	}
	return OK;
} /* procheader */
#endif
