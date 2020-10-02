/*
>>: yam.h
*/
#include "yamsys.h"		/* installation specific stuff */

#ifndef XMODEM
#define VERSION "YAM 2.11 Rev 9-06-81\n"
#define lprintf printf
#else
#define VERSION "XYAM 2.11 Rev 9-06-81\n"
#endif
/*
 * yam	-Yet Another Modem program
 *		Chuck Forsberg 7-16-81
 *
 *	Design goals: A general program performing the functions
 *	of MODEM, BYE, and XMODEM, portable to non-timesharing
 *	systems.
 *
 *	Coding style is/was aimed at portability and clarity.
 *	File transfer routines are written without timing assumptions
 *   allowing error free high speed operation even if view selected
 *
 *	Comments to me at:
 *	Chuck Forsberg 503/621-3406 Voice
 *		RBBS RCP/M 503/621-3193	(latest source code)
 *		  At 300, 450, or 1200 bps
 *		Portland ABBS 503/224-6409 or CBBS NW 503/284-5260
 *		Source TCE022, CPS 70715,131
 *
 *	Files:
 *		yam.h		header and globals
 *		yamsys.h	Modem port adresses, installation specific data
 *		yam1.c		First time initialization
 *					Main command decoding
 *		yam2.c		Christensen protocol handler
 *		yam3.c		Conversation with data capture
 *					File xmsn w/o error checking.
 *	(yam4 reserved for a Forward Error Correcting technique for Telemail)
 *		yam5.c		Misc modem related functions, such as baudrate
 *		yam7.c		Disk i/o open,close, directory
 *					Wild card expansion, CP/M related stuff
 *
 *		compiled with -e 5800 for all files and -o for yam[235]
 *		(-e 6000 for XYAM)
 */

#ifdef CPM
char defdisk;		/* default disk */
char user;
#define PATHLEN 20	/* plenty long */
#endif

#ifdef BDSC
#define FLAG char
FILE fin,fout;		/* #define FILE struct _buf */
int logfd;		/* used by logfile */
#define stdin 0
#define stdout 1
#define stderr 1
char cfast;	/* BDS C fastest access is to extern's */
char checksum;	/* delcared here for speed */
int wcj, firstch;
#endif

#define CMDLEN 132

#define BRKCHR 200
#define ENQ 005
#define CAN ('X'&037)
#define XOFF ('s'&037)
#define XON ('q'&037)
#define SOH 01
#define EOT 4
#define ACK 6
#define NAK 025
#define BDNMCH 0x75
#define OKMNCH ACK
#define TIMEOUT (-1)
#define ERRORMAX 5
#define RETRYMAX 5

/* declare all globally used functions not returning int */
char *index(), *cisubstr();

FLAG Quiet;
char Ttycol, Lpcol;	/* column for tab expansion */
unsigned Numsecs;	/* Number of 128 byte sectors, used by compsec */
unsigned Numblks;	/* Number of CP/M blocks, used by compsec */
unsigned Numfiles;	/* Total number of files expanded */
unsigned Secpblk;	/* Number of sectors per block */
FLAG Rfile;		/* receiving into a file */
FLAG Tfile;		/* transmitting from a file */
FLAG Batch;		/* Batch (multi-file) xfer */
FLAG Creamfile;		/* true means o.k. to blast old file on rx */
FLAG Txgo;		/* file sending start/stop (XON/XOFF) */
FLAG Dumping;		/* not Squelched ^R */
FLAG Pflag;		/* printing the rx stuff */
FLAG Ctlview;		/* Term function shows control chars as ^char */
FLAG View;	/* View data being transmitted/received if file xfer */
FLAG Echo;	/* Echo modem data back to itself only in chat */
FLAG Hdx;	/* Half Duplex connection -no echo from far end */
FLAG Gototerm;	/* go to term after file xfer */
FLAG Exoneof;	/* exit from term() when finished with file */
/* Following flags affect data as it is written to file, not captured */
FLAG Image;	/* Tell it like it is. */
FLAG Zeof;	/* Terminate data capture & close file on ^Z */
FLAG Squelch;	/* Turn capture on after ^R, off before ^T */
char *Txmoname;		/* asciz string corresponding to Txeoln mode */
FLAG Txeoln;	/* what to do at the end of a line */
#define EOL_NOTHING 0	/* send it all as is */
#define EOL_CR	1		/* send /r only */
#define EOL_NL	2		/* send \n only */
#define EOL_CRWAIT 3	/* send \r then wait for \n echo */
#define EOL_CRPROMPT 4	/* send \r then wait for silence. */
#define TX_BINARY 0200	/* don't stop on CPMEOF or whatever **/

/* For controls based on time since last RX char mostly */
unsigned Timeout;	/* measure of how long since last rx char */
unsigned Tpause;	/* when ++Timeout==Tpause, do something ... */
FLAG Xoffflg;		/* !=0 if we sent XOFF to stop the bubble machine */
FLAG Waitecho;		/* Wait for echo to stop when sending file */

/* If Waitbunch, Wait for Throttle loops each waitnum tx chars */
FLAG Waitbunch;
unsigned Waitnum;	/* do Waitecho each time chars_sent == Waitnum */
unsigned Throttle;
FLAG Txwait;		/* used with throttle */
unsigned Txtimeout;	/* counted down to 0, then Txwait becomes false */

unsigned T1pause;	/* for setting 0.1 second timeout for readline */
char Lastrx;		/* last char received, for detecting CAN CAN */

/*
 * start, end, input, output, printer-output pointers and marker
 * used with data capture
 */
char *bufst, *bufend, *bufcq, *bufcdq, *buffcdq, *bufpcdq, *bufmark;
unsigned Free;		/* Number of characters free in capture buf */
unsigned Low;		/* when free==low, send the XOFF */
unsigned Bufsize;
FLAG Wrapped;		/* Circular buffer insertion pointer has wrapped around */
char Dport,Sport;	/* data and status port addresses */
unsigned Baudrate;  /* current baudrate */
FLAG Originate;	/* Originate mode requested */
FLAG Parity;
#define SEND8 0	/* RAW on Unix */
#define NORMAL 1	/* cooked o.k. on Unix */
	/* following would be CBREAK on Unix */
#define EVEN7 2
#define ODD7 3
#define EVEN8 4
#define ODD8 5

char cmdbuf[CMDLEN+2], *cp;  /* for commands entered directly (not argv) */
char Rname[PATHLEN], Tname[PATHLEN];	/* saved file names */
char Phone[80];		/* phone number read in from file */

/*
 * Unix is a trademark of Western Electric
 * CP/M, Digital Research
 * Coherent, Mark Willams Co., Chicago IL
 * IDRIS, Whitesmiths Co.
 */

/*
Some #DEFINES you should know about:
	RESTRICTED disallows downloading of TAG2 files, upload .com>.obj
	LOGFILE logs all file xmsn attempts, also is the file
	TERMRESET string initializes terminal to desired mode
	PHONES file with phone numbers
	FILE means struct _buf for BDS C
*/
 pointer has wrapped around */
char Dport,Sport;	/* data and status port addresses */
