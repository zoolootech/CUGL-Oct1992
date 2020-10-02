/*-
 *  ----------------------------------------------------------------------
 *  File        :   DIOTST01.C
 *  Creator     :   Blake Miller
 *  Version     :   01.01.00        February 1991
 *  Language    :   Microsoft C     Version 5.1
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *              :   Test Program #1
 *  ----------------------------------------------------------------------
 *  Link : DIOLIB?.LIB
 *  ----------------------------------------------------------------------
 *  Revision History:
 *  022891 BVM  :   Change int to short.
 *  091190 BVM  :   Creation
 *  ----------------------------------------------------------------------
 */

#define P_NAME  "DIOTST01"
#define P_TITL  "DIOLIB Digital IO Function Demonstration"
#define P_AUTH  "Blake Miller"
#define P_VERS  "01.01.00"
#define P_DATE  "February 28, 1991"
#define P_PURP  "Toggle bits at the 8255"

/*  Include Files ----------------------------*/

#define     DIOTST01_C_DEFINED  1
#include    <conio.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    "DIOLIB.H"
#undef      DIOTST01_C_DEFINED

/*  Function Prototype -----------------------*/

void kb_dump (void);    /* keyboard flush function  */
void kb_wait (void);    /* keypress wait / message  */

/*-
 *  ----------------------------------------------------------------------
 *  Function Definition
 *  ----------------------------------------------------------------------
 */

void main (void)
	{
	short           i;          /* counter      */
	short           bitnum;     /* bit number   */
	short           bstate;     /* bit state    */
	char            s[32];      /* gets buffer  */
	unsigned char   prtbyt;     /* port byte    */
	DIODAT          dioprt;     /* port data    */

	/*  Print Program Header
	 */
	printf ("\n" );
	printf ("Program    : %s - %s.\n", P_NAME, P_TITL );
	printf ("Author     : %s\n", P_AUTH );
	printf ("Version    : %s  %s\n", P_VERS, P_DATE );
	printf ("Purpose    : %s.\n", P_PURP );
	printf ("\n" );
	printf ("This program will fiddle with the bits of the 8255.\n");
	printf ("\n");

	/*  Prompt user for address of the 8255.
	 *  Exit if an empty string is entered.
	 */
	printf ("NOTE: 300 Hex is 768 in decimal.\n");
	printf ("Enter the base address of the 8255 in decimal : ");
	gets (s);
	if ( *s == 0 ){
		printf ("Empty string detected.  Exiting...\n");
		exit (0);
		}

	i = atoi(s);            /* get address of board     */
	dio_init (&dioprt, i);  /* initialize port data     */

	/*  --------------------------------------------------------
	 *  Digital Output Demonstration
	 *  --------------------------------------------------------
	 */

	printf ("\n" );
	printf ("Digital Output Demonstration.\n");
	printf ("Set up output detection circuitry on ANY of the 8255\n");
	printf ("output lines.  They will be toggled, flashed, etc.\n");
	printf ("ALL lines will be set for output so remove any driving\n");
	printf ("(INPUT) circuitry at this time, please.\n");
	kb_wait();

	dio_config (&dioprt, 0, 0, 0, 0);   /* all ports output     */

	printf ("Clear all bits in digital IO...\n");
	dio_dump_bytes (&dioprt);           /* all bits clear       */

	printf ("Toggle bits in digital IO...\n");

	bitnum = 0;

	while ( 1 ){

		if ( kbhit() ) break;

		dio_bitput (&dioprt, bitnum, 0);    /* clear bit (OFF)      */
		bitnum++;
		bitnum = (bitnum % 24);             /* bits from 0 to 23    */
		dio_bitput (&dioprt, bitnum, 1);    /* set   bit (ON )      */

		for ( i = 0; i < 200; i++ ){
			printf ("......\r");
			}

		}

	printf ("Set all bits in digital IO...\n");
	dio_put_byte (&dioprt, DIO_PORTA, 0xFF);
	dio_put_byte (&dioprt, DIO_PORTB, 0xFF);
	dio_put_byte (&dioprt, DIO_PORTC, 0xFF);
	kb_wait();

	printf ("Clear all bits in digital IO...\n");
	dio_put_byte (&dioprt, DIO_PORTA, 0);
	dio_put_byte (&dioprt, DIO_PORTB, 0);
	dio_put_byte (&dioprt, DIO_PORTC, 0);
	kb_wait();

	printf ("Set all bits in digital IO Port A...\n");
	dio_put_byte (&dioprt, DIO_PORTA, 0xFF);
	kb_wait();
	dio_put_byte (&dioprt, DIO_PORTA, 0);

	printf ("Set all bits in digital IO Port B...\n");
	dio_put_byte (&dioprt, DIO_PORTB, 0xFF);
	kb_wait();
	dio_put_byte (&dioprt, DIO_PORTB, 0);

	printf ("Set all bits in digital IO Port C Low...\n");
	dio_put_byte (&dioprt, DIO_PORTC, 0x0F);
	kb_wait();
	dio_put_byte (&dioprt, DIO_PORTC, 0);

	printf ("Set all bits in digital IO Port C High...\n");
	dio_put_byte (&dioprt, DIO_PORTC, 0xF0);
	kb_wait();
	dio_put_byte (&dioprt, DIO_PORTC, 0x00);


	/*  --------------------------------------------------------
	 *  Digital Input Demonstration
	 *  --------------------------------------------------------
	 */

	printf ("\n" );
	printf ("Digital Input Demonstration.\n");
	printf ("Set up some input circuitry on ANY of the 8255\n");
	printf ("digital I/O lines.  They will be sensed.\n");
	printf ("ALL lines will be set for input so remove any detection\n");
	printf ("(OUTPUT) circuitry at this time, please.\n");
	kb_wait();

	dio_config (&dioprt, 1, 1, 1, 1);   /* all ports input  */
	dio_load_bytes (&dioprt);           /* all bits read    */

	printf ("Read ALL bits in digital IO...\n");
	for ( bitnum = 0; bitnum <= 23; bitnum++ ){
		dio_bitget (&dioprt, bitnum, &bstate);
		printf ("Bit %2d is ", bitnum);
		if ( bstate )   printf ("SET  \n");
		else            printf ("CLEAR\n");
		}
	kb_wait();

	printf ("Set any new input configuration before all ports\n");
	printf ("will be sensed and the settings printed out.\n");
	kb_wait();

	dio_get_byte (&dioprt, DIO_PORTA, &prtbyt);
	printf ("Data for Port A is %02X\n", (unsigned int) prtbyt);
	dio_get_byte (&dioprt, DIO_PORTB, &prtbyt);
	printf ("Data for Port B is %02X\n", (unsigned int) prtbyt);
	dio_get_byte (&dioprt, DIO_PORTC, &prtbyt);
	printf ("Data for Port C is %02X\n", (unsigned int) prtbyt);
	kb_wait();

	printf ("That is the end of this demonstration program.\n");
	kb_wait();

	exit ( 0 );
	}

/*-
 *  ----------------------------------------------------------------------
 *  Support Function Definitions
 *  ----------------------------------------------------------------------
 */

/*- Flush Keyboard ----------------------------**
 *  Check for keypress, and read them out of
 *  the buffer until there are no more.
 */
void kb_dump (void)
	{
	while ( kbhit() ){              /* key was pressed          */
		if ( (getch() == 0x00) ||   /* read out keypress        */
			 (getch() == 0xE0) )
			getch();                /* no function key codes    */
		}
	}

/*- Keyboard Wait ----------------------------**
 *  Flush keyboard.
 *  Print blank line, then print a message to press
 *  spacebar to continue, then wait for keypress, then
 *  erase message and return.
 */
void kb_wait (void)
	{
	kb_dump ();
	printf ("\r\n");
	printf ("[ PRESS <SPACE-BAR> TO CONTINUE ]\r");
	if ( getch() == 0 ) getch();
	printf ("                                 \r");
	}

/*-
 *  ----------------------------------------------------------------------
 *  END DIOTST01.C Source File
 *  ----------------------------------------------------------------------
 */
