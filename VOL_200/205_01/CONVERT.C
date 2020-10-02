static char *progid = { "convert.c by Michael Yokoyama" };

#include <stdio.h>
#include <ctype.h>

main(argc,argv)
int argc;
char *argv[];
{
	int n;

	if (argc != 2) {
		fprintf(stderr,"Convert decimal to hexadecimal and octal\n");
		fprintf(stderr,"Usage:  convert <n>  (where 0 < n < 65536)\n");
		exit(0);
	}

	n = atoi(argv[1]);

	printf("Decimal  Hexadecimal  Octal\n");
	printf(" %5u       %4x    %6o\n",n, n, n);
}
