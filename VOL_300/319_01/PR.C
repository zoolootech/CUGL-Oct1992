/*
	CPP V5 -- printing routines for debugging.

	source:  pr.c
	started: May 19, 1986
	version: May 31, 1988

	Written by Edward K. Ream.
	This software is in the public domain.

	See the read.me file for disclaimer and other information.
*/

#include "cpp.h"


#ifdef SHERLOCK

/*
	WARNING!!  This table must change if enum.h changes.
*/
static char * tok_tab [] = {
	"<NULL>", "<ERROR>",

	"auto", "char", "const", "double", "extern",
	"float", "int", "long", "register", "short",
	"static", "typedef", "signed", "struct", "union",
	"unsigned", "void", "volatile",

	"break", "case",
	"continue", "default", "do", "else", "enum", "for",
	"goto",	"if", "return", "switch", "while",

	"entry", "sizeof",

	",", "<newline>", ";", "[", "{", "(", "]", "}", ")",

	":", "?",

	"ARRAY", "->", ".", "&&", "||", ",",

	"=",
	"&=", "/=", "<<=", "-=", "%=",
	"|=", "+=", ">>=", "*=", "^=",

	"&", "|", "+", "*", "^",
	"/", "<<", "-", "%", ">>",

	"==", ">=", ">", "<=", "<", "!=",

	"--", "++", "!", "~",

	"(cast)", "post--", "post++", "pre--", "pre++",
	"&", "-", "+", "*", "call",
	
	"character", "EOP",
	"identifier", "integer", "float", "long integer",
	"string",
	"...", "label"
};

/*
	Return the print string of a bool.
*/
char *
pr_bool(flag)
bool flag;
{
	return flag ? "TRUE" : "FALSE";
}

/*
	Return the print string of a character.
*/
static char pdefault[] = "@";

char *
pr_ch(c)
int c;
{
	switch (c) {
	case ARG_FLAG:		return "ARG:";
	case POUND_FLAG:	return "POUND:";
	case CONCAT_FLAG:	return "<CONCAT_FLAG>";
	case EORT:		return "<EORT>";
	case EXPAND_OFF:	return "<EXPAND_OFF>";
	case '\0':		return "<\\0>";
	case '\t':		return "<TAB>";
	case '\n':		return "<NL>";
	case '\r':		return "<CR>";
	case '\a':		return "<ALERT>";
	case ' ':		return "< >";
	case END_FILE:		return "<END_FILE>";
	default:
		pdefault[0] = c;
		pdefault[1] = '\0';
		return &pdefault[0];
	}
}

/*
	Return the print string of tok.
	Do not assume that t_symbol[] refers to tok.
*/
char *
pr_op(tok)
en_tokens tok;
{
	if ((int)tok >= 0 && (int)tok <= LABEL_TOK) {
		return tok_tab [(int)tok];
	}
	else {
		return "BAD TOKEN";
	}
}

/*
	Return the print string of a string.
*/
static char pr_buf [1000];

char *
pr_str(s)
char *s;
{
	strcpy(pr_buf, "");
	while (*s) {
		strcat(pr_buf, pr_ch(*s));
		s++;
	}
	return &pr_buf[0];
}

/*
	Return the print string of the current token.
	Assume the t_symbol refers to the global token.
*/
static char buffer [200];
static char buffer2 [100];

char *
pr_tok()
{
	if (is(ID_TOK)) {
		if (t_length >= 200-5) {
			printf("pr_tok:  bad id: %s\n", t_symbol);
			exit(1);
			return "";
		}
		else {
			strcpy(buffer, "id: ");
			strcat(buffer, t_symbol);
			return buffer;
		}
	}
	else if (is(STRING_TOK)) {
		if (t_length >= 200-5) {
			printf("pr_tok:  bad string: %s\n", t_symbol);
			exit(1);
			return "";
		}
		else {
			strcpy(buffer, "string: ");
			strcat(buffer, t_symbol);
			return buffer;
		}
	}
	else if (is(INT_TOK)) {		
			strcpy(buffer, "integer: ");
			ltoa(t_value, buffer2, 10);
			strcat(buffer, buffer2);
			return buffer;
	}
	else if (token < LABEL_TOK) {
		return  tok_tab [(int)token];
	}
	else {
		return "BAD TOKEN";
	}
}

#endif /* SHERLOCK */
