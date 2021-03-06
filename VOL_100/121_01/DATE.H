/*
 * date.h - system defines for the computime clock/calendar board
 *
 * version 1.0	- September 1985
 *	add version number
 *	change header file name to "date.h"
 */

struct time {
	char seconds ;
	char minutes ;
	char hours ;
	} ;

struct date {
	char weekday ;
	char day ;
	char month ;
	char year ;
	} ;

#define CLBASE	0x80
#define CLDATA	CLBASE+1
#define CLADDR	CLBASE+2
#define CLOFF	32
#define CLMASK	0x0F
#define CLOMASK	0x03
#define CLAMPM	0x04
#define CL24HR	0x08
#define CLHOLD	16
#define CLREL	0
#define CLCHAR	47
                                                                              