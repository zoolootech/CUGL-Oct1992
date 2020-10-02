/*
	detab.c:  change tabs to blanks appropriately -
		  (uses standard i/o only)
*/

#include <stdio.h>

#define TABSPACE 4
#define MAXLINE 256

main()
{
	int c, col, tabstops[MAXLINE];

	settabs(tabstops);
	col = 1;
	while ((c = getchar()) != EOF) 
		if (c == '\t')
			do {
				putchar(' ');
				++col;
			} while (!tabstops[col]);
		else if (c == '\n')
		{
			putchar('\n');
			col = 1;
		}
		else
		{
			putchar(c);
			++col;
		}
}

settabs(tabstops)
int tabstops[];
{
	int i;

	for (i = 0; i < MAXLINE; ++i)
		tabstops[i] = (i % TABSPACE == 1);
}
