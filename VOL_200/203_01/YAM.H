/*
$TITLE ('yam.h: lattice c version')
$DATE (27 OCT 85)
*/

#include "yamsys.h"		/* installation specific stuff */
#include <stdio.h>

/*
 * yam	-Yet Another Modem program
 *	original by Chuck Forsberg 10-18-81
 *	MSDOS conversion Olen Pederson 11/85
*/

#define TRUE 1
#define FALSE 0
#define ERROR -1
#define OK 0

extern char defdisk;			/* default disk */
#define PATHLEN 65				/* maximum path name length */	
#define FLAG int

	/* characters will be stripped if received and Ignrx true */
extern char rxnono[15];

extern FILE *fin,*fout;
extern int cfast;
extern char checksum;			/* delcared here for speed */
extern unsigned oldcrc;	 		/* accumulates CRC checksum */
extern int wcj, firstch, errors;
extern FLAG firstsec;			/* first sector, C instead of NAK for crc */

#ifndef IOTYPE					/* may be char* or unsigned for memory mapped i/o */
#define IOTYPE int
#endif

#define TABS 8					/* initial tab expansion */
#define CMDLEN 132
#define LOWWATER 2000			/* low water value causes term to send XOFF */
	/* ascii literals */
#define SOH 1
#define STX 2
#define ETX 3
#define EOT 4
#define ENQ 5
#define ACK 6
#define SO 016
#define SI 017
#define DLE 020
#define NAK 025
#define CAN 030
#define ESC 033

#define WANTCRC 0103		/* send C not NAK to get crc not checksum */
#define TIMEOUT (-2)
#define ERRORMAX 5
#define RETRYMAX 5
#define CPMEOF 0x1A

#ifndef SECSIZ
#define SECSIZ 128			/* Sector size disk calls */
#endif

#define KSIZE 1024			/* block length with k option */

/* declare all globally used functions not returning int */
char getcty(), putcty(), *index(), *cisubstr();
unsigned getfree();
FILE *fopen();

extern FLAG Quiet;
extern char Ttycol;					/* column for tab expansion */
extern int tabstop;					/* current tab expansion */
extern FLAG Rfile;					/* receiving into a file */
extern FLAG Tfile;					/* transmitting from a file */
extern FLAG Crcflg;		 			/* 1 if using crc, 0 otherwise */
extern FLAG Batch;					/* Batch (multi-file) xfer */
extern FLAG Creamfile;				/* true means o.k. to blast old file on rx */
extern int blklen;					/* length of blocks for s and r, usually 128 */
extern unsigned totsecs;			/* accumulator of number of sectors sent/received */
extern unsigned toterrs;			/* total number of errors in xfer(s) */
extern FLAG Txgo;		 			/* file sending start/stop (XON/XOFF) */
extern FLAG Dumping;				/* not Squelched ^R */
extern FLAG Pflag;					/* printing the rx stuff */
#ifdef MYSYSTEM
extern FLAG Cis02;					/* Special functions for CIS02 access to Compuserve */
#endif
extern FLAG Ctlview;				/* Term function shows control chars as ^char */
extern FLAG View;					/* View data being transmitted/received if file xfer */
extern FLAG Echo;					/* Echo modem data back to itself only in chat */
extern FLAG Hdx;					/* Half Duplex connection -no echo from far end */
extern FLAG Chat;					/* treat kbd \r as crlf when chatting */
extern FLAG Gototerm;				/* go to term after file xfer */
extern FLAG Exoneof;				/* exit from term() when finished with file */

/* Following flags affect data as it is written to file, not captured */
extern FLAG Zeof;		/* Terminate data capture & close file on ^Z */
extern FLAG Squelch;	/* Turn capture on after ^R, off before ^T */
extern char GOchar;		/* normally \n; resumes sending in EOL _ CRWAIT mode */
extern char *Txmoname;	/* asciz string corresponding to Txeoln mode */
extern FLAG Txeoln;		/* what to do at the end of a line */
extern char xon;		/* character for xon */
extern char xoff;		/* character for xoff */

#define EOL_NOTHING 0	/* send it all as is */
#define EOL_CR	1		/* send /r only */
#define EOL_NL	2		/* send \n only */
#define EOL_CRWAIT 3	/* send \r then wait for GOchar echo */
#define EOL_CRPROMPT 4	/* send \r then wait for silence. */
#define TX_BINARY 0200	/* don't stop on CPMEOF or whatever **/

/* For controls based on time since last RX char mostly */
extern unsigned Timeout;		/* measure of how long since last rxchar */
extern unsigned Tpause;	 		/* when ++Timeout==Tpause, do something ... */
extern FLAG Xoffflg;			/* !=0 if we sent XOFF to stop the bubble machine */
extern FLAG Waitecho;	 		/* Wait for echo to stop when sending file */

/* If Waitbunch, Wait for Throttle loops each waitnum txchars */
extern FLAG Waitbunch;
extern unsigned Waitnum;		/* do Waitecho each time chars_sent == Waitnum */
extern unsigned Throttle;
extern FLAG Txwait;		 		/* used with throttle */
extern unsigned Txtimeout;		/* counted down to 0, then Txwait becomes false */

extern unsigned T1pause;		/* for setting 0.1 second timeout for readline */
extern int Lastrx;				/* last char received, for detecting CAN CAN */

/*
 * start, end, input, output, printer-output pointers and marker
 * used with data capture
 */
extern char *bufst, *bufend, *bufcq, *bufcdq, *buffcdq, *bufpcdq, *bufmark;
extern unsigned Nfree;				/* Number of characters free in capture buf */
extern unsigned Bufsize;
extern FLAG Wrapped;				/* Buffer insertion pointer has wrapped around */
extern IOTYPE Dport;				/* data port address */
extern unsigned commport;			/* comm port number, related to actual port # */
extern char Mstatus;	 			/* contents of modem error register if modem error */
extern unsigned Baudrate;			/* current baudrate */
extern FLAG Originate;				/* Originate mode requested */
extern FLAG Ignrx;					/* if true, rxnono bits will be thrown awaw */


	/* default strip parity bits from console kbd */
#ifndef KBMASK
#define KBMASK 0177
#endif

extern char cmdbuf[CMDLEN+2], *cp;	/* for commands entered directly (not argv) */
extern char Rname[PATHLEN], Tname[PATHLEN];		 /* saved file names */
extern char Phone[80];				/* phone number read in from file */

/* declarations for USQ feature */
#define SQMAGIC 0xFF76	/* SQueezed file prefix */
#define NUMVALS 257		 /* 256 data values plus SPEOF*/
extern union
{
			FILE *ufbuf;
			char ubuf[KSIZE];
			/* Decoding tree for usq feature */
			struct
			{
				int children[2];			/* left, right */
			} dnode[NUMVALS - 1];
} Utility;

	/* buffer type for file I/O.  used by DOS function find first and
	   next. */
struct find_buf
{
	char res[21];
	char attrib;
	unsigned time;
	unsigned date;
	long filesize;
	char pname[13];
};
#define FBSIZ sizeof(struct find_buf)

/*
 * Unix is a trademark of Western Electric
 * CP/M, Digital Research
 * MS-DOS, Micro-soft
*/
