/* Extended regular expression matching and search.
   Copyright (C) 1985 Free Software Foundation, Inc.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "awk.h"

/* To test, compile with -Dtest.  This Dtestable feature turns this into   */
/* a self-contained program which reads a pattern, describes how it	   */
/* compiles, then reads a string and searches for it.			   */


STATIC VOID	 NEAR PASCAL	init_syntax_once(void);
STATIC VOID	 NEAR PASCAL	store_jump(char *from, char opcode, char *to);
STATIC VOID	 NEAR PASCAL	insert_jump(char op, char *from,
					    char *to, char *current_end);



/* JF this var has taken on whole new meanings as time goes by.  Various   */
/* bits in this int tell how certain pieces of syntax should work	   */

static int	     obscure_syntax = 0;


/*  Define the syntax stuff, so we can do the \<...\> things.  */


STATIC VOID NEAR PASCAL init_syntax_once(void)
{
    register int       c;
    static   BOOL      done = FALSE;

    if (!done)
    {
	memset(re_syntax_table, 0, sizeof(re_syntax_table));
	for (c = 'a'; c <= 'z'; c++)
	    re_syntax_table[c] = Sword;
	for (c = 'A'; c <= 'Z'; c++)
	    re_syntax_table[c] = Sword;
	for (c = '0'; c <= '9'; c++)
	    re_syntax_table[c] = Sword;
	done = TRUE;
    }
    return;
}



/* compile_pattern takes a regular-expression descriptor string in the	    */
/* user's format and converts it into a buffer full of byte commands for    */
/* matching.								    */
/*									    */
/*  pattern   is the address of the pattern string			    */
/*  size      is the length of it.					    */
/*  bufp      is a  struct re_pattern_buffer *	which points to the info    */
/*	      on where to store the byte commands.			    */
/*	      This structure contains a  char *  which points to the	    */
/*	      actual space, which should have been obtained with malloc.    */
/*	      compile_pattern may use  realloc	to grow the buffer space.   */
/*									    */
/* The number of bytes of commands can be found out by looking in the	    */
/* struct re_pattern_buffer that bufp pointed to, after compile_pattern     */
/* returns.								    */

#define PATPUSH(ch)		(*b++ = (char) (ch))

#define PATFETCH(c) \
 {if (p == pend) goto end_of_pattern; \
  c = * (unsigned char *) p++; }

#define PATFETCH_RAW(c) \
 {if (p == pend) goto end_of_pattern; \
  c = * (unsigned char *) p++; }

#define PATUNFETCH		   p--

#define EXTEND_BUFFER \
  { char *old_buffer = bufp->buffer; \
    if (bufp->allocated == (1<<16)) goto too_big; \
    bufp->allocated *= 2; \
    if (bufp->allocated > (1<<16)) bufp->allocated = (1<<16); \
    if (!(bufp->buffer = (char *) realloc (bufp->buffer, bufp->allocated))) \
      goto memory_exhausted; \
    c = bufp->buffer - old_buffer; \
    b += c; \
    if (fixup_jump) fixup_jump += c; \
    if (laststart) laststart += c; \
    begalt += c; \
    if (pending_exact) pending_exact += c; \
  }


/* JF this function is used to compile UN*X style regexps.  In particular,  */
/* ( ) and | don't have to be \ed to have a special meaning                 */

int PASCAL re_set_syntax(int syntax)
{
    auto     int	ret;

    ret 	   = obscure_syntax;
    obscure_syntax = syntax;
    return(ret);
}


char * PASCAL re_compile_pattern(char *pattern, int size, REPAT_BUFFER *bufp)
{
    register char	     *b    = bufp->buffer;
    register char	     *p    = pattern;
    auto     char	     *pend = pattern + size;
    register unsigned	      c, c1;
    auto     char	     *p1;

    /* address of the count-byte of the most recently inserted "exactn"  */
    /* command. This makes it possible to tell whether a new exact-match */
    /* character can be added to that command or requires a new "exactn" */
    /* command. 							 */
    auto     char	     *pending_exact = 0;

    /* address of the place where a forward-jump should go to the end of the */
    /* containing expression. Each alternative of an "or", except the last,  */
    /* ends with a forward-jump of this sort.				     */
    auto     char	     *fixup_jump = 0;

    /* address of start of the most recently finished expression. This tells */
    /* postfix * where to find the start of its operand.		     */
    auto     char	    *laststart = 0;

    /* In processing a repeat, TRUE means zero matches is allowed */
    auto     BOOL	     zero_times_ok;

    /* In processing a repeat, TRUE means many matches is allowed */
    auto     BOOL	     many_times_ok;

    /* address of beginning of regexp, or inside of last \( */
    auto     char	    *begalt = b;

    /* Stack of information saved by \( and restored by \). Four stack	    */
    /* elements are pushed by each \(: First, the value of b. Second, the   */
    /* value of fixup_jump. Third, the value of regnum. Fourth, the value of*/
    /* begalt.								    */
    auto     int	     stackb[40];
    auto     int	    *stackp = stackb;
    auto     int	    *stacke = stackb + 40;
    auto     int	    *stackt;

    /* Counts \('s as they are encountered.  Remembered for the matching \), */
    /* where it becomes the "register number" to put in the stop_memory      */
    /* command								     */
    auto     int	     regnum = 1;

    bufp->fastmap_accurate = 0;
    init_syntax_once();

    if (bufp->allocated == 0)
    {
	bufp->allocated = 28;
	if (bufp->buffer)
	    /* EXTEND_BUFFER loses when bufp->allocated is 0 */
	    bufp->buffer = (char *) realloc(bufp->buffer, 28);
	else
	    /* Caller did not allocate a buffer.  Do it for him */
	    bufp->buffer = (char *) malloc(28);
	if (!bufp->buffer)
	    goto memory_exhausted;
	begalt = b = bufp->buffer;
    }

    while (p != pend)
    {
	if (b - bufp->buffer > bufp->allocated - 10)
	    EXTEND_BUFFER;	   /* Note that EXTEND_BUFFER clobbers c */

	PATFETCH(c);

	switch (c)
	{
	    case '$':
		/* $ means succeed if at end of line, but only in special  */
		/* contexts. If randomly in the middle of a pattern, it is */
		/* a normal character.					   */
		if (p == pend || (*p == '\\' && (p[1] == ')' || p[1] == '|')))
		{
		    PATPUSH(RECODE_ENDLINE);
		    break;
		}
		goto normal_char;
	    case '^':
		/* ^ means succeed if at beg of line, but only if no	   */
		/* preceding pattern.					   */
		if (laststart)
		    goto normal_char;
		PATPUSH(RECODE_BEGLINE);
		break;
	    case '*':
	    case '+':
	    case '?':
		/* If there is no previous pattern, char not special.	  */
		if (!laststart)
		    goto normal_char;
		/* If there is a sequence of repetition chars, collapse it
		 * down to equivalent to just one.  */
		zero_times_ok = FALSE;
		many_times_ok = FALSE;
		while (TRUE)
		{
		    zero_times_ok |= (c != '+');
		    many_times_ok |= (c != '?');
		    if (p == pend)
			break;
		    PATFETCH(c);
		    if (!(c == '*' || c == '+' || c == '?'))
		    {
			PATUNFETCH;
			break;
		    }
		}

		/* Now we know whether 0 matches is allowed, and whether 2 or
		 * more matches is allowed.  */
		if (many_times_ok)
		{
		    /* If more than one repetition is allowed, put in a
		     * backward jump at the end.  */
		    store_jump(b, RECODE_MAYBE_FINALIZE_JUMP, laststart - 3);
		    b += 3;
		}
		insert_jump(RECODE_ON_FAILURE_JUMP, laststart, b + 3, b);
		pending_exact = 0;
		b += 3;
		if (!zero_times_ok)
		{
		    /* At least one repetition required: insert before the
		     * loop a skip over the initial on-failure-jump
		     * instruction */
		    insert_jump(RECODE_DUMMY_FAILURE_JUMP, laststart,
				laststart + 6, b);
		    b += 3;
		}
		break;

	    case '.':
		laststart = b;
		PATPUSH(RECODE_ANYCHAR);
		break;

	    case '[':
		if (b - bufp->buffer
		    > bufp->allocated - 3 - (1 << BYTEWIDTH) / BYTEWIDTH)
		    /* Note that EXTEND_BUFFER clobbers c */
		    EXTEND_BUFFER;

		laststart = b;
		if (*p == '^')
		{
		    PATPUSH(RECODE_CHARSET_NOT);
		    ++p;
		}
		else
		    PATPUSH(RECODE_CHARSET);
		p1 = p;

		PATPUSH((1 << BYTEWIDTH) / BYTEWIDTH);
		/* Clear the whole map */
		bzero(b, (1 << BYTEWIDTH) / BYTEWIDTH);
		/* Read in characters and ranges, setting map bits */
		while (TRUE)
		{
		    PATFETCH(c);
		    if (c == ']' && p != p1 + 1)
			break;
		    if (*p == '-')
		    {
			PATFETCH(c1);
			PATFETCH(c1);
			while (c <= c1)
			    b[c / BYTEWIDTH] |= 1 << (c % BYTEWIDTH), c++;
		    }
		    else
		    {
			b[c / BYTEWIDTH] |= 1 << (c % BYTEWIDTH);
		    }
		}
		/* Discard any bitmap bytes that are all 0 at the end of the
		 * map. Decrement the map-length byte too. */
		while (b[-1] > 0 && b[b[-1] - 1] == 0)
		    b[-1]--;
		b += b[-1];
		break;

	    case '(':
		if (!(obscure_syntax & RE_NO_BK_PARENS))
		    goto normal_char;
		if (stackp == stacke)
		    goto nesting_too_deep;
		if (regnum < RE_NREGS)
		{
		    PATPUSH(RECODE_START_MEMORY);
		    PATPUSH(regnum);
		}
		*stackp++ = b - bufp->buffer;
		*stackp++ = fixup_jump ? fixup_jump - bufp->buffer + 1 : 0;
		*stackp++ = regnum++;
		*stackp++ = begalt - bufp->buffer;
		fixup_jump = 0;
		laststart = 0;
		begalt = b;
		break;

	    case ')':
		if (!(obscure_syntax & RE_NO_BK_PARENS))
		    goto normal_char;
		if (stackp == stackb)
		    goto unmatched_close;
		begalt = bufp->buffer + (*--stackp);
		if (fixup_jump)
		    store_jump(fixup_jump, RECODE_JUMP, b);
		if (stackp[-1] < RE_NREGS)
		{
		    PATPUSH(RECODE_STOP_MEMORY);
		    PATPUSH(stackp[-1]);
		}
		stackp -= 2;
		fixup_jump = 0;
		if (*stackp)
		    fixup_jump = bufp->buffer + *stackp - 1;
		laststart = bufp->buffer + (*--stackp);
		break;

	    case '|':
		if (!(obscure_syntax & RE_NO_BK_VBAR))
		    goto normal_char;
		insert_jump(RECODE_ON_FAILURE_JUMP, begalt, b + 6, b);
		pending_exact = 0;
		b += 3;
		if (fixup_jump)
		    store_jump(fixup_jump, RECODE_JUMP, b);
		fixup_jump = b;
		b += 3;
		laststart = 0;
		begalt = b;
		break;

	    case '\\':
		if (p == pend)
		    goto invalid_pattern;
		PATFETCH_RAW(c);
		switch (c)
		{
		    case '(':
			if (obscure_syntax & RE_NO_BK_PARENS)
			    goto normal_backsl;
			if (stackp == stacke)
			    goto nesting_too_deep;
			if (regnum < RE_NREGS)
			{
			    PATPUSH(RECODE_START_MEMORY);
			    PATPUSH(regnum);
			}
			*stackp++ = b - bufp->buffer;
			*stackp++ = fixup_jump ? fixup_jump - bufp->buffer + 1
					       : 0;
			*stackp++ = regnum++;
			*stackp++ = begalt - bufp->buffer;
			fixup_jump = 0;
			laststart = 0;
			begalt = b;
			break;

		    case ')':
			if (obscure_syntax & RE_NO_BK_PARENS)
			    goto normal_backsl;
			if (stackp == stackb)
			    goto unmatched_close;
			begalt = bufp->buffer + (*--stackp);
			if (fixup_jump)
			    store_jump(fixup_jump, RECODE_JUMP, b);
			if (stackp[-1] < RE_NREGS)
			{
			    PATPUSH(RECODE_STOP_MEMORY);
			    PATPUSH(stackp[-1]);
			}
			stackp -= 2;
			fixup_jump = 0;
			if (*stackp)
			    fixup_jump = bufp->buffer + *stackp - 1;
			laststart = bufp->buffer + (*--stackp);
			break;

		    case '|':
			if (obscure_syntax & RE_NO_BK_VBAR)
			    goto normal_backsl;
			insert_jump(RECODE_ON_FAILURE_JUMP, begalt, b + 6, b);
			pending_exact = 0;
			b += 3;
			if (fixup_jump)
			    store_jump(fixup_jump, RECODE_JUMP, b);
			fixup_jump = b;
			b += 3;
			laststart = 0;
			begalt = b;
			break;

		    case 'w':
			laststart = b;
			PATPUSH(RECODE_WORDCHAR);
			break;

		    case 'W':
			laststart = b;
			PATPUSH(RECODE_NOTWORDCHAR);
			break;

		    case '<':
			PATPUSH(RECODE_WORDBEG);
			break;

		    case '>':
			PATPUSH(RECODE_WORDEND);
			break;

		    case 'b':
			PATPUSH(RECODE_WORDBOUND);
			break;

		    case 'B':
			PATPUSH(RECODE_NOTWORDBOUND);
			break;

		    case '`':
			PATPUSH(RECODE_BEGBUF);
			break;

		    case '\'':
			PATPUSH(RECODE_ENDBUF);
			break;

		    case '1':
		    case '2':
		    case '3':
		    case '4':
		    case '5':
		    case '6':
		    case '7':
		    case '8':
		    case '9':
			c1 = c - '0';
			if (c1 >= regnum)
			    goto normal_char;
			for (stackt = stackp - 2; stackt > stackb; stackt -= 4)
			    if (*stackt == c1)
				goto normal_char;
			laststart = b;
			PATPUSH(RECODE_DUPLICATE);
			PATPUSH(c1);
			break;
		    default:
normal_backsl:
			goto normal_char;
		}
		break;

	    default:
normal_char:
		if (!pending_exact || pending_exact + *pending_exact + 1 != b
		    || *pending_exact == 0177 || *p == '*' || *p == '^'
		    || *p == '+' || *p == '?')
		{
		    laststart = b;
		    PATPUSH(RECODE_EXACTN);
		    pending_exact = b;
		    PATPUSH(0);
		}
		PATPUSH(c);
		(*pending_exact)++;
	}
    }

    if (fixup_jump)
	store_jump(fixup_jump, RECODE_JUMP, b);

    if (stackp != stackb)
	goto unmatched_open;

    bufp->used = b - bufp->buffer;
    return(NULL);

invalid_pattern:
    return("Invalid regular expression");

unmatched_open:
    return("Unmatched \\(");

unmatched_close:
    return("Unmatched \\)");

end_of_pattern:
    return("Premature end of regular expression");

nesting_too_deep:
    return("Nesting too deep");

too_big:
    return("Regular expression too big");

memory_exhausted:
    return("Memory exhausted");
}


/* Store where `from' points a jump operation to jump to where `to' points. */
/* `opcode' is the opcode to store.                                         */

STATIC VOID NEAR PASCAL store_jump(char *from, char opcode, char *to)
{
    from[0] = opcode;
    from[1] = (to - (from + 3)) & 0377;
    from[2] = (to - (from + 3)) >> 8;
    return;
}


/* Open up space at char FROM, and insert there a jump to TO.  CURRENT_END  */
/* gives te end of the storage no in use, so we know how much data to copy  */
/* up.	OP is the opcode of the jump to insert.  If you call this function, */
/* you must zero out pending_exact.					    */

STATIC VOID NEAR PASCAL insert_jump(char op, char *from,
				    char *to, char *current_end)
{
    register char	 *pto	= current_end + 3;
    register char	 *pfrom = current_end;

    while (pfrom != from)
	*--pto = *--pfrom;
    store_jump(from, op, to);
    return;
}


/* Given a pattern, compute a fastmap from it.	The fastmap records which   */
/* of the (1 << BYTEWIDTH) possible characters can start a string that	    */
/* matches the pattern.  This fastmap is used by re_search to skip quickly  */
/* over totally implausible text.					    */
/*									    */
/* The caller must supply the address of a (1 << BYTEWIDTH)-byte data area  */
/* as bufp->fastmap.  The other components of bufp describe the pattern to  */
/* be used.								    */

VOID PASCAL re_compile_fastmap(REPAT_BUFFER *bufp)
{
    register int		c;
    auto     unsigned char     *pattern = (unsigned char *) bufp->buffer;
    auto     int		size	= bufp->used;
    register char	       *fastmap = bufp->fastmap;
    register unsigned char     *p	= pattern;
    register unsigned char     *pend	= pattern + size;
    register int		j;
    auto     unsigned char     *stackb[NFAILURES];
    auto     unsigned char    **stackp	= stackb;

    memset(fastmap, 0, (1 << BYTEWIDTH));
    bufp->fastmap_accurate = TRUE;
    bufp->can_be_null	   = 0;

    while (p)
    {
	if (p == pend)
	{
	    bufp->can_be_null = 1;
	    break;
	}
	c = *p++;
	switch (c)
	{
	    case RECODE_EXACTN:
		fastmap[p[1]] = 1;
		break;
	    case RECODE_BEGLINE:
	    case RECODE_BEFORE_DOT:
	    case RECODE_AT_DOT:
	    case RECODE_AFTER_DOT:
	    case RECODE_BEGBUF:
	    case RECODE_ENDBUF:
	    case RECODE_WORDBOUND:
	    case RECODE_NOTWORDBOUND:
	    case RECODE_WORDBEG:
	    case RECODE_WORDEND:
		continue;
	    case RECODE_ENDLINE:
		fastmap['\n'] = 1;
		if (bufp->can_be_null != 1)
		    bufp->can_be_null = 2;
		break;
	    case RECODE_FINALIZE_JUMP:
	    case RECODE_MAYBE_FINALIZE_JUMP:
	    case RECODE_JUMP:
		bufp->can_be_null = 1;		 /* intentional fall thru */
	    case RECODE_DUMMY_FAILURE_JUMP:
		j = *p++ & 0377;
		j += SIGN_EXTEND_CHAR(*(char *) p) << 8;
		p += j + 1;	/* The 1 compensates for missing ++ above */
		if (j > 0)
		    continue;
		/* Jump backward reached implies we just went through the
		 * body of a loop and matched nothing. Opcode jumped to
		 * should be an on_failure_jump. Just treat it like an
		 * ordinary jump. For a * loop, it has pushed its failure
		 * point already; if so, discard that as redundant.  */
		if (*p != RECODE_ON_FAILURE_JUMP)
		    continue;
		p++;
		j = *p++ & 0377;
		j += SIGN_EXTEND_CHAR(*(char *) p) << 8;
		p += j + 1;	/* The 1 compensates for missing ++ above */
		if (stackp != stackb && *stackp == p)
		    stackp--;
		continue;
	    case RECODE_ON_FAILURE_JUMP:
		j = *p++ & 0377;
		j += SIGN_EXTEND_CHAR(*(char *) p) << 8;
		p++;
		*++stackp = p + j;
		continue;
	    case RECODE_START_MEMORY:
	    case RECODE_STOP_MEMORY:
		p++;
		continue;
	    case RECODE_DUPLICATE:
		bufp->can_be_null = 1;
		fastmap['\n'] = 1;
	    case RECODE_ANYCHAR:
		for (j = 0; j < (1 << BYTEWIDTH); j++)
		    if (j != '\n')
			fastmap[j] = 1;
		if (bufp->can_be_null)
		    return;
		/* Don't return; check the alternative paths so we can set
		 * can_be_null if appropriate.  */
		break;
	    case RECODE_WORDCHAR:
		for (j = 0; j < (1 << BYTEWIDTH); j++)
		    if (SYNTAX(j) == Sword)
			fastmap[j] = 1;
		break;
	    case RECODE_NOTWORDCHAR:
		for (j = 0; j < (1 << BYTEWIDTH); j++)
		    if (SYNTAX(j) != Sword)
			fastmap[j] = 1;
		break;
	    case RECODE_CHARSET:
		for (j = *p++ * BYTEWIDTH - 1; j >= 0; j--)
		    if (p[j / BYTEWIDTH] & (1 << (j % BYTEWIDTH)))
		    {
			fastmap[j] = 1;
		    }
		break;
	    case RECODE_CHARSET_NOT:
		/* Chars beyond end of map must be allowed */
		for (j = *p * BYTEWIDTH; j < (1 << BYTEWIDTH); j++)
		    fastmap[j] = 1;

		for (j = *p++ * BYTEWIDTH - 1; j >= 0; j--)
		    if (!(p[j / BYTEWIDTH] & (1 << (j % BYTEWIDTH))))
		    {
			fastmap[j] = 1;
		    }
		break;
	}

	/* Get here means we have successfully found the possible starting
	 * characters of one path of the pattern.  We need not follow this
	 * path any farther. Instead, look at the next alternative remembered
	 * in the stack. */
	if (stackp != stackb)
	    p = *stackp--;
	else
	    break;
    }
    return;
}


/* Entry points compatible with bsd4.2 regex library */


static REPAT_BUFFER		  re_comp_buf;

char * PASCAL re_comp(char *s)
{
    if (!s)
    {
	if (!re_comp_buf.buffer)
	    return "No previous regular expression";
	return(NULL);
    }

    if (!re_comp_buf.buffer)
    {
	if (!(re_comp_buf.buffer = (char *) malloc(200)))
	    return("Memory exhausted");
	re_comp_buf.allocated = 200;
	if (!(re_comp_buf.fastmap = (char *) malloc(1 << BYTEWIDTH)))
	    return("Memory exhausted");
    }
    return(re_compile_pattern(s, strlen(s), &re_comp_buf));
}


int PASCAL re_exec(char *s)
{
    auto     int       len = strlen(s);

    return(0 <= re_search(&re_comp_buf, s, len, 0, len, 0));
}
