/* xc
% cc -O fmt.c -o fmt
% strip fmt
 */
/*
 * fmt - simple text formatter
 *
 * Usage: fmt [-#] [file...]
 *
 * Flags:
 * -#   set maximum line width to # (default 72)
 *
 * Fmt produces, from the concatenation of the input files, or from the
 * standard input if no files are specified, standard output with each
 * line as close as possible to width characters wide.  It preserves
 * blank lines, the spaces at the beginnings of lines and between words,
 * start of paragraph indentations, and dot command lines (used by many
 * more complex text formatters).
 * It is useful for formatting mail messages, and can be used to justify
 * a paragraph from within vi(1) with the key sequence {!}fmt.
 *
 * This program is in the public domain.
 * David MacKenzie
 * 6522 Elgin Lane
 * Bethesda, MD 20817
 *
 * Latest revision: 05/08/88
 */

/* Define only one OS as nonzero. */
#define UNIX 0
#define MSDOS 1			/* Specifically, Aztec C. */

/* Define as nonzero for debugging. */
#define DEBUG 0
#define DEBUGFILE "debug.out"

#define DEF_WIDTH 72		/* Default formatted line width. */
#define DEF_TWIDTH 8		/* Default width of tab stops. */

/* Characters which end sentences and are followed by two spaces. */
#define SENT_ENDERS ".!?\""

/* The states that the formatter can be in, describing what's read. */
#define STATE_INDENT 1
#define STATE_WORD 2
#define STATE_SPACE 3

#include <stdio.h>

#if MSDOS
#define getc(f) agetc(f)

_main(argc, argv)
#else
main(argc, argv)
#endif
    int     argc;
    char  **argv;
{
    void    fmt();

#if UNIX
    char   *rindex();

#endif
    char   *prog;		/* Base of name we were called with. */
    int     lwid = DEF_WIDTH;	/* Output line width. */
    int     twid = DEF_TWIDTH;	/* Assumed width of tabs. */
    int     optind;		/* Loop index. */

#if UNIX
    prog = rindex(argv[0], '/');
    prog = prog ? prog + 1 : argv[0];
#endif
#if MSDOS
    prog = "fmt";
#endif

    for (optind = 1; optind < argc && *argv[optind] == '-'; ++optind)
	if ((lwid = atoi(&argv[optind][1])) <= 0) {
	    fprintf(stderr, "%d: illegal line width\n", lwid);
		exit(1);
	}

    if (optind == argc)
	fmt("-", lwid, twid);
    else
	for (; optind < argc; ++optind)
	    fmt(argv[optind], lwid, twid);

    exit(0);
}

/*
 * Send the formatted contents of file ("-" for stdin) to stdout.
 * Except when reading an indent, we increment out_pos when characters
 * are read into the pend_ buffers, not when they are printed.
 * Whenever we're done with using the current value of a variable, we
 * reset it.
 * Terminology: "white characters" consist of spaces (' ' and '\t') and
 * newline.  The '\r' character is treated, perhaps wrongly, as an
 * ordinary nonwhite character.
 */

void
fmt(file, lwid, twid)
    char   *file;
    int     lwid;
    int     twid;
{
    char   *malloc();

#if DEBUG
    FILE   *errp;

#endif
    FILE   *fp = stdin;		/* Input file pointer. */
    int     c;			/* One character of input. */
    int     last_nw;		/* Last nonwhite character read. */
    int     out_pos;		/* Logical output column. */
    int     pos_incr;		/* Temporary for tabs calculation. */
    int     old_indent_pos;	/* For breaking at indented paragraphs. */
    int     i;			/* Loop counter for printing strings. */
    char    initial;		/* To prevent newline before first line. */
    char    dot_line;		/* For preserving text formatter dot cmds. */
    char    state;		/* Type of characters currently reading. */

    /*
     * Instead of null terminating these strings, we keep length counts. That
     * way we can add single characters to the ends of them easily. The _len
     * variables count physical characters (string length); the _pos
     * variables count screen length (adjusting for tab width). 
     */
    char   *indent;		/* Indentation. */
    int     indent_len;
    int     indent_pos;
    char   *pend_spaces;	/* Buffered inter-word spaces/tabs. */
    int     spaces_len;
    int     spaces_pos;
    char   *pend_word;		/* Buffered nonwhite characters. */
    int     word_len;

#if DEBUG
    errp = fopen(DEBUGFILE, "a");
    fprintf(errp, "File: %s\n", file);
#endif

    if (strcmp(file, "-") && !(fp = fopen(file, "r"))) {
	perror(file);
	exit(1);
    }
    if (!(indent = malloc(lwid)) ||
	!(pend_spaces = malloc(lwid)) ||
	!(pend_word = malloc(lwid))) {
	perror("malloc");
	exit(1);
    }
    /* Set up by pretending we just received "\n\n". */
    out_pos = lwid;
    old_indent_pos = 0;
    indent_len = 0;
    indent_pos = 0;
    spaces_len = 0;
    spaces_pos = 0;
    word_len = 0;
    initial = 1;
    dot_line = 0;
    state = STATE_INDENT;

    while ((c = getc(fp)) != EOF) {
	switch (state) {
	case STATE_INDENT:
#if DEBUG
	    fprintf(errp,
		"INDENT: out_pos=%d, dot_line=%d, indent_pos=%d, c=%c\n",
		out_pos, dot_line, indent_pos, c);
#endif
	    /*
	     * When in this state, we're reading the indentation at the start
	     * of an input line into indent, but we're not printing it. That
	     * only happens when the end of an *output* line is reached.
	     * Thus, this indent might never get printed if it's one of a
	     * string of short lines. 
	     */
	    switch (c) {
	    case ' ':
	    case '\t':
		pos_incr = c == ' ' ? 1 : twid - indent_pos % twid;
		/* If indent > max. line wid-1, ignore rest of it. */
		if (indent_pos + pos_incr < lwid) {
		    indent[indent_len++] = c;
		    indent_pos += pos_incr;
		}
		break;
	    case '\n':
		/*
		 * We were reading the indent, but got no printable
		 * characters before the newline.  We basically just read a
		 * blank line. 
		 */
		/* Terminate previous partial line. */
		if (spaces_len)
		    putchar('\n');
		spaces_len = 0;
		spaces_pos = 0;
		putchar('\n');
		initial = 1;
		out_pos = lwid;
		indent_len = 0;
		indent_pos = 0;
		break;
	    default:
		/* First nonwhite character on this input line. */
		/* If char ends sentence, add extra space. */
		if (index(SENT_ENDERS, last_nw) && c >= 'A' && c <= 'Z') {
		    pend_spaces[spaces_len++] = ' ';
		    ++spaces_pos;
		    ++out_pos;
		}
		pend_word[word_len++] = c;
		last_nw = c;
		++out_pos;
		if (indent_pos > old_indent_pos)
		    /* New para. - force output line break at end of word. */
		    out_pos = lwid;
		/* Preserve text formatter command lines (start with '.'). */
		if (c == '.' && indent_pos == 0) {
		    out_pos = lwid;
		    dot_line = 1;
		}
		state = STATE_WORD;
		break;
	    }
	    break;
	case STATE_WORD:
#if DEBUG
	    fprintf(errp,
		"WORD:   out_pos=%d, dot_line=%d, indent_pos=%d, c=%c\n",
		out_pos, dot_line, indent_pos, c);
#endif
	    /*
	     * When in this state, we have read at least one nonwhite
	     * character and are buffering them in pend_word until we read a
	     * white character, at which point we can determine whether to
	     * break the output line. 
	     */
	    switch (c) {
	    case ' ':
	    case '\t':
	    case '\n':
	white:
		if (out_pos >= lwid) {
		    /* Wrap. */
		    if (initial)
			initial = 0;
		    else
			putchar('\n');
		    for (i = 0; i < indent_len; ++i)
			putchar(indent[i]);
		    out_pos = indent_pos + word_len;
		} else {
		    for (i = 0; i < spaces_len; ++i)
			putchar(pend_spaces[i]);
		}
		spaces_len = 0;
		spaces_pos = 0;
		for (i = 0; i < word_len; ++i)
		    putchar(pend_word[i]);
		word_len = 0;
		if (c == '\n') {
		    /* If we read end of a dot cmd line, force line break. */
		    if (dot_line)
			out_pos = lwid;
		    old_indent_pos = indent_pos;
		    indent_len = 0;
		    indent_pos = 0;
		    pend_spaces[spaces_len++] = ' ';
		    ++spaces_pos;
		    ++out_pos;
		    dot_line = 0;
		    state = STATE_INDENT;
		} else {
		    pend_spaces[spaces_len++] = c;
		    pos_incr = c == ' ' ? 1 : twid - out_pos % twid;
		    spaces_pos += pos_incr;
		    out_pos += pos_incr;
		    state = STATE_SPACE;
		}
		break;
	    default:
		/* If word is longer than line max., wrap it. */
		if (word_len < lwid) {
		    pend_word[word_len++] = c;
		    last_nw = c;
		    ++out_pos;
		} else {
		    ungetc(c, fp);
		    c = ' ';
		    goto white;	/* Ick! Ick! */
		}
		break;
	    }
	    break;
	case STATE_SPACE:
#if DEBUG
	    fprintf(errp,
		"SPACE:  out_pos=%d, dot_line=%d, indent_pos=%d, c=%c\n",
		out_pos, dot_line, indent_pos, c);
#endif
	    /*
	     * When in this state, we have read at least one space or tab
	     * after having read at least one nonwhite character on the line,
	     * and are buffering them into pend_spaces. 
	     */
	    switch (c) {
	    case ' ':
	    case '\t':
		pos_incr = c == ' ' ? 1 : twid - out_pos % twid;
		/* If space length > max. line wid-1, ignore rest of it. */
		if (spaces_pos + pos_incr < lwid) {
		    pend_spaces[spaces_len++] = c;
		    spaces_pos += pos_incr;
		    out_pos += pos_incr;
		}
		break;
	    case '\n':
		/*
		 * These spaces were at the end of an input line.  Since it
		 * might not be the end of an output line, we ignore them in
		 * favor of this rule: if the last nonwhite character was one
		 * that ends sentences, we add two spaces, otherwise one. 
		 */
		/* We read the end of a dot cmd line ending with spaces. */
		if (dot_line)
		    out_pos = lwid;
		old_indent_pos = indent_pos;
		indent_len = 0;
		indent_pos = 0;
		spaces_len = 0;
		spaces_pos = 0;
		pend_spaces[spaces_len++] = ' ';
		++spaces_pos;
		dot_line = 0;
		state = STATE_INDENT;
		break;
	    default:
		pend_word[word_len++] = c;
		last_nw = c;
		++out_pos;
		state = STATE_WORD;
		break;
	    }
	    break;
	}
    }
    if (word_len) {
	if (out_pos >= lwid) {
	    /* Wrap. */
	    putchar('\n');
	    for (i = 0; i < indent_len; ++i)
		putchar(indent[i]);
	} else {
	    for (i = 0; i < spaces_len; ++i)
		putchar(pend_spaces[i]);
	}
	for (i = 0; i < word_len; ++i)
	    putchar(pend_word[i]);
    }
    /* Don't print newline if file ended with several newlines. */
    if (word_len || spaces_len)
	putchar('\n');
    free(pend_word);
    free(pend_spaces);
    free(indent);
    if (fp != stdin)
	fclose(fp);
#if DEBUG
    fclose(errp);
#endif
}
