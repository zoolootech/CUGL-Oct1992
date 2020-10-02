/* TLCGTOK.C - "The Last Cross-referencer" - C Get Token routines		*/
/*	Last Modified:	02/10/89											*/

/*
---------------------------------------------------------------------
Copyright (c) 1987-1989, Eduard Schwan Programs [esp] - All rights reserved
TLC (The Last C-Cross-Referencer) and TLP (same, but for Pascal) are
Cross-Reference Generators crafted and shot into the Public Domain by
Eduard Schwan.  The source code and executable program may be freely
distributed as long as the copyright/author notices remain intact, and
it is not used in part or whole as the basis of a commercial product.
Any comments, bug-fixes, or enhancements are welcome.
Also, if you find TLC and it's source code useful, a contribution of
$20 (check/money order) is encouraged!  Hopefully we will all see more
source code distributed!
	Eduard Schwan, 1112 Oceanic Drive, Encinitas, Calif. 92024
---------------------------------------------------------------------
*/

/*
HEADER:		The Last Cross-Referencer;
TITLE:		TLC/TLP - The Last Cross-Referencer;
VERSION:	1.01;

DESCRIPTION: "TLC/TLP.
			C Get Token routines";

KEYWORDS:	Utility, Cross-reference, C, Pascal, Apple, Macintosh, APW, Aztec;
SYSTEM:		Macintosh MPW, v3.0;
FILENAME:	TLCGTOK.C;
WARNINGS:	"Has not yet been ported to MS-DOS.
			Shareware, $20 Check/Money Order suggested.";

SEE-ALSO:	README.TLC,TLCHELP.DOC,TLPHELP.DOC;
AUTHORS:	Eduard Schwan;
COMPILERS:	AZTEC C65 v3.2b, APPLEIIGS APW C v1.0, APPLE MACINTOSH MPW C v3.0;
*/


/*------------------------ include files -------------------------*/

#include	<stdio.h>
#include	<ctype.h>
#include	"tlc.h"

/*------------------------- definitions -------------------------*/

#define 	Eoln				0	/* character constants */
#define 	Tab 				'\t'
#define 	CReturn 			'\r'
#define 	Newline 			'\n'
#define 	Space				' '
#define 	Minus				'-'
#define 	Plus				'+'
#define 	Equals				'='
#define 	Single_quote		'\''
#define 	Double_quote		'"'
#define 	Pound_sign			'#'
#define 	Slash				'/'
#define 	Backslash			'\\'
#define 	Asterisk			'*'
#define 	Period				'.'
#define 	Underscore			'_'


/*--------------------- external declarations --------------------*/

#include	"tlc.ext"


/*------------------------ static variables -----------------------*/

static char 			curr_ch;
static pos_int			curr_column;
static int				error;
static boolean			got_token;
static boolean			in_comment;
static boolean			pushed_token;
static token_rec_type	prev_token;
static int				sym_length;

static boolean			case_sensitive;
static boolean			do_underscores;
static boolean			del_quotes;


/*================[ init_scanner ]==================*/

VOID init_scanner(Case_Sensitive, Do_Underscores, Del_Quotes)
boolean 	Case_Sensitive;
boolean 	Do_Underscores;
boolean 	Del_Quotes;

	{ /* init_scanner() */
/*
debug(printf("init_scanner:case=%d under=%d delq=%d\n",\
Case_Sensitive,Do_Underscores,Del_Quotes);)
*/
	case_sensitive = Case_Sensitive;
	do_underscores = Do_Underscores;
	del_quotes	   = Del_Quotes;
	curr_column = 0;
	error		= 0;
	in_comment	= FALSE;
	pushed_token= FALSE;
	} /* init_scanner() */


/*=================[ push_token ]===================*/

VOID push_token()

	{ /* push_token() */
/*
debug(printf("push_token:\n");)
*/
	if (pushed_token)
		{
		fprintf(stderr,"Error! Token stack overflow..");
		exit(1);
		}
	else
		{
		prev_token = token;
		pushed_token = TRUE;
		}
	} /* push_token() */


/*================[ init_sym_buff ]=================*/

static VOID init_sym_buff()

	{ /* init_sym_buff() */
/*-------- do this if no FILLCHAR ------------
pos_int k;
for (k=0;k<LINE_SIZE;k++) token.tok_string[k]=0;
----------------------------------------------*/
	FILLCHAR(token.tok_string, LINE_SIZE, 0);
	sym_length = 0;
/*debug(printf("init_sym_buff:\n");)*/
	} /* init_sym_buff() */


/*================[ add_sym_buff ]=================*/

static VOID add_sym_buff(curr_ch)
char curr_ch;

	{ /* add_sym_buff() */
/*
debug(printf("add_sym_buff:'%c'\n",curr_ch);)
*/
	if (sym_length < LINE_SIZE)
		{
		token.tok_string[sym_length] = curr_ch;
		sym_length++;
		}
	} /* add_sym_buff() */


/*================[ del_sym_buff ]=================*/

static VOID del_sym_buff()

	{ /* del_sym_buff() */
/*
debug(printf("del_sym_buff:\n");)
*/
	if (sym_length > 0)
		{
		sym_length--;
		token.tok_string[sym_length] = Eoln;
		}
	} /* del_sym_buff() */


/*================[ in_symbolset ]=================*/

static boolean in_symbolset(ch)
register char	ch;

	{ /* in_symbolset() */
	return(
			isalpha(ch) || isdigit(ch) || ch==Underscore
		  );
	} /* in_symbolset() */


/*==============[ do_identifier ]===============*/

static VOID do_identifier()

	{ /* do_identifier() */
	boolean 	done;

/*
debug(printf("do_identifier:\n");)
*/
	done = FALSE;
	while (!error && !done)
		{
		curr_ch = curr_line[curr_column];
		if (!case_sensitive)
			curr_ch = tolower(curr_ch);
		if (curr_ch == Eoln)
			done = TRUE;
		else
		if (in_symbolset(curr_ch))
			{
			if (curr_ch!=Underscore || do_underscores)
				add_sym_buff(curr_ch);
			curr_column++;
			}
		else
			done = TRUE;
		} /*while*/

	if (!error)
		got_token = TRUE;

	} /* do_identifier() */


/*================[ in_numberset ]=================*/

static boolean in_numberset(ch,first_time,base)
register char		ch;
register boolean	first_time;
register pos_int	base;

	{ /* in_numberset() */
	return(
			isdigit(ch) ||
			ch=='x' || ch=='e' || ch=='l' ||
			ch==Period	||
			(first_time && (ch==Minus || ch==Plus)) ||
			(base==B_hex && ch>='a' && ch<='f')
		  );
	} /* in_numberset() */


/*================[ convert_number ]=================*/

static long convert_number(sptr,base)
char*		sptr;
pos_int 	base;

	{ /* convert_number() */
	register char	ch;
	register long	result;
	boolean 		done;

/*
debug(printf("convert_number:'%s'\n",sptr);)
*/
	result	= 0;
	done	= FALSE;

	while (*sptr && !done)
		{
		ch = *(sptr++);
		if (isdigit(ch))
			{ /* shift result & add next digit in */
			result = result*base + (ch-'0');
			}
		else
		if (base==B_hex && ch>='a' && ch<='f')
			result = result*base + (ch-'a'+10);
		else
		if (ch!='x') /* only x (for hexmode) is allowable */
			done = TRUE;
/*
debug(printf("  added %c and got %ld\n",ch,result););
*/
		}
	return(result);
	} /* convert_number() */


/*================[ do_number ]=================*/

static VOID do_number()

	{ /* do_number() */
	boolean 	done,
				first_time;
	pos_int 	base;

/*
debug(printf("do_number:\n");)
*/
	done			= FALSE;
	first_time		= TRUE;
	if (curr_line[curr_column] == '0')
		base	= B_octal;
	else
		base	= B_decimal;
	while (!error && !done)
		{
		curr_ch = tolower(curr_line[curr_column]);
		if (curr_ch == Eoln)
			done = TRUE;
		else
			{ /* see if it's a valid digit, if so, add it */
			if (curr_ch=='x')
				base = B_hex;
			if (in_numberset(curr_ch,first_time,base))
				{
				add_sym_buff(curr_line[curr_column]);
				curr_column++;
				first_time = FALSE;
				}
			else
				{
				done = TRUE;
/*
debug(printf(" stopping because %c/%d isnt in number_set\n",curr_ch, curr_ch);)
*/
				}
			} /* see if it's a valid digit, if so, add it */
		} /*while*/

	/* all done validating & collecting number, now convert it */
	if (!error)
		token.tok_value = convert_number(token.tok_string,base);

	if (!error)
		got_token = TRUE;

	} /* do_number() */


/*================[ do_str_literal ]=================*/

static VOID do_str_literal()

	{ /* do_str_literal() */
	char		terminator;
	boolean 	done,skip_slash;

/*
debug(printf("do_str_literal:\n");)
*/
	terminator = curr_ch; /* single/double quote */

/* remove leading/trailing quotes */
	if (del_quotes)
		init_sym_buff();

	/* read until trailing quote found */
	done = FALSE;
	while (!error && !done)
		{
		curr_ch = curr_line[curr_column];
		skip_slash = FALSE;
		if (curr_ch == Eoln)
			{ /* Hit End of line without finding trailing quote - error */
			error = TERR_BAD_STR;
			}
		else
			if (curr_ch == terminator)
				{
				done = TRUE; /* found trailing quote - end */
				}
			else
				if (curr_ch == Backslash)
					{ /* skip next character (in case its emedded terminator)*/
					skip_slash = TRUE;
					}

			/* if valid character in string, add it to buffer */
			if (!error)
				{
				if (!(done && del_quotes))
					add_sym_buff(curr_ch);
				}
			curr_column++;

		/* if backslash "escape sequence", put the next character */
		/* in the buffer too, but don't check it for ' or "       */
		if (skip_slash)
			{
			skip_slash = FALSE;
			curr_ch = curr_line[curr_column];
			add_sym_buff(curr_ch);
			curr_column++;
			}
		} /*while*/

	if (!error)
		got_token = TRUE;

	} /* do_str_literal() */


/*================[ do_special_syms ]=================*/

static VOID do_special_syms()

	{ /* do_special_syms() */

/*
debug(printf("do_special_syms:\n");)
*/
	/*
	the token type for special symbols is the ASCII code of
	that symbol, unless it is a multi-char. symbol, in which
	case the results of the following line are changed later
	*/
	token.tok_type = curr_ch;

	/*
	now handle each character type
	*/
	switch (curr_ch)
		{
		case Slash:
			if (curr_line[curr_column++]==Asterisk)
				{
				in_comment = TRUE;
/*debug(puts("/*--entering comment");)*/
				}
			else
				{
				curr_column--;
				got_token = TRUE;
				}
			break;

		case Minus: /**** TEMPORARY SMARTS.. SOMEDAY THIS SHOULD BE DONE IN PARSER! ****/
			if (isdigit(curr_line[curr_column+1]))
				{
				do_number();
				token.tok_type = TOK_NCONST;
				}
			else
				got_token = TRUE;
			break;
		case Equals:
		case Pound_sign:
		default:
/*------ allow any characters for now DEBUG!
			error = TERR_BAD_CHR;
-----------------------------------------------*/ got_token = TRUE;
			break;
		} /*switch*/

	} /* do_special_syms() */


/*=================[ get_token ]===================*/

int get_token()

	{ /* get_token() */

	if (pushed_token)
		{ /* retrieve previous token & exit */
		token		 = prev_token;
		pushed_token = FALSE;
debug(printf(" get prev. token\n");)
		return(0); /* no error */
		}

	got_token			= FALSE;
	error				= 0;
	token.tok_type		= 0;
	token.tok_value 	= 0;
	token.tok_string[0] = 0;
	do	{
		/*
		skip if still in comment from previous call
		*/
		if (in_comment)
			{
			while (in_comment &&
				   (curr_ch = curr_line[curr_column]) != Eoln)
				{
				curr_column++;
				if (curr_ch == Asterisk)
					{
					curr_ch = curr_line[curr_column];
					if (curr_ch == Slash)
						{
						in_comment = FALSE;
						curr_column++;
/*debug(puts("--** exiting comment");)*/
						}
					}
				}
			if (in_comment) /* then must have hit end of line */
				{
				error = TERR_EOLN;
				token.tok_column = curr_column;
				curr_column = 0;
				}
			} /* if in_comment */

		/*
		skip leading spaces/tabs or trailing \n or \r
		*/
		if (!error)
			{
			while (curr_line[curr_column] == Space ||
				   curr_line[curr_column] == Tab ||
				   curr_line[curr_column] == CReturn ||
				   curr_line[curr_column] == Newline)
				{
				curr_column++;
				}
			if (curr_line[curr_column] == Eoln)
				{ /* hit end of line */
				error = TERR_EOLN;
				token.tok_column = curr_column;
				curr_column = 0;
				}
			}

		/*
		now at 1st character of new token, handle it
		*/
		if (!error)
			{ /* if !error */
			curr_ch = curr_line[curr_column++];
			token.tok_column = curr_column;
			if (!case_sensitive)
				curr_ch = tolower(curr_ch);
			init_sym_buff();
			add_sym_buff(curr_ch);
			if (!error)
				{
				if (isalpha(curr_ch) || curr_ch==Underscore)
					{
					do_identifier();
					token.tok_type = TOK_ID;
					}
				else
				if (isdigit(curr_ch))
					{
					do_number();
					token.tok_type = TOK_NCONST;
					}
				else
				if (curr_ch==Single_quote)
					{
					do_str_literal();
					token.tok_type	= TOK_CCONST;
					token.tok_value = (unsigned int) token.tok_string[0];
					}
				else
				if (curr_ch==Double_quote)
					{
					do_str_literal();
					token.tok_type = TOK_SCONST;
					}
				else
					{ /* check for special symbols */
					do_special_syms();
					}
				}
			} /* if !error */

		} while (!(error || got_token));

/*
printf("get_token:typ=%d col=%d val=%ld str='%s' err=%d\n",
token.tok_type,token.tok_column,token.tok_value,token.tok_string,error);
*/
	return (error);
	} /* get_token() */
