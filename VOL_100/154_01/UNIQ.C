/* uniq.c:	filters-out contiguous, duplicate lines */

/* Adapted from Software Tools, by Chuck Allison, Apr 1985 */

#include <stdio.h>
#define MAXLINE 256
#define MAXFILES 150

main(argc,argv)
int argc;
char *argv[];
{
	int i, maxarg = MAXFILES, xargc;
	char *xargv[MAXFILES];
	FILE *f;

	if (argc == 1)
		uniq(stdin);
    else
	{
		/* expand filespecs (Mark Williams C only!).. */
		xargc = exargs("files",argc,argv,xargv,maxarg);

	    for (i = 0; i < xargc; ++i)
	        if ((f = fopen(xargv[i],"r")) == NULL)
			{
		        printf("can't open: \"%s\"\n",xargv[i]);
		        exit(1);
		    } 
			else
			    uniq(f);
	}
}

uniq(f)
FILE *f;
{
	char line1[MAXLINE], line2[MAXLINE], *l1, *l2;

	l1 = fgets(line1,MAXLINE-1,f);    /* ..get first line.. */

	while (l1 != NULL)
	{
		fputs(line1,stdout);

		/* ..skip duplicates of line1.. */
		while ((l2 = fgets(line2,MAXLINE-1,f)) != NULL)
			if (strcmp(line1,line2) != 0) break;

		/* ..check EOF.. */
		if (l2 == NULL) break;

		fputs(line2,stdout);

		/* ..skip duplicates of line2.. */
		while ((l1 = fgets(line1,MAXLINE-1,f)) != NULL)
			if (strcmp(line1,line2)) break;
	}
	fclose(f);
}
