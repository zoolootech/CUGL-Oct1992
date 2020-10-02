/*
	CPP V5 -- directives

	source:  dir.c
	started: October 7, 1985
	version: May 31, 1988

	External routines defined in this file: do_pp

	Written by Edward K. Ream.
	This software is in the public domain.

	See the read.me file for disclaimer and other information.
*/

#include "cpp.h"

#define TRACE_LINE(name) TRACEP(name, printf("line %d\n", t_line))

static long	eval		(void);
static long 	eval1		(void);
static bool	gt_prec		(en_tokens, en_tokens);
static bool	isfnch		(int, en_tokens);

static void	pp_elif		(void);
static void	pp_else		(void);
static void	pp_endif	(void);
static void	pp_enum		(void);
static void	pp_error	(void);
static void	pp_if		(void);
static void	pp_ifdef	(bool);
static void	pp_incl		(void);
static void	pp_line		(void);
static void	pp_undef	(void);
static int	prec		(int);

static void	push_op		(en_tokens);
static en_tokens pop_op		(void);
static void	push_val	(long);
static long	pop_val		(void);
static void	skip_lines	(void);

/*
	Do one preprocessor directive.
*/

#define EQL(string) str_eq(t_symbol+1,string+1)

void
do_pp()
{
	TICK("do_pp");

	skip_ws(FALSE);

	/* Get the directive into t_symbol[]. */
	if (!isalpha(ch)) {
		goto not_alpha;
	}
	t_id(t_symbol, MAX_SYMBOL);

	/* 3/3/89: bug fix:  full white space allowed here. */
	skip_ws(FALSE);

	/* Skip simple white space after the directive. */
	/* -----
	while (ch == ' ' || ch == '\t') {
		sysnext();
	}
	----- */

	switch(t_symbol [0]) {

	case 'd':
		if (t_length == 6 && EQL("define")) {
			pp_def();
			return;
		}
		goto not_pp;

	case 'e':
		if (t_length == 4) {
			if (EQL("else")) {
				pp_else();
				return;
			}
			else if (EQL("elif")) {
				pp_elif();
				return;
			}
		}
#ifdef HAS_PP_ENUM
		else if (t_length == 4 && EQL("enum")) {
			pp_enum();
			return;
		}
#endif
		else if (t_length == 5 && EQL("endif")) {
			pp_endif();
			return;
		}
		else if (t_length == 5 && EQL("error")) {
			pp_error();
			return;
		}
		goto not_pp;

	case 'i':
		switch(t_length) {
		case 2:	if (EQL("if")) {
				pp_if();
				return;
			}
			goto not_pp;
		case 5:	if (EQL("ifdef")) {
				pp_ifdef(TRUE);
				return;
			}
			goto not_pp;
		case 6:	if (EQL("ifndef")) {
				pp_ifdef(FALSE);
				return;
			}
			goto not_pp;
		case 7:	if (EQL("include")) {
				pp_incl();
				return;
			}
			goto not_pp;
		}
		goto not_pp;

	case 'l':
		if (t_length == 4 && EQL("line")) {
			pp_line();
			return;
		}
		goto not_pp;

	case 'p':
		if (t_length == 6 && EQL("pragma")) {
			/* Do NOTHING!! */
			skip_pp();
			return;
		}
		goto not_pp;
	
	case 'u':
		if (t_length == 5 && EQL("undef")) {
			pp_undef();
			return;
		}
		goto not_pp;

	default:
		goto not_pp;
	}
	
not_alpha:
	/*
		Be more permissive than the new C standard.
		Just skip the rest of the line.
	*/
	skip_pp();
	return;

not_pp:
	err2(t_symbol, " is not a valid preprocessor directive.");
	skip_pp();
}

#undef EQL

/*
	Handle the #error directive.
	Produce a diagnostic message.
*/
static char	err_msg[] = "#error:  ";

static void
pp_error()
{
	char	message[MAX_SYMBOL];
	int	i;

	strcpy(message, err_msg);
	for (i = strlen(message); i < MAX_SYMBOL; i++) {
		if (ch == '\n' || ch == END_FILE) {
			break;
		}
		else {
			message[i] = ch;
			sysnext();
		}
	}
	message[i] = '\0';
	fatal(message);
}
		
	
/*
	Evaluate a constant expression to either true or false.
	A constant expression consists of:

	1. integer constants or character constants
	2. the unary - + and ~ operators
	3. the binary + - * / & | ^ << >> == != < > <= >= oprators
	4. the ternary ? : operator
	5. the ( and ) groupers.

	Identifiers are expanded if they are defined, otherwise they
	are taken to have a value of zero. All arithmetic is integer and
	ints are expanded to long.
*/

#define MAX_EVAL_VAL 100
#define MAX_EVAL_OP 50

static	long 		val_stack[MAX_EVAL_VAL];
static	int		val_ptr = 0;
static	en_tokens	op_stack[MAX_EVAL_OP];
static	int		op_ptr = 0;
static long result;
static bool paren_seen;
static bool error_seen;

static long
eval()
{
	TRACETOK("eval");

	error_seen = FALSE;
	get_token(TRUE);
	result = eval1();

	RETURN_LONG("eval", result);
}

static char	* junk;
static int	junki;

static long
eval1()
{
	register en_tokens op, op2;
	register long val1, val2, val3;
	int op_1ptr;

	TRACETOK("eval1");

	op_1ptr  = op_ptr;

	/* State S1: unary +, unary -, !, ~, constant or id is expected here. */

s1:
	TRACEPN("v_eval1", printf("at state 1\n"));

	while (is(PLUS_TOK) || is(MINUS_TOK) || is(TILDE_TOK) || is(NOT_TOK)) {
		if (is(PLUS_TOK)) {
			push_op(UPLUS_TOK);
		}
		else if (is(MINUS_TOK)) {
			push_op(UMINUS_TOK);
		}
		else if (is(NOT_TOK)) {
			push_op(NOT_TOK);
		}
		else {
			push_op(TILDE_TOK);
		}
		get_token(TRUE);
	}

	/* We expect a constant or identifier here. */
	if (is(INT_TOK) || is(LONG_TOK) || is(CHAR_TOK)) {
		push_val((long) t_value);
		get_token(TRUE);
	}
	else if (is(ID_TOK)) {
		/* Special case defined id and defined(id). */
		if (str_eq(t_symbol, "defined")) {

			/* Do not macro expand an id here! */
			get_token(FALSE);
			if (!is(ID_TOK) && !is(LPAREN_TOK)) {
				error("Id or '(' expected after 'defined'.");
				goto bad_expr;
			}
			paren_seen = is(LPAREN_TOK);
			if (paren_seen) {
				get_token(FALSE);
				if (!is(ID_TOK)) {
					error("Id expected after '('.");
					goto bad_expr;
				}
			}
			if(mst_lookup(t_symbol, &junk, &junki)) {
				push_val(1L);
			}
			else {
				push_val(0L);
			}
			get_token(TRUE);
			if (paren_seen) {
				if (is(RPAREN_TOK)) {
					get_token(TRUE);
				}
				else {
					error("')' expected.");
					goto bad_expr;
				}
			}
		}
		else {
			/* The identifier must be undefined, so it gets 0. */
			push_val(0L);
			get_token(TRUE);
		}
	}
	else if (is(LPAREN_TOK)) {
		get_token(TRUE);

		/* Evaluate the expression recursively. */		
		result = eval1();
		if (is(RPAREN_TOK)) {
			get_token(TRUE);
			push_val(result);
		}
		else {
			error("')' expected.");
			goto bad_expr;
		}
	}		
	else {
		error("Integer constant or parenthesized expression expected.");
		goto bad_expr;
	}

	/* Perform all unary ops and enter state S2. */
	TRACEPN("v_eval1", printf("at state 1A\n"));
	while (op_ptr > op_1ptr) {
		switch (op = pop_op()) {
		case UPLUS_TOK:	 break;
		case UMINUS_TOK: push_val(-pop_val());		break;
		case NOT_TOK:	 push_val((long)(!pop_val()));	break;
		case TILDE_TOK:	 push_val(~pop_val());		break;
		default:	 push_op(op);			goto s2;
		}
	}

	/* State S2: binary op or end_of_expression expected here. */

s2:
	TRACEPN("v_eval1", printf("at state 2\n"));

	/*
		Perform binary operators until the operator stack is
		empty or until token operator has a higher precedence
		than the operator on the top of the operator stack.
	*/

	while (op_ptr > op_1ptr && gt_prec(op_stack[op_ptr - 1], token)) {

		val2 = pop_val();
		val1 = pop_val();
		op   = pop_op();

		switch (op) {
		case PLUS_TOK:		push_val(val1 + val2);	break;
		case MINUS_TOK:		push_val(val1 - val2);	break;
		case STAR_TOK:		push_val(val1 * val2);	break;
		case DIV_TOK:		push_val((long)(val2?(val1/val2):0));
					break;
		case MOD_TOK:		push_val(val1 % val2);	break;
		case AND_TOK:		push_val(val1 & val2);	break;
		case OR_TOK:		push_val(val1 | val2);	break;
		case XOR_TOK:		push_val(val1 ^ val2);	break;
		case LSHIFT_TOK:	push_val(val1 << val2);	break;
		case RSHIFT_TOK:	push_val(val1 >> val2);	break;
		case EQUAL_TOK:		push_val((long)(val1 == val2));	break;
		case NE_TOK:		push_val((long)(val1 != val2));	break;
		case LT_TOK:		push_val((long)(val1 <  val2));	break;
		case GT_TOK:		push_val((long)(val1 >  val2));	break;
		case LE_TOK:		push_val((long)(val1 <= val2));	break;
		case GE_TOK:		push_val((long)(val1 >= val2));	break;
		case LAND_TOK:		push_val((long)(val1 && val2)); break;
		case LOR_TOK:		push_val((long)(val1 || val2)); break;

		case COLON_TOK:		op2 = pop_op();
					if (op2 != QUESTION_TOK) {
						goto bad_expr;
					}
					val3 = pop_val();		
					push_val(val3 ? val1 : val2);
					break;

		default:		goto bad_expr;
		}
	}
		
	/* Enter state S1 or return on end-of-expression. */
	if (is(NULL_TOK) || is(RPAREN_TOK)) {
		val1 = pop_val();
		RETURN_LONG("eval1", val1);
	}
	else if (is(ERR_TOK)) {
		goto bad_expr;
	}
	else {
		push_op(token);
		get_token(TRUE);
		goto s1;
	}

bad_expr:
	if (!error_seen) {
		error("Bad constant expression--zero assumed.");
	}
	error_seen = TRUE;
	RETURN_LONG("eval1", 0L);
}


/*
	Return TRUE if a reduction is possible with op1 on the operator stack 
	and op2 as the new operator.  

	Always return TRUE if op2 delimits the expression, otherwise
	return TRUE if op1 has higher precedence than op2, otherwise
	return TRUE if they associate left to right.

	This code reflects the table on page 49 of K & R.
*/

static bool
gt_prec(op1, op2)
register en_tokens op1;
register en_tokens op2;
{
	register int prec1, prec2;

	TRACEP("gt_prec", printf("(%s, %s)\n",
		pr_op(op1), pr_op(op2)));

	if (op2 == RPAREN_TOK || op2 == NULL_TOK) {
		return TRUE;
	}
	else if (op2 == ERR_TOK) {
		/* Abort immediately. */
		return FALSE;
	}

	prec1 = prec((int)op1);
	prec2 = prec((int)op2);

	if (prec1 != prec2) {
		/* Associativity doesn't matter. */
		return prec1 > prec2;
	}
	else if (prec1 == 14 || prec1 == 3 || prec1 == 2) {
		/* Associate right to left. */
		return FALSE;
	}
	else {
		/* Associate left to right. */
		return TRUE;
	}
}


/*
	Return TRUE if c is legal in a file name.
	Assume that the delim character is not legal.
*/
static bool
isfnch(c, delim)
int	  c;
en_tokens delim;
{
	TICK("isfnch");

	switch (c) {
	case '*':
	case '?':
	case '\n':
	case ' ':
	case '\t':
	case END_FILE:
				return FALSE;

	default:		return (c != (int)delim);
	}
}


/*
	Return the precedence of an operator.

	This code reflects the table on page 49 of K & R.
*/

static int
prec(operator)
int operator;
{
	TRACEP("prec", printf("(%s)\n", pr_op(operator)));

	switch (operator) {
	case TILDE_TOK:		return 14;
	case STAR_TOK:
	case DIV_TOK:
	case MOD_TOK:		return 13;
	case PLUS_TOK:
	case MINUS_TOK:		return 12;
	case LSHIFT_TOK:
	case RSHIFT_TOK:	return 11;
	case LT_TOK:
	case LE_TOK:
	case GT_TOK:
	case GE_TOK:		return 10;
	case EQUAL_TOK:
	case NE_TOK:		return 9;
	case XOR_TOK:		return 7;
	case OR_TOK:		return 6;
	case COLON_TOK:
	case QUESTION_TOK:	return 3;
	default:		return -1;
	}
}


/*
	Routines to push and pop the operator and operand stacks.
*/
static void
push_op(op)
en_tokens op;
{
	TRACEP("push_op", printf("(%s), op_ptr = %d\n",
		pr_op(op), op_ptr));

	if (op_ptr < MAX_EVAL_OP) {
		op_stack[op_ptr++] = op;
	}
	else {
		fatal("Operator stack overflow.");
	}
}

static en_tokens
pop_op()
{
	TRACEP("pop_op", printf("op_ptr = %d\n", op_ptr));

	if (op_ptr > 0) {

		TRACEP("pop_op", printf("returns: %s\n",
			pr_op(op_stack[op_ptr - 1])));
		
		return op_stack [--op_ptr];
	}
	else {
		syserr("Operator stack underflow.");
		return NULL_TOK;
	}
}

static void
push_val(val)
long val;
{
	TRACEP("push_val", printf("(%ld), val_ptr = %d\n", val, val_ptr));

	if (val_ptr < MAX_EVAL_VAL) {
		val_stack [val_ptr++] = val;
	}
	else {
		fatal("Value stack overflow.");
	}
}

static long
pop_val()
{
	TRACEP("pop_val", printf("val_ptr = %d\n", val_ptr));

	if (val_ptr > 0) {

		TRACEP("pop_val", printf("returns %ld\n",
			val_stack [val_ptr - 1]));

		return val_stack [--val_ptr];
	}
	else {
		syserr("Value stack underflow.");
		return 0L;
	}
}

/*
	Handle the #elif directive when lines are NOT being skipped.
	The skip_lines() routines handles #elif when lines ARE being skipped.
*/
static void
pp_elif()
{
	TRACE_LINE("pp_elif");

	if (t_iflevel == 0) {
		error("#elif ignored--no matching #if.");
		skip_pp();
		return;
	}

	/* Skip lines if the constant expression evaluates to zero. */
	if (eval() == 0L) {
		/* Skip until #else or #endif. */
		skip_lines();
	}
}


/*
	Handle the #else directive when lines are NOT being skipped.
	The skip_lines() routine handles #else when lines ARE being skipped.	
*/
static void
pp_else()
{
	TRACE_LINE("pp_else");

	if (t_iflevel == 0) {
		error("#else ignored--no matching #if.");
		skip_pp();
	}
	else if (t_ifstack [t_iflevel - 1] == TRUE) {
		error("Duplicate #else ignored.");
		skip_pp();
	}
	else {
		t_ifstack [t_iflevel - 1] = TRUE;
		skip_lines();
	}
}


/*
	Handle the #endif directive when lines are NOT being skipped.
	The skip_lines() routine handles #endif when lines ARE being skipped.
*/
static void
pp_endif()
{
	TRACE_LINE("pp_endif");

	if (t_iflevel == 0) {
		error("#endif ignored--no matching #if.");
		skip_pp();
	}
	else {
		t_iflevel--;
		skip_pp();
	}
}
			
/*
	Handle the #if directive when lines are NOT being skipped.
	The skip_lines() routine handles #if when lines ARE being skipped.
*/
static void
pp_if()
{
	TRACE_LINE("pp_if");

	/* Indicate that no #else has been seen. */
	if (t_iflevel >= MAX_IF) {
		error("#if ignored--nested too deeply.");
		skip_pp();
		return;
	}
	t_ifstack [t_iflevel++] = FALSE;

	/* Skip lines if the constant expression evaluates to zero. */
	if (eval() == 0L) {
		/* Skip until #else or #endif. */
		skip_lines();
	}
}

/*
	Handle the #ifdef and #ifndef directives when lines are NOT being 
	skipped.
	The skip_lines() routine handles #if when lines ARE being skipped.

		flag == TRUE:	#ifdef
		flag == FALSE:	#ifndef
*/
static void
pp_ifdef(flag)
bool flag;
{
	bool result;
	char *text;
	int nargs;

	TRACE_LINE("pp_ifdef");

	if(t_iflevel > MAX_IF) {
		error("#ifdef or #ifndef ignored--nested too deeply.");
		skip_pp();
		return;
	}

	if (!isid1(ch)) {
		error("#ifdef or #ifndef ignored--identifier expected.");
		skip_pp();
		return;
	}
	
	/* Get id into t_symbol[]. */
	t_id(t_symbol, MAX_SYMBOL);
	skip_pp();
	
	/* Indicate that no #else has been seen. */
	t_ifstack [t_iflevel++] = FALSE;

	/* Skip lines if required. */
	result = mst_lookup(t_symbol, &text, &nargs);
	if ((flag && result == FALSE) || (!flag && result == TRUE)) {
		/* Skip until #else or #endif. */
		skip_lines();
	}
}


/*
	Handle the #include directive.
*/
static void
pp_incl()
{
	bool	old_mflag;
	bool	err_flag;
	int	i;
	char	f_name [MAX_FILE_NAME];
	char	path_name [200];
	char	delim;

	TRACE_LINE("pp_incl");

	/* File name is OK. */
	err_flag = FALSE;

	/*
		Check for opening delimiter.
		Allow simple macro expansions here.
	*/
	if (isid1(ch)) {
		old_mflag = m_flag;
		t_id(&t_symbol[0], MAX_SYMBOL);

		/* We expect a proper macro here. */
		outer_expand(&t_symbol[0], old_mflag);
	}

	if (ch == '"') {
		delim = '"';
		sysnext();
	}
	else if (ch == '<') {
		delim = '>';
		sysnext();
	}
	else {
		err_flag = TRUE;
		delim = '\r';
	}

	/* Get file name into f_name[]. */
	for (i = 0; i < MAX_FILE_NAME-1 && isfnch(ch,(en_tokens)delim); i++) {
		f_name[i] = ch;
		sysnext();
	}
	f_name[i] = '\0';

	if (err_flag || ch != delim) {
		err3("#include ", f_name, " ignored--bad delimiter.");
		skip_pp();
		return;
	}

	/* Skip over the delimiter. */
	sysnext();

	/*
		We must skip the line here, because after
		sysopen() is called we would not be able to skip
		characters in the old level.
	*/
		
	skip_pp();
	if (ch == '\n') {
		sysnlput();
		sysnext();
	}
	bump_line();

	if (t_inlevel >= MAX_INCLUDE) {
		t_line--;
		err3("#include ", f_name, " ignored--nested too deeply.");
		t_line++;
		return;
	}

	/* Open the file. */
	if(sysopen(f_name) == FALSE) {
		for (i = 0; i < n_paths; i++) {
			strcpy(path_name, paths [i]);
			strcat(path_name, f_name);

			TRACEP("pp_incl", printf("attempting to open %s\n",
				path_name));

			if (sysopen(path_name) == TRUE) {
				return;
			}
		}
		t_line--;
		err3("#include ", f_name, " ignored--file not found.");
		t_line++;
	}
}


/*
	Set the line number and file name.
*/
static void
pp_line()
{
	bool	old_mflag;
	int	i;

	TRACE_LINE("pp_line");

	/* Allow macro expansion. */
	if (isid1(ch)) {
		old_mflag = m_flag;
		t_id(&t_symbol[0], MAX_SYMBOL);

		/* We expect a macro id here. */
		outer_expand(&t_symbol[0], old_mflag);
	}

	(void) t_number(FALSE);
	t_line = (int) t_value;

	/* See if an optional file name is present. */
	skip_ws(FALSE);

	/* Allow macro expansion. */
	if (isid1(ch)) {
		old_mflag = m_flag;
		t_id(&t_symbol[0], MAX_SYMBOL);

		/* We expect a macro id here. */
		outer_expand(&t_symbol[0], old_mflag);
	}
	if (ch == '\n' || ch == END_FILE) {
		return;
	}

	/* Copy line to t_file[]. */
	for(i = 0; i < MAX_FILE_NAME - 1; i++) {
		if (ch == END_FILE || ch == '\n') {
			break;
		}
		t_file[i] = ch;
		sysnext();
	}
	t_file [i] = '\0';
	skip_pp();
}


/*
	Undefine a previously #defined variable.
*/
static void
pp_undef()
{
	TRACE_LINE("pp_undef");

	/* Get the identifier into t_symbol[]. */
	if (!isid1(ch)) {
		error("#undef ignored--identifier expected.");
		skip_pp();
		return;
	}
	t_id(t_symbol, MAX_SYMBOL);

	/* Delete the identifier. Give no warning if it doesn't exist. */
	mst_delete(t_symbol);
	skip_pp();
}


/*
	Skip lines until a matching #else or #endif directive is found.
	Thus,  interior #ifdef, #ifndef, #if, #elif and  #else directives
	must be recognized and dealt with.

	A fine point:  This code skips lines without doing any parsing of the
	line.  The ONLY things this code looks for are lines which start with
	#ifdef, #ifndef, #if or #else.  One should not detect "unknown"
	preprocessor directives or other possible "errors," for the very good
	reason that one wants to use conditional compilation to exclude lines
	which might not even be written in the C language.

	Examples can be given where this action might be suspect.  Consider a
	string which is continued from one line to the next using the backslash
	newline convention.  If the continuation line starts with one of the
	four directives that this routine is looking for then either the lex or
	the person using the lex will become confused.  This is a minor hole in
	the C language, and it seems better to ignore the fine points and do
	what is sensible in most cases, namely skip entire lines as a unit.

	The routine skip_past() is used to do the skipping of exactly 1 line.
*/

#define EQL(string) str_eq(t_symbol,string)
static void
skip_lines()
{
	register int level = 0;		/* Inner nesting level */

	TRACE_LINE("skip_lines");

	/* Just in case. */
	if (t_iflevel <= 0) {
		syserr("skip_lines: Can't happen.");
	}

	/* Start a new line. */
loop:

	if (ch == END_FILE) {
		error("File ends inside range of #ifdef.");
		return;
	}

	/* Skip the line if it doesn't start with '#'. */
	if (ch != '#') {
		skip_past();
		goto loop;
	}

	/* Skip the line if '#' isn't followed by an id. */
	sysnext();
	skip_ws(FALSE);

	if (!isid1(ch)) {
		skip_past();
		goto loop;
	}

	/* Get the directive into t_symbol[]. */
	t_id(t_symbol, MAX_SYMBOL);

	if (EQL("ifdef") || EQL("ifndef") || EQL("if")) {
		level++;
	}
	else if (EQL("elif")) {
		if (level > 0) {
			/* This directive will itself be skipped. */
			skip_pp();
		}
		else if (eval() == 0L) {
			/* Call to eval will bump line number. */
			TRACEPN("skip_lines",
				printf("stopped by #elif at line %d\n",
					t_line - 1));
			return;
		}
	}
	else if (EQL("else")) {
		if (level > 0) {
			/* Skip this directive. */
			skip_pp();
		}
		else if (t_ifstack [t_iflevel - 1] == FALSE) {
			t_ifstack [t_iflevel - 1] = TRUE;
			skip_pp();
			TRACEPN("skip_lines",
				printf("stopped by #else at line %d\n",
					t_line));
			return;
		}
		else {
			error("Extra #else ignored.");
		}
	}
	else if (EQL("endif")) {
		if (level > 0) {
			level--;
		}
		else {
			t_iflevel--;
			skip_pp();
			TRACEPN("skip_lines",
				printf("stopped by #endif at line %d\n",
					t_line));
			return;
		}
	}
	else {
		skip_past();
	}
	goto loop;
}

#undef EQL
