/*
$title ('yam3.c: terminal mode utilities')
$date (9 nov 85)
*/

#include "yam.h"
#define BRKKEY STX

int overrun;	/* true if data is lost */

/****************************************************************************
FUNCTION:
	two-way conversation with remote -whatever.
	Printer is buffered such that it needn't be as fast as the baudrate
	as long as it eventually gets a chance to catch up.
	This buffering also allows one to write the received data out to disk
	after the conservation has started.

CALLING PARAMETERS:
	none.
===========================================================================*/
term()
{
	register c, cc;
	unsigned charsent;

	sayterm();
	charsent = 0;
		/* reset tab stop */
	Ttycol = 0;
		/* data flow flags */
	Waitecho=Txwait=overrun=FALSE;

#ifdef IBMPC
		/* enbable terminal mode */
	enblcm(TRUE);
#endif

	while(1)
	{

/* IBMPC implementation uses assembly language interrupt routine for modem
   input.  Because the IBM terminal I/O is so painfully slow, interrupts were
   added to avoid loosing characters and allow operation at >300 baud.*/
#ifndef IBMPC
			/**** modem in ****/
		if (MIREADY)
		{
			*bufcq = MICHAR;
			if (!Ignrx)
				bufcq++;
			else
				if(!index(*bufcq, rxnono))
					bufcq++;
			Timeout=0;
			if(bufcq >= bufend)
			{
				Wrapped=TRUE;
				bufcq=bufst;
			}

			if(--Nfree == LOWWATER)
				MOCHAR(Xoffflg=TRUE);
			else if(Nfree==0)
				overrun = TRUE;
			continue;
		}
#endif
			/**** keyboard ****/
		if(CIREADY)
		{
			cfast = CICHAR;
			if (doykbd(cfast))
				return;
			continue;
		}

			/**** console out ****/
		if(bufcdq != bufcq)
		if (COREADY)
		{
			switch(cfast = (*bufcdq & 0x7F) )
			{
			case '\b':
				 if(Ttycol)
					--Ttycol;
				break;
			case '\t':
				CONOUT(' ');
				if (++Ttycol % tabstop)
					continue;
				else
					goto chuckit;
			case '\r':
				Ttycol=0;
				break;
			case '\n':
					/* continue transmit if enabled */
				if(Txeoln==EOL_CRWAIT)
					Waitecho=FALSE;
				Ttycol=0;
				break;
#ifdef XMODEM
			case CPMEOF:
				pstat("");
				return;
				/* bell */
			case 007:
				break;
#endif
			default:
				if (cfast==xoff)
				{
#ifdef STATLINE
					sayterm();
#endif
					Txgo=FALSE;
					goto chuckit;
				}
				else if (cfast==xon)
				{
					Txgo=TRUE;
#ifdef STATLINE
					sayterm();
#endif
					goto chuckit;
				}

				else if(Ctlview)
				{
						/* preceed with ~ if parity bit set */
					if (cfast & 0x80)
					{
						CONOUT('~');
						++Ttycol;
					}
						/* view control characters */
					if (cfast < ' ')
					{
						CONOUT('^');
						CONOUT(cfast|0x40);
						Ttycol += 2;
						goto chuckit;
					}
				} /* ctlview */
				else
					++Ttycol;
			} /* switch */

				/* resume file sending if waiting for go char */
			if (cfast==GOchar && Txeoln==EOL_CRPROMPT)
				Waitecho=FALSE;

			CONOUT(cfast);

			if(Echo)
			{
				sendbyte(cfast);
				if(Chat && cfast== '\r')
				{
					CONOUT('\n');
#ifndef XMODEM
					sendbyte('\n');
#endif
				}
			} /* echo */
chuckit:
				/* if not receiving, buffer need not
				   be dumped.  bump up number of free space left */
				/* Some systems echo data sent(as in editor). 
				   If receiving an echo term would emit dc3
				   when the buffer fills up. This will prevent that too */
			if (!Rfile)
				Nfree++;

			if(++bufcdq >= bufend)
				bufcdq=bufst;
			continue;
		}

			/**** printer ****/
		if (Pflag && (bufpcdq!=bufcq) && LPREADY)
		{
			LPOUT(*bufpcdq++ & 0177);
			if (bufpcdq >= bufend)
				bufpcdq=bufst;
		}

			/**** modem out ****/
		if(MOREADY)
		{
				/* if transmit file is open(Tfile) and we are not waiting
				   for throttle(Txwait) and not waiting for an echo(!Waitecho)
				   and transmission is not suspended by xoff(Txgo) send char. */
			if(Tfile && !Txwait && !Waitecho && Txgo)
			{
					/* get char from file */
				c = getc(fin);
				if(c==EOF)
				{
					closetx(FALSE);
#ifdef XMODEM
					pstat("");
					return;
#else
						/* return to command mode on EOF if true */
					if(Exoneof)
						return;

						/* explain were in term mode now */
					sayterm();
#endif
				}

					/* wait for echoes to stop after waitnum characters.
					   Waitbunch set by 'et', Waitnum by 'pwn' */
				if(Waitbunch && ++charsent==Waitnum)
				{
					charsent=0;
						/* cause wait for echo */
					if(Waitnum>1)
					{
						Waitecho=TRUE;
						Timeout=0;
					}
						/* reset throttle wait parms */
					else
					{
						Txwait=TRUE;
						Txtimeout=Throttle;
					}
				}
					/* end of line processing */
				if(c=='\r')
				{
					switch(Txeoln)
					{
					case EOL_NL:
						continue;
					case EOL_CRPROMPT:
					case EOL_CRWAIT:
						Waitecho=TRUE;
						Timeout=0;
					case EOL_CR:
						if(Tfile)
						{
							if((cc=getc(fin))!='\n')
								ungetc(cc, fin);
						}
						break;
					}
				}
				MOCHAR(c);
				continue;
			}
		}

			/* time to do something */
		if(Timeout == Tpause)
		{

			Waitecho=FALSE;

				/* dump buffer if full */
			if(Xoffflg)
			{
				dumprxbuff();
					/* allow term output to catch up */
				if(bufcdq != bufcq)
					continue;
					/* allow printer output to catch up */
				if(Pflag && bufpcdq != bufcq)
					continue;
				Xoffflg=FALSE;
				MOCHAR(xon);
			}
#ifdef CDO
			if(CDO)
			{
				pstat("Carrier Lost");
				return;
			}
#endif
		}
		else
			++Timeout;

			/* buffer overflow */
		if (overrun)
		{
			lpstat("\007OVERRUN: DATA LOST");
			overrun = 0;
			return;
		}

		if(--Txtimeout==0)
			Txwait=FALSE;
	}

} /* term */


/****************************************************************************
FUNCTION:
	display the status information on entering term()

CALLING PARAMETERS:
	none
===========================================================================*/
sayterm()
{
	if(Tfile)
		pstat("Sending '%s' %s", Tname, Txgo?"":"Stopped by XOFF");
	else if(Rfile)
		pstat("Term Receiving '%s'", Rname);
	else
		pstat("Term Function");
} /* sayterm */


/****************************************************************************
FUNCTION:
	open a capture file and set the removal pointer to get max goods
	called from main menu with t [filename].

CALLING PARAMETERS:
	char *name:
		pointer to file name to capture text to.
===========================================================================*/
opencapt(name)
char *name;
{
		/* close any existing file */
	dumprxbuff();
	closerx(TRUE);
	if(openrx(name)==ERROR)
		return ERROR;
	if(buffcdq<bufst)
		buffcdq=bufst;
	if(Wrapped)
		buffcdq=bufcq+1;
	if(buffcdq >= bufend)
		buffcdq=bufst;
	return OK;
} /* opencapt */


/****************************************************************************
FUNCTION:
	dump circular buffer to disk

CALLING PARAMETERS:
	none
===========================================================================*/
dumprxbuff()
{
	register c;

	Nfree=Bufsize;
	if(!Rfile || buffcdq==NULL)
		return OK;
	while(buffcdq != bufcq)
	{
		c= *buffcdq++;
		if(buffcdq >= bufend)
			buffcdq=bufst;
		switch(c &= 0177)
		{
		case 0:
			 continue;
		case 0x1A:		/* ^Z */
			if(Zeof)
			{
				closerx(TRUE);
				return OK;
			}
			else
				break;
		case 0x12:
			if(Squelch)
			{
				Dumping=TRUE;
				continue;
			}
			break;
		case 0x14:
			if(Squelch)
			{
				Dumping=FALSE;
				continue;
			}
			break;
		default:
			break;
		}
		if(Dumping)
			if(putc(c, fout)==ERROR)
			{
				printf("\nDisk Full\n");
				closerx(FALSE);
				return ERROR;
			}
	}
	return OK;
} /* dumprxbuff */


/****************************************************************************
FUNCTION:
	set buffer pointers back to start of data in buffer, or start of buffer
	if the pointers have wrapped.  Entering term after a rewind will cause
	re-display of all captured data in buffer.

CALLING PARAMETERS:
	none
===========================================================================*/
rewindcb()
{
	bufcdq=buffcdq=bufpcdq=Wrapped?bufcq+1:bufst;
}

/****************************************************************************
FUNCTION:
	replot redisplays the buffer contents thru putcty allowing xoff
	number will represent how many lines to go back first 

CALLING PARAMETERS:
	number:
		number of characters to re-display
===========================================================================*/
replot(number)
int number;
{
	char doexit;
	char *reptr;
	char c;
	char *smark;
	char *p;
	int	count;
	int n;

	doexit=FALSE;

fromtop:
	smark=reptr=bufmark=Wrapped?bufcq+1:bufst;

	if(number)
	{
		reptr=bufcq;

	/* backup up number lines.  They will be displayed by nextline */
backsome:
		/* backing up is confusing unless screen is cleared first */
	for (n=1; n<TLENGTH; n++)
		CONOUT('\n');

		for(;;)
		{
			--reptr;

			/* adjust for pointer wrap */
			if(reptr<bufst)
			{
				if(Wrapped)
					reptr = bufend-1;
				else
				{
					reptr = bufst;
					break;
				}
			}
			/* hit begining of buffer */
			if(reptr==bufcq)
				break;
			/* test for line count */
			if((*reptr&0177)=='\n' && --number<=0)
				break;
		}
		bufmark=reptr;
	}

nextscreen:
	count = TLENGTH - number;

	/* display count lines */
nextline:
	while(reptr != bufcq)
	{
		cfast= *reptr++ & 0177;
		if(reptr >= bufend)
			reptr=bufst;
		if(c=putcty(cfast))
			goto choose;
		if (cfast=='\n' && --count<=0)
			break;
	}

	if (doexit)
	{
		sayterm();
		return;
	}
#ifdef STATLINE
	pstat("more ?");
#else
	printf("\rmore ?");
#endif
	c=getcty();
#ifdef STATLINE
	pstat("");
#else
	printf("\r       \r");
#endif

choose:
/* Treat Control chars and letters the same */
	switch((c|0x60)&0x7f)
	{
	case 'x':
		doexit = TRUE;
		number = TLENGTH;
		reptr = bufcq;
		goto backsome;
	case 'o':	/* /|0x60=o  search command */
		putcty('/');
		smark = bufmark; /* search from start of screen */
		gets(Utility.ubuf);
		uncaps(Utility.ubuf);
		/* FALL THROUGH TO */
	case 'n': /* find next */
		if ((p = cisubstr(smark, Utility.ubuf)) && p<bufend)
		{
			smark = 1 + (bufmark = reptr = p);	/* +1 so n can work */
			number = 1;
			goto backsome;
		} else
			putcty(007);
		/* FALL THROUGH TO */
	case 'b': /* beginning of buffer */
		number = 0;
		goto fromtop;
	case 'r':	/* control-r or r = previous page */
	case 'h':	/* backspace */
		number = TLENGTH-3;
		reptr = bufmark;
		goto backsome;
	case 0x60:	/* space bar = next page */
		number = 3;
		bufmark = reptr;
		goto nextscreen;
	case 'p': /* previous line */
		number = 1;
		reptr = bufmark;
		goto backsome;
	case 'j':	/* linefeed */
		bufmark = reptr;
		count = 1;
		goto nextline;
	default:
		return;
	}
} /* replot */


/****************************************************************************
FUNCTION:
	moment waits a moment and returns FALSE, if no character keyboarded in
	the meantime, otherwise returns TRUE.

CALLING PARAMETERS:
	none
===========================================================================*/
moment()
{
	sleep(3);
	if (CIREADY)
	{
		getcty();
		return TRUE;
	}
	return FALSE;
} /* moment */



/****************************************************************************
FUNCTION:
	chat function.  Allow person to talk to operator at other end

CALLING PARAMETERS:
	none
===========================================================================*/
dochat()
{
#ifdef XMODEM
	printf("Exit chat with ^Z\n");
#endif
	Chat=Ctlview=Hdx=Echo=TRUE;
	term();
} /* dochat */



/****************************************************************************
FUNCTION:
	Handle keyboard input. Returns TRUE iff caller should return to
	command mode.

CALLING PARAMETERS:
	cf:
		char from keyboard.  for IBM PC, keycode is in high byte, ascii
		char is in low byte.
===========================================================================*/
doykbd(cf)
int cf;
{
	switch (cf & 0x7F)
	{
		case ENQ:
			if (moment())	/* ^E exits term */
				break;
			pstat(" ");
			return TRUE;
		case 0x12:			/* ^r */
			if (moment())
				break;
			replot(TLENGTH-1);
			return;
		case BRKKEY:		/* BRK KEY sends break */
			if (moment())
				break;
			sendbrk();
			while (CIREADY)
				getcty();	/* discard garbage */
			return FALSE;
		case '\r':
			if(Hdx)
				printf("\n");
			if(Chat)
				sendbyte('\n');
			break;
		default:
			if (cf==xon)
			{
				if(Tfile && !Txgo)
				{
					Txgo=TRUE;
					sayterm();
				}
			}
			else if (cf==xoff)
			{
				if(Tfile && Txgo)
				{
					Txgo=FALSE;
					sayterm();
				}
			}
	}

#ifdef IBMPC
		/* extended key codes for IBM PC. Low order byte is 0 */
	if (!(cf & 0xFF))
		switch (cf)
		{
				/* del key */
			case 0x5300:
				cf = 0x7F;
				break;
			default:
				return FALSE;
		}
#endif

		/* send char to modem if not special key */
	MOCHAR(cf & 0x7f);
	if(Hdx)
		CONOUT(cf);
	return FALSE;
}/* doykbd */
