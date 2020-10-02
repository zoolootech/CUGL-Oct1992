/*
HEADER:                 CUGxxx.xx;
TITLE:                  Source code for MSBASIC-like COLOR command;
DATE:                   06/24/86;
DESCRIPTION:
  "A program for use under MS/PC-DOS to change the colors of text."
  "For further documentation, see the help screen; available by entering
   the color command without parameters."
KEYWORDS:               Color, MS-DOS, PC-DOS, ANSI; 
FILENAME:               COLOR.C;
WARNINGS:
  "The following statement must appear in the AUTOEXEC.BAT 
       file:
		DEVICE=ANSI.SYS

   In addition, the file named ANSI.SYS must be on the disk
   drive used to boot the system.
   The author claims copyrights and authorizes non-commercial
      use only.";
AUTHOR:                 Michael M. Yokoyama;
COMPILERS:              Microsoft C;
REFERENCES:		MS-DOS Version 2.11 User's Guide;
    AUTHORS:            " ";
    TITLE:              " ";
    CITATION:           " ";
ENDREF
*/
/*  	COLOR.C

	by Michael M. Yokoyama

	Selects text colors for use in MS/PC-DOS.

	This program uses ANSI escape sequences.  To use this program, you 
	must have the file named ANSI.SYS on the root directory of your boot 
	disk, and the following statement in your CONFIG.SYS file.

		device=ANSI.SYS
*/ 

#include <stdio.h>
#define ESC 27

main(argc, argv)
int argc;
char *argv[];
{
	if ((argc == 1) || (argc > 3 )) {
		help();
		exit(1);
	}
	if (argv[1][1] == '+') {
		printf("%c[0m",ESC);
		printf("%c[1m",ESC);
	} 
	else printf("%c[0m",ESC);
	printf("%c[3%cm",ESC,*argv[1]);
	if (argv[2] == NULL ) {
		printf("%c[2J",ESC);	
		exit(1);
	}
	printf("%c[4%cm",ESC,*argv[2]);
	printf("%c[2J",ESC);	
}

help()
{
	int c;
	printf("%c[2J%c[0mUsage:  COLOR <foreground>[+] [<background>]\n",ESC,ESC);	
	printf("  COLOR 0 3    Black on yellow\n");
	printf("Default:    The background color code may be omitted; the default is black.\n");
	printf("  COLOR 4      Blue (on default black)\n");
	printf("%c[0mNormal color codes:\n",ESC);
	for (c = 40; c <= 47; c++) {
	printf("%c[0;%d;30m",ESC,c);
	colors();
	printf("\n");
	}
	printf("%c[0mHighlight:  Append a plus (+) after the foreground code to highlight the color.\n",ESC); 
	printf("  COLOR 2+ 4   Bright green on blue\n");
	printf("Highlight color codes:\n");
	for (c = 40; c <= 46; c++) {
	printf("%c[1;%d;30m",ESC,c);
	colors();
	printf("\n");
	}
	printf("%c[1;47;30m",ESC,c);
	colors();
	printf("%c[0m",ESC);
}

colors()
{
		printf("%c[30m Black: 0  %c[31mRed: 1  %c[32mGreen: 2  %c[33mYellow: 3  ",ESC,ESC,ESC,ESC);
		printf("%c[34mBlue: 4  %c[35mMagenta: 5  %c[36mCyan: 6  %c[37mWhite: 7 %c[0;43;30m",ESC,ESC,ESC,ESC,ESC);
}
