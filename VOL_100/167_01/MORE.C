/* 
HEADER: 	CUG
TITLE:		MORE.C - An Emulation of MS-DOS 'more' Utility
			 for CP/M
VERSION:	1.00
DATE:		11/08/85
DESCRIPTION:	"An emulation of MS-DOS 'more' utility for use
		with the CP/M operating system "
KEYWORDS:	more, filter, utility, CP/M
SYSTEM:		Any CP/M system
FILENAME:	MORE.C
WARNINGS:	NONE
CRC:		xxxx
SEE-ALSO:	NONE
AUTHORS:	Ian Ashdown - byHeart Software
COMPILERS:	ANY
REFERENCES:	AUTHORS:
		TITLE:
ENDREF
*/

/*-------------------------------------------------------------*/

/* MORE.C - An Emulation of MS-DOS "more" Utility for CP/M
 *
 * Version 1.00		September 15th, 1985
 *
 * Copyright 1985:	Ian Ashdown
 *			byHeart Software
 *			2 - 2016 West First Avenue
 *			Vancouver, B.C. V6J 1G8
 *
 * Synopsis:	MORE < [file]
 *
 * Description:	MORE is used to view a long text file one screen
 *		at a time. If there is more information to be
 *		displayed, -MORE- appears at the bottom of the
 *		screen. Press any key to display another screen
 *		of information. This process continues until all
 *		the input file has been read via the standard
 *		input. CNTL-C can be used to terminate the
 *		program.
 *
 * Diagnostics: None.
 *
 * Bugs:	Lines longer than 256 characters are truncated.
 */

/*** Include Files ***/

#include <stdio.h>

/*** Definitions ***/

#define BREAK	0x03	/* Define CNTL-C as break key */
#define BUFLEN	 257	/* Length of line buffer */
#define SCR_HT	  24	/* Terminal screen height in lines */

/*** Main Body of Program ***/

main()
{
  char c,
       buffer[BUFLEN],
       *fgets();
  int i;

  while(1)
  {
    for(i = 1; i < SCR_HT; i++)		/* Display a screen */
    {
      if(fgets(buffer,BUFLEN,stdin) == NULL)  /* End of file ? */
	exit(0);
      else				/* Display a line */
	printf(buffer);
    }
    printf("-MORE- ");
    do			/* Wait for operator response */
      c = bdos(6,0xff);	/* (No character echo to screen) */
    while(c == NULL);
    if(c == BREAK)	/* Terminate program? */
      break;
    putchar('\n');
  }
}

/*** Explanation of Aztec CII Functions ***/

/* bdos(bc,de)
 * int bc,de;
 *
 * Calls CP/M's BDOS with 8080 CPU register pair BC set to "bc"
 * and register pair DE set to "de". The value returned by the
 * 8080 CPU accumulator is the return value.
 */

/*** End of MORE.C ***/
#define BREAK	0x03	/* Define CNTL-C as break key */
#define BUFLEN	 25