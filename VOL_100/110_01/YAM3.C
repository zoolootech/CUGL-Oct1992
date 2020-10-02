/*
>>:yam3.c 9-6-81
 *
 * two-way conversation with remote -whatever.
 * Printer is buffered such that it needn't be as fast as the baudrate
 * as long as it eventually gets a chance to catch up.
 * This buffering also allows one to write the received data out to disk
 * after the conservation has started.
 */
#include "yam.h"

term()
{
#ifndef BDSC
	register char cfast;
#endif
	register c, cc;
	unsigned register charsent;

sayterm:
#ifdef TERMRESET
	lprintf(TERMRESET);
#endif
	charsent=0;
	Waitecho=Txwait=FALSE;
	if(Tfile)
		lpstat("Sending '%s' %s", Tname, Txgo?"":"Stopped");
	if(Rfile)
		pstat("Term Receiving '%s'", Rname);
	else
		pstat("Term Function");
	for(;;) {
		if(MIREADY) {
			 *bufcq++ = MICHAR;
			Timeout=0;
			if(bufcq >= bufend) {
				Wrapped=TRUE;
				bufcq=bufst;
			}
			if(--Free == Low)
				outp(MODATA, Xoffflg=XOFF);
			continue;
		}
		if(COREADY && bufcdq != bufcq) {
			switch(cfast= (*bufcdq & 0177) ) {
			case '\b':
				if(Ttycol)
					--Ttycol;
				break;
			case '\t':
				if(++Ttycol & 07)
#ifdef CDATA
					outp(CDATA, ' ');
#else
					bios(4,' ');
#endif
				else if(++bufcdq >= bufend)
					bufcdq=bufst;
				continue;
			case '\r':
				Ttycol=0; break;
			case '\n':
				if(Txeoln==EOL_CRWAIT)
					Waitecho=FALSE;
				break;
cxoff:
			case XOFF:
#ifdef STATLINE
				if(Tfile)
					pstat("Sending stopped by XOFF");
#endif
				Txgo=FALSE; break;
cxon:
			case XON:
				Txgo=TRUE;
#ifdef STATLINE
				if(Tfile)
					goto sayterm;
#endif
				break;
#ifdef XMODEM
			case CPMEOF:
				return;
#endif
			default:
				if(
#ifndef XMODEM
				  Ctlview &&
#endif
				  cfast<040) {
					putchar('^'); putchar(cfast|0100);
					Ttycol += 2;
					if(++bufcdq >= bufend)
						bufcdq=bufst;
					continue;
				}
				else
					++Ttycol; break;
			}
#ifdef CDATA
			outp(CDATA, cfast);
#else
			bios(4, cfast);
#endif
			if(Echo)
				sendline(cfast);
			if(++bufcdq >= bufend)
				bufcdq=bufst;
			continue;
		}
		if(Pflag && bufpcdq!=bufcq && POREADY) {
#ifdef CPM
			bios(5, *bufpcdq++);
#endif
			if(bufpcdq >= bufend)
				bufpcdq=bufst;
		}
		if(CIREADY) {
			switch(cfast= CICHAR&0177){
			case ENQ:
				if(moment())	/* ^E ^E quick sends one */
					break;
				return;
			case XON:
				if(Tfile && !Txgo)
					goto cxon;
				break;
			case XOFF:
				if(Tfile && Txgo)
					goto cxoff;
				break;
			case 'v'&037:
				if(moment())
					break;
				replot(TLENGTH<<1); goto sayterm;
			default:
				break;
			}
			outp(MODATA, cfast);
			if(Hdx) {
#ifdef CDATA
				while(!COREADY)
					;
				outp(CDATA, cfast);
#else
				bios(4, cfast);
#endif
			}
			continue;
		}
		if(MOREADY) {
			if(Tfile && !Txwait && !Waitecho && Txgo) {
				c= getc(fin);
#ifdef CPM
				if(c==EOF || (c==CPMEOF && Txeoln != TX_BINARY))
#else
				if(c==EOF)
	/* Compiler please notice the following....*/
	/* Compiler please notice the following....*/
	/* Compiler please notice the following....*/
#endif
				{
					closetx();
#ifdef XMODEM
					return;
#else
					if(Exoneof)
						return;
					else
						goto sayterm;
#endif
				}
				if(Waitbunch && ++charsent==Waitnum) {
					charsent=0;
					if(Waitnum>1) {
						Waitecho=TRUE; Timeout=0;
					} else {
						Txwait=TRUE; Txtimeout=Throttle;
					}
				}

				if(c=='\r') {	/* end of line processing */
					switch(Txeoln) {
					case EOL_NL: continue;
					case EOL_CRPROMPT:
					case EOL_CRWAIT:
						Waitecho=TRUE; Timeout=0;
					case EOL_CR:
						if((cc=getc(fin))!='\n')
							ungetc(cc, fin);
						break;
					}
				}
				outp(MODATA, c);
				continue;
			}
		}
		if(++Timeout == Tpause) {
			Waitecho=FALSE;
			if(Xoffflg) {
				dumprxbuff();
				if(bufcdq != bufcq)
					continue;
				if(Pflag && bufpcdq != bufcq)
					continue;
				Free=Bufsize;
				Xoffflg=FALSE;
				outp(MODATA, XON);
			}
#ifdef CDO
/* this seems to be the magic cookie */

			if(CDO) {
				lprintf("Carrier Lost\n");
				return;
			}

/* Compiler, Please notice the following!!! */
/* Compiler, Please notice the following!!! */
/* Compiler, Please notice the following!!! */
/* Compiler, Please notice the following!!! */
/* Compiler, Please notice the following!!! */
/* Compiler, Please notice the following!!! */
/* Compiler, Please notice the following!!! */
/* Compiler, Please notice the following!!! */
/* Compiler, Please notice the following!!! */

#endif

/* should be compiling again here! */
		}
		if(--Txtimeout==0)
			Txwait=FALSE;
	}
}
/* open a capture file and set the removal pointer to get max goods */
opencapt(name)
char *name;
{
	dumprxbuff(); closerx(TRUE);
	if(openrx(name)==ERROR)
		return ERROR;
	if(buffcdq<bufst)
		buffcdq=bufst;
	if(Wrapped)
		buffcdq=bufcq+1;
	if(buffcdq >= bufend)
		buffcdq=bufst;
/*	dumprxbuff(); */
}


dumprxbuff()
{
	register c;

	if(!Rfile || buffcdq==NULL)
		return OK;
	while(buffcdq != bufcq) {
		c= *buffcdq++;
		if(buffcdq >= bufend)
			buffcdq=bufst;
		if(!Image) {
			switch(c &= 0177) {
			 case 0:
				 continue;
			case 032:		/* ^Z or CPMEOF */
				if(Zeof) {
					closerx(TRUE);
					return;
				}
				else
#ifdef CPM
					continue;
#else
					break;
#endif
			case 022:
				if(Squelch) {
					Dumping=TRUE;
					continue;
				}
				break;
			case 024:
				if(Squelch) {
					Dumping=FALSE;
					continue;
				}
				break;
			default:
				break;
			}
		}
		if(Dumping || Image)
			if(putc(c, fout)==ERROR) {
				printf("\nDisk Full\n");
				closerx(FALSE);
				return ERROR;
			}
	}
	Free=Bufsize;
	return OK;
}

rewindcb()
{
	bufcdq=buffcdq=bufpcdq=Wrapped?bufcq+1:bufst;
}

/*
 * replot redisplays the buffer contents thru putcty allowing XOFF
 * number will represent how many lines to go back first 
 */
replot(number)
{
	char *s, c;
	int count;

#ifdef Z19
	/* set it to truncate long lines so count is right */
	printf("\033w");
#endif
	bufmark=Wrapped?bufcq+1:bufst;

	if(number) {
		s=bufcq;
backsome:
#ifdef CLEARS
		/* backing up is confusing unless screen is cleard first */
		printf(CLEARS);
#endif
		for(;;) {
			--s;
			if(s<bufst) {
				if(Wrapped)
					s= bufend-1;
				else {
					s=bufst;
					break;
				}
			}
			if(s==bufcq)
				break;
			if((*s&0177)=='\n' && --number<=0)
				break;
		}
		bufmark=s;
	}
	s=bufmark;
nextscreen:
#ifdef T4014
	/* Do the big flash on the 4014 or 4012 */
	printf("\033\014");
	sleep(CLKMHZ * 5);
#endif
	count=TLENGTH-2;
nextline:
	while(s != bufcq) {
		cfast= *s++ & 0177;
		if(s >= bufend)
			s=bufst;
		if(c=putcty(cfast))
			goto choose;
		if(cfast=='\r' && --count<=0)
			break;
	}

	pstat("Replot cmd?");
	c=getcty();
#ifdef STATLINE
	pstat("");
#endif
choose:
/* Treat Control chars and letters the same */
	switch((c|0140)&0177) {
	case 'v':	/* control-v or v */
	case 'h':	/* backspace */
		number=TLENGTH-2; s=bufmark; goto backsome;
	case 0140:		/* space bar */
		bufmark=s; goto nextscreen;
	case 'p':
		number=1; s=bufmark; goto backsome;
	case 'n':
	case 'j':	/* linefeed */
		bufmark=s; count=1; goto nextline;
	default:
		return;
	}
}

/*
 * moment waits a moment and returns FALSE, unless a character is hit,
 * in which case that character is returned
 */
moment()
{
	int c;
	for(c=300*CLKMHZ; --c>0;)
		if(kbhit())
			return(getchar());
	return FALSE;
}
chat()
{
	printf("Ring My Chimes, Maybe I'll Come\n");
	printf("Exit chat with ^Z\n");
	Ctlview=Hdx=Echo=TRUE;
	term();
}
li