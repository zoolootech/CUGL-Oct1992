/* XMERGER.C   VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:37:42 PM */
/*
%CC1 $1.C -O -X -E6000
%CLINK $1 DIO -F CHARFUN STRFUN -S -E6000
%DELETE $1.CRL 
*/
/* 
Description:

Merge two sorted files, with output redirection.

The files must be sorted on the same key used in the merge.
The key can be a sequence of digit fields.
The key length can be specified.
The key start can be defined by a string each record is searched for.

Records can be of variable length, defined by a delimiter string,
eg "\n" if each line is a record.
Records can be multiline.

Various options:  see help_mess() for summary.
For usage:  see help_mess().


The logic is adapted from that of MERGE, from the Van Nuys Toolkit, 
by Eugene H. Mallory.
The function decode() is adapted from code in SEARCH, from the
Van Nuys Toolkit.


By J.A. Rupley, Tucson, Arizona
Coded for BDS C compiler, version 1.50a
*/

#include "BDSCIO.H"
#include "DIO.H"

#define BUFLEN 	60*128

FILE fcb;
char buf1[BUFLEN], buf2[BUFLEN], *s1, *s2;
char delim[MAXLINE], pattern[MAXLINE];
char digits1[MAXLINE], digits2[MAXLINE];
char filename[20];
int flag1, flag2;
int xi, xj;
int flag;
int upper_case, reverse, column, length, file_only, input_only, duplicate;
int merge, num_only, reject, title, unique;
int optionerr;

main(argc, argv)
char **argv;
int argc;

{
	dioinit(&argc, argv);

	flag = flag1 = flag2 = 0;

	/*for standard merge*/
	/*
		column = length = 0;
		merge = 1;
		upper_case = 0;
		reverse = 0;
		unique = 0;
		num_only = 0;
		reject = 0;
		title = 0;
		strcpy(delim, "\n");
		*pattern = '\0';
	*/

	/*for CAS files = default setup*/
	column = 0;
	length = 20;
	merge = 0;
	upper_case = 0;
	reverse = 1;
	unique = 1;
	num_only = 1;
	reject = 1;
	title = 3;
	strcpy(delim, "\n\*");
	strcpy(pattern, "AN  CA");

	get_options(&argc, argv);
	if (optionerr)
		help_mess();
	if (argc < 2)
	{
		typef("XMERGE: No file name given.\n");
		help_mess();
	}
	if (fopen(argv[1], fcb) == -1)
		error("MERGE: Unable to open file %s.", argv[1]);
	strcpy(filename, argv[1]);

	if (length == 0)
		length = BUFLEN;
	if (column > 0)
		column--;
	if (duplicate || file_only || input_only)
		merge = unique = 0;
	if (unique)
		merge = input_only = file_only = duplicate = 0;
	if (merge)
		input_only = file_only = duplicate = 0;
	if (!*pattern)
		reject = 0;

	getinput();
	getfile();
	while (TRUE)
	{
		flag = string_compare(s1, s2);
		if (flag == 0)
		{
			if (unique || duplicate)
			{
				putrec(3, buf1);
				*buf1 = *buf2 = '\0';
				getinput();
				getfile();
			}
			else
			    if (merge)
			{
				putrec(1, buf1);
				*buf1 = '\0';
				getinput();
			}
			else
			    if (input_only || file_only)
			{
				*buf1 = *buf2 = '\0';
				getinput();
				getfile();
			}
		}
		else
		    if (flag > 0)
		{
			if (unique || merge || file_only)
				putrec(2, buf2);
			*buf2 = '\0';
			getfile();
		}
		else
		    {
			if (unique || merge || input_only)
				putrec(1, buf1);
			*buf1 = '\0';
			getinput();
		}
	}
}


void putrec(input, buffer)
int input;
char *buffer;
{
	if (title && *buffer)
	{
		if (*buffer == '\f' || *buffer == '\n')
			while (*buffer == '\f' || *buffer == '\n')
				putchar(*buffer++);
		else
		    putchar('\n');
		if ((title > 1) && (input > 1))
		{
			puts(filename);
			putchar('\n');
		}
		if ((title % 2) && (input % 2))
		{
			puts(_infile);
			putchar('\n');
		}
	}
	while (*buffer)
		putchar(*buffer++);
}


int bufx_fill(input, sx, bufx, flagx)
char **sx, *bufx;
int input, *flagx;
{
	char *p_delim;
	int c2, c, i;

restart :
	*sx = bufx[0] = '\0';
	i = 0;
	p_delim = delim;

	while (i < BUFLEN)
	{
		if (*flagx)
			return 0;
		if (input == 2)
		{
			if ((c = getc(fcb)) == '\r')
			{
				if ((c2 = getc(fcb)) == '\n')
					c = c2;
				else
				    ungetc(c2, fcb);
			}
		}
		else
		    if (input == 1)
			c = getchar();
		else
		    error("XMERGE: wrong input identifier.");
		if ((c == EOF) || (c == CPMEOF))
		{
			*flagx = 1;
			bufx[i] = '\0';
			if (i)
				strcat(bufx, delim);
			set_sx(input, sx, bufx);
			if (!*sx && reject)
				goto restart;
			return 1;
		}
		bufx[i++] = c;
		if (c == *p_delim)
		{
			if (!*++p_delim)
			{
				bufx[i] = '\0';
				set_sx(input, sx, bufx);
				if (!*sx && reject)
					goto restart;
				return 1;
			}
		}
		else
		    p_delim = delim;
	}
	error("XMERGE: record overflows buffer.");
}


void set_sx(input, sx, bufx)
int input;
char **sx, *bufx;
{
	int temp;

	if (strlen(pattern) == 0)
		*sx = bufx;
	else
		if (temp = str1_in_str2(pattern, bufx))
			*sx = bufx + temp;
		else
			*sx = 0;
	return;
}


void getinput()
{
	if (!bufx_fill(1, &s1, buf1, &flag1))
		inputeof();
}


void getfile()
{
	if (!bufx_fill(2, &s2, buf2, &flag2))
		fileeof();
}


void inputeof()
{
	if (unique || merge || file_only)
	{
		do
		    {
			putrec(2, buf2);
		}
		while (bufx_fill(2, &s2, buf2, &flag2))
			;
	}
	dioflush();
	exit();
}


void fileeof()
{
	fclose(fcb);
	if (unique || merge || input_only)
	{
		do
		    {
			putrec(1, buf1);
		}
		while (bufx_fill(1, &s1, buf1, &flag1))
			;
	}
	dioflush();
	exit();
}


        /*page eject*/

int num_compare(t1, t2)
char *t1, *t2;
{
	int lenx, temp, index1, index2;

	lenx = index1 = index2 = 0;
	digits1[0] = digits2[0] = '\0';

	if (!*t1 && !*t2)
		return 0;
	else
	    if (!*t1)
		return -1;
	else
	    if (!*t2)
		return 1;

	while (*t1 && (!isdigit(*t1) || (*t1 == '0')))
		t1++;
	while (isdigit(*t1))
	{
		digits1[index1++] = *t1;
		t1++;
	}

	while (*t2 && (!isdigit(*t2) || (*t2 == '0')))
		t2++;
	while (isdigit(*t2))
	{
		digits2[index2++] = *t2;
		t2++;
	}
	digits1[index1] = digits2[index2] = '\0';

	lenx = ((t1 - s1) > (t2 - s2)) ? (t1 - s1) : (t2 - s2);
	if (temp = (index1 - index2))
		return temp;
	if (temp = strcmp(digits1, digits2))
		return temp;
	if (lenx > (length + column))
		return 0;
	return num_compare(t1, t2);
}


int string_compare(t1, t2)
char *t1, *t2;
{
	int i;
	if ((strlen(t1) <= column) && (strlen(t2) <= column))
		return 0;
	if (strlen(t1) <= column)
	{
		if (!reverse)
			return -1;
		else
		    return 1;
	}
	if (strlen(t2) <= column)
	{
		if (!reverse)
			return 1;
		else
		    return -1;
	}
	if (num_only)
	{
		t1 = t1 + column;
		t2 = t2 + column;
		if (!reverse)
			return num_compare(t1, t2);
		else
		    return -num_compare(t1, t2);
	}
	if (upper_case)
	{
		for (i = column; i < column + length; i++)
		{
			if (toupper(t1[i]) != toupper(t2[i]))
				if (!reverse)
					return toupper(t1[i]) - toupper(t2[i]);
			else
			    return toupper(t2[i]) - toupper(t1[i]);
			if (t1[i] == '\0')
				return 0;
		}
		return 0;
	}
	else
	    {
		for (i = column; i < column + length; i++)
		{
			if (t1[i] != t2[i])
			{
				if (!reverse)
					return t1[i] - t2[i];
				else
				    return t2[i] - t1[i];
			}
			if (t1[i] == '\0')
				return 0;
		}
		return 0;
	}
}


/*
the next two functions perform as follows:
if string 1 is found in string2, return the position of the character 
in str2 immediately following the str1 sequence;
else 0 is returned
*/

int str1_in_str2(str1, str2)
char *str1, *str2;
{
	int i, loc;

	for (i = 0; i < strlen(str2); i++)
		if (loc = str1_start_str2(str1, &str2[i]))
			return (loc + i);
	return 0;
}


int str1_start_str2(str1, str2)
char *str1, *str2;
{
	int loc;

	loc = 0;
	while (toupper(*str1++) == toupper(*str2++))
	{
		loc++;
		if (!*str1)
			return loc;
		if (!*str2)
			return 0;
	}
	return 0;
}


void setuppat(pat, str)
char *pat;
char *str;
{
	int c;

	xj = xi = 0;

	while (c = pat[xi++])
		decode(c, pat, str, 0);
	str[xj] = 0;
	return;
}


int decode(c, pat, str, not_flag)
int c, not_flag;
char *pat, *str;
{
	switch (c)
	{
	case '_' :
		str[xj++] = ' ';
		break;
	case '^' :
		c = pat[xi++];
		if (c > 0x7f || c <= 0x40)
			error("SEARCH: illegal character follows ^");
		str[xj++] = toupper(c) - 0x40;
		break;
	case '\\' :
		c = pat[xi++];
		if (c == 0)
			error("SEARCH: \\ requires character following.");
		switch (c)
		{
		case 'N' :
			c = '\n';
			break;
		case 'F' :
			c = '\f';
			break;
		case 'R' :
			c = '\r';
			break;
		case 'T' :
			c = '\t';
			break;
		case 'B' :
			c = '\b';
			break;
		}
		if (isdigit(c))
			c = c - '0';
		str[xj++] = c;
		break;
	default :
		str[xj++] = c;
	}
	return;
}


void help_mess()
{
	typef("USAGE:  xmerge  <fn1  fn2  [redir: >fn3 etc]  [options: -p.. -d.. etc]\n");
	typef("    fn1 and fn2 must be sorted on the key tested in the merge.\n");
	typef("XMERGE: Legal options:\n");
	typef("    default setup = CAS files;  false = 0;  _, \\x, and ^x  OK in strings.\n");
	typef("    options = true/false toggles, unless value(n)/string(str) indicated.\n");
	typef("-Sn=1    Select for:   CAS files (n=1);   standard merge (n=0).\n");
	typef("         =1  for CAS files;           =0  for standard merge.\n");
	typef("         the Sn option, if used, must be last.\n");
	typef("-U=0     Upper case compares.         -R=1     Reverse sort.\n");
	typef("-I=0     Output non-matching (fn1)    -F=0     Output non-matching (fn2).\n");
	typef("-A=1     Output unique (fn1 and fn2)  -B=0     Output duplicates (fn1 & fn2).\n");
	typef("-X=1     Reject record if it contains no Pstring.\n");
	typef("-#=1     Digit sequences only used in string compares.\n");
	typef("-Cn=0    Column position to begin compare.\n");
	typef("-Ln=10   Length of string to compare (0 => BUFLEN = ca 8K).\n");
	typef("-Tn=3    Type filenames at head of record:\n");
	typef("         =1|3  for fn1;    =2|3  for fn2;    =0  for no header written.\n");
	typef("-Dstr    Delimiter string for record:\n");
	typef("         =\\n\\*   for CAS files;       =\\n   for standard merge\n");
	typef("-Pstr    Pattern at end of which start compare:\n");
	typef("         =AN__CA   for CAS files;     =\\0   for standard merge");
	exit(0);
}


void get_options(argcpntr, argv)
char **argv;
int *argcpntr;
{
	int ii, jj;
	char *ss;

	optionerr = 0;

	for (ii = *argcpntr - 1; ii > 0; ii--)
		if (argv[ii][0] == '-')
		{
			ss = &argv[ii][1];

			switch (toupper(*ss++))
			{
			case 'U' :
				upper_case = !upper_case;
				break;
			case 'R' :
				reverse = !reverse;
				break;
			case 'I' :
				input_only = !input_only;
				break;
			case 'F' :
				file_only = !file_only;
				break;
			case 'A' :
				unique = !unique;
				break;
			case 'B' :
				duplicate = !duplicate;
				break;
			case 'X' :
				reject = !reject;
				break;
			case '#' :
				num_only = !num_only;
				break;
			case 'S' :
				if (*ss == '0')
				{
					column = length = 0;
					merge = 1;
					upper_case = 0;
					reverse = 0;
					unique = 0;
					num_only = 0;
					reject = 0;
					title = 0;
					strcpy(delim, "\n");
					*pattern = '\0';
					break;
				}
				if (*ss == '1')
				{
					column = 0;
					length = 20;
					merge = 0;
					upper_case = 0;
					reverse = 1;
					unique = 1;
					num_only = 1;
					reject = 1;
					title = 3;
					strcpy(delim, "\n\*");
					strcpy(pattern, "AN  CA");
					break;
				}
			case 'C' :
				column = atoi(ss);
				break;
			case 'L' :
				length = atoi(ss);
				break;
			case 'T' :
				title = atoi(ss);
				break;
			case 'D' :
				setuppat(ss, delim);
				break;
			case 'P' :
				setuppat(ss, pattern);
				break;
			case 'H' :
				optionerr = TRUE;
				break;
			default :
				typef("XMERGE: Illegal option %c.\n"
				    , *--ss);
				ss++;
				optionerr = TRUE;
				break;
			}

			for (jj = ii; jj < (*argcpntr - 1); jj++)
				argv[jj] = argv[jj + 1];
			(*argcpntr)--;
		}
}


