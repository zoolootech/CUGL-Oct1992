/* [MACRO.C of JUGPDS Vol.46]*/
/*
*****************************************************************
*								*
*	Written by  Hakuo Katayose (JUG-CP/M No.179)		*
*		    49-114 Kawauchi-Sanjuunin-machi		*
*		    Sendai, Miyagi 980                          *
*		    Phone: 0222-61-3219				*
*								*
*       Modifird by Toshiya Oota   (JUG-CPM No.10)              *
*                   Sakae ko-po 205 				*
*		    5-19-6 Hosoda				*
*		    Katusikaku Tokyo 124			*
*								*
*		for MS-DOS Lattice C V3.1J & 80186/V20/V30	*
*								*
*	Compiler Option: -ccu -k0(1) -ms -n -v -w		*
*								*
*	Edited & tested by Y. Monma (JUG-CP/M Disk Editor)	*
*			&  T. Ota   (JUG-CP/M Sub Disk Editor)	*
*								*
*****************************************************************
*/

/* macro (m4) - expand macros with arguments */

#include "stdio.h"
#include "dos.h"
#include "tools.h"
#include "toolfunc.h"
	
#define ARGFLAG     '$'

#define	IFTYPE	    3
#define	INCTYPE	    4
#define	ARTTYPE	    5
#define SUBTYPE	    7
#define DEFTYPE	    8
#define LENTYPE     9

#define	UBUFSIZE  256
#define ARGSIZE	  128
#define CALLSIZE  128
#define EVALSIZE 1024

#define MAXTOK	  80
#define MAXDEF	 256

char	ubuf[UBUFSIZE];
int	ubufp;

struct	mac {
	char	*name;
	char	*repn;
} deftbl[1000];

int	tcnt;

char	ctype[256];
FILE	*infp;

char	evalst[EVALSIZE];
int	ep, cp;

void	initialize(),eval(),dodef(),dolen(),pbnum(),doif(),dosub(),doincr();
void	doarith(),install(),puttok(),putchr(),ungets(),ungetch();

void main(argc, argv)
int	argc;
char	*argv[];

{
	int	ap, t, nbl, plev[CALLSIZE];
	char	token[(MAXTOK-1)], defn[MAXDEF], *balp, s[2];
	int	callst[CALLSIZE];
	int	argstk[ARGSIZE];

	balp = "()";
	if (argc < 2) {
		fprintf(STDERR, "MAC999 Usage: macro filename");
		exit();
	}
	if ((infp = fopen(argv[1], READ)) == NO) {
		fprintf(STDERR, "MAC901 Can't open file:%s", argv[1]);
		exit();
	}
	initialize();
	s[0] = DEFTYPE;
	s[1] = EOS;
	install("define", s);
	s[0] = IFTYPE;
	s[1] = EOS;
 	install("ifelse", s);
	s[0] = INCTYPE;
	s[1] = EOS;
	install("incr", s);
	s[0] = ARTTYPE;
	s[1] = EOS;
	install("arith", s);
	s[0] = SUBTYPE;
	s[1] = EOS;
	install("substr", s);
	s[0] = LENTYPE;
	s[1] = EOS;
	install("len", s);

	cp = 0;
	ap = 1;
	ep = 1;
	while ((t = gettok(token, MAXTOK)) != EOF) {
		if (t == ALPHA) {
		if (lookup(token, defn) == NO)
			puttok(token);
		else { /* defined, put it in eval stack */
			if (++cp > CALLSIZE) {
				fprintf(STDERR, "MAC902 Call stack overflow.\n");
			}
				callst[cp] = ap;
				ap = push(ep, argstk, ap);
				puttok(defn); /* stack definition */
				putchr(EOS);
				ap = push(ep, argstk, ap);
				puttok(token); /* stack name */
				putchar(EOS);
				ap = push(ep, argstk, ap);
				t = gettok(token, MAXTOK); /* peek a next */
				ungets(token);
				if (t != LPAREN) /* add () if not present */
					ungets(balp);
				plev[cp] = 0;
		}
	}
	else if (t == LBRACKET) { /* strip one level of [] */
		nbl = 1;
		while (1) {
			if ((t = gettok(token, MAXTOK)) == LBRACKET)
				nbl++;
			else if (t == RBRACKET) {
				if (--nbl == 0)
					break;
			}
			else if (t == EOF) {
				fprintf(STDERR, "MAC903 EOF inserting.\n");
				exit();
			}
			puttok(token);
		}
	}
	else if (cp == 0)	/* not in a macro at all */
		puttok(token);
	else if (t == LPAREN) {
		if (plev[cp] > 0)
			puttok(token);
		plev[cp]++;
	}
	else if (t == RPAREN) {
		if (--plev[cp] > 0)
			puttok(token);
		else { /* end of argument list */
			putchr(EOS);
			eval(argstk, callst[cp], ap-1);
			ap = callst[cp]; /* pop eval stack */
			ep = argstk[ap];
			cp--;
			}
		}
		else if (t == ',' && plev[cp] == 1) { /* new arg */
			putchr(EOS);
		ap = push(ep, argstk, ap);
		}
	else
		puttok(token); /* just stack it */
	}
	if (cp != 0)
	fprintf(STDERR, "MAC904 Unexpeced EOF.");
}

/* eval - expand arg i through j: evaluate builtin or push back defn */
void eval(argstk, i, j)
int	argstk[];

{
	int	t, argno, k, td;

	t = argstk[i];
	if ((td = evalst[t]) == DEFTYPE)
		dodef(argstk, i , j);
	else if (evalst[t] == INCTYPE)
		doincr(argstk, i, j);
	else if (evalst[t] == ARTTYPE)
		doarith(argstk, i, j);
	else if (evalst[t] == SUBTYPE)
		dosub(argstk, i, j);
	else if (evalst[t] == IFTYPE)
		doif(argstk, i, j);
	else if (evalst[t] == LENTYPE)	
		dolen(argstk, i, j);
	else {
		for (k = t + strlen(&evalst[t]) - 1; k > t; k--)
			if (evalst[k-1] != ARGFLAG)
				ungetch(evalst[k]);
			else {
				argno = evalst[k--]-'0';
				if (argno >= 0 && argno < j-i)
					ungets(&evalst[argstk[i+argno+1]]);
			}
		if (k == t) /* do last character */
			ungetch(evalst[k]);
	}
}


/* dodef - install definition in table */
void dodef(argstk, i, j)
int	argstk[];

{
	if (j-i >2)
		install(&evalst[argstk[i+2]], &evalst[argstk[i+3]]);
}


/* delen - compute length of string */
void dolen(argstk, i, j)
int	argstk[];

{
	int	num;

	num = strlen(&evalst[argstk[i+2]]);
	pbnum(num);
}


/* pbnum - convert number to string, push back on input */
void pbnum(num)
int	num;

{
	do {
		ungetch(num%10+'0');
		num /= 10;
	} while (num != 0);
}


/* doif - select one of two arguents */
void doif(argstk, i, j)
int	argstk[];

 {
	if (j-i < 5)
		return;
	if (!strcmp(&evalst[argstk[i+2]], evalst[argstk[i+3]]))
		ungets(&evalst[argstk[i+4]]);
	else
		ungets(&evalst[argstk[i+5]]);
}


/* dosub - select substring */
void dosub(argstk, i, j)
int	argstk[];

{
	int	fc, nc, app, k;

	if (j-i < 3)
		return;
	nc = (j-i < 4) ? MAXTOK : ctoi(&evalst[argstk[i+4]]);
	app = argstk[i+2];
	fc = app + ctoi(&evalst[argstk[i+3]]) - 1;
	if (fc >= app && fc < app + strlen(&evalst[app])) {
		k = fc + min(nc, strlen(&evalst[fc])) - 1;
		while (k >= fc)
			ungetch(evalst[k--]);
	}
}


/* doincr - increment argument by 1 */
void doincr(argstk, i, j)
int	argstk[];

{
	int	k, num;

	k = (j-i < 3) ? 1 : ctoi(&evalst[argstk[i+3]]);
	num = ctoi(&evalst[argstk[i+2]])+k;
	pbnum(num);
}


/* doarith - arithmetic computation */
void doarith(argstk, i, j)
int	argstk[];

{
	int	k, num, l;

	if (j-i < 3)
		return;
	num = ctoi(&evalst[argstk[i+2]]);
	k = (j-i < 4) ? num : ctoi(&evalst[argstk[i+4]]);
	switch (evalst[(l = argstk[i+3])]) {
		case 	'+' :
			num += k;
			break;
		case	'-' :
			num -= k;
			break;
		case	'*' :
			num *= k;
			break;
		case	'/' :
			num /= k;
			break;
		case	'%' :
			num %= k;
			break;
		case	'=' :
			switch (evalst[l+1]) {
				case '<' :
					num = (num <= k);
					break;
				case '>' :
					num = (num >= k);
					break;
	  			case '=' :
					num = (num ==k);
					break;
				case EOS :
					num = (num < k);
					break;
				default :
					return;
			}
			break;
		case '<':
			switch (evalst[l+1]) {
				case '<' :
					num = (num <<= k);
					break;
				case '>' :
					num = (num != k);
					break;
				case '=' :
					num = (num <= k);
					break;
				case EOS :
					num = (num < k);
				default :
				return;
			}
			break;
		case '>' :
			switch (evalst[l+1]) {
				case '<' :
					num = (num != k);
					break;
				case '>' :
					num = (num >>= k);
					break;
				case '=' :
					num = (num >= k);
					break;
				case EOS :
					num = (num > k);
					break;
				default :
					return;
			}
			break;
		default :
			return;
			break;
	}
	pbnum(num);
}


/* ctoi - convert string to integer */
ctoi(s)
char	*s;

{
	int	c, sign;

	sign = 1;
	while (*s == ' ' || *s == '\t')
		s++;
	c = 0;
	if (*s == '+' || *s == '-')
		sign = (*s++ == '-') ? -1 : 1;
	while (*s != EOS && ctype[*s] == DIGIT)
		c = c*10 + *s++ - '0';
	return(sign * c);
}


/* lookup - locate name, extract definition from table */
lookup(s, defn)
char	*s, *defn;

{
	int	i;

	i = tcnt;
	while (--i >= 0)
		if (!strcmp(deftbl[i].name, s))
			break;
	if (i < 0)
		return(NO);
	strcpy(defn, deftbl[i].repn);
	return(YES);
}


/* install - add name and definition to table */
void install(s, defn)
char	*s, *defn;
{
struct mac *p;
   p = &deftbl[tcnt];
   if ((p->name = (char *) malloc(strlen(s)+1)) == NULL) {
	fprintf(STDERR, "MAC905 Variable table overflow!\n");
	exit();
   }
   strcpy(p->name, s);
   if ((p->repn = (char *) malloc(strlen(defn)+1)) == NULL) {
	fprintf(STDERR, "MAC906 Variable table overflow!\n");
	exit();
   }
   strcpy(p->repn, defn);
   tcnt++;
}


/* gettok - get alphanumeric string or single non-alpha */
gettok(w, lim)
char	*w;
int	lim;
{
int	c;
    if ((c = getchr()) == EOF)
	return(c);
   else {
	*w++ = c;
	if (ctype[c] != LETTER && ctype[c] != DIGIT) {
	    *w = '\0';
	    return(c);
	}
	else
	    lim--;
   }
   while (lim-- > 0 && (c= getchr()) != EOF) {
	*w++ = c;
	if (ctype[c] != LETTER && ctype[c] != DIGIT) {
	    *--w = NULL;
	    ungetch(c);
	    return(ALPHA);
	}
   }
   *w = NULL;
   ungetch(c);
   fprintf(STDERR,"MAC907 Token too long.\n");
   return(ALPHA);
}


/* initialize - initializ arrays and others */
void initialize()

{
	int i;

	for (i = 0; i < 26; i++)
		ctype[i] = i;
	for (i = 'a'; i <= 'z'; i++)
		ctype[i] = LETTER;
	for (i = 'A'; i <= 'Z'; i++)
		ctype[i] = LETTER;
	for (i = '0'; i <= '9'; i++)
		ctype[i] = DIGIT;
	ubufp =0;
	tcnt =0;
}


/* push -push ep onto argstk, return new pointer ap */
push(evlp, argstk, argp)
int	argstk[];

{
	if (argp > ARGSIZE) {
		fprintf(STDERR, "MAC908 Arg stack overflow!\n");
		exit();
	}
	argstk[argp] = evlp;
	return(argp+1);
}


/* puttok - put a token either on output or into evaluatin stack */
void puttok(s)
char	*s;

{
	while (*s) putchr(*s++);
}


/* putchr - put single char on output or into evaluation stack */
void putchr(c)

{
	if (!cp)
		putchar(c);
	else {
		if (ep > EVALSIZE) {
			fprintf(STDERR, "MAC909 Evalation stack overflow!\n");
			exit();
		}
		evalst[ep++] = c;
	}
}


/* ungets - push string backonto input */
void ungets(s)
char	*s;

{
	char	*p;

	p = s;
	while (*s)
		s++;
	while (p != s)
		ungetch(*--s);
}

/* getchr - get single character */
getchr()

{
    if (ubufp)
	return( (int) ubuf[--ubufp]);
   else
	return( (int) getc(infp));
}

/* ungetch - push string back onto input */
void ungetch(c)
int	c;

{
	if (ubufp > UBUFSIZE)
		fprintf(STDERR,"MAC910 ungetch: too many characters\n");
	else
		ubuf[ubufp++] = c;
}

/* Enf of MACRO.C */
