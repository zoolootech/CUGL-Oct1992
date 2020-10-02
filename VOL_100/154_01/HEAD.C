/* head.c:	display first n lines of text files -

-----
	(c) Chuck Allison, 1985
-----
*/

#include <stdio.h>
#define MAXLINE 80
#define MAXFILES 150

main(argc,argv)
int argc;
char *argv[];
{
	int i, count, n = 5, xargc; 
	char line[MAXLINE], *xargv[MAXFILES];
	FILE *f;

	if (argc < 2)
	{
		puts("usage:  head [-n] file1 file2 ...");
		exit(1);
	}

	/* ..process optional head count.. */
	if (argv[1][0] == '-')
	{
		if (argc == 2)
		{
			puts("usage:  head [-n] file1 file2 ...");
			exit(1);
		}
		n = atoi(argv[1]+1);
		++argv;
		--argc;
	}

	/* ..expand filespecs (Mark Williams C only!).. */
	xargc = exargs("files",argc,argv,xargv,MAXFILES);

	for (i = 0 ; i < xargc; ++i)
		if ((f = fopen(xargv[i],"r")) != NULL)
		{
			printf("-----  %s  -----\n",xargv[i]);

			/* ..get lines.. */
			for (count=0; count<n && fgets(line,MAXLINE-1,f); count++)
				fputs(line,stdout);

			puts("-----  end  -----\n\n\n");
			fclose(f);
		}
		else
			fprintf(stderr,"couldn't open %s\n",xargv[i]);
}
