#include	<stdio.h>

FILE	*seekfp,		/* the address file */
	*fortfp;		/* the actual fortune file */

#include	<quip.h>

long	ftell(),
	fseek();

openfiles()
{
	seekfp = fopen(seekname, "r+");
	if (seekfp == 0) {
		puts("Cannot open address file.");
		exit(0);
	}
	fortfp = fopen(quipname, "r+");
	if (fortfp == 0) {
		puts("Cannot open fortunes file.");
		exit(0);
	}
}

putaddr(addr)
long	addr;
{
	if (fwrite(&addr, sizeof(long), 1, seekfp) < 1)
		printf("write error on address file\n");
}

char	fortune[2048];

addfort()
{
	int	i,
		c;
	long	now;

	if (fseek(seekfp, 0L, 2) / 2 >= 32767L) {
		puts("Sorry, the quip file has too many entries.");
		return;
	}
	fseek(fortfp, 0L, 2);
	puts("Add your quip and end with a ^D (can be a maximum of 2048 characters):");
	i = 0;
	while ((c = getchar()) != EOF)
		fortune[i++] = c;
	time(&now);
	fprintf(fortfp, ". %s\n", getlogin());
	putaddr(ftell(fortfp));
	if (fwrite(fortune, i, 1, fortfp) != 1) {
		printf("Write failed!\n");
		exit(-1);
	}
}

closefiles()
{
	fclose(seekfp);
	fclose(fortfp);
}

main()
{
	if (!strcmp(getlogin(), "demo") || !strcmp(getlogin(), "intro")) {
		printf("Sorry, but demo cannot add quips.\n");
		exit(0);
	}
	openfiles();
	addfort();
	closefiles();
}
