/*
	CPP V5 -- Token parsing routines.

	source:  tok.c
	started: October 7, 1985
	version: May 26, 1988; July 21, 1988

	Written by Edward K. Ream.
	This software is in the public domain.

	See the read.me file for disclaimer and other information.
*/

#include "cpp.h"

/* Declare internal variables and routines. */
static int	 is_base_digit	(int);
static void	 scan_number	(int);
static en_tokens t_int		(void);

/*
	Return >= 0 if ch is a digit in the indicated base.
	Otherwise, return -1.
*/
static int
is_base_digit(base)
int base;
{
	TRACECH("is_base_digit");

	if (ch >= '0' && ch <= '9') {
		RETURN_INT("is_base_digit", ch - '0');
	}
	else if (base != 16) {
		RETURN_INT("is_base_digit", -1);
	}
	else if (ch >= 'a' && ch <= 'f') {
		RETURN_INT("is_base_digit", ch - 'a' + 10);
	}
	else if (ch >= 'A' && ch <= 'F') {
 		RETURN_INT("is_base_digit", ch - 'A' + 10);
	}
	else {
		RETURN_INT("is_base_digit", -1);
	}
}

/*
	Get value of a string of digits into t_value.
	Continue until a non base digit is found.
*/
static void
scan_number(base)
int base;
{
	int result;

	TRACECH("scan_number");

	t_value = 0;
	while (	(result = is_base_digit(base)) != -1) {
		t_value = ((long)base * t_value) + (long) result;
		sysnext();
	}

	LEAVE("scan_number");
}

/*
	Output a comment. Allow nested comments if nest_flag is TRUE.
	Surprisingly, this routine needs to be as fast as possible.
*/
void
copy_comment()
{
	register int clevel;
	int start_line;
	char line [LONG_DIGITS];

	TRACECH("copy_comment");

	/* Save starting line number for run-on comments. */
	start_line = t_line;
	clevel = 1;

	for (;;) {
		switch (ch) {

		case END_FILE:
			conv2s(start_line, line);
			err3("File ends in a comment starting at line ",
				line, ".");
			RETURN_VOID("copy_comment");

		case '\n':
			/* Keep track of line numbering. */
			bump_line();
			sysnlput();
			sysnext();
			continue;

		case '/':

			syscput(ch);
			sysnext();
			if (ch == '*') {
				syscput(ch);
				sysnext();
				if (nest_flag) {
					clevel++;
				}
			}
			continue;

		case '*':
			syscput(ch);
			sysnext();
			if (ch == '/') {
				syscput(ch);
				sysnext();
				if (--clevel == 0) {
					RETURN_VOID("copy_comment");
				}
			}
			continue;


		default:
			syscput(ch);
			sysnext();
		}
	}
}

void
skip_comment()
{
	register int clevel;
	int start_line;
	char line [LONG_DIGITS];

	TRACECH("skip_comment");

	/* Save starting line number for run-on comments. */
	start_line = t_line;
	clevel = 1;

	for (;;) {
		switch (ch) {

		case END_FILE:
			conv2s(start_line, line);
			err3("File ends in a comment starting at line ",
				line, ".");
			RETURN_VOID("skip_comment");

		case EORT:
			conv2s(start_line, line);
			err3("Macro arg ends in a comment starting at line ",
				line, ".");
			RETURN_VOID("skip_comment");
			

		case '\n':
			/* Keep track of line numbering. */
			bump_line();
			sysnext();
			continue;

		case '/':
			sysnext();
			if (ch == '*') {
				sysnext();
				if (nest_flag) {
					clevel++;
				}
			}
			continue;

		case '*':
			sysnext();
			if (ch == '/') {
				sysnext();
				if (--clevel == 0) {
					RETURN_VOID("skip_comment");
				}
			}
			continue;

		default:
			sysnext();
		}
	}
}

/*
	Copy an identifier into symbol[] and its length in the global t_length.
	Surprisingly, this routine should be as fast as possible.
*/
void
t_id(symbol, max_length)
char *	symbol;
int	max_length;
{
	int length = 0;

	TRACEPB("t_id", printf("(%p, %d)\n", symbol, max_length));

	max_length--;
	while (isid2(ch) && length < max_length) {
		*symbol++ = ch;
		length++;
		sysnext();
	}
	*symbol = '\0';
	t_length = length;

	if (length >= max_length) {
		error("Identifier too long.");
	}

	LEAVE("t_id");
}

/*
	Parse an integer constant (octal, decimal or hexadecimal) or float.
	Put the value in t_value if it is an integer.

	dot_flag is TRUE if a decimal point has been seen.

	Return the token type (INT_TOK, LONG_TOK, FLOAT_TOK).

	Legal integer forms:	ddd,	0ddd,	0xddd
	Legal float forms:	xxx.yyyE+zzz

	+-zzz is optional
	one of xxx and yyy may be omitted
	one of . and E may be omitted
*/
en_tokens
t_number(dot_flag)
bool dot_flag;
{
	en_tokens result;
	bool need_exp = FALSE;

	TRACECH("t_number");

	/* Defaults. */
	t_value = 0;

	if (dot_flag) {
		goto frac_part;
	}

	/* Integer part. */

	result = t_int();
	if (ch == '.') {
		sysnext();
		goto frac_part;
	}
	else if (ch == 'e' || ch == 'E') {
		goto exp_part;
	}
	else {
		RETURN_INT("t_number", result);
	}

	/* Fraction part. */
frac_part:	
	t_int();

exp_part:
	if (ch == 'e' || ch == 'E') {
		need_exp = TRUE;
		sysnext();
	}
	if (ch == '+' || ch == '-') {
		need_exp = TRUE;
		sysnext();
	}
	if (ch >= '0' && ch <= '9') {
		t_int();
	}
	else if (need_exp) {
		error("Ill formed floating constant.");
	}

	if (ch == 'l' || ch == 'L' || ch == 'f' || ch == 'F') {
		sysnext();
	}

	RETURN_INT("t_number", FLOAT_TOK);
}

static en_tokens
t_int()
{
	TRACECH("t_int");

	/* Leading 0 or 0x changes base. */	
	if (ch == '0') {
		sysnext();
		if (ch == 'x' || ch == 'X') {
			sysnext();
			scan_number(16);
		}
		else if (isdigit(ch)) {
			scan_number(8);
		}
		else {
			/* Lone '0'. */
			t_value = 0;
		}
	}
	else {
		scan_number(10);
	}

	if (ch == 'l' || ch == 'L') {
		sysnext();
		if(ch == 'u' || ch == 'U') {
			sysnext();
		}
		RETURN_INT("t_int", LONG_TOK);
	}
	else if (ch == 'u' || ch == 'U') {
		sysnext();
		if (ch == 'l' || ch == 'L') {
			RETURN_INT("t_int", LONG_TOK);
		}
		else {
			RETURN_INT("t_int", INT_TOK);
		}
	}
	else {
		RETURN_INT("t_int", INT_TOK);
	}
}

/*
	Copy a string into out[] and puts its length in the global t_length.
	Copy the opening or closing delimiters if the copy_flag is TRUE.

	This is used to parse both strings and character constants.
*/
void
t_string(out, max_out, copy_flag)
register char * out;	/* The output buffer.				*/
int	max_out;	/* The size of out[].				*/
bool	copy_flag;	/* Copy the delimiters if copy_flag is TRUE.	*/
{
	register int length;
	int	start_line;
	char *	start_string;
	char	line [10];
	int	delim;

	TRACECH("t_string");

	/* Save starting line number for error messages. */
	start_line = t_line;
	start_string = out;

	/* Handle the opening single or double quote */
	delim = ch;
	sysnext();
	length = 0;
	if (copy_flag) {
		*out++ = delim;
		length++;
	}

	max_out--;
	while (length < max_out) {

		switch(ch) {

		case END_FILE:
		case '\n':
			goto runon1;

		case '"':
		case '\'':
			if (ch == delim) {
				sysnext();
				if(copy_flag) {
					*out++ = delim;
					length++;
				}
				*out++ = '\0';
				t_length  = length;
				TRACEP("t_string", printf("<%s>\n",
					start_string));
				RETURN_VOID("t_string");
			}
			else{
				*out++ = ch;
				length++;
				sysnext();
			}
			continue;

		case '\\':

			sysnext();
			if (ch == END_FILE) {
				goto runon1;
			}
			else if (ch == '\n') {
				/* Ignore back slash and newline. */
				t_line++;
				sysnext();
			}
			else {
				*out++ = '\\';
				*out++ = ch;
				length += 2;
				sysnext();
			}
			continue;

		default:
			*out++ = ch;
			length++;
			sysnext();
		}
	}

	conv2s(start_line, line);
	err3("String starting at line ", line, " is too long.");

	*out = '\0';
	t_length = length;
	RETURN_VOID("t_string");

runon1:
	*out   = '\0';
	err2("String crosses a line: ", start_string);
	t_length  = length;
	LEAVE("t_string");
}

/*
	Parse a string (including delimiters) from in[] to out.
	Return the length of the ORIGINAL string.
*/
int
in_string(in, out, max_out)
char *	in;		/* The output buffer	*/
char *	out;		/* The output buffer.	*/
int	max_out;	/* The size of out[].	*/
{
	register int length;
	int	start_line;
	char *	start_string;
	char	line [10];
	int	delim;

	TRACEPB("in_string", printf("(<%s>, %p, %d)\n",
		in, out, max_out));

	/* Save starting line number for error messages. */
	start_line = t_line;
	start_string = out;

	/* Copy the opening single or double quote */
	*out++ = delim = *in++;
	length = 1;
	max_out--;
	while (length < max_out) {

		switch(*in) {

		case END_FILE:
		case '\n':
			goto runon1;

		case '"':
		case '\'':
			if (*in == delim) {
				*out++ = *in++;
				*out++ = '\0';
				length++;
				RETURN_INT("in_string", length);
			}
			else{
				*out++ = *in++;
				length++;
			}
			continue;

		case '\\':

			in++;
			if (*in == END_FILE) {
				goto runon1;
			}
			else if (*in == '\n') {
				/* Ignore back slash and newline. */
				t_line++;
				in++;

				/* Keep track of ORIGINAL length. */
				length += 2;
			}
			else {
				*out++ = '\\';
				*out++ = *in++;
				length += 2;
			}
			continue;

		default:
			*out++ = *in++;
			length++;
		}
	}

	conv2s(start_line, line);
	err3("Strinized argument starting at line ", line, " is too long.");
	*out = '\0';
	RETURN_INT("in_string", length);

runon1:
	*out   = '\0';
	err2("Stringized argument crosses a line: ", start_string);
	RETURN_INT("in_string", length);
}
