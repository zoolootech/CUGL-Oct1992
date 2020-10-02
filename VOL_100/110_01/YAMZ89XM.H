/*
>>:yamsys.h 9-03-81
 *
 * global equates for specific installation and modem ports
 * Other modem specific stuff is in yam5.c
 *
 */

#define XMODEM
#define RESTRICTED

/* files have single letter ext so pip yam?????.? gets all source but no crl */
#define HELPFILE "A:XYAMHELP.T"
#define PHONES "........."
#define LOGFILE "A:XYAM.LOG"

#define CPM
#define BDSC
#define CLKMHZ 2
#define SECPBLK 32	/* 4k blocks on Magnolia DD controller 8" disk */
/* ********* following string must be in UPPER case ********* */
#define DISKS "ABEFIJ"	/* legal disks for default selection */
#define MAXUSER 0	/* maximum user number */

/* defines for Heath Z89 aux board port at 0320 */
#define STATLINE	/* do special status line information */
#define Z19				/* terminal type */
#define TERMRESET	"\033x1\033j\033Y8P\033K\033k\033v"
#define Z89				/* type of modem port */
#define DEFBAUD 300	/* initial baud rate setting */
#define DPORT 0320
#define SPORT 0325

char inp();				/* for fastest 8080 code */
#define CDO ((inp(Dport+6)&0200)==0)	/* RLSD is carrier detect */
#define MIREADY (inp(Sport)&1)	/* value != 0 if char available */
#define MIERROR (inp(Sport)&0)		/* != 0 if any error condx */
#define MICHAR (inp(Dport))		/* get char assuming miready */

#define MOREADY (inp(Sport)&040)	/* modem ready to load next char */
/* It would be nice to have parameterized macros to do the following */
#define MODATA Dport 		/* modem data output port */

char bios();
#define POREADY bios(15,0)

#define CIREADY (inp(CSTAT)&CIMASK)
#define CICHAR (inp(CDATA))

#define COREADY (inp(CSTAT)&COMASK)

/* STDIO file included here to simplify cross-compiles of cyams */
#include "a:bdscio.h"
STATLINE	/* do special status line information */
#define 