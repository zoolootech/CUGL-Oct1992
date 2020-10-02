static char *progid = { "Expose v1.1 by Michael Yokoyama" };

/*  EXPOSE.C  This command exposes messages in executable files
*/

#include <stdio.h>
#include <ctype.h>

main(argc,argv)
int argc;			/* Number of command line words	  */
char *argv[];			/* Pointers to command line words */
{
	int c;
	FILE *in;		/* File	used for input		  */

	if (argc != 2) {	/* Check if enough arguments	  */
		fprintf(stderr,"Usage:	expose filename\n");
		exit(1);
	}

	if ((in = fopen(argv[1],"r")) == NULL) {
		fprintf(stderr,"expose:  Can't open %s\n",argv[1]);	
		exit(2);
	}

	while ((c = getc(in)) != EOF)
		if (isprint(c))
			putchar(c);
	fclose(in);
}
