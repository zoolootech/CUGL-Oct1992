/*	Symbol Table Sort

	Written by
	Bob Pasky
	36 Wiswall Rd.	Newton Centre, MA 02159
	Makes BDS-C .SYM file compatible with VBUG symbol table.

	VBUG is mnemonic & symbolic debugger for Z-80(tm, Zilog Inc.)
	micros which I wrote. It displays its trace addresses as
	a symbol plus offset, and allows the user to reference a
	location in the same manner. So that the trace runs as fast as
	possible, the symbol table is ordered by the address of the
	symbol, and vbug does a linear search of the addresses until
	it finds one which is greater than the address to be displayed
	and backs off to the previous symbol. It will then print that
	symbol and a hex byte for the difference between the actual
	address and the symbol's address, e.g., "FOOBAR +0A:".
	In vbug, as in my assembler, only the first 7 characters are
	significant -- I must take in to account that fact when creating
	symbol names in my C programs.

	The format for VBUG's symbol file is:
		2 bytes: address (low byte-high byte order),
		1 byte: symbol length + 3,
		1-7 bytes: symbol characters;
	Addresses must be sorted in ascending order.
	An address field of 0xFFFF ends table.

	The program reads the .sym file produced by "Clink", storing
	the symbols and their addresses in "sym" and "ad" arrays,
	respectively. The addresses and an index into "sym" array
	are sorted using the "qsort" function. The .sym file is
	renamed to .csy, and a new .sym file is opened (vbug uses the
	.sym type also). Finally, the symbols and their addresses are
	written to this file in the format described above.

	The program is called by:
		A>symbug [<symbolfile> [<symbolfile> ...] ]
	where <symbolfile> is the name of the .sym file produced by
	"Clink". If no name is given at invocation of the program,
	a prompt will be printed requesting a <symbolfile> name.
	Entering simply <CR> at the prompt will cause the program
	to exit().

	This program may not be DIRECTLY useable by anyone else, but
	it demonstrates some simple string manipulation functions.
	One feature which should be noted is that the string data
	are not moved around in memory; only a pointer to the
	string (actually the index number into the array) is moved.
	When the sorting is complete, reading the index numbers
	sequentially from the array "ad" gives the string array index in
	the order desired. (Remember, this program outputs by ascending
	address field; the symbols already happen to be in alphabetical
	order.)

*/

#include "bdscio.h"

#define CTRLQ 0x11	/* quit key: control-q */
			/* YOU may want to change this to control-c */

#define MAXSYM 200	/* max number of symbols expected in file */
#define MAXSYMLEN 10	/* max symbol length expected from input */
#define SYMLEN 7	/* max symbol length to be output */

struct adtab {
	int v;		/* value */
	int o;		/* offset into sym */
	} ad[MAXSYM];	/* max number of symbols expected */

char sym[MAXSYM][MAXSYMLEN];	/*array of symbol names */

main(argc,argv)
char **argv;
{
	int i, j, fd;
	int total;
	char fnbuf[30], *fname;		/* filename buffer & ptr */
	char onbuf[30], *oname;		/* output filename buf & ptr */
	char ibuf[BUFSIZ];		/* buffered input buffer */
	char *gets();
	char tempstr[80];		/* size of line produced by CLINK */
	int cmpadd();

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
			printf ("\nEnter SYM file name: ");
			if (!*(fname = gets(fnbuf))) break;
		}
		strcpy (oname,fname);
		strcat (fname,".SYM");

		if ((fd = fopen(fname,ibuf)) == ERROR)
		{
			printf("Can't open %s\n",fname);
			continue;
		}
		else	printf("\nReading %-13s\n",fname