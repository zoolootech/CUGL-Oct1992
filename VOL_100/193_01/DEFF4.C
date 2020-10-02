/*-----------------------------------------------------------------*/
/*  LINKING FUNCTIONS  ==>  DEFF4                                  */
/*  Some additional functions to be used with CLINK when turning   */
/*  a CRL programme into a COM programme with BDS C.               */
/*                                                                 */
/*  This file contains the following functions -                   */
/*                                                                 */
/*       SET_BIT        PUT_ERROR           UP_STR                 */
/*       LOWER_STR      ENTAB               PLIST                  */
/*       LIST_STATUS    LISTS               LISTD                  */
/*       MAKE_FCB       PEC_CLEAR                                  */
/*                                                                 */
/*  all of which are described in the file DEFF4.TXT.              */
/*                                                                 */
/*  WRITTEN:  14th July, 1986                                      */
/*  -------                                                        */
/*  Copyright 1986 by Cogar Computer Services Pty. Ltd.            */
/*  All rights reserved.                                           */
/*-----------------------------------------------------------------*/
#include <pec.h>	/* Required for these functions            */
/*=================================================================*/
/*  SUBROUTINE  ==>  set_bit                                       */
/*  Will set the nominated BYTE in the file control block          */
/*  Choices are -                                                  */
/*                 R/O       Read only                             */
/*                 R/W       Read/Write (re-sets the bit)          */
/*                 SYS       System file                           */
/*                 DIR       Directory (re-sets SYS bit)           */
/*                 ARC       Archive bit                           */
/*                 CPY       Copy (re-sets Archive bit)            */
/*-----------------------------------------------------------------*/

set_bit(fcb_buf,str)
char fcb_buf[36], str[3];
{
	up_str(str);	/* convert to upper case */

	if(strcmp(str,"R/O") == 0)
		fcb_buf[9] = fcb_buf[9] | 0x80;
	else if(strcmp(str,"R/W") == 0)
		fcb_buf[9] = fcb_buf[9] & 0x7f;
	else if(strcmp(str,"SYS") == 0)
		fcb_buf[10] = fcb_buf[10] | 0x80;
	else if(strcmp(str,"DIR") == 0)
		fcb_buf[10] = fcb_buf[10] & 0x7f;
	else if(strcmp(str,"ARC") == 0)
		fcb_buf[11] = fcb_buf[11] | 0x80;
	else if(strcmp(str,"CPY") == 0)
		fcb_buf[11] = fcb_buf[11] & 0x7f;
	else put_error(str);

	if(set_attributes(fcb_buf) == -1)
		printf("\nUnable to find the nominated file.\n");
}
/*-----------------------------------------------------------------*/
/*  SUBROUTINE  ==>  put_error                                     */
/*  Used with set_bit                                              */
/*-----------------------------------------------------------------*/

put_error(string)
char *string;
{
printf("\n\nThe mode  %s is unknown...terminating programme.", string);
exit();
}
/*-----------------------------------------------------------------*/
/*  LIBRARY FILE  ==  up_str                                       */
/*  Will convert a string to UPPER case ASCII                      */
/*-----------------------------------------------------------------*/

up_str(s)
char *s;
{
	short i;
	i = 0;
	
	while(s[i])
	{
		s[i] = toupper(s[i]);
		i++;
	}
	s[i] = '\0';
}
/*-----------------------------------------------------------------*/
/*  LIBRARY FILE  ==  lower_str                                    */
/*  Will convert a NULL-terminated string to ASCII lower case      */
/*-----------------------------------------------------------------*/

lower_str(s)
char *s;
{
	short i;
	i = 0;

	while(s[i])
	{
		s[i] = tolower(s[i]);
		i++;
	}
	s[i] = '\0';
}
/*-----------------------------------------------------------------*/
/*  LIBRARY FILE  ==>  ENTAB                                       */
/*  Will compensate for TAB characters on those printers which     */
/*  don't recognise the ASCII tab.   Can be altered for different  */
/*  tab sizes, as needed.                                          */
/*-----------------------------------------------------------------*/

entab(position, size)
short position, size;
{
	short tab_count;

	for(tab_count = 0; tab_count <= size - position%size; tab_count++)
		listc(SPACE);
	tab_count--;	/* Must go back one                        */

	position = position + tab_count;
	return(position);
}
/*-----------------------------------------------------------------*/
/*  LIBRARY FILE  ==>  plist                                       */
/*  Will list one character to the line printer using the actual   */
/*  I/O port addresses.                                            */
/*  Presently configured for GODBOUT INTERFACER II board but can   */
/*  be modified for any "Centronics" connection provided the       */
/*  ports and the strobe signals are known.                        */
/*  NOTE:  The strobe signals used are those for the Itoh 8510     */
/*  ----   printer.                                                */
/*                                                                 */
/*  Written for BDS C but should be portable to other C's.         */
/*  Copyright 1986 by Cogar Computer Services Pty. Ltd.            */
/*-----------------------------------------------------------------*/
/*      INTERFACER II EQUATES                                      */
/*-----------------------------------------------------------------*/
#define BASE_PORT 0x0c8		/* Interfacer II printer ports     */
#define HAND_SHAKE BASE_PORT + 1	/* Handshaking port        */
#define LIST_PORT HAND_SHAKE + 1	/* Printer data port       */
/*-----------------------------------------------------------------*/
/*      ITOH 8510 PRINTER EQUATES                                  */
/*-----------------------------------------------------------------*/
#define OFF_STROBE 0x0ff	/* To turn all strobes off         */
#define IN_DATA    0x0bf   	/* Active, data input signal       */
#define BUSY_INPUT 0x07f	/* Active, input busy signal       */
#define BUSY       0x080	/* Printer busy signal             */
#define DELAY      100		/* 1 millisecond delay ?           */
#define LOOPS      1000		/* To do BUSY test                 */
/*-----------------------------------------------------------------*/

plist(c)
int c;				/* The character to list           */
{
	short i;			/* Delay counter                   */

	if(list_status() != TRUE)  /* Test the printer status      */
	{
		printf("\nPrinter must be turned off.");
		exit();
	}
	else outp(LIST_PORT,c);  /* OK, so print character         */
	outp(HAND_SHAKE,IN_DATA); /* Tell printer we've sent it    */
	outp(HAND_SHAKE,OFF_STROBE); /* Sequence finished          */
/*-----------------------------------------------------------------*/
/*  We now have to wait 1 millisec until the printer can accept    */
/*  more data.   I don't know how to get a 1 millisec delay in     */
/*  BDS C, as the smallest allowable delay is 1/30 = 33.3 millisec */
/*  for a 6 mHz clock, if we use the "sleep" function.   So what   */
/*  I have done is just to use an empty loop until I got the delay */
/*  approximately right for this printer.                          */
/*-----------------------------------------------------------------*/
	for(i = 0; i < DELAY; i++)
		;			/* Empty loop              */

}
/*-----------------------------------------------------------------*/

list_status()
{
	short i, n;
	i = n = 0;		/* Has to be changed if successful */
	while(i < LOOPS && n != 1)
	{
	outp(HAND_SHAKE, BUSY_INPUT); /* Ask what the status is    */
	outp(HAND_SHAKE, OFF_STROBE); /* End of enquiry            */
	if(inp(LIST_PORT) != 0)	        /* Then printer is busy    */
		 i++;		        /* So do it again          */
	else n = 1;			/* Not busy, so tell us    */
	}
	return(n);
}
/*-----------------------------------------------------------------*/
/*---------------END OF PLIST--------------------------------------*/
/*-----------------------------------------------------------------*/
/*  LIBRARY FILE  ==>  LISTS                                       */
/*  Will transmit a null-terminated string to the line printer.    */
/*  Uses function ==>  listc  -  from DEFF3                        */
/*-----------------------------------------------------------------*/
lists(str)	/* To list a null-terminated string                */
char *str;
{
	char c;
	
	while(c = *str++)
		listc(c);
}
/*-----------------------------------------------------------------*/
/*  LIBRARY FILE  ==>  LISTD                                       */
/*  The listing (printer) equivalent of print_dec in DEFF3.        */
/*  Will print out a decimal number in the listing.                */
/*-----------------------------------------------------------------*/
listd(count)	/* To list a decimal number                        */
short count;
{
	short i;		/* Counter                                 */

	if(count < 0)
	{
		listc('-');	/* For negative number             */
		count = -count; /* Make number positive            */
	}

	i = 0;		/* Starting condition                      */
	if((i = count/10) != 0)
		listd(i);	/* Recursive call                  */
	listc(count % 10 + '0'); /* Make the digit printable       */
}
/*-----------------------------------------------------------------*/
/*  SUBROUTINE  ==>  make_fcb                                      */
/*  Needs to know -                                                */
/*                   where to look for the name                    */
/*                   where name is to go  (fcb)                    */
/*-----------------------------------------------------------------*/

make_fcb(name,fcb)
char  name[15], fcb[36];
{
	char c;
	short i, j, count;

/*-----------------------------------------------------------------*/
/*  Note this routine DOESN'T set the drive name in the file       */
/*  control block.   You must do that separately.                  */
/*-----------------------------------------------------------------*/
	if(name[2] == ':')
		i = 2;	/* Then skip over drive name               */
	else i = 0;	/* Or there is no drive name               */

	count = 0;
	
	for (j = 1; j < 9; j++)	/* Do the "name" part              */
	{
		if((c = name[i]) != '.')
		{
			fcb[j] = c;
			i++;
		}
		else if((c = name[i]) == '.')
		{
			fcb[j] = SPACE;
		}
	}
	if (name[i] == '.')
		i++;		/* Skip over the period            */
		
	for (j =9; j < 12; j++)	/* Now do the "type" part          */
	{
	if (isalnum(name[i]) == TRUE)	/* A character             */
		{
			fcb[j] = name[i];
			i++;
		}
	else fcb[j] = SPACE;
	}
}
/*-----------------------------------------------------------------*/
/*  UTILITY:       pec_clear                                       */
/*  Will clear the screen of any terminal without using any of     */
/*  the special terminal functions.                                */
/*-----------------------------------------------------------------*/

pec_clear()
{
	short i;

	for(i = 0; i < 25; i++)
		putchar('\n');
}
/*-----------------------------------------------------------------*/= 0)
		l