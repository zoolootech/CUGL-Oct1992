/* rm.c: remove files

-----
	(c) Chuck Allison, 1985
-----
*/

#include <stdio.h>
#include <ctype.h>
#define NLOOK 5
#define MAXFILES 150
#define MAXLINE 256
#define ESC 27
#define yes 1
#define no 0

main(argc,argv)
int argc;
char *argv[];
{
	int i,
		k,
		go = no,
		delete = yes,
		xargc;
	char *xargv[MAXFILES];
	static char prompt[] = "files to delete (^Z when done)\n";

	/* ..get optional "go" switch.. */
	if (argv[1][0] == '-')
		if (tolower(argv[1][1]) == 'g')
		{
			go = yes;
			argv++;
			argc--;
		}
		else
		{
			fprintf(stderr,"unknown switch: -%c\n",argv[1][1]);
			exit(1);
		}

	/* ..expand wildcards, if any.. (Mark Williams C only!) */
	xargc = exargs(prompt,argc,argv,xargv,MAXFILES);

	for (i = 0; i < xargc; ++i)
	{
		if (!exist(xargv[i]))
		{
			printf("%s not found\n",xargv[i]);
			continue;
		}
		delete = yes;
again:
		if (go == no)
		{
			printf("\nDelete %c[7m%s%c[0m? (y,n,p,g,q): ",
			  ESC,xargv[i],ESC);
			fflush(stdout);
			k = getkey();
			switch (tolower(k))
			{
				case 'g':
					go = yes;
				case 'y':
					delete = yes;
					break;
				case 'n':
					delete = no;
					break;
				case 'p':
					look(xargv[i]);
					goto again;
				case 'q':
					exit(0);
				default :
					goto again;
			}
		}
		if (delete)
		{
			if (unlink(xargv[i]) == 0)
				fprintf(stdout,"%s deleted\n",xargv[i]);
			else
				fprintf(stdout,"error deleting %s\n",xargv[i]);
		}
	}
}

look(s)			/* ..review first NLOOK (5) lines of file.. */
char *s;
{
	FILE *f;
	int i;
	char l[MAXLINE];

	putchar('\n');
	if ((f = fopen(s,"r")) == NULL) 
		printf("can't open %s\n",s);
	else
	{
		printf("\n***** %s: *****\n",s);
		for (i = 0; i < NLOOK && fgets(l,MAXLINE-1,f); ++i)
			fputs(l,stdout);
		fclose(f);
		printf("********************\n\n");
	}
}

int exist(s)
char *s;
{
	FILE *f;
	int found;

	if ((f = fopen(s,"r")) == NULL)
		found = no;
	else
	{
		found = yes;
		fclose(f);
	}
	return found;
}
