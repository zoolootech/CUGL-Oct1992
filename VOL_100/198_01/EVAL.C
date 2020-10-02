/*	EVAL.C:	Expresion evaluation functions for
		MicroEMACS

	written 1986 by Daniel Lawrence				*/

#include	<stdio.h>
#include	"estruct.h"
#include	"edef.h"
#include	"evar.h"

varinit()		/* initialize the user variable list */

{
	register int i;

	for (i=0; i < MAXVARS; i++)
		uv[i].u_name[0] = 0;
}

char *gtfun(fname)	/* evaluate a function */

char *fname;		/* name of function to evaluate */

{
	register int fnum;		/* index to function to eval */
	register int arg;		/* value of some arguments */
	char arg1[NSTRING];		/* value of first argument */
	char arg2[NSTRING];		/* value of second argument */
	char arg3[NSTRING];		/* value of third argument */
	static char result[2 * NSTRING];	/* string result */
#if	ENVFUNC
	char *getenv();
#endif

	/* look the function up in the function table */
	fname[3] = 0;	/* only first 3 chars significant */
	mklower(fname);	/* and let it be upper or lower case */
	for (fnum = 0; fnum < NFUNCS; fnum++)
		if (strcmp(fname, funcs[fnum].f_name) == 0)
			break;

	/* return errorm on a bad reference */
	if (fnum == NFUNCS)
		return(errorm);

	/* if needed, retrieve the first argument */
	if (funcs[fnum].f_type >= MONAMIC) {
		if (macarg(arg1) != TRUE)
			return(errorm);

		/* if needed, retrieve the second argument */
		if (funcs[fnum].f_type >= DYNAMIC) {
			if (macarg(arg2) != TRUE)
				return(errorm);
	
			/* if needed, retrieve the third argument */
			if (funcs[fnum].f_type >= TRINAMIC)
				if (macarg(arg3) != TRUE)
					return(errorm);
		}
	}
		

	/* and now evaluate it! */
	switch (fnum) {
		case UFADD:	return(itoa(atoi(arg1) + atoi(arg2)));
		case UFSUB:	return(itoa(atoi(arg1) - atoi(arg2)));
		case UFTIMES:	return(itoa(atoi(arg1) * atoi(arg2)));
		case UFDIV:	return(itoa(atoi(arg1) / atoi(arg2)));
		case UFMOD:	return(itoa(atoi(arg1) % atoi(arg2)));
		case UFNEG:	return(itoa(-atoi(arg1)));
		case UFCAT:	bytecopy(result, arg1, NSTRING-1);
				bytecopy(&result[strlen(result)], arg2,
					NSTRING-1-strlen(result));
				return(result);
		case UFLEFT:	return(bytecopy(result, arg1, atoi(arg2)));
		case UFRIGHT:	arg = atoi(arg2);
				if (arg > strlen(arg1)) arg = strlen(arg1);
				return(strcpy(result,
					&arg1[strlen(arg1)-arg]));
		case UFMID:	arg = atoi(arg2);
				if (arg > strlen(arg1)) arg = strlen(arg1);
				return(bytecopy(result, &arg1[arg-1],
					atoi(arg3)));
		case UFNOT:	return(ltos(stol(arg1) == FALSE));
		case UFEQUAL:	return(ltos(atoi(arg1) == atoi(arg2)));
		case UFLESS:	return(ltos(atoi(arg1) < atoi(arg2)));
		case UFGREATER:	return(ltos(atoi(arg1) > atoi(arg2)));
		case UFSEQUAL:	return(ltos(strcmp(arg1, arg2) == 0));
		case UFSLESS:	return(ltos(strcmp(arg1, arg2) < 0));
		case UFSGREAT:	return(ltos(strcmp(arg1, arg2) > 0));
		case UFIND:	return(bytecopy(result, fixnull(getval(arg1)),
					NSTRING-1));
		case UFAND:	return(ltos(stol(arg1) && stol(arg2)));
		case UFOR:	return(ltos(stol(arg1) || stol(arg2)));
		case UFLENGTH:	return(itoa(strlen(arg1)));
		case UFUPPER:	return(mkupper(arg1));
		case UFLOWER:	return(mklower(arg1));
		case UFTRUTH:	return(ltos(atoi(arg1) == 42));
		case UFASCII:	return(itoa((int)arg1[0]));
		case UFCHR:	result[0] = atoi(arg1);
				result[1] = 0;
				return(result);
		case UFGTCMD:	cmdstr(getcmd(), result);
				return(result);
		case UFGTKEY:	result[0] = tgetc();
				result[1] = 0;
				return(result);
		case UFRND:	return(itoa((ernd() % abs(atoi(arg1))) + 1));
		case UFABS:	return(itoa(abs(atoi(arg1))));
		case UFSINDEX:	return(itoa(sindex(arg1, arg2)));
		case UFENV:
#if	ENVFUNC
				return(fixnull(getenv(arg1)));
#else
				return("");
#endif
		case UFBIND:	return(transbind(arg1));
 		case UFBAND:	return(int_asc(asc_int(arg1) & asc_int(arg2)));
 		case UFBOR:	return(int_asc(asc_int(arg1) | asc_int(arg2)));
 		case UFBXOR:	return(int_asc(asc_int(arg1) ^ asc_int(arg2)));
		case UFBNOT:	return(int_asc(~asc_int(arg1)));
	}

	exit(-11);	/* never should get here */
}

char *gtusr(vname)	/* look up a user var's value */

char *vname;		/* name of user variable to fetch */

{

	register int vnum;	/* ordinal number of user var */

	/* scan the list looking for the user var name */
	for (vnum = 0; vnum < MAXVARS; vnum++)
		if (strcmp(vname, uv[vnum].u_name) == 0)
			break;

	/* return errorm on a bad reference */
	if (vnum == MAXVARS)
		return(errorm);

	return(uv[vnum].u_value);
}

#if	DECEDT
extern int advset;
#endif

char *gtenv(vname)

char *vname;		/* name of environment variable to retrieve */

{
	register int vnum;	/* ordinal number of var refrenced */
	char *getkill();

	/* scan the list, looking for the referenced name */
	for (vnum = 0; vnum < NEVARS; vnum++)
		if (strcmp(vname, envars[vnum]) == 0)
			break;

	/* return errorm on a bad reference */
	if (vnum == NEVARS)
		return(errorm);

	/* otherwise, fetch the appropriate value */
	switch (vnum) {
		case EVFILLCOL:	return(itoa(fillcol));
		case EVPAGELEN:	return(itoa(term.t_nrow + 1));
		case EVCURCOL:	return(itoa(getccol(FALSE)));
		case EVCURLINE: return(itoa(getcline()));
		case EVRAM:	return(itoa((int)(envram / 1024l)));
		case EVFLICKER:	return(ltos(flickcode));
		case EVCURWIDTH:return(itoa(term.t_nrow));
		case EVCBUFNAME:return(curbp->b_bname);
		case EVCBFLAGS:	return(itoa(curbp->b_flag));
		case EVCFNAME:	return(curbp->b_fname);
		case EVSRES:	return(sres);
		case EVDEBUG:	return(ltos(macbug));
		case EVSTATUS:	return(ltos(cmdstatus));
		case EVPALETTE:	return(palstr);
		case EVASAVE:	return(itoa(gasave));
		case EVACOUNT:	return(itoa(gacount));
		case EVLASTKEY: return(itoa(lastkey));
		case EVCURCHAR:
			return(curwp->w_dotp->l_used ==
					curwp->w_doto ? itoa('\n') :
				itoa(lgetc(curwp->w_dotp, curwp->w_doto)));
		case EVDISCMD:	return(ltos(discmd));
		case EVVERSION:	return(VERSION);
		case EVPROGNAME:return(PROGNAME);
		case EVSEED:	return(itoa(seed));
		case EVDISINP:	return(ltos(disinp));
		case EVWLINE:	return(itoa(curwp->w_ntrows));
		case EVCWLINE:	return(itoa(getwpos()));
		case EVTARGET:	saveflag = lastflag;
				return(itoa(curgoal));
		case EVSEARCH:	return(pat);
		case EVREPLACE:	return(rpat);
		case EVMATCH:	return((patmatch == NULL)? "": patmatch);
		case EVKILL:	return(getkill());
		case EVCMODE:	return(itoa(curbp->b_mode));
		case EVGMODE:	return(itoa(gmode));
		case EVTPAUSE:	return(itoa(term.t_pause));
		case EVPENDING:
#if	TYPEAH
				return(ltos(typahead()));
#else
				return(falsem);
#endif
		case EVLWIDTH:	return(itoa(llength(curwp->w_dotp)));
		case EVLINE:	return(getctext());
		case EVHARDTAB:	return(int_asc(tabsize));
		case EVSOFTTAB:	return(int_asc(stabsize));
		case EVFCOL:	return(itoa(curwp->w_fcol));
		case EVHSCROLL:	return(ltos(hscroll));
		case EVHJUMP:	return(int_asc(hjump));
		case EVADVANCE:
#if	DECEDT
			return(itoa(advset));
#else
			return(itoa(1));
#endif
		case EVVT100KEYS: return(itoa(vt100keys));
	}
	exit(-12);	/* again, we should never get here */
}

char *fixnull(s)	/* Don't return NULL pointers! */

char *s;

{
	if (s == NULL)
		return("");
	else
		return(s);
}
char *getkill()		/* return some of the contents of the kill buffer */

{
	register int size;	/* max number of chars to return */
	static char value[NSTRING];	/* temp buffer for value */

	if (kbufh == NULL)
		/* no kill buffer....just a null string */
		value[0] = 0;
	else {
		/* copy in the contents... */
		if (kused < NSTRING)
			size = kused;
		else
			size = NSTRING - 1;
		bytecopy(value, kbufh->d_chunk, size);
	}

	/* and return the constructed value */
	return(value);
}

int setvar(f, n)		/* set a variable */

int f;		/* default flag */
int n;		/* numeric arg (can overide prompted value) */

{
	register int status;	/* status return */
	VDESC vd;		/* variable num/type */
	char var[NVSIZE+1];	/* name of variable to fetch */
	char value[NSTRING];	/* value to set variable to */

	/* first get the variable to set.. */
	if (clexec == FALSE) {
		status = mlreply("Variable to set: ", &var[0], NVSIZE);
		if (status != TRUE)
			return(status);
	} else {	/* macro line argument */
		/* grab token and skip it */
		execstr = token(execstr, var, NVSIZE+1);
	}

	/* check the legality and find the var */
	findvar(var, &vd, NVSIZE+1);
	
	/* if its not legal....bitch */
	if (vd.v_type == -1) {
		mlwrite("%%No such variable as '%s'", var);
		return(FALSE);
	}

	/* get the value for that variable */
	if (f == TRUE)
		strcpy(value, itoa(n));
	else {
		status = mlreply("Value: ", &value[0], NSTRING);
		if (status != TRUE)
			return(status);
	}

	/* and set the appropriate value */
	status = svar(&vd, value);

#if	DEBUGM
	/* if $debug == TRUE, every assignment will echo a statment to
	   that effect here. */
	
	if (macbug) {
		strcpy(outline, "(((");

		/* assignment status */
		strcat(outline, ltos(status));
		strcat(outline, ":");

		/* variable name */
		strcat(outline, var);
		strcat(outline, ":");

		/* and lastly the value we tried to assign */
		bytecopy(&outline[strlen(outline)], value, NSTRING-NVSIZE-20);
		strcat(outline, ")))");

		/* expand '%' to "%%" so mlwrite wont bitch */
		makelit(outline, NSTRING);

		/* write out the debug line */
		mlforce(outline);
		update(TRUE);

		/* and get the keystroke to hold the output */
		if (get1key() == abortc) {
			mlforce("[Macro aborted]");
			status = FALSE;
		}
	}
#endif

	/* and return it */
	return(status);
}

findvar(var, vd, size)	/* find a variables type and name */

char *var;	/* name of var to get */
VDESC *vd;	/* structure to hold type and ptr */
int size;	/* size of var array */

{
	register int vnum;	/* subscript in varable arrays */
	register int vtype;	/* type to return */

fvar:	vtype = -1;
	switch (var[0]) {

		case '$': /* check for legal enviromnent var */
			for (vnum = 0; vnum < NEVARS; vnum++)
				if (strcmp(&var[1], envars[vnum]) == 0) {
					vtype = TKENV;
					break;
				}
			break;

		case '%': /* check for existing legal user variable */
			for (vnum = 0; vnum < MAXVARS; vnum++)
				if (strcmp(&var[1], uv[vnum].u_name) == 0) {
					vtype = TKVAR;
					break;
				}
			if (vnum < MAXVARS)
				break;

			/* create a new one??? */
			for (vnum = 0; vnum < MAXVARS; vnum++)
				if (uv[vnum].u_name[0] == 0) {
					vtype = TKVAR;
					bytecopy(uv[vnum].u_name,
							&var[1], NVSIZE);
					break;
				}
			break;

		case '&':	/* indirect operator? */
			var[4] = 0;
			if (strcmp(&var[1], "ind") == 0) {
				/* grab token, and eval it */
				execstr = token(execstr, var, size);
				bytecopy(var, fixnull(getval(var)), size);
				goto fvar;
			}
	}

	/* return the results */
	vd->v_num = vnum;
	vd->v_type = vtype;
	return;
}

int svar(var, value)		/* set a variable */

VDESC *var;	/* variable to set */
char *value;	/* value to set to */

{
	register int vnum;	/* ordinal number of var refrenced */
	register int vtype;	/* type of variable to set */
	register int status;	/* status return */
	register int c;		/* translated character */
	register char * sp;	/* scratch string pointer */

	/* simplify the vd structure (we are gonna look at it a lot) */
	vnum = var->v_num;
	vtype = var->v_type;

	/* and set the appropriate value */
	status = TRUE;
	switch (vtype) {
	case TKVAR: /* set a user variable */
		if (uv[vnum].u_value != NULL)
			free(uv[vnum].u_value);
		sp = malloc(strlen(value) + 1);
		if (sp == NULL)
			return(FALSE);
		strcpy(sp, value);
		uv[vnum].u_value = sp;
		break;

	case TKENV: /* set an environment variable */
		status = TRUE;	/* by default */
		switch (vnum) {
		case EVFILLCOL:	fillcol = atoi(value);
				break;
		case EVPAGELEN:	status = newsize(TRUE, atoi(value));
				break;
		case EVCURCOL:	status = setccol(atoi(value));
				break;
		case EVCURLINE:	status = gotoline(TRUE, atoi(value));
				break;
		case EVRAM:	break;
		case EVFLICKER:	flickcode = stol(value);
				break;
		case EVCURWIDTH:status = newwidth(TRUE, atoi(value));
				break;
		case EVCBFLAGS:	curbp->b_flag =
				 ((curbp->b_flag & ~(BFCHG|BFINVS|BFTRUNC)) |
				  (atoi(value) & (BFCHG|BFINVS|BFTRUNC)));
				upmode();
				break;
		case EVCBUFNAME:bytecopy(curbp->b_bname, value, NBUFN);
				upmode();
				break;
		case EVCFNAME:	bytecopy(curbp->b_fname, value, NFILEN);
				upmode();
				break;
		case EVSRES:	status = TTrez(value);
				break;
		case EVDEBUG:	macbug = stol(value);
				break;
		case EVSTATUS:	cmdstatus = stol(value);
				break;
		case EVPALETTE:	bytecopy(palstr, value, 48);
				spal(palstr);
				break;
		case EVASAVE:	gasave = atoi(value);
				break;
		case EVACOUNT:	gacount = atoi(value);
				break;
		case EVLASTKEY:	lastkey = atoi(value);
				break;
		case EVCURCHAR:	ldelete(1L, FALSE);	/* delete 1 char */
				c = atoi(value);
				if (c == '\n')
					lnewline();
				else
					linsert(1, c);
				backchar(FALSE, 1);
				break;
		case EVDISCMD:	discmd = stol(value);
				break;
		case EVVERSION:	break;
		case EVPROGNAME:break;
		case EVSEED:	seed = atoi(value);
				break;
		case EVDISINP:	disinp = stol(value);
				break;
		case EVWLINE:	status = resize(TRUE, atoi(value));
				break;
		case EVCWLINE:	status = forwline(TRUE,
						atoi(value) - getwpos());
				break;
		case EVTARGET:	curgoal = atoi(value);
				thisflag = saveflag;
				break;
		case EVSEARCH:	strcpy(pat, value);
				setjtable(pat);
#if	MAGIC
				mcclear();
#endif
				break;
		case EVREPLACE:	strcpy(rpat, value);
				break;
		case EVMATCH:	break;
		case EVKILL:	break;
		case EVCMODE:	curbp->b_mode = atoi(value);
				curwp->w_flag |= WFMODE;
				break;
		case EVGMODE:	gmode = atoi(value);
				break;
		case EVTPAUSE:	term.t_pause = atoi(value);
				break;
		case EVPENDING:	break;
		case EVLWIDTH:	break;
		case EVLINE:	putctext(value); break;
		case EVHARDTAB:	tabsize = asc_int(value);
				if (tabsize <= 0) tabsize = 1;
				upwind();
				break;
		case EVSOFTTAB:	stabsize = asc_int(value);
				upwind();
				break;
		case EVFCOL:	curwp->w_fcol = atoi(value);
				if (curwp->w_fcol < 0)
					curwp->w_fcol = 0;
				curwp->w_flag |= WFHARD | WFMODE;
				break;
		case EVHSCROLL:	hscroll = stol(value);
				lbound = 0;
				break;
		case EVHJUMP:	hjump = asc_int(value);
				if (hjump < 1)
					hjump = 1;
				if (hjump > term.t_ncol - 1)
					hjump = term.t_ncol - 1;
				break;
		case EVADVANCE:
#if	DECEDT
				advset = atoi(value);
#endif
				break;
		case EVVT100KEYS: vt100keys = atoi(value); break;
		}
		break;
	}
	return(status);
}

/* atoi and itoa defined to asc_int and int_asc in estruct.h */

/*	asc_int:	ascii string to integer......This is too
		inconsistant to use the system's	*/

atoi(st)

char *st;

{
	int result;	/* resulting number */
	int sign;	/* sign of resulting number */
	char c;		/* current char being examined */

	result = 0;
	sign = 1;

	/* skip preceding whitespace */
	while (*st == ' ' || *st == '\t')
		++st;

	/* check for sign */
	if (*st == '-') {
		sign = -1;
		++st;
	}
	if (*st == '+')
		++st;

	/* scan digits, build value */
	while ((c = *st++) != '\0')
		if (c >= '0' && c <= '9')
			result = result * 10 + c - '0';
		else
			break;

	return(result * sign);
}

/*	int_asc:	integer to ascii string.......... This is too
			inconsistant to use the system's	*/

char *itoa(i)

int i;	/* integer to translate to a string */

{
	register int digit;		/* current digit being used */
	register char *sp;		/* pointer into result */
	register int sign;		/* sign of resulting number */
	static char result[INTWIDTH+1];	/* resulting string */

	/* record the sign...*/
	sign = 1;
	if (i < 0) {
		sign = -1;
		i = -i;
	}

	/* and build the string (backwards!) */
	sp = result + INTWIDTH;
	*sp = 0;
	do {
		digit = i % 10;
		*(--sp) = '0' + digit;	/* and install the new digit */
		i = i / 10;
	} while (i);

	/* and fix the sign */
	if (sign == -1) {
		*(--sp) = '-';	/* and install the minus sign */
	}

	return(sp);
}

int gettyp(tok)	/* find the type of a passed token */

char *tok;	/* token to analyze */

{
	register char c;	/* first char in token */

	/* grab the first char (this is all we need) */
	c = *tok;

	/* no blanks!!! */
	if (c == 0)
		return(TKNUL);

	/* a numeric literal? */
	if (c >= '0' && c <= '9')
		return(TKLIT);

	switch (c) {
		case '"':	return(TKSTR);

		case '!':	return(TKDIR);
		case '@':	return(TKARG);
		case '#':	return(TKBUF);
		case '$':	return(TKENV);
		case '%':	return(TKVAR);
		case '&':	return(TKFUN);
		case '*':	return(TKLBL);

		default:	return(TKCMD);
	}
}

char *getval(tok)	/* find the value of a token */

char *tok;		/* token to evaluate */

{
	register int status;	/* error return */
	register BUFFER *bp;	/* temp buffer pointer */
	register int blen;	/* length of buffer argument */
	register int distmp;	/* temporary discmd flag */
	char buf[NSTRING];	/* string buffer for some returns */

	switch (gettyp(tok)) {
		case TKNUL:	return("");

		case TKARG:	/* interactive argument */
				strcpy(tok, getval(&tok[1]));
				distmp = discmd;	/* echo it always! */
				discmd = TRUE;
				status = getstring(tok,
					   buf, NSTRING, ctoec('\n'));
				discmd = distmp;
				if (status == ABORT)
					return(errorm);
				return(buf);

		case TKBUF:	/* buffer contents fetch */

				/* grab the right buffer */
				strcpy(tok, getval(&tok[1]));
				bp = bfind(tok, FALSE, 0);
				if (bp == NULL)
					return(errorm);
		
				/* if the buffer is displayed, get the window
				   vars instead of the buffer vars */
				if (bp->b_nwnd > 0) {
					curbp->b_dotp = curwp->w_dotp;
					curbp->b_doto = curwp->w_doto;
				}

				/* make sure we are not at the end */
				if (bp->b_linep == bp->b_dotp)
					return(errorm);
		
				/* grab the line as an argument */
				blen = bp->b_dotp->l_used - bp->b_doto;
				if (blen >= NSTRING)
					blen = NSTRING-1;
				bytecopy(buf, bp->b_dotp->l_text + bp->b_doto,
					blen);
				buf[blen] = 0;
		
				/* and step the buffer's line ptr ahead a line */
				bp->b_dotp = bp->b_dotp->l_fp;
				bp->b_doto = 0;

				/* if displayed buffer, reset window ptr vars*/
				if (bp->b_nwnd > 0) {
					curwp->w_dotp = curbp->b_dotp;
					curwp->w_doto = 0;
					curwp->w_flag |= WFMOVE;
				}

				/* and return the spoils */
				return(buf);		

		case TKVAR:	return(gtusr(tok+1));
		case TKENV:	return(gtenv(tok+1));
		case TKFUN:	return(gtfun(tok+1));
		case TKDIR:	return(errorm);
		case TKLBL:	return(itoa(gtlbl(tok)));
		case TKLIT:	return(tok);
		case TKSTR:	return(tok+1);
		case TKCMD:	return(tok);
	}
}

gtlbl(tok)	/* find the line number of the given label */

char *tok;	/* label name to find */

{
	return(1);
}

int stol(val)	/* convert a string to a numeric logical */

char *val;	/* value to check for stol */

{
	/* check for logical values */
	if (val[0] == 'F' || val[0] == 'f' || val[0] == 'N' || val[0] == 'n')
		return(FALSE);
	if (val[0] == 'T' || val[0] == 't' || val[0] == 'Y' || val[0] == 'y')
		return(TRUE);

	/* check for numeric truth (!= 0) */
	return((atoi(val) != 0));
}

char *ltos(val)		/* numeric logical to string logical */

int val;	/* value to translate */

{
	if (val)
		return(truem);
	else
		return(falsem);
}

char *mkupper(str)	/* make a string upper case */

char *str;		/* string to upper case */

{
	char *sp;

	sp = str;
	while (*sp) {
		if ('a' <= *sp && *sp <= 'z')
			*sp += 'A' - 'a';
		++sp;
	}
	return(str);
}

char *mklower(str)	/* make a string lower case */

char *str;		/* string to lower case */

{
	char *sp;

	sp = str;
	while (*sp) {
		if ('A' <= *sp && *sp <= 'Z')
			*sp += 'a' - 'A';
		++sp;
	}
	return(str);
}

#if	(MSC & MSDOS) | VMS
#else
int abs(x)	/* take the absolute value of an integer */

int x;

{
	return(x < 0 ? -x : x);
}
#endif

int ernd()	/* returns a random integer */

{
	seed = abs(seed * 1721 + 10007);
	return(seed);
}

int sindex(source, pattern)	/* find pattern within source */

char *source;	/* source string to search */
char *pattern;	/* string to look for */

{
	char *sp;	/* ptr to current position to scan */
	char *csp;	/* ptr to source string during comparison */
	char *cp;	/* ptr to place to check for equality */

	/* scanning through the source string */
	sp = source;
	while (*sp) {
		/* scan through the pattern */
		cp = pattern;
		csp = sp;
		while (*cp) {
			if (!eq(*cp, *csp))
				break;
			++cp;
			++csp;
		}

		/* was it a match? */
		if (*cp == 0)
			return((int)(sp - source) + 1);
		++sp;
	}

	/* no match at all.. */
	return(0);
}

#if	DEBUGM
int dispvar(f, n)		/* display a variable's value */

int f;		/* default flag */
int n;		/* numeric arg (can overide prompted value) */

{
	register int status;	/* status return */
	VDESC vd;		/* variable num/type */
	char var[NVSIZE+1];	/* name of variable to fetch */

	/* first get the variable to dispaly.. */
	if (clexec == FALSE) {
		status = mlreply("Variable to display: ", &var[0], NVSIZE+1);
		if (status != TRUE)
			return(status);
	} else {	/* macro line argument */
		/* grab token and skip it */
		execstr = token(execstr, var, NVSIZE + 1);
	}

	/* check the legality and find the var */
	findvar(var, &vd, NVSIZE + 1);
	
	/* if its not legal....bitch */
	if (vd.v_type == -1) {
		mlwrite("%%No such variable as '%s'", var);
		return(FALSE);
	}

	/* and display the value */
	strcpy(outline, var);
	strcat(outline, " = ");

	/* and lastly the current value */
	strcat(outline, fixnull(getval(var)));

	/* expand '%' to "%%" so mlwrite wont bitch */
	makelit(outline, NSTRING);

	/* write out the result */
	mlforce(outline);
	update(TRUE);

	/* and return */
	return(TRUE);
}

/*	describe-variables	Bring up a fake buffer and list the contents
				of all the environment variables
*/

desvars(f, n)

{
	register WINDOW *wp;	/* scanning pointer to windows */
	register BUFFER *bp;	/* buffer to put binding list into */
	register int uindex;	/* index into uvar table */
	char outseq[NSTRING+NVSIZE+20];	/* output buffer for keystroke sequence */

	/* split the current window to make room for the variable list */
	if (splitwind(FALSE, 1) == FALSE)
		return(FALSE);
	curwp = spltwp; curbp = curwp->w_bufp;

	/* and get a buffer for it */
	bp = bfind("Variable list", TRUE, 0);
	if (bp == NULL || bclear(bp) == FALSE) {
		mlwrite("Can not display variable list");
		return(FALSE);
	}

	/* let us know this is in progress */
	mlwrite("[Building variable list]");

	/* disconect the current buffer */
        if (--curbp->b_nwnd == 0) {             /* Last use.            */
                curbp->b_dotp  = curwp->w_dotp;
                curbp->b_doto  = curwp->w_doto;
                curbp->b_markp = curwp->w_markp;
                curbp->b_marko = curwp->w_marko;
		curbp->b_fcol  = curwp->w_fcol;
        }

	/* connect the current window to this buffer */
	curbp = bp;	/* make this buffer current in current window */
	bp->b_mode = 0;		/* no modes active in binding list */
	bp->b_nwnd++;		/* mark us as more in use */
	wp = curwp;
	wp->w_bufp = bp;
	wp->w_linep = bp->b_linep;
	wp->w_flag = WFHARD|WFFORCE;
	wp->w_dotp = bp->b_dotp;
	wp->w_doto = bp->b_doto;
	wp->w_markp = NULL;
	wp->w_marko = 0;

	/* build the environment variable list */
	for (uindex = 0; uindex < NEVARS; uindex++) {

		/* add in the environment variable name */
		strcpy(outseq, "$");
		strcat(outseq, envars[uindex]);
		pad(outseq, 14);
		
		/* add in the value */
		bytecopy(&outseq[strlen(outseq)],
				gtenv(envars[uindex]), NSTRING);
		strcat(outseq, "\n");

		/* and add it as a line into the buffer */
		if (linstr(outseq) != TRUE)
			return(FALSE);
	}

	linstr("\n");

	/* build the user variable list */
	for (uindex = 0; uindex < MAXVARS; uindex++) {
		if (uv[uindex].u_name[0] == 0)
			break;

		/* add in the user variable name */
		strcpy(outseq, "%");
		strcat(outseq, uv[uindex].u_name);
		pad(outseq, 14);
		
		/* add in the value */
		bytecopy(&outseq[strlen(outseq)],
				uv[uindex].u_value, NSTRING);
		strcat(outseq, "\n");

		/* and add it as a line into the buffer */
		if (linstr(outseq) != TRUE)
			return(FALSE);
	}

	curwp->w_bufp->b_mode |= MDVIEW;/* put this buffer view mode */
	curbp->b_flag &= ~BFCHG;	/* don't flag this as a change */
	wp->w_dotp = lforw(bp->b_linep);/* back to the beginning */
	wp->w_doto = 0;
	upmode();
	mlwrite("");	/* clear the mode line */
	return(TRUE);
}

/*	describe-functions	Bring up a fake buffer and list the
				names of all the functions
*/

desfunc(f, n)

{
	register WINDOW *wp;	/* scanning pointer to windows */
	register BUFFER *bp;	/* buffer to put binding list into */
	register int uindex;	/* index into funcs table */
	char outseq[80];	/* output buffer for keystroke sequence */

	/* split the current window to make room for the variable list */
	if (splitwind(FALSE, 1) == FALSE)
		return(FALSE);
	curwp = spltwp; curbp = curwp->w_bufp;

	/* and get a buffer for it */
	bp = bfind("Function list", TRUE, 0);
	if (bp == NULL || bclear(bp) == FALSE) {
		mlwrite("Can not display function list");
		return(FALSE);
	}

	/* let us know this is in progress */
	mlwrite("[Building function list]");

	/* disconect the current buffer */
        if (--curbp->b_nwnd == 0) {             /* Last use.            */
                curbp->b_dotp  = curwp->w_dotp;
                curbp->b_doto  = curwp->w_doto;
                curbp->b_markp = curwp->w_markp;
                curbp->b_marko = curwp->w_marko;
		curbp->b_fcol  = curwp->w_fcol;
        }

	/* connect the current window to this buffer */
	curbp = bp;	/* make this buffer current in current window */
	bp->b_mode = 0;		/* no modes active in binding list */
	bp->b_nwnd++;		/* mark us as more in use */
	wp = curwp;
	wp->w_bufp = bp;
	wp->w_linep = bp->b_linep;
	wp->w_flag = WFHARD|WFFORCE;
	wp->w_dotp = bp->b_dotp;
	wp->w_doto = bp->b_doto;
	wp->w_markp = NULL;
	wp->w_marko = 0;

	/* build the function list */
	for (uindex = 0; uindex < NFUNCS; uindex++) {

		/* add in the environment variable name */
		strcpy(outseq, "&");
		strcat(outseq, funcs[uindex].f_name);
		strcat(outseq, "\n");

		/* and add it as a line into the buffer */
		if (linstr(outseq) != TRUE)
			return(FALSE);
	}

	linstr("\n");

	curwp->w_bufp->b_mode |= MDVIEW;/* put this buffer view mode */
	curbp->b_flag &= ~BFCHG;	/* don't flag this as a change */
	wp->w_dotp = lforw(bp->b_linep);/* back to the beginning */
	wp->w_doto = 0;
	upmode();
	mlwrite("");	/* clear the mode line */
	return(TRUE);
}

pad(s, len)	/* pad a string to indicated length */

char *s;	/* string to add spaces to */
int len;	/* wanted length of string */

{
	while (strlen(s) < len) {
		strcat(s, "          ");
		s[len] = '\0';
	}
}
#endif

