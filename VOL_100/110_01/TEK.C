/* tek.c puts LINES lines on the screen then gets more while user */
/* is reading the previous. */

#define LINES 36
#include "BDSCIO.H"
int c;
char *s;
char first;
FILE in;
char *buf;
char lines;

main(argc,argv)
int argc;
char *argv[];
{
	int ac;
	buf= sbrk(0);
	first=1;
	for (ac=1;ac < argc;ac++) {
		if(fopen(argv[ac], in)==ERROR) {
			printf("Can't open %s\n", argv[ac]);
			continue;
		}
ragain:
		s=buf;
		lines=LINES;
		while((c=getc(in)) != EOF && c != CPMEOF && c != 0)
		{
			*s++ =c;
			if(c == '\n' && --lines == 0)
				break;
		}
		*s++ =0;
		if( !first)
			getchar();
		first=0;
		printf("\033\014");
		sleep(20);
		for( s=buf; *s; )
			putchar(*s++);
		if(c != CPMEOF && c != EOF)
			goto ragain;
		printf("<<<< EOF >>>>");
	}
	printf(".... END ....");
}
