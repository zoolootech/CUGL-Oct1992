/* UNVANR.C    VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:37:29 PM */
/*
%CC1 $1.C -X -E6000
%CLINK $1 DIO -S -E6000
%DELETE $1.CRL 
*/
/* 
Description:

Filter to convert Van Nuys Toolkit format back to standard C tokens.

DIO redirection.

Adapted from Van Nuys Toolkit program CASE, by Eugene H. Mallory.

By J.A. Rupley, Tucson, Arizona
Coded for BDS C compiler, version 1.50a
*/

#include "bdscio.h"
#include "dio.h"

#define MAXNAMES 40

int lastchar;
int preprocessor;

main(argc, argv)

char **argv;
int argc;

{
	char word[10000];
	char delimiter[1];
	int c;
	int flag;
	int i;

	char *keyword[MAXNAMES];
	int keycount[MAXNAMES];

	i = 0;
	/*
		keyword[i++] = "flush";
		keyword[i++] = "NOT";
	*/
	keyword[i++] = "AND";        /*0*/
	keyword[i++] = "OR";
	keyword[i++] = "EQUAL";
	keyword[i++] = "NOTEQUAL";
	keyword[i++] = "BEGIN";
	keyword[i++] = "THEN";        /*5*/
	keyword[i++] = "LOOP";
	keyword[i++] = "END";
	keyword[i++] = "ENDIF";
	keyword[i++] = "ENDLOOP";
	keyword[i++] = "FUNCTION";        /*10*/

	keyword[i++] = "MAIN";
	keyword[i++] = "CHAR";
	keyword[i++] = "INT";
	keyword[i++] = "REGISTER";
	keyword[i++] = "STRUCT";
	keyword[i++] = "UNION";
	keyword[i++] = "UNSIGNED";
	keyword[i++] = "BREAK";
	keyword[i++] = "CONTINUE";
	keyword[i++] = "DEFAULT";
	keyword[i++] = "IF";
	keyword[i++] = "ELSE";
	keyword[i++] = "GOTO";
	keyword[i++] = "DO";
	keyword[i++] = "FOR";
	keyword[i++] = "WHILE";
	keyword[i++] = "RETURN";
	keyword[i++] = "SIZEOF";
	keyword[i++] = "SWITCH";
	keyword[i++] = "CASE";
	keyword[i++] = "DEFINE";
	keyword[i++] = "IFDEF";
	keyword[i++] = "IFNDEF";
	keyword[i++] = "UNDEF";
	keyword[i++] = "INCLUDE";
	keyword[i++] = "$$$";


	lastchar = 0;
	for (i = 0; i < MAXNAMES; i++)
		keycount[i] = 0;

	dioinit(&argc, argv);
	if (!_dioinflg)
	{
		printf(
		"\nUSAGE:\n");
		printf(
		"     unvanuys  <vanuys_fid  [>out_fid or |DIO_pipe or default = crt]\n\n");
		printf(
		"one of three formatting filters:\n\n");
		printf(
		"unvanuys - converts keywords of Van Nuys Toolkit to standard K & R 'c'\n");
		printf(
		"cbprep - produces consistent spacing of 'c' text and one statement per line\n");
		printf(
		"         output must be filtered by cb\n");
		printf(
		"cb - indents according to control level\n\n");
		dioexit();
	}

	do
	    {
		flag = get_word(word, delimiter);
		if (preprocessor)
		{
			lower_case(word);
			puts(word);
			if (flag)
				putchar(delimiter[0]);
			continue;
		}
		for (i = 0; i < MAXNAMES; i++)
		{
			if (strequ(keyword[i], "$$$"))
				break;
			if (strequ(keyword[i], word))
			{
				keycount[i]++;
				switch (i)
				{
				case 0 :
					strcpy(word, "&&");
					break;
				case 1 :
					strcpy(word, "||");
					break;
				case 2 :
					strcpy(word, "==");
					break;
				case 3 :
					strcpy(word, "!=");
					break;
				case 4 :
				case 5 :
				case 6 :
					strcpy(word, "{");
					break;
				case 7 :
				case 8 :
				case 9 :
					strcpy(word, "}");
					break;
				case 10 :
					strcpy(word, "");
					break;
				default :
					lower_case(word);
				}
				break;
			}
		}
		puts(word);
		if (flag)
			putchar(delimiter[0]);
	}
	while (flag)
		;

	for (i = 0; i < MAXNAMES; i++)
	{
		if (strequ(keyword[i], "$$$"))
			break;
		if (keycount[i])
		{
			typef("%7d %ss\n", keycount[i], keyword[i]);
		}
	}
	dioflush();
}


int get_word(cptr, dptr)
char *cptr, *dptr;
{
	int c;
	int nest;
	int cptr_start;

	cptr_start = cptr;
	nest = 0;
	if (lastchar == '#')
		preprocessor = 1;
	else
	    preprocessor = 0;
lup :
	c = getchar();
	if (c == EOF)
	{
		*cptr++ = 0;
		*dptr = CPMEOF;
		return 0;
	}
	if (lastchar == '/' && c == '*')
	{
		lastchar = 0;
		while ((c != '/') || (lastchar != '*') || (nest > 0))
		{
			if ((lastchar == '/') && (c == '*'))
				nest++;
			if ((lastchar == '*') && (c == '/'))
				nest--;
			*cptr++ = c;
			lastchar = c;
			c = getchar();
		}
		*cptr++ = c;
		goto lup;
	}
	if (c == '"')
	{
		lastchar = '\\';
		while (c != '"' || lastchar == '\\')
		{
			*cptr++ = c;
			lastchar = c;
			c = getchar();
		}
		*cptr++ = c;
		if (c == '\\' && lastchar == '\\')
		{
			*cptr++ = c;
			lastchar = 0;
			c = getchar();
		}
		goto lup;
	}

	if (c == '\'')
	{
		lastchar = '\\';
		while (c != '\'' || lastchar == '\\')
		{
			*cptr++ = c;
			lastchar = c;
			c = getchar();
			if (c == '\\' && lastchar == '\\')
			{
				*cptr++ = c;
				lastchar = 0;
				c = getchar();
			}
		}
		*cptr++ = c;
		goto lup;
	}

	if (isalpha(c) || c == '_' || isdigit(c))
	{
		*cptr++ = c;
		goto lup;
	}
	*cptr++ = 0;
	*dptr = c;
	lastchar = c;
	return 1;
}


void lower_case(cptr)
char *cptr;
{
	while (*cptr != 0)
	{
		*cptr = tolower(*cptr);
		cptr++;
	}
}


strequ(s1, s2)
char *s1, *s2;
{
	while (toupper(*s1++) == toupper(*s2++))
		if (!*s1 && !*s2)
			return (1);
	return (0);
}
