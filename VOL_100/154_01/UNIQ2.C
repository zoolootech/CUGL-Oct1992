/* uniq2.c:	filters-out ALL repeated lines (keeps 1st occurrence) -

------
	(c) Chuck Allison  May 1985
------

*/

#include <stdio.h>
#define MAXWIDTH 256
#define MAXLINES 500
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
			{
			    uniq(f);
				fclose(f);
			}
	}
}

uniq(f)
FILE *f;
{
	int i, n, j, temp, idx[MAXLINES], mark[MAXLINES];
	char *a[MAXLINES], s[MAXWIDTH];

	/* ..read lines into memory.. */
	for (n = 0; fgets(s,MAXWIDTH-1,f) != NULL; ++n)
	{
		a[n] = (char *) malloc(strlen(s)+1);
		if (a[n] == NULL || n == MAXLINES)
		{
			printf(stderr,"memory failure\n");
			exit(1);
		}
		strcpy(a[n],s);
		idx[n] = n;		/* ..identity map for indices.. */
		mark[n] = 0;	/* ..initialize line as unmarked.. */
	}

	/* ..sort with indices (must use a stable algorithm).. */
	for (i = n-2; i >= 0; --i)
		for (j = i+1; j < n; ++j)
		{
			if (strcmp(a[idx[j-1]],a[idx[j]]) <= 0)
				break;
			temp = idx[j];
			idx[j] = idx[j-1];
			idx[j-1] = temp;
		}

	/* ..mark duplicates.. */
	for (i = 1; i < n; ++i)
		if (strcmp(a[idx[i-1]],a[idx[i]]) == 0)
			mark[idx[i]] = 1;

	/* ..print unmarked lines.. */
	for (i = 0; i < n; ++i)
		if (!mark[i])
			fputs(a[i],stdout);
}

