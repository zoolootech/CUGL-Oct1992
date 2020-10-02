/*
$title ('yam5.c: user specific routines')
$date (21 OCT 85)
*/
/*
  basic low-level modem functions
*/

#include <dos.h>
#include "yam.h"
char portmode;		/* baud, parity, stop bit configuration byte */
unsigned io_config; /* hardware config byte */

	/* offsets to status ports */
#define ierport Dport+1
#define iicport Dport+2
#define lcrport Dport+3
#define mcrport Dport+4
#define mcsport Dport+6

#define serialio 0x14
#define get_io_config 0x11

/* serial equates for int 14h in MS-DOS, IBM-PC, Zenith Z-160 */
#define baudmask 0xe0  /* baud rate mask */
#define B110 0
#define B150 0x20
#define B300 0x40
#define B600 0x60
#define B1200 0x80
#define B2400 0xA0
#define B4800 0xC0
#define B9600 0xE0
#define wlenmask 0x3   /* word length mask */
#define len7 2
#define len8 3
#define parmask 0x18   /* mask for parity bits */
#define nopar  0
#define even   0x18
#define odd    0x8
#define stopmask 0x4   /* mask for stop bits */
#define stop1  0x0
#define stop2  0x4

/* modem command equates for 8251 */
#define dtr 0x01
#define rts 0x02

#define l6mode (len7 | even | stop2)
#define normode (len8 | stop1 | nopar)

/****************************************************************************
FUNCTION:
	send initialzation  sequence to CRT.  Some people like the screen to
	be cleared, etc.

CALLING PARAMETERS:
	none.
===========================================================================*/
terminit()
{
} /* terminit */


/****************************************************************************
FUNCTION:
	change modem port to alternate modem port.  Only ports 0 and 1 are 
	supported as interrupts must be available for the driver.

CALLING PARAMETERS:
	port:
		number of port to change to.  For IBM PC, this is 0 or 1
===========================================================================*/
chngport(port)
int port;
{
		/* first check to see if port exists.  bits 9-11 of confuration
		   says how many ports in system */
	if ( (port > ( (io_config>>9)&0x7)-1) || (port > 1) )
	{
		printf("port not availablet\n");
		return ERROR;
	}
	else
	{
			/* disable current port */
		dsblcm();
		resport(commport);

			/* set new port */
		switch(port)
		{
			case 1:
				Dport = 0x2f8;
				commport = 1;
				break;
			default:
				Dport = 0x3f8;
				commport = 0;
		}
	}
		/* enable new port */
	setport(commport);
	enblcm(0);
	return OK;
} /* chngport */


/****************************************************************************
FUNCTION:
	peform special user init routines.  Called from INIT() at load time
	with resetflag false.  Called from RESET with resetflag true.  If reset
	flag is true, the port will be reset to the last value as defined by set
	baud, or initialization.  This allows the port to be reset and the current
	state of the port displayed.  Some systems allow configuration of the
	serial port to be read.   IBM does not, so to retain IBM compatiblity
	that data is assumed unavailable.

CALLING PARAMETERS:
	resetflag:
		if true, serial port and modem will be initialzed to default condition.
===========================================================================*/
userinit(resetflag)
int resetflag;
{
	union REGS inregs;
	union REGS outregs;

		/* get i/o configuration byte */
	int86(get_io_config,&inregs,&outregs);
	io_config = outregs.x.ax;

		/* reset port mode */
	if (resetflag)
	{
			/* baudrate is already set to a default value */
		setbaud(Baudrate);

			/* modem will not accept command unless TR is true(terminal
			   ready) */
		offhook();

			/* set modem configuration:
				wait 100 seconds for carrier
				answer on first ring
				enable connect 1200 message,
				echo result codes
			*/
		sendstring("ATS7=100 S0=1 X1 Q0\r");

			/* slight delay needed after commands */
		sleep(5);
						
			/* set back on hook so will not answer phone */
		onhook();
	}

} /* userinit */


/****************************************************************************
FUNCTION:
	perform user required clean up before exit

CALLING PARAMETERS:
	none.
===========================================================================*/
userexit()
{
		/* reset comm port interrupt */
	resport(commport);
		/* disable interrupts */
	dsblcm();
		/* reset interrupt vectors */
	res_comm();
}


/****************************************************************************
FUNCTION:
	set serial port mode.  Baud rate, start, stop and parity are set
	via interrupt 14h.

CALLING PARAMETERS:
	mode:
		mode byte as defined in int 14h.
===========================================================================*/
setmode(mode)
char mode;
{
	union REGS inregs;
	union REGS outregs;

		/* dx contains port number */
	inregs.x.dx = commport;

		/* this routine always does a set command */
	inregs.h.ah = 0;

		/* set mode */
	inregs.h.al = mode;

		/* send command thru bios */
	int86(serialio,&inregs,&outregs);
	if (outregs.h.ah & 0x80)
		printf("MODEM PORT TIMEOUT STATUS= %x \007\n",outregs.x.ax);

} /* setmode */


/****************************************************************************
FUNCTION:
	set baud rate for serial port.  This routine uses the monitor setup
	routine, which should be compatible with most IBM PC's.

CALLING PARAMETERS:
	baud:
		unsigned integer representing baud rate desired.
===========================================================================*/
setbaud(baud)
int baud;
{
	unsigned ratediv;

	Baudrate = baud;
	switch(baud)
	{
		case 110:
			ratediv = B110;
			break;
		case 150:
			ratediv = B150;
			break;
		case 300:
			ratediv = B300;
			break;
		case 600:
			ratediv = B600;
			break;
		case 1200:
			ratediv = B1200;
			break;
		case 2400:
			ratediv = B2400;
			break;
		case 4800:
			ratediv = B4800;
			break;
		case 9600:
			ratediv = B9600;
			break;
		default:   /* use existing rate */
			Baudrate=readbaud();
			ratediv = portmode & baudmask;
		}

		/* set serial port baud rate */
	portmode = ((portmode & ~baudmask) | ratediv);
	setmode(portmode);

	display_status();
} /* setbaud */


/****************************************************************************
FUNCTION:
	print modem status to console.

CALLING PARAMETERS:
	none.
===========================================================================*/
display_status()
{
	char mode;

	mode = portmode & 0x1f;

	if (mode == l6mode)
		printf("Level 6");
	else if (mode == normode)
		printf("normal");
	else
		printf("special");
	printf(" mode\n");

	printf("Baud rate = %d\n",readbaud());
} /* display_status */


/****************************************************************************
FUNCTION:
	send break to modem if break key is pressed.

CALLING PARAMETERS:
	none.
===========================================================================*/
sendbrk()
{
	char lcrval;

		/* send break command */
	lcrval = inp(lcrport); 
	outp(lcrport, (lcrval | 0x40) );
	sleep(10);
	outp(lcrport,lcrval);
} /* sendbrk */


/****************************************************************************
FUNCTION:
	allow user to change port configuration manually.  This routine builds
	a string which is then used to call setparams to set port mode.

CALLING PARAMETERS:
	none
===========================================================================*/
changecnfg()
{
	char option;

		/* mask of baud rate bits */
	portmode = portmode & baudmask;

	printf("stop bits (1/2)? ");
	option=getopt();
	switch (option)
	{
		case '1':
			portmode = portmode | stop1;
			break;
		case '2':
		default:
			portmode = portmode | stop2;
	}	

	printf("\nparity (E/O/N)? ");
	option=getopt();
	switch (option)
	{
		case 'e':
			portmode = portmode | even;
			break;
		case 'o':
			portmode = portmode | odd;
			break;
		case 'n':
		default:
			portmode = portmode | nopar;
	}	

	printf("\ndata bits (7/8)? ");
	option=getopt();
	switch (option)
	{
		case '7':
			portmode = portmode | len7;
			break;
		case '8':
		default:
			portmode = portmode | len8;
	}	
	printf("\n\n");

		/* change port mode */
	setmode(portmode);
	display_status();

} /* changecnfg */


/****************************************************************************
FUNCTION:
	returns index of findchar in string at sptr.  returns 0xffff if not
	found.

CALLING PARAMETERS:
	*sptr:
		pointer to source string.
	findchar:
		character to search for.
===========================================================================*/
int findb(sptr,findchar)
char *sptr;
char findchar;

{
	int i,j;

	j = strlen(sptr);
	for (i = 0; i < j; i++)
	{
		if (sptr[i] == findchar)
			return i;
	}
	return 0xffff;
} /* findb */


/****************************************************************************
FUNCTION:
	set transmission parameters, ie baud rate, # bits, parity.  These
	parameters are specified in the phone list with:
			S# #stop bits
			PX parity--E=even, N=none, O=odd 
			D# #data bits
	It expects the pointer to be set to the beginning of the data field, which
	should be the phone #.

CALLING PARAMETERS:
	*strptr:
		pointer to string containing serial port configuration information.
===========================================================================*/
setparams(strptr)
char *strptr;
{
	int offset;

		/* skip past phone # to baud rate if present */
	if ( (offset = findb(strptr,'b')) != 0xffff)
	{
		while (*strptr++!='b');
		Baudrate = atoi(strptr);
	}

		/* skip to configuration string */
	while (!isspace(*++strptr));

		/* set stop bits */
	if((offset = findb(strptr,'S')) != 0xffff)
	{		   
		portmode = portmode & ~stopmask;
		switch(strptr[offset+1])
		{
			case '1':
				portmode = portmode | stop1;
				break;
			case '2':
				portmode = portmode | stop2;
			default:
		}
	}

		/* set parity */
	if((offset = findb(strptr,'P')) != 0xffff)
	{		   
		portmode = portmode & ~parmask;
		switch(strptr[offset+1])
		{
			case 'E':
				portmode = portmode | even;
				break;
			case 'O':
				portmode = portmode | odd;
				break;
			case 'N':
			default:
				portmode = portmode | nopar;
		}
	}

		/* set data bits */
	if((offset = findb(strptr,'D')) != 0xffff)
	{		   
		portmode = portmode & ~wlenmask;
		switch(strptr[offset+1])
		{
			case '7':
				portmode = portmode | len7;
				break;
			case '8':
				portmode = portmode | len8;
			default:
		}
	}

		/* set special flags.  Ignrx causes rxnono string test to be skipped
		   allowing special characters to be recieved */
	if( (offset = findb(strptr,'I')) != 0xffff)
		Ignrx = TRUE;
	else
		Ignrx = FALSE;

		/* this will set the baud and the port mode */
	setbaud(Baudrate);

		/* call to setbaud will disable interrupts */
	enblcm(0);

		/* pause after reset */
	sleep(10);
} /* setparams */


/****************************************************************************
FUNCTION:
	place phone on hook by dropping DTR, causing modem to hang up.

CALLING PARAMETERS:
	none.
===========================================================================*/
onhook()
{
	char mcval;

	mcval = inp(mcrport);

		/* drop DTR to make modem hangup */
	outp(mcrport, (mcval &(~dtr)) );

		/* wait till DCE goes low */
	sleep (3);
	printf("On Hook");

} /* onhook */


/****************************************************************************
FUNCTION:
	set DTR to modem to allow auto answer, etc.  This should
	set TR to modem

CALLING PARAMETERS:
	none.
===========================================================================*/
offhook()
{
	outp(mcrport, (inp(mcrport) | dtr) );
} /* offhook */



/****************************************************************************
FUNCTION:
	read a char from the modem.  This routine is used by the file tranfer
	routines.

CALLING PARAMETERS:
	decisecs:
		number of loops to wait for character

RETURNED PARAMETERS:
	integer value of char read.  value will be negative if an error occurred.
===========================================================================*/
readbyte(decisecs)
int decisecs;
{
	if(MIREADY)
		return MICHAR;
	while(--decisecs>=0)
	{
		if(MIREADY)
			return MICHAR;
		if(CDO)
			return TIMEOUT;
		if(MIREADY)
			return MICHAR;
		if(CIREADY)
		{
			CICHAR;	 			/* dismiss character */
			return TIMEOUT;
		}
		if(MIREADY)
			return MICHAR;

		for(Timeout=T1pause; --Timeout;)
			if(MIREADY)
				return MICHAR;
	}
	return TIMEOUT;
}


/****************************************************************************
FUNCTION:
	send a string of characters to modem.

CALLING PARAMETERS:
	char *sptr:
		pointer to string of characters to send to modem.
===========================================================================*/
sendstring(sptr)
char *sptr;
{
	while(*sptr != '\0')
	{
		sendbyte(*sptr);
		sptr++;	 
	}		   
} /* sendstring */		   

/****************************************************************************
FUNCTION:
	send a char to modem.  This routine is used by the file transfer routines

CALLING PARAMETERS:
	data:
		char to send to modem
===========================================================================*/
sendbyte(data)
char data;

{
	while(!MOREADY)
		;
	MOCHAR(data);
} /* sendbyte */


/****************************************************************************
FUNCTION:
	throw away any characters input from modem.

CALLING PARAMETERS:
	none.
===========================================================================*/
purgeline()
{
	while(MIREADY)
		MICHAR;
}


/****************************************************************************
FUNCTION:
	read the baud rate from the configuration byte.

CALLING PARAMETERS:
	none

RETRUNED PARAMETERS:
	unsigned value of baud rate.
===========================================================================*/
readbaud()
{
		/* calculate baud stored in byte 5-7 */
	switch(portmode & baudmask)
	{
		case B110:
			return 110;
		case B150:
			return 150;
		case B300:
			return 300;
		case B600:
			return 600;
		case B1200:
			return 1200;
		case B2400:
			return 2400;
		case B4800:
			return 4800;
		case B9600:
			return 9600;
		default:
			printf("illegal baud rate\n");
			return 0xffff;
	}
} /* readbaud */



/****************************************************************************
FUNCTION:
	dial phone number in phone list.  A pointer to the line containing the
	phone number is passed to this routine.  The phone number must be separated
	from the rest of text by a tab.  Also contained in the line are
	configuration parmaters for baud rate, num of start and stop bits, etc.

CALLING PARAMETERS:
	char *phoneptr:
		pointer to string containing phone # to dial
===========================================================================*/
dial(phoneptr)
char *phoneptr;
{

		/* display number */
	printf("%s",phoneptr);

		/* skip to number which is marked by tab character */
	while (*phoneptr != '\t')
		phoneptr++;

		/* set transmission params */
	setparams(phoneptr);

		/* dial the phone number */
	dial_number(phoneptr);

}/* dial */


/****************************************************************************
FUNCTION:
	dial phone number pointed to by phoneptr.

CALLING PARAMETERS:
	char *phoneptr:
		pointer to string containing phone # to dial
===========================================================================*/
dial_number(phoneptr)
char *phoneptr;
{
		/* find number, if none just quit */
	while (!isdigit(*phoneptr))
	{
		if ((*phoneptr == '\n') || (*phoneptr == '\0'))
			return;
		phoneptr++;
	}

		/* enable modem */
	offhook();

		/* disable interrupts so command is not captured */
	dsblcm();

		/* send dial command to modem, use adaptive dial */
	sendstring("ATD");

		/* send number to modem until character other than
		   number or '-' found */
	while ((isdigit(*phoneptr)) || (*phoneptr == '-'))
		sendbyte(*phoneptr++);

		/* terminate command with a <CR> */
	sendbyte('\r');

		/* wait for all of command to be accepted by modem to elimate
		   partial display on screen */
	sleep(10);

		/* re-enable interrupts */
	enblcm(0);
}/* dial_number */
