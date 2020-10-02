#include        <stdio.h>
#include        "c.h"
#include        "expr.h"
#include        "gen.h"
#include        "cglbdec.h"

/*
 *	68000 C compiler
 *
 *	Copyright 1984, 1985, 1986 Matthew Brandt.
 *  all commercial rights reserved.
 *
 *	This compiler is intended as an instructive tool for personal use. Any
 *	use for profit without the written consent of the author is prohibited.
 *
 *	This compiler may be distributed freely for non-commercial use as long
 *	as this notice stays intact. Please forward any enhancements or questions
 *	to:
 *
 *		Matthew Brandt
 *		Box 920337
 *		Norcross, Ga 30092
 */

static int      errno[80];
static int      numerrs;
static char     inline[132];
int             total_errors = 0;
extern char     *lptr;          /* shared with preproc */
extern FILE     *inclfile[10];  /* shared with preproc */
extern int      inclline[10];   /* shared with preproc */
extern int      incldepth;      /* shared with preproc */
char            *linstack[20];  /* stack for substitutions */
char            chstack[20];    /* place to save lastch */
int             lstackptr = 0;  /* substitution stack pointer */

int     isalnum(c)
char    c;
{       return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9');
}

int     isidch(c)
char    c;
{       return isalnum(c) || c == '_' || c == '$';
}

int     isspace(c)
char    c;
{       return c == ' ' || c == '\t' || c == '\n';
}

int     isdigit(c)
char    c;
{       return (c >= '0' && c <= '9');
}

initsym()
{       lptr = inline;
        inline[0] = 0;
        numerrs = 0;
        total_errors = 0;
        lineno = 0;
}

int     getline(listflag)
int     listflag;
{       int     rv;
        if( lineno > 0 && listflag) {
                fprintf(list,"%6d\t%s",lineno,inline);
                while(numerrs--)
                        fprintf(list," *** error %d\n",errno[numerrs]);
                numerrs = 0;
                }
        ++lineno;
        rv = (fgets(inline,131,input) == NULL);
        if( rv && incldepth > 0 ) {
                fclose(input);
                input = inclfile[--incldepth];
                lineno = inclline[incldepth];
                return getline(0);
                }
        if( rv )
                return 1;
        lptr = inline;
        if(inline[0] == '#')
                return preprocess();
        return 0;
}

/*
 *      getch - basic get character routine.
 */
int     cgetch()
{       while( (lastch = *lptr++) == '\0') {
                if( lstackptr > 0 ) {
                        lptr = linstack[--lstackptr];
                        lastch = chstack[lstackptr];
                        return lastch;
                        }
                if(getline(incldepth == 0))
                        return lastch = -1;
                }
        return lastch;
}
 
/*
 *      error - print error information
 */
error(n)
int     n;
{       errno[numerrs++] = n;
        ++total_errors;
}

/*
 *      getid - get an identifier.
 *
 *      identifiers are any isidch conglomerate
 *      that doesn't start with a numeric character.
 *      this set INCLUDES keywords.
 */
int     getid()
{       register int    i;
        i = 0;
        while(isidch(lastch)) {
                if(i < 19)
                        lastid[i++] = lastch;
				cgetch();
                }
        lastid[i] = '\0';
        lastst = id;
}
 
/*
 *      getsch - get a character in a quoted string.
 *
 *      this routine handles all of the escape mechanisms
 *      for characters in strings and character constants.
 */
int     getsch()        /* return an in-quote character */
{       register int    i, j;
        if(lastch == '\n')
                return -1;
        if(lastch != '\\') {
                i = lastch;
				cgetch();
                return i;
                }
		cgetch();        /* get an escaped character */
        if(isdigit(lastch)) {
                i = 0;
                for(i = 0;j < 3;++j) {
                        if(lastch <= '7' && lastch >= '0')
                                i = (i << 3) + lastch - '0';
                        else
                                break;
						cgetch();
                        }
                return i;
                }
        i = lastch;
		cgetch();
        switch(i) {
                case '\n':
						cgetch();
                        return getsch();
                case 'b':
                        return '\b';
                case 'f':
                        return '\f';
                case 'n':
                        return '\n';
                case 'r':
                        return '\r';
				case 't':
						return '\t';
                default:
                        return i;
                }
}

int     radix36(c)
char    c;
{       if(isdigit(c))
                return c - '0';
        if(c >= 'a' && c <= 'z')
                return c - 'a' + 10;
        if(c >= 'A' && c <= 'Z')
                return c - 'A' + 10;
        return -1;
}
 
/*
 *      getbase - get an integer in any base.
 */
getbase(b)
{       register int    i, j;
        i = 0;
        while(isalnum(lastch)) {
                if((j = radix36(lastch)) < b) {
                        i = i * b + j;
						cgetch();
                        }
                else break;
                }
        ival = i;
        lastst = iconst;
}
 
/*
 *      getfrac - get fraction part of a floating number.
 */
getfrac()
{       double  frmul;
        frmul = 0.1;
        while(isdigit(lastch)) {
                rval += frmul * (lastch - '0');
				cgetch();
                frmul *= 0.1;
                }
}
 
/*
 *      getexp - get exponent part of floating number.
 *
 *      this algorithm is primative but usefull.  Floating
 *      exponents are limited to +/-255 but most hardware
 *      won't support more anyway.
 */
getexp()
{       double  expo, exmul;
        expo = 1.0;
        if(lastst != rconst)
                rval = ival;
        if(lastch = '-') {
                exmul = 0.1;
				cgetch();
                }
        else
                exmul = 10.0;
        getbase(10);
        if(ival > 255)
                error(ERR_FPCON);
        else
                while(ival--)
                        expo *= exmul;
        rval *= expo;
}
 
/*
 *      getnum - get a number from input.
 *
 *      getnum handles all of the numeric input. it accepts
 *      decimal, octal, hexidecimal, and floating point numbers.
 */
getnum()
{       register int    i, j, k;
        i = 0;
        if(lastch == '0') {
				cgetch();
                if(lastch == 'x' || lastch == 'X') {
						cgetch();
                        getbase(16);
                        }
                else getbase(8);
                }
        else    {
                getbase(10);
                if(lastch == '.') {
						cgetch();
                        rval = ival;    /* float the integer part */
                        getfrac();      /* add the fractional part */
                        lastst = rconst;
                        }
                if(lastch == 'e' || lastch == 'E') {
						cgetch();
                        getexp();       /* get the exponent */
                        }
                }
}

/*
 *      getsym - get next symbol from input stream.
 *
 *      getsym is the basic lexical analyzer.  It builds
 *      basic tokens out of the characters on the input
 *      stream and sets the following global variables:
 *
 *      lastch:         A look behind buffer.
 *      lastst:         type of last symbol read.
 *      laststr:        last string constant read.
 *      lastid:         last identifier read.
 *      ival:           last integer constant read.
 *      rval:           last real constant read.
 *
 *      getsym should be called for all your input needs...
 */
int     getsym()
{       register int    i, j, k;
        SYM             *sp;
restart:        /* we come back here after comments */
        while(isspace(lastch))
				cgetch();
        if( lastch == -1)
                lastst = eof;
        else if(isdigit(lastch))
                getnum();
        else if(isidch(lastch)) {
                getid();
                if( (sp = search(lastid,defsyms.head)) != 0 ) {
                        linstack[lstackptr] = lptr;
                        chstack[lstackptr++] = lastch;
                        lptr = sp->value.s;
						cgetch();
                        goto restart;
                        }
                }
        else switch(lastch) {
                case '+':
                        cgetch();
                        if(lastch == '+') {
                                cgetch();
                                lastst = autoinc;
                                }
                        else if(lastch == '=') {
                                cgetch();
                                lastst = asplus;
                                }
                        else lastst = plus;
                        break;
                case '-':
                        cgetch();
                        if(lastch == '-') {
                                cgetch();
                                lastst = autodec;
                                }
                        else if(lastch == '=') {
                                cgetch();
                                lastst = asminus;
                                }
                        else if(lastch == '>') {
                                cgetch();
                                lastst = pointsto;
                                }
                        else lastst = minus;
                        break;
                case '*':
                        cgetch();
                        if(lastch == '=') {
                                cgetch();
                                lastst = astimes;
                                }
                        else lastst = star;
                        break;
                case '/':
                        cgetch();
                        if(lastch == '=') {
                                cgetch();
                                lastst = asdivide;
                                }
                        else if(lastch == '*') {
                                cgetch();
                                for(;;) {
                                        if(lastch == '*') {
                                                cgetch();
                                                if(lastch == '/') {
                                                        cgetch();
                                                        goto restart;
                                                        }
                                                }
                                        else
                                                cgetch();
                                        }
                                }
                        else lastst = divide;
                        break;
                case '^':
                        cgetch();
                        lastst = uparrow;
                        break;
                case ';':
                        cgetch();
                        lastst = semicolon;
                        break;
                case ':':
                        cgetch();
                        lastst = colon;
                        break;
                case '=':
                        cgetch();
                        if(lastch == '=') {
                                cgetch();
                                lastst = eq;
                                }
                        else lastst = assign;
                        break;
                case '>':
                        cgetch();
                        if(lastch == '=') {
                                cgetch();
                                lastst = geq;
                                }
                        else if(lastch == '>') {
                                cgetch();
                                if(lastch == '=') {
                                        cgetch();
                                        lastst = asrshift;
                                        }
                                else lastst = rshift;
                                }
                        else lastst = gt;
                        break;
                case '<':
                        cgetch();
                        if(lastch == '=') {
                                cgetch();
                                lastst = leq;
                                }
                        else if(lastch == '<') {
                                cgetch();
                                if(lastch == '=') {
                                        cgetch();
                                        lastst = aslshift;
                                        }
                                else lastst = lshift;
                                }
                        else lastst = lt;
                        break;
                case '\'':
                        cgetch();
                        ival = getsch();        /* get a string char */
                        if(lastch != '\'')
                                error(ERR_SYNTAX);
                        else
                                cgetch();
                        lastst = iconst;
                        break;
                case '\"':
                        cgetch();
                        for(i = 0;i < MAX_STRLEN;++i) {
                                if(lastch == '\"')
                                        break;
                                if((j = getsch()) == -1)
                                        break;
                                else
                                        laststr[i] = j;
                                }
                        laststr[i] = 0;
                        lastst = sconst;
                        if(lastch != '\"')
                                error(ERR_SYNTAX);
                        else
                                cgetch();
                        break;
                case '!':
                        cgetch();
                        if(lastch == '=') {
                                cgetch();
                                lastst = neq;
                                }
                        else lastst = not;
                        break;
                case '%':
                        cgetch();
                        if(lastch == '=') {
                                cgetch();
                                lastst = asmodop;
                                }
                        else lastst = modop;
                        break;
                case '~':
                        cgetch();
                        lastst = compl;
                        break;
                case '.':
                        cgetch();
                        lastst = dot;
                        break;
                case ',':
                        cgetch();
                        lastst = comma;
                        break;
                case '&':
                        cgetch();
                        if( lastch == '&') {
                                lastst = land;
                                cgetch();
                                }
                        else if( lastch == '=') {
                                lastst = asand;
                                cgetch();
                                }
                        else
                                lastst = and;
                        break;
                case '|':
                        cgetch();
                        if(lastch == '|') {
                                lastst = lor;
                                cgetch();
                                }
                        else if( lastch == '=') {
                                lastst = asor;
                                cgetch();
                                }
                        else
                                lastst = or;
                        break;
                case '(':
                        cgetch();
                        lastst = openpa;
                        break;
                case ')':
                        cgetch();
                        lastst = closepa;
                        break;
                case '[':
                        cgetch();
                        lastst = openbr;
                        break;
                case ']':
                        cgetch();
                        lastst = closebr;
                        break;
                case '{':
                        cgetch();
                        lastst = begin;
                        break;
                case '}':
                        cgetch();
                        lastst = end;
                        break;
                case '?':
                        cgetch();
                        lastst = hook;
                        break;
                default:
                        cgetch();
                        error(ERR_ILLCHAR);
                        goto restart;   /* get a real token */
                }
        if(lastst == id)
                searchkw();
}

needpunc(p)
enum e_sym      p;
{       if( lastst == p)
                getsym();
        else
                error(ERR_PUNCT);
}

