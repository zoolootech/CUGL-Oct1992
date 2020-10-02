#include	<stdio.h>

FILE	*seekfp,		/* the address file */
	*fortfp;		/* the actual fortune file */

#include 	<quip.h>

openfiles()
{
	seekfp = fopen(seekname, "w");
	if (seekfp == 0) {
		puts("Cannot open address file.");
		exit(0);
	}
	fortfp = fopen(quipname, "r");
	if (fortfp == 0) {
		puts("Cannot open fortunes file.");
		exit(0);
	}
}

putaddr(addr)
long	addr;
{
	if (fwrite((char *)&addr, sizeof(long), 1, seekfp) < 1)
		printf("write error on address file\n");
}

int figureforts()
{
	int	c,
		count = 0;
	char	line[128];

	putaddr(0L);
	for (;;) {
		if (fgets(line, 128, fortfp) == 0)
			return count;
		if (line[0] == '.') {
			putaddr((long)ftell(fortfp));
			count++;
		}
	}
}

closefiles()
{
	fclose(seekfp);
	fclose(fortfp);
}

main()
{
	openfiles();
	printf("files open\n");
	printf("there are %d quips",figureforts());
	closefiles();
}
