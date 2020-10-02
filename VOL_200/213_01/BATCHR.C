/* BATCHR.C    VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:35:57 PM */
/*
%CC1 $1.C -O -X -E5680
%CLINK $1 DIO WILDEXP -S -E5680
%DELETE $1.CRL 
*/
/* 
Description:

Submit utility BATCH from Van Nuys Toolkit, by Eugene H. Mallory.

Minor modifications:
	converted to standard C tokens;
	extended menu to include all BAT, ZEX, AND SUB files in all 
		user areas of current drive and of drive A:, 
		if no valid file given on command line;
	error exit if no files found.

By J.A. Rupley, Tucson, Arizona
Coded for BDS C compiler, version 1.50a
*/
/*********************************************************************
*                               BATCH                                *
**********************************************************************
*                  COPYRIGHT 1983 EUGENE H. MALLORY                  *
*********************************************************************/
#include "BDSCIO.H"
#include "DIO.H"
#define BCOUNT 66
char string[MAXLINE];
char fname[MAXLINE];
char c;
int fd1, fd2;
char fcb1[BUFSIZ], fcb2[BUFSIZ];
char barray[BCOUNT][80];
char buffer[128];
int bctr, flag;
int string_compare();

main(argc, argv)
char **argv;
int argc;

{
	int i, j, menuflag;
	char *fvector[7];
	char **fvectorp;
	char *disk;
	int diskno, vctr, len;

	dioinit(&argc, argv);

	if (DIOIN)
	{
		bctr = 0;
		while (!getstring(string))
		{
			len = strlen(string);
			string[len - 1] = 0;
			linecopy(barray[bctr++], string, argv, argc, 0);
			if (bctr == BCOUNT)
				error("BATCH: Batch file too big.");
		}
		goto writesub;
	}


	if (argc >= 2 && argv[1][0] == '/')
	{
		bctr = 0;
		typef("*");
		while (gets(string))
		{
			if (string[0] == 0)
				goto writesub;
			linecopy(barray[bctr++], string, argv, argc, 1);
			if (bctr == BCOUNT)
				error("BATCH: Batch file too big.");
			typef("*");
		}
		goto writesub;
	}

	if (argc < 2)
	{
		disk = "ABCDEFGHIJKL";
		vctr = 7;
		diskno = BDOS(25);
		if (!diskno)
			vctr = 4;
		fvector[0] = "DUMMY";
		fvector[1] = "??:*.BAT";
		fvector[2] = "??:*.ZEX";
		fvector[3] = "??:*.SUB";
		fvector[1][0] = disk[diskno];
		fvector[2][0] = disk[diskno];
		fvector[3][0] = disk[diskno];
		fvector[4] = "A?:*.BAT";
		fvector[5] = "A?:*.ZEX";
		fvector[6] = "A?:*.SUB";
		fvectorp = fvector;
		wildexp(&vctr, &fvectorp);
		if (vctr < 2)
			error("\nNo BAT, ZEX, or SUB files found on drives A: or %c:\n", disk[diskno]);
		qsort(&fvectorp[1], vctr - 1, 2, &string_compare);
		fvectorp++;
filemenu :
		i = menu(vctr - 1, fvectorp);
		if (i == -1)
			goto exitbatch;
		strcpy(fname, fvectorp[i]);
		fd1 = fopen(fname, fcb1);
		if (fd1 == ERROR)
			error("BATCH: Unable to open %s", fname);
	}
	else
	    {
		strcpy(fname, argv[1]);
		strcat(fname, ".BAT");
		fd1 = fopen(fname, fcb1);
		if (fd1 == ERROR)
		{
			strcpy(fname, "a:");
			strcat(fname, argv[1]);
			strcat(fname, ".BAT");
			fd1 = fopen(fname, fcb1);
		}
		if (fd1 == ERROR)
		{
			dioflush();
			error("BATCH: Unable to find: %s. \N", fname);
		}
	}

	if (!fgets(string, fcb1))
	{
		dioflush();
		error("BATCH: Batch file %s empty.", fname);
	}

	bctr = 0;
	do
	    {
		len = strlen(string);
		string[len - 1] = 0;
		linecopy(barray[bctr++], string, argv, argc, 1);
		if (bctr == BCOUNT)
			error("BATCH: Batch file too big.");
	}
	while (fgets(string, fcb1))
		;

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
				error("BATCH: Unable to append to A:$$$.SUB.");
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
			error("BATCH: Unable to write A:$$$.SUB.");
	}
	close(fd2);

exitbatch :
	dioflush();
	bios(1);
}

menu(icounter, strings)
char **strings;
int icounter;
{
	int max, bias, i, j;
	char *prompt;
	prompt = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	bias = 0;
xxxx :
	typef("\n\0x1b*");
	if ((icounter - bias) > 26)
		max = 26;
	else
		max = icounter - bias;
	for (i = 0; i < (max + 1) / 2; i++)
	{
		typef("%c) %s", prompt[i], strings[i + bias]);
		if (i + bias + (max + 1) / 2 < icounter)
		{
			for (j = 0; j <= 37 - strlen(strings[i + bias]); j++)
				typef(" ");
			typef("%c) %s\n",
			prompt[i + (max + 1) / 2],
			strings[i + bias + (max + 1) / 2]);
		}
		else
			typef("\n");
	}
	typef("\n\n");
	if (icounter > bias + 26)
		typef("More selections are available.\n");
	typef("Type selection, ESC to exit, CR for more, - to backup :");
	c = bdos(1);
	typef("\n\014");
	if (c == '\033')
		return -1;
	if (c == '\r' && (icounter > bias + 26))
		bias += 26;
	else
		if (c == '\r' && (icounter <= bias + 26))
			bias = 0;
	if (c == '-')
		bias -= 26;
	if (bias < 0)
		bias = 0;
	if (!isalpha(c))
		goto xxxx;
	for (i = 0; toupper(c) != prompt[i]; i++)
		;
	if (i >= max)
		goto xxxx;
	if (i + bias >= icounter)
		goto xxxx;
	return i + bias;
}


int string_compare(s, t)
char **s, **t;
{
	char *s1, *t1;
	int i;
	s1 = *s;
	t1 = *t;
	for (i = 0; i < MAXLINE; i++)
	{
		if (t1[i] != s1[i])
			return s1[i] - t1[i];
		if (s1[i] == '\0')
			return 0;
	}
	return 0;
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

