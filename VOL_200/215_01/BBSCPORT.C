/*
	bbscport.c

	Support routines used by BBSc.c to access the modem port.
	Used with the Godbout Interfacer IV I/O board.
				Mike Kelly

	03/09/83 v1.0	written
	07/08/83 v1.0	Added code to look for modem carrier detect,
			and exit() if not still connected.
*/

#include "bdscio.h"
#include "bbscdef.h"


#define LASTDATE  " 07/08/83 "

#define PGMNAME "BBSCPORT "
#define VERSION " 1.0 "


getuser()		/* inquires for which user # to use */
{
	int	user;

	printf("\nWhich user # to communicate with?  ==>");
	while((user = getchar()) != '5' && user != '6' && user != '7')
	{
		printf("\b \b");	/* backspace,space,backspace */
	}
	putchar('\n');
	user -= 48;	/* adjust from ascii */
	portinit(user);	/* init i/o board */
}
/*	end of function		*/



portinstat()	/*  returns 1 if no char, 0 if char waiting */
{
	char byte0;

	if (offline)
	{
		byte0 = inp(LCSTAT);	/* get local console status */
	}
	else
	{
		byte0 = inp(GBI4PSTAT);	/* get a status byte from port */
		portcarrier(byte0);	/* check carrier status */
	}

	if ((byte0 & SDAV) == SDAV)	/* mask all but char available */
	{
		return(0);	/* char waiting, so get out */
	}
	else
	{
		return(1);	/* no char waiting */
	}
}
/*	end of function		*/
  
char portin()		/* get one byte from the port */
{
	char byte;

	if (offline)
	{
		byte = toupper(inp(LCDATA));
	}
	else
	{
  		byte = toupper(inp(GBI4PDATA));
	}

#ifdef DEBUG
	if (debug)
	{
		savepos();
		to(25,12);
		printf("byte in=%02x",byte);
		retpos();
	}
#endif

	return(byte);
}
/*	end of function		*/

portsin(buf,max)	/* get a line of input max. chars long */
int	max;
char	*buf;
{
	int	cnt,
		byte;
	char	bytex;

	cnt = 0;

#ifdef DEBUG
	if (debug)
	{
		printf("in portsin\n");
		printf("  max=%05d  cnt=%05d\n",max,cnt);
	}
#endif

	byte = FALSE;
	while (++cnt <= max && byte != '\r')
	{
		while(portinstat());
		while((byte = portin()) < ' ' || byte > '}')
		{
			if (byte == '\r')	/* carriage return */
			{
				break;
			}
			if (byte == '\b' && cnt > 1)	/* backspace */
			{
				while (portoutstat());
				portout(byte);
				while (portoutstat());
				portout(' ');
				while (portoutstat());
				portout(byte);
				*buf--;	/* backout last char */
				cnt--;	/* decrement count too */
			}
			while(portinstat());
		}
		if (byte != '\r')
		{
			*buf++ = byte;
		}
		portout(byte);	/* echo good chars only */
	}

	*buf++	= '\0';			/* tag \0 on end */

#ifdef DEBUG
	if (debug)
	{
		printf("  cnt=%05d\n",cnt);
	}
#endif
}
/*	end of function		*/

portoutstat()			/* returns 0 if buffer empty, 1 if not empty */
{
	char byte0;

		byte0 = inp(GBI4PSTAT);	/* get status from port */
		portcarrier(byte0);	/* check carrier status */
#ifdef DEBUG
/*		if (debug)
		{
			if (statcnt++ > STATMAX)
			{
				savepos();
				to(25,23);
				printf("stat out=%02x",byte0);
				retpos();
			}
		}
*/
#endif
		if ((byte0 & SEMPTY) == SEMPTY)	/* mask all but xmit */
		{				/*  buffer empty bit */
			return(0);	/* buffer empty so get out */
		}
		else
		{
			return(1);	/* buffer not empty */
		}
}
/*	end of function		*/

portout(byte)		/* send one byte to the port */
char byte;		/*  return CTL_K for those times want to check if */
{			/*  the person wants to stop sending using */
	char	byte0;	/*  <ctl>-K or K (see porttype), else return 0 */

#ifdef DEBUG
	if (debug)
	{
		savepos();
		to(25,35);
		printf("byte out=%02x",byte);
		retpos();
	}
#endif

	if (portinstat() == 0)			/* == 0 means char waiting */
	{				
		if ((byte0 = portin()) == 0x13)	/* ctl-S?, then */
		{				/*  simulate xon/xoff */
			while (portinstat());	/* wait for any next char */
			portin();		/* gobble up the char */
		}
		if (byte0 == CTL_K || byte0 == 'K')	/* look for stop */
		{					/*  sending key from */
/*			return(CTL_K);			/*  the port */
*/
			stop_that = TRUE;	/* set switch on */
		}
	}
	outp(GBI4PDATA,byte);	/* send the byte to port */
	outp(LCDATA,byte);	/* to local console too */
	return(OK);			/* all ok */
}
/*	end of function		*/
  
portsout(string)	/* send a string to the port */
char *string;
{
	char byte;

#ifdef DEBUG
	if (debug)
	{
		printf("\nportsout string out=%s\n",string);
	}
#endif
	while (byte = (*string++))
	{
		while (portoutstat());	/* returns false when ready */
  		portout(byte);		/* send one byte at a time */
	}
}
/*	end of function		*/

porttype(buf)		/* type a file to the port */
char	*buf;
{
	char	byte,
		byte2;

	portsout("\r\nType ctl-K or K to skip this\r\n\n");
	while ((byte = getc(buf)) != EOF && byte != CPMEOF)
	{
		while (portoutstat());	/* wait for port to be free */
		portout(byte);
		if (stop_that)		/* received ctl-K or K */
		{
			while(portoutstat());
			portsout("\n\r");
			stop_that = FALSE;	/* reset switch */
			return;		/* nuf's enough */
		}
	}
}
/*	end of function		*/


portinit(user)				/* init godbout IV i/o board */
char	user;
{

#ifdef DEBUG
/*	if (debug)
	{
		savepos();
		to(25,67);
		printf("user=%02x",user);
		retpos();
	}
*/
#endif

	outp(GBI4PSEL,user);		/* tell who is calling */
	outp(GBI4PMODE,0x6e);
	outp(GBI4PMODE,0x75);	        /* 300 baud */
	outp(GBI4PCMD,0x27);
}
/*	end of function		*/

char gobble()				/* gobble up any answer */
{
	int	cnt;

	while (cnt++ < 20)
	{
		portin();
	}
}
/*	end of function		*/

portcarrier(byte0)		/* check modem for carrier */
char	byte0;			/*  exit() if not still connected */
{				/* gets passed port status byte */

	if (offline)		/* don't check if in offline mode */
	{
		return;
	}

	if ((byte0 & SCARRIER) == SCARRIER)
	{
		return;			/* yup, still there */
	}
	exit(0);			/* nope, lost carrier, so get out */
					/*  with some form of gracefulness */
}
/*	end of function		*/


/*	end of program  	*/
