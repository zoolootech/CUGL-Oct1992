/*
HEADER:         CUGXXX;
TITLE:          HP LaserJet Printing Utility;
DATE:           3-20-86;
DESCRIPTION:    Printing utility for the HP LaserJet;
KEYWORDS:       Laser printer;
FILENAME:       LJ.C;
WARNINGS:       None;
AUTHORS:        Joe Barnhart, Ray Duncan, Abel DeSouza;
COMPILER:       C-86;
REFERENCES:     US-DISK 1307, Dr. Dobbs Journal, 9-85, p. 117;
ENDREF
*/
/**
 **
 **		LJ.C -- A printing utility for the HP LaserJet
 **		This program prints a series of files on the LaserJet
 **		printer.  The files are printed in a "landscape" font at
 **		17 characters to the inch.  To take advantage of this
 **		density, two "pages" of information from the file are
 **		printed on each piece of paper (left and right halves).
 **
 **		Usage is:		LJ  [-b] file1 file2 file3 ...
 **							-b ignore page breaks.
 **
 **		Where file# is a valid MS-DOS filename, included on the
 **		command line.
 **
 **		Reference Dr. Dobbs Journal, Sept 1985 p117
 **		Joe Barnhart	original version		May 5, 1985
 **		Ray Duncan		date and time stamping	May 27, 1985
 **		Joe Barnhart	revised date stamping	June 6, 1985
 **		Abel DeSouza	revised for LaserJet cartridge 92286A & CI86
 **								Sept 16, 1985
 **		revised for C86 and & page breaks option 10/9/85
 **
 */

#include <f:stdio.h>

#define MAXLINE 55			/* maximum lines per page */
#define PAGE	12		/* ^L MS DOS Page break   */
#define TAB		4			/* width of one tab stop */
#define FALSE	0
#define TRUE	1

typedef struct {
		int ax, bx, cx, dx, si, di, ds, es;
	} REGSET;

main(argc, argv)
	int		argc;
	char	*argv[];
	{

	char	c, page, *arg;
	int	filenum, i;
	FILE	*fp, *prn, *fopen();

	/* set defaults */
	page = FALSE;
	i = 1;

	if ( ( prn = fopen( "PRN", "w" ) ) == NULL)
		printf( "Error opening printer as file.\n" );

	else if (argc != 1) {
		if (*(arg = *++argv) == '-') {
/*			printf("argument string: %s \n", arg);    */
			switch (c = *++arg, toupper(c)) {
				case 'B' :
					page = TRUE;
					i = 2;
					break;
				default:
					user_err(1);
				}
			}
		argv--;
/*		printf("argc = %d, i = %d\n", argc, i); */
		if (argc > i) {
			/* initialize the LaserJet for landscape printing */
			fprintf( prn, "\033E\033&l1O\033(8U\033(s0p16.66h8.5v0s-1b0T" );
			for( filenum = i; filenum < argc; filenum++ )  {
				fp = fopen( argv[filenum], "r" );
				if( fp == NULL )
					printf( "File %s doesn't exist.\n", argv[filenum] );
				else {
					printf( "Now printing %s\n", argv[filenum] );
					printfile( fp, prn, argv[filenum], page );
					fclose( fp );
					}
				}
			fprintf( prn, "\015\033E" );		/* clear LaserJet */
			}
		else
			user_err(2);
		}
	else
		user_err(0);
	}

/***************************************************************/

user_err(sw)
	int sw;
	{
	printf("\nERROR: Invalid parameter specification\n\n");
	printf("Usage: lj <-b> <filename> <filename>...\n");

	switch(sw) {
		case 1:
			printf("Wrong flag specified");
			break;
		case 2:
			printf("File name required");
			break;
		default :
			printf("Flags: -b  Ignore page breaks\n");
		}
	exit(0);
	}

/***************************************************************/

printfile( fp, prn, filename, page)
	FILE	*fp, *prn;
	char	*filename, page;
	{

	int pagenum = 1;

	while( !feof( fp ) ) {
		fprintf( prn, "\033&a0r85m5L\033&l8D\015" );		/* set left half */
		printpage( fp, prn );						/* print page */
		if ( !feof( fp ) ) {						/* if more .. */
			fprintf( prn,"\033&a0r171m91L\033&l8D" );		/* set right half */
			printpage( fp, prn, page );					/* print another */
			}
		stamp( prn, filename, pagenum++ );			/* title */
		fputc( PAGE, prn );							/* kick paper */
		}
	}

/***************************************************************/

printpage( fp, prn, page)
	FILE	*fp, *prn;
	char	page;
	{

	char	c;
	int		line, col;

	line = col = 0;
	while( line < MAXLINE )
		switch( c = fgetc(fp) ) {
			case '\n':				/* newline found */
				col = 0;			/* zero column */
				line++; 			/* adv line cnt */
				fputc( '\n', prn);
				break;
			case '\t':				/* tab found */
				do
					fputc( '\040', prn);
				while ( (++col % TAB ) != 0 );
				break;
			case PAGE:				/* page break or */
				if (page) break;
			case EOF:			/* EOF found */
				line = MAXLINE; 	/* force terminate */
				break;
			default:				/* no special case */
				fputc( c, prn );	/* print character */
				col++;
				break;
			}
	}

/***************************************************************/

stamp( prn, filename, pagenum )
	FILE	*prn;
	char	*filename;
	{

	char	datestr[10], timestr[10];

	fprintf(prn, "\033&a171m51L" );			/* widen margins */
	fprintf(prn, "\015\033&a57R" );			/* move to row 58 */
	fprintf(prn, "File: %-83s", filename );
	fprintf(prn, "Page %-3d", pagenum);
	timestamp( timestr );
	datestamp( datestr );
	fprintf( prn, "   %s   %s", datestr, timestr);
	}

/***************************************************************/

datestamp( datestr )
	char	*datestr;
	{

	REGSET	regs;
	int month, day, year;

	regs.ax = 0x2a00;
	sysint( 0x21, &regs, &regs );
	month = ( regs.dx >> 8 ) & 255;
	day	  = regs.dx & 255;
	year  = regs.cx - 1900;
	sprintf( datestr, "%02d/%02d/%02d", month, day, year );
	}

/***************************************************************/

timestamp( timestr )
	char	*timestr;
	{

	REGSET	regs;
	int hours, mins;

	regs.ax = 0x2c00;
	sysint( 0x21, &regs, &regs );
	hours = ( regs.cx >> 8 ) & 255;
	mins  = regs.cx & 255;
	sprintf( timestr, "%02d:%02d", hours, mins );
	}
