/*	INPUT:	Various input routines for MicroEMACS
		written by Daniel Lawrence
		5/9/86						*/

#include	<stdio.h>
#include	"estruct.h"
#include	"edef.h"

/*
 * Ask a yes or no question in the message line. Return either TRUE, FALSE, or
 * ABORT. The ABORT status is returned if the user bumps out of the question
 * with a ^G. Used any time a confirmation is required.
 */

mlyesno(prompt)

char *prompt;

{
	char c;			/* input character */
	char buf[NPAT];		/* prompt to user */

	/* build and prompt the user */
	strcpy(buf, prompt);
	strcat(buf, " [y/n]? ");
	mlwrite(buf);

	for (;;) {
		/* get the responce */
		c = tgetc();

		if (c == ectoc(abortc))		/* Bail out! */
			return(ABORT);

		if (c=='y' || c=='Y')
			return(TRUE);

		if (c=='n' || c=='N')
			return(FALSE);
	}
}

/*
 * Write a prompt into the message line, then read back a response. Keep
 * track of the physical position of the cursor. If we are in a keyboard
 * macro throw the prompt away, and return the remembered response. This
 * lets macros run at full speed. The reply is always terminated by a carriage
 * return. Handle erase, kill, and abort keys.
 */

mlreply(prompt, buf, nbuf)
    char *prompt;
    char *buf;
{
	return(nextarg(prompt, buf, nbuf, ctoec('\n')));
}

mltreply(prompt, buf, nbuf, eolchar)

char *prompt;
char *buf;
int eolchar;

{
	return(nextarg(prompt, buf, nbuf, eolchar));
}

/*	ectoc:	expanded character to character
		colapse the CTRL and SPEC flags back into an ascii code   */

ectoc(c)

int c;

{
	if (c & CTRL)
		c = c & ~(CTRL | 0x40);
	if (c & SPEC)
		c= c & 255;
	return(c);
}

/*	ctoec:	character to extended character
		pull out the CTRL and SPEC prefixes (if possible)	*/

ctoec(c)

int c;

{
        if (c>=0x00 && c<=0x1F)
                c = CTRL | (c+'@');
        return (c);
}
 
/* get a command name from the command line. Command completion means
   that pressing a <SPACE> will attempt to complete an unfinished command
   name if it is unique.
*/

int (*getname())()

{
#if	ST520 & LATTICE
#define register		
#endif
	register int cpos;	/* current column on screen output */
	register int c;
	register char *sp;	/* pointer to string for output */
	register NBIND *ffp;	/* first ptr to entry in name binding table */
	register NBIND *cffp;	/* current ptr to entry in name binding table */
	register NBIND *lffp;	/* last ptr to entry in name binding table */
	char buf[NSTRING];	/* buffer to hold tentative command name */
	int (*fncmatch())();

	/* starting at the beginning of the string buffer */
	cpos = 0;

	/* if we are executing a command line get the next arg and match it */
	if (clexec) {
		if (macarg(buf) != TRUE)
			return(FALSE);
		return(fncmatch(&buf[0]));
	}

	/* build a name string from the keyboard */
	while (TRUE) {
		c = tgetc();

		/* if we are at the end, just match it */
		if (c == 0x0d) {
			buf[cpos] = 0;

			/* and match it off */
			return(fncmatch(&buf[0]));

		} else if (c == ectoc(abortc)) {	/* Bell, abort */
			ctrlg(FALSE, 0);
			TTflush();
			return( (int (*)()) NULL);

		} else if (c == 0x7F || c == 0x08) {	/* rubout/erase */
			if (cpos != 0) {
				TTputc('\b');
				TTputc(' ');
				TTputc('\b');
				--ttcol;
				--cpos;
				TTflush();
			}

		} else if (c == 0x15) {	/* C-U, kill */
			while (cpos != 0) {
				TTputc('\b');
				TTputc(' ');
				TTputc('\b');
				--cpos;
				--ttcol;
			}

			TTflush();

		} else if (c == ' ') {
/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
	/* attempt a completion */
	buf[cpos] = 0;		/* terminate it for us */
	ffp = &names[0];	/* scan for matches */
	while (ffp->n_func != NULL) {
		if (strncmp(buf, ffp->n_name, strlen(buf)) == 0) {
			/* a possible match! More than one? */
			if ((ffp + 1)->n_func == NULL ||
			   (strncmp(buf, (ffp+1)->n_name, strlen(buf)) != 0)) {
				/* no...we match, print it */
				sp = ffp->n_name + cpos;
				while (*sp)
					TTputc(*sp++);
				TTflush();
				return(ffp->n_func);
			} else {
/* << << << << << << << << << << << << << << << << << */
	/* try for a partial match against the list */

	/* first scan down until we no longer match the current input */
	lffp = (ffp + 1);
	while ((lffp+1)->n_func != NULL) {
		if (strncmp(buf, (lffp+1)->n_name, strlen(buf)) != 0)
			break;
		++lffp;
	}

	/* and now, attempt to partial complete the string, char at a time */
	while (TRUE) {
		/* add the next char in */
		buf[cpos] = ffp->n_name[cpos];

		/* scan through the candidates */
		cffp = ffp + 1;
		while (cffp <= lffp) {
			if (cffp->n_name[cpos] != buf[cpos])
				goto onward;
			++cffp;
		}

		/* add the character */
		TTputc(buf[cpos++]);
	}
/* << << << << << << << << << << << << << << << << << */
			}
		}
		++ffp;
	}

	/* no match.....beep and onward */
	TTbeep();
onward:;
	TTflush();
/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
		} else {
			if (cpos < NSTRING-1 && c > ' ') {
				buf[cpos++] = c;
				TTputc(c);
			}

			++ttcol;
			TTflush();
		}
	}
}

#if	BCOMPL
/*
 * Read in the name of an buffer using TENEX style expansion and
 * return the pointer to it.  If argument 'createf' is non-zero, a new
 * buffer will be created, if requested.  'prompt' can be NULL, in
 * which case neither a prompt or the default is printed in the
 * minibuffer.  'def_name' may be a default buffer name or NULL.  If
 * 'prompt' is present, it will be printed in the minibuffer and can
 * be chosen by simply hitting RETURN.
 *
 * returns NULL, iff the user ABORTs or can't / isn't allowed to
 * create a new buffer.
 */

BUFFER *tenexbuf(createf, prompt, def_bname)

int createf;
char *prompt, *def_bname;

{
	register int cpos;	/* current column on screen output */
	register int c;
	char bufname[NBUFN];	/* buffer to hold tentative buffer name */

	/* starting at the beginning of the string buffer */
	cpos = 0;

	/* if we are executing a command line get the next arg and match it */
	if (clexec) {
		if (macarg(bufname) != TRUE)
			return(NULL);
		return(bfind(&bufname[0], createf, 0));
	}

	if (prompt)
		mlwrite ("%s[%s]: ", prompt,
			 def_bname ? def_bname : "no default");

	/* build a name string from the keyboard */
	while (TRUE) {
		c = tgetc();

		/* if we are at the end, just match it */
		if (c == '\n'  ||  c == '\r') {
			if (def_bname && cpos==0)
				return(bfind(def_bname, createf, 0));
			else {
				bufname[cpos] = 0;
				return(bfind(&bufname[0], createf, 0));
			}

		} else if (c == ectoc(abortc)) {	/* Bell, abort */
			ctrlg(FALSE, 0);
			TTflush();
			return NULL;

		} else if (c == 0x7F || c == 0x08) {	/* rubout/erase */
			if (cpos != 0) {
				TTputc('\b');
				TTputc(' ');
				TTputc('\b');
				--ttcol;
				--cpos;
				TTflush();
			}

		} else if (c == 0x15) {	/* C-U, kill */
			while (cpos != 0) {
				TTputc('\b');
				TTputc(' ');
				TTputc('\b');
				--cpos;
				--ttcol;
			}

			TTflush();

		} else if (c == ' ') {		/* attempt a completion */

			BUFFER *bp, *expbuf;
			int exp_pos, i;
			
			bufname[cpos] = 0;	/* terminate it for us */
			/*
			 * Search for the next common expansion.  expbuf points
			 * to the first matching buffer, exp_pos gets
			 * initialized to the full buffer name.  Each further
			 * matching buffer will shrink (via exp_pos) the most
			 * common name prefix.
			 */
			for (expbuf=NULL, exp_pos=0, bp = bheadp;
			     bp;
			     bp = bp->b_bufp) {
				if ( strlen (bp->b_bname) > cpos+1  &&
				     0==strncmp (bufname, bp->b_bname, cpos)) {
					/*
					 * If you want a more
					 * 'agressive' completion, you
					 * could count the matches
					 * right here after this
					 * comment.  Then, if we found
					 * exactly one matching
					 * buffer, we could directly
					 * switch to it (expbuf).
					 */
					if ( ! expbuf ) {
						/* install first match */
						expbuf = bp;
						exp_pos = strlen
							(expbuf->b_bname) -1;
						continue;
					}
					/* another match; reduce prefix size */
					i=cpos;
					while (i<=exp_pos)
						if (bp->b_bname[i] !=
						    expbuf->b_bname[i])
							break;
						else
							++i;
					exp_pos = i-1;
				}
			}

			if (!expbuf) {
				TTbeep();
				TTflush();
				continue;
			}
			
			while (cpos <= exp_pos) {
				/* add the next char in */
				bufname[cpos] = expbuf->b_bname[cpos];
				TTputc(bufname[cpos++]);
			}
			TTflush();
		} else {
			if (cpos < NBUFN-1 && c > ' ') {
				bufname[cpos++] = c;
				TTputc(c);
			}

			++ttcol;
			TTflush();
		}
	}
}
#endif

/*	tgetc:	Get a key from the terminal driver, resolve any keyboard
		macro action					*/

int tgetc()

{
	int c;	/* fetched character */

	/* if we are playing a keyboard macro back, */
	if (kbdmode == PLAY) {

		/* if there is some left... */
		if (kbdptr < kbdend)
			return((int)*kbdptr++);

		/* at the end of last repitition? */
		if (--kbdrep < 1) {
			kbdmode = STOP;
#if	VISMAC == 0
			/* force a screen update after all is done */
			update(FALSE);
#endif
		} else {

			/* reset the macro to the begining for the next rep */
			kbdptr = &kbdm[0];
			return((int)*kbdptr++);
		}
	}

	/* fetch a character from the terminal driver */
	c = TTgetc();

	/* record it for $lastkey */
	lastkey = c;

	/* save it if we need to */
	if (kbdmode == RECORD) {
		*kbdptr++ = c;
		kbdend = kbdptr;

		/* don't overrun the buffer */
		if (kbdptr == &kbdm[NKBDM - 1]) {
			kbdmode = STOP;
			TTbeep();
		}
	}

	/* and finally give the char back */
	return(c);
}

/*	GET1KEY:	Get one keystroke. The only prefixs legal here
			are the SPEC and CTRL prefixes.
								*/

#if	DECEDT
int save1key = 0;
#endif

#if	VMS
extern int tacnt;
#endif

get1key()

{
	int    c, code, i;
#if	AMIGA
	int	d;
#endif

	/* get a keystroke */
#if	DECEDT
	if (save1key) {c = save1key; save1key = 0;} else
#endif
        c = tgetc();

#if	MSDOS | ST520
	if (c == 0) {				/* Apply SPEC prefix	*/
	        c = tgetc();
	        if (c>=0x00 && c<=0x1F)		/* control key? */
        	        c = CTRL | (c+'@');
		return(SPEC | c);
	}
#endif

#if	DECEDT
	/* catch arrow keys and function keys */
	if (c == 27) {
#if	TYPEAH & (VMS | BSD | MSDOS | (USG & MSC))
		if (kbdmode != PLAY && !vt100keys) {
#if	VMS
			tacnt = 0;
#endif
			if (!typahead()) return(CTRL | (c+'@'));
		}
#endif
		save1key = tgetc();
		if (save1key == '[' || save1key == 'O') {
			save1key = 0; c = tgetc();
			if (c >= '0' && c <= '9') {
				code = 0;
				for(i = 0; i < 3 && c >= '0' && c <= '9'; i++)
					{code = code*10+c-'0'; c = tgetc();}
				if (c == 27) save1key=27;
				c = code;
			        if (c>=0x00 && c<=0x1F) c = CTRL | (c+'@');
			}
			return(SPEC | c);
		}
	}
#endif

#if	AMIGA
	/* apply SPEC prefix */
	if ((unsigned)c == 155) {
		c = tgetc();

		/* first try to see if it is a cursor key */
		if ((c >= 'A' && c <= 'D') || c == 'S' || c == 'T')
			return(SPEC | c);

		/* next, a 2 char sequence */
		d = tgetc();
		if (d == '~')
			return(SPEC | c);

		/* decode a 3 char sequence */
		c = d + 32;
		/* if a shifted function key, eat the tilde */
		if (d >= '0' && d <= '9')
			d = tgetc();
		return(SPEC | c);
	}
#endif

#if  WANGPC
	if (c == 0x1F) {			/* Apply SPEC prefix    */
	        c = tgetc();
		return(SPEC | c);
	}
#endif

        if (c>=0x00 && c<=0x1F)                 /* C0 control -> C-     */
                c = CTRL | (c+'@');
        return (c);
}

/*	GETCMD:	Get a command from the keyboard. Process all applicable
		prefix keys
							*/

extern int inmeta, incex, inspecial;

getcmd()

{
	int c;		/* fetched keystroke */
	int oldmeta;
	int code, i;

	/* get initial character */
	c = get1key();

	/* process prefixes */
	if (inmeta == TRUE || c == metac ||
	    incex == TRUE || c == ctlxc) {
		oldmeta = inmeta;
		if (c == ctlxc) {c = get1key(); incex = TRUE;}
		if (c == metac) {
			c = get1key(); inmeta = TRUE;
			if (oldmeta == FALSE && c == metac)
				{c = get1key(); oldmeta = TRUE;}
		}
	        if (islower(c))		/* Force to upper */
        	        c ^= DIFCASE;
	        if (c>=0x00 && c<=0x1F)		/* control key */
	        	c = CTRL | (c+'@');
		/* temporary ESC sequence fix......... */
		if (inmeta == TRUE && (c == '[' || c == 'O')) {
			c = get1key();
			if (c >= '0' && c <= '9') {
				code = 0;
				for(i = 0; i < 3 && c >= '0' && c <= '9'; i++)
					{code = code*10+c-'0'; c = get1key();}
#if DECEDT
				if (c == (CTRL|(27+'@'))) save1key=27;
#endif
				c = code;
			        if (c>=0x00 && c<=0x1F) c = CTRL | (c+'@');
			}
			inmeta = oldmeta;
			inspecial = TRUE;
		}
		if (inmeta == TRUE)	{inmeta = FALSE; c |= META;}
		if (incex == TRUE)	{incex = FALSE;	c |= CTLX;}
	}

	if (inspecial == TRUE)	{inspecial = FALSE; c |= SPEC;}

	/* otherwise, just return it */
	return(c);
}

/*	A more generalized prompt/reply function allowing the caller
	to specify the proper terminator. If the terminator is not
	a return ('\n') it will echo as "<NL>"
							*/
getstring(prompt, buf, nbuf, eolchar)

char *prompt;
char *buf;
int eolchar;

{
	register int cpos;	/* current character position in string */
	register int c;
	register int quotef;	/* are we quoting the next char? */
	int len;
	char *str, chbuf[2];
	char *getkill();

	cpos = 0;
	quotef = FALSE;

	/* prompt the user for the input string */
	mlwrite(prompt);

	for (;;) {
		/* get a character from the user */
		c = get1key();

		/* If it is a <ret>, change it to a <NL> */
		if (c == (CTRL | 0x4d) && quotef == FALSE)
			c = CTRL | 0x40 | '\n';

		/* if they hit the line terminate, wrap it up */
#if	DECEDT
		if (quotef == FALSE && (c & SPEC)) c = eolchar;
#endif
		if (c == eolchar && quotef == FALSE) {
			buf[cpos++] = 0;

			/* clear the message line */
			mlwrite("");
			TTflush();

			/* if we default the buffer, return FALSE */
			if (buf[0] == 0)
				return(FALSE);

			return(TRUE);
		}

		/* change from command form back to character form */
		c = ectoc(c);

		if (c == ectoc(abortc) && quotef == FALSE) {
			/* Abort the input? */
			ctrlg(FALSE, 0);
			TTflush();
			return(ABORT);
		} else if ((c==0x7F || c==0x08) && quotef==FALSE) {
			/* rubout/erase */
			if (cpos != 0) {
				outstring("\b \b");
				--ttcol;

				if ((buf[--cpos]&0x7f) < 0x20 ||
				    (buf[cpos]&0x7f) == 0x7f) {
					outstring("\b \b");
					--ttcol;
				}

				if (buf[cpos] == '\n') {
					outstring("\b\b  \b\b");
					ttcol -= 2;
				}
				TTflush();
			}

		} else if (c == 0x15 && quotef == FALSE) {
			/* C-U, kill */
			while (cpos != 0) {
				outstring("\b \b");
				--ttcol;

				if ((buf[--cpos]&0x7f) < 0x20 ||
				    (buf[cpos]&0x7f) == 0x7f) {
					outstring("\b \b");
					--ttcol;
				}

				if (buf[cpos] == '\n') {
					outstring("\b\b  \b\b");
					ttcol -= 2;
				}
			}
			TTflush();

		} else if (c == quotec && quotef == FALSE) {
			quotef = TRUE;
		} else {
			if (c == 0x19 && quotef == FALSE) {
				str = getkill();
				len = strlen(str);
			}
			else { str = chbuf; *chbuf = c; len = 1; }
			quotef = FALSE;

			while (cpos < nbuf-1 && len-- > 0) {
				c = buf[cpos++] = *str++;

				if (c == '\n') { /* put out <NL> for <ret> */
					outstring("<NL>");
					ttcol += 3;
				} else {
					if ((c&0x7f) < 0x20 ||
					    (c&0x7f) == 0x7f) {
						if (disinp) TTputc('^');
						++ttcol;
						c = ((c&0x7f) ^ 0x40);
					}
					if (disinp) TTputc(c);
				}
				++ttcol;
			}
			TTflush();
		}
	}
}

outstring(s)	/* output a string of input characters */

char *s;	/* string to output */

{
	if (disinp)
		while (*s)
			TTputc(*s++);
}

ostring(s)	/* output a string of output characters */

char *s;	/* string to output */

{
	if (discmd)
		while (*s)
			TTputc(*s++);
}

