/*	
** cls.c	  Clear Screen Program		by F.A.Scacchitti  5/4/85
**		    Copyright 1984 		   25 Glenview Lane
**		  All rights reserved		   Roch, NY  14609
**
**		Written in Small-C  (Version 2.10)
**
**
**
**	Characters used are for a Xerox 820-II
**      Use -n switch when compiling
**
*/

main()  {

   putchar(5);		/* Next character is cursor       */
   putchar(2);		/* Cursor character - solid block */
   putchar(26);		/* ^Z clears the screen           */
			/* That's all folks               */

}


