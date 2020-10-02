/*
	bbsclock.c

	This program will...
		access the Hayes Chronograph using 
		the Godbout Interfacer IV I/O board.

	05-07-83 v1.0	started writing
	06-19-83 v1.0	last updated

*/

#include "bdscio.h"

#define VERSION   "1.0"
#define ESC	  0x1b
#define CR	  0x0d
#define LF	  0x0a
#define CLEAR	  'E'

#define GB4PDATA 0x10			/* data port   */
#define GB4PSTAT GB4PDATA+1		/* status port */
#define GB4PCMD  GB4PDATA+3		/* command port */
#define GB4PSEL  GB4PDATA+7		/* user select port */
#define GB4PMODE GB4PDATA+2		/* mode port */
#define USER      6			/* user number 6=chronograph */

#define SDREADY	  0x80			/* data set ready */
#define SCARRIER  0x40			/* carrier detect */
#define SFRAME    0x20			/* framing error  */
#define SOVERRUN  0x10			/* overrun error  */
#define SDAV	  0x02			/* character waiting */
#define SEMPTY	  0x01			/* transmit buffer empty */

char    gb4in();

char gb4istat()				/* input status routine */
{
	char	byte0,
		byte1;

	byte0 = inp(GB4PSTAT);		/* get status byte from port */
	byte1 = byte0 & SDAV;		/* mask all but char avail. bit */
	if (byte1 == SDAV)
	{
		return(0);		/* char waiting, so get out */
	}
	else
	{
		return(1);		/* no char waiting */
	}

}
  
char gb4in()     		       /* get one byte from the port */
{
	char	byte0;

	outp(GB4PSEL,USER);		/* tell who is calling */
  	byte0 = inp(GB4PDATA);		/* get byte from port */

	return(byte0);
}

char gb4sin(string)			/* get a string from the port */
char *string;
{
	char	byte0;

	int  i;

	i = 0;
	byte0 = 0x00;
	while (byte0 != 0x0d)
	{

  		while (gb4istat());	/* returns when ready */
		byte0 = gb4in();	/* get a byte */
  		*string++ = byte0;
		++i;
  	}
	*string++ = 0x00;
}

gb4out(byte2)			/* send one byte to the port */
char byte2;
{
	outp(GB4PSEL,USER);		/* tell who is calling */
	outp(GB4PDATA,byte2);
}
  
gb4sout(string)				/* send a string to the port */
char *string;
{
	char bytex;

	while (*string)
	{
		bytex = (*string++);

		while (gb4ostat());
  		gb4out(bytex);
	}
}

gb4ostat()			/* returns 0 if buffer empty */
{				/* returns 1 if buffer not empty */
	char	byte0,
		byte1;

	byte0 = inp(GB4PSTAT);
	byte1 = byte0 & SEMPTY;	/* mask all but xmit buffer empty bit */
	if (byte1 == SEMPTY)
	{
		return(0);	/* yup, it's empty */
	}
	else
	{
		return(1);	/* nope, it's not */
	}
}

char gb4init()				/* init godbout IV i/o board */
{
	outp(GB4PSEL,USER);		/* tell who is calling */
	outp(GB4PMODE,0x6e);
	outp(GB4PMODE,0x75);	        /* 300 baud */
	outp(GB4PCMD,0x27);
	gb4sout("ATLC\r");		/* clear line feed option */
	gb4gobble();			/* gobble any response */
	gb4sout("ATVT\r");		/* clear time separator */
	gb4gobble();			/* gobble any response */
	gb4sout("ATVD\r");		/* clear date separator */
	gb4gobble();			/* gobble any response */
}

gb4gobble()
{
	char	byte;

	byte = 0x00;
	while (byte != 0x0d)
	{
		while (gb4istat());
		byte = gb4in();
	}
}


gettime(_time)
char	*_time;
{
	char	newtime[15],
		time0[15],
		*timeptr;

	timeptr = &time0;
	newtime[0] = "\0";
	gb4init();		/* init i/o board */
	gb4sout("ATRT\r");	/* ask for time */
	gb4sin(time0);		/* time in HHMMSSx where x=A(M) or P(M) */

	newtime[0] = (*timeptr++);
	newtime[1] = (*timeptr++);
	newtime[2] = ':';
	newtime[3] = (*timeptr++);
	newtime[4] = (*timeptr++);
	newtime[5] = ':';
	newtime[6] = (*timeptr++);
	newtime[7] = (*timeptr++);
	newtime[8] = ' ';
	newtime[9] = (*timeptr++);
	newtime[10]= 'M';
	newtime[11]= '\0';
		
	strcpy(_time,newtime);	/* copy newtime to returned time */
}	

getdate(_mm,_dd,_yy,_month,_day,_year,_date,_week)
char	*_mm,		/* 2 digit */
	*_dd,		/* 2 digit */
	*_yy,		/* 2 digit */
	*_month,	/* long */
	*_day,		/* long */
	*_year,		/* long */
	*_date,		/* long month day, year */
	*_week;		/* day of week */
{
	char	date1[5],
		date2[30];

	gb4init();		/* init i/o board */
	strcpy(date1,"\0");	/* init strings */
	strcpy(date2,"\0");

	gb4sout("ATRW\r");	/* ask for day of week */
	gb4sin(date2);		/* week is 0=mon,1=tue,... */

	switch (date2[0])
	{
		case '0':
			strcpy(_week,"Monday ");
			break;
		case '1':
			strcpy(_week,"Tuesday ");
			break;
		case '2':
			strcpy(_week,"Wednesday ");
			break;
		case '3':
			strcpy(_week,"Thursday ");
			break;
		case '4':
			strcpy(_week,"Friday ");
			break;
		case '5':
			strcpy(_week,"Saturday ");
			break;
		case '6':
			strcpy(_week,"Sunday ");
			break;
		default:
			strcpy(_week,"Unknown ");
			break;
	}

	strcpy(date2,"\0");

	gb4sout("ATRD\r");	/* ask for the date */
	gb4sin(date2);		/* date is YYMMDD */

	if (date2[2] == '0')	/* if MM < 10 */
	{
		switch (date2[3])
		{
			case '1':
				strcpy(_month,"January ");	
				break;
			case '2':
				strcpy(_month,"February ");	
				break;
			case '3':
				strcpy(_month,"March ");	
				break;
			case '4':
				strcpy(_month,"April ");
				break;
			case '5':
				strcpy(_month,"May ");	
				break;
			case '6':
				strcpy(_month,"June ");
				break;
			case '7':
				strcpy(_month,"July ");	
				break;
			case '8':
				strcpy(_month,"August");
				break;
			case '9':
				strcpy(_month,"September ");
				break;
			default:
				strcpy(_month,"Unknown ");
				break;
		}
	}
	else
	{
		switch(date2[3])	/* MM > 9 */
		{
			case '0':
				strcpy(_month,"October ");
				break;
			case '1':
				strcpy(_month,"November ");
				break;
			case '2':
				strcpy(_month,"December ");
				break;
			default:
				strcpy(_month,"Unknown ");
				break;
		}
	}

	substr(date2,_day,5,2);
	substr(date2,_dd,5,2);

	substr(date2,_yy,1,2);
	strcpy(date1,"19");
	strcat(date1,_yy);
	strcpy(_year,date1);

	substr(date2,_mm,3,2);

	strcpy(_date,"\0");	/* clear date */
	strcat(_date,_month);	/* then concat the month,etc. */
	strcat(_date,_day);
	strcat(_date,", ");
	strcat(_date,_year);
}



/*	t	e	s	t	i	n	g
main(argc,argv)
int	argc;
char	*argv[];


{

        char    answer[15],	       /* hold answer from chrono */
        	byte,
		mm[3],
		dd[3],
		yy[3],
		month[16],
		day[3],
		year[5],
		date[27],
		week[13];		/* day of week */

        int	stat,
	        ans,
		i;

	int	debug;


	debug = FALSE;
	strcpy(mm,"\0");
	strcpy(dd,"\0");
	strcpy(yy,"\0");
	strcpy(month,"\0");
	strcpy(day,"\0");
	strcpy(year,"\0");
	strcpy(date,"\0");
	strcpy(week,"\0");

	gb4init();			/* init i/o board */

	clear();
	printf("\nStarting Setclock\n");
	printf("    Version %s\n",VERSION);
  
	if (argc > 1)
	{
		printf("\nargc=%02u  argc hex=%04x\n",argc,argc);
		for (i = 1; i < argc; i++)
        		printf("argv[%02u]=%s \n",i,argv[i]);
		if (strcmp(*++argv,"DEBUG") == 0)
		{
        		printf("\nDebug mode on!\n\n");
	        	debug = TRUE;
			*--argv;	/* reset pointer */
		}
	}

again:
	printf("asking for time\n");

	gettime(answer);
	printf("\nThe time is %s\n",answer);
	prthex(answer);		/* print in hex */

	getdate(mm,dd,yy,month,day,year,date,week);
	printf("\nThe date is %s\n",date);
	prthex(date);
	printf("mm=%s dd=%s yy=%s month=%s day=%s year=%s date=%s week=%s\n",
		mm,dd,yy,month,day,year,date,week);

}
/*

	t	e	s	t	i	n	g
*/

/*	end of program  	*/
