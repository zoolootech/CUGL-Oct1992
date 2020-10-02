/*XSRCHR.C    VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:38:02 PM */
/*
%CC1 $1.C -O -X -E7A00
%CLINK $1 SCOPE DIO WILDEXP -F CHARFUN STRFUN -S -E7A00
%DELETE $1.CRL 
*/
/* 
Description:

Search over each record of each file in wildcard filelist; 
if the seach pattern is found in a record, display it, 
with DIO output redirection.

If the search pattern is given as the first command line parameter,
the record delimiter is assumed to be '\n'.

If the -p option is selected or the search pattern is not given on the
command line, there is full screen input of:
	search patterns;
	record delimiter;
	output delimiters;
	cas_flag;

In the case of full screen input, there can be up to 10 search patterns, 
which may be or'd, and'd, or not'd.

If the cas_flag is set, the file name is printed at the start of
the first line of output of each record.

Strings can include meta characters, such as '*', '?', etc.  
See help_mess for a listing of them.


Usage:

xsrch   [pattern]   input   [>output_ufn (def = crt)]   [-options (-h = help)] 

The input parameter is required.  It can be either:
	input_afn1 [input_anf2, ..]   and/or   <input_ufn_list
		
If a pattern to be searched for is given on the command line (rather than
given through full-screen input), it must precede the input parameter.

It is best to select full screen input with the option '-p', rather than
trust to default selection.

See help_mess for more information on options.


From public domain code: 
	Van Nuys SEARCH, DIO, WILDEXP (by Eugene H. Mallory):
		WILDEXP modified to allow du: == u/d:.
		DIO modified to avoid cr-cr-lf triple with putchar.
		some functions rewritten from code in SEARCH:
			decode()
			find()
			search()
	CUG SCOPE:
		Full screen input based on the template file XSRCH.ZIP:
			select  and, not, or   strings;
			select parts of record for output;
        		select record delimiter;
			select flags.


Depending on test, 1-5 x slower than FIND.COM.

Effects on efficiency:
major	use of machine coded routines for character and string operations;
major	use of file read() with large block size;
major	inline C-code for touppper in search();
minor	reduced parameter passing;
minor	reduced setting of variables; 
minor	use of ! instead of  == 0;
minor	return from recursive calls of search with longjmp;
minor	expand code of search(), so branching gives fewer operations/branch;
minor	pointer arithmetic instead of arrays in search().


By J.A. Rupley, Tucson, Arizona
Coded for BDS C compiler, version 1.50a
*/

/* page eject */
#include "bdscio.h"
#include "dio.h"

/*configuration*/
#define READSECS 10
#define BUFLEN (((6 * READSECS) + 1) * 128)
#define DELIM "\n"
#define NO_SUMMARY FALSE
#define LIST_FLAG TRUE
#define RECORD_NUMBERS FALSE
#define INP_PATTERN FALSE
#define DISP_CTRL FALSE
#define SKIP 0
#define CAS_FLAG FALSE

/*search constants*/
#define FOUND 0x80
#define NOTFOUND 0x81
#define MATCH 0x82
#define NOTMATCH 0x83
#define NOTBEGIN 0xff
#define NOTEND 0x9d
#define SKIPANY 0x86
#define SKIPONE 0x87
#define IGNOREONE 0x1c
#define LINEEND 0x1d
#define LINESTART 0x7f
#define ANYALPHA 0x1e
#define ANYNUMBER 0x1f

/*buffers and arrays*/
#define NPAT 10
#define NDRIVER 7
#define NOUT 10

char string[BUFLEN];
char pat_buf[42];
char delim[42];
char pat_array[NPAT][42];
char out_start[NOUT][21];
char out_end[NOUT][21];
char fname[MAXLINE];
char jbuf[JBUFSIZE];

/*external constants, used for speed or control*/
int i, j, firstpat, nextpat;
int skip, no_summary, list_flag, record_numbers;
int disp_ctrl, inp_pattern, optionerr;
int cas_flag;
int npat, nout;
char *pattern;
int ioblks, iofile;

main(argc, argv)
char **argv;
int argc;
{
	int ii;
	int string_compare();

	cas_flag = CAS_FLAG;
	inp_pattern = INP_PATTERN;
	record_numbers = RECORD_NUMBERS;
	no_summary = NO_SUMMARY;
	list_flag = LIST_FLAG;
	optionerr = FALSE;
	disp_ctrl = DISP_CTRL;
	skip = SKIP;

	dioinit(&argc, argv);

	get_options(&argc, argv);

	if (optionerr || ((argc < 2) && !DIOIN))
		help_mess();

	/*set up patterns, of all sorts*/

	if (((argc < 3) && !DIOIN) || inp_pattern)
	{
		inp_pattern = TRUE;
		full_screen();
	}
	else
	{
		strcpy(delim, DELIM);

		pat_decode(argv[1], &pat_array[0][1]);
		pat_array[0][0] = 'O';
		npat = 1;
		argv = &argv[1];
		argc -= 1;
	}

	/* now wildcard and sort what is left on command line */

	wildexp(&argc, &argv);
	qsort(&argv[1], argc - 1, 2, &string_compare);

	/* search filelist from command line, if any*/

	for (ii = 1; ii < argc; ii++)
	{
		strcpy(fname, argv[ii]);
		find_all(fname);
	}

	/* search filelist from input file, if any*/

	if (DIOIN)
		while (!getstring(fname))
			find_all(fname);

	dioflush();
	exit();
}
/* END OF MAIN*/

/*page eject*/

int find_all(fname)
char *fname;
{
	int c, temp;
	int record;
	int next, isave;
	char csave, csavefirst;
	char ichar;
	int find_count;
	int jj, kk;
	int first_char, end_char;
	int first_temp, end_temp;

	/*fill after end of file name with spaces to column 20*/
	/*then open file, print filename, and process file*/
	
	for (jj = strlen(fname); jj; jj--)
		if ((c = fname[jj - 1]) != ' ' && c != '\r' && c != '\n')
			break;
	while (jj < 20)
		fname[jj++] = ' ';
	fname[jj] = '\0';

	if ((iofile = open(fname, 0)) == ERROR)
	{
		typef("\nFIND_ALL: cannot open file %s.\n", fname);
		close(iofile);
		return (0);
	}
	else
		if (!no_summary)
			printf("\f\n%s\n", fname);

	find_count = 0;
	record = 1;
	string[0] = LINESTART;
	string[1] = '\0';

	if (skip)
		if ((record += skip_lines(skip)) == ERROR)
		{
			typef(" %s.\n", fname);
			close(iofile);
			return (0);
		}

	firstpat = 0;

	/*main loop to search file for pattern, record by record*/
	/*first, with -pat_gets- find next record terminated by -delim- */

	while ((temp = pat_gets(delim)) != ERROR)
	{

		/*intercept console input*/

		if (bdos(11))
		{
			if ((c = bdos(1)) == 0x03)        /*ctrl-c*/
			{
				typef("\nFIND_ALL: Control C intercepted\n");
				dioexit();
			}
			else
			    if (c == 0x18)        /*ctrl-x*/
			{
				typef("\nFIND_ALL: Control X intercepted\n");
				close(iofile);
				return (1);
			}
		}

		/*store end characters of record */
		/*replace by special delimiters for search*/

		csavefirst = string[firstpat];
		string[firstpat] = LINEEND;
		isave = firstpat + 1;
		csave = string[isave];
		string[isave] = '\0';
		next = nextpat;

		/*with -all_pats- search record for all patterns*/
		/*output as appropriate*/

		if (all_pats() == FOUND)
		{

			if (!find_count++)
			{
				outs(delim, MAXLINE, disp_ctrl);
				printf("\n");
			}

			if (list_flag)
			{
				if (nout)
				{
					if (cas_flag)
						printf("%s", fname);

					if (record_numbers)
						printf("Record number %-7d ", record);

					first_char = 1;

					for (kk = 0; kk < nout; kk++)
					{
						firstpat = first_char;
						pattern = out_start[kk];

						if (find() != FOUND)
							continue;

						first_char = firstpat++;
						pattern = out_end[kk];

						if (find() != FOUND)
							continue;

						end_char = firstpat;

						outs(&string[first_char], (end_char - first_char), disp_ctrl);

						if (temp == FOUND && kk == (nout - 1))
							for (jj = 0; (ichar = out_end[kk][jj]); jj++)
								if (ichar == LINEEND && (out_end[kk][jj - 1] != '!'))
									outs(delim, MAXLINE, disp_ctrl);
					}
				}
				else
				{
					if (record_numbers)
						printf("%7d ", record);

					string[isave - 1] = '\0';

					outs(&string[1], BUFLEN, disp_ctrl);

					if (temp == FOUND)
						outs(delim, BUFLEN, disp_ctrl);
				}
			}

			/*if not list_flag, ie if select files, not lines*/

			else
			{
				if (!no_summary)
					summary_output(find_count);

				close(iofile);
				return (1);
			}
		}

		record++;

		/*normal exit at end of file*/

		if (temp == NOTFOUND)
		{
			outs(delim, MAXLINE, disp_ctrl);
			printf("\n");

			if (!no_summary)
			{
				if (find_count)
					printf("\n%s\n", fname);
				summary_output(find_count);
			}

			close(iofile);
			return (1);
		}

		/*reset string and pointers for search of next record*/

		firstpat = 0;
		string[isave - 1] = csavefirst;
		string[isave] = csave;
		strcpy(&string[1], &string[next]);
	}

	typef("\nFIND_ALL: line overflows buffer %s.\n", fname);
	close(iofile);
	return (0);
}
/*END OF FIND_ALL*/



/*page eject*/

void full_screen()
{
	int iobuf;
	int ii, jj;
	int offset;

	char pat_driver[NDRIVER][10];
	strcpy(pat_driver[0], "Aand patt");
	strcpy(pat_driver[1], "Nnot patt");
	strcpy(pat_driver[2], "Oor patte");
	strcpy(pat_driver[3], "Dsearch d");
	strcpy(pat_driver[4], "Routput d");
	strcpy(pat_driver[5], "Ccas_flag");
	strcpy(pat_driver[6], "\0       ");

	if ((iobuf = open("xsrch.zip", 0)) == ERROR)
		if ((iobuf = open("a15:xsrch.zip", 0)) == ERROR)
			error("FULL_SCREEN: cannot open XSRCH.ZIP template.");

	read(iobuf, string, 20);
	scope(string, 80, 1920, 0);
	string[1920] = '\0';

	npat = nout = 0;

	for (jj = 0; pat_driver[jj][0]; jj++)
	{
		firstpat = 0;
		pattern = &pat_driver[jj][1];

		if (find() != FOUND)
			error("FULL_SCREEN: no pat_driver pattern.");

		offset = firstpat;

		while (TRUE)
		{
			if (npat == NPAT || nout == NOUT)
				error("FULL_SCREEN: too many patterns");

			offset += 80;

			for (ii = 39; ii >= 0; ii--)
				if (string[offset + ii] != ' ')
					break;
			if (ii < 0)
				break;

			switch (pat_driver[jj][0])
			{
			case 'C' :
				cas_flag = string[offset] - '0';
				break;
			case 'R' :
				strncpy(pat_buf, &string[offset], ii + 1);
				if (strlen(pat_buf) > 20)
				{
					pat_decode(&pat_buf[20], out_end[nout]);
					for (ii = 19; ii >= 0; ii--)
						if (string[offset + ii] != ' ')
							break;
					strncpy(pat_buf, &string[offset], ii + 1);
					pat_decode(pat_buf, out_start[nout]);
				}
				else
				    {
					pat_decode(pat_buf, out_start[nout]);
					strcpy(out_end[nout], out_end[nout - 1]);
				}
				nout++;
				break;
			case 'D' :
				strncpy(pat_buf, &string[offset], ii + 1);
				pat_decode(pat_buf, delim);
				break;
			case 'A' :
			case 'N' :
			case 'O' :
				strncpy(pat_buf, &string[offset], ii + 1);
				pat_decode(pat_buf, &pat_array[npat][1]);
				pat_array[npat][0] = pat_driver[jj][0];
				npat++;
				break;
			default :
				error("FULL_SCREEN: bad pattern driver");
			}
		}
	}

	if (npat < 1)
		error("FULL_SCREEN: no search pattern");
	return;
}
/* END OF FULL_SCREEN*/



/*page eject*/

int all_pats()
{
	int itemp;
	int kk;
	int and_flag, or_flag, not_flag;

	and_flag = or_flag = not_flag = 0;

	for (kk = 0; kk < npat; kk++)
	{
		firstpat = 0;
		pattern = &pat_array[kk][1];

		if (find() == FOUND)
		{
			switch (pat_array[kk][0])
			{
			case 'N' :
				return NOTFOUND;
			case 'A' :
				and_flag++;
				break;
			case 'O' :
				return FOUND;
			default :
				error("ALL_PATS: bad pat_array[kk][0].");
			}
		}
		else
		    {
			switch (pat_array[kk][0])
			{
			case 'A' :
				return NOTFOUND;
			case 'N' :
				not_flag++;
			}
		}
	}

	if (and_flag || not_flag)
		return FOUND;
	else
	    return NOTFOUND;
}
/* END OF ALL_PATS*/


int pat_gets(pat)
char *pat;
{
	while (TRUE)
	{
		pattern = pat;

		if (find() == FOUND)
			return FOUND;

		if ((nextpat = strlen(string)) > BUFLEN - 2 * 128)
			return ERROR;

		if ((ioblks = read(iofile, &string[nextpat], READSECS)) <= 0)
			return NOTFOUND;

		string[nextpat + ioblks * 128] = '\0';
	}

	return ERROR;
}
/* END OF PAT_GETS */


int skip_lines(count)
int count;
{
	int itemp;
	int ii;

	for (ii = 0; ii < count; ii++)
	{
		firstpat = 0;

		if ((itemp = pat_gets(delim)) == NOTFOUND || itemp == ERROR)
		{
			typef("\nSKIP_LINES: buffer overflow or unexpected end of file ");
			return (ERROR);
		}

		strcpy(&string[1], &string[nextpat]);

		if (!((ii + 1) % 10))
			typef("%d ", ii + 1);
	}

	typef("\n");

	return (ii);
}
/* END OF SKIP_LINES*/


void summary_output(find_count)
int find_count;
{
	int kk;

	printf("%5d finds for search pattern  = \n", find_count);

	for (kk = 0; kk < npat; kk++)
	{
		printf("   %c ", pat_array[kk][0]);

		outs(&pat_array[kk][1], MAXLINE, TRUE);

		if (kk < (npat - 1))
			if (!((kk + 1) % 3))
				printf("\n");
		else
		    if (pat_array[kk][0] != pat_array[kk + 1][0])
			printf("\n");
	}

	printf("\n\n");

	return;
}
/* END OF SUMMARY_OUTPUT */


int outs(str, count, ctrl_display)
char *str;
int count, ctrl_display;
{
	int ii, linecount;
	char c;

	linecount = 0;

	for (ii = 0; ii < count; ii++)
	{
		switch (str[ii] & 0x7f)
		{
		case '\0' :
			return (linecount);
		case '\n' :
			linecount++;
			putchar(str[ii]);
			break;
		default :
			if (ctrl_display)
			{
				if (str[ii] > 0x80)
					putchar('~');
				if ((c = (str[ii] & 0x7f)) < 0x20 || c == 0x7f)
				{
					putchar('^');
					putchar(c + 0x40);
					if (c != '\r')
						break;
				}
			}
			putchar(str[ii]);
		}
	}

	return (ERROR);
}
/* END OF OUTS*/



/*page eject*/

void pat_decode(pat, str)
char *pat;
char *str;
{
	int c;

	j = i = 0;

	while (c = pat[i++])
	{
		if (c == '!')
		{
			c = pat[i++];

			switch (c)
			{
			case '*' :
			case '?' :
			case '!' :
			case '$' :
			case 0 :
				error("PAT_DECODE: bad pattern after !.");
			}

			decode(c, pat, str, 1);
		}
		else
		    decode(c, pat, str, 0);
	}

	str[j] = 0;

	return;
}
/* END OF PAT_DECODE */


int decode(c, pat, str, not_flag)
int c, not_flag;
char *pat, *str;
{
	switch (c)
	{
	case '$' :
		str[j++] = SKIPONE;
		break;
	case '*' :
		str[j++] = SKIPANY;
		if (pat[i] == '!' && pat[i + 1] == '}')
			error("DECODE: *!} not allowed.");
		break;
	case '?' :
		str[j++] = SKIPONE;
		break;
	case '}' :
		str[j++] = LINEEND;
		if (pat[i] != 0)
			error("DECODE: } must be last.");
		if (i == 2 && not_flag)
			error("DECODE: no pattern before !}.");
		break;
	case '{' :
		str[j++] = LINESTART;
		if (i != (not_flag + 1))
			error("DECODE: { must be first.");
		if (not_flag)
		{
			if (pat[i] == 0)
				error("DECODE: no pattern after !{.");
			if (pat[i] == '*')
				error("DECODE: !{* not allowed.");
			if (pat[i] == '!' && pat[i + 1] == '}')
				error("DECODE: !{!} not allowed.");
		}
		break;
	case '@' :
		str[j++] = ANYALPHA;
		break;
	case '#' :
		str[j++] = ANYNUMBER;
		break;
	case '_' :
		str[j++] = ' ';
		break;
	case '^' :
		c = pat[i++];
		if (c > 0x7f || c <= 0x40)
			error("DECODE: illegal character follows ^");
		str[j++] = toupper(c) - 0x40;
		break;
	case '\\' :
		c = pat[i++];
		if (c == 0)
			error("DECODE: \\ requires character following.");
		switch (c)
		{
		case 'n' :
		case 'N' :
			c = '\n';
			break;
		case 'f' :
		case 'F' :
			c = '\f';
			break;
		case 'r' :
		case 'R' :
			c = '\r';
			break;
		case 't' :
		case 'T' :
			c = '\t';
			break;
		case 'b' :
		case 'B' :
			c = '\b';
			break;
		}
		str[j++] = c;
		break;
	default :
		str[j++] = c;
	}
	if (not_flag)
		str[j - 1] |= 0x80;
	return;
}
/* END OF DECODE */


int find()
{
	i = 0;
	j = firstpat;

	switch (setjmp(jbuf))
	{
	case NOTFOUND :
		return NOTFOUND;
	case FOUND :
		nextpat = j;
		if (pattern[0] == NOTBEGIN)
			firstpat++;
		if (pattern[strlen(pattern) - 1] == NOTEND)
			nextpat--;
		return FOUND;
	}
loop :
	switch (search())
	{
	case NOTMATCH :
		j = ++firstpat;
		if (!string[j] || (string[j] == 0x1a))
			return NOTFOUND;
		i = 0;
		goto loop;
	case MATCH :
		if (!i++)
			firstpat = j;
		j++;
		if (!pattern[i])
			longjmp(jbuf, FOUND);
		if (!string[j] || (string[j] == 0x1a))
			return NOTFOUND;
		goto loop;
	case FOUND :
		longjmp(jbuf, FOUND);
	case NOTFOUND :
		return NOTFOUND;
	}
}
/* END OF FIND */

int search()
{
	int isave;

	if (pattern[i] & 0x80)
		switch (pattern[i])
		{
		case ANYALPHA | 0x80 :
			if (!isalpha(string[j]) && j && string[j] != LINEEND)
				return MATCH;
			else
			    return NOTMATCH;
		case ANYNUMBER | 0x80 :
			if (!isdigit(string[j]) && j && string[j] != LINEEND)
				return MATCH;
			else
			    return NOTMATCH;
		case SKIPANY :
			if (!pattern[++i])
			{
				j = strlen(string);
				longjmp(jbuf, FOUND);
			}
			isave = i;
loop :
			i = isave;
loop2 :
			switch (search())
			{
			case MATCH :
				if (!pattern[++i])
				{
					i--;
					return MATCH;
				}
				if (!string[++j] || (string[j] == 0x1a))
				{
					i--;
					j--;
					longjmp(jbuf, NOTFOUND);
				}
				goto loop2;
			case NOTMATCH :
				if (!string[++j] || (string[j] == 0x1a))
				{
					j--;
					return NOTMATCH;
				}
				goto loop;
			case FOUND :
				longjmp(jbuf, FOUND);
			case NOTFOUND :
				longjmp(jbuf, NOTFOUND);
			}
		case SKIPONE :
			if (!j++)
				return NOTMATCH;
			i++;
			if (!string[j] || (string[j] == 0x1a))
				longjmp(jbuf, NOTFOUND);
			switch (search())
			{
			case MATCH :
				return MATCH;
			case NOTMATCH :
				j--;
				return NOTMATCH;
			case FOUND :
				longjmp(jbuf, FOUND);
			case NOTFOUND :
				longjmp(jbuf, NOTFOUND);
			}
		default :
			if (!inp_pattern && ((string[j] & 0x7f) >= 'a')
			    && ((string[j] & 0x7f) <= 'z'))
				if ((pattern[i] & 0x7f) != ((string[j] & 0x7f) - 0x20)
				    && j && (string[j] & 0x7f) != LINEEND)
					return MATCH;
			else
			    return NOTMATCH;
			else
			    if ((pattern[i] & 0x7f) != (string[j] & 0x7f)
			    && j && (string[j] & 0x7f) != LINEEND)
				return MATCH;
			else
			    return NOTMATCH;
		}
	else
	    switch (pattern[i])
	{
	case 0 :
		longjmp(jbuf, FOUND);
	case ANYALPHA :
		if (isalpha(string[j]))
			return MATCH;
		else
		    return NOTMATCH;
	case ANYNUMBER :
		if (isdigit(string[j]))
			return MATCH;
		else
		    return NOTMATCH;
	default :
		if (!inp_pattern && ((string[j] & 0x7f) >= 'a')
		    && ((string[j] & 0x7f) <= 'z'))
			if (pattern[i] == ((string[j] & 0x7f) - 0x20))
				return MATCH;
		else
		    return NOTMATCH;
		else
		    if (pattern[i] == (string[j] & 0x7f))
			return MATCH;
		else
		    return NOTMATCH;
	}
}
/* END OF SEARCH */



/*page eject*/

int string_compare(s, t)
char **s, **t;
{
	char *s1, *t1;

	for (s1 = *s, t1 = *t; *s1; s1++, t1++)
		if (*t1 != *s1)
			return (*s1 - *t1);
	return 0;
}
/* END OF STRING_COMPARE */


int strncpy(str1, str2, n)
char *str1, *str2;
int n;
{
	while (n-- && *str2)
		*str1++ = *str2++;

	*str1 = 0;

	return (str1);
}
/* END OF STRNCPY*/


void get_options(argcpntr, argv)
char **argv;
int *argcpntr;
{
	char *ss;
	int jj, ii;

	for (ii = *argcpntr - 1; ii > 0; ii--)
		if (argv[ii][0] == '-')
		{
			for (ss = &argv[ii][1]; *ss != '\0';)
			{
				switch (toupper(*ss++))
				{
				case 'A' :
					cas_flag = !cas_flag;
					break;
				case 'L' :
					list_flag = !list_flag;
					break;
				case 'F' :
					no_summary = !no_summary;
					break;
				case 'N' :
					record_numbers = !record_numbers;
					break;
				case 'P' :
					inp_pattern = !inp_pattern;
					break;
				case 'C' :
					disp_ctrl = !disp_ctrl;
					break;
				case 'S' :
					skip = atoi(ss);
					break;
				case 'H' :
					optionerr = TRUE;
					break;
				default :
					typef("sv]: illegal option %c.\n"
					    , *--ss);
					ss++;
					optionerr = TRUE;
					break;
				}
				while (isdigit(*ss))
					ss++;
			}

			for (jj = ii; jj < (*argcpntr - 1); jj++)
				argv[jj] = argv[jj + 1];
			(*argcpntr)--;
		}
	return;
}
/* END OF GET_OPTIONS */


voiä help_mess()
{
	typef("XSHOW: A>xsrch  [pattern]  input_afn and/or <input_ufn_list \n");
	typef("      [-options (-h = help)]  [>output_ufn (def = crt)]\n");
	typef("   one input file argument required.\n\n");
	typef("legal options are:\n");
	typef("-f=1     toggle list of file name and results summary(def = ON).\n");
	typef("-n=0     toggle line numbering (def = OFF).\n");
	typef("-l=1     toggle listing of selected lines (def = ON).\n");
	typef("-p=0     toggle request for patterns (search, etc) (def = OFF).\n");
	typef("-c=0     toggle display of control chars (def = OFF).\n");
	typef("-a=0     toggle filename display before each record (def = OFF).\n");
	typef("-s#=0    skip -#- records before start search (def = 0).\n");
	typef("\nmeta characters are:.\n");
	typef("* - any character string.	? - any single character.\n");
	typef("_ - a space.			^ - a control character follows.\n");
	typef("{ - beginning of line.		} - end of line.\n");
	typef("@ - any alphabetic character.	# - any numeric character.\n");
	typef("! - not the following character.\n");
	typef("\\ - a literal character or C character constant follows.\n");
	typef("\nControl C = abort    Control X = next file");
	exit(0);
}
/* END OF HELP_MESS */


