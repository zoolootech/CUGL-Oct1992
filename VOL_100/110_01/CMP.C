#include "BDSCIO.H"
#define CPM 1
FILE ina, inb;

char Ascii;
char Verbose;
char eof;
int npats;
char **patts;
unsigned line, addr;
int ca,cb, cc;

main(argc, argv)
char **argv;
{
	char *cp;

	eof=Ascii=Verbose=0;
	npats=line=addr=0;

	while (--argc) {
		cp = *++argv;
		if(*cp == '-') {
			while( *++cp) {
#ifdef CPM
				switch(tolower(*cp)) {
#else
				switch(*cp) {
#endif
				case 'a':
					Ascii++; break;
				case 'l':
					Verbose++; break;
				default:	
					goto usage;
				}
			}
		}
		else if( !npats && argc) {
			if(argv[0][0]) {
				npats=argc;
				patts=argv;
			}
		}
	}
	if(npats != 2) {
usage:
		printf("Usage: cmp [-al] filea fileb\n");
		exit(9);
	}
	if(fopen(patts[0], ina)==ERROR || fopen(patts[1], inb)==ERROR)	{
		printf("Can't open %s and %s\n", patts[0], patts[1]);
		exit(8);
	}

	for(;;) {
		++addr;
		if((ca=getc(ina))==(cb=getc(inb))) {
#ifdef CPM
			if(Ascii && cb==CPMEOF)
				exit(0);
#endif
			if(cb=='\n')
				line++;
			if(cb != EOF)
				continue;
			else
				exit(0);
		} else {
#ifdef CPM
			if(Ascii) {
				if(ca==CPMEOF)
					ca=EOF;
				if(cb==CPMEOF)
					cb=EOF;
			}
#endif
	
			if(ca==EOF)
				eof=1;
			if(cb==EOF)
				eof=2;
			if(eof)
				printf("%s is shorter", patts[eof-1]);
			else
				printf("%s=0%o  %s=0%o", patts[0], ca, patts[1], cb);
			printf(" At line %d character 0%o Record %d\n",line,  addr, addr/SECSIZ);
			if( eof || !Verbose)
				exit(1);
		}
	}
}
