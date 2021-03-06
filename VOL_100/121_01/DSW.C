/*
HEADER: CUG 121.??;

    TITLE:	Dsw - delete files selectively;
    VERSION:	1.0;
    DATE:	09/01/85;
    DESCRIPTION: "This program lists a partial or entire directory, one entry
		at a time, on the console and prompts for each file to be
		deleted.  Optionally, the directory entries may be sorted
		before prompting."
    KEYWORDS:	delete, selective;
    SYSTEM:	CP/M;
    FILENAME:	DSW.C;
    WARNINGS:	"Requires files.c for link.  Handles a maximum of 128 files
		(easily changed).";
    AUTHORS:	Cal Thixton;
    COMPILERS:	BDS-C 1.50;
*/

#include <bdscio.h>

#define	MAXFILES 128	/* maximum # of files to store	*/

main(argc,argv)
char	*argv[];
int	argc;
{
	char	*files[MAXFILES];
	char	leftover,sflag;		/* to sort or not to sort	*/
	int	cmp();			/* declare here for qsort	*/
	int	i,j;

	_allocp = NULL;	   /* initialize allocation pointer */
	leftover = sflag = 0;
	if (argv[1][0] == '-' && argv[1][1] == 'S')
		sflag = 1;
	if (argc > 1 + sflag)
		for (j=sflag+1; j<argc && leftover<MAXFILES-1; j++)
			{
			files[leftover]=alloc(strlen(argv[j])+1);
			strcpy(files[leftover++],argv[j]);
			leftover=nameok(leftover,files,MAXFILES);
			}
	else	{
		files[leftover]=alloc(15);
		strcpy(files[leftover++],"????????.???");
		leftover=nameok(leftover,files,MAXFILES);
		}
	if (sflag)
		qsort(files,leftover,2,&cmp);
	if (!leftover) {
		printf("No files to delete.\n");
		exit(0);
		}
	for (j=0; j<leftover; j++)
		{
		printf("delete %-12s (y/n) ? ",files[j]);
		if (toupper(getchar()) == 'Y')
			unlink(files[j]);
		putchar('\n');
		}
}

cmp(a,b)
char	**a,**b;
{
	return(strcmp(*a,*b));
}
                                                                                                                               