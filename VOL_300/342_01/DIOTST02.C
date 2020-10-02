/*-
 *  ----------------------------------------------------------------------
 *  File        :   DIOTST02.C
 *  Creator     :   Blake Miller
 *  Version     :   01.01.00        February 1991
 *  Language    :   Microsoft C     Version 5.1
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *              :   Test Program #2
 *  ----------------------------------------------------------------------
 *  Link : DIOLIB?.LIB
 *  ----------------------------------------------------------------------
 *  Revision History:
 *  022891 BVM  :   Creation
 *  ----------------------------------------------------------------------
 */

#define P_NAME  "DIOTST02"
#define P_TITL  "DIOLIB Multi Digital IO"
#define P_AUTH  "Blake Miller"
#define P_VERS  "01.01.00"
#define P_DATE  "February 28, 1991"
#define P_PURP  "Toggle bits on the 8255 ports"

/*  Include Files ----------------------------*/

#define     DIOTST02_C_DEFINED  1
#include    <conio.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    "DIOLIB.H"
#undef      DIOTST02_C_DEFINED

/*  Function Prototype -----------------------*/

void pg_exit (short);
void kb_dump (void);    /* keyboard flush function  */
void kb_wait (void);    /* keypress wait / message  */

/*-
 *  ----------------------------------------------------------------------
 *  Function Definition
 *  ----------------------------------------------------------------------
 */

void main (void)
	{
	short           i, j;       /* counter      */
	short           bitnum;     /* bit number   */
	short           bstate;     /* bit state    */
	char            s[32];      /* gets buffer  */
	DIODAT          dioprt;     /* port data    */

	short           chpcnt = 0; /* number 8255 on board */
	short           bitcnt = 0; /* total number bits    */

	/*  Print Program Header
	 */
	printf ("\n" );
	printf ("Program    : %s - %s.\n", P_NAME, P_TITL );
	printf ("Author     : %s\n", P_AUTH );
	printf ("Version    : %s  %s\n", P_VERS, P_DATE );
	printf ("Purpose    : %s.\n", P_PURP );
	printf ("\n" );
	printf ("This program will fiddle with the bits of the 8255.\n");
	printf ("This program was designed to work with digital I/O\n");
	printf ("boards containing multiple 8255s.\n");
	printf ("This program assumes that the 8255s can do input and output!\n");
	printf ("\n");
	printf ("Do you wish to continue [Y]:N : ");
	gets (s); if ( *s == 'N' ) exit (0);

	/*  Prompt user for number of 8255 on board.
	 *  Exit if an empty string is entered.
	 */
	printf ("Enter the number of 8255s on the board : ");
	gets (s); if ( *s == 0 ) exit (0);
	chpcnt = atoi(s);
	bitcnt = 24 * chpcnt;   /* 24 bits on each 8255 */

	/*  Set up 8255 DIODAT array size data
	 *  based on number of ports specified.
	 */
	if ( dio_pa_aloc (chpcnt) ){
		printf ("Error allocating memory for DIODAT buffers.\n");
		exit (1);
		}

	/*  Prompt user for base address of each 8255.
	 *  Exit if any empty string is entered.
	 */
	printf ("NOTE: 300 Hex is 768 in decimal.\n");
	printf ("NOTE: On most boards, the 8255s are spaced 4 bytes apart.\n");
	for ( j = 0; j < chpcnt; j++ ){
		printf ("Enter base address (decimal) of 8255 #%2d : ", j);
		gets (s); if ( *s == 0 ) pg_exit (0);
		i = atoi(s);            /* get address of 8255  */
		dio_pa_setadr (j, i);   /* set address of 8255  */
		}

	/*  --------------------------------------------------------
	 *  Digital Output Demonstration
	 *  --------------------------------------------------------
	 */

	printf ("\n" );
	printf ("----------------------------------------------\n");
	printf ("Digital Output Demonstration.\n");
	printf ("Set up output detection circuitry on ANY of the 8255\n");
	printf ("output lines.  They will be toggled, flashed, etc.\n");
	printf ("ALL lines will be set for output so remove any driving\n");
	printf ("(INPUT) circuitry at this time, please.\n");
	kb_wait();

	for ( j = 0; j < chpcnt; j ++ ){
		dio_pa_config (j, 0, 0, 0, 0);   /* all ports output     */
		}

	printf ("Clear all bits in digital IO...\n");
	for ( bitnum = 0; bitnum < bitcnt; bitnum++ ){
		dio_pa_bitput (bitnum, 0);
		}

	printf ("Toggle bits in digital IO.\n");
	printf ("Press key to make it stop.\n");

	bitnum = 0; /* start at bit 0   */

	while ( 1 ){

		if ( kbhit() ) break;

		dio_pa_bitput (bitnum, 0);      /* clear bit (OFF)      */
		bitnum++;                       /* next bit number      */
		bitnum = (bitnum % bitcnt);     /* Range: [0:bitnum-1]  */
		dio_pa_bitput (bitnum, 1);      /* set   bit (ON )      */

		for ( i = 0; i < 1000; i++ ){   /* simple delay loop    */
			printf ("...\r");
			}
		}

	/*  --------------------------------------------------------
	 *  Digital Input Demonstration
	 *  --------------------------------------------------------
	 */

	printf ("\n" );
	printf ("----------------------------------------------\n");
	printf ("Digital Input Demonstration.\n");
	printf ("Set up some input circuitry on ANY of the 8255\n");
	printf ("digital I/O lines.  They will be sensed.\n");
	printf ("ALL lines will be set for input so remove any detection\n");
	printf ("(OUTPUT) circuitry at this time, please.\n");
	kb_wait();

	for ( j = 0; j < chpcnt; j ++ ){
		dio_pa_config (j, 1, 1, 1, 1);      /* all ports input */
		}
	for ( bitnum = 0; bitnum < bitcnt; bitnum++ ){
		dio_pa_bitget (bitnum, &bstate);    /* all bits read    */
		}

	printf ("Read ALL bits in digital IO...\n");
	for ( bitnum = 0; bitnum < bitcnt; bitnum++ ){
		dio_pa_bitget (bitnum, &bstate);    /* bit is read      */
		printf ("Bit %2d is ", bitnum);
		if ( bstate )   printf ("SET  \n");
		else            printf ("CLEAR\n");
		if ( (bitnum+1) % 15 == 0 )         /* so lots o bits doesn't   */
			kb_wait();                      /* scroll off the screen!   */
		}
	kb_wait();

	printf ("Set any new input configuration before all bits\n");
	printf ("will be sensed and the settings printed out.\n");
	kb_wait();

	for ( bitnum = 0; bitnum < bitcnt; bitnum++ ){
		dio_pa_bitget (bitnum, &bstate);    /* bit is read      */
		printf ("Bit %2d is ", bitnum);
		if ( bstate )   printf ("SET  \n");
		else            printf ("CLEAR\n");
		if ( (bitnum+1) % 15 == 0 )
			kb_wait();
		}
	kb_wait();

	printf ("----------------------------------------------\n");
	printf ("That is the end of this demonstration program.\n");
	kb_wait();

	exit ( 0 );
	}


/*- Special Program Exit
 *  Called from anywhere in program.
 *  Allows program to free DIODAT memory
 *  that may have been allocated.
 */
void pg_exit ( short ecode )
	{
	dio_pa_free ();     /* free memory in case allocated    */
	printf ("%s Program Exiting : %d\n", P_NAME, ecode );
	exit (ecode);
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
 *  END DIOTST02.C Source File
 *  ----------------------------------------------------------------------
 */
