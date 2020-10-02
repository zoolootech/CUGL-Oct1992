
/*
	Tabify.c	written by Leor Zolman

	This filter takes sequences of spaces in a file and turns
	them, whenever possible, into tabs. Usage:

		A>tabify oldfile newfile <cr>

	Quoted strings are not processed, but there should NOT be
	any `lone' double quotes within the file being tabified.

Ver 5	(named to distinguish between TABIFY and TABIFY4 and to
	encourage distribution of the file.)
	Changes:
		1. Asterisks no longer displayed
		2. #DEFINE used for TEXT/ASM versions
	By S. Kluger  11/13/83

	Comment out one of the following #defines:
*/

#define QUOT '''	/*  for ASM file processing */
/*		
#define QUOT '"'	/*  for TEXT file processing */
*/

#include "bdscio.h"

int scount, column, ifd, ofd, i;
int c;
char ibuf[BUFSIZ], obuf[BUFSIZ];

main(argc,argv)
char **argv;
{
	if (argc < 2 || argc > 3) {
		printf("usage: tabify oldfile [newfile]\n");
		exit();
	}

	ifd = fopen(argv[1],ibuf);

	if (argc == 2) argv[2] = "tabify.tmp";
	ofd = fcreat(argv[2],obuf);

	if (ifd == ERROR || ofd == ERROR) {
		printf("Can't open file(s)\n");
		exit();
	}

	scount = column = 0;

	do {
		c = getc(ibuf);
		if (c == ERROR) {
			putc(CPMEOF,obuf);
			break;
		 }
		switch(c) {
		   case '\r':	putc1(c,obuf);
				scount = column = 0;
				break;
		   case '\n':	putc1(c,obuf);
				scount = 0;
				break;
		   case ' ':	column++;
				scount++;
				if (!(column%8)) {
				   if (scount > 1)
					putc1('\t',obuf);
				   else
					putc1(' ',obuf);
					scount = 0;
				 }
				break;
		   case '\t':	scount = 0;
				column += (8-column%8);
				putc1('\t',obuf);
				break;
		   case QUOT:	for (i = 0; i < scount; i++)
					putc1(' ',obuf);
				putc1(QUOT,obuf);
				do {
				   c = getc(ibuf);
				   if (c == ERROR) {
				    printf("Quote error.\n");
				    exit();
				   }
				   putc1(c,obuf);
				} while (c != QUOT);
				do {
					c = getc(ibuf);
					putc1(c,obuf);
				} while (c != '\n');
				column = scount = 0;
				break;
		   case CPMEOF:	putc(CPMEOF,obuf);
				break;
		   default:	for (i=0; i<scount; i++)
					putc1(' ',obuf);
				scount = 0;
				column++;
				putc1 (c,obuf);
		 }
	 } while (c != CPMEOF);

	fclose(ibuf);
	fclose(obuf);

	if (argc == 2) {
		unlink(argv[1]);
		rename(argv[2],argv[1]);
	}
}

putc1(c,buf)
char c;
{
	if (putc(c,buf) < 0) {
		printf("Write error (out of disk space?)\n");
		exit();
	}
}
}
}

putc1(c,buf)
char c;
{
	if (putc(c,buf) < 0) {
		printf("Write error (out of disk space?)\n"