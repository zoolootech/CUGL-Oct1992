
/*
	Tabify.c	written by Leor Zolman

	This filter takes sequences of spaces in a file and turns
	them, whenever possible, into tabs. Usage:

		A>tabify oldfile [newfile] <cr>

	Quoted strings are not modified. Likewise, the remainder of the
	line is not modified. The \<NL> escape for line continuation
	in C is recognized and handled accordingly.

*/
/* Changed 6-10-81 CAF newfile optional, eliminated crt list */
/* Changed 8-5-81 CAF Quote terminated at \n */
/* Changed 9-8-81 CAF Strip parity rubouts nulls */

#include "a:bdscio.h"
#define stdout 1

main(argc,argv)
char **argv;
{
	int scount, column, ifd, ofd, i;
	int lastc, c, tabwidth;
	FILE ibuf,outfil, *obuf;

	tabwidth= 8;
	if(argc<2) {
usage:
		puts("usage: tabify [-tabwidth] oldfile [newfile]");
		exit();
	}
	if(argv[1][0] == '-') {
		tabwidth= atoi( &argv[1][1]);
		argv++;
		if( --argc < 2)
			goto usage;
	}
	ifd = fopen(argv[1],ibuf);
	if(argc>2) {
		obuf= outfil;
		ofd = fcreat(argv[2],obuf);
	}
	else {
		obuf=stdout;
		ofd=0;
	}
	if (ifd == ERROR || ofd == ERROR) {
		puts("Can't open file(s)");
		exit();
	}

	scount = column = 0;

	do {
		c = getc(ibuf);
		if (c == ERROR)
			break;
		switch(c &=0177) {
		 case '\n':
			 putc1('\r', obuf);
			putc1(c,obuf);
			column = scount = 0;
		case 0:
		case 0177:
		case '\r':
			break;
		case ' ':
			column++; scount++;
			if (!(column%tabwidth)) {
				if (scount > 1)
					putc1('\t',obuf);
				else
					putc1(' ',obuf);
				scount = 0;
			}
			break;
		case '\t':
			scount = 0;
			column += (tabwidth-column%tabwidth);
			putc1('\t',obuf);
			break;
		case '"':
			putc1('"',obuf);
			do {
				lastc=c;
				c = getc(ibuf);
				if (c == ERROR || c==CPMEOF) {
					puts("Quote error.");
					break;
				}
				putc1(c&=0177,obuf);
			} while (c != '"' && (c != '\n' || lastc=='\\'));
			while (c != '\n') {
				c = getc(ibuf);
				if(c==EOF || c==CPMEOF)
					break;
				putc1(c&=0177,obuf);
			}
			column = scount = 0;
			break;
		case 0x1a:
			break;
		default:
			for (i=0; i<scount; i++)
				putc1(' ',obuf);
			scount = 0;
			/* don't count spaces for control characters */
			if(c >= ' ')
				column++;
			putc1 (c,obuf);
		}
	} while (c != CPMEOF && c != EOF);

	putc1(CPMEOF, obuf);
	fflush(obuf);
	fclose(ibuf);
	fclose(obuf);
}

putc1(c,buf)
char c;
{
/*
	putchar(c);
*/
	if (putc(c,buf) < 0) {
		puts("putc just retured an error!");
		exit();
	}
}

e
					putc1(' ',obuf);
				scount = 