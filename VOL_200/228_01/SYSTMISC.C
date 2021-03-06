/*
HEADER:         CUGXXX;
TITLE:          (MS-DOS) System-independent sound generation;
DATE:           10-30-84;
DESCRIPTION:    System functions to create sounds using system time/date;
KEYWORDS:       Sound generation, system functions;
FILENAME:       SYSTMISC.C;
WARNINGS:       IBM-PC and close compatibles;
AUTHORS:        Garth Kennedy;
COMPILER:       Lattice C V2.12;
REFERENCES:     US-DISK 1310;
ENDREF
*/
/**
* This is a collection of system functions that get the
* system time and date and create a sound from the internal
* speaker. This is written in the Lattice 2.12 compiler
* and is taken from a larger program
*
* Garth Kennedy    Hinsdale, Ill    9 Nov 1984
**/


int tme[10];		       /* time date array */
/**
*		 tme[0] - day of week (0-6) (sun- sat)
*		 tme[1] - hours (0-23)
*		 tme[2] - minutes (0-59)
*		 tme[3] - Seconds (0-59)
*		 tme[4] - 100s of seconds (0-99)
*		 tme[6] - year (1980 - 2099)
*		 tme[7] - month (1-12)
*		 tme[8] - day of month (1-31)
**/

/**
* sound(freq,tlen)
*
* set the frequency of the speaker
* the passed parameter <freq> is the frequency in Hz
*		       <tlen> is the duration of the sound
*		       each increment of <tlen> is approximately
*		       20 ms on my IBM PC-I
*  had to use machine locations here - couldnt see how to get at
*  this within DOS.
*
* Garth Kennedy   30 Oct 1984
*
**/

sound(freq,tlen)
int freq;	  /* freq of sound in Hertz */
int tlen;	  /* duration of sound arbitrary units	*/
{
    unsigned frdiv = 1331000L/freq;   /* timer divisor */
    int i,j,k;

    outp(0x43,0xB6);	       /* write timer mode register */
    outp(0x42,frdiv & 0x00FF);	/* write divisor LSB */
    outp(0x42,frdiv >> 8);	/* write divisor MSB */

    i = inp(0x61);		/* get current port B setting */
    outp(0x61,i | 0x03);	/* turn speaker on */
    for ( j = 0;j <= tlen; j++)
    {				/* loop till timed out */
	for (k = 0; k < 1000;k++)
	;
    }
    outp(0x61,i);		/* restore port B setting */
				/* turn speaker off */
    return;
}

/**/
/**
* gtime(tme)
*
* Get the time from the system
* return time in the integer array tme[]
*		 tme[0] - day of week (0-6) (sun- sat)
*		 tme[1] - hours (0-23)
*		 tme[2] - minutes (0-59)
*		 tme[3] - Seconds (0-59)
*		 tme[4] - 100s of seconds (0-99)
*
* Garth Kennedy  31 Oct 1984
**/
gtime(tme)
int tme[];
{
    int intno = 0x21;		  /* interrupt number (DOS Function Call) */
    union REGS inregs;		  /* input registers */
    union REGS outregs; 	  /* output registers */

    inregs.h.ah = 0x2C; 	  /* function number for get time */
    int86(intno,&inregs,&outregs);   /* make function call */
    tme[0] = outregs.h.al;	     /* day of the week */
    tme[1] = outregs.h.ch;	     /* hours */
    tme[2] = outregs.h.cl;	     /* minutes */
    tme[3] = outregs.h.dh;	     /* seconds */
    tme[4] = outregs.h.dl;	     /* 1/100 seconds */
    return;
}
/**
* gdate()
*
* Get the date from the system
* return date in the integer array tme[]
*		 tme[6] -                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                