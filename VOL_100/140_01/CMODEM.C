/*
                           CMODEM.C

Revision history: Latest revision first.

04/Jun/82   Removed PMMI defines and ifdef for Australia,  much 
            more  extensive  documentation  added,  modem  port 
            defines now obtained from BDSCIO.H.  Code tidied up 
            a  bit  and VARBAUD  ifdefs  added,  some  messages 
            changed and numerous small changes. Bill Bolton

??/???/81   Patched together from existing Cnet and YAM code by 
            Steve Passe, Cnode SYSOP, USA.

This  program  uses  the  "Christensen" protocol  as  (used  in 
YAM/MODEM7) for file transfers.

When  you execute CMODEM it signs on and immediately goes  into 
the  terminal mode (without any messages) and is ready to  send 
ASCII  text from your console keyboard out your modem port  and 
receive ASCII text from your modem port  and display it on your 
console.  An  "esc"  convention allows file  transfers  without 
leaving  the program.

To send a file:
		type '<esc> s <cr>',
To receive one:
		type '<esc> r <cr>'

    (you will be prompted for the file name in both cases)

For other commands:
		type '<esc> ? <cr>'

		(<esc> is the ASCII escape character)

If you should need to send an <esc> to the remote system simply 
hit  <esc>  twice  in a row for each <esc>  that  you  wish  to 
send. While transfering files CMODEM echos everything, from the 
file it sends or receives, to the console so be prepared to see 
some  weird things when transfering object  files.  It  doesn't 
send  non-ASCII  data (i.e.  control codes) to the console  but 
replaces them with a full stop.

This is really just a very minimal implementation of a  program 
to send and receive files in using the "Christensen" protocols. 
For   a  "full  feature"  communications  program  you   should 
definitely look at YAM.

Add  whatever code necessary to the initialize_port()  function 
to setup your port for:
	8 bits,
	1 stop bit,
	no parity,
	baud rate of 300 if necessary,
	etc.).

If  you  are  certain  that  your modem  port  will  always  be 
initialised  in the correct data format before entry to  CMODEM 
(perhaps initialised at cold boot time) you can just leave this 
function empty.

If your modem UART has programmable baud rates you (2651,  8250 
or  whatever) and you want to be able to set them  from  CMODEM 
you  should  insert  code  in the baud()  function  and  define 
VARBAUD.  This is probably not really worthwhile for Australian 
conditions but I left the code in place in case anyone wants to 
try it.  It is easier to leave your UART set at 300 baud as the 
CCITT V21 modems currently available in Australia just wont run 
any faster than 300 baud....maybe that will change with digital 
filtering in the future.

If you are using a direct connect modem and you want to be able 
to  drop the line when finished you should insert code  in  the 
hangup() function.

The  defines for the modem port must be set up in the  BDSCIO.H 
file  for your system.  Note that this program uses BDOS call 6 
(direct  console  I/O)  for all console  access  and  therefore 
should be easily usable by those with odd consoles that are not 
easily defined in BDSCIO.H (such as Sorcerer or memory mapped). 
If you have TELNET running you should be able to compile CMODEM 
and  run  it  without any modifications (make  sure  thae  data 
format as described above is correct). 

This  program  uses large chunks of the YAM  package  for  file 
transfers (thanks Chuck).

*/

#include "bdscio.h"

#define SOH 0x01
#define EOT 0x04
#define ACK 0x06
#define NAK 0x15
#define CAN 0x18

#define RETRYMAX 10
#define TIMEOUT (-1)
#define PATHLEN 20
#define WCEOT (-2)
#define CLKMHZ 4         /* CPU speed in Mhz */

#define CONSTAT 2
#define CONIN 3
#define DIR_IO 0x06
#define INPUT 0xff
#define NORMAL 0x1c
#define ORIG 0x01
#define ANSWER 0x02
#define READY 0x5f
#define OPT300 0x20
#define OPT600 0X00
#define CLEAR 0x3f
#define FLAG char

int Baud;
FLAG Tfile, Rfile;
char Tname[PATHLEN], Rname[PATHLEN];
unsigned T1pause, Timeout;
char File_buf[BUFSIZ];
char Checksum, Lastrx;
int Wcj, Firstch;

main()
{
	char received, to_send, in_modem(), getch(), escflag;

	Tfile = Rfile = FALSE;
	T1pause = 311*CLKMHZ;
	escflag = NULL;
	Baud = 300;

	initialize_port();

	printf("\n\CMODEM a 'Christensen' protocol file transfer program\n\n");

	while (TRUE) {
		if (received = in_modem()) {
			 putch(received);
		}
		else if (to_send = getch()) {
			if (to_send == ESC) {
				if (escflag) {
					escflag = NULL;
					out_modem(to_send);
				}
				else escflag = TRUE;
			}
			else {
				if (escflag) {
					escflag = NULL;
					commands(to_send);
				}
				else out_modem(to_send);
			}
		}
	}
}

char		/* get incoming byte from modem */
in_modem()
{
	if (inp(MSTAT) & MIMASK) {	/* status & char ready bit */
		return inp(MDATA);	/* ok, get the char */
	}			/* or */
	else return FALSE;	/* return empty */
}

out_modem(out_char)	/* send byte to modem */
char out_char;
{
	while (!(inp(MSTAT) & MOMASK))
		;								/* wait */
	outp(MDATA, out_char);	/* finally, send it */
}

char		/* keyboard 'hook' and 'filter' */
getch()
{
	return bdos(DIR_IO, INPUT);
}

commands(cmd)
char cmd;
{
	int baudrate;

	switch (tolower(cmd)) {
#ifdef VARBAUD
	case 'b':
		printf("\n\n\tbaudrate: ");
		scanf("%d", &baudrate);
		baud(baudrate);
		break;
#endif
	case 'q':
		hangup();
		break;
	case 'r':
		printf("\n\n\treceive: ");
		scanf("%s", Rname);
		download(Rname);
		break;
	case 's':
		printf("\n\n\tsend: ");
		scanf("%s", Tname);
		upload(Tname);
		break;
	case '?':
	case 'h':
		help();
		break;
	default:
		printf("\nbad command: '%c'\n", cmd);
	}
}

/*

	Most of the following functions taken from YAM package by
Chuck Forsberg, BDS "C" UG disk Utilities #, some modifications made for
compatibility with cnode code.

*/

upload(filename)
char *filename;
{
	int err_flag;

	if(opentx(Tname)==ERROR) {
		printf("\ncan't open '%s'\n", filename);
		return ERROR;
	}
	printf("\n'%s' open for transmission\n", Tname);
	if(wctx()==ERROR) {
		abort('t');
	}
	return OK;
}



wctx()
{
	int sectnum, attempts;
	char txbuf[SECSIZ];

	printf("Awaiting initial NAK - ");
	while((Firstch=readbyt(400))!=NAK && Firstch!=TIMEOUT && Firstch!=CAN)
	{
		printf("\n\tgot 0x%2x, not NAK", Firstch);
					/* let user see it if strange char */
	}
	if(Firstch==CAN)
		return ERROR;
	if (Firstch == TIMEOUT) {
		printf("\ntimeout on initial NAK!");
		return ERROR;
	}
	sectnum=1;
	while(filbuf(txbuf, SECSIZ)) {
		if(wcputsec(txbuf, sectnum)==ERROR) {
			return ERROR;
		}
		else {
			sectnum++;
		}
	}
	closetx();
	attempts=0;
	do {
		out_modem(EOT);
		purgeline();		/* why? */
		attempts++;
	}
		while((Firstch=(readbyt(100)) != ACK) && attempts < RETRYMAX)
			;		/* wait for ACK */
	if(attempts == RETRYMAX) {
		printf("\nNo ACK on EOT; Aborting... ");
		return ERROR;
	}
	else
		return OK;
}

wcputsec(txbuf, sectnum)
char *txbuf;
int sectnum;
{
	char attempts, *cp, xbyt;
	
	Firstch=0;	/* part of logic to detect CAN CAN */

	for(attempts=0; attempts <= RETRYMAX; attempts++) {
		Lastrx= Firstch;
		out_modem(SOH);
		out_modem(sectnum);
		out_modem(~sectnum);
		Checksum=0;
		for(Wcj=SECSIZ,cp=txbuf; --Wcj>=0; ) {
			out_modem(*cp);
			isprint(*cp) ? putchar(*cp) : putchar('.');
			Checksum += *cp++;
		}
		out_modem(Checksum);
		purgeline();

		Firstch=readbyt(100);
		if(Firstch==CAN && Lastrx==CAN) {
cancan:
			printf("\nReceiver CANcelled transmission ");
			return ERROR;
		}
		else if(Firstch==ACK)
			return OK;
		else if(Firstch==TIMEOUT)
			printf("\nTimeout on sector ack attempt %d", attempts);
		else {
			printf("\nGot %2x for sector ACK attempt %d", Firstch, attempts);
			for(;;) {
				Lastrx=Firstch;
				if((Firstch=readbyt(1))==TIMEOUT)
					break;
				if(Firstch==CAN && Lastrx==CAN)
					goto cancan;
			}
		}
	}
	printf("\nNo ACK on sector; Abort ");
	return ERROR;
}

opentx(name)
char *name;
{
	printf("'%s' ", name);
	if(fopen(name, File_buf)==ERROR){
		return ERROR;
	}
	Tfile= TRUE;
	return OK;
}

/* fill buf with count chars padding with ^Z for CPM */

filbuf(buf, count)
char *buf;
int count;
{
	int c, m;
	m=count;
	while((c=getc(File_buf))!=EOF) {
		*buf++ =c;
		if(--m == 0)
			break;
	}
	if(m==count)
		return 0;
	else		/* won't pad properly under cp/m because of */
			/* two different EOF's (EOF & CPMEOF), and the */
			/* fact that cp/m doesn't know type of file */
			/* instinctively, fix later */
		while(--m>=0)
			*buf++ = 0x1a;		/* control z */
	return count;
}


download(filename)
char *filename;
{
	printf("ready to receive '%s'\n", Rname);
	if(wcrx(Rname)==ERROR) {
		abort('r');
	}
	return OK;
}

/*
 * Adapted from CMODEM13.C, written by
 * Jack M. Wierda and Roderick W. Hart
 */

wcrx(name)
char *name;
{
	int sectnum, sectcurr, sectcomp;
	char *cp, rxbuf[128], sendchar;

	if(openrx(name)==ERROR)
		return ERROR;
	sectnum=0;
	sendchar=NAK;

	for(;;) {
		out_modem(sendchar);	/* send it now, we're ready! */
		sectcurr=wcgetsec(rxbuf, (sectnum & 0x7f) ? 50 : 130);
		if(sectcurr==(sectnum+1 & 0xff)) {	/* mask low byte */

			sectnum++;
			for(cp=rxbuf,Wcj=128; --Wcj>=0; )
				if(putc(*cp++, File_buf)==ERROR) {
					printf("\nDisk Full\n");
					return ERROR;
				}
			sendchar=ACK;
		}
		else if(sectcurr==sectnum) {
			printf("\nReceived dup Sector %d",sectcurr);
			sendchar=ACK;
		}
		else if(sectcurr==WCEOT) {
			out_modem(ACK);
			/* don't pad the file any more than it already is */
			closerx(FALSE);
			return OK;
		}
		else {
			printf(" Sync Error\n");
			return ERROR;
		}
	}
}

/*
 * wcgetsec fetches a Ward Christensen type sector.
 * Returns sector number encountered or ERROR if valid sector not received,
 * or CAN CAN received
 * or WCEOT if eot sector
 * time is timeout for first char, set to 4 seconds thereafter
 ***************** NO ACK IS SENT IF SECTOR IS RECEIVED OK **************
 *    (Caller must do that when he is good and ready to get next sector)
 */

wcgetsec(rxbuf, time)
char *rxbuf;
int time;
{
	int sectcurr,errors;
	char *cp;

	for(Lastrx=errors=0; errors<RETRYMAX; errors++) {
		do {
			Firstch=readbyt(time);
		}
		while(Firstch != SOH && Firstch != TIMEOUT && Firstch != EOT
			  && Firstch != CAN);	/* wait for one of these */
		if(Firstch==SOH) {
			sectcurr=readbyt(1);
			if((sectcurr+readbyt(1))==255) {
				Checksum=0;
				for(cp=rxbuf,Wcj=128; --Wcj>=0; ) {
					isprint(*cp = readbyt(1)) ? putchar(*cp) : putchar('.');
					Checksum += (*cp++);
				}
				if(((Checksum-readbyt(1))& 0xff)==0)
					return sectcurr;
				else
					printf("Checksum Error #%d", errors);
			}
			else
				printf("Sector number garbled #%d", errors);
		}
		else if(Firstch==EOT)
			return WCEOT;
		else if(Firstch==CAN) {
			if(Lastrx==CAN) {
				printf("\nSender CANcelled");
				return ERROR;
			} else {
				Lastrx=CAN;
				continue;
			}
		}
		else if(Firstch==TIMEOUT)
			printf("\nSOH Timeout #%d", errors);

		Lastrx=0;
		while(readbyt(1)!=TIMEOUT)
			;
		out_modem(NAK);
		time=40;
	}
	/* try to stop the bubble machine. */
	out_modem(CAN);out_modem(CAN);out_modem(CAN);
	return ERROR;
}

openrx(name)
char *name;
{
	printf("\nsaving it as '%s'", name);	/* show the name right away */
	if(fopen(name, File_buf) != ERROR) {
		fclose(File_buf);
		printf("\nI already have one, try another name");
		return ERROR;
	}
	if(fcreat(name, File_buf)==ERROR){
		printf("\nCan't create '%s'", name);
		return ERROR;
	}
	Rfile= TRUE;
	return OK;
}

readbyt(decisecs)
int decisecs;
{
	if (inp(MSTAT) & MIMASK)
		return inp(MDATA);
	while (--decisecs >= 0) {
		if (inp(MSTAT) & MIMASK)
			return inp(MDATA);
		if (bdos(DIR_IO, INPUT))	/* local forced timeout */
			return TIMEOUT;
		if (inp(MSTAT) & MIMASK)
			return inp(MDATA);
		for (Timeout = T1pause; --Timeout; )
			if (inp(MSTAT) & MIMASK)
				return inp(MDATA);
	}
	return TIMEOUT;
}

abort(flag)
char flag;
{
	flag == 't' ? closetx() : closerx();
	out_modem(CAN);out_modem(CAN);out_modem(CAN);
	return ERROR;
}

closetx()
{
	if(Tfile) {
		fclose(File_buf);
		printf("\n\n%s closed", Tname);
		Tfile=FALSE;
	}
}

/* for the upld overlay command */

closerx()
{
	if(Rfile) {
		fflush(File_buf);
		fclose(File_buf);
		printf("\n\n%s closed", Rname);
		Rfile=FALSE;
	}
}
purgeline()
{
	while (inp(MSTAT) & MIMASK)
		inp(MDATA);
}

/* end of code from yam */

help()
{
	printf("\n\tCommands:");
#ifdef VARBAUD
	printf("\n\t\t\t'b'aud <baudrate>");
#endif
	printf("\n\t\t\t'r'eceive <filename>");
	printf("\n\t\t\t's'end <filename>");
	printf("\n\t\t\t'q'uit");
	printf("\n");
}

initialize_port()
{
/* put code to init your modem port here */
/*	this is for initializing an 8251 uart...
	outp(MSTAT,0xaa);	/* insure next port access of 'command' type */
	outp(MSTAT,0x40);	/* 'command' a reset */
	outp(MSTAT,0x8e);	/* set the 'mode', 1-stop bit, sync 16x */
			/*	eight bits, no parity */
	outp(MSTAT,0x27);	/* trans, DTR, rec, RTS */
*/
}

isprint(c)
char c;
{
	return (c>=' ' && c <= '~') || (c == 0x0d) || (c == 0x0a) || (c==0x09);
}

hangup()
{
/* insert code to hang up line (using CCITT 108)
   if you have a direct connect modem */

	printf("\nhanging up...\n");
	exit();
}

#ifdef VARBAUD
baud(baudrate)
int baudrate;
{
	/* Insert code to set the baud rate of your UART here */

}
#endif
