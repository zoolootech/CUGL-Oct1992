/*
	Binary to HEX-ASCII utility

	Makes a binary file into a HEX file, 32 bytes/block with
	proper sumcheck byte at the end. The starting address for the
	HEX file is settable; the input is assumed to be contiguous
	locations. Some garbage may be included at the end of the
	HEX file since the program does not recognize 0x1A as an
	end-of-file code in the binary input.

	Called with:
		bin2hex [<binary file 1> [<bin. file 2>...]]

	Nothing special required, but if you have NSECTS
	set larger than 1 you may want to include the
	modified "getc" for better end-of-file indication.

written by:
	Robert Pasky
	36 Wiswall Rd
	Newton Centre, MA 01259
*/

#include "bdscio.h"

#define CTRLQ 0x11	/* quit key: control-q */

int i, j, fd;
int addr,sad;
char fnbuf[30], *fname;		/* filename buffer & ptr */
char onbuf[30], *oname;		/* output filename buf & ptr */
char ibuf[BUFSIZ];	/* buffered input buffer */
char obuf[BUFSIZ];	/* buffered output */
char hextab[16];
char kybdbuf[20];
char sumcheck;

main(argc,argv)
char **argv;
{
	strcpy(hextab,"0123456789ABCDEF");

	while (1)
	{
		oname = onbuf;
		if (argc-1)
		 {
			fname = *++argv;
			argc--;
		 }
		else
		 {
			printf ("\nEnter binary file name: ");
			if (!*(fname = get