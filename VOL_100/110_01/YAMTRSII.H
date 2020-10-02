/*
>>:yamsys.h 9-06-81
 *
 * global equates for specific installation and modem ports
 * Other modem specific stuff is in yam5.c
********* Version for TRS or other unit.  Check definitions carefully, I am
gussing here!!!   please upload this file when corrected CAF
 *
 */

/* files have single letter ext so pip yam?????.? gets all source but no crl */
#define HELPFILE "B:YAMHELP.T"
#define PHONES "A:YAMPHONE.T"

#define CPM
#define BDSC
#define CLKMHZ 4
#define SECPBLK 16	/* 2k blocks on Magnolia DD controller 8" disk */
/* ********* following string must be in UPPER case ********* */
#define DISKS "ABEFIJ"	/* legal disks for default selection */
#define MAXUSER 0	/* maximum user number */

#define DEFBAUD 1200	/* initial baud rate setting */
#define DPORT 0320
#define SPORT 0325

char inp();				/* for fastest 8080 code */
#define MIREADY (inp(Sport)&1)		/* value != 0 if char available */
#define MIERROR FALSE			/* != 0 if any error condx */
#define MICHAR (inp(Dport))		/* get char assuming miready */

#define MOREADY (inp(Sport)&040)	/* modem ready to load next char */
/* It would be nice to have parameterized macros to do the following */
#define MODATA Dport 			/* modem data output port */

char bios();
#define POREADY bios(15,0)

#define CIREADY (bios(2,0))
#define CICHAR (bios(3,0))

#define COREADY TRUE

/* STDIO file included here to simplify cross-compiles of cyams */
#include "a:bdscio.h"
#define BDSC
#define CLKMHZ 4
#define SECPBLK 16	/* 2k blocks on Ma