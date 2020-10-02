/*
	TMS9900/99105  Cross-Assembler  v. 1.0

	January, 1985

	Original 6800 version Copyright (c) 1980 William C. Colley, III.
	Modified for the TMS9900/99105  Series by Alexander Cameron.

File:	a99get.c

Routines to get source text from the disk and return it in
manageable chunks such as operators, labels, opcodes, etc.
*/

/*  Get Globals:  */

#include "a99.gbl"

/*
Function to get an opcode from the present source line and return
its value and attributes.  The function returns -1 if no opcode was
on the line, 0 if the opcode was not in the opcode table, and 1
otherwise.  If the opcode was on the line, but not in the table, the
error is marked up.
*/

getopcod(value,attrib)
char *attrib;
unsigned *value;
{
	char dxop[2*SYMLEN],temp[SYMLEN+1], getopc();
	switch (getchr(value,SKIP))
	{
		case ALPHA:	backchr;
				getname(temp);
				if (bsearch(temp,value,attrib,0,numopcs(),getopc)) return 1;
				else
				{	
					strcpy(dxop,temp);
					strcat(dxop,PADDING);
					if(slookup(dxop))
					{
						markerr('O');
						return 0;
					}
					*value = sympoint -> symvalu; /* DXOP's value */
					*attrib = 0x01;	/* uses format 1 */
					return 1;
				}

		default:	markerr('S');
				return -1;

		case END_LIN:	return 0;


	}
}

/*
Function to look up an item in a fixed table by binary search.  The function
returns 1 if the item was found, 0 if not.  name points to the name to be
looked up (null terminated).  value and attrib catch two bytes of parameters
attached to the item if the item is found.  high and low are for the benefit of
the recursion.  For the first call use the number of items in the table for
high and 0 for low.  table is a pointer to a function to get an item from the
appropriate table.  Note that in the comparison, lower case is converted to
upper case.
*/

bsearch(name,value,attrib,low,high,table)
char *name, *attrib, (*table)();
int low, high;
unsigned *value;
{
	char temp[5], *pntr1, *pntr2;
	int t, check;
	if (low > high) return 0;
	check = low + (high - low >> 1);
	(*table)(check,temp,value,attrib);
	pntr1 = temp;
	pntr2 = name;
	while ((t = toupper(*pntr2++) - *pntr1) == 0 && *pntr1++ != '\0');
	if (t == 0) return 1;
	if (t > 0) return bsearch(name,value,attrib,check+1,high,table);
	return bsearch(name,value,attrib,low,check-1,table);
}

/*
Function to push back an item so that getitem will retrieve it
a second time.  Only one level of pushback is supported.
*/

backitem(value,attrib)
unsigned value, attrib;
{
	oldvalu = value;
	oldattr = attrib;
	backflg = TRUE;
}

/*
Function to get an item from the present source line.  Items may be
looked-up symbols, numeric constants, string constants, operators, etc.
The function returns the attribute of the item gotten.  value is a
pointer to where the value of the item will be stored, and quoteflg
determines whether getitem will look for one- and two-character string
constants or not.
*/

getitem(value,quoteflg)
unsigned *value;
char quoteflg;
{
	char c, c1, attrib, *tmpptr, temp[2*SYMLEN];
	unsigned base, u;
	if (backflg)
	{
		backflg = FALSE;
		*value = oldvalu;
		return oldattr;
	}
	switch ((attrib = getchr(&c,SKIP)))
	{
		case END_LIN:
		case COMMA:	return attrib;

		case NUMERIC:	backchr;
				getnum(value,&base);
				return VALUE;

		case BAS_DES:	switch (c)
				{
					case '#':	base = 16;
							break;

					case '%':	base = 2;
				}
				*value = 0;
				while ((attrib = getchr(&c,NOSKIP)) == NUMERIC || attrib == ALPHA)
				{
					*value = *value * base + _aton(toupper(c),base);
				}
				backchr;
				return VALUE;

		case OPERATR:	switch (c)
				{
					case '>':
					case '=':
					case '<':	getchr(&c1,NOSKIP);
							if (c == '>' && c1 == '=') c = GRTEQ;
							else if (c == '>' && c1 == '<') c = NOTEQ;
							else if (c == '=' && c1 == '>') c = GRTEQ;
							else if (c == '=' && c1 == '<') c = LESEQ;
							else if (c == '<' && c1 == '>') c = GRTEQ;
							else if (c == '<' && c1 == '=') c = LESEQ;
							else backchr;

					default:	*value = c;
							return attrib;
				}

		case QUOTE:	if (quoteflg)
				{
					*value = c;
					return attrib;
				}
				tmpptr = temp;
				attrib = 0;
				while ((getchr(&c1,NOSKIP),c1) != '\n' &&  c1 != c)
					if (++attrib <= 2) *tmpptr++ = c1;
				if (c1 == '\n' || attrib > 2)
				{
					if (c1 == '\n') backchr;
					evalerr = TRUE;
					markerr('"');
					return VALUE;
				}
				tmpptr = temp;
				for (*value = 0; attrib != 0; attrib--) *value = (*value << 8) + *tmpptr++;
				return VALUE;

		case ALPHA:	backchr;
				getname(temp);
				if ((*value = chkoprat(temp) & 0xff) != NO_OPR) return OPERATR;
				strcat(temp,PADDING);
				if (slookup(temp) == 0)
				{
					*value = sympoint -> symvalu;
					if (sympoint -> symname[0] > 0x7f) directok = FALSE;
				}
				else
				{
					evalerr = TRUE;
					markerr('U');
				}
				return VALUE;
	}
}

/*
Function to get a symbol name, opcode, or label from the present source
line.  The name is returned (null terminated) in buffer.  Names are
terminated by any non-alphanumeric character.
*/

getname(buffer)
char *buffer;
{
	char c, d, count;
	count = 0;
	while ((c = getchr(&d,NOSKIP)) == ALPHA || c == NUMERIC)
		if (++count <= SYMLEN) *buffer++ = d;
	backchr;
	*buffer = '\0';
}

/*
Function to check a string against the list of operators that get spelled out.
This list consists of GE, GT, LE, LT, EQ, NE, AND, OR, XOR, NOT, MOD, SHL,
SHR, HIGH, LOW.  The function returns the token for the operator if the
string is an operator, the token NO_OPR otherwise.
*/

chkoprat(string)
char *string;
{
	char t, getopr();
	unsigned s;
	return (bsearch(string,&s,&t,0,numoprs(),getopr)) ? t : NO_OPR;
}

/*
Function to get an Intel format number from the present input line.
value points to the number to be returned, and base points to an
unsigned where the base of the number will be saved during internal
computations.  The function returns a fixed value of 1 and flags errors
as it goes along.
*/

getnum(number,base)
unsigned *number, *base;
{
	char c1, c2;
	unsigned b;
	if ((c1 = getchr(&c2,NOSKIP)) != ALPHA && c1 != NUMERIC)
	{
		backchr;
		return 0xffff;
	}
	if ((b = getnum(number,base)) == 0xffff)
	{
		*number = 0;
		switch (toupper(c2))
		{
			case 'H':	*base = 16;	return 1;
			case 'D':	*base = 10;	return 1;
			case 'O':
			case 'Q':	*base = 8;	return 1;
			case 'B':	*base = 2;	return 1;
			default:	*base = 10;	b = 1;
		}
	}
	*number += b * _aton(toupper(c2),*base);
	return b * *base;
}

/*
Internal function in getnum to convert a character to a digit.  The base
is passed so that this routine can check for digits larger than the base.
*/

_aton(letter,base)
char letter;
unsigned base;
{
	unsigned n;
	if (letter >= '0' && letter <= '9') n = letter - '0';
	else if (letter >= 'A' && letter <= 'F') n = letter - ('A' - 10);
	else
	{
		markerr('E');
		evalerr = TRUE;
		return 0;
	}
	if (n < base) return n;
	markerr('D');
	evalerr = TRUE;
	return 0;
}

/*
Function to get a character from the present source line.  The function
returns the attribute of the character fetched.  value is a pointer to
the location where the character itself will be saved, and skipflg
determines whether or not white space is skipped.
*/

getchr(value,skipflg)
char *value, skipflg;
{
	char c;
	while (TRUE) if ((c = getattr(*value = *linptr++)) != TRASH &&
			(c != BLANK || skipflg != SKIP)) return c;
}

/*
Function to get a new line of source text from the disk.  Returns 0
if EOF encountered, 1 otherwise.
*/

getlin()
{
	char count, c;
	linptr = linbuf;
	count = 0;
	backflg = FALSE;
	while (movchr(&c))
	{
		if (c == '\n')
		{
			*linptr = '\0';
			*--linptr = c;
			linptr = linbuf;
			return 1;
		}
		if (count < LINLEN) *linptr++ = c;
		count++;
	}
	return 0;
}

/*
Function to move a character from the source disk buffer to the
location dest.  Returns 0 if no character available, 1 otherwise.
*/

movchr(dest)
char *dest;
{
	int t;
	if (sorbuf.pointr >= sorbuf.space + (NSECT * 128))
	{
		sorbuf.pointr =sorbuf.space;
		if ((t = read(sorbuf.fd,sorbuf.space,NSECT)) == -1)
		{
			printf("\nDisk read error. ++ %s\n",errmsg(errno()));
			wipeout("\n");
		}
		if (t != NSECT) sorbuf.space[t * 128] = CPMEOF;
	}
	return((*dest = *sorbuf.pointr++ & 0x7f) == CPMEOF) ? 0 : 1;
}

/*
Function to rewind the source file.
*/

rewind()
{
	seek(sorbuf.fd,0,0);
	sorbuf.pointr = sorbuf.space + (NSECT * 128);
}

