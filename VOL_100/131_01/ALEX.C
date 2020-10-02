/*
	************************************************************
			      ACRL Scanner
	************************************************************

	W. Lemiszki					  9 Jan 1982

	Filename: alex.c				 BDS C v1.50
*/

#include "acrl.h"

/*
	Scans the next token from the current line, stores the
	token in tokbuf, and sets the global variables 'class', 'type',
	'value', and 'reloc'.  For ASCII character tokens, 'type' is
	the ASCII code.
*/

scan()
{
L1:	value = reloc = 0;

	while ((type = *lptr++) == ' '  ||	/* skip spaces */
		type == '\t' ||			/* tabs */
		type == '\f'   )		/* and formfeeds */
		;

	if (type == '\n'  ||  type == ';')
		{
		lptr--;				/* never scan past EOL */
		class = EOL;
		return;
		}

	switch (class = cclass(type))
	    {
	    case 0:				/* Illegal character */
		error('I');
		goto L1;			/* scan next */

	    case 'A':				/* Identifier */
		identifier();
		break;

	    case '1':				/* Number */
		number();
		break;

	    case '$' :				/* String */
		string();
		break;

	    default:				/* 1 char token */
		break;
	    }
}




/*
 *	Scan an identifier
 *	------------------
 */
identifier()
{
	idn();					/* ident to tokbuf */
	if (look(tokbuf))			/* defined ? */
		{
		class = id->clas;
		type = id->typ;
		value = id->val;
		}
	    else				/* undefined */
		{
		class = IDENT;
		type = idUND;
		}
	if (type & RELBIT)			/* relocatable ? */
		{
		type ^= RELBIT;			/* turn bit off */
		reloc = TRUE;			/* set global */
		}
}



/* Copy chars into 'tokbuf' until non-alphanumeric */
idn()
{
	char cc, *ptr;

	ptr = tokbuf;				/* move into token buffer */
	do {
	    *ptr++ = toupper(type);
	    } while ((cc=cclass(type=*lptr++)) == 'A' || cc == '1');
	lptr--;					/* backup */
	*ptr = EOS;
}



/* Check the symbol tables */
int look(s)
char *s;
{
	if (lookup(RESVD, s))			/* reserved word ? */
		return TRUE;
	if (infunc  &&  lookup(LOCAL, s))	/* local symbol ? */
		return TRUE;
	if (lookup(GLOBAL, s))			/* global symbol ? */
		return TRUE;
	if (!infunc  &&  lookup(FUNCS, s))	/* function name ? */
		return TRUE;
	return FALSE;
}




/*
 *	Scan a number
 *	-------------
 */
number()
{
	char c, *ptr;
	int radix;

	value = 0;				/* init */
	radix = 10;

	idn();					/* ident to tokbuf */
	for (ptr=tokbuf; *++ptr; )		/* scan to end of string */
		;
	c = *--ptr;				/* get last char */
	*ptr = EOS;				/* erase it */
	switch (c)				/* check if radix char */
	    {
	    case 'H':  radix = 16;  break;
	    case 'D':  radix = 10;  break;
	    case 'Q':
	    case 'O':  radix =  8;  break;
	    case 'B':  radix =  2;  break;
	    default :  *ptr = c;		/* no, put it back */
	    }

	for (ptr = tokbuf;  c = *ptr++; )	/* convert tokbuf */
		{
		if ((c -= '0') > 9)		/* ascii to bcd */
			c -= 7;
		if (c >= radix)
			error ('I');
		value = value * radix + c;
		}

	class = OPERAND;
	type = NUMBER;
}




/*
 *	Scan a quoted string
 *	--------------------
 *	From input buffer into 'tokbuf'
 */
string()
{
	char c, *ptr;

	for (ptr=tokbuf;  (c=*lptr) != '\n';  )
	    {
	    lptr++;
	    if (c == '\'')			/* quote ? */
		if (*lptr == '\'')		/* double quote ? */
		    lptr++;			/* just move 1 */
		  else
		    break;			/* delim */
	    *ptr++ = c;
	    }
	*ptr = EOS;				/* terminate string */
	class = STRING;
}




/*
	CHARACTER CLASSES

	Tokens are scanned based on their 1st character.  The following
	values (from cctab[]) indicate that further action is necessary
	to complete scanning this token.  Otherwise it's a 1 char token,
	and cclass returns its token class. Its type is its ASCII code.
	 0  -	illegal character
	'A' -	first char of identifier
	'1' -	first char of number
	'$' -	string delimitter (')
*/

/* Return character class */
byte cclass(c)
char c;
{
	if (c < 128)
		return (cctab[c]);		/* from table */
	    else
		return (0);			/* illegal */
}



/*
 *	Init the character class table
 *	------------------------------
 */
initctab()
{
	setmem(cctab, 128, 0);			/* fill with zeros */

	setmem(&cctab['A'], 26, 'A');		/* upper case letters */
	setmem(&cctab['a'], 26, 'A');		/* lower case letters */
	cctab['?'] = 'A';
	cctab['@'] = 'A';

	setmem(&cctab['0'], 10, '1');		/* digits */

	cctab['\''] = '$';			/* quote */

	cctab[':'] = PSEUDO;
	cctab['*'] = OPERATOR;
	cctab['+'] = OPERATOR;
	cctab['-'] = OPERATOR;
	cctab['/'] = OPERATOR;
	cctab['$'] = OPERAND;
	cctab['('] = OPERAND;
	cctab[')'] = MISC;
	cctab[','] = MISC;
}




/*
	SYMBOL TABLE ROUTINES

	Symbols are stored as linked lists of 'entry's, with the head
	of each list stored in the 'roots[]' array.  To speed up access
	of reserved words, 256 lists are allocated for this table, and
	a simple hash function is used to select the list for any word.
*/


/*
 *	Lookup an identifier in a table
 *	-------------------------------
 *	If found, the global pointer 'id' is left pointing to
 *	the entry.
 */
int lookup(level, str)
int level;					/* which table to use */
char *str;					/* ptr to ident string */
{
	if (level == RESVD)			/* reserved word ? */
		level += hash(str);
	if (level == LOCAL)			/* local symbol ? */
		level += numfunc;

	for (id = roots[level];  id != NULL;  id = id->next)
		if (strcmp(str, id->nam) == 0)
			return TRUE;
	return FALSE;
}



/* Hash function:  sum the chars */
byte hash(str)
char *str;					/* ident string */
{
	int i;

	for (i=0;  *str;  i += *str++)		/* sum up the chars */
		;
	return (i & 255);			/* mod 256 */
}




/*
 *	Add a symbol into a table
 *	-------------------------
 *	This function assumes the identifier name is static or
 *	already strsav()'d.
 */
install(level, name, class, type, value)
int level;					/* the table to use */
char *name;					/* the identifier string */
byte class;					/* the symbol's class */
byte type;					/* the symbol's type */
int  value;					/* the symbol's value */
{
	char *alloc();

	id = alloc(sizeof(*id));		/* first make an entry */
	id->nam = name;				/* store parameters */
	id->clas = class;
	id->typ = type;
	id->val = value;

	if (level == RESVD)			/* reserved word ? */
		level += hash(name);
	if (level == LOCAL)			/* local symbol ? */
		level += numfunc;

	id->next  =  roots[level];		/* add to head of list */
	roots[level]  =  id;
}



/* Save a string in upper memory */
char *strsave(s)
char *s;					/* string pointer */
{
	char *p;

	p = alloc(strlen(s) + 1);		/* get memory */
	strcpy(p, s);				/* move string */
	return (p);
}


/* Allocate memory */
char *alloc(n)
int n;						/* number of bytes */
{
	char *p, *sbrk();

	if ((p=sbrk(n)) == ERROR)
		fatal("Out of memory\n");
	return (p);
}




/*
 *	Init the symbol tables
 *	----------------------
 */
initsymb()
{
	int i;

	for (i = 0;  i < ROOTSIZ;  ++i)		/* init all lists empty */
		roots[i] = NULL;

	install(RESVD,	"AND",	    OPERATOR,	opAND,	0);
	install(RESVD,	"OR",	    OPERATOR,	opOR,	0);
	install(RESVD,	"XOR",	    OPERATOR,	opXOR,	0);
	install(RESVD,	"HIGH",	    OPERAND,	opHIGH,	0);
	install(RESVD,	"LOW",	    OPERAND,	opLOW,	0);
	install(RESVD,	"SHL",	    OPERATOR,	opSHL,	0);
	install(RESVD,	"SHR",	    OPERATOR,	opSHR,	0);
	install(RESVD,	"MOD",	    OPERATOR,	opMOD,	0);
	install(RESVD,	"NOT",	    OPERAND,	opNOT,	0);

	install(RESVD,	"DB",	    PSEUDO,	psDB,	0);
	install(RESVD,	"DS",	    PSEUDO,	psDS,	0);
	install(RESVD,	"DW",	    PSEUDO,	psDW,	0);
	install(RESVD,	"END",	    PSEUDO,	psEND,	0);
	install(RESVD,	"EQU",	    PSEUDO,	psEQU,	0);
	install(RESVD,	"SET",	    PSEUDO,	psSET,	0);
	install(RESVD,	"FUNCTION", PSEUDO,	psFUNC,	0);
	install(RESVD,	"FEND",	    PSEUDO,	psFEND,	0);
	install(RESVD,	"INCLUDE",  PSEUDO,	psINCL,	0);
}


/*EOF*/
