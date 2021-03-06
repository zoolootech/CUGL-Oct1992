
From:   RHEA::DECWRL::"elsie!ado@seismo.ARPA" 11-JUN-1985 15:52
To:     seismo!minow%rex.DEC@decwrl
Subj:   cpp

Received: from DECWRL by DEC-RHEA with SMTP; Tue, 11 Jun 85 12:50-PDT
Received: from seismo.ARPA (seismo.arpa.ARPA) by decwrl.ARPA (4.22.01/4.7.34)
        id AA21114; Tue, 11 Jun 85 12:47:28 pdt
Received: from elsie.UUCP by seismo.ARPA with UUCP; Tue, 11 Jun 85 15:44:45 EDT
Date: Tue, 11 Jun 85 15:44:45 EDT
Return-Path: <elsie!ado@seismo.ARPA>
Message-Id: <8506111944.AA14028@seismo.ARPA>

I've made my first, surely bug-ridden, try at handling floating point values in
"#if" directives.  We'll be running it here at elsie to see what happens.
The only thing that changes is "cpp5.c", which appears below.

Signed/unsigned work surely needs to be done.
There's also no checking for bogus stuff such as
        #if 5.0 | 4.0
although stuff such as
        #if 5.2 | 4.2
could fairly easily be detected by checking
        x == (long) x
when converting from a double to a long in "evaleval".

                                --ado

/*
 *                          C P P 5 . C
 *              E x p r e s s i o n   E v a l u a t i o n
 *
 * Edit History
 * 31-Aug-84    MM      USENET net.sources release
 * 04-Oct-84    MM      __LINE__ and __FILE__ must call ungetstring()
 *                      so they work correctly with token concatenation.
 *                      Added string formal recognition.
 * 25-Oct-84    MM      "Short-circuit" evaluate #if's so that we
 *                      don't print unnecessary error messages for
 *                      #if !defined(FOO) && FOO != 0 && 10 / FOO ...
 * 31-Oct-84    ado/MM  Added token concatenation
 *  6-Nov-84    MM      Split from #define stuff, added sizeof stuff
 * 19-Nov-84    ado     #if error returns TRUE for (sigh) compatibility
 * 22-Apr-85    ado/MM  added evalone to properly handle \value
 * 29-Apr-85    ado     Cleaned up #if ... sizeof
 */

#include        <stdio.h>
#include        <ctype.h>
#include        "cppdef.h"
#include        "cpp.h"

/*
 * Evaluate an #if expression.
 */

static char     *opname[] = {           /* For debug and error messages */
"end of expression", "val", "id",
  "+",   "-",  "*",  "/",  "%",
  "<<", ">>",  "&",  "|",  "^",
  "==", "!=",  "<", "<=", ">=",  ">",
  "&&", "||",  "?",  ":",  ",",
  "unary +", "unary -", "~", "!",  "(",  ")", "(none)",
};

/*
 * opdope[] has the operator precedence:
 *     Bits
 *        7     Unused (so the value is always positive)
 *      6-2     Precedence (000x .. 017x)
 *      1-0     Binary op. flags:
 *          01  The binop flag should be set/cleared when this op is seen.
 *          10  The new value of the binop flag.
 * Note:  Expected, New binop
 * constant     0       1       Binop, end, or ) should follow constants
 * End of line  1       0       End may not be preceeded by an operator
 * binary       1       0       Binary op follows a value, value follows.
 * unary        0       0       Unary op doesn't follow a value, value follows
 *   (          0       0       Doesn't follow value, value or unop follows
 *   )          1       1       Follows value.  Op follows.
 */

static char     opdope[OP_MAX] = {
  0001,                                 /* End of expression            */
  0002,                                 /* Digit                        */
  0000,                                 /* Letter (identifier)          */
  0141, 0141, 0151, 0151, 0151,         /* ADD, SUB, MUL, DIV, MOD      */
  0131, 0131, 0101, 0071, 0071,         /* ASL, ASR, AND,  OR, XOR      */
  0111, 0111, 0121, 0121, 0121, 0121,   /*  EQ,  NE,  LT,  LE,  GE,  GT */
  0061, 0051, 0041, 0041, 0031,         /* ANA, ORO, QUE, COL, CMA      */
/*
 * Unary op's follow
 */
  0160, 0160, 0160, 0160,               /* NEG, PLU, COM, NOT           */
  0170, 0013, 0023,                     /* LPA, RPA, END                */
};
/*
 * OP_QUE and OP_RPA have alternate precedences:
 */
#define OP_RPA_PREC     0013
#define OP_QUE_PREC     0034

/*
 * S_ANDOR and S_QUEST signal "short-circuit" boolean evaluation, so that
 *      #if FOO != 0 && 10 / FOO ...
 * doesn't generate an error message.  They are stored in optab.skip.
 */
#define S_ANDOR         2
#define S_QUEST         1

typedef struct optab {
    char        op;                     /* Operator                     */
    char        prec;                   /* Its precedence               */
    char        skip;                   /* Short-circuit: TRUE to skip  */
} OPTAB;
static double   evalue;                 /* Current value from evallex() */

#ifdef  nomacargs
FILE_LOCAL int
isbinary(op)
register int    op;
{
        return (op >= FIRST_BINOP && op <= LAST_BINOP);
}

FILE_LOCAL int
isunary(op)
register int    op;
{
        return (op >= FIRST_UNOP && op <= LAST_UNOP);
}
#else
#define isbinary(op)    (op >= FIRST_BINOP && op <= LAST_BINOP)
#define isunary(op)     (op >= FIRST_UNOP  && op <= LAST_UNOP)
#endif













/*
 * The following definitions are used to specify basic variable sizes.
 */

#ifndef S_CHAR
#define S_CHAR          (sizeof (char))
#endif
#ifndef S_SINT
#define S_SINT          (sizeof (short int))
#endif
#ifndef S_INT
#define S_INT           (sizeof (int))
#endif
#ifndef S_LINT
#define S_LINT          (sizeof (long int))
#endif
#ifndef S_FLOAT
#define S_FLOAT         (sizeof (float))
#endif
#ifndef S_DOUBLE
#define S_DOUBLE        (sizeof (double))
#endif
#ifndef S_PCHAR
#define S_PCHAR         (sizeof (char *))
#endif
#ifndef S_PSINT
#define S_PSINT         (sizeof (short int *))
#endif
#ifndef S_PINT
#define S_PINT          (sizeof (int *))
#endif
#ifndef S_PLINT
#define S_PLINT         (sizeof (long int *))
#endif
#ifndef S_PFLOAT
#define S_PFLOAT        (sizeof (float *))
#endif
#ifndef S_PDOUBLE
#define S_PDOUBLE       (sizeof (double *))
#endif
#ifndef S_PFPTR
#define S_PFPTR         (sizeof (int (*)()))
#endif





typedef struct types {
    short       type;                   /* This is the bit if           */
    char        *name;                  /* this is the token word       */
    short       excluded;               /* but these aren't legal here  */
} TYPES;

#define ANYSIGN         (T_SIGNED | T_UNSIGNED)
#define ANYFLOAT        (T_FLOAT  | T_DOUBLE)
#define ANYINT          (T_CHAR   | T_SHORT | T_INT | T_LONG)

static TYPES basic_types[] = {
        T_CHAR,         "char",         ANYFLOAT | ANYINT,
        T_INT,          "int",          ANYFLOAT | T_CHAR | T_INT,
        T_FLOAT,        "float",        ANYFLOAT | ANYINT | ANYSIGN,
        T_DOUBLE,       "double",       ANYFLOAT | ANYINT | ANYSIGN,
        T_SHORT,        "short",        ANYFLOAT | ANYINT,
        T_LONG,         "long",         ANYFLOAT | ANYINT,
        T_SIGNED,       "signed",       ANYFLOAT | ANYINT | T_CHAR | T_INT,
        T_UNSIGNED,     "unsigned",     ANYFLOAT | ANYINT | T_CHAR | T_INT,
        0,              NULL,           0       /* Signal end           */
};


/*
 * The order of this table is important -- it is also referenced by
 * the command line processor to allow run-time overriding of the
 * built-in size values.  The order must not be changed:
 *      char, short, int, long, float, double (func pointer)
 */
SIZES size_table[] = {
    { T_CHAR,   S_CHAR,         S_PCHAR         },      /* char         */
    { T_SHORT,  S_SINT,         S_PSINT         },      /* short int    */
    { T_INT,    S_INT,          S_PINT          },      /* int          */
    { T_LONG,   S_LINT,         S_PLINT         },      /* long         */
    { T_FLOAT,  S_FLOAT,        S_PFLOAT        },      /* float        */
    { T_DOUBLE, S_DOUBLE,       S_PDOUBLE       },      /* double       */
    { T_FPTR,   0,              S_PFPTR         },      /* int (*())    */
    { 0,        0,              0               },      /* End of table */
};









int
eval()
/*
 * Evaluate an expression.  Straight-forward operator precedence.
 * This is called from control() on encountering an #if statement.
 * It calls the following routines:
 * evallex      Lexical analyser -- returns the type and value of
 *              the next input token.
 * evaleval     Evaluate the current operator, given the values on
 *              the value stack.  Returns a pointer to the (new)
 *              value stack.
 * For compatiblity with older cpp's, this return returns 1 (TRUE)
 * if a syntax error is detected.
 */
{
        register int    op;             /* Current operator             */
        register double *valp;          /* -> value vector              */
        register OPTAB  *opp;           /* Operator stack               */
        int             prec;           /* Op precedence                */
        int             binop;          /* Set if binary op. needed     */
        int             op1;            /* Operand from stack           */
        int             skip;           /* For short-circuit testing    */
        double          value[NEXP];    /* Value stack                  */
        OPTAB           opstack[NEXP];  /* Operand stack                */
        extern double   *evaleval();    /* Does actual evaluation       */

        valp = value;
        opp = opstack;
        opp->op = OP_END;               /* Mark bottom of stack         */
        opp->prec = opdope[OP_END];     /* And its precedence           */
        opp->skip = 0;                  /* Not skipping now             */
        binop = 0;
again:  ;
#ifdef  DEBUG_EVAL
        printf("In #if at again: skip = %d, binop = %d, line is: %s",
            opp->skip, binop, infile->bptr);
#endif
        if ((op = evallex(opp->skip)) == OP_SUB && binop == 0)
            op = OP_NEG;                        /* Unary minus          */
        else if (op == OP_ADD && binop == 0)
            op = OP_PLU;                        /* Unary plus           */
        else if (op == OP_FAIL)
            return (1);                         /* Error in evallex     */
#ifdef  DEBUG_EVAL
        printf("op = %s, opdope = %03o, binop = %d, skip = %d\n",
            opname[op], opdope[op], binop, opp->skip);
#endif
        if (op == DIG) {                        /* Value?               */
            if (binop != 0) {
                cerror("misplaced constant in #if", NULLST);
                return (1);
            }
            else if (valp >= &value[NEXP-1]) {
                cerror("#if value stack overflow", NULLST);
                return (1);
            }
            else {
#ifdef  DEBUG_EVAL
                printf("pushing %f onto value stack[%d]\n",
                    evalue, valp - value);
#endif
                *valp++ = evalue;
                binop = 1;
            }
            goto again;
        }
        else if (op > OP_END) {
            cerror("Illegal #if line", NULLST);
            return (1);
        }
        prec = opdope[op];
        if (binop != (prec & 1)) {
            cerror("Operator %s in incorrect context", opname[op]);
            return (1);
        }
        binop = (prec & 2) >> 1;
        for (;;) {
#ifdef  DEBUG_EVAL
            printf("op %s, prec %d., stacked op %s, prec %d, skip %d\n",
                opname[op], prec, opname[opp->op], opp->prec, opp->skip);
#endif
            if (prec > opp->prec) {
                if (op == OP_LPA)
                    prec = OP_RPA_PREC;
                else if (op == OP_QUE)
                    prec = OP_QUE_PREC;
                op1 = opp->skip;                /* Save skip for test   */
                /*
                 * Push operator onto op. stack.
                 */
                opp++;
                if (opp >= &opstack[NEXP]) {
                    cerror("expression stack overflow at op \"%s\"",
                        opname[op]);
                    return (1);
                }
                opp->op = op;
                opp->prec = prec;
                skip = (valp[-1] != 0);         /* Short-circuit tester */
                /*
                 * Do the short-circuit stuff here.  Short-circuiting
                 * stops automagically when operators are evaluated.
                 */
                if ((op == OP_ANA && !skip)
                 || (op == OP_ORO && skip))
                    opp->skip = S_ANDOR;        /* And/or skip starts   */
                else if (op == OP_QUE)          /* Start of ?: operator */
                    opp->skip = (op1 & S_ANDOR) | ((!skip) ? S_QUEST : 0);
                else if (op == OP_COL) {        /* : inverts S_QUEST    */
                    opp->skip = (op1 & S_ANDOR)
                              | (((op1 & S_QUEST) != 0) ? 0 : S_QUEST);
                }
                else {                          /* Other ops leave      */
                    opp->skip = op1;            /*  skipping unchanged. */
                }
#ifdef  DEBUG_EVAL
                printf("stacking %s, valp[-1] == %d at %s",
                    opname[op], valp[-1], infile->bptr);
                dumpstack(opstack, opp, value, valp);
#endif
                goto again;
            }
            /*
             * Pop operator from op. stack and evaluate it.
             * End of stack and '(' are specials.
             */
            skip = opp->skip;                   /* Remember skip value  */
            switch ((op1 = opp->op)) {          /* Look at stacked op   */
            case OP_END:                        /* Stack end marker     */
                if (op == OP_EOE)
                    return (valp[-1]);          /* Finished ok.         */
                goto again;                     /* Read another op.     */

            case OP_LPA:                        /* ( on stack           */
                if (op != OP_RPA) {             /* Matches ) on input   */
                    cerror("unbalanced paren's, op is \"%s\"", opname[op]);
                    return (1);
                }
                opp--;                          /* Unstack it           */
                /* goto again;                  -- Fall through         */

            case OP_QUE:
                goto again;                     /* Evaluate true expr.  */

            case OP_COL:                        /* : on stack.          */
                opp--;                          /* Unstack :            */
                if (opp->op != OP_QUE) {        /* Matches ? on stack?  */
                    cerror("Misplaced '?' or ':', previous operator is %s",
                        opname[opp->op]);
                    return (1);
                }
                /*
                 * Evaluate op1.
                 */
            default:                            /* Others:              */
                opp--;                          /* Unstack the operator */
#ifdef  DEBUG_EVAL
                printf("Stack before evaluation of %s\n", opname[op1]);
                dumpstack(opstack, opp, value, valp);
#endif
                valp = evaleval(valp, op1, skip);
#ifdef  DEBUG_EVAL
                printf("Stack after evaluation\n");
                dumpstack(opstack, opp, value, valp);
#endif
            }                                   /* op1 switch end       */
        }                                       /* Stack unwind loop    */
}
























FILE_LOCAL int
evallex(skip)
int             skip;           /* TRUE if short-circuit evaluation     */
/*
 * Return next eval operator or value.  Called from eval().  It
 * calls a special-purpose routines for 'char' strings and
 * numeric values:
 * evalchar     called to evaluate 'x'
 * evalnum      called to evaluate numbers.
 */
{
        register int    c, c1, t;
        extern double   evalnum();

again:  do {                                    /* Collect the token    */
            c = skipws();
            if ((c = macroid(c)) == EOF_CHAR || c == '\n') {
                unget();
                return (OP_EOE);                /* End of expression    */
            }
        } while ((t = type[c]) == LET && catenate());
        if (t == INV) {                         /* Total nonsense       */
            if (!skip) {
                if (isascii(c) && isprint(c))
                    cierror("illegal character '%c' in #if", c);
                else
                    cierror("illegal character (%d decimal) in #if", c);
            }
            return (OP_FAIL);
        }
        else if (t == QUO) {                    /* ' or "               */
            if (c == '\'') {                    /* Character constant   */
                evalue = evalchar(skip);        /* Somewhat messy       */
#ifdef  DEBUG_EVAL
                printf("evalchar returns %d.\n", evalue);
#endif
                return (DIG);                   /* Return a value       */
            }
            cerror("Can't use a string in an #if", NULLST);
            return (OP_FAIL);
        }
        else if (t == LET) {                    /* ID must be a macro   */
            if (streq(token, "defined")) {      /* Or defined name      */
                c1 = c = skipws();
                if (c == '(')                   /* Allow defined(name)  */
                    c = skipws();
                if (type[c] == LET) {
                    evalue = (lookid(c) != NULL);
                    if (c1 != '('               /* Need to balance      */
                     || skipws() == ')')        /* Did we balance?      */
                        return (DIG);           /* Parsed ok            */
                }
                cerror("Bad #if ... defined() syntax", NULLST);
                return (OP_FAIL);
            }
            else if (streq(token, "sizeof"))    /* New sizeof hackery   */
                return (dosizeof());            /* Gets own routine     */
            /*
             * The Draft ANSI C Standard says that an undefined symbol
             * in an #if has the value zero.  We are a bit pickier,
             * warning except where the programmer was careful to write
             *          #if defined(foo) ? foo : 0
             */
            if (!skip)
                cwarn("undefined symbol \"%s\" in #if, 0 used", token);
            evalue = 0;
            return (DIG);
        }
        else if (t == DIG) {                    /* Numbers are harder   */
            evalue = evalnum(c);
#ifdef  DEBUG_EVAL
            printf("evalnum returns %d.\n", evalue);
#endif
        }
        else if (c == '.') {
                c = cget();
                if (isascii(c) && isdigit(c)) {
                        unget();
                        evalue = evalnum('.');
                        return DIG;
                }
                unget();
        }
        else if (strchr("!=<>&|\\", c) != NULL) {
            /*
             * Process a possible multi-byte lexeme.
             */
            c1 = cget();                        /* Peek at next char    */
            switch (c) {
            case '!':
                if (c1 == '=')
                    return (OP_NE);
                break;

            case '=':
                if (c1 != '=') {                /* Can't say a=b in #if */
                    unget();
                    cerror("= not allowed in #if", NULLST);
                    return (OP_FAIL);
                }
                return (OP_EQ);

            case '>':
            case '<':
                if (c1 == c)
                    return ((c == '<') ? OP_ASL : OP_ASR);
                else if (c1 == '=')
                    return ((c == '<') ? OP_LE  : OP_GE);
                break;

            case '|':
            case '&':
                if (c1 == c)
                    return ((c == '|') ? OP_ORO : OP_ANA);
                break;

            case '\\':
                if (c1 == '\n')                 /* Multi-line if        */
                    goto again;
                cerror("Unexpected \\ in #if", NULLST);
                return (OP_FAIL);
            }
            unget();
        }
        return (t);
}


















FILE_LOCAL int
dosizeof()
/*
 * Process the sizeof (basic type) operation in an #if string.
 * Sets evalue to the size and returns
 *      DIG             success
 *      OP_FAIL         bad parse or something.
 */
{
        register int    c;
        register TYPES  *tp;
        register SIZES  *sizp;
        short           typecode;

        if ((c = skipws()) != '(')
            goto nogood;
        /*
         * Scan off the tokens.
         */
        typecode = 0;
        while ((c = skipws())) {
            if ((c = macroid(c)) == EOF_CHAR || c == '\n')
                goto nogood;                    /* End of line is a bug */
            else if (c == '(') {                /* thing (*)() func ptr */
                if (skipws() == '*'
                 && skipws() == ')') {          /* We found (*)         */
                    if (skipws() != '(')        /* Let () be optional   */
                        unget();
                    else if (skipws() != ')')
                        goto nogood;
                    typecode |= T_FPTR;         /* Function pointer     */
                }
                else {                          /* Junk is a bug        */
                    goto nogood;
                }
            }
            else if (type[c] != LET)            /* Exit if not a type   */
                break;
            else if (!catenate()) {             /* Maybe combine tokens */
                /*
                 * Look for this unexpandable token in basic_types.
                 */
                for (tp = basic_types; tp->name != NULLST; tp++) {
                    if (streq(token, tp->name))
                        break;
                }
                if (tp->name == NULLST) {
                    cerror("#if sizeof, unknown type \"%s\"", token);
                    return (OP_FAIL);
                }
                if ((typecode & tp->excluded) != 0) {
                    cerror("#if sizeof: illegal type combination", NULLST);
                    return (OP_FAIL);
                }
                typecode |= tp->type;           /* Or in the type bit   */
            }
        }
        /*
         * We are at the end of the type scan.  Chew off '*' if necessary.
         */
        if (c == '*') {
            typecode |= T_PTR;
            c = skipws();
        }
        if (c == ')') {                         /* Last syntax check    */
            /*
             * We assume that all function pointers are the same size:
             *          sizeof (int (*)()) == sizeof (float (*)())
             * We assume that signed and unsigned don't change the size:
             *          sizeof (signed int) == (sizeof unsigned int)
             */
            if ((typecode & T_FPTR) != 0)       /* Function pointer     */
                typecode = T_FPTR | T_PTR;
            else {                              /* Var or var * datum   */
                typecode &= ~(T_SIGNED | T_UNSIGNED);
                if ((typecode & (T_SHORT | T_LONG)) != 0)
                    typecode &= ~T_INT;
            }
            if ((typecode & ~T_PTR) == 0) {
                cerror("#if sizeof() error, no type specified", NULLST);
                return (OP_FAIL);
            }
            /*
             * Exactly one bit (and possibly T_PTR) may be set.
             */
            for (sizp = size_table; sizp->bits != 0; sizp++) {
                if ((typecode & ~T_PTR) == sizp->bits) {
                    evalue = ((typecode & T_PTR) != 0)
                        ? sizp->psize : sizp->size;
                    return (DIG);
                }
            }                                   /* We shouldn't fail    */
            cierror("#if ... sizeof: bug, unknown type code 0x%x", typecode);
            return (OP_FAIL);
        }

nogood: unget();
        cerror("#if ... sizeof() syntax error", NULLST);
        return (OP_FAIL);
}




















FILE_LOCAL double
e(n)
register int    n;
{
        register double result;
        register int    isneg;

        isneg = n < 0;
        if (isneg)
                n = -n;
        result = 1;
        while (n > 0) {
                result *= 10;
                --n;
        }
        return isneg ? (1 / result) : result;
}

FILE_LOCAL int
ctoi(c)
register int    c;
{
        register char * cp;
        register char * dp;
        extern char *   index();

        if (isascii(c) && isupper(c))
                c = tolower(c);
        cp = "0123456789abcdef";
        dp = index(cp, c);
        return (dp == 0) ? -1 : (dp - cp);
}

FILE_LOCAL double;
purenumb(base, resultp)
double *        resultp;
{
        register int    digits;
        register int    new;
        register int    c;

        *resultp = 0;
        digits = 0;
        for ( ; ; ) {
                c = cget();
                new = ctoi(c);
                if (new < 0 || new >= base) {
                        unget();
                        return digits;
                }
                *resultp *= base;
                *resultp += new;
                ++digits;
        }
}

FILE_LOCAL double
evalnum(c)
register int    c;
/*
 * Expand number for #if lexical analysis.  Note: evalnum recognizes
 * the unsigned suffix, but only returns a signed int value.
 * c may be '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', or '.'.
 */
{
        register int    base;
        register int    ucount, lcount;
        register int    expsignchar;
        register double result, fracpart, exppart;
        register double divisor;
        register int    intdigs, fracdigs;

        if (c != '0') {
                base = 10;
                unget();
        } else if ((c = cget()) == 'x' || c == 'X') {
                base = 16;
        } else {
                base = 8;
                unget();
        }
        intdigs = purenumb(base, &result);
        c = cget();
        if (c != '.' && c != 'e' && c != 'E') {
                if (base == 16 && intdigs == 0) {
                        /* "0[xX][^0-9a-fA-F]" */
                        unget();        /* whatever follows the 'x' or 'X' */
                        unget();        /* the 'x' or 'X' */
                        return 0;
                }
                ucount = lcount = 0;
                for ( ; ; ) {
                        switch (c) {
                                case 'u': case 'U':
                                        if (ucount != 0)
                                                break;
                                        ++ucount;
                                        c = cget();
                                        continue;
                                case 'l': case 'L':
                                        if (lcount != 0)
                                                break;
                                        ++lcount;
                                        c = cget();
                                        continue;
                        }
                        break;
                }
                unget();
                return result;
        }
        /* We get here if c == '.'  || c == 'e' || c == 'E' */
        if (base != 10) {
                if (base == 16 && intdigs == 0) {
                        /* "0[xX]." */
                        unget();        /* the '.' */
                        unget();        /* the 'x' or 'X' */
                        return 0;
                }
                unget();
                return result;
        }
        if (c == '.') {
                fracdigs = purenumb(10, &fracpart);
                if (fracdigs != 0)
                        result += fracpart / e(fracdigs);
                if ((c = cget()) != 'e' && c != 'E') {
                        unget();
                        return result;
                }
        }
        if ((c = cget()) == '-' || c == '+')
                expsignchar = c;
        else {
                expsignchar = '\0';
                unget();
        }
        if (purenumb(10, &exppart) <= 0) {
                if (expsignchar != '\0')
                        unget();
                unget();        /* the 'e' or 'E' */
                return result;
        }
        return result * e((expsignchar == '-') ? (int) -exppart : (int) exppart);
}






















FILE_LOCAL int
evalchar(skip)
int             skip;           /* TRUE if short-circuit evaluation     */
/*
 * Get a character constant
 */
{
        register int    c;
        register int    value;
#if BIG_ENDIAN
        register int    count;
#endif

        instring = TRUE;
        if ((value = evalone()) == EOF_CHAR) {
            switch (cget()) {
            case '\'':                  /* Empty char constant          */
                cerror("Empty character constant", NULLST);
                return (0);

            /* case '\n':               -- Unterminated char constant   */
            default:                    /* Junk or eof                  */
                cerror("Unterminated character constant", NULLST);
                return (0);
            }
        }
        /*
         * We warn on multi-byte constants and try to hack
         * (big|little)endian machines.
         */
#if BIG_ENDIAN
        count = 0;
#endif
        while ((c = evalone()) != EOF_CHAR) {
            if (!skip)
                ciwarn("multi-byte constant '%c' isn't portable", c);
#if BIG_ENDIAN
            count += BITS_CHAR;
            value += (c << count);
#else
            value <<= BITS_CHAR;
            value += c;
#endif
        }
        switch (cget()) {
        case '\'':                      /* Normal char termination      */
            break;

        /* case '\n':                   -- End of line seen             */
        default:                        /* Junk or end of file seen     */
            cerror("Unterminated multi-character constant", NULLST);
            break;
        }
        instring = FALSE;
        return (value);
}
















FILE_LOCAL int
evalone()
/*
 * Called from evalchar() above to get a single character with \ escapes.
 * Returns the character or EOF_CHAR (on errors).
 */
{
        register char   *cp;
        register char   *digits;
        register int    c;
        register int    value;
        register int    count;
        extern char     *strchr();

        switch (c = cget()) {
        default:
            return (c);

        case '\n':
        case '\'':
            unget();
            return (EOF_CHAR);                  /* Exits mult-char loop */

        case '\\':                              /* \ escape seen        */
            break;
        }
        switch (c = cget()) {
        case 'a':                               /* New in Standard      */
#if ('a' == '\a' || '\a' == ALERT)
            return (ALERT);                     /* Use predefined value */
#else
            return ('\a');                      /* Use compiler's value */
#endif

        case 'b':
            return ('\b');

        case 'f':
            return ('\f');

        case 'n':
            return ('\n');

        case 'r':
            return ('\r');

        case 't':
            return ('\t');

        case 'v':                               /* New in Standard      */
#if ('v' == '\v' || '\v' == VT)
            return (VT);                        /* Use predefined value */
#else
            return ('\v');                      /* Use compiler's value */
#endif

        case 'x':                               /* '\xFF'               */
            digits = "0123456789abcdef";
            c = cget();
            break;

        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7':
            digits = "01234567";
            break;

        default:
            return (c);
        }
        value = 0;
        for (count = 0; count < 3; ++count) {
            if (c == EOF_CHAR)
                return (c);
            if (isascii(c) && isupper(c))
                c = tolower(c);
            if ((cp = strchr(digits, c)) == 0)
                break;
            value = value * strlen(digits) + cp - digits;
            c = cget();
        }
        unget();
        if (count == 0)                         /* '\xnonsense'         */
            return (EOF_CHAR);
        return (value);
}











FILE_LOCAL double *
evaleval(valp, op, skip)
register double *valp;
int             op;
int             skip;           /* TRUE if short-circuit evaluation     */
/*
 * Apply the argument operator to the data on the value stack.
 * One or two values are popped from the value stack and the result
 * is pushed onto the value stack.
 *
 * OP_COL is a special case.
 *
 * evaleval() returns the new pointer to the top of the value stack.
 */
{
        register double v1, v2;

        if (isbinary(op))
            v2 = *--valp;
        v1 = *--valp;
#ifdef  DEBUG_EVAL
        printf("%s op %s", (isbinary(op)) ? "binary" : "unary",
            opname[op]);
        if (isbinary(op))
            printf(", v2 = %f.", v2);
        printf(", v1 = %f.\n", v1);
#endif
        switch (op) {
        case OP_EOE:
             break;

        case OP_ADD:
            v1 += v2;
            break;

        case OP_SUB:
            v1 -= v2;
            break;

        case OP_MUL:
            v1 *= v2;
            break;

        case OP_DIV:
        case OP_MOD:
            if (v2 == 0) {
                if (!skip) {
                    cwarn("%s by zero in #if, zero result assumed",
                        (op == OP_DIV) ? "divide" : "mod");
                }
                v1 = 0;
            }
            else if (op == OP_DIV)
                v1 /= v2;
            else
                v1 = (long) v1 % (long) v2;
            break;

        case OP_ASL:
            v1 = (long) v1 << (long) v2;
            break;

        case OP_ASR:
            v1 = (long) v1 >> (long) v2;
            break;

        case OP_AND:
            v1 = (long) v1 & (long) v2;
            break;

        case OP_OR:
            v1 = (long) v1 | (long) v2;
            break;

        case OP_XOR:
            v1 = (long) v1 ^ (long) v2;
            break;

        case OP_EQ:
            v1 = (v1 == v2);
            break;

        case OP_NE:
            v1 = (v1 != v2);
            break;

        case OP_LT:
            v1 = (v1 < v2);
            break;

        case OP_LE:
            v1 = (v1 <= v2);
            break;

        case OP_GE:
            v1 = (v1 >= v2);
            break;

        case OP_GT:
            v1 = (v1 > v2);
            break;

        case OP_ANA:
            v1 = (v1 && v2);
            break;

        case OP_ORO:
            v1 = (v1 || v2);
            break;

        case OP_COL:
            /*
             * v1 has the "true" value, v2 the "false" value.
             * The top of the value stack has the test.
             */
            v1 = (*--valp) ? v1 : v2;
            break;

        case OP_NEG:
            v1 = (-v1);
            break;

        case OP_PLU:
            break;

        case OP_COM:
            v1 = ~((long) v1);
            break;

        case OP_NOT:
            v1 = !v1;
            break;

        default:
            cierror("#if bug, operand = %d.", op);
            v1 = 0;
        }
        *valp++ = v1;
        return (valp);
}




#ifdef  DEBUG_EVAL
dumpstack(opstack, opp, value, valp)
OPTAB           opstack[NEXP];  /* Operand stack                */
register OPTAB  *opp;           /* Operator stack               */
double          value[NEXP];    /* Value stack                  */
register double *valp;          /* -> value vector              */
{
        printf("index op prec skip name -- op stack at %s", infile->bptr);
        while (opp > opstack) {
            printf(" [%2d] %2d  %03o    %d %s\n", opp - opstack,
                opp->op, opp->prec, opp->skip, opname[opp->op]);
            opp--;
        }
        while (--valp >= value) {
            printf("value[%d] = %f\n", (valp - value), *valp);
        }
}
#endif






From:   RHEA::DECWRL::"elsie!ado@seismo.ARPA" 11-JUN-1985 15:58
To:     seismo!minow%rex.DEC@decwrl
Subj:   cpp5.c delinting

Received: from DECWRL by DEC-RHEA with SMTP; Tue, 11 Jun 85 12:58-PDT
Received: from seismo.ARPA (seismo.arpa.ARPA) by decwrl.ARPA (4.22.01/4.7.34)
        id AA21249; Tue, 11 Jun 85 12:57:52 pdt
Received: from elsie.UUCP by seismo.ARPA with UUCP; Tue, 11 Jun 85 15:57:31 EDT
Date: Tue, 11 Jun 85 15:57:31 EDT
Return-Path: <elsie!ado@seismo.ARPA>
Message-Id: <8506111957.AA14370@seismo.ARPA>

A couple of delinting changes to the new "cpp5.c". . .changes to the function
"purenumb" and to the declarations in "evalnum":

        FILE_LOCAL double
        purenumb(base, resultp)
        double *        resultp;
        {
                register int    digits;
                register int    new;
                register int    c;

                *resultp = 0;
                digits = 0;
                for ( ; ; ) {
                        c = cget();
                        new = ctoi(c);
                        if (new < 0 || new >= base) {
                                unget();
                                return digits;
                        }
                        *resultp = *resultp * base + new;
                        ++digits;
                }
        }

        FILE_LOCAL double
        evalnum(c)
        register int    c;
        /*
         * Expand number for #if lexical analysis.  Note: evalnum recognizes
         * the unsigned suffix, but only returns a signed int value.
         * c may be '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', or '.'.
         */
        {
                register int    base;
                register int    ucount, lcount;
                register int    expsignchar;
                double          result, fracpart, exppart;
                register int    intdigs, fracdigs;

                                --ado

