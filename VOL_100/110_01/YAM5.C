/*
>>:yam5.c 9-03-81
 * Modem related functions. If your modem supports baudrate setting,
 * and hangup, write your own routines here.
 * If your modem supports an autocall, put that routine here also
 * define MODEMSTUFF to suppress default baud rate related functions.
 * define AUTOCALL in your autocall routine to suppress the default
 * which merely prints the phone number and recommended baud rate.
 */

/*
 * setbaud(nbaud) If legal rate, set modem registers and Baudrate
 */
#include "yam.h"

#ifdef TUART
#define MODEMSTUFF
/*
 * set baud rate for modem driven by Cromenco TUART
 */
setbaud(nbaud)
unsigned nbaud;
{
	char command, baudcmd;
	command=0;
	switch(nbaud) {
		case 110: baudcmd=0001;break;
		case 150: baudcmd=0202;break;
		case 300: baudcmd=0204;break;
		case 1200: baudcmd=0210;break;
		case 2400: baudcmd=0220;break;
		case 4800: baudcmd=0240;break;
		case 9600: baudcmd=0300;break;
		case 19200: baudcmd=0220;command=020;break;
		case 38400: baudcmd=0240;command=020; break;
	default:
		return ERROR;
	}
	outp(Sport, baudcmd);
	outp(Sport+2, command);
	Baudrate=nbaud;
	return 0;
}
readbaud()
{
	Baudrate=DEFBAUD;
}

/* Bye disconnects the line and allows another call */
bye()
{
	outp(Sport+2, 03);	/* turn on break */
	sleep(40/CLKMHZ);	/* wait two seconds */
	setbaud(Baudrate);
}

/* onhook disconnects the line for good */
onhook()
{}	/* no way to go on hook with this setup */
#endif
#ifdef Z89
#define MODEMSTUFF
/*
 * Routine to set baud rate for 8250 driven modem port
 */
setbaud(nbaud)
unsigned nbaud;
{
	unsigned bcmd;

	if(nbaud==0)
		return ERROR;
	bcmd= 57600;		/* 1.8432 Mhz clock */
	bcmd /= (nbaud/2);	/* this must be done unsigned! */
	outp(Dport+3, 0203);    /* enable divisor latch */
	outp(Dport, bcmd);
	outp(Dport+1, (bcmd>>8));
	/* 8 data bits, 1 stop bit (2 if 110 baud), disable divisor latch */
	outp(Dport+3, nbaud==110? 07:03);
	/* turn on dtr and rts, also output2 is baudrate is 1200 or more */
	outp(Dport+4, nbaud>=1200?017:03);
	Baudrate=nbaud;
/*
	printf("bcmd=%d bcmdms=0%o bcmdls=0%o\n",bcmd,(bcmd>>8),(bcmd&0377));
*/
	return OK;
}
/* fetch the baudrate from the modem port */
readbaud()
{
	char dp3;
	unsigned div;

	dp3=inp(Dport+3);
	outp(Dport+3, 0200|dp3);
	div= inp(Dport) | (inp(Dport+1)<<8);	/* fetch divisor */
	outp(Dport+3, dp3);	/* restore uart modes */
	Baudrate=57600;		/* be sure all this remains unsigned! */
	Baudrate /= div;
	Baudrate <<= 1;
}

/* Bye hangs up the line and then resets for another call */
bye()
{
	onhook();
	sleep(40/CLKMHZ);
	setbaud(Baudrate);
}

/* onhook goes off line for good */
onhook()
{
	outp(Dport+4, 0);
}

#endif

#ifndef MODEMSTUFF
setbaud(){}
onhook{}
bye(){}
readbaud()
{
	Baudrate=DEFBAUD;
}
#endif

/*
 * Readline from MODEM13.C rewritten to allow much higher
 * baud rates.
 * Timeout is in deciseconds (1/10th's)
 * For top speed, character ready is checked in many places.
 * returns TIMEOUT if kbd character is ready.
 */
readline(decisecs)
{
	if(MIREADY)
		return MICHAR;
	while(--decisecs>=0) {
#ifdef CDO
		if(MIREADY)
			return MICHAR;
		if(CDO)
			return TIMEOUT;
#endif
		if(MIREADY)
			return MICHAR;
		if(CIREADY) {
			CICHAR;		/* dismiss character */
			return TIMEOUT;
		}
		if(MIREADY)
			return MICHAR;
		for(Timeout=T1pause; --Timeout; )
			if(MIREADY)
				return MICHAR;
	}
	return TIMEOUT;
}

sendline(data)
char data;
{
	while(!MOREADY)
		;
	outp(MODATA, data);
}
purgeline()
{
	while(MIREADY)
		MICHAR;
}

/* default "autodial" routine */
#ifndef AUTODIAL
dial(name)
char *name;
{
	char *s;
#ifdef CAFPERSONAL
	Sport=(Dport=208)+(SPORT-DPORT);
#endif
	if((s=cisubstr(name, "\tb")))
		if(!setbaud(atoi(s+2)))
			printf("Baudrate set to %u: ", Baudrate);
	printf("%s", name);
	return OK;
}
#endif
igher
 * baud rates.
 * Timeout is in deci