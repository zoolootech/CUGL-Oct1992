/*-----------------------------------------------------------------*/
/*  FILE:          DEFF4.C
    ----
    COMPILER:      BDS C  V 1.50
    --------
    WRITTEN:       22/10/85
    -------
    REVISED:       16/11/86
    -------
    VERSION:       1.4
    -------
    ADDED FCTs:    B_TEST
    ----------                                                     */

/*  This file contains the following functions -                   */
/*                                                                 */
/*       SET_BIT        PUT_ERROR           UP_STR                 */
/*       LOWER_STR      ENTAB               PLIST                  */
/*       LIST_STATUS    LISTS               LISTD                  */
/*       MAKE_FCB       PEC_CLEAR           HAZ_CLEAR              */
/*       BRIGHT         NORMAL              LINE(S)                */
/*       PRINT_U        GO_TO               OUT_CHAR               */
/*       ISNUM          INKEY               PRT_HEX                */
/*       VIDEO_CHAR     B_TEST                                     */
/*-----------------------------------------------------------------*/
/*            BIOS Functions
              --------------

         W_BOOT         CONST               CONIN
         CONOUT         LIST_C              SELDSK
         SET_TRK        SET_SEC             SET_DMA
         READ_SEC       WRITE_SEC           LISTST                 */
/*-----------------------------------------------------------------*/
/*                                                                 */
/*  all of which are described in the file DEFF4.TXT.              */
/*                                                                 */
/*  WRITTEN:  14th July, 1986                                      */
/*  -------                                                        */
/*  BIOS functions added September 27, 1986                        */
/*  Copyright 1986 by Cogar Computer Services Pty. Ltd.            */
/*  All rights reserved.                                           */
/*-----------------------------------------------------------------*/
#include <pec.h>	/* Required for these functions            */
/*=================================================================*/
/*  LIBRARY FUNCTION:   B_TEST

    Will test the nominated bit in a byte and return TRUE if it
    is set ( = 1) or FALSE if it is not set.   Note that "bit"
    must be one of "B_ZERO" to "B_SEVEN" as per pec.h              */
/*-----------------------------------------------------------------*/

char b_test(byte, bit)
int bit;
char byte;
{
	int set_bit;

	if(bit == 0)
		set_bit = 0x1;
	else if(bit == 1)
		set_bit = 0x2;
	else if(bit == 2)
		set_bit = 0x4;
	else if(bit == 3)
		set_bit = 0x8;
	else if(bit == 4)
		set_bit = 0x10;
	else if(bit == 5)
		set_bit = 0x20;
	else if(bit == 6)
		set_bit = 0x40;
	else if(bit == 7)
		set_bit = 0x80;

	if((byte & set_bit) > 0)
		return(1);
	else return(0);
}
/*-----------------------------------------------------------------*/
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

void set_bit(fcb_buf,str)
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

void put_error(string)
char *string;
{
printf("\n\nThe mode  %s is unknown...terminating programme.", string);
exit();
}
/*-----------------------------------------------------------------*/
/*  LIBRARY FILE  ==  up_str                                       */
/*  Will convert a string to UPPER case ASCII                      */
/*-----------------------------------------------------------------*/

void up_str(s)
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

void lower_str(s)
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

char entab(position, size)
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
#define BASE_PORT 0x0c8	        /* Interfacer II printer ports     */
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
#define LOOPS      1000	        /* To do BUSY test                 */
/*-----------------------------------------------------------------*/

void plist(c)
int c;				/* The character to list           */
{
	short i;		/* Delay counter                   */

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

char list_status()
{
	short i, n;
	i = n = 0;		/* Has to be changed if successful */
	while(i < LOOPS && n != 1)
	{
	outp(HAND_SHAKE, BUSY_INPUT); /* Ask what the status is    */
	outp(HAND_SHAKE, OFF_STROBE); /* End of enquiry            */
	if(inp(LIST_PORT) != 0)         /* Then printer is busy    */
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
void lists(str) /* To list a null-terminated string                */
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
void listd(count)	/* To list a decimal number                */
short count;
{
	short i;		/* Counter                         */

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

void make_fcb(name,fcb)
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

	for (j = 1; j < 9; j++) /* Do the "name" part              */
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

	for (j =9; j < 12; j++)   /* Now do the "type" part        */
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

void pec_clear()
{
	short i;

	for(i = 0; i < 25; i++)
		putchar('\n');
}
/*-----------------------------------------------------------------*/
/*            TERMINAL FUNCTIONS                                   */
/*            ------------------                                   */
/*  For use with Hazeltine Esprit terminal                         */
/*  Use with LEAD-IN as set by switches.   Change this setting if  */
/*  different in your case.                                        */
/*-----------------------------------------------------------------*/
#define	LEAD_IN 0x07e   /* Tilde selected by switches              */
#define	BRIGHT  0x01f   /* Needs to have foreground switch set     */
#define	DIM	0x019
#define	CLEAR	0x01c
/*-----------------------------------------------------------------*/

void haz_clear()
{
	putchar(LEAD_IN);
	putchar(CLEAR);
}
/*-----------------------------------------------------------------*/

void bright()
{
	putchar(LEAD_IN);
	putchar(BRIGHT);
}
/*-----------------------------------------------------------------*/

void normal()
{
	putchar(LEAD_IN);
	putchar(DIM);
}
/*-----------------------------------------------------------------*/
/*            LINE                                                 */
/*  Spaces down one line on the console display                    */
/*-----------------------------------------------------------------*/

void line()
{
	putchar(CR);
	putchar(LF);
}
/*-----------------------------------------------------------------*/
/*            LINES                                                */
/*  Spaces down the number of lines nominated by the user          */
/*-----------------------------------------------------------------*/

void lines(number)
unsigned number;
{
	unsigned i;			/* The counter             */

	putchar(CR);
	for(i = 0; i < number; i++)
		putchar(LF);
}
/*-----------------------------------------------------------------*/
/*            PRINT_U                                              */
/*  Print an unsigned decimal number to the console                */
/*-----------------------------------------------------------------*/

void print_u(n)
unsigned n;
{
	unsigned i;

	i = 0;
	if((i = n/10) != 0)
		print_u(i);
	putchar(n % 10 + '0');
}
/*-----------------------------------------------------------------*/
/*            GO_TO                                                */
/*  Direct cursor addressing function where -                      */
/*                                                                 */
/*       x    is the column number (0 to 79)                       */
/*       y    is the row number (0 to 23)                          */
/*******************************************************************/
#define	LEAD_IN 0x7e    /* Tilde for Hazeltine terminal            */
#define	CUR_ADR 0x11    /* Direct cursor address signal            */

void go_to(column, row)
int column, row;
{
	out_char(LEAD_IN);
	out_char(CUR_ADR);
	out_char(column);
	out_char(row);
}
/*******************************************************************/
/*            OUT_CHAR                                             */
/*  Send the nominated character to the console but limit size to  */
/*  7 bits.                                                        */
/*******************************************************************/
void out_char(c)
int c;
{
	bios(4, c & 0x7f);
}
/*******************************************************************/
/*  LIBRARY FUNCTION  ==>  ISNUM                                   */
/*  ----------------                                               */
/*                                                                 */
/*  Returns TRUE (1) if a character is numeric (0....9) and        */
/*  FALSE (0) if the character isn't numeric.                      */
/*******************************************************************/

char isnum(c)
int c;
{
	if(c >= '0' && c <= '9')
		return(1);
	else return(0);
}
/*******************************************************************/
/*  LIBRARY FUNCTION  ==>  INKEY                                   */
/*  ----------------                                               */
/*  Duplicates the Microsoft Basic "inkey" function which tests    */
/*  the keyboard for input, and returns TRUE/FALSE depending on    */
/*  whether a key has been pressed or not.                         */
/*                                                                 */
/*  Requires:      DEFF3 to get BDOS functions                     */
/*  --------                                                       */
/*  Written to run under BDS C.                                    */
/*  Copyright 1986 by Cogar Computer Services Pty. Ltd.            */
/*******************************************************************/

char inkey()
{
	char IN;
	IN = 0xff;	/* Required to initiate DIRECTC function   */


	if(directc(IN) == 0)
		return(0);	/* No key pressed                  */
	else return(1);         /* Char waiting at keyboard        */
}
/*******************************************************************/
/*            PRT_HEX

    This routine will print the character in hexadecimal format
    with a leading zero when the number is less than 16 = 0x10     */
/*******************************************************************/

void prt_hex(number)
char number;
{
	if (number < 16)
	{
		putchar('0');
		printf("%x", number);
	}
	else printf("%x", number);
}
/*******************************************************************/
/*            BIOS FUNCTIONS UNDER BDS C

This file provides some of the CPM 2.2 BIOS functions which are
available under BDS C.   It uses either the

     bios(n, c)              for one-byte quantities

or   biosh(n, bc, de)        for two-byte quantities

Copyright 1986 - Cogar Computer Services Pty. Ltd.

*/
/*******************************************************************/
/*            W_BOOT                                               */
/*                                                                 */
/*  Performs a warm boot.   Doesn't return.                        */
/*******************************************************************/

void w_boot()
{
	bios(1, 0);
}
/*******************************************************************/
/*            CONST                                                */
/*                                                                 */
/*  Returns the CONSOLE STATUS -

                   0         if no character waiting
                   0xff      if character waiting at console
*/
/*******************************************************************/

char const()
{
	return(bios(2, 0));
}
/*******************************************************************/
/*            CONIN

  Returns the next character from the console.   Under CP/M 2.2 the
  parity bit (= bit 7) should be set to zero when the character is
  read in.
*/
/*******************************************************************/

char conin()
{
	return(bios(3, 0));
}
/*******************************************************************/
/*            CONOUT

  Outputs the nominated ASCII character to the console.            */
/*******************************************************************/

void conout(c)
char c;
{
	bios(4, c);
}
/*******************************************************************/
/*            LIST_C

  Outputs the nominated character to the list device, normally the
  line printer.                                                    */
/*******************************************************************/

void list_c(c)
char c;
{
	bios(5, c);
}
/*******************************************************************/
/*            SELDSK

  Selects the nominated disk as the logical disk for all I/O.
  Note the disks are numbered -

         A    0
         B    1
         C    2
         D    3                                                    */
/*******************************************************************/

int seldsk(drive)
char drive;
{
	char c;

	c = toupper(drive) - 65; /* Convert A = 0; B = 1;...C = 3  */

	return(biosh(9, c, 0));
}
/*******************************************************************/
/*            SET_TRK

  Will set the read/write head of the disk drive nominated by
  SELDSK to the track nominated here.   Note this is the
  absolute track number.   Acceptable values are

              0 to 65,535 (0xffff)
*/
/*******************************************************************/

void set_trk(track_no)
unsigned track_no;
{
	bios(10, track_no);
}
/*******************************************************************/
/*            SET_SEC

  Will set the nominated sector number (within the previously
  nominated track?) for later read/write.   Acceptable values are

              0 to 255 */
/*******************************************************************/

void set_sec(sector_no)
int sector_no;
{
   	bios(11, sector_no);
}
/*******************************************************************/
/*            SET_DMA

  Sets the DMA buffer address to the nominated pointer address.    */
/*******************************************************************/

void set_dma(buffer)
char *buffer;
{
	bios(12, buffer);
}
/*******************************************************************/
/*            READ_SEC

  Reads the 128-byte sector nominated by previous SETTRK and SETSEC
  calls into the buffer defined by SET_DMA.   Returns 0 in the read
  was successful, otherwise returns 1 for an (unspecified) error.  */
/*******************************************************************/

char read_sec()
{
	return(bios(13, 0));
}
/*******************************************************************/
/*            WRITE_SEC

  Writes the 128-byte buffer denoted by SET_DMA to the track and
  sector defined by SETTRK and SETSEC.   Returns 0 if the write was
  successful, otherwise returns 1 for an (unspecified) write error.*/
/*******************************************************************/

char write_sec()
{
	return(bios(14, 0));
}
/*******************************************************************/
/*            LISTST

  Checks the status of the list device (the line printer) and then
  returns

              0xff      if the printer is ready to accept a character
  or,          0         if the printer is busy (or off line?).    */
/*******************************************************************/

char listst()
{
	return(bios(15, 0));
}
/*******************************************************************/
/*  LIBRARY FILE  ==>  VIDEO_CHAR

    This will print one character to the console in the mode
    nominated by the user.   The available modes are -

         B  -  Bright
         N  -  Normal

    Note: As written uses the "Bright" and "Normal" functions which
    ----  are specific to Hazeltine terminals.   These need to be
          rewritten for other terminals.

    Copyright 1986 - Cogar Computer Services Pty. Ltd.             */
/*-----------------------------------------------------------------*/

void video_char(ch, attr)
char ch, attr;
{
	if(toupper(attr) == 'B')
		bright();
	else normal();

	putchar(ch);

	if(toupper(attr) == 'B')
		normal();
	else bright();
}
/*-----------------------------------------------------------------*/
SEC

  Reads the 128-byte sector nominated by previous SETTRK and SET