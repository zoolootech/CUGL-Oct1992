 /*******************************************************\
**	A preprocessor using the "C" syntax		**
**	described by K&R in "The C Programming		**
**	Language."  Based on the macro processor	**
**	in Ratfor from K&P's "Software Tools."		**
**							**
**	Robert T Pasky					**
**	36 Wiswall Rd					**
**	Newton Centre, MA 02159				**
**	(617) 964-3641					**
 \*******************************************************/

/*
**	format is:
**		pp <infile> [<outfile>] [-d]\n");
**
**	if <outfile> is not given, output will be placed in
**	<infile>.PP.
**	The optional -d turns on the debug mode which is only
**	useful if you want to watch what goes on inside the
**	program as it crunches files.
**
**	This version is not simply a translation from the
**	Ratfor version: it was modified to conform to the
**	"official" format of the C preprocessor as defined
**	in the K&R C Programming Language book.
**	The output file can be fed into the BDS C compiler;
**	or any other compiler (and other languages?) for
**	which the input was intended.
**
**	This program has #include file capability and
**	symbolic parameter substitution. That means you
**	can write something like:
**		#define max(a,b) (a) > (b) ? (a) : (b)
**	which will replace:
**		max(x+1,y-1)
**	with:
**		(x+1) > (y-1) ? (x+1) : (y-1)
**
**	Here "max(a,b)" is a template with two parameters:
**	a and b. When the preprocessor sees "max(x+1,y-1)"
**	it replaces the first parameter (a) in the definition
**	with "x+1" and the second parameter ("b") with "y-1".
**	Notice that the "(" must follow "max" with no intervening
**	spaces. That's so the preprocessor can differentiate
**	between argumented and non-argumented macros, e.g.,
**	"#define ERROR (-1)".
**	Secondly, the a and b in the definition are surrounded
**	by parentheses. This is often a good idea; it avoids
**	problems that might occur with precedence rules, e.g.:
**	#define div(a,b) a/b
**	... div(x+2,5)
**	would result in "x+2/5" which is interpreted as x+(2/5)
**	instead of the intended "(x+2)/5".
**
**	Note: this program has NOT been rigorously tested
**	and, as provided here, is quite limited in its array
**	size (it's easy enough to increase the defines for
**	the array sizes if you have lots of memory).
**	The proportions allocated among the various arrays
**	may also need tuning. For example, if you like to use
**	lots of short define strings you might increase MAXTOK
**	and MAXDEF. On the other hand, if you have relatively
**	few defines but they tend to be long-winded, you could
**	shorten these in favor of increasing MAXTBL (and
**	possibly DEFSIZ).
*/

#include "bdscio.h"

#define STDOUT 1

#define STDERR 1	/* EKR 3/10/83 */
#define YES 1		/* EKR 3/10/83 */
#define NO 0		/* EKR 3/10/83 */
#define NUL NULL	/* EKR 3/10/83 */
#define EQUAL 0		/* EKR 3/10/83 */

#define ALPHA 'a'
#define LETTER 'a'
#define DIGIT '9'
#define LPAREN '('
#define RPAREN ')'
#define LBRACK '('
#define RBRACK ')'
#define COMMA ','
#define ARGFLAG 0xFA
#define HASH '#'
#define DEFTYPE 0xFC
#define INCTYPE 0xFB
#define MAXFILNAME 20
#define CHARBUFSIZE 80
#define MAXDEF 600
#define MAXTOK 200
#define MAXTBL 1000
#define MAXPTR 600
#define DEFSIZ 129
#define TOKSIZ 129
#define EVALSIZE 200
#define ARGSIZE 200
#define CALLSIZE 200

int bp;
char buf[CHARBUFSIZE];
char inbuf[BUFSIZ], incbuf[BUFSIZ], outbuf[BUFSIZ];
char outfile[MAXFILNAME], incfil[MAXFILNAME];
char defn[MAXDEF], token[MAXTOK], table[MAXTBL];
int namptr[MAXPTR];
int lastp, lastt;
int cp, ep;
char evalst[EVALSIZE];
int ap, argstk[ARGSIZE], callst[CALLSIZE];
int nlb, plev[CALLSIZE];
int incflg;

int debug;

main(argc, argv)
int argc;
char **argv;
{
/*
**	macro - expand macros with arguments
*/
	char t, *defnam, *incnam, *s;
	int deftyp[2];
	char *balp;

	debug = FALSE;
	deftyp[0] = DEFTYPE;
	deftyp[1] = NUL;
	defnam = "#define";
	incnam = "#include";
	incflg = NO;
	balp = "()";
	bp = -1;
	lastp = lastt = -1;

	printf("pp v1.0\n");

	if (argc < 2)
		perr("usage: pp <infile> [<outfile>] [-d]\n");
	s = *++argv;
	if (fopen(s, inbuf) == ERROR)
		perr1("cannot open <%s>", s);
	argc -= 2; argv++;
	if (argc > 0 && **argv != '-') {
		strcpy (outfile, s = *argv++);
		argc--;
		}
	else	{
		strcpy (outfile, s);
		strcat (outfile, ".PP");
		}
	if (fcreat(outfile, outbuf) == ERROR)
		perr1("cannot create <%s>", outfile);
	fprintf(STDERR, "output will be in <%s>\n", outfile);

	while (argc-- > 0) {
		s = *argv++;
		if (strcmp(s, "-D") == EQUAL)
			debug = TRUE;
		}
	if (debug) fprintf(STDERR,"debug is on\n");

	instal(defnam, deftyp);
	deftyp[0] = INCTYPE;
	instal(incnam, deftyp);
	cp = -1;
	ap = ep = 0;
	for (t = gettok(); t != CPMEOF; t = gettok()) {
		if (t == ALPHA) {
			if (lookup(token, defn) == NO)
				puttok(token);
			else {
				if (*defn == INCTYPE) {
					doinc();
					continue;
					}
				if (++cp > CALLSIZE)
					perr("call stack overflow\n");
				callst[cp] = ap;
				ap = push(ep, argstk, ap);
				puttok(defn);
				putchr(NUL);
if (debug) fprintf(STDERR, "push defn: <%s>\n", defn);
				ap = push(ep, argstk, ap);
/*********************/
				if (*defn == DEFTYPE)
					while (isspace(gettok()))
						;
/*********************/
				puttok(token);
				putchr(NUL);
if (debug) fprintf(STDERR, "push name: <%s>\n", token);
				ap = push(ep, argstk, ap);
				t = gettok();
				pbstr(token);
if (debug) fprintf(STDERR, "next token: <%s>\n", token);
				if (t != LPAREN)
					pbstr(balp);	/* add () if none */
				plev[cp] = 0;
				}
			}
		else if (cp == -1)
			puttok(token);
/*		else if (t == LBRACK) {
**			nlb = 1;
**			while (1) {
**				t = gettok();
**				if (t == LBRACK)
**					nlb++;
**				else if (t == RBRACK) {
**					nlb--;
**					if (nlb == 0)
**						break;
**					}
**				else if (t == CPMEOF)
**					perr("EOF in string\n");
**				puttok(token);
**				}
**			}
*/
		else if (t == LPAREN) {
			if (plev[cp]++ > 0)
				puttok(token);
			}
		else if (t == RPAREN) {
			if (--plev[cp] > 0)
				puttok(token);
			else {
				putchr(NUL);
				eval(argstk, callst[cp], ap - 1);
				ap = callst[cp];	/* pop eval stack */
				ep = argstk[ap];
				cp--;
				}
			}
		else if (t == COMMA && plev[cp] == 1) {
			putchr(NUL);
			ap = push(ep, argstk, ap);
			}
		else
			puttok(token);
		}
	if (cp != -1)
		perr("unexpected EOF (not at level 0)\n");

	putc(CPMEOF, outbuf);
	fflush(outbuf);
	fclose(outbuf);
	exit();
}

perr(s)
char *s;
{
	fprintf(STDERR, s);
	exit();
}

perr1(s, t)
char *s, *t;
{
	fprintf(STDERR, s, t);
	exit();
}
/*
**	lookup - locate name, extract def. from table
*/
lookup(name, defn)
char name[], defn[];
{
	int i, j, k;

if(debug) fprintf(STDERR, "lookup: name <%s>...", name);

	for (i = lastp; i >= 0; i--) {

if(debug) fprintf(STDERR, "\nnamptr: %d table: <%s>",
	 namptr[i], &table[namptr[i]]);

		if (strcmp(&table[namptr[i]], name) == EQUAL) {
			j = strlen(name) + 1;
			strcpy (defn, &table[namptr[i] + j]);

if(debug) fprintf(STDERR, "found! defn: <%s>\n", defn);

			return(YES);
			}
		}
if(debug) fprintf(STDERR, "not found.\n");

	return(NO);
}

/*
**	instal - add name and definition to table
*/
instal(name, defn)
char name[], defn[];
{
	int dlen, nlen;

if(debug) fprintf(STDERR, "instal: name <%s> defn <%s>\n", name, defn);

	nlen = strlen(name) + 1;
	dlen = strlen(defn) + 1;
	if (lastt + nlen + dlen > MAXTBL || lastp >= MAXPTR)
		fprintf(STDERR, "<%s>: too many defns\n", name);

	namptr[++lastp] = lastt + 1;
	strcpy(&table[lastt + 1], name);
	strcpy(&table[lastt + nlen + 1], defn);
	lastt += nlen + dlen;
}

/*
**	getdef - for no arguments - get name and definition
*/
getdef(token, defn)
char token[], defn[];
{
	int i, nlpar;
	char c;

if(debug) fprintf(STDERR, "getdef: token <%s>\n", token);

	if ((c = ngetc()) != LPAREN)
		perr("missing left paren\n");
	else if (gettok() != ALPHA)
		perr("non-alphanumeric token\n");
	else if ((c = ngetc()) != COMMA)
		perr("missing comma in define\n");
	/* else got name */
	nlpar = 0;
	for (i = 0; nlpar >= 0; i++)
		if (i >= DEFSIZ)
			perr("definition too long\n");
		else if ((defn[i] = ngetc()) == CPMEOF)
			perr("missing right paren\n");
		else if (defn[i] == LPAREN)
			nlpar++;
		else if (defn[i] == RPAREN)
			nlpar--;
		/* else normal char in defn[i] */
	defn[i - 1] = NUL;
}
/*
**	Get alphanumeric string or single non-alpha for define
*/
gettok()
{
	char tok, ntok;
	int i, clev;

if(debug) fprintf(STDERR, "gettok: ");
	for (i = 0; i < TOKSIZ; i++) {
		tok = type(token[i] = ngetc());
		if (tok != LETTER && tok != DIGIT && tok != HASH)
			break;
		}
	if (i == 0) {
		if (tok == '"' || tok == '\'') {

if (debug) fprintf(STDERR, "gettok: in quote\n");

			for (i++; i < TOKSIZ; i++) {
				if ((token[i] = ngetc()) != '\\') {
					if (token[i] == tok)
						break;
					}
				else
					token[++i] = ngetc();
				}
			if (i >= TOKSIZ)
				perr("token too long\n");
			token[i+1] = NUL;
			return(tok);
			}
		else if (tok == '/') if ((ntok = ngetc()) == '*') {
			clev = 1;
			i = 0;
			tok = token[i] = ' ';
if (debug) fprintf(STDERR, " in comment\n");

comnt:			while (tok != '*' && tok != '/' && tok != CPMEOF)
				tok = ngetc();
			if (tok == CPMEOF)
				perr("EOF in comment\n");
			ntok = ngetc();
			if (tok == '/' && ntok == '*')
				clev++;
			if (tok == '*' && ntok == '/')
				clev--;
			if (clev > 0) {
				tok = ntok;
				goto comnt;
				}
			}
		else if (tok == '/' && ntok != '*')
			putbak(ntok);
		}
	if (i >= TOKSIZ)
		perr("token too long\n");
	if (i > 0) {
		putbak(token[i--]);
		tok = ALPHA;
		}
	token[i+1] = NUL;
if(debug) fprintf(STDERR, " token: <%s> type: %c\n", token, tok);
	return(tok);

}

puttok(str)
char str[];
{
	int i;

if (debug) fprintf(STDERR, "puttok: <%s> cp:%d ep:%d ap:%d\n",
	str, cp, ep, ap);

	for (i = 0; str[i] != NUL; i++)
		putchr(str[i]);
}

putchr(c)
char c;
{
	if (cp == -1) {
		if (c == '\n')
			putc('\r', outbuf);
		putc(c, outbuf);
		}
	else {
		if (ep > EVALSIZE)
			perr("evaluation stack overflow\n");
		evalst[ep++] = c;
		}
}

push(ep, argstk, ap)
int ep, argstk[], ap;
{
if (debug) fprintf(STDERR, "push: ep: %d ap: %d\n", ep, ap);

	if (ap > ARGSIZE)
		perr("arg stack overflow\n");
	argstk[ap] = ep;
	return (ap + 1);
}

eval(argstk, i, j)
int argstk[], i, j;
{
	int k, m, n, t, td, argno;

	t = argstk[i];
	td = evalst[t];

if (debug) fprintf(STDERR, "eval: t: %d td: <%s>\n", t, &evalst[t]);

	if (td == DEFTYPE)
		dodef(argstk, i, j);
	else {
		for (k = t + strlen(&evalst[t]) - 1; k > t; k--) {

if (debug) fprintf(STDERR, "eval: k: %d evalst: <%c%c>\n",
	k, evalst[k-1], evalst[k]);

			if (evalst[k-1] != ARGFLAG)
				putbak(evalst[k]);
			else {
				if (isdigit(evalst[k]))
					argno = evalst[k] - '0';
				else	argno = -1;
				if (argno >= 0 && argno < (j - i)) {
					n = i + argno + 1;
					m = argstk[n];
					pbstr(&evalst[m]);

if (debug) fprintf(STDERR, "eval: n%d: m%d: <%s>", n, m, &evalst[m]);

					}
				k--; /* skip over $ */
				}
			}
		if (k == t) {
			putbak(evalst[k]);

if (debug) fprintf(STDERR, "eval: k==t: <%s>", &evalst[k]);

			}
		}

}

dodef(argstk, i, j)
int argstk[], i, j;
{
	int a1, a2, k;
	char t;

if (debug) fprintf(STDERR, "dodef: i:%d j:%d\n", i, j);

	if (j - i > 1) {
		a1 = argstk[i+1];
		do
			t = gettok();
		while (isspace(*token));
		ap = push(ep, argstk, ap);
/*		if (++cp > CALLSIZE)
**			perr("call stack overflow\n");
*/
		while (*token != '\n') {
			for (k = i + 2; k <= j; k++) {
				a2 = argstk[k];

if (debug) fprintf(STDERR, "strcmp: <%s> %d:<%s>\n",
	token, k, &evalst[a2]);

				if (strcmp(&evalst[a2], token) == EQUAL)
					break;
				}
			if (k <= j) {
				putchr(ARGFLAG);
				putchr('0' + k - i - 1);
				}
			else
				puttok(token);
			t = gettok();
			}
		while (isspace(evalst[ep - 1]))
			ep--;
		putchr(NUL);
		a2 = argstk[ap - 1];

if (debug) fprintf(STDERR, "dodef: name: <%s> defn: <%s>\n",
	&evalst[a1], &evalst[a2]);

		instal(&evalst[a1], &evalst[a2]); /* subarrays */
/*		cp--;
*/
		}
}

doinc()
{
	char c, d;
	int i;

	if (incflg == YES)
		perr("can't nest includes\n");
	while(isspace(c = ngetc()))
		;
	if (c == CPMEOF)
		perr("unexpected EOF (#include)\n");
	if (c == '<')		d = '>';
	else if (c == '"')	d = '"';
	else
		perr("bad include argument\n");
	for (i = 0; c != CPMEOF && c != '\n'; i++) {
		if ((c = ngetc()) == d)
			break;
		incfil[i] = c;
		if (i > MAXFILNAME)
			perr("filename too long\n");
		}
	if (i > 0)
		incfil[i] = NUL;
	else
		perr("illegal include name\n");
	if (fopen(incfil, incbuf) == ERROR)
		perr1("can't include <%s>\n", incfil);
	while (c != '\n' && c != CPMEOF)
		c = ngetc();
	incflg = YES;
}


type(c)
char c;
{
	if (isalpha(c)) return(LETTER);
	if (isdigit(c)) return(DIGIT);
	else return(c);
}

pbstr(in)
char *in;
{
	int i;

	for (i = strlen(in) - 1; i >= 0; i--)
		putbak(in[i]);
}

putbak(c)
char c;
{
	if (bp++ >= CHARBUFSIZE)
		perr("too many chars pushed back\n");
	buf[bp] = c;
}

ngetc()
{
	char c;

	if (bp < 0) {
		bp = 0;
		while ((buf[bp] = c = agetc()) == '\r')
			;
		}
	else
		c = buf[bp];
	if (c != CPMEOF)
		bp--;
	return (c);
}

agetc()
{
	int c;

	if (incflg == YES) {
		if ((c = getc(incbuf)) != CPMEOF)
			return(c);
		else
			incflg = NO;
		}
	return (getc(inbuf));
}



 == YES) {
		if ((c = getc(incbuf)) != CPMEOF)
			return(c);
		else
			incflg 