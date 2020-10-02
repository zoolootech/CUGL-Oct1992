
/*
>>:yamsys.h 9-01-81
 *
 * global equates for specific installation and modem ports
 * Other modem specific stuff is in yam5.c
 *
 */

/* files have single letter ext so pip yam?????.? gets all source but no crl */
#define HELPFILE "YAMHELP.T"
#define PHONES "YAMPHONE.T"
#define CPM
#define CDOS	/* a variant of cp/m, perhaps better, certainly different! */
#define BDSC
char inp();		/* for tighter compiled code ... */
#define CLKMHZ 4
#define SECPBLK 8	/* standard 1k blocks on 4fdc */
/* ********* following string must be in UPPER case ********* */
#define DISKS "AB"	/* legal disks for default selection */
#define MAXUSER 1

#define T4014		/* for the big flash ... */
/* defines for TU-ART board with modem port at 0x60 */
#define TUART				/* type of modem port */
#define DEFBAUD 19200	/* initial baud rate setting */
#define SPORT 0x60
#define DPORT 0x61
#define MIREADY (inp(Sport)&0100)	/* value != 0 if char available */
#define MIERROR (inp(Sport)&03)	/* != 0 if any error condx */
#define MICHAR (inp(Dport))		/* get char assuming miready */

#define MOREADY (inp(Sport)&0200)	/* modem ready to load next char */
/* It would be nice to have parameterized macros to do the following */
#define MODATA (Dport)			/* modem data output port */

char bios();
#define POREADY bios(15,0)

#define CIREADY (inp(CSTAT)&CIMASK)
#define CICHAR (inp(CDATA))

#define COREADY (inp(CSTAT)&COMASK)

#include "bdsciotu.h"
ine CLKMHZ 4
#define SECPBLK 8	/* standard 1k blocks on 4fdc */
/*