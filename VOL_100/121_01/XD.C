/*
HEADER: CUG 121.??;

    TITLE:	Xd - hexadecimal dump;
    VERSION:	1.0;
    DATE:	09/01/85;
    DESCRIPTION: "This program dumps a file to the console in one or more
		of three formats: ascii, hex (2 digits/byte) or word
		(standard 8080 format, assuming low byte first).";
    KEYWORDS:	file, dump, hex;
    SYSTEM:	CP/M;
    FILENAME:	XD.C;
    WARNINGS:	"Copyright (c) 1981, Mike W. Meyer."
    AUTHORS:	Mike W. Meyer;
    COMPILERS:	BDS-C 1.50;
*/

/*	Xd (the eXtended Dump program) replaces DR's dump program.
 *
 *	usage notes: xd [-wba] filename
 *		w - dump in word format (hex)
 *		b - dump in byte format (hex)
 *		a - dump as ascii
 *	you can ask for more that one format at a time, and the default
 *	is ba.
 *
 *	Dumps ascii as:
 *		control chars - ^ followed by uppercase char
 *		high bit on - use a | followed by the character in
 *			the low 7 bits. If this should be a control character,
 *			use a $ instead of a ^ in the standard control char
 *			format, & no | is output.
 *		special - 7f (del) is output as "^ " (control - space),
 *			and ff is "$ " (control space with the high bit on)
 */
#include <bdscio.h>
#define INPUT	0
#define LINELEN	16

/* Global flags to tell if we do/don't print things */
char byte, word, ascii ;

main(argc, argv) char **argv; {
	char i, buf[SECSIZ] ;
	int file, addr ;

	byte = ascii = 1 ;
	word = 0 ;
	if (argc == 1)
		usage();
	while (**++argv == '-') {
		argc--;
		byte = word = ascii = 0 ;
		while (*++*argv)
			switch (toupper(**argv)) {
				case 'A': ascii++ ;
					break ;
				case 'W': word++ ;
					break ;
				case 'B': byte++ ;
					break ;
				default:
					usage();
				}
		}
	if (argc != 2)
		usage();
	if ((file = open(*argv, INPUT)) == ERROR) {
		printf("can't open file %s!\n", *argv) ;
		exit(1) ;
		}
	addr = 0 ;
	while (read(file, buf, 1) > 0) {
		for (i = 0; i < SECSIZ; i += LINELEN)
			put16(buf + i, addr + i) ;
		addr += SECSIZ ;
		}
	}

/*
 * put16 - output a line of 16 bytes
 */
put16(cline, addr) char *cline; {
	char pad[LINELEN] ;

	sprintf(pad, "%04x   ", addr) ;
	if (byte) {
		putpad(pad) ;
		bytedump(cline) ;
		}
	if (ascii) {
		putpad(pad) ;
		ascdump(cline) ;
		}
	if (word) {
		putpad(pad) ;
		worddump(cline) ;
		}
	}

/*
 * putpad - output the string in pad, and make it a string
 *	of blanks...
 */
putpad(string) char *string; {

	puts(string) ;
	strcpy(string, "       ") ;
	}

/*
 * bytedump - dump a string of LINELEN bytes as hex
 */
bytedump(a) char *a; {
	char x ;

	x = LINELEN ;
	while (x--)
		printf("%02x ", *a++) ;
	putchar('\n') ;
	}

/*
 * ascdump - dump a string of LINELEN bytes as ascii
 */
ascdump(a) char *a; {
	char x, c, c2, low, hflg, cflg ;

	for (x = LINELEN; x--; a++) {
		hflg = *a & 0x80 ;
		cflg = (low = *a & 0x7f) < 0x20 ;
		if (low != 0x7f)
			c2 = cflg ? (*a & 0x5f) | 0x40
				  : low ;
		else {
			c2 = ' ' ;
			cflg++ ;
			}
		c = " |^$"[(hflg ? 1 : 0) + (cflg ? 2 : 0)] ;
		printf("%c%c ", c, c2) ;
		}
	putchar('\n') ;
	}

/*
 * worddump - dump a string of LINELEN bytes as words
 */
worddump(a) int *a; {
	char x ;

	x = LINELEN / 2;	/*only half as many if words */
	while (x--)
		printf("%04x  ", *a++) ;
	putchar('\n') ;
	}

/*
 * usage - print usage message & exit
 */
usage()
{
	puts("usage: xd [-wba] <filename>\n") ;
	exit(1) ;
}
               