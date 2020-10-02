/*-
 *  ----------------------------------------------------------------------
 *  File        :   I8255TO1.C
 *  Creator     :   Blake Miller
 *  Version     :   01.00.00            February 1991
 *  Language    :   Microsoft Quick C   Version 2.0
 *  Purpose     :   I8255 Digital IO
 *              :   Demonstration Program #1
 *  ----------------------------------------------------------------------
 */

#include    <conio.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    "I8255FN.H"

#define     PPI_BASE    0x300   /* I8255 Base Address   */

void    main ( void );
void kb_dump ( void );  /* flush keyboard buffer    */
int  kb_wait ( void );  /* wait for keypress        */

/*  convert unsigned data value to a binary string
 */
extern void scv_uvtobs ( void *, char *, int, int, int, int );

void main (void)
	{
	unsigned char   inbyte;         /* input byte   */
	I8255DAT        diodat;         /* 8255 data    */
	char            strdat[16];     /* binary text  */

	/*  Initialize 8255 data, assuming base address is at PPI_BASE
	 */
	I8255_init (&diodat, PPI_BASE);

	/*  Initialize 8255 hardware, and set all ports to output
	 */
	I8255_config (&diodat, 0, 0, 0, 0);

	/*  Set data for all ports to zero
	 */
	I8255_put_byte (&diodat, 1, 0);
	I8255_put_byte (&diodat, 2, 0);
	I8255_put_byte (&diodat, 3, 0);

	printf ("Set port A to all 1\n");
	I8255_put_byte (&diodat, 1, 0xFF);
	kb_wait();
	printf ("Set port A to all 0\n");
	printf ("Set port B to all 1\n");
	I8255_put_byte (&diodat, 1, 0x00);
	I8255_put_byte (&diodat, 2, 0xFF);
	kb_wait();
	printf ("Set port B to all 0\n");
	printf ("Set port C to all 1\n");
	I8255_put_byte (&diodat, 2, 0x00);
	I8255_put_byte (&diodat, 3, 0xFF);
	kb_wait();
	I8255_put_byte (&diodat, 3, 0x00);

	printf ("Set all ports to input.\n");
	I8255_config (&diodat, 1, 1, 1, 1);

	kb_dump ();
	printf ("\nPrint out input value of Port A till keypress...\n");
	while ( !kbhit() ){
		I8255_get_byte (&diodat, 1, &inbyte);
		scv_uvtobs ( (void *)&inbyte, strdat, 7, 0, '1', '0' );
		printf ("Port A Data : %02X = %s\r", (unsigned int)inbyte, strdat);
		}

	kb_dump ();
	printf ("\nPrint out input value of Port B till keypress...\n");
	while ( !kbhit() ){
		I8255_get_byte (&diodat, 2, &inbyte);
		scv_uvtobs ( (void *)&inbyte, strdat, 7, 0, '1', '0' );
		printf ("Port B Data : %02X = %s\r", (unsigned int)inbyte, strdat);
		}

	kb_dump ();
	printf ("\nPrint out input value of Port C till keypress...\n");
	while ( !kbhit() ){
		I8255_get_byte (&diodat, 3, &inbyte);
		scv_uvtobs ( (void *)&inbyte, strdat, 7, 0, '1', '0' );
		printf ("Port C Data : %02X = %s\r", (unsigned int)inbyte, strdat);
		}

	exit (0);
	}

/*- Flush Keyboard ----------------------------**
 *  Check for keypress, and read them out of
 *  the buffer until there are no more.
 */
void kb_dump (void)
	{
	short   ch;
	while ( kbhit() ){          /* key was pressed          */
		ch = getch();           /* read out keypress        */
		if ( (ch == 0x00) || (ch == 0xE0) )
			getch();            /* eat function key codes   */
		}
	}

/*- Keyboard Wait ----------------------------**
 *  Flush keyboard.
 *  Print blank line, then print a message to press
 *  spacebar to continue, then wait for keypress, then
 *  erase message and return.
 */
int kb_wait ( void )
	{
	short   ch;

	kb_dump ();
	printf ("\r\n");
	printf ("[ PRESS <SPACE-BAR> TO CONTINUE ]\r");
	ch = getch();
	if ( (ch == 0x00) || (ch == 0xE0) )
		ch = getch() | 0x0100;
	printf ("                                 \r");
	return (ch );
	}

/*-
 *  ----------------------------------------------------------------------
 *  END I8255T01.C Source File
 *  ----------------------------------------------------------------------
 */
