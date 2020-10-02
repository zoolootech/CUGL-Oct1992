/*
	pr.c:  detabbing file printer -

		usage:  pr [-l] [-n#] [-t#] [-h] [file1] file2] ...
		options:
				-l		print with line numbers
				-n#		print # lines per page (4-line header xtra)
				-t#		set tab to # spaces (default 4)
				-h		do not print a page header
		Wildcards, drives, and paths are all processed correctly.

		Adapted from Software Tools by Chuck Allison - Apr 1985
*/

#include <stdio.h>
#include <ctype.h>
#include <time.h>

#define MAXLINE 256
#define MAXFILES 150
#define yes 1
#define no 0

/* ..set default parameters.. */
int number = no,
	lines_per_page = 55,
	tabspace = 4,
	headers = yes;

/* ..date and time definitions.. */
unsigned mon,
	day,
	year,
	hour,
	min,
	sec;

char datestr[13],
	timestr[6],
	weekday[4],
	am_pm[3];

int tabstops[MAXLINE];

main(argc,argv)
int argc;
char *argv[];
{
	FILE *f;
	int i,
		xargc,				/* ..expanded arg count (after options).. */
		maxarg = MAXFILES;	/* ..max allowable args after expanding.. */
	char *s,
		*xargv[MAXFILES];	/* ..arg vector after expanding.. */

	/* ..process switches.. */
	for ( ; *(s = *(argv+1)) == '-'; ++argv, --argc)
	{
		while (*++s)
			switch(tolower(*s))
			{
				case 'l':
					number = yes;
					break;
				case 'n':
					lines_per_page = atoi(s+1);
					goto next_arg;
				case 't':
					tabspace = atoi(s+1);
					goto next_arg;
			  	case 'h':
					headers = no;
					break;
				default :
					fprintf(stderr,"unknown switch: -%c\n",*s);
					exit(1);
			}
		next_arg: /* ..cycle on outer for().. */ ;
	}

	/* ..initialize tab settings.. */
	settabs();

	/* ..get date and time.. */
	date(&year,&mon,&day,weekday);
	sprintf(datestr,"%3s %2d/%02d/%02d",weekday,mon,day,year%1900);
	time(&hour,&min,&sec);
	strcpy(am_pm , (hour >= 12) ? "pm" : "am");
	hour = (hour == 12) ? hour : hour % 12;
	sprintf(timestr,"%2d:%02d %2s",hour,min,am_pm);

	/* ..process files.. */
	if (argc == 1)
		pr(stdin,"");
	else
	{
		/* ..expand filespecs.. */
		xargc = exargs("",argc,argv,xargv,maxarg);

		/* ..print each file.. */
		for (i = 0; i < xargc; ++i)
			if ((f = fopen(xargv[i],"r")) != NULL)
			{
				pr(f,xargv[i]);
				fclose(f);
			}
			else
				fprintf(stderr,"can't open: %s\n",xargv[i]);
	}
}

pr(fp, name)
FILE *fp;
char *name;
{
	int lineno, pageno, offset;
	char line[MAXLINE];

	offset = 0;	/* ..# pages already printed * lines_per_page.. */
	pageno = 1;
	lineno = 1;
	if (headers)
		header(name,pageno);
	else
		printf("\n\n\n\n\n");

	while (fgets(line,MAXLINE-1,fp) != NULL)
	{
		if (lineno == 0)
		{
			if (headers)
				header(name,++pageno);
			else
				printf("\n\n\n\n\n");
			lineno = 1;
		}

		/* ..print line number, if requested.. */
		if (number)
			printf("%8d  ",offset+lineno);
		else
			printf("       ");
		lineno++;

		/* ..output detabbed line.. */
		detab(line);

		/* ..check for page break.. */
		if (lineno > lines_per_page)
		{
			putchar('\f');
			offset += lines_per_page;
			lineno = 0;
		}
	}
	/* ..form-feed after last partial page.. */
	if (lineno > 0) putchar('\f');
}

header(file,page)
char *file;
int page;
{
	int i;

	printf("\n    ");
	for (i = 0; i < 75; ++i)
		putchar('-');
	printf("\n    %s                %s       %s",
	  file,datestr,timestr);
	for (i = 1; i < 24-strlen(file); ++i)
		putchar(' ');
	printf("Page %4d\n    ",page);
	for (i = 0; i < 75; ++i)
		putchar('-');
	printf("\n\n\n");
}

detab(line)
char *line;
{
	int i, col;

	col = 1;

	/* ..note: line[] has a terminating '\n' per fgets().. */
	for (i = 0; i < strlen(line); ++i)
		if (line[i] == '\t')
			/* ..tab.. */
			do
			{
				putchar(' ');
				++col;
			} while (!tabstops[col]);
		else
		{
			putchar(line[i]);
			++col;
		}
}

settabs()
{
	int i;

	for (i = 0; i < MAXLINE; ++i)
		tabstops[i] = (i % tabspace == 1);
}
