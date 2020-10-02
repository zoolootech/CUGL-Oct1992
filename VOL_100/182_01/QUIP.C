#include	<dg_stdio.h>

FILE		*seekfd,		/* the address file */
		*fortfp;		/* the actual fortune file */

#include	<quip.h>

unsigned	bmap[2048];

bittst(bitno)
int	bitno;
{
	return (bmap[bitno / 16] & (1 << (bitno % 16)));
}

bitset(bitno)
int	bitno;
{
	bmap[bitno / 16] |= (1 << (bitno % 16));
}

openfiles()
{
	seekfd = fopen(seekname, "r");
	if (seekfd == 0) {
		puts("Cannot open address file.");
		exit(0);
	}
	fortfp = fopen(quipname, "r");
	if (fortfp == 0) {
		puts("Cannot open fortunes file.");
		exit(0);
	}
}

long
pickfort()
{
	unsigned	size,
			count = 0;
	int		whseek;
	long		whfort;

	if (fseek(seekfd,0l,2))
		printf("fseek screwed up\n");
	size = ftell(seekfd) / sizeof(long);
	do {
		whseek = (rand() % (size - 1)) + 1;
		if (count++ > 0177776)
			exit(-1);
	} while (bittst(whseek));
	bitset(whseek);
	fseek(seekfd, (long)whseek * (long)sizeof(long), 0);
	if (fread((char *)&whfort, sizeof(long), 1, seekfd) < 1)
		puts("Read error in address file.");
	return ((long)whfort);
}

showfort(addr)
long	addr;
{
	int	lastc = ' ',
		c;

	fseek(fortfp, addr, 0);
	for (;;) {
		c = getc(fortfp);
		if (c == '.' && lastc == '\n') {
			return;
		}
		if (c < ' ' && c != '\n' && c != '\007' && c != '\t')
			c = '@';
		putchar(c);
		lastc = c;
	}
}

closefiles()
{
	fclose(seekfd);
	fclose(fortfp);
}

main(argc, argv)
int	argc;
char	*argv[];
{
	int		times = 1;

	if (argc > 1)
		times = atoi(argv[1]);
	srand(-1);
	openfiles();
	while (times--) {
		showfort(pickfort());
		if (times)
			printf("-------\n");
	}
	closefiles();
}
