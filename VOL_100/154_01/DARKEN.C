/* darken.c:  overstrike text lines for darker output -

-----
	(c) Chuck Allison, 1985
-----
*/

#include <stdio.h>

#define MAXLINE 256
#define MAXFILES 150

int strikes = 3;			/* ..default line-print count.. */

main(argc,argv)
int argc;
char *argv[];
{
	FILE *f;
	int i,
		xargc,				/* ..expanded arg count (after options).. */
		maxarg = MAXFILES;	/* ..max allowable args after expanding.. */
	char *xargv[MAXFILES];	/* ..arg vector after expanding.. */

	/* ..process switches.. */
	if (argv[1][0] == '-')
	{
		strikes = atoi(argv[1]+1);
		++argv;
		--argc;
	}

	/* ..process files.. */
	if (argc == 1)
		dk(stdin,"");
	else
	{
		/* ..expand filespecs.. (Mark Williams only!) */
		xargc = exargs("",argc,argv,xargv,maxarg);

		/* ..print each file.. */
		for (i = 0; i < xargc; ++i)
			if ((f = fopen(xargv[i],"r")) != NULL)
			{
				dk(f,xargv[i]);
				fclose(f);
			}
			else
				fprintf(stderr,"can't open: %s\n",xargv[i]);
	}
}

dk(f)
FILE *f;
{
	int i;
	char *line;

	line = (char *) malloc(MAXLINE);

	while (fgets(line,MAXLINE-1,f) != NULL)
	{
		/* ..check for form-feed at line start.. */
		if (*line == '\f')
		{
			putchar('\f');
			line++;
		}
		line[strlen(line)-1] = '\0';	/* zap \n */
		for (i = 0; i < strikes; ++i)
			printf("%s\r",line);
		putchar('\n');
	}
}
