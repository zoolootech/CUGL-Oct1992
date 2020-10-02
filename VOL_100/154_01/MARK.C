/* mark.c:	underline C reserved words from standard input */

#include <stdio.h>
#include <ctype.h>

char line[81], nextline[81];
int pos;

main()
{
	int start, i;
	char word[21];

	while (gets(line) != NULL) {
		/* ..clear nextline.. */
		reset();

		/* ..process each line in turn.. */
		while ((start = getword(word)) >= 0)
			if (search(word))
				/* .. a reserved word was found - mark nextline.. */
				for (i = start; i < start+strlen(word); ++i)
					nextline[i] = '_';
                nextline[strlen(line)+1]= '\0';

		/* ..print line and underscores, if any.. */
		printf("%s\r%s\n",line,nextline);
	}
}

reset()			/* ..clear nextline - start at beginning of line.. */
{
	int i;
	
	for (i = 0; i < 80; ++i)
		nextline[i] = ' ';
	pos = 0;
}

int search(s)	/* ..see if s is a reserved word.. */
char *s;
{
	static char *reserved[] = { "auto", "break", "case", "char",
		"continue", "default", "do", "double", "else", "extern",
		"float", "for", "goto", "if", "int", "long", "register",
		"return", "short", "sizeof", "static", "struct", "switch",
		"typedef", "union", "unsigned", "while" };
	int i, flag;

	for (i = 0; i < 27 && (flag = strcmp(reserved[i],s)) <= 0; ++i)
		if (flag == 0)
			/* ..s is a reserved word.. */
			return 1;

	return 0;
}

int getword(s)
char *s;
{
	int c, start, p;

	for (c = pos; line[c] != '\0' && !islower(line[c]); ++c) ;

	if (line[c] == '\0')
	 	/* ..no more words on line.. */
		return EOF;

	/* ..build word.. */
	start = c, p = 0;
	while (islower(line[c]))
		s[p++] = line[c++];
	s[p] = '\0';

	/* ..save position in line.. */
	pos = c;

	/* ..return start position of line.. */
	return start;
}
