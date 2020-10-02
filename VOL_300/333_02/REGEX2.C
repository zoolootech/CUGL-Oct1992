
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "awk.h"


/* Like re_match but tries first a match starting at index `startpos',      */
/* then at startpos + 1, and so on.  `range' is the number of places to try */
/* before giving up.  If `range' is negative, the starting positions tried  */
/* are startpos, startpos - 1, etc.  It is up to the caller to make sure    */
/* that range is not so large as to take the starting position outside of   */
/* the input strings.  The value returned is the position at which the	    */
/* match was found, or -1 if no match was found.			    */

int PASCAL re_search(REPAT_BUFFER *pbufp, char *string, int size,
		     int startpos, int range, REREGS *regs)
{
    register char      *fastmap   = pbufp->fastmap;

    /* Update the fastmap now if not correct already */
    if (fastmap && !pbufp->fastmap_accurate)
	re_compile_fastmap(pbufp);

    while (TRUE)
    {
	/* If a fastmap is supplied, skip quickly over characters that	    */
	/* cannot possibly be the start of a match. Note, however, that if  */
	/* the pattern can possibly match the null string, we must test it  */
	/* at each starting point so that we take the first null string we  */
	/* get. 							    */
	if (fastmap && startpos < size && pbufp->can_be_null != 1)
	{
	    if (range > 0)
	    {
		register int	  lim = 0;
		register char	 *p;
		auto	 int	  irange = range;

		if (startpos < size && startpos + range >= size)
		    lim = range - (size - startpos);

		p = &string[startpos];
		while (range > lim && !fastmap[*p++])
		    range--;
		startpos += irange - range;
	    }
	    else
	    {
		register char   c;

		c = string[startpos];
		if (!fastmap[c])
		    goto advance;
	    }
	}

	if (range >= 0 && startpos == size
	    && fastmap && pbufp->can_be_null == 0)
	    return(-1);

	if (0 <= re_match(pbufp, string, size, startpos, regs))
	    return(startpos);
advance:
	if (!range)
	    break;
	if (range > 0)
	{
	    --range;
	    ++startpos;
	}
	else
	{
	    ++range;
	    --startpos;
	}
    }
    return(-1);
}


/* Match the pattern described by `pbufp' against data which is the virtual  */
/* concatenation of `string1' and `string2'.  `size1' and `size2' are the    */
/* sizes of the two data strings.  Start the match at position `pos'.    Do  */
/* not consider matching past the position `mstop'.  If pbufp->fastmap is    */
/* nonzero, then it had better be up to date.				     */
/*									     */
/* The reason that the data to match is specified as two components which    */
/* are to be regarded as concatenated is so that this function can be used   */
/* directly on the contents of an Emacs buffer.  -1 is returned if there is  */
/* no match.  Otherwise the value is the length of the substring which was   */
/* matched.								     */

int PASCAL re_match(REPAT_BUFFER *pbufp, char *string, int size,
		    int pos, REREGS *regs)
{
    register int	  op;
    register char	 *p    = pbufp->buffer;
    register char	 *pend = p + pbufp->used;
    auto     char	 *end;			   /* end of string */
    auto     char	 *end_match;
    register char	 *d, *dend;
    register int	  mcnt;

    /* Failure point stack.  Each place that can handle a failure further
     * down the line pushes a failure point on this stack.  It consists of
     * two char *'s. The first one pushed is where to resume scanning the
     * pattern; the second pushed is where to resume scanning the strings. If
     * the latter is zero, the failure point is a "dummy". If a failure
     * happens and the innermost failure point is dormant, it discards that
     * failure point and tries the next one. */

    static   int	  stacksiz = 0;
    static   char	**stackb   = NULL;
    auto     char	**stackp, **stacke;

    /* Information on the "contents" of registers. These are pointers into
     * the input strings; they record just what was matched (on this attempt)
     * by some part of the pattern. The start_memory command stores the start
     * of a register's contents and the stop_memory command stores the end. 
     *
     * At that point, regstart[regnum] points to the first character in the
     * register, regend[regnum] points to the first character beyond the end
     * of the register, and regstart_segend[regnum] is either the same as
     * regend[regnum] or else points to the end of the input string into
     * which regstart[regnum] points. The latter case happens when
     * regstart[regnum] is in string1 and regend[regnum] is in string2.  */

    auto     char	 *regstart[RE_NREGS];
    auto     char	 *regstart_segend[RE_NREGS];
    auto     char	 *regend[RE_NREGS];
    static   char	  outmem_msg[] = "Out of memory in re_match()";


    if (0 == stacksiz)
    {
	stacksiz = 2 * NFAILURES;
	if (NULL == (stackb = (char **) malloc(stacksiz * sizeof(char *))))
	    panic(outmem_msg);
    }
    stackp = stackb;
    stacke = stackb + stacksiz;

    end = string + size;

    /* Compute where to stop matching, within the two strings */
    end_match = string + size;

    /* Initialize \( and \) text positions to -1 to mark ones that no \( or */
    /* \) has been seen for.						    */
    for (mcnt = 0; mcnt < sizeof(regstart) / sizeof(*regstart); mcnt++)
	regstart[mcnt] = (char *) -1;

    /* `p' scans through the pattern as `d' scans through the data. `dend' is
     * the end of the input string that `d' points within. `d' is advanced
     * into the following input string whenever necessary, but this happens
     * before fetching; therefore, at the beginning of the loop, `d' can be
     * pointing at the end of a string, but it cannot equal string2.  */

    d	 = string + pos;
    dend = end_match;

    /* This loop loops over pattern commands. It exits by returning from the
     * function if match is complete, or it drops through if match fails at
     * this starting point in the input data. */

    while (TRUE)
    {
	if (p == pend)
	    /* End of pattern means we have succeeded! */
	{
	    /* If caller wants register contents data back, convert it to
	     * indices */
	    if (regs)
	    {
		regend[0]   = d;
		regstart[0] = string;
		for (mcnt = 0; mcnt < RE_NREGS; mcnt++)
		{
		    if ((mcnt != 0) && regstart[mcnt] == (char *) -1)
		    {
			regs->start[mcnt] = -1;
			regs->end[mcnt] = -1;
			continue;
		    }
		    regs->start[mcnt] = regstart[mcnt] - string;
		    regs->end[mcnt] = regend[mcnt] - string;
		}
		regs->start[0] = pos;
	    }
	    return(d - string - pos);
	}

	/* Otherwise match next pattern command */
	op = *p++;
	switch (op)
	{
		/* \( is represented by a start_memory, \) by a stop_memory.
		 * Both of those commands contain a "register number"
		 * argument. The text matched within the \( and \) is
		 * recorded under that number. Then, \<digit> turns into a
		 * `duplicate' command which is followed by the numeric value
		 * of <digit> as the register number. */

	    case RECODE_START_MEMORY:
		regstart[*p] = d;
		regstart_segend[*p++] = dend;
		break;

	    case RECODE_STOP_MEMORY:
		regend[*p] = d;
		if (regstart_segend[*p] == dend)
		    regstart_segend[*p] = d;
		p++;
		break;

	    case RECODE_DUPLICATE:
		{
		    int 	    regno = *p++;    /* Get which register to
						      * match against */
		    register char  *d2, *dend2;

		    d2 = regstart[regno];
		    dend2 = regstart_segend[regno];
		    while (TRUE)
		    {
			/* At end of register contents => success */
			if (d2 == dend2)
			    break;

			/* mcnt gets # consecutive chars to compare */
			mcnt = dend - d;
			if (mcnt > dend2 - d2)
			    mcnt = dend2 - d2;
			/* Compare that many; failure if mismatch, else skip
			 * them. */
			if (bcmp(d, d2, mcnt))
			    goto fail;
			d += mcnt, d2 += mcnt;
		    }
		}
		break;

	    case RECODE_ANYCHAR:
		/* Match anything but a newline.  */
		if (*d++ == '\n')
		    goto fail;
		break;

	    case RECODE_CHARSET:
	    case RECODE_CHARSET_NOT:
		{
		    /* Nonzero for charset_not */
		    auto     int    not = 0;
		    register int    c;

		    if (*(p - 1) == RECODE_CHARSET_NOT)
			not = 1;

		    c = *(unsigned char *) d;

		    if (c < *p * BYTEWIDTH
			&& p[1 + c / BYTEWIDTH] & (1 << (c % BYTEWIDTH)))
			not = !not;

		    p += 1 + *p;

		    if (!not)
			goto fail;
		    d++;
		    break;
		}

	    case RECODE_BEGLINE:
		if (d == string || d[-1] == '\n')
		    break;
		goto fail;

	    case RECODE_ENDLINE:
		if (d == end || *d == '\n')
		    break;
		goto fail;

		/* "or" constructs ("|") are handled by starting each
		 * alternative with an on_failure_jump that points to the
		 * start of the next alternative. Each alternative except the
		 * last ends with a jump to the joining point. (Actually,
		 * each jump except for the last one really jumps to the
		 * following jump, because tensioning the jumps is a hassle.) */

		/* The start of a stupid repeat has an on_failure_jump that
		 * points past the end of the repeat text. This makes a
		 * failure point so that, on failure to match a repetition,
		 * matching restarts past as many repetitions have been found
		 * with no way to fail and look for another one.  */

		/* A smart repeat is similar but loops back to the
		 * on_failure_jump so that each repetition makes another
		 * failure point. */

	    case RECODE_ON_FAILURE_JUMP:
		if (stackp == stacke)
		{
		    auto     char     **stackx;

		    stacksiz = stacksiz + (2 * NFAILURES);
		    stackx   = (char **) realloc(stackb,
						 stacksiz * sizeof(char *));
		    if (NULL == stackx)
			panic(outmem_msg);
		    stackp = stackx + (stackp - stackb);
		    stacke = stackx + stacksiz;
		    stackb = stackx;
		}
		mcnt = *p++ & 0377;
		mcnt += SIGN_EXTEND_CHAR(*p) << 8;
		p++;
		*stackp++ = p + mcnt;
		*stackp++ = d;
		break;

		/* The end of a smart repeat has an maybe_finalize_jump back.
		 * Change it either to a finalize_jump or an ordinary jump. */

	    case RECODE_MAYBE_FINALIZE_JUMP:
		mcnt = *p++ & 0377;
		mcnt += SIGN_EXTEND_CHAR(*p) << 8;
		p++;
		/* Compare what follows with the begining of the repeat. If
		 * we can establish that there is nothing that they would
		 * both match, we can change to finalize_jump */
		if (p == pend)
		    p[-3] = RECODE_FINALIZE_JUMP;
		else
		if (*p == RECODE_EXACTN || *p == RECODE_ENDLINE)
		{
		    register int    c = *p == RECODE_ENDLINE ? '\n' : p[2];
		    register char  *p1 = p + mcnt;

		    /* p1[0] ... p1[2] are an on_failure_jump. Examine what
		     * follows that */
		    if (p1[3] == RECODE_EXACTN && p1[5] != c)
			p[-3] = RECODE_FINALIZE_JUMP;
		    else
		    if (p1[3] == RECODE_CHARSET || p1[3] == RECODE_CHARSET_NOT)
		    {
			int		not = p1[3] == RECODE_CHARSET_NOT;

			if (c < p1[4] * BYTEWIDTH
			  && p1[5 + c / BYTEWIDTH] & (1 << (c % BYTEWIDTH)))
			    not = !not;
			/* not is 1 if c would match */
			/* That means it is not safe to finalize */
			if (!not)
			    p[-3] = RECODE_FINALIZE_JUMP;
		    }
		}
		p -= 2;
		if (p[-1] != RECODE_FINALIZE_JUMP)
		{
		    p[-1] = RECODE_JUMP;
		    goto nofinalize;
		}

		/* The end of a stupid repeat has a finalize-jump back to the
		 * start, where another failure point will be made which will
		 * point after all the repetitions found so far. */

	    case RECODE_FINALIZE_JUMP:
		stackp -= 2;

	    case RECODE_JUMP:
nofinalize:
		mcnt = *p++ & 0377;
		mcnt += SIGN_EXTEND_CHAR(*p) << 8;
		p += mcnt + 1;	/* The 1 compensates for missing ++ above */
		break;

	    case RECODE_DUMMY_FAILURE_JUMP:
		if (stackp == stacke)
		{
		    auto     char     **stackx;

		    stacksiz = stacksiz + (2 * NFAILURES);
		    stackx   = (char **) realloc(stackb,
						 stacksiz * sizeof(char *));
		    if (NULL == stackx)
			panic(outmem_msg);
		    stackp = stackx + (stackp - stackb);
		    stacke = stackx + stacksiz;
		    stackb = stackx;
		}
		*stackp++ = NULL;
		*stackp++ = NULL;
		goto nofinalize;

	    case RECODE_WORDBOUND:
		if (d == string || d == end)
		    break;
		if ((SYNTAX(((unsigned char *) d)[-1]) == Sword)
		    != (SYNTAX(*(unsigned char *) d) == Sword))
		    break;
		goto fail;

	    case RECODE_NOTWORDBOUND:
		if (d == string || d == end)
		    goto fail;
		if ((SYNTAX(((unsigned char *) d)[-1]) == Sword)
		    != (SYNTAX(*(unsigned char *) d) == Sword))
		    goto fail;
		break;

	    case RECODE_WORDBEG:
		if (d == end
		      || SYNTAX(*(unsigned char *) (d)) != Sword)
		    goto fail;
		if (d == string
		      || SYNTAX(((unsigned char *) d)[-1]) != Sword)
		    break;
		goto fail;

	    case RECODE_WORDEND:
		if (d == string
		      || SYNTAX(((unsigned char *) d)[-1]) != Sword)
		    goto fail;
		if (d == end || SYNTAX(*(unsigned char *) d) != Sword)
		    break;
		goto fail;

	    case RECODE_WORDCHAR:
		if (SYNTAX(*(unsigned char *) d++) == 0)
		    goto fail;
		break;

	    case RECODE_NOTWORDCHAR:
		if (SYNTAX(*(unsigned char *) d++) != 0)
		    goto fail;
		break;

	    case RECODE_BEGBUF:
		if (d == string)
		    break;
		goto fail;

	    case RECODE_ENDBUF:
		if (d == end)
		    break;
		goto fail;

	    case RECODE_EXACTN:
		/* Match the next few pattern characters exactly. mcnt is how
		 * many characters to match. */
		mcnt = *p++;
		do
		{
		    if (*d++ != *p++)
			goto fail;
		} while (--mcnt);
		break;
	}
	continue;		/* Successfully matched one pattern command;
				 * keep matching */

	/* Jump here if any matching operation fails. */
fail:
	if (stackp == stackb)
	    break;		/* Matching at this start point fails	 */

	/* A restart point is known.  Restart there and pop it. 	 */
	if (!(*(stackp - 2)))	/* If innermost failure point is dormant */
	{			/* flush it and keep looking		 */
	    stackp -= 2;
	    goto fail;
	}
	d = *--stackp;
	p = *--stackp;
	if (d >= string && d <= end)
	    dend = end_match;
    }
    return(-1); 		/* Failure to match */
}
