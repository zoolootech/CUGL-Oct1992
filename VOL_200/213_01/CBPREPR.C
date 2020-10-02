/* CBPREPR.C    VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:36:05 PM */
/*
%CC1 $1.C -O -X -E6000
%CLINK $1 DIO -S -E6000
%DELETE $1.CRL 
*/
/* 
Description:

Filter to prepare C code for formatting:
	add and delete white space to produce consistent text;
	introduce newlines to produce one statment per line;
	left justify all lines.

DIO redirection.

To indent according to control level, output from CBPREP
must be filtered by CB, the formatting program written 
by W.C. Colley and R. Hollinbeck and based on a unix utility.

CBPREP uses the routines getch() and comment() of CB.


By J.A. Rupley, Tucson, Arizona
Coded for BDS C compiler, version 1.50a
*/

#include "bdscio.h"
#include "dio.h"

/*debug*/
/*
#define DEBUG
*/

int j;
int cc;
int peek;
int sflag;
int expr_flag;
int lastchar;
int c;
int control_keys, else_key, do_key, case_key, default_key, flow_keys;
int typedef_key, struct_key, union_key, c_keys, keys_total;
int close_loc, paren_count, ctrl_count, last_count, last_key;

char string[200];
char *keyword[30];

main(argc, argv)
int argc;
char *argv[];
{
	int k;
	int count;

	dioinit(&argc, argv);

	/*exit if improper command line parameters*/
	if (!_dioinflg)
		use_mess();

	/*set up pointers to keyword strings*/
	init_key();

	close_loc = paren_count = ctrl_count = last_count = 0;
	last_key = keys_total + 1;
	expr_flag = 0;
	sflag = 0;
	peek = -1;
	j = 0;

	/*  Start of text manipulation.  */

	while (((c = getchr()) != EOF) && (c != CPMEOF))
	{
		/*at terminators reset control values*/
		if (c == '}' || c == '{' || c == ':')
			paren_count = ctrl_count = 0;

		switch (c)
		{
			/*no white space starting line*/
			/*only single spaces elsewhere*/
		case '\t' :
			c = ' ';
		case ' ' :
			while (((peek = getchr()) == ' ') || (peek == '\t'))
				peek = -1;
			if (j)
				string[j++] = c;
			break;
			/*at end of line, perform additional*/
			/*formatting checks on entire line*/
		case '\n' :
			if (check_puts())
				printf("\n");
			break;
			/*block delimiters on separate line*/
		case '}' :
		case '{' :
			if (j)
				if (check_puts())
					printf("\n");
			string[j++] = c;
			while (((peek = getchr()) == ' ') || (peek == '\t'))
				peek = -1;
			if (peek != '\n')
			{
				check_puts();
				printf("\n");
			}
			break;
			/*space after, no space before ; and ,*/
		case ';' :
		case ',' :
			if (j && (string[j - 1] == ' '))
				j--;
			string[j++] = c;
			if (((peek = getchr()) != ' ') && (peek != '\t'))
				string[j++] = ' ';
			break;
			/*remove some spaces adjacent to brackets/parentheses*/
			/* -[- -]  -(- -) */
			/*add space if identifier follows close parenthesis*/
			/* )+ident  ]+ident*/
		case '(' :
		case '[' :
			while (((peek = getchr()) == ' ') || (peek == '\t'))
				peek = -1;
			if ((j > 1) && (string[j - 1] == ' ') &&
			    isname_char(string[j - 2]))
				j--;
			string[j++] = c;
			break;
		case ')' :
		case ']' :
			if (j && (string[j - 1] == ' '))
				j--;
			string[j++] = c;
			if (isname_char(peek = getchr()))
				string[j++] = ' ';
			break;
			/*pass over text*/
			/*delete imbedded newlines*/
		case '"' :
		case '\'' :
			string[j++] = c;
			while ((cc = getchr()) != c)
			{
				string[j++] = cc;
				if (cc == '\n')
					j--;
				if (cc == '\\')
					string[j++] = getchr();
			}
			string[j++] = cc;
			break;
		case '\\' :
			string[j++] = c;
			string[j++] = getchr();
			break;
			/*pass over #defines*/
		case '#' :
			if (j)
				if (check_puts())
					printf("\n");
			string[j++] = c;
			while ((cc = getchr()) != '\n')
				string[j++] = cc;
			string[j++] = cc;
			puts();
			break;
			/*pass over comments*/
			/*add lead tab if comment follows text*/
			/*if "/" not comment start, fall through to default*/
			/*there is also comment check during operator output*/
		case '/' :
			if ((peek = getchr()) == '*')
			{
				comment();
				break;
			}
		default :
			if (isoperator(c))
				operators();
			else
			    string[j++] = c;
			break;
		}
	}
	dioflush();
	exit();
}

        /*page eject*/
/*
main routine to introduce and delete spaces from text with operators
*/

void operators()
{
	/*checks on special expression operators or operator pairs*/
	/*remove lead and trail spaces as appropriate*/
	peek = getchr();
	if ((c == peek) && ((c == '+') || (c == '-')))
	{
		string[j++] = c;
		string[j++] = peek;
		peek = -1;
		return;
	}
	while (string[j - 1] == ' ')
		j--;
	if ((c == '-') && (peek == '>'))
	{
		string[j++] = c;
		string[j++] = peek;
		peek = -1;
		while (((c = getchr()) == ' ') || (c == '\t'))
			;
		peek = c;
		return;
	}
	if (c == '.')
	{
		string[j++] = c;
		while (((c = getchr()) == ' ') || (c == '\t'))
			;
		peek = c;
		return;
	}
	while (((cc = getchr()) == ' ') || (cc == '\t'))
		;
	peek = cc;

	/*test whether last character sequence is expression*/
	/*for start of line (j=0), check_puts() tested end of prev line*/
	if (j)
		expr_flag = isexpression(string[j - 1]);
	/*add lead space (trail space is as appropriate added below)*/
	if (j && (string[j - 1] != '(') && (string[j - 1] != '['))
		string[j++] = ' ';

	/*expr_flag TRUE is followed by a binary or assign operator*/
	if (expr_flag)
	{
		while (isbinop(c))
		{
			peek = getchr();
			if ((c == '/') && (peek == '*'))
			{
				comment();        /*watch for comments*/
				return;
			}
			string[j++] = c;
			while (((c = getchr()) == ' ') || (c == '\t'))
				;
			expr_flag = 0;
		}
		string[j++] = ' ';
	}

	/*handling of unary operator sequences*/
	/*for when expr_flag = 0 at entry to operators()*/
	/*or on fall through from above block executed when expr_flag = 1*/
	while (isoperator(c))
	{
		string[j++] = c;
		while (((c = getchr()) == ' ') || (c == '\t'))
			;
	}
	peek = c;
	return;
}

        /*page eject*/
/*
return true if last character sequence is an expression;
enter routine with string index j set at last character location + 1
ie at the end of the string;
*/

/*last_count = string position of last keyword found*/
/*last_key = identifier of last keyword*/
/*paren_count = count of control exprsn parenthesis imbalance*/
/*ctrl_count = string position after last parenthesis check*/
/*close_loc = string position of close of parenthesis*/
/*above externals reset as needed below or in check_puts()*/

int isexpression(testchar)
char testchar;
{
	int k, l, count;
	int pcount;
	int temp;
	char word[80];

	k = last_count;
	last_key = keys_total + 1;
	string[j] = '\0';

	/*can an open exprsn be closed*/
	/*if so, set close_loc flag and exit if terminal ')'*/
	if (paren_count)
	{
		count = end_paren(&string[ctrl_count], &paren_count);
		if (!paren_count)
		{
			close_loc = ctrl_count += count;
			if (!string[close_loc])
				return FALSE;
		}
		else
		    ctrl_count = strlen(string);
	}

	/*check if previous word == expression*/
	switch (testchar)
	{
	case '\'' :
	case '"' :
	case '_' :
	case '+' :
	case '-' :
	case ']' :
	case '.' :        /*for float constants*/
		return TRUE;
	default :
		if (isdigit(testchar))
			return TRUE;
		if (!isalpha(testchar) && (testchar != ')'))
			return FALSE;

		/*keywords are not expressions*/
		/*and may change syntax for following operator*/

		/*locate last keyword in line*/
		/*if a control keyword is met*/
		/*check parenthesis balance for following expression*/
		while (k < j)
		{
			count = next_alpha_word(&string[k], word);
			k += count;
			if (strlen(word) < 2 || strlen(word) > 8)
				continue;
			for (l = 0; l <= keys_total; l++)
				if (!strcmp(word, keyword[l]))
				{
					last_count = k;
					last_key = l;
					if (l <= control_keys)
					{
						paren_count = 0;
						ctrl_count = last_count;
						count = end_paren(&string[ctrl_count], &paren_count);
						if (!paren_count)
							ctrl_count += count;
						else
							ctrl_count = strlen(string);
					}
					break;
				}
		}

		/*set last_count and*/
		/*return if no keyword*/
		if (last_key > keys_total)
		{
			last_count = strlen(string);
			return TRUE;
		}
		else
		    while (string[last_count] == ' ')
			last_count++;

		/*return if keyword or (keyword) at end of line*/
		if (!string[last_count] || string[last_count] == ')')
			return FALSE;

		/*assume the following declarations are the last on a line*/
		if (last_key == union_key || last_key == struct_key ||
		    last_key == typedef_key)
			return FALSE;

		if (last_key > control_keys)
			return TRUE;        /*not control keyword*/

		/*check exprsn following control keyword*/
		if (!paren_count)
		{
			if (!string[ctrl_count])
				return FALSE;        /*control exprsn ends line*/
			else
				return TRUE;
		}
		else
		    return TRUE;        /*unclosed control expression*/
	}
}

        /*page eject*/
/*
process line buffer, including a call to line_check(),
then print it
*/

int check_puts()
{
	/*terminate string*/
	if (!j)
		return 1;
	else
	    string[j] = '\0';

	/*remove trailing spaces*/
	while (string[j - 1] == ' ')
		string[j--] = '\0';

	/*set expression flag for next line and try to close open expressions*/
	expr_flag = isexpression(string[j - 1]);

	/*insert newline if open expression was closed*/
	if (close_loc && close_loc < (j - 1))
	{
		while (string[close_loc] == ' ')
			close_loc++;
		insert_char(string, close_loc, '\n');
	}

	/*reset control values*/
	last_count = 0;
	last_key = keys_total + 1;
	ctrl_count = 0;
	close_loc = 0;

	/*process string*/
	if (!line_check(string))
		return 0;
	else
	    j = strlen(string);

	/*remove trailing spaces again*/
	while (string[j - 1] == ' ')
		string[j--] = '\0';

	/*print buffer and continue*/
	puts();
	return 1;
}

        /*page eject*/
/*
main routine for processing line of text
to make changes not done on the character scan
*/

int line_check(teststr)
char *teststr;
{
	int k, l;
	int count;
	int tempchar;
	int pcount;

	/*space after if, while, for, switch, etc, */
	for (k = 0; k <= keys_total; k++)
	{
		if (count = str1_start_str2(keyword[k], teststr))
		{
			tempchar = teststr[count];
			/*reject if keyword is first part of longer name*/
			/*reject if space present or end of exprsn or line*/
			if ((tempchar != ' ') && (tempchar != ';') &&
			    (tempchar != ')') && (tempchar != '\n') &&
			    !isname_char(tempchar))
				count = insert_char(teststr, count, ' ');
			break;
		}
		count = 0;
	}

	/*newline after (control expression)*/
	/*if newline needed, then process remainder of line*/
	/*so have only one statement or control expression per line*/
	if (count && (k <= control_keys))
	{
		/*go to end of control expression */
		pcount = 0;
		if ((count = end_paren(teststr, &pcount)) > 0)
			return break_line(teststr, count);
		count = 0;
	}

	/*newline after else and do */
	if (count && ((k == do_key) || (k == else_key)))
	{
		/*reject if keyword is first part of longer name*/
		if (!isname_char(teststr[count]))
			return break_line(teststr, count);
		count = 0;
	}

	/*newline after : except if operator (-?-:-) */
	/*allow (case '-' :) and (anyword :) */
	if (count && ((k == case_key) || (k == default_key)))
	{
		if (count = str1_in_str2(":", teststr))
			return break_line(teststr, count);
		count = 0;
	}
	/*this sequence destroys the value of count = keyword ident*/
	if (count = str1_in_str2(" ", teststr))
	{
		count = good_char(teststr, count);
		if (teststr[count++] == ':')
			return break_line(teststr, count);
	}

	/*only one statement per line*/
	count = 0;
	while (teststr[count] != '\0')
	{
		count = good_char(teststr, count);
		if (((tempchar = teststr[count++]) == ';') ||
		    (tempchar == '}') || (tempchar == '{'))
			return break_line(teststr, count);
	}
	return 1;
}

        /*page eject*/
void use_mess()
{
	printf(
	"\nUSAGE:\n");
	printf(
	"     cbprep  <input_fid  [>out_fid or |DIO_pipe or default = crt]\n\n");
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

/*
"c" reserved words---
the first four require a control expression following;
some of the others may have an exprsn or statemnt etc following;
*/

void init_key()
{
	int k;

	k = 0;
	keyword[k++] = "if";
	keyword[k++] = "while";
	keyword[k++] = "for";
	keyword[k++] = "switch";
	control_keys = k - 1;        /*end of keywrds requiring exprsn*/
	keyword[k++] = "do";
	do_key = k - 1;
	keyword[k++] = "else";
	else_key = k - 1;        /*end of keywrds with following line feed */
	keyword[k++] = "return";
	keyword[k++] = "case";
	case_key = k - 1;
	keyword[k++] = "default";
	default_key = k - 1;
	keyword[k++] = "break";
	keyword[k++] = "continue";
	keyword[k++] = "goto";
	flow_keys = k - 1;        /*end of keywords for flow control*/
	keyword[k++] = "auto";
	keyword[k++] = "static";
	keyword[k++] = "extern";
	keyword[k++] = "register";
	keyword[k++] = "typedef";
	typedef_key = k - 1;        /*typedef keyword*/
	keyword[k++] = "char";
	keyword[k++] = "short";
	keyword[k++] = "int";
	keyword[k++] = "long";
	keyword[k++] = "unsigned";
	keyword[k++] = "float";
	keyword[k++] = "double";
	keyword[k++] = "struct";
	struct_key = k - 1;        /*struct keyword*/
	keyword[k++] = "union";
	union_key = k - 1;        /*union keyword*/
	keyword[k++] = "sizeof";
	c_keys = k - 1;        /*end of 'c' keywords*/
	keyword[k++] = "FILE";        /*additional keywords*/
	keys_total = k - 1;
}

int getchr()
{
	int tempchar;

	tempchar = lastchar;
	lastchar = (peek < 0) ? getchar() : peek;
	if ((lastchar == EOF) || (lastchar == CPMEOF))
	{
		if (tempchar != '\n')
		{
			puts();
			printf("\n");
		}
		dioflush();
		exit();
	}
	else
		lastchar = (lastchar & 0x7f);
	peek = -1;
	return (lastchar == '\r' ? getchr() : lastchar);
}

void puts()
{
	if (!j)
		return;
	string[j] = '\0';
	printf("%s", string);
	j = 0;
	return;
}

void comment()
{

	if (j)
	{
		check_puts();
		printf("        ");
	}
	string[j++] = c;
	string[j++] = peek;
	peek = -1;
	sflag = 1;
rep :
	while ((c = string[j++] = getchr()) != '*')
		if (c == '\n')
			puts();
	if ((j > 1) && (string[j - 2] == '/'))
	{
		sflag++;
		goto rep;
	}
	while ((c = string[j++] = getchr()) == '*')
		;
	if ((c == '/') && !(--sflag))
	{
		puts();
		return;
	}
	else
	    if (c == '\n')
		puts();
	goto rep;
}

int isoperator(testchar)
int testchar;
{
	switch (testchar)
	{
	case '!' :
	case '%' :
	case '&' :
	case '*' :
	case '+' :
	case '-' :
	case '/' :
	case ':' :
	case '<' :
	case '=' :
	case '>' :
	case '?' :
	case '^' :
	case '|' :
	case '~' :
	case '.' :
		return TRUE;
	default :
		return FALSE;
	}
}

int isbinop(testchar)
char testchar;
{
	switch (testchar)
	{
	case '%' :
	case '/' :
	case ':' :
	case '<' :
	case '=' :
	case '>' :
	case '?' :
	case '^' :
	case '|' :
		return TRUE;
	case '&' :
		if ((peek == '&') || (string[j - 1] == '&'))
			return TRUE;
	case '*' :
	case '-' :
	case '+' :
		if (expr_flag)
			return TRUE;
	case '!' :
		if (peek == '=')
			return TRUE;
	default :
		return FALSE;
	}
}

int isname_char(testchar)
char testchar;
{
	if (isalpha(testchar) || isdigit(testchar) || (testchar == '_'))
		return TRUE;
	else
	    return FALSE;
}

/*
find next all alpha word contained in string;
skip over text within " or ';
copy word to buffer;
return position in string of character following end of word;
*/

int next_alpha_word(str, buf)
char *str, *buf;
{
	int count, word;

	count = 0;
restart :
	word = 0;

	/*find next alpha character or end of string*/
	while (!isalpha(str[count = good_char(str, count)]) && str[count])
		count++;

	/*copy characters into buffer until meet non-alpha character*/
	/*if non-alpha is part of a 'c' identifier*/
	/*go to end of identifier and then continue search*/
	while (isname_char(str[count]))
	{
		if (!isalpha(str[count]))
		{
			while (isname_char(str[count]))
				count++;
			goto restart;
		}
		else
			buf[word++] = str[count++];
	}

	/*exit if end of string*/
	buf[word] = '\0';
	return count;
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
	while (*str1++ == *str2++)
	{
		loc++;
		if (!*str1)
			return loc;
		if (!*str2)
			return 0;
	}
	return 0;
}

/*
test if string pointer is at good character
= non_text and non_space character;
if it is not, then advance pointer to next good character
*/

int good_char(str, loc)
char *str;
int loc;
{
	int passchar, tempchar;

	while (1)
	{
		if (!(tempchar = str[loc++]))
			return --loc;

		if ((tempchar == '"') || (tempchar == '\''))
		{
			while ((passchar = str[loc++]) != tempchar)
			{
				if (passchar == '\\')
					passchar = str[loc++];
			}
			continue;
		}
		if (tempchar == '\\')
			tempchar = str[loc++];

		if (tempchar != ' ')
			return --loc;
	}
}

int break_line(teststr, count)
char *teststr;
int count;
{
	count = good_char(teststr, count);
	if (teststr[count] != '\0')
	{
		if (teststr[count] != '\n')
			count = insert_char(teststr, count, '\n');
		/*recursive call to check out rest of line*/
		return line_check(&teststr[count]);
	}
	else
	    return 1;
}

int insert_char(str, loc, newchar)
char *str;
int loc, newchar;
{
	char tempstr[200];
	int k;

	strcpy(tempstr, str);
	k = loc;
	if (newchar == '\n')
		while (tempstr[loc - 1] == ' ')
			loc--;
	tempstr[loc] = newchar;
	strcpy(&tempstr[loc + 1], &str[k]);
	strcpy(str, tempstr);
	return ++loc;
}

/*
check for balance of parentheses within string:
return 0 if no parentheses present and enter with *p_pflag = 0;
return -1 if imbalance;
else return pointer to next character after close of parentheses;
enter with pointer p_pflag to location with current value of paren count,
which is revised according to the search;
*/

int end_paren(str, p_pflag)
char *str;
int *p_pflag;
{
	int loc, pflag;

	pflag = *p_pflag;
	loc = 0;
	if (pflag == 0)
		if (loc = str1_in_str2("(", str))
			pflag++;
	while (pflag)
	{
		loc = good_char(str, loc);
		if (str[loc] == '(')
			pflag++;
		if (str[loc] == ')')
			pflag--;
		if (str[loc] == '\0')
		{
			loc = -1;
			break;
		}
		loc++;
	}
	*p_pflag = pflag;
	return loc;
}


