/*
	CPP V5 -- Main routine

	source:  cpp.c
	started: October 7, 1985
	version: see below

	Written by Edward K. Ream.
	This software is in the public domain.

	See the read.me file for disclaimer and other information.
*/

#include "cpp.h"

#define SIGNON "CPP V5.3:  August 4, 1989"

#ifdef SHERLOCK
#define USAGE1 "usage: cpp in out [options] ++/--routine\n\n"
#else
#define USAGE1 "usage: cpp in out [options]\n\n"
#endif

#define USAGE2	"-c           keep all comments and white space\n"
#define USAGE3  "-d id=value  #define id value\n"
#define USAGE4  "-n           comments DO nest\n"
#define USAGE5  "-s <path>    set search path for #include's\n"
#define USAGE6  "-u id        #undef id\n"
#define USAGE7  "-x           disallow single-line comments\n"

main(argc, argv)
int argc;
char **argv;
{
	char *in = NULL, *out = NULL;
	char *arg;
	char *def;
	char *p1, *p;
	char path_buf[200];

	/* These two calls MUST come before any others. */
	SL_INIT();
	SL_PARSE(argc, argv, "++", "--");

	/*
		The call to mst_init() MUST be placed here so we can define
		macros using the -d option.

		The call to mst2_init() must FOLLOW the gathering of
		command line arguments so that the __line__ and __file__
		macros may be disabled using the -u option.
	*/
	mst_init();

	/* Allow user to abort. */
	syscsts();

	TICKB("main");

	/* Always put out the sign on message. */
	printf("%s\n", SIGNON);

	/* Set search path from the INCLUDE environment variable. */
	arg = getenv("INCLUDE");
	if (arg != 0) {
		strcpy(path_buf, arg);
		arg = path_buf;
		for (p = &path_buf[0]; ;) {
			if (*p != ';' && *p != '\0') {
				p++;
				continue;
			}
			if (*p == ';') {
				*p = '\0';
				p++;
			}
			if (n_paths >= MAX_PATHS) {
				printf("too many default path names.\n");
				exit(BAD_EXIT);
			}
			else {
				p1 = str_mcat(arg, "\\");
				TRACEP("main",
					printf("default path: %s\n", arg));
				paths [n_paths++] = p1;
			}
			if (*p == '\0') {
				break;
			}
			else {
				arg = p;
			}
		}
	}

	/* Make first test for correct command line. */
	if (argc < 3) {
		printf("%s%s%s%s", USAGE1, USAGE2,  USAGE3,  USAGE4);
		printf("%s%s%s", USAGE5, USAGE6, USAGE7);
		exit(BAD_EXIT);
	}

	/* Process all the arguments on the command line. */
	argc--;
	argv++;
	while (argc-- > 0) {
		arg = *argv++;

		if (str_eq(arg, "-c")) {
			/* Keep comments in output. */
			com_flag = TRUE;
		}
		else if(str_eq(arg, "-d")) {
			/* Define a variable. */
			if (argc--) {
				arg = *argv++;
				/* Scan for an optional equal sign. */
				for (def = arg;	*def; def++) {
					if (*def == '=') {
						*def = '\0';
						def++;
						break;
					}
				}
				mst_enter(arg, def, -1);
			}
			else {
				printf("Trailing -d\n");
				exit(BAD_EXIT);
			}
		}
		else if (str_eq(arg, "-n")) {
			/* Disallow nested comments. */
			nest_flag = FALSE;
		}
		else if (str_eq(arg, "-s")) {
			/* Define a path. */
			if (argc--) {
				arg = *argv++;
				if (n_paths >= MAX_PATHS) {
					printf("too many path names.\n");
					exit(BAD_EXIT);
				}
				else {
					p1 = str_mcat(arg, "\\");
					paths [n_paths++] = p1;
				}
			}
			else {
				printf("Trailing -s.\n");
				exit(BAD_EXIT);
			}
		}
		else if(str_eq(arg, "-u")) {
			/* Suppress the initial definition of a variable. */
			if (argc--) {
				arg = *argv++;
				mst_unarg(arg);
			}
			else {
				printf("Trailing -u.\n");
				exit(BAD_EXIT);
			}
		}
		else if (str_eq(arg, "-x")) {
			/* 8/1/89 Disable single-line comments. */
			slc_flag = FALSE;
		}
		else if (in == NULL) {
			in = arg;
		}
		else if (out == NULL) {
			out = arg;
		}
		else {
			printf("Extra file argument: %s\n", arg);
			exit(BAD_EXIT);
		}
	}

	/* Make sure that both file arguments were provided. */
	if (in == NULL) {
		printf("Missing input, output file arguments.\n");
		exit(BAD_EXIT);
	}
	else if (out == NULL) {
		printf("Missing output file argument.\n");
		exit(BAD_EXIT);
	}
	else if (str_eq(in, out)) {
		fatal("Can not copy input file to output file.");
	}

	/*
		Open the output file.

		This should be done BEFORE opening the input file because
		opening the input file will cause a character to be written
		to the output file.
	*/
	if (syscreat(out) == FALSE) {
		printf("Can not open %s\n", out);
		exit(BAD_EXIT);
	}

	/* Open the input file. */
	if (sysopen(in) == FALSE) {
		printf("Can not open %s\n", in);
		sysabort();
	}

	/*
		Initialize the predefined macros (__line__ and __file__) here
		so that they can be suppressed with the -u command line option.
	*/
	mst2_init();

	/* Start off at a new line. */
	begin_line(TRUE);

	/* Copy the program to the output file. */
	copy_file();

	/* Close the output file. */
	sysoclose();

	TRACE("m_stat", m_stat());
	TRACE("dump", sl_dump());

	sysend();

	LEAVE("main");
	exit(0);
}

/*
	Copy the input file to the output file.
*/

void
copy_file()
{
	int	i;
	bool	old_mflag;
	char	id_buf[MAX_SYMBOL];

	TICKB("copy_file");

	t_symbol[0] = '\0';
	t_subtype = 0;

	/*
		We branch to the 'rescan' label whenever a construct is seen
		that does not result directly in a token being returned, i.e.,
		for macros, PP directives and whitespace.
	*/

loop:
	TRACEP("copy_file_loop", printf("ch: %s\n", pr_ch(ch)));

	if (isid1(ch)) {
		old_mflag = m_flag;
		t_id(&t_symbol[0], MAX_SYMBOL);
		if (!outer_expand(&t_symbol[0], old_mflag)) {
			syssput(&t_symbol[0]);
		}
		goto loop;
	}

	switch (ch) {

	case ' ':
	case '\t':
		syscput(ch);
		sysnext();
		goto loop;

	case '\n':

                /* Allow user to abort here. */
		syscsts();

		sysnlput();
		sysnext();
		bump_line();
		begin_line(TRUE);
		goto loop;

	case '#':
		error("Unexpected # ignored.");
		sysnext();
		goto loop;

	case '/':
		/* comment or / or /= or // */
		sysnext();
		if (ch == '*') {
			sysnext();
			if (com_flag) {
				syscput('/');
				syscput('*');
				copy_comment();
			}
			else {
				skip_comment();
			}
		}
		else if (slc_flag && ch == '/') {
			/* 8/1/89 C++ style single-line comment. */
			skip_past();
			goto loop;
		}
		else {
			syscput('/');
		}
		goto loop;

	case '\'':
		t_string(&t_symbol[0], MAX_SYMBOL, TRUE);
		syssput(&t_symbol[0]);
		goto loop;

	case '"':
		/* Do concatenation of string literals here. */
		TRACE("nocat",
			t_string(&t_symbol[0], MAX_SYMBOL, TRUE);
			syssput(&t_symbol[0]);
			goto loop;
		);

		/*
			Note that a macro may expand into several strings,
			so that we have to be careful not to stop searching
			for more strings to concatenate too soon.
		*/

		t_symbol[0] = '"';
		for(i = 1;;) {
			if (ch == ' ' || ch == '\t') {
				sysnext();
			}
			else if (ch == '"') {
				t_string(&t_symbol[i], MAX_SYMBOL-i, FALSE);
				i = strlen(&t_symbol[0]);
			}
			else if (isid1(ch)) {
				old_mflag = m_flag;
				t_id(&id_buf[0], MAX_SYMBOL);
				if (!outer_expand(&id_buf[0], old_mflag)) {
					/* Not a macro. */
					syssput(&id_buf[0]);
					goto loop;
				}
			}
			else {
				break;
			}
		}
		t_symbol[i++] = '"';
		t_symbol[i]   = '\0';
		syssput(&t_symbol[0]);
		goto loop;

	/*
		We must be VERY careful about exactly when we switch from one
		input file to the next.  This is the place. 
	*/

	case END_FILE:

		/* Switch input streams. */
		sysiclose();
		if (t_inlevel == -1) {
			RETURN_VOID("copy_file");
		}
		else {
			begin_line(TRUE);
			goto loop;
		}

	default:
		/* Be permissive. */
		syscput(ch);
		sysnext();
		goto loop;
	}
}

/*
	Return the next preprocessor token.
	This should be called only in contexts where output is NOT required.
*/

#ifdef SHERLOCK
char bug_s1 [] = "get_token";
char bug_s2 [] = "returns token %d, %s\n";
#endif

/* CAUTION: evaluate value only once! */
#define T_RETURN(value)\
	token = value;\
	TRACEPN(bug_s1, printf(bug_s2, token, pr_tok()));\
	return;

void
get_token(expand_flag)
bool expand_flag;
{
	int i;
	bool	old_mflag;
	unsigned char mesgbuf [40];
	unsigned char cbuf [2];

	TRACEP("get_token", printf("(%s) line %d\n",
		pr_bool(expand_flag), t_line));

	t_symbol[0] = '\0';
	t_subtype = 0;

	/*
		We branch to the 'rescan' label whenever a construct is seen
		that does not result directly in a token being returned, i.e.,
		for macros, PP directives and whitespace.
	*/
rescan:

	/* Special case for "wide" characters and strings. */
	if (ch == 'L') {
		sysnext();
		if (isid2(ch)) {
			syspushback(ch);
			ch = 'L';
		}
	}

	if (isid1(ch)) {
		old_mflag = m_flag;
		t_id(&t_symbol[0], MAX_SYMBOL);
		if (expand_flag && outer_expand(&t_symbol[0], old_mflag)) {
			goto rescan;
		}
		else {
			T_RETURN(ID_TOK);
		}
	}

	TICK("get_token1");

	switch (ch) {

	case ' ':
	case '\t':
		sysnext();
		goto rescan;

	case '\n':
	case END_FILE:

		TRACEPN("get_token", printf("con_flag: NULL_TOK\n"));
		T_RETURN(NULL_TOK);

	case '#':
		error("Unexpected # ignored.");
		sysnext();
		goto rescan;

	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':

		T_RETURN(t_number(FALSE));

	case '.':
		sysnext();
		if (ch >= '0' && ch <= '9') {
			T_RETURN(t_number(TRUE));
		}
		else if (ch == '.') {
			sysnext();
			if (ch == '.') {
				sysnext();
				T_RETURN(DOTS3);
			}
		}
		else {
			T_RETURN(DOT_TOK);
		}

	case '"':
		t_string(&t_symbol[0], MAX_SYMBOL, FALSE);
		T_RETURN(STRING_TOK);

	case '\'':
		t_string(&t_symbol[0], MAX_SYMBOL, FALSE);
		t_value = (long) char_val(&t_symbol[0]);
		T_RETURN(CHAR_TOK);


	case '=':	/* = or == */

		sysnext();
		if (ch == '=') {
			sysnext();
			T_RETURN(EQUAL_TOK);
		}
		else {
			T_RETURN(ASSN_TOK);
		}


	case '+':	/* + or ++ or += */

		sysnext();
		if (ch == '+') {
			sysnext();
			T_RETURN(INC_TOK);
		}
		else if (ch == '=') {
			sysnext();
			T_RETURN(PLUS_ASSN_TOK);
		}
		else {
			T_RETURN(PLUS_TOK);
		}


	case '-':	/* - or -- or -> */

		sysnext();
		if (ch == '=') {
			sysnext();
			T_RETURN(MINUS_ASSN_TOK);
		}
		else if (ch == '-') {
			sysnext();
			T_RETURN(DEC_TOK);
		}
		else if (ch == '>') {
			sysnext();
			T_RETURN(ARROW_TOK);
		}
		else {
			T_RETURN(MINUS_TOK);
		}


	case '*':	/* * or *= */

		sysnext();
		if (ch == '=') {
			sysnext();
			T_RETURN(STAR_ASSN_TOK);
		}
		else {
			T_RETURN(STAR_TOK);
		}


	case '/':	/* comment or / or /= */

		sysnext();
		if (ch == '*') {
			sysnext();
			skip_comment();
			goto rescan;
		}
		else if (ch == '=') {
			sysnext();
			T_RETURN(DIV_ASSN_TOK);
		}
		else {
			T_RETURN(DIV_TOK);
		}


	case '%':	/* % or %= */

		sysnext();
		if (ch == '=') {
			sysnext();
			T_RETURN(MOD_ASSN_TOK);
		}
		else {
			T_RETURN(MOD_TOK);
		}


	case '>':	/* > or >= or >> or >>= */

		sysnext();
		if (ch == '>') {
			sysnext();
			if (ch == '=') {
				sysnext();
				T_RETURN(RSHIFT_ASSN_TOK);
			}
			else {
				T_RETURN(RSHIFT_TOK);
			}
		}
		else if (ch == '=') {
			sysnext();
			T_RETURN(GE_TOK);
		}
		else {
			T_RETURN(GT_TOK);
		}


	case '<':	/* < or or <= or << or <<= */

		sysnext();
		if (ch == '<') {
			sysnext();
			if (ch == '=') {
				sysnext();
				T_RETURN(LSHIFT_ASSN_TOK);
			}
			else {
				T_RETURN(LSHIFT_TOK);
			}
		}
		else if (ch == '=') {
			sysnext();
			T_RETURN(LE_TOK);
		}
		else {
			T_RETURN(LT_TOK);
		}


	case '!':	/* ! or != */

		sysnext();
		if (ch == '=') {
			sysnext();
			T_RETURN(NE_TOK);
		}
		else {
			T_RETURN(NOT_TOK);
		}
			

	case '|':	/* | or |= or || */

		sysnext();
		if (ch == '=') {
			sysnext();
			T_RETURN(OR_ASSN_TOK);
		}
		else if (ch == '|') {
			sysnext();
			T_RETURN(LOR_TOK);
		}
		else {
			T_RETURN(OR_TOK);
		}


	case '&':	/* & or &= or && */

		sysnext();
		if (ch == '=') {
			sysnext();
			T_RETURN(AND_ASSN_TOK);
		}
		else if (ch == '&') {
			sysnext();
			T_RETURN(LAND_TOK);
		}
		else {
			T_RETURN(AND_TOK);
		}


	case '^':	/* ^ or ^= */

		sysnext();
		if (ch == '=') {
			sysnext();
			T_RETURN(XOR_ASSN_TOK);
		}
		else {
			T_RETURN(XOR_TOK);
		}


	case '?':	sysnext();	T_RETURN(QUESTION_TOK);
	case ':':	sysnext();	T_RETURN(COLON_TOK);

	case '~':	sysnext();	T_RETURN(TILDE_TOK);
	case ',':	sysnext();	T_RETURN(COMMA_TOK);
	case '(':	sysnext();	T_RETURN(LPAREN_TOK);
	case ')':	sysnext();	T_RETURN(RPAREN_TOK);
	case '[':	sysnext();	T_RETURN(LBRACK_TOK);
	case ']':	sysnext();	T_RETURN(RBRACK_TOK);
	case '{':	sysnext();	T_RETURN(LCURLY_TOK);
	case '}':	sysnext();	T_RETURN(RCURLY_TOK);
	case ';':	sysnext();	T_RETURN(SEMICOLON_TOK);

	default:

		strcpy(mesgbuf, "Character error: ");
		cbuf [0] = ch;
		cbuf [1] = '\0';
		strcat(mesgbuf, cbuf);
		error(mesgbuf);
		sysnext();
		goto rescan;
	}
}

#undef T_RETURN

/*
	Do beginning of line processing.
	Look for preprocessor directives only if flag is TRUE.
*/
void
begin_line(flag)
bool flag;
{
	TRACEPB("begin_line", printf("t_line: %d\n", t_line));

	for (;;) {
		if (com_flag) {
			copy_ws(TRUE);
		}
		else {
			skip_ws(TRUE);
		}

		/* PP directives are not allowed as the result of macro expansion. */
		if (flag && ch == '#' && !m_flag) {
			sysnext();
			do_pp();
		}
		else {
			break;
		}
	}

	RETURN_VOID("begin_line");
}

/*
	Bump the line number of the current file.
*/
void
bump_line()
{
	t_line++;
	TRACEP("bump_line", printf("t_line = %d\n", t_line));
}
