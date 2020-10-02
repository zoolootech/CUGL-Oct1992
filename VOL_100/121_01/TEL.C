/*
HEADER: CUG 121.??;

    TITLE:	tel - dumb terminal + file xfer;
    VERSION:	1.0;
    DATE:	01/06/86;
    DESCRIPTION: "Tel is a considerably modified version of Telnet v2.3, as
		distributed with v1.46 of the BDS C compiler.  The following
		features have been added: multiple files may be transmitted or
		received by one command, directories may be listed, and the
		quit command hangs up a hayes smartmodem.  To accommodate
		these changes, some of the command names have been changed.
		This program uses a modem to emulate a dumb terminal.
		Incoming data may be buffered in memory and written to disk, 
		data may be transmitted from disk to the modem, or files may
		be formally transferred in a "checksum" mode with automatic
		handshaking and buffering.";
    KEYWORDS:	terminal, modem, emulate, file transfer;
    SYSTEM:	CP/M;
    FILENAME:	TEL.C;
    WARNINGS:	"Requires local.c and files.c for link.
		The checksum file transfer does not follow the XMODEM
		protocol.
		Variable baud rate support is present, but commented out
		(hardware-specific coding would be required to implement it).
		Tel assumes that the CP/M console is much faster than the
		modem (e.g. a 1200+ baud console with a 300 baud modem).";
    SEE-ALSO:	CALL.C, TELNET.C, TL370.C;
    AUTHORS:	Leor Zolman, Leo Kenen, Cal Thixton, Mike W. Meyer;
    COMPILERS:	BDS-C 1.50;
*/

#include <bdscio.h>	/* System, h'ware constants			*/
#include <hardware.h>

#ifdef MULL	/* muellers version if MULL is defined.			*/

#define TITLE "Mulltel version 1.5	12/3/80 tj"
#define	INITB	3	/* 1200 baud initially				*/
#define	SSPECIAL '\\'	/* escape character initially			*/

#else

#define TITLE "Telnet version 3.0-Intersystems	10/25/80 (mwm)\n"
#define	SSPECIAL 29	/* Initial escape character ^]			*/
#define	INITB	5	/* 4800 baud initially. See setbaud()		*/

#endif

#define	SELDSK	14	/* change logged in disk	*/
#define	DIRFRST	17	/* search for first occurance of a file */
#define	DIRNEXT	18	/* search for next occurance of a file */
#define	CURRDSK	25	/* return currently logged in disk	*/
#define	maxfiles	64	/* maximun # of file to send as once */

/*
	The following #defines need not be changed:
*/

#define	ACK	0x06	/* Ascii ACK for handshaking	*/
#define	NAK	0x15
#define	EOT	0x04	/* End of transmission		*/
#define	ETX	0x03	/* Abort Transmission		*/
#define	STX	0x02	/* Beginning of next filename to send	*/
#define	NUL	0x0	/* end of file name	*/
#define PHA	0xfd	/* for phase checking	*/
#define CR	0x0d	/* carriage return	*/
#define LF	0x0a	/* line feed		*/
#define NL	'\n'	/* god knows, compiler dependent!	*/

/*
	File Control Block structure
*/

struct	FCB	{
	char	entry,		/* entry type. assumed 0	*/
		name[8],	/* file name	*/
		type[3],	/* file type	*/
		extent,		/* which entention is this one?	*/
		na,naa,		/* not used (??)	*/
		count,		/* record count	*/
		map[16],	/* disk allocation map	*/
		next;		/* next record to read	*/
	};

/*
	External variable declarations:
*/

char	SPECIAL;	/* current SPECIAL or escape character		*/
char	rflag;		/* receiving file open flag			*/
char	tflag;		/* transmitting file open flag			*/
char	chflag;		/* checksumming enabled flag			*/
char	cflag;		/* text-collection enabled flag 		*/
char	pflag;		/* pausing flag 				*/
char	spflag;		/* stripping parity bit flag			*/
char	nflag;		/* recognizing nulls flag			*/
char	fflag;		/* true if changing CR-LF's into
				just CR when transmitting		*/
char	lastc;		/* last char transmitted			*/
char	dodflag;	/* true if displaying outging data		*/
char	didflag;	/* true if displaying incoming data		*/
char	hdflag;		/* true if effectively working in half-duplex 	*/
char	hd_sav;		/* save flag when transmitting or receiving 	*/
char	abortf;		/* true when file I/O aborted			*/
char	rbuf[BUFSIZ]; 	/* file I/O buffer for incoming data file 	*/
char	tbuf[SECSIZ]; 	/* sector buffer for file being transmitted	*/
char	tmp[20];	/* tempory holding place for file name		*/
char	rname[20]; 	/* name of receiving file			*/
char	tname[20]; 	/* name of transmitting file			*/
int	rfd, tfd;	/* file descriptors				*/
char	*cptr;		/* pointer to free space in buf 		*/
unsigned free;		/* number of bytes free in buf			*/
int 	bcount;		/* counts bytes in current block when checksumming */
int 	scount;		/* Number of sectors sent/received		*/
int 	checksum;	/* the checksum value itself			*/
char	timoutf;	/* 1 if time-out happens while waiting for modem data*/
char	*i;		/* odd-job char pointer 			*/
int 	dod_sav, did_sav;	/* scratch variables			*/
unsigned bufspace;	/* # of bytes available for collection buffer in ram */
char	toupper();	/* This makes for better code 			*/
char	fcb[34];	/* file control block				*/
struct {			/* so i can't use initializers		*/
	char	rate[5],	/* readable baud rate			*/
		hex;
	} bauds [7];
char	current;		/* current baud rate being used		*/
char	files[maxfiles][17];	/* files to send 			*/
char	leftover;	/* number of file to send 			*/
char	thisdisk;	/* currently logged in disk			*/
char	*buf;		/* text collection pointer; will
				point to the location just after itself */

/*
	place for main.c
*/

main()
{
	char	c, c2;
	int n;

	init();

	while(1) {
		if (abortf) {
			if (rflag) 
				rclose();
			if (tflag) 
				tabort();
			reset();
			abortf = 0;
			}
		if (tflag && xmit()) {
			printf("\nTransmission complete.\n");
			close(tfd);
			reset();
			}
		if (abortf) 
			continue;
		if (miready()) {
			c = c2 = getmod();
			if (spflag)
				c &= 0x7f;
			if (tflag && (c == ETX)) {
				printf("Receiver has aborted;\n");
				abortf = 1;
				continue;
				}
			if (didflag && (c || nflag) && (c != CPMEOF))
				display(c);
			if (cflag && !pflag) {
				if (c || nflag)
					if (!free) {
					     printf("**BUFFER FULL**\007\007");
					     rdump(0);
						}
					else {
						*cptr++ = c;
						free--;
						}
				if (chflag) {
					checksum += c2;
					bcount++;
					if (bcount == SECSIZ) {
						bcount = 0;
						outmod(checksum >> 8);
						outmod(checksum);
						checksum = 0;
						c = getmod();
						if(c == STX) {
							getname(tmp);
							rdump(0);
							rclose();
							scount=0;
							strcpy(rname,tmp);
							if(opento()) {
								outmod(ETX);
								abortf=1;
								}
							outmod(PHA);
							}
						else if (c == EOT) {
							rdump(0);
							rclose();
							reset();
							printf("\n%s received OK\n",rname);
							}
						else if (c == ACK) {
							if (cptr > buf+1000) 
								rdump(0);
							if (!didflag) 
								printf("-%d",++scount);
							outmod(PHA);
							}
						else  {
							cptr -= SECSIZ;
							free += SECSIZ;
							printf("\nChecksum error. Retrying <%d>\n",scount+1);
							outmod(PHA);
							timoutf = 0;
							}
						}
					}
				}
			}
		if (kbready()) {
			c = getch();
			if (c != SPECIAL) {
				if (pflag || (!tflag && !(rflag && chflag))) {
					outmod(c);
					if (dodflag) 
						display(c);
					}
				}
			else special();
			}
	}
}


/*
	place for special.c
*/

/*
	Handle special Telnet command:
*/

special()
{
	char c;
	int nn,n;

	printf("\n?: ");
	if ( (c = getchar()) != NL) printf("...");
	if(c==SPECIAL) {
		outmod(SPECIAL);
		printf("Current Escape 0x%02x sent", SPECIAL) ;
		}
	else switch (toupper(c)) {
		case 'E' :	
			printf("Current Escape Character: 0x%02x\n",SPECIAL);
			printf("Enter New Escape Character: ");
			SPECIAL=getchar();
			putchar(NL);
			break;

		case '7':
			spflag = ask("Strip parity");
			break;

		case 'B':
			dobauds();
			break;

		case 'N':
			nflag = ask("Recognize incoming nulls");
			break;

		case 'F':
			fflag = ask("Transmit CR-LF pairs as CR only");
			break;

		case 'H':
			if (rflag || tflag)  { 
				printf("Must abort transfer first\n");
				break;
				}
			hd_sav = hdflag = ask ("Half Duplex");
			reset();
			break;

		case 'Z':
			puts(CLEARS) ;
			break;

		case 'P':
			if (pflag) 
				printf("Already pausing");
			else if (!(tflag || rflag))
				printf("Not transmitting or receiving");
			else {
				pflag = 1;
				dod_sav = dodflag;
				did_sav = didflag;
				dodflag = !hdflag;
				didflag = 1;
				printf("Pausing from %s", tflag ?
			"transmission" : "collection");
				}
			putchar(NL);
			break;

		case 'R':
			if (!pflag) 
				printf("Not pausing");
			else {
				pflag = 0;
				dodflag = dod_sav;
				didflag = did_sav;
				printf("%s now enabled again.", tflag ?
				"transmission" : "collection");
			}
			putchar(NL);
			break;

		case 'K':
			printf("Text buffer: ZAPPED!");
			free = bufspace;
			cptr = buf;
			putchar(NL);
			break;

		case 'V':
			if (rflag) {
				putchar(NL);
				i = buf;
				while (i < cptr && !kbready())
					putchar(*i++);
				if(i<cptr)
					getchar();
				printf("\n%u bytes free",free);
				}
			else 
				printf("No receiving file open");
			putchar(NL);
			break;


		case 'D':
			if (rflag)
				rdump(1);
			else 
				printf("No output file");
			putchar(NL);
			break;

		case 'C':
			if (rflag) 
				rclose();
			else 
				printf("No output file");
			putchar(NL);
			break;


		case 'Q': hangup() ;	/* hangup the modem */
			
		case 'X':
			printf("Quit\n");
			if (tflag) tabort();
			if (rflag) rclose();
			exit();

		case 'A':
			if (tflag || rflag) {
  				if (chflag) outmod(ETX);
				abortf = 1;
 				break;
				}
			printf("No transfer to abort.\n");
			putchar(NL);
			break;

		case 'O':
			if (rflag) 
				rclose();
			if (tflag) 
				tabort();
			rflag = 1;
			if (!askstuff()) {
				reset();
				return;
				}
			cptr = buf;
			free = bufspace;
			rflag = cflag = 1;
			scount = pflag = checksum = bcount = 0;
			if (chflag) {
				printf("Trying to link...");
				do {
					c = getmod();
					if (abortf) {
						printf("aborting...\n");
						reset();
						return;
						}
					timoutf = 0;
				} while (c & 0x7f);
				printf("linked.\n");
				outmod(0);
				if(getmod() != STX) {
					printf("STX ERROR\n");
					reset();
					return;
					}
				getname(rname);
				if(opento()) {
					outmod(ETX);
					abortf=1;
					}
				outmod(PHA);
				}
			break;

		case 'T':
			printf("Transmit\n");
			if (tflag) 
				tabort();
			if (rflag) 
				rclose();
			printf("\nEnter file names to send:\n");
			leftover=0;
			while(1) {
				++leftover;
				printf("%d) File name: ",leftover);
				gets(files[leftover-1]);
				n=leftover;
				if(nameok())	/* return 1 too many or end */
					break;
				for(nn=n;nn<=leftover;nn++) 
					printf("File %d: %s\n",nn,files[nn-1]);
				}
			printf("%d Files to send\n",--leftover);
			if (!leftover || !askstuff()) {
				reset();
				break; 
				}
			strcpy(tname,files[leftover-1]);
			tflag = 1;
			if(leftover && ((tfd=open(tname,0))==ERROR)) {
				printf("Fatal error: Cannot open %s\n",tname);
				reset();
				abortf=1;
				break;
				}
			scount = pflag = checksum = bcount = 0;
			if (read(tfd,tbuf,1) <=0) {
				printf("Read error from %s\n",tname);
				reset();
				abortf = 1;
				break;
				}
			if (chflag) {
				printf("Trying to link...");
				while (1) {
				  outmod(0);
				  for (n=0; n<5000; n++)
					if (miready()) {
						if(!(getmod()&0x7f)){
							printf("linked.\n");
							outmod(STX);
							sendname(tname);
							printf("\nSending %d: %s\n",leftover,tname);
							if(getmod()!=PHA) {
								printf("ERROR\n");
								abortf=1;
								}
							return;
							}
						}
					else
						if (kbabort()) {
							printf("aborting.\n");
							return;
							}
					}
				}
			break;

		case 'S':
			dostat();
			break;

		case 'L':
			if(rflag || tflag) {
				printf("Cannot list disk while %s data\n",
					(rflag)?"receiving":"transmitting");
				break;
				}
			dir();
			break;

		case 'G':
			if(rflag || tflag) {
				printf("Cannot change disk while %s data\n",
					(rflag)?"receiving":"transmitting");
				break;
				}
			printf("Newly logged-in Disk: %c:\n",togdisk());
			break;

		default:
			prcoms();
	}
}




/*
	Print out legal Telnet commands:
*/

prcoms()
{
	puts(CLEARS) ;
	printf("BDS Telnet commands are:\n");
	printf("?: This listing\n");
	printf("0x%02x: send 0x%02x\n",SPECIAL,SPECIAL);
	printf("7: Select policy regarding Parity bits\n");
	printf("a: Abort transfer of file\n");
	printf("b: Select baud rate\n");
	printf("c: Close input file (after dumping buffer)\n");
	printf("d: Dump (append) text buffer to output file\n");
	printf("e: Change ESCAPE Character\n");
	printf("f: Select whether to transmit CR-LF as just CR\n");
	printf("g: Go (login) to other disk\n");
	printf("h: Set Half/full duplex mode\n");
	printf("k: Kill (erase) contents of input buffer\n");
	printf("l: List files on disks\n");
	printf("n: Accept or ignore Nulls\n");
	printf("o: Open input file, start collection\n");
	printf("p: Pause (suspend collection or transmission)\n");
	printf("q: Dump & close input file (if open) and Quit to CP/M\n");
	printf("r: Resume after pausing\n");
	printf("s: Display Status\n");
	printf("t: Transmit a file\n");
	printf("v: View contents of input buffer\n");
	printf("x: eXit to CP/M, don't hangup the modem\n") ;
	printf("z: Clear screen\n");
}

/*
	Print opening message and initialize program:
*/

init()
{
	printf(TITLE);

	_allocp = NULL;	  /* initialize allocation pointer */

	hdflag = hd_sav = lastc = timoutf = cflag = 0;
	nflag = pflag = abortf = fflag = 0;
	spflag=1;
	SPECIAL=SSPECIAL;	/* initial escape character */

	buf = &buf + 1;
	bufspace = buf + 500 - topofmem();/* compute space available */
	bufspace = -bufspace;		  /* for text collection buf */

	reset();
/*	initbaud();
	setbaud(current=INITB);	/* defualt baud rate: 4800 	*/
leave this stuff out, for now... */
}

/*
	hangup a hayes smartmodem
*/

hangup() {

	sleep(20) ;		/* a nice long wait */
	hayesput("...") ;	/* get it's attention */
	hayesput("AT H\r") ;	/* tell it to hang up */
	}			/* and we is through */
/*
	set baud rates
*/

setbaud(r)
char	r;
{
/*
	outp(CTCPORT,SETBAUD);		/* set baud rate	*/
	outp(CTCPORT,bauds[r].hex);
are you kidding */
}

initbaud()
{
	strcpy(bauds[0].rate,"110");	bauds[0].hex=0x57;
	strcpy(bauds[1].rate,"300");	bauds[1].hex=0x20;
	strcpy(bauds[2].rate,"600");	bauds[2].hex=0x10;
	strcpy(bauds[3].rate,"1200");	bauds[3].hex=0x8;
	strcpy(bauds[4].rate,"2400");	bauds[4].hex=0x4;
	strcpy(bauds[5].rate,"4800");	bauds[5].hex=0x2;
	strcpy(bauds[6].rate,"9600");	bauds[6].hex=0x1;
}


dobauds()
{
	char	n;

	puts(CLEARS) ;
	printf("Current Baud rate is %s\n\n",
		bauds[current].rate);
	for(n=0; n<7; n++)
	      printf("%c: %s\n",n+0x41,bauds[n].rate);
	printf("\nWhich rate? ");
	n=toupper(getchar())-0x41;
	if(n>=0 && n<7)
		setbaud(current=n);
	putchar(NL);
}

/*
	rest of tel.c stuff
*/


/*
	Get all the info pertinent to a file transfer; i.e,
	whether or not the file is text (and needs parity
	stripped, nulls ignored, echoing to console, etc.),
	whether or not checksumming and handshaking are
	required (they always go together), and make sure
	the user is in full duplex mode.
*/

askstuff()
{
	chflag = ask("Handshaking & checksumming:");
	if (!chflag && ask("Is this a text file")) {
		nflag = 0;
		spflag = didflag = 1;
		dodflag = !hdflag;
		}	
	else {	
		spflag = didflag = dodflag = 0;
		nflag = 1;
		}
	if(!chflag && rflag) {
		printf("Input file? ");
		gets(rname);
		if(!strlen(rname) || opento())
			return(0);
		}
	hd_sav=hdflag;
	hdflag = 0;
	return(ask("\nOK...type y to begin, n to abort:"));
}

/*
	Routine to print out a string and return true
	if the user responds positively
*/

ask(s)
char	*s;
{
	char	c;

	while (1) {
		printf("%s (y/n)? ",s);
		switch(c=toupper(getchar())) {
			case 'Y':	printf("es\n");	
			case '1' :	return 1;

			case 'N' :	printf("o\n");
			case '0' :
			case NL :	return 0;

			default :	printf("???\n");
					break;
			}
		}
}


/*
	Print out status of Telnet
*/

dostat()
{
	putchar(NL);
	if (rflag) {
		printf("Output file = %s\n",rname);
		printf("\nText collection: ");
			if (cflag)
				if (pflag) printf("on, but pausing\n");
				else printf("on\n");
			else printf("off\n");
		}
	else printf("No output file\n");
	printf("Text buffer has %u bytes free\n",free);
	if (tflag) {
		printf("Transmitting: %s ",tname);
		if (pflag)
			printf("(but pausing)");
		putchar(NL);
		}
	else printf("Not transmitting any file\n");
	printf("Current Escape Character: 0x%02x\n",SPECIAL);
	printf("Incoming nulls are being %s\n",nflag?"collected" : "ignored");
	printf("Parity bits are being %s\n",spflag ?"stripped" : "preserved");
	printf("%s\n",hdflag ? "Half-Duplex" : "Full-Duplex");
	printf("Current Baud rates: Main: %s  Aux: %s\n",
    		bauds[current&0xf],bauds[current>>4]);
	printf("Disk: %c:\n",disk());
}


/*
	Routine to dump contents of the memory text buffer
	to the output file and clear the buffer for more
	data:
*/

rdump(n)
{
	for (i=buf; i<cptr; i++)
		putc(*i,rbuf);
	cptr = buf;
	free = bufspace;
	if (n)
		printf("\nBuffer written. ");
}


/*
	Routine to dump and close the output file:
*/

rclose()
{
	rdump(1);
	printf(" Closing %s ",rname);
	if (!chflag)
		putc(CPMEOF,rbuf);
	fflush(rbuf);
	close(rfd);
	putchar(NL);
}

/*
	Routine to reset telnet
*/

reset()
{
	leftover = scount = timoutf = rflag = tflag = chflag = cflag = 0;
	didflag = spflag = 1;
	dodflag = !hdflag;
	hdflag=hd_sav;
}


/*
	Get a byte from the modem:
*/

getmod()
{
	char	c;
	unsigned n;

	if (timoutf) 
		return;
	for (n=20000; !miready() && n; n--)
		if (kbabort()) 
			return;
	if (!n) {
		timoutf = 1;
		return(1);
		}
	c = mgetc();
	return(c);
}


/*
	Output a byte to the modem:
*/

outmod(c)
char c;
{
	while (!moready())
		if (kbabort())
			return;
	mputc(c);
}

/*
	return true if a key has been pressed, else 0
*/

kbready()
{
	return bios(2);
}

/*
	Get a character from the keyboard:
	(Uses a direct BIOS instead of going through
	the BDOS. By naming this the same as the library
	version of "getchar", we insure that THIS version
	is used by things like "gets" instead of the library
	version.)
*/

getchar()
{
	char c;

	c = getch();
	if (c == CR)
		c = NL;
	putchar(c);
	return c;
}

getch()
{
	return bios(3);
}


/*
	Return true if keyboard hit and SPECIAL typed:
*/

kbabort()
{
	if (kbready() && getch() == SPECIAL) {
		abortf = 1;
		return 1;
		}
	return 0;
}


/*
	Write a character to the console.
*/

putchar(c)
char c;
{
	if (c == NL)
		putch2(CR);
	putch2(c);
}

putch2(c)
char c;
{
	bios(4,c);
}


/*
	Write character to console display
*/

display(c)
char c;
{
	putch2(c&0x7f);
}



/*
	Read a sector of the transmission file:
*/

read1()
{
	int i;

	i = read(tfd, tbuf, 1);
	if ( i == ERROR) {
		printf("\nRead error from %s; Aborting.\n",tname);
		tabort();
		}	
	return(i);
}

tabort()
{
	if (chflag)
		while (bcount++ != 133)
			outmod(ETX);
	printf("\nTransmission of %s aborted.\n",tname);
 	close(tfd);
	reset();
}

/*
	create receiving file. if it already exists, change type name
	and try again.
*/

opento()	
{
	char	c;

	c=1;
	printf("Creating %s...",rname);
	while((rfd=open(rname,0)) != ERROR) {
		close(rfd);
		addtype(rname,c++);
		printf("File exists..Trying %s...",rname);
		}
	if ((rfd=fcreat(rname,rbuf)) == ERROR) {
		printf("Cannot create %s\n",rname);
		reset();
		return(1);
		}
	return(0);
}

/*
	add sub name to file name if file already exsists
*/

addtype(f,c)
char	*f,c;
{
	char	d,o[4];

	for(d=0;f[d] && f[d]!='.';d++);
	sprintf(o,"%-3d",c);
	if(!f[d]) {
		f[d]='.';
		f[d+1]=0;
		}
	else
		f[d+1]=0;
	strcat(f,o);
}

/*
	send file name being transmitted
*/

sendname(f)
char	*f;
{
	char	c;

	for(c=0; *f && c<12; c++)
		outmod(*(f++));
	outmod(NUL);
}

/*
	get file name being sent
*/

getname(a)
char	*a;
{
	char	d,c,*q;

	q=a;
	for(d=0; (c=getmod()) && d<12; d++)
		*(q++)=c;
	*q=0;
}

/*
	display files on currently logged in disk
*/

dir()
{
	char	f;
	struct	FCB	fcb;

	putchar(NL);
	bdos(SELDSK,thisdisk&0x0f);
	strcpy(files[0],"????????.???");
	leftover=1;
	nameok();
	for(f=0;f<leftover;f++)
		printf("%-12s%c",files[f],((f+1)%6)?' ':'\n');
	putchar(NL);
}

/* 
	returns disk name of currently logged in disk
*/

disk()
{
	return("AB"[bdos(CURRDSK,0)&0x0f]);
}

/* 
	change currently logged-in disk to other disk
*/

togdisk()
{
	bdos(SELDSK,(thisdisk=((thisdisk)?0:1)));
	return(disk());
}

/*
	check to see if this file is in directory
*/

nameok()
{
	char	f,c,o[20];
	struct	FCB	fcb;

	if(!strlen(files[leftover-1])) 
		return(1);
	setfcb(fcb,files[leftover-1]);
	leftover--;
	for(f=DIRFRST;(c=bdos(f,&fcb))!=255 && leftover<maxfiles;f=DIRNEXT) {
		pickout(0x80+(c*32),o);
		strcpy(files[leftover++],o);
		}
	return((leftover<maxfiles)?0:1);
}


xmit()
{
	int incheck;
	int n;
	char c;

	if (pflag || !moready())
		return 0;
	c = tbuf[bcount++];
	checksum += c;
	if ((!(spflag && (c&0x7f)==CPMEOF && !chflag)) &&
		(!(!chflag && c==NL && lastc==CR && !nflag && fflag)))
			outmod(c);
	lastc = c;
	if (dodflag)
		display(c);
	if (bcount != SECSIZ)
		return 0;
	bcount = 0;
	if (!chflag)
		return(!read1());
	incheck = (getmod() << 8) + getmod();
	if (incheck != checksum) {
		for (n=0; n<20000; n++); 	/* let line settle down */
		printf("\nError. Resending sector %d...\n",scount+1);
		outmod(NAK);
		}
	else if (read1()) {
		if (!dodflag) 
			printf(".%d",++scount);
		outmod(ACK);
		}
	else if(leftover) {
		close(tfd);
		--leftover;
		strcpy(tname,files[leftover-1]);
		while(leftover && ((tfd=open(tname,0))==ERROR)){
			if(!dodflag)
				printf("Unknown error: Can't open %s\n",tname);
			leftover=0;
			}
		if (!leftover) {
			reset();
			putchar(NL);
			outmod(EOT); 
			return(1);
			}
		if(!dodflag)
			printf("\nSending %d: %s\n",leftover,tname);
		outmod(STX);
		sendname(tname);
		scount = pflag = checksum = bcount = 0;
		if (read(tfd,tbuf,1) <=0) {
			printf("Read error from %s\n",tname);
			reset();
			abortf = 1;
			return;
			}
		}
        else { 
		outmod(EOT); 
		return(1);
		}
	checksum = 0;
	if (getmod() != PHA) {
		printf("\nPhase error; aborting...");
		abortf = 1;
		}
	return 0;
}
                                      