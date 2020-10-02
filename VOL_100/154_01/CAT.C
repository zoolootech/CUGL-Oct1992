/* cat.c:	concatenate files */

/*	Practically copied right out of Kernighan/Ritchie by
	Chuck Allison for Mark Williams C - 1985
*/

#include <stdio.h>
#define MAXFILES 150

main(argc,argv)
int argc;
char *argv[];
{
	FILE *fp;
	int maxarg = MAXFILES, xargc, i;
	char *xargv[MAXFILES];

	if (argc == 1)
		filecopy(stdin);
	else {
		/* ..expand filespecs (Mark Williams C only!).. */
		xargc = exargs("files",argc,argv,xargv,maxarg);

		for (i = 0; i < xargc; ++i)
			if ((fp = fopen(xargv[i],"r")) == NULL) {
				printf("can't open: %s\n",xargv[i]);
				exit(1);
			} else {
				filecopy(fp);
				fclose(fp);
			}
	}
}

filecopy(fp)
FILE *fp;
{
	int c;

	while ((c = getc(fp)) != EOF)
		putc(c,stdout);
}
