/* XSORTR.C   VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:37:56 PM */
/*
%CC1 $1.C -O -X -E5980
%CLINK $1 DIO -F CHARFUN STRFUN -S -E5980
%DELETE $1.CRL 
*/
/* 
Description:

Sort the file given as the only command line parameter (except for 
DIO redirectors and -xxx options).

Output is to terminal or as redirected by DIO.

The file is read, record by record, to construct a tree, 
each node of which holds file sector and character-count 
information for the corresponding record.
Display of the tree gives the sorted file.

The sort key can be a sequence of digit fields.
The key length can be specified.
The key start can be defined by a string each record is searched for.

Records can be of variable length, defined by a delimiter string,
eg <\n> if each line is a record.
Records can be multiline.

The number of records is limited only by the free space 
below the stack (enough for ca 2000 records) 
and the amount of free disk space (which need be only enough 
to hold the sorted file).  

With a CMI hard disk, the install speed is about 1 second per record.

The impetus for writing this code came from the need to merge 
and sort lists of abstracts.  If the cas_flag is set, an unsorted file,  
comprising a set of lists and presumed to contain duplicate records, 
is sorted and output.  If a record is a duplicate, only one copy of 
the record is displayed, but the first identifying line of the duplicate 
is displayed at the head of this copy.

To carry out a transformation of set of chem. abstracts files into a 
single file of primary information (abs. #, title, etc, but no abstract or
index terms), then to sort the file:

	xsrchr  [<filelist]  file1  [file2 ....]  +cas/temp  -p 
			[program enters full screen input -
				type in search and io delimiter strings
				then ESC to continue]
	xsortr  cas/temp  +cas/sort

For a summary of the various options: see help_mess().

For usage:  see help_mess().

NOTE: the program expects a modified version of DIO, the failure to use
which may cause complications, probably easily remedied. 


The functions install() and treeprint() are from MERGE, from the
Van Nuys Toolkit, by Eugene H. Mallory.
The function decode() is an adaptation of code from SEARCH, from the
Van Nuys Toolkit.


By J.A. Rupley, Tucson, Arizona
Coded for BDS C compiler, version 1.50a
*/

        /*page eject*/

#include "bdscio.h"
#include "dio.h"

#define SIZE_STRUCT 	12
#define SMALL_BUF	20
#define NO_KEY		1
#define DUP		2
#define EMPTY		4

#define ISDIGIT(x)	(((x >= '0') && (x <= '9')) ? x : 0)
#define TOUPPER(x)	(((x >= 'a') && (x <= 'z')) ? x - ' ' : x)
struct tnode
{
	struct tnode *left;
	struct tnode *right;
	int c_pattern;
	int c_end;
	int s_start;
	char c_start;
	char type;
}
;

struct tnode *root, *space;

char buf1[SECSIZ * 2], buf2[SECSIZ * 2], *s1, *s2;
char delim[SMALL_BUF], pattern[SMALL_BUF];
char *p_pattern, *p_delim;
char select_key, reject_key;
char *ptr;

int fd;
int xi, xj, xtemp, c_limit;
int sec_start, char_start, char_pattern, sec_end, char_end, c_count;
int rctr, lctr, maxr, maxl, left_moves, right_moves, left_right, rec_num;
int flag1, flag2;
int optionerr;
int cas_flag, lines_out, verbose, out_lim, num_only, upper_case, reverse, column, length;
int good_count, good_delim, digit_count, digit_delim;
int no_key_count, dup_count, empty_count, no_key_flag, dup_flag, empty_flag;

main(argc, argv)
char **argv;
int argc;

{
	dioinit(&argc, argv);

	set_options(&argc, argv);

	root = NULL;
	maxl = maxr = rec_num = 0;
	left_moves = right_moves = left_right = 0;

	sec_start = char_start = sec_end = char_end = 0;
	flag1 = flag2 = 0;
	dup_count = no_key_count = empty_count = 0;

	good_delim = digit_delim = 0;
	for (xi = 0; delim[xi]; xi++)
	{
		if (delim[xi] > 0x1f)
			good_delim++;
		if (isdigit(delim[xi]))
			digit_delim++;
	}

	while (!flag2)
	{
		rec_num++;

		space = sbrk(SIZE_STRUCT);
		if (space == -1)
			error("XSORT: Out of space at record %d.", rec_num);

		if (!get_record() && (flag2 == 1))
			break;

		space->s_start = sec_start;
		space->c_start = char_start;
		space->c_pattern = char_pattern;
		space->c_end = (char_end - strlen(delim));
		space->type = NULL;
		space->left = NULL;
		space->right = NULL;

		/*If a record is empty, a duplicate, or lacks the sort key:*/
		/*	a flag is set, for controlling displays that show */
		/*		the progess of the sort;*/
		/*	a counter is updated, for display in the */
		/*		summary output;*/
		/*	space->type is set, for testing in later compares*/
		/*		and in selection/rejection for output.*/

		dup_flag = no_key_flag = empty_flag = 0;
		c_limit = space->c_end - space->c_pattern;

		if (!good_count)
		{
			empty_flag++;
			empty_count++;
			space->type |= 4;
		}
		else
		    if ((space->c_pattern < 0)
		    || (num_only && !digit_count)
		    || (c_limit <= column))
		{
			no_key_flag++;
			no_key_count++;
			space->type |= 1;
		}

		rctr = lctr = 0;

		fill_buffx(space, buf2, &s2);

		install(&root, &space, &root);

		if (lctr && rctr)
			left_right++;
		else
			if (lctr)
				left_moves++;
			else
				if (rctr)
					right_moves++;

		/*For duplicate records, comp_record() sets dup_flag */
		/*and node->type/space->type |= DUP.*/

		if (dup_flag)
			dup_count++;

		/*Screen output during record-by-record read of file.*/

		if (verbose || dup_flag || empty_flag || no_key_flag)
		{
			typef("rec_num= %-4d lctr= %-4d rctr= %-4d ",
			rec_num, lctr, rctr);
			if (!(space->type & (EMPTY + NO_KEY)))
			{
				c_limit = 16 < c_limit ? 16 : c_limit;
				for (xi = 0; xi < c_limit; xi++)
				{
					if (s2[xi] < 0x20)
						break;
					typef("%c", s2[xi]);
				}
			}
			if (no_key_flag)
				typef("\t%d no sort key(s)", no_key_count);
			if (dup_flag)
				typef("\t%d duplicate(s)", dup_count);
			if (empty_flag)
				typef("\t%d empty record(s)", empty_count);
			typef("\n");
		}

		if (kbhit())
			getchar();
	}

	typef("\n\nSummary:\n");

	typef("%d records read with:\n\t%d missing sort key(s)\t%d duplicate(s)\t%d empty record(s)\n",
	rec_num, no_key_count, dup_count, empty_count);
	typef("during sorting there were:\n\t%d moves left\t\t%d moves right\t%d left and right\n\n",
	left_moves, right_moves, left_right);

	typef("Tree installed - pausing...\n");
	sleep(60);

	rctr = lctr = 0;
	if (root != NULL)
		treeprint(root);

	dioflush();
}

        /*page eject*/


install(nodeptr, spaceptr, rootptr)
struct tnode **nodeptr, **spaceptr, **rootptr;
{
	if (*nodeptr == NULL)
	{
		if (rctr == 0 && lctr > 30)
		{
			(*spaceptr)->right = (*rootptr);
			*rootptr = *spaceptr;
		}
		else
		    if (lctr == 0 && rctr > 30)
		{
			(*spaceptr)->left = (*rootptr);
			*rootptr = *spaceptr;
		}
		else
		    {
			*nodeptr = *spaceptr;
		}
		return;
	}
	else
	    {

		if (comp_record(*nodeptr, *spaceptr) > 0)
		{
left :
			lctr++;
			install(&((*nodeptr)->left), spaceptr, rootptr);
			return;
		}
		else
		    {
right :
			rctr++;
			install(&((*nodeptr)->right), spaceptr, rootptr);
			return;
		}
	}
}


treeprint(nodeptr)
struct tnode *nodeptr;
{
	if (lctr > maxl)
		maxl = lctr;
	if (rctr > maxr)
		maxr = rctr;

	if (nodeptr->left != NULL)
	{
		lctr++;
		treeprint(nodeptr->left);
		lctr--;
	}

	put_record(nodeptr);

	if (nodeptr->right != NULL)
	{
		rctr++;
		treeprint(nodeptr->right);
		rctr--;
	}
}


        /*page eject*/

/*
Return (0, <0, >0), depending on values of the records held at spaceptr 
(the record just read from the disk file) and at nodeptr (a record 
previously installed in the tree).
The buffer (buf2) for the new record at spaceptr is filled once before
entering install; the buffer (buf1) for the record previously installed at 
nodeptr must be filled fresh for each compare.
Records that are empty are treated as infinitely small.  Records that 
contain no sort key are treated as smaller than all except empty records.
If the record at spaceptr duplicates the record at nodeptr, dup_flag and 
spaceptr->type (or nodeptr->type) are set.
Change in whether node->type or space->type is set, or whether in install
the test on the value returned from comp_record() is '>=' or '>',
determines the order of display of duplicates (or of the header line if the
cas_flag is set (see put_record()).
*/

int comp_record(nodeptr, spaceptr)
struct tnode *nodeptr, *spaceptr;
{

	xi = spaceptr->type & (EMPTY + NO_KEY);
	xj = nodeptr->type & (EMPTY + NO_KEY);

	if (xi || xj)
	{
		if (xi && (xi >= xj))
			xtemp = 1;
		else
		    xtemp = -1;
	}
	else
	    {
		fill_bufx(nodeptr, buf1, &s1);
		if (!(xtemp = string_compare(s1 + column, s2 + column)))
		{
			dup_flag++;
			nodeptr->type |= DUP;
		}
	}

	if (!reverse)
		return xtemp;
	else
	    return -xtemp;
}


/*
Output the record for which sector and character count is held at nodeptr.  
Reject_key (and select_key) are tested to decide on rejection (or selection)
for output.
Out_lim (and lines_out) control the number of characters (or lines) 
displayed.
Cas_flag if set causes display of the first line of a duplicate record 
at the head of the output of the main copy of the record.  This line 
should give identifying information, so, in the case of duplicates, 
the output shows the sources of both copies.
If one wants the one-line output for the duplicate to follow output 
of the main copy of the record, change the test on comp_record in install
between '>=' to '>' or the set of type between node->type and space->type
in comp_record().
*/

int put_record(nodeptr)
struct tnode *nodeptr;
{
	int cas_duplicate, line_count;

	cas_duplicate = 0;
	line_count = lines_out;

	if (nodeptr->type & reject_key)
	{
		if (cas_duplicate = ((nodeptr->type & DUP) && cas_flag))
			line_count = 1;
		else
			return 0;
	}

	if (select_key)
		if (!(nodeptr->type & select_key))
			return 0;

	/*If out_lim is set, adjust the character-count and sector.*/
	/*information to limit the number of characters output.*/
	/*If line_count is set, limit the number of lines displayed.*/
	/*If cas_flag is set, do not output a record delimiter at the*/
	/*end of the line; this allows the main copy of the record to follow*/
	/*immediately after the header line.*/

	xtemp = out_lim + nodeptr->c_start;
	if (out_lim && (xtemp < nodeptr->c_end))
	{
		xi = xtemp / SECSIZ;
		c_limit = xtemp % SECSIZ;
	}
	else
	    {
		xi = nodeptr->c_end / SECSIZ;
		c_limit = nodeptr->c_end % SECSIZ;
	}

	xj = nodeptr->c_start;

	seek(fd, nodeptr->s_start, 0);

	while (xi--)
	{
		read(fd, buf2, 1);
		while (xj < SECSIZ)
		{
			putchar(buf2[xj]);
			if (buf2[xj++] == '\n')
				if (!--line_count)
				{
					if (!cas_duplicate)
						puts(delim);
					return 1;
				}
		}
		xj = 0;
	}

	if (read(fd, buf2, 1) == -1)
		error("XSORT: read error.");

	while (xj < c_limit)
	{
		putchar(buf2[xj]);
		if (buf2[xj++] == '\n')
			if (!--line_count)
			{
				if (!cas_duplicate)
					puts(delim);
				return 1;
			}
	}

	puts(delim);
	return 1;
}


/*
Fill buf1 or buf2 with two sectors from the file, with the first sector 
containing the first character after the end of the key delimiter.
Thus at least one sector of key information can be tested in the sort.
The addresses s1 and s2 are pointers to the start of the key information.
The key characters are stripped (& 0x7f).
*/

int fill_bufx(nodeptr, bufx, sx)
char *bufx, **sx;
struct tnode *nodeptr;
{
	if (nodeptr->type & (EMPTY + NO_KEY))
		return 0;

	seek(fd, (nodeptr->s_start + nodeptr->c_pattern / SECSIZ), 0);
	if (read(fd, bufx, 2) == -1)
		error("XSORT: read error.");

	*sx = bufx + (nodeptr->c_pattern % SECSIZ);

	xtemp = nodeptr->c_end - nodeptr->c_pattern;
	ptr = *sx + ((length < xtemp) ? length : xtemp);

	*ptr = '\0';

	for (ptr = *sx; *ptr; ptr++)
		*ptr = *ptr & 0x7f;

	return 1;
}


/*
Sector by sector read of the file to be sorted, setting:
	end of record (found by test with delim);
	start of key information (found by test with pattern).
Information stored:
	sec_start = file sector containing start of record;
	char_start = character count to start of record from 0 = start
				of sector sec_start;
	char_pattern = character count for start of key information.
	char_end = character count for end of record, adjusted in main() 
				by subtracting strlen(delim);
*/

int get_record()
{
	p_delim = delim;
	p_pattern = pattern;
	sec_start = sec_end;
	c_count = char_start = char_end % SECSIZ;
	char_pattern = sec_end = char_end = -1;
	good_count = digit_count = 0;

	if (!*pattern)
		char_pattern = char_start;

	if (seek(fd, sec_start, 0) == -1)
		error("XSORT: seek error.");

	while (TRUE)
	{
		if ((flag1 = read(fd, buf2, 1)) == -1)
			error("XSORT: read error.");

		if (flag1 && !flag2)
			if (!sec_scan())
				continue;

		char_end = c_count;
		sec_end = sec_start + c_count / SECSIZ;

		if (!flag1 || flag2)
		{
			if (char_end != char_start)
			{
				char_end = char_end + strlen(delim);
				flag2 = 2;
			}
			else
				flag2 = 1;
			return 0;
		}
		else
		    return 1;
	}
}


/*
Search a sector for character sequences = delim and pattern.
Maintain: good_count = number of non-control characters, used in testing
for an empty record; digit_count = number of digits in the record, 
also used in testing for an empty record.
Input is stripped (& 0x7f).
*/

int sec_scan()
{
	for (xi = (c_count % SECSIZ); xi < SECSIZ; xi++)
	{
		if ((xtemp = buf2[xi] & 0x7f) == CPMEOF)
			return -(flag2 = 1);
		c_count++;

		xtemp = TOUPPER(xtemp);

		if (*p_pattern)
		{
			if (xtemp == *p_pattern)
			{
				if (!*++p_pattern)
					char_pattern = c_count;
			}
			else
				p_pattern = pattern;
		}
		else
		    if (ISDIGIT(xtemp))
			digit_count++;

		if (xtemp > 0x1f)
			good_count++;

		if (xtemp == *p_delim)
		{
			if (!*++p_delim)
			{
				if ((good_count -= good_delim) < 0)
					good_count = 0;
				if ((digit_count -= digit_delim) < 0)
					digit_count = 0;
				return 1;
			}
		}
		else
		    p_delim = delim;
	}

	return 0;
}


/*
Compare records by values of digit strings.  A digit string is terminated 
by the first non-digit character.  Alpha and white characters are passed over.
By recursive call of num_compare(), digit strings are tested in sequence
until the records differ or until meeting the allowed length of the compare.
*/

int num_compare(t1, t2)
char *t1, *t2;
{
	while ((!(xi = ISDIGIT(*t1)) || (xi == '0')) && *t1)
		t1++;
	while ((!(xj = ISDIGIT(*t2)) || (xj == '0')) && *t2)
		t2++;

	for (xtemp = 0; xi && xj; t1++, t2++)
	{
			if (!xtemp && (xi != xj))
				xtemp = xi - xj;
			xi = ISDIGIT(*t1);
			xj = ISDIGIT(*t2);
	}

	if (!*t1 || !*t2)
	{
		if (!*t1 && !*t2)
			return 0;
		else
			if (!*t1)
				return -1;
		else
			return 1;
	}

	if (xi)
		return 1;
	if (xj)
		return -1;

	if (xtemp)
		return xtemp;

	return num_compare(t1, t2);
}


/*
Compare character strings.
Call num_compare() if num_only is set.
*/

int string_compare(t1, t2)
char *t1, *t2;
{
	if (num_only)
		return num_compare(t1, t2);

	if (upper_case)
	{
		for( ;TOUPPER(*t1) == TOUPPER(*t2); t1++, t2++)
			if (!*t1)
				return 0;
		return TOUPPER(*t1) - TOUPPER(*t2);
	}
	else
	{
		for( ;*t1 == *t2; t1++, t2++)
			if (!*t1)
				return 0;
		return *t1 - *t2;
	}
}


        /*page eject*/

/*
Get_options(), etc, set control variables, open input file, display 
help message, etc.
*/

void set_options(argcpntr, argv)
char **argv;
int *argcpntr;
{
	/*default options:	for CAS files*/
	verbose = select_key = out_lim = 0;
	lines_out = column = upper_case = 0;
	cas_flag = DUP;
	num_only = 1;
	length = 20;
	reverse = 1;
	reject_key = EMPTY + DUP + NO_KEY;
	strcpy(delim, "\n\*\r\n");
	strcpy(pattern, "AN  CA");

	get_options(argcpntr, argv);

	if (optionerr)
		help_mess();

	if (*argcpntr < 2)
	{
		typef("XSORT: No file.\n");
		help_mess();
	}

	if ((fd = open(argv[1], READ)) == -1)
		error("XSORT: Can't open file %s.", argv[1]);

	if ((length == 0) || (length > SECSIZ))
		length = SECSIZ;
	if (column > 0)
		column--;
	if (length <= column)
		error("XSORT: Length <= column.");

	if (!*pattern)
		reject_key = reject_key & (EMPTY + DUP);
}


/*
Setuppat() and decode() convert command line character strings, with the
symbols '^' prefacing a control character, '_' = space, and '\' used 
as in 'C' (\n = newline, etc).
*/

void pat_decode(pat, str)
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
			error("DECODE: illegal character follows ^");
		str[xj++] = toupper(c) - 0x40;
		break;
	case '\\' :
		c = pat[xi++];
		if (c == 0)
			error("DECODE: \\ requires character following.");
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
	typef("USAGE:  xsort  fn1  [redir: >fnout etc]  [options: -p.. -d.. etc]\n");
	typef("\n");
	typef("XSORT: Legal options:\n");
	typef("    default setup = CAS files;  false = 0;  _, \\x, and ^x  OK in strings.\n");
	typef("    options = true/false toggles, unless value(n)/string(str) indicated.\n");
	typef("-Sn=1 \tSelect for:   CAS files (n=1);   standard sort (n=0).\n");
	typef("      \t\t=1  for CAS files;\t\t=0  for standard sort.\n");
	typef("      \t\tthe Sn option, if used, must be last.\n");
	typef("-U=0  \tUpper case compares.\t\t-R=1     Reverse sort.\n");
	typef("-An=0 \tOutput selected types.\t\t-Xn=7    Reject selected types.\n");
	typef("      \t\t1 => key missing\t2 => duplicates\t\t4 => empty\n");
	typef("-V=0  \tVerbose output to screen during sorting.\n");
	typef("-#=1  \tDigit sequences only used in string compares.\n");
	typef("-On=0 \tIf n>0, output only n characters; if n<0, output n lines.\n");
	typef("-Cn=0 \tColumn position to begin compare.\n");
	typef("-Ln=20\tLength of string to compare (default = SECSIZ).\n");
	typef("-Dstr \tDelimiter string for record:\n");
	typef("      \t\t=\\n\\*\\r\\n for CAS files;\t=\\n   for standard sort\n");
	typef("-Pstr \tPattern at end of which start compare:\n");
	typef("      \t\t=AN__CA   for CAS files;\t=\\0   for standard sort");
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
			case 'V' :
				verbose = !verbose;
				break;
			case 'A' :
				select_key = atoi(ss);
				if (select_key)
					reject_key = 0;
				break;
			case 'X' :
				reject_key = atoi(ss);
				if (reject_key)
					select_key = 0;
				break;
			case '#' :
				num_only = !num_only;
				break;
			case 'S' :
				if (*ss == '0')
				{
					reject_key = lines_out = cas_flag = 0;
					select_key = out_lim = num_only = 0;
					column = length = upper_case = reverse = 0;
					verbose = 1;
					strcpy(delim, "\n");
					*pattern = '\0';
					break;
				}
				if (*ss == '1')
				{
					verbose = select_key = out_lim = 0;
					lines_out = column = upper_case = 0;
					cas_flag = DUP;
					num_only = 1;
					length = 20;
					reverse = 1;
					reject_key = EMPTY + DUP + NO_KEY;
					strcpy(delim, "\n\*\r\n");
					strcpy(pattern, "AN  CA");
					break;
				}
			case 'O' :
				out_lim = lines_out = 0;
				if (*ss == '-')
					lines_out = atoi(++ss);
				else
				    out_lim = atoi(ss);
				break;
			case 'C' :
				column = atoi(ss);
				break;
			case 'L' :
				length = atoi(ss);
				break;
			case 'D' :
				pat_decode(ss, delim);
				break;
			case 'P' :
				pat_decode(ss, pattern);
				break;
			case 'H' :
				optionerr = TRUE;
				break;
			default :
				typef("XSORT: Illegal option %c.\n", *--ss);
				ss++;
				optionerr = TRUE;
				break;
			}

			for (jj = ii; jj < (*argcpntr - 1); jj++)
				argv[jj] = argv[jj + 1];
			(*argcpntr)--;
		}
}


