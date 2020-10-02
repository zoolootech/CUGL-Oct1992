#include <stdio.h>
#define PATHLEN 65	
#define FLAG int
#define CMDLEN 132

#ifndef IOTYPE				/* may be char* or unsigned for memory mapped i/o */
#define IOTYPE int
#endif

/* global configuration buffers */
char rxnono[15];    /* characters will be stripped if received and Ignrx true */

char defdisk;				/* default disk */
FILE *fin,*fout;
int cfast;
char checksum;				/* delcared here for speed */
unsigned oldcrc;			/* accumulates CRC checksum */
int wcj, firstch, errors;
FLAG firstsec;	 			/* first sector, C instead of NAK for crc */

FLAG Quiet;
char Ttycol, Lpcol;			/* column for tab expansion */
int tabstop;				/* current tab expansion */
FLAG Rfile;					/* receiving into a file */
FLAG Tfile;					/* transmitting from a file */
FLAG Crcflg;		 		/* 1 if using crc, 0 otherwise */
FLAG Batch;					/* Batch (multi-file) xfer */
FLAG Creamfile;				/* true means o.k. to delete old file on rx */
int blklen;					/* length of blocks for s and r, usually 128 */
unsigned totsecs;			/* accumulator of number of sectors sent/received */
unsigned toterrs;			/* total number of errors in xfer(s) */
FLAG Txgo;		 			/* file sending start/stop (XON/XOFF) */
FLAG Dumping;				/* not Squelched ^R */
FLAG Pflag;					/* printing the rx stuff */
#ifdef MYSYSTEM
FLAG Cis02;					/* Special functions for CIS02 access to Compuserve */
#endif
FLAG Ctlview;				/* Term function shows control chars as ^char */
FLAG View;					/* View data being transmitted/received if file xfer */
FLAG Echo;					/* Echo modem data back to itself only in chat */
FLAG Hdx;					/* Half Duplex connection -no echo from far end */
FLAG Chat;		 			/* treat kbd \r as crlf when chatting */
FLAG Gototerm;				/* go to term after file xfer */
FLAG Exoneof;				/* exit from term() when finished with file */

/* Following flags affect data as it is written to file, not captured */
FLAG Zeof;			/* Terminate data capture & close file on ^Z */
FLAG Squelch;		/* Turn capture on after ^R, off before ^T */
char GOchar;		/* normally \n; resumes sending in EOL _ CRWAIT mode */
char *Txmoname;		/* asciz string corresponding to Txeoln mode */
FLAG Txeoln;		/* what to do at the end of a line */
char xon;			/* character for xon */
char xoff;			/* character for xoff */

/* For controls based on time since last RX char mostly */
unsigned Timeout;		/* measure of how long since last rxchar */
unsigned Tpause;		/* when ++Timeout==Tpause, do something ... */
FLAG Xoffflg;			/* !=0 if we sent XOFF to stop the bubble machine */
FLAG Waitecho;	 		/* Wait for echo to stop when sending file */

/* If Waitbunch, Wait for Throttle loops each waitnum txchars */
FLAG Waitbunch;
unsigned Waitnum;		/* do Waitecho each time chars_sent == Waitnum */
unsigned Throttle;
FLAG Txwait;		 	/* used with throttle */
unsigned Txtimeout;		/* counted down to 0, then Txwait becomes false */

unsigned T1pause;		/* for setting 0.1 second timeout for readline */
int Lastrx;				/* last char received, for detecting CAN CAN */

/*
 * start, end, input, output, printer-output pointers and marker
 * used with data capture
 */
char *bufst, *bufend, *bufcq, *bufcdq, *buffcdq, *bufpcdq, *bufmark;
unsigned Nfree;				/* Number of characters free in capture buf */
unsigned Bufsize;
FLAG Wrapped;				/* Buffer insertion pointer has wrapped around */
IOTYPE Dport;				/* data port address */
unsigned commport;			/* comm port number, related to actual port # */
char Mstatus;	 			/* contents of modem error register if modem error */
unsigned Baudrate;			/* current baudrate */
FLAG Originate;				/* Originate mode requested */
FLAG Ignrx;					/* if true, rxnono bits will be thrown awaw */
char cmdbuf[CMDLEN+2], *cp;	/* for commands entered directly (not argv) */
char Rname[PATHLEN], Tname[PATHLEN];		 /* saved file names */

#define NUMVALS 257			 /* 256 data values plus SPEOF*/
#define KSIZE  1024			/* block length with k option */

/* Some declarations for USQ feature */
union
{
			FILE *ufbuf;
			char ubuf[KSIZE];
			/* Decoding tree for usq feature */
			struct
			{
				int children[2];			/* left, right */
			} dnode[NUMVALS - 1];
} Utility;

char Phone[80];				/* phone number read in from file */
