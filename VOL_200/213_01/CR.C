/* CR.C        VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:36:24 PM */
/*
%CC1 $1.C -X
%CLINK $1 -N
%DELETE $1.CRL
*/
/* 
Description:

Opens fname.c, extracts lines with key "%", constructs and runs submit file.
Simplifies compilation and linking in C program development.

From Van Nuys Toolkit, by Eugene H. Mallory.
No changes except converted to standard C tokens.

By J.A. Rupley, Tucson, Arizona
Coded for BDS C compiler, version 1.50a
*/
/*********************************************************************
*                               C                                    *
**********************************************************************
*                  COPYRIGHT 1983 EUGENE H. MALLORY                  *
*********************************************************************/
#include "BDSCIO.h"
#define BCOUNT 200
char string[MAXLINE];
char fname[MAXLINE], fname2[MAXLINE];
char c;
int fd1, fd2;
FILE fcb1;
char barray[BCOUNT][MAXLINE];
char buffer[128];
int bctr, flag;
main(argc, argv)
char **argv;
int argc;
{
	char *n, fname[MAXLINE];
	FILE fcb;
	int i, j, len, flag;
	if (argc == 1)
	{
		if (fopen("CNAME.$$$", fcb) == -1)
		{
			printf("E: Unable to open file CNAME.$$$.\n");
			exit(0);
		}
		fgets(fname, fcb);
		fname[strlen(fname) - 1] = 0;
		strcpy(fname2, fname);
		for (i = argc - 2; i > 0; i--)
			argv[i + 1] = argv[i];
		argv[1] = fname2;
		argc++;
		fclose(fcb);
		printf("  C %s\n", argv[1]);
	}
	if (argc >= 2)
	{
		fcreat("CNAME.$$$", fcb);
		fputs(argv[1], fcb);
		fputs("\n\032", fcb);
		fflush(fcb);
		fclose(fcb);
		strcpy(fname, argv[1]);
	}

	strcat(fname, ".C");

	fd1 = fopen(fname, fcb1);
	if (fd1 == ERROR)
	{
		printf("C: Cannot open file %s.\n", fname);
		exit(0);
	}
	flag = bctr = 0;
	while (fgets(string, fcb1))
	{
		len = strlen(string);
		string[len - 1] = 0;
		if (string[0] == '%')
		{
			flag = 1;
			linecopy(barray[bctr++], &string[1], argv, argc, 0);
		}
		else
		    if (flag == 1)
			break;
		if (bctr == BCOUNT)
		{
			printf("C: Batch file too big.");
			exit(0);
		}
	}
	/*	SUBSTITUTED ARGUMENTS ARE NOW IN BARRAY */

writesub :
	fd2 = open("A:$$$.SUB", 2);
	if (fd2 == -1)
	{
		fd2 = creat("A:$$$.SUB");
	}
	else
	{
		do
		    {
			flag = read(fd2, buffer, 1);
			if (flag == -1)
			{
				printf("C: Unable to append to A:$$$.SUB.");
				exit(0);
			}
		}
		while (flag)
			;
	}
	/* Now $$.sub should be opened correctly */
	while (bctr--)
	{
		for (i = 0; i < 128; i++)
			buffer[i] = 0x1a;
		strcpy(&buffer[1], barray[bctr]);
		buffer[0] = strlen(barray[bctr]);
		i = write(fd2, buffer, 1);
		if (i != 1)
		{
			printf("%S: Unable to write A:$$$.SUB.", argv[0]);
			exit(0);
		}
	}
	close(fd2);
	bios(1);
}

linecopy(dest, source, argv, argc, bias)
char *dest, *source, **argv;
int argc, bias;
{
	int i;
	char c, tempstr[MAXLINE];
	if (*source == '*')
		*source = ';';
	while (c = *source++)
	{
		if (c == '$' && isdigit(*source))
		{
			i = *source - '0' + bias;
			if (i < argc)
			{
				strcpy(dest, argv[i]);
				dest += strlen(argv[i]);
			}
			source++;
		}
		else
		    if (c == '$' && *source == '*')
		{
			for (i = 1 + bias; i < argc; i++)
			{
				if (i < argc)
				{
					strcpy(dest, argv[i]);
					strcat(dest, " ");
					dest += strlen(argv[i]) + 1;
				}
			}
			source++;
		}
		else
		    if (c == '$' && *source == '#')
		{
			sprintf(tempstr, "%d", argc - bias);
			strcpy(dest, tempstr);
			dest += strlen(tempstr);
			source++;
		}
		else
		    if (c == '$' && *source == '$')
		{
			source++;
			*dest++ = c;
		}
		else
		    if (c == '^' && isalpha(*source))
		{
			*dest++ = toupper(*source) - 0x40;
			source++;
		}
		else
		    {
			*dest++ = c;
		}
	}
	*dest = 0;
}

