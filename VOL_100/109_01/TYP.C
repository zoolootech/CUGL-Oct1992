 /*  This program will simply display an ASCII file on the
console using as large a buffer as possible.  When I TYPE a
long file the constant disk accessing drives me crazy! This
allows me to examine the file in peace!  NB*128 is the
buffer size in bytes.  Make it as large as possible for your
system. Here it is used as a 32K bufsize.
*/
#define NB 256	
#define CPMEOF -1
#define EOF 26
#define CR 13
char ibuf[NB*128+8];
main (argc, argv)
char **argv;
int fd,c,argc;
{
	fd = FOpenBig (argv[1],ibuf,NB);
	if (fd == -1)  {
		printf("\nCan't open file !");
		exit ();
	}
	while ((c = GetcBig(ibuf)) != EOF && c != CPMEOF)    {
		if (c == CR)
			continue;
		else
		putchar(c);
	}
	close(fd);
	exit();
}
