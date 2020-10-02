#include "A:BDSCIO.H"
/*
 * see prints one or more ufn's unambiguously
 * Chuck Forsberg Computer Development Inc
 */
int c;
FILE in;

main(argc,argv)
int argc;
char *argv[];
{
	int ac;
	for (ac=1;ac < argc;ac++) {
		if(fopen(argv[ac],in)==ERROR)
			continue;
		else
			printf("Listing '%s'\n", argv[ac]);
		for(;;) {
			c=getc(in);
next:
			if(c == EOF || c == CPMEOF)
				break;
			if(c >= 040 && c < 0177 )
				putchar(c);
			else
				switch(c) {
				case 012:
					printf("<012>\n"); continue;
				case 015:
					if((c=getc(in))==012) {
						printf("$\n"); continue;
					} else {
						printf("<015>"); goto next;
					}
				default:
					printf("<%o>", c); continue;
				}
		}
	}
}
√ª:*ﬂ;Î!ÔEz≥»’^#V#~#ÂfoÎÕ	;·#—√Ò:DM*