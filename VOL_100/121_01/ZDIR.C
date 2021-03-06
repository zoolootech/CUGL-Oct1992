/*
HEADER: CUG 121.??;

    TITLE:	Zdir - produce a directory listing;
    VERSION:	1.0;
    DATE:	12/01/85;
    DESCRIPTION: "This program produces a directory list on the console,
		with six directory entries per line.  Optionally, the
		output may be sorted and/or written to a disk file as well."
    KEYWORDS:	sorted, directory;
    SYSTEM:	CP/M;
    FILENAME:	ZDIR.C;
    WARNINGS:	"Requires files.c for link. Handles a maximum of 128 files
		(easily changed).";
    SEE-ALSO:	SDIR.C (another directory lister);
    AUTHORS:	Steve Blasingame;
    COMPILERS:	BDS-C 1.50;
*/

/*	Flags:
 *	-S 	sort file names.
 *	-F	place file names in file DIR.DIR.
 */

#include <bdscio.h>

#define	MAXFILES 128	/* maximum # of filenames to store	*/

char	fd[BUFSIZ];	/* i/o area	*/


main(argc,argv)
char	*argv[];
int	argc;
{
	char	*files[MAXFILES];
	char	leftover;
	char	fflag,sflag;		/* to sort or not to sort */
	int	cmp();			/* declare here for qsort	*/
	int	i,j;

	_allocp = NULL;		/* initialize allocation pointer */
	leftover = fflag = sflag = 0;
	i = 1;			/* argument being processed */
	while (argv[i][0] == '-')
		{
		for(j=1;argv[i][j];j++)
			switch (argv[i][j]) {
			case 'F':	/* place listing in DIR.DIR	*/
				fflag=1;
				break;
			case 'S':	/* sort file names		*/
				sflag=1;
				break;
			default:
				usage();
				exit();
			}
		i++;
		}

	if (fflag && fcreat("DIR.DIR",fd)==ERROR) {
		printf("Cannot create DIR.DIR\n");
		exit();
		}

	if (argc > i)
		for(j=i;j<argc && leftover<MAXFILES-1;j++) {
			files[leftover]=alloc(strlen(argv[j])+1);
			strcpy(files[leftover++],argv[j]);
			leftover=nameok(leftover,files,MAXFILES);
			}
	else {
		files[leftover]=alloc(15);
		strcpy(files[leftover++],"????????.???");
		leftover=nameok(leftover,files,MAXFILES);
		}

	if (sflag)
		qsort(files,leftover,2,&cmp);

	if (!leftover) {
		printf("No files to print.\n");
		fclose(fd);
		exit(0);
		}

	if (fflag) {
		for(j=0;j<leftover;j++)
			fprintf(fd,"%s\n",files[j]);
		putc(CPMEOF,fd);
		fflush(fd);
		fclose(fd);
		}

	for (j=0; j<leftover; j++)
		printf("%-12s%c",files[j],((j+1)%6)?' ':'\n');
	putchar('\n');
	printf("%d: Files. %s",leftover,(fflag)?"DIR.DIR done.\n":"\n");
}

cmp(a,b)
char	**a,**b;
{
	return(strcmp(*a,*b));
}

usage()
{
	printf("Usage: zdir -[f][s] [<filename>] ...\n");
}
                     