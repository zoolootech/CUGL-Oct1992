/* include.c:	Do-it-yourself file inclusion (standard input) */

/* guards against recursive inclusion; allows 7 levels of nesting..

	Adapted from Software Tools by Chuck Allison -

*/

#include <stdio.h>
#include <ctype.h>
#define MAXLINE 81
#define MAXWORD 20
#define NDEEP 8

struct
{
	char fname[MAXWORD];		/* ..file name.. */
	FILE *fp;					/* ..file pointer.. */
} infile[NDEEP];

int level;						/* ..include depth.. */

main()
{
	char line[MAXLINE], s[MAXWORD], *zap();

	/* ..start with standard input.. */
	infile[0].fp = stdin; strcpy(infile[0].fname,"");

	for (level = 0; level >= 0; --level)
	{
		while (fgets(line,MAXLINE-1,infile[level].fp) != NULL)
		{
			sscanf(line,"%9s",s);
			if (strcmp(s,"#include") != 0)
				/* ..output text line.. */
				fputs(line,stdout);
			else
			{
				/* ..attempt to include.. */
				if (level == NDEEP)
				{
					fprintf(stderr,"nest overflow.\n");      
					exit(1);
				}

				if (sscanf(line+strlen(s),"%19s",s) == 0)
					/* ..filename missing - just go on.. */
					continue;

				strcpy(s,zap(s));
				if (search(s))
				{
					/* ..self-inclusion.. */
					fprintf(stderr,"recursion on %s\n",s);
					continue;
				} 

				infile[level].fp = fopen(s,"r");
				if (infile[level].fp == NULL)
				{
					/* ..open error.. */
					fprintf(stderr,"can't open: %s\n",s);
					--level;
				}
			}
		}
		if (level > 0)
			fclose(infile[level].fp);
	}
}

char *zap(s)				/* ..strip "" or <> from filename.. */
char *s;
{
	int l;

	if (s[l=strlen(s)-1] == '\"' || s[l] == '>')
		s[l] = '\0';
	return (s[0]=='\"' || s[0]=='<') ? s+1 : s;
}

int search(s)				/* ..check for a recursive include.. */
char *s;
{
	int i;

	/* ..(not-so) quick and dirty search.. */
	for (i=0; i <= level; ++i) 
		if (strcmp(s,infile[i].fname) == 0)
			/* ..found it - recursive include request.. */
			return 1;
	/* ..add filename to list of open files.. */
	strcpy(infile[++level].fname,s);
 	return 0;
}
