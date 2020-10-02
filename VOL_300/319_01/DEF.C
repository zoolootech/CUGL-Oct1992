/*
	CPP V5 -- definition and expansion routines.

	source:  def.c
	started: October 22, 1985
	version:
		July 21, 1988
		February 14, 1989:
			bug fixes in pp_def().
			bug fix in substitute().
		August 2, 1989:
			support for C++ single-line comment added to pp_def().
			overflow checks made in substitute().
			Big buffers allocated on heap in expand(), rescan().

	Written by Edward K. Ream.
	This software is in the public domain.

	See the read.me file for disclaimer and other information.
*/

#include "cpp.h"

/*
	Forward declarations of internal routines.
*/

static void	aexpand		(char *name, int *nargs, char **p1, char *out,
					char **p2);
static void	aparse		(char *name, int *nargs, char *out, char **p);
static bool	a1parse		(char *name, char *out, int max_out);
static void	disable_name	(char *name);
static void	enable_name	(void);
static bool	is_disabled	(char *name);
static void	expand		(char *name, int nargs, char *rtext, char *out,
					int max_out);
static void	rescan		(char *name, char *in, char *out, int max_out,
					bool bounded_flag);
static void	substitute	(char *name, int nargs, char *in, char **args,
					char *out, int maxout);

/*
	Define static variables used only by this module.
*/
static char *	x_stack [MAX_MDEPTH];	/* Pointers to disabled macro names. */
static int	x_count = 0;		/* Number of disabled macro names.   */

/*
	Externally visible routines.
*/

/*
	Check an identifier seen at the outer level to see whether it
	could be a macro call and expand it and return TRUE if so.
*/
bool
outer_expand(name, old_mflag)
char	*name;
bool	old_mflag;
{
	char 	*p, *p1, *p2;
	int	i;
	char *	rtext;			/* Replacement text for name.	   */
	int	nargs;			/* Required number of args.	   */
	char	temp_buf[MAX_RTEXT];	/* Temp spot for macro.		   */	

	TRACEPB("outer_expand", printf("(%s)\n", name));

	if (m_flag || old_mflag || !mst_lookup(name, &rtext, &nargs)) {
		RETURN_BOOL("outer_expand", FALSE);
	}
	else if (nargs != -1) {
		skip_ws(TRUE);
		if (ch != '(') {
			warn3("Function-like macro, ", name,
				"appears without arguments.");
			RETURN_BOOL("outer_expand", FALSE);
		}
	}

	/*
		Expand the macro into temp_buf[].
		This will use ONLY file characters.
	*/
	expand(name, nargs, rtext, &temp_buf[0], MAX_RTEXT);

	/*
		Expand will have changed ch to point past any args.
		Use sys_fpb so that
		1.  The old ch will appear just AFTER the macro expansion.
		2.  It will not be mistaken added to the rescan buffer.
	*/
	sys_fpb(ch);

	/*
		Fully rescan temp_buf[] into macro_buf[].
		Use an unbounded rescan, i.e., allow file characters to
		complete a partial list of actual parameters which may
		be started in the rescanned text.
	*/
	p_rescan = NULL;
	m_flag   = FALSE;
	rescan(name, &temp_buf[0], &macro_buf[0], MAX_RTEXT, FALSE);

	/* Make sure we save the now current ch. */
	sys_fpb(ch);

	/* Delete any internal flags from the output. */
	TRACEP("outer_expand", printf("macro1: %s\n", pr_str(&macro_buf[0])));

	p1 = p2 = &macro_buf[0];
	while (*p2) {
		if (*p2 == EXPAND_OFF || *p2 == EORT) {
			p2++;
		}
		else {
			*p1++ = *p2++;
		}
	}
	*p1 = '\0';
	TRACEP("outer_expand", printf("macro2: %s\n", pr_str(&macro_buf[0])));

	/*
		Inhibit any further macro expansion until the end of the
		current local input stream.
	*/

	if (m_flag) {
		warning("outer_expand: Can't happen.");
	}
	m_flag = TRUE;
	p_rescan = &macro_buf[0];

	/* Reload ch from the global macro buffer. */
	sysnext();

	RETURN_BOOL("outer_expand", TRUE);
}

/*
	Handle the #define directive by parsing the statement, and entering
	the macro's name, number of arguments and replacement text into the
	macro table.

	Formal arguments are found and replaced by a flag byte followed by the
	number of the formal argument.
*/
void
pp_def()
{
	int	i;

	char	name[MAX_SYMBOL];	/* Name of macro.		*/

	char	rtext [MAX_RTEXT];	/* Replacement text.		*/
	int	rcount = 0;		/* Number of chars in rtext[].	*/

	char	atext [MAX_ATEXT];	/* Text of formal parameters.	*/
	int	acount = 0;		/* Number of chars in atext[].	*/

	char *	argps [MAX_NARGS];	/* Pointers into atext[].	 */
	int	nargs = 0;		/* Number of entries in argps[]. */

	TICKB("pp_def");

	/* Make sure the name is present. */
	if (!isid1(ch)) {
		error("#define ignored--no symbol name given.");
		skip_1line();
		return;
	}

	/* Get the macro name into name. */
	t_id(&name[0], MAX_SYMBOL);
	TRACEPN("pp_def", printf("macro name is %s\n", name));
	
	if (ch != '(') {
		/* Indicate no argument list and continue. */
		nargs = -1;
		goto gettext;
	}
	else {
		sysnext();
	}

	if (ch == ')') {
		sysnext();
		nargs = 0;
		goto gettext;
	}

	/* 
		Put the formal arguments into dynamic storage.
		Set pointers to the arguments argps[].
	*/
	while (nargs < MAX_NARGS) {

		skip_ws(TRUE);

		if (!isid1(ch)) {
			err2(	"#define ignored--",
				"formal arg must be an identifier.");
			skip_1line();
			RETURN_VOID("pp_def");
		}

		/* Copy one formal arg to atext[]. */
		t_id(&t_symbol[0], MAX_SYMBOL);
		argps [nargs++] = &atext[acount];
		str_cpy(atext + acount, t_symbol);
		acount += strlen(t_symbol)+1;

		TRACEPN("pp_def", printf("formal [%d] is %s\n",
			nargs-1, pr_str(argps [nargs-1])));

		/* Bug fix: 2/14/89 */
		skip_ws(TRUE);
			
		if (ch == ')') {
			sysnext();
			goto gettext;
		}
		else if (ch == ',') {
			sysnext();
		}
	}
	error("#define ignored--too many arguments.");
	skip_1line();
	RETURN_VOID("pp_def");


	/*
		At this point, nargs contains the number of formal arguments,
		or -1 if no argument list was given.
		0 is allowed and means that the argument list was ().
	*/

gettext:

	TRACEPN("pp_def", printf("nargs is %d\n", nargs));

	skip_bl();

	/*
		Put the replacement text into rtext[].
		The text is tokenized, which means that partial comments
		or strings are not valid.

		Replace formal arg n by n FLAG on the fly.
	*/

	for (rcount = 0;;) {
		switch (ch) {

		case END_FILE:
		case '\n':
			goto done;

		/* Replace a sequence of white space by a blank. */
		case ' ':
		case '\t':
			sysnext();
			if (rcount > 0 && rtext[rcount-1] != ' ') {
				rtext[rcount++] = ' ';
			}
			continue;

		/* Look for continuation of definitions. */
		case '\\':
			sysnext();
			if (ch == '\n') {
				sysnlput();
				sysnext();
				if (rcount > 0 && rtext[rcount-1] != ' ') {
					rtext[rcount++] = ' ';
				}
				bump_line();
			}
			else {
				warning("Backslash not followed by newline.");
				rtext[rcount++] = '\\';
			}
			continue;

		/* Eliminate comments. */
		case '/':
			sysnext();
			if (ch == '*') {
				if (rcount > 0 && rtext[rcount-1] != ' ') {
					rtext[rcount++] = ' ';
				}
				sysnext();
				skip_comment();
			}
			else if (slc_flag && ch == '/') {
				/* 8/1/89 C++ style single-line comment. */
				if (rcount > 0 && rtext[rcount-1] != ' ') {
					rtext[rcount++] = ' ';
				}
				while (ch != END_FILE && ch != '\n') {
					sysnext();
				}
				goto done;
			}
			else {
				rtext[rcount++] = '/';
			}
			continue;

		/* Handle complete strings. */
		case '"':
		case '\'':
			t_string(&rtext[rcount], MAX_RTEXT-rcount, TRUE);
			rcount += t_length;
			continue;

		/* Handle # formal param and ## */
		case '#':
			sysnext();
			if (ch == '#') {
				sysnext();
				rtext[rcount++] = CONCAT_FLAG;
				continue;
			}

			/* We expect an identifier here. */

			/* Bug fix 2/14/89:  delete leading white space here. */
			if (ch == ' ' || ch == '\t') {
				if (rcount > 0) {
					rtext[rcount++] = ' ';
				}
				skip_bl();
			}
			if (isid1(ch)) {
				t_id(&rtext[rcount], MAX_RTEXT-rcount);
				i = arg_search( rtext + rcount, nargs,
						&argps[0]);
				if (i >= 0) {
					/* Replace id with flags. */
					rtext[rcount++] = POUND_FLAG;
					rtext[rcount++] = i + ARG_OFFSET;
				}
				else {
					/* Accept id. */
					rcount += t_length;
				}
			}
			else {
				/* Replace the blank with # */
				rtext[rcount++] = '#';
				warning("Id expected after '#'.");
			}
			continue;

		default:

			/* Possible formal parameter. */
			if (isid1(ch)) {
				t_id(&rtext[rcount], MAX_RTEXT-rcount);
				i = arg_search(	rtext + rcount, nargs,
						&argps[0]);
				if (i >= 0) {
					rtext[rcount++] = ARG_FLAG;
					rtext[rcount++] = i+ARG_OFFSET;
				}
				else {
					rcount += t_length;
				}
			}
			else {
				rtext[rcount++] = ch;
				sysnext();
			}
			continue;
		}
	}

done:

	/* This check is made only here to save a little time. */
	if (rcount >= MAX_RTEXT) {
		fatal("Replacement text of macro is too long.");
	}

	/* Strip at most one blank off the end of the definition. */
	if (rtext[rcount - 1] == ' ') {
		rcount--;
	}

	/* Terminate the replacement text properly. */
	rtext[rcount] = '\0';

	TRACEPN("pp_def", printf("rtext: <%s>\n", pr_str(rtext)));

	/*
		Enter the symbol, replacement text and number of arguments
		into the macro table.

		Note that mst_enter allocates space for copies of name and
		rtext so the fact that they are on the stack doesn't matter.
	*/
	mst_enter(&name[0], rtext, nargs);
	skip_pp();

	RETURN_VOID("pp_def");
}


/*
	Internal Routines.
*/

/*
	Macro expand all arguments in old_argps and leave them in new_atext[].
	Put pointers to them in new_argps[].
*/
static void
aexpand(name, nargs, old_argps, new_atext, new_argps)
char *	name;		/* Name of macro whose args are being expanded.	*/
int *	nargs;		/* Number of args the macro should have.	*/
char *	old_argps[];	/* Pointers to unexpanded tokens for each arg.	*/
char 	new_atext[];	/* Storage for expanded args.			*/
char *	new_argps[];	/* Pointers to expanded args.			*/
{
	char	save_ch;
	int	i;
	char *	new;
	int	new_count = 0;

	TRACEPB("aexpand", printf("(%s, %d, %p, %p, %p)\n",
		name, *nargs, old_argps, new_atext, new_argps));

	/*
		Save the current value of ch!
		rescan will change ch, and it must be restored afterwards.
	*/
	save_ch = ch;

	/* Rescan all arguments. */
	for (i = 0; i < *nargs; i++) {
		new = &new_atext[new_count];
		new_argps[i] = new;

		/* Do a bounded rescan. */
		rescan(	"<NO_NAME>", old_argps[i], new,
			MAX_RTEXT-new_count, TRUE);

		new_count += strlen(new)+1;
	}

	/* Restore ch */
	ch = save_ch;

	RETURN_VOID("aexpand");
}

/*
	Parse a list of actual arguments into atext[] and put pointers to
	the arguments in argps[].  
	Set *nargs to -1 on error.
	The size of atext[] is MAX_ATEXT and the size of argps is MAX_ARGPS.
*/
static int call_start;		/* Line where arguments started.	*/

static void
aparse(name, nargs, atext, argps)
char *	name;		/* Macro whose arguments are being parsed.	   */
int *	nargs;		/* Pointer to arg count.			   */
char 	atext[];	/* Buffer into which UNEXPANDED arguments are put. */
char *	argps[];	/* Array of pointers to arguments.		   */
{
	int	arg_c = 0;	/* Number of arguments parsed so far.	*/
	int	length = 0;	/* Total length of arguemnts parsed.	*/
	char	msg [100];
	char	line [10];
	bool	flag;

	if (*nargs == -1) {
		return;
	}

	TRACEPB("aparse", printf("(%s, %d, %p, %p)\n",
		name, *nargs, atext, argps));

	if (ch != '(') {
		syserr("aparse: Can't happen.");
	}
	else {
		sysnext();
	}

	/* Save starting line number of the macro call. */
	call_start = t_line;

	if (ch == ')') {
		sysnext();
		goto check;
	}

	for(;;) {
		/* Set pointer to the start of the argument. */
		argps[arg_c] = &atext[length];

		/* Parse the argument. */
		flag = a1parse(name, &atext[length], MAX_ATEXT-length);
		if (flag || ch == END_FILE) {
			*nargs = -1;
			RETURN_VOID("aparse");
		}
		else {
			length += strlen(argps[arg_c])+1;
			arg_c++;
		}

		if (ch == ')') {
			sysnext();
			break;
		}
		else if (ch == ',') {
			sysnext();
		}
		else {
			/* Error detected in a1parse. */
			break;
		}
	}

check:
	if (arg_c != *nargs) {
		if (call_start != t_line) {
			strcpy(msg, "Call to macro ");
			strcat(msg, name);
			strcat(msg, " starting at line ");
			conv2s(call_start, line);
			strcat(msg, line);
		}
		else {
			strcpy(msg, "Call to macro ");
			strcat(msg, name);
		}
		strcat(msg, " requires ");
		conv2s(*nargs, line);
		strcat(msg, line);
		strcat(msg, " arguments.");
		error(msg);
		*nargs = -1;
	}
	TRACEP("aparse",
		{int i;
		if (*nargs != 0) printf("\n");
		for(i = 0; i < *nargs; i++) {
			printf("arg[%d]: @%p=%p: <%s>\n",
				i, &argps[i], argps[i], pr_str(argps[i]));
		}});
	RETURN_VOID("aparse");
}

/*
	Parse one actual argument into out[].
	The size of out is max_length.
	Return TRUE if an error was seen.

	Formal arguments are simply identifiers, which are handled by t_id(),
	but actual arguments are LISTS of tokens separated by commas.  As an
	added twist, commas inside single or double quotes, or commas which are
	"protected" by additional parentheses do NOT separate actual args.
	Thus, each of the following calls have to M have ONE actual argument:

		M(a)
		M(a * N(c,b))
		M(',')
		M("a,b")
		M((a,b))
		M((a,")",b))

	This routine changes all comments and white space to a single blank.
*/
static bool
a1parse(name, out, max_out)
char *	name;			/* Name of the macro being expanded.	*/
char *	out;			/* Output buffer.			*/
int	max_out;		/* Size of out[].			*/
{
 	int count;	/* Number of characters in out[].	*/
	int plevel;	/* Parenthesis level.			*/

	char buffer [100];	/* Buffer for messages.		*/
	char linebuf [10];	/* Buffer for messages.		*/

	TRACEPB("a1parse", printf("(%s, %p, %d)\n",
		name, out, max_out));

	/* No parens have been seen yet. */
	plevel = 0;

	for(count = 0;;) {

		/* Make sure there is room for one more. */
		if (count >= max_out) {
			goto toolong;
		}

		switch (ch) {

		case END_FILE:
			goto runon;

		case '\n':
			sysnlput();
			sysnext();
			bump_line();
			if (count > 0 && out[count-1] != ' ') {
				out[count++] = ' ';
			}
			continue;

		/* Convert a sequence of white space to one blank. */
		case ' ':
		case '\t':
			sysnext();
			if (count > 0 && out[count-1] != ' ') {
				out[count++] = ' ';
			}
			continue;

		case '\\':
			sysnext();
			if (ch == '\n') {
				sysnlput();
				sysnext();
				if (count > 0 && out[count-1] != ' ') {
					out[count++] = ' ';
				}
				bump_line();
			}
			else {
				out[count++] = '\\';
			}
			continue;

		case ',':
			if (plevel == 0) {
				goto end_arg;
			}
			else {
				out[count++] = ch;
				sysnext();
			}
			continue;
				
		case ')':

			if (plevel == 0) {
				goto end_arg;
			}
			else {
				plevel--;
				out[count++] = ch;
				sysnext();
				continue;
			}

		case '(':
			plevel++;
			out[count++] = ch;
			sysnext();
			continue;

		case '"':
		case '\'':
			t_string(&out[count], max_out-count, TRUE);
			count += t_length;
			if (count >= max_out) {
				goto toolong;
			}
			continue;

		case '/':
			sysnext();
			if (ch == '*') {
				sysnext();
				skip_comment();

				/* Change a comment into one blank. */
				if (count > 0 && out[count] != ' ') {
					out[count++] = ' ';
				}
			}
			else {
				out[count++] = '/';
			}
			continue;

		default:
			if (isid1(ch)) {
				t_id(&out[count], max_out-count);
				count += t_length;
				if (count >= max_out) {
					goto toolong;
				}
			}
			else {
				out[count++] = ch;
				sysnext();
			}
		}
	}

end_arg:
	/* Finish off the argument. */
	out[count] = '\0';
	TRACEP("a1parse", printf("out: <%s>\n", pr_str(out)));
	RETURN_BOOL("a1parse", FALSE);

runon:
	if (call_start != t_line) {
		conv2s(call_start, linebuf);
		str_cpy(buffer, "Runon macro call at line ");
		str_cat(buffer, linebuf);
	}
	else {
		str_cpy(buffer, "Runon macro call");
	}
	str_cat(buffer, "--last arg set to null.");
	error(buffer);
	RETURN_BOOL("a1parse", TRUE);

toolong:
	conv2s(call_start, linebuf);
	str_cpy(buffer, "Macro arg starting at line ");
	str_cat(buffer, linebuf);
	str_cat(buffer, " is too long.");
	fatal(buffer);
	RETURN_BOOL("a1parse", TRUE);
}

/*
	Search an array of formal arguments for a match.
*/
static int
arg_search(name, nargs, argps)
char *	name;	/* Name to search for.	*/
int	nargs;	/* Number of args.	*/
char **	argps;	/* Array of args.	*/
{
	int i;

	/* See if the id is a formal arg. */
	for (i = 0; i < nargs ; i++) {
		if (str_eq(name, argps[i])) {
			return i;
		}
	}
	return -1;
}

/*
	Disable the expansion of one name.
*/
static void
disable_name(name)
char *name;
{
	TRACEP("disable_name", printf("(%s)\n", name));

	if (x_count < MAX_MDEPTH) {
		x_stack [x_count++] = name;
	}
}

/*
	Enable the expansion of the last disabled name.
*/
static void
enable_name()
{
	TICK("enable_name");

	if (x_count == 0) {
		syserr("enable_name: Can't happen.");
	}
	x_count--;
}

/*
	Fully expand the macro described by name, nargs, rtext.
*/
static void
expand(name, nargs, rtext, out, max_out)
char *	name;		/* Name of macro expanded.	*/
int	nargs;		/* Required number of args.	*/
char *	rtext;		/* Replacement text for name.	*/
char *	out;		/* Output buffer.		*/
int	max_out;	/* Size of out[].		*/
{
	char *  old_argps[MAX_NARGS];
	char *  new_argps[MAX_NARGS];

	char *	old_atext;	/* Buffer for actual arguments.	   */
	char *	new_atext;	/* Buffer for expansion of args.   */	

	TRACEPB("expand", printf("(%s, %d, <%s>, %p, %d) ch: %s\n",
		name, nargs, pr_str(rtext), out, max_out, pr_ch(ch)));

	/*
		Allocate memory on the heap so we don't crash the stack.
	*/
	old_atext = m_alloc(MAX_ATEXT);
	new_atext = m_alloc(MAX_ATEXT);

	/* Parse all arguments to the macro. */
	aparse(name, &nargs, old_atext, old_argps);

	/* Fully macro expand all arguments. */
	aexpand(name, &nargs, &old_argps[0], &new_atext[0], &new_argps[0]);

	/* Substitute all expanded arguments into rtext[], giving out[]. */
	substitute(name, nargs, rtext, &new_argps[0], &out[0], max_out);

	/* Free all locally allocated memory. */
	m_free(old_atext);
	m_free(new_atext);

	RETURN_VOID("expand");
}

/*
	Return TRUE if macro expansion of name has been disabled.
*/
static bool
is_disabled(name)
char *name;
{
	int i;

	TRACEPB("is_disabled", printf("(%s), x_count: %d\n",
		name, x_count));

	for (i = x_count-1; i >= 0; i--) {
		TRACEP("is_disabled_v", printf("compare: <%s> <%s>\n",
			name, x_stack[i]));
		if (str_eq(name, x_stack[i])) {
			RETURN_BOOL("is_disabled", TRUE);
		}
	}
	RETURN_BOOL("is_disabled",FALSE);
}

/*
	This is it: the guts of macro expansion.
	Fully rescan in[] to out[], looking for macro calls.

	A key distinction:  bounded versus unbounded rescanning.
	Set the global b_rescan to TRUE for bounded rescans.  
	sysnext() returns EORT when p_rescan is empty on bounded rescans.

	The CALLER of rescan must save and restore ch as appropriate.
	(Note that rescan is called recursively from within, so it must
	save ch in that case just as any other caller must.

	Note that rescan will completely process rescan buffer in all cases.
	For unbounded rescans, the previous rescan buffer is appended to in[].
	For bounded rescans, the previous rescan buffer is kept separate.
*/
static void
rescan(name, in, out, max_out, bounded_flag)
char *	name;		/* Name of macro being rescanned.	*/
char *	in;		/* Text to be rescanned.		*/
char *	out;		/* Output buffer.			*/
int	max_out;	/* Size of out[].			*/
bool	bounded_flag;	/* TRUE if bounded rescan.		*/
{
	int	save_mflag;		/* Saved m_flag.	*/
	char *	save_prescan;		/* Saved p_rescan.	*/

	char *	local_in;	/* Local rescan buffer.	*/
	char *	expand_buf;	/* Expansion buffer.	*/
	char *	id_buf;		/* Saved rescan id.	*/

	int	out_len = 0;		/* Index into out[].	*/
	char *	rtext;			/* Ptr to replacement text.	*/
	int	nargs;			/* Required number of args.	*/
	int	i, level;

	TRACEPB("rescan", printf("(%s, <%s>, %p, %d, %s) ch: %s\n",
		name, pr_str(in), out, max_out, pr_bool(bounded_flag),
		pr_ch(ch)));

	/*
		Allocate buffers here so we don't crash the stack.
	*/
	local_in   = m_alloc(MAX_RTEXT);
	expand_buf = m_alloc(MAX_RTEXT);
	id_buf     = m_alloc(MAX_SYMBOL);

	if (bounded_flag) {
		/* Save old rescan buffer and use in[] as the new buffer. */
		save_mflag   = m_flag;
		save_prescan = p_rescan;

		/*
			Indicate the end of the buffer with EORT flag.
			Copy in[] to local_in[] so we have room for it.
		*/
		p_rescan = &local_in[0];
		strcpy(&local_in[0], in);
		i = strlen(&local_in[0]);
		local_in[i++] = EORT;
		local_in[i]   = '\0';
	}
	else {
		/*
			The key programming trick in this whole file.
			Set rescan to old rescan appended behind in[].
		*/
		if (!m_flag) {
			p_rescan = in;
		}
		else {
			if (strlen(in) + strlen(p_rescan) >= max_out) {
				goto run_on;
			}
			str_cpy(local_in, in);
			str_cat(&local_in[0], p_rescan);
			p_rescan = &local_in[0];
		}
	}

	/* Get input from rescan buffer. */
	m_flag = TRUE;

	TRACEP("rescan",
		printf("p_rescan: strlen: %d, <%s>\n",
			strlen(p_rescan), pr_str(p_rescan)));

	/*
		Note1:	At this point, sysnext() will return the characters
		that have just been placed in the rescan buffer pointed to by
		p_rescan.  This means that t_id(), t_string(), etc.  may be
		used to parse the rescan buffer.

		Note2:  For bounded rescans, sysnext() will return EORT when
		the rescan buffer is exhausted.  This means that t_string() and
		skip_comment() will complain about "unexpected end of input," 
		which is as it should be.
	*/

	/* Disable further expansions of name for the duration. */
	disable_name(name);

	/*
		Scan the buffer until it is exhausted or a macro call is seen.
		(The recursive call to rescan() will finish off the buffer.)
	*/
	out_len = 0;
	sysnext();
	while (m_flag) {

		if (out_len >= max_out) {
			goto run_on;
		}

		if (ch == EORT) {
			break;
		}
		else if (ch == EXPAND_OFF) {
			/* Inhibited identifier. */
			out[out_len++] = ch;
			sysnext();
			t_id(&out[out_len], max_out-out_len);
			out_len += strlen(&out[out_len]);
		}
		else if (ch == '"' || ch == '\'') {
			/* Handle complete strings. */
			t_string(&out[out_len], max_out-out_len, TRUE);
			out_len += t_length;
		}
		else if (isid1(ch)) {
			/* Handle identifiers. */
			t_id(&id_buf[0], MAX_SYMBOL);

			/*
				Possible inner macro call.
				Do not adjust out_len until we know for sure.
			*/
			if (!mst_lookup(&id_buf[0], &rtext, &nargs)) {
				/* Not a macro call. */
				str_cpy(&out[out_len], &id_buf[0]);
				out_len += t_length;
				continue;
			}
			else if (is_disabled(&id_buf[0])) {
				/*
					Disabled macro.
					Inhibit ALL further expansion.
				*/
				out[out_len++] = EXPAND_OFF;
				str_cpy(&out[out_len], &id_buf[0]);
				out_len += t_length;
				continue;
			}
			else if (nargs > -1) {
				skip_ws(TRUE);
				/*
					Function-like macro name without args
					are ok in bounded expansions.
				*/
				if (ch != '(') {
					if (ch != EORT && !bounded_flag) {
					   warn3("Function-like macro ",
						 &id_buf[0],
						 " appears without arguments.");
					}
					str_cpy(&out[out_len], &id_buf[0]);
					out_len += t_length;
					continue;
				}
			}

			/* Valid macro call. */
			if (out_len >= max_out) {
				goto run_on;
			}

			out[out_len] = '\0';

			/* Expand the text into expand_buf[]. */
			expand(	&id_buf[0], nargs, rtext,
				&expand_buf[0], MAX_RTEXT);

			/*
				Do not pull an extra file character into
				the buffer.
			*/
			if (m_flag) {
				/* Append current ch to expand_buf[]. */
				i = strlen(&expand_buf[0]);
				expand_buf[i++] = ch;
				expand_buf[i] = '\0';
			}
			else {
				sys_fpb(ch);
			}

			/* Rescan will append the rest of p_rescan. */
			TRACEP("rescan", printf("recursive call. ch: %s\n",
				pr_ch(ch)));
			rescan(	&id_buf[0], &expand_buf[0],
				&out[out_len], MAX_RTEXT-out_len, FALSE);

			out_len=strlen(&out[0]);

			/* Use the ch that the recursive call left behind. */
			continue;
		}
		/* Comments might appear here due to concatenation. */
		else if (ch == '/') {
			sysnext();
			if (ch == '*') {
				sysnext();
				skip_comment();
			}
			else {
				out[out_len++] = '/';
			}
		}
		else {
			out[out_len++] = ch;
			sysnext();
		}
	}

	out[out_len] = '\0';
	TRACEP("rescan", printf("out: <%s>\n", pr_str(out)));

	enable_name();

	/* Restore the b_rescan global. */
	if (bounded_flag) {
		/* Restore the old rescan buffer. */
		m_flag   = save_mflag;
		p_rescan = save_prescan;
	}
	else {
		/* Everything is complete. */
		m_flag = FALSE;
	}

	/* Free buffers. */
	m_free(local_in);
	m_free(expand_buf);
	m_free(id_buf);

	RETURN_VOID("rescan");

run_on:
	strcpy(id_buf, "Macro expansion for ");
	strcat(id_buf, name);
	strcat(id_buf, " is too long...\nExpansion set to empty string");
	error(id_buf);

	/* Truncate the ENTIRE macro. */
	out[0] = '\0';

	/* Free the buffers. */
	m_free(local_in);
	m_free(expand_buf);
	m_free(id_buf);

	RETURN_VOID("rescan");
}

/*
	Substitute expanded arguments from argps[] into in[] leaving out[].
	8/1/89: run-on logic added.
*/
static void
substitute(name, nargs, rtext, argps, out, max_out)
char *	name;	/* The name of the macro being expanded.	*/
int	nargs;	/* Number of args for the macro.		*/
char *	rtext;	/* Replacement text of the macro.		*/
char **	argps;	/* Pointer to fully expanded arguments.		*/
char *	out;	/* Output buffer.				*/
int	max_out;	/* Size of out[]. 			*/
{
	int	limit;
	int	c, i, count;
	int	arg_num;
	char	*p, *p2;
	char	str_buf[MAX_SYMBOL];

	TRACEPB("substitute", printf("(%s, %d, <%s>, %p, %p, %d)\n",
		name, nargs, pr_str(rtext), argps, out, max_out));

	/* Make SURE the output buffer is never over-run. */
	max_out -= 4;
	if (max_out < 10) {
		goto run_on;
	}

	/*
		Put the final replacement text into out[].
		Replace ARG_FLAG n by arg n.
		Replace POUND_FLAG n by the stringized arg n.
		Delete CONCAT_FLAG and surrounding whitespace.
	*/

	limit = strlen(rtext);
	count = 0;
	for (i = 0; i < limit;) {

		/*
			Check for run-on expansion.
			This is actually possible.
		*/
		if (count >= max_out) {
			goto run_on;
		}

		c = rtext[i++];
		if (c == ARG_FLAG) {

			/* Copy the argument. */
			arg_num = rtext[i++] - ARG_OFFSET;
			if (arg_num >= nargs) {
				/* Previous user error. */
				out[count++] = ' ';
				continue;
			}

			if (count + strlen(argps[arg_num]) >= max_out) {
				goto run_on;
			}

			str_cpy(out + count, argps [arg_num]);
			count += strlen(argps[arg_num]);
		}
		else if (c == POUND_FLAG) {

			/* Stringize an actual argument */
			arg_num = rtext[i++] - ARG_OFFSET;
			if (arg_num >= nargs) {
				/* Previous user error. */
				out[count++] = ' ';
				continue;
			}
			out[count++] = '"';
			for(p = argps[arg_num]; *p; ) {
			/* bug fix: 2/14/89 (test for single quote also. */
			if (*p == '"' || *p == '\'') {
				/* Copy the string. */
				p += in_string(p, &str_buf[0], MAX_SYMBOL);

				/* Stingize the string. */
				for (p2 = &str_buf[0]; *p2; ) {
					if (count >= max_out-1) {
						goto run_on;
					}
					if (*p2 == '\\' || *p2 == '"') {
						out[count++] = '\\';
						out[count++] = *p2++;
					}
					else {
						out[count++] = *p2++;
					}
				}
			}
			else {
				out[count++] = *p++;
			}
			} /* end for */
			out[count++] = '"';
		}
		else if (c == CONCAT_FLAG) {

			/* Delete preceding whitespace. */
			while (count-1 >= 0 && out[count-1] == ' ') {
				count--;
			}

			/* Delete following whitespace. */
			while (i < limit && rtext[i] == ' ') {
				i++;
			}
		}
		else {
			out[count++] = c;
		}
	}

	/* Append current ch to output. */
	out[count] = '\0';

	TRACEP("substitute",
		printf("count: %d, out: <%s>\n", count, pr_str(out)));

	RETURN_VOID("substitute");

run_on:
	strcpy(str_buf, "Macro expansion for ");
	strcat(str_buf, name);
	strcat(str_buf, " is too long...\nExpansion set to empty string");
	error(str_buf);

	/* Truncate the ENTIRE macro. */
	out[0] = '\0';
	RETURN_VOID("substitute");
}
