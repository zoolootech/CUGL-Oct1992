/*
$title ('yam10.c: file display, unsqueeze')
$date (15 AUG 85)
*/
/*
 * File unsqueezer module from Richard Greenlaw's USQ
 *  and other vital functions
*/
#include "yam.h"

/* *** Stuff for first translation module *** */
#define USDLE 0x90
/* *** Stuff for second translation module *** */
#define SPEOF 256	/* special endfile token */
#define LARGE 30000
int bpos;	/* last bit position read */
int curin;	/* last byte value read */
/* Variables associated with repetition decoding */
int repct;	/*Number of times to return value*/
int value;	/*current byte value or EOF */

/****************************************************************************
FUNCTION:
	unsqueeze a squeezed file to the crt.

CALLING PARAMETERS:
	none
===========================================================================*/
unsqueeze()
{
	int i, c;
	int numnodes;		/* size of decoding tree */

	/* Initialization */
	repct = 0;
	bpos = 99;	/* force initial read */

	/* Process rest of header (SQMAGIC already read) */
	getw(fin);	/* ignore checksum ... */

	/* List current and original file names */
	fprintf(stderr, "%s -> ", Tname);
	while(c = getc(fin))
		putc(c, stderr);
	putc('\n', stderr);

	numnodes = getw(fin);
	if(numnodes < 0 || numnodes >= NUMVALS)
	{
		printf(stderr, "%s has invalid decode tree size\n", Tname);
		return ERROR;
	}

	/* Initialize for possible empty tree (SPEOF only) */
	Utility.dnode[0].children[0] = -(SPEOF + 1);
	Utility.dnode[0].children[1] = -(SPEOF + 1);

	/* Get decoding tree from file */
	for(i = 0; i < numnodes; ++i)
	{
		Utility.dnode[i].children[0] = getw(fin);
		Utility.dnode[i].children[1] = getw(fin);
	}

	while((c = getcr()) != CPMEOF)
	{
		if( !(c=putcty(c)))
			continue;
		if(c==003 || c==CAN || c==013)
		{
			return c;
		}
	}
	return CPMEOF;
} /* unsqueeze */


/****************************************************************************
FUNCTION:
	Get bytes with decoding - this decodes repetition,
	calls getuhuff to decode file stream into byte
	level code with only repetition encoding.
	The code is simple passing through of bytes except
	that USDLE is encoded as USDLE-zero and other values
	repeated more than twice are encoded as value-USDLE-count.

CALLING PARAMETERS:
	none
===========================================================================*/
int getcr()
{
	int c;

	if(repct > 0)
	{
		/* Expanding a repeated char */
		--repct;
		return value;
	}
	else
	{
		/* Nothing unusual */
		if((c = getuhuff()) != USDLE)
		{
			/* It's not the special delimiter */
			value = c;
			if(value == CPMEOF)
				repct = LARGE;
			return value;
		}
		else
		{
			/* Special token */
			if((repct = getuhuff()) == 0)
				/* USDLE, zero represents USDLE */
				return USDLE;
			else
			{
				/* Begin expanding repetition */
				repct -= 2;	/* 2nd time */
				return value;
			}
		}
	}
}

/****************************************************************************
FUNCTION:
	Decode file stream into a byte level code with only
	repetition encoding remaining.

CALLING PARAMETERS:
	none
===========================================================================*/
int
getuhuff()
{
	int i;

	/* Follow bit stream in tree to a leaf*/
	i = 0;	/* Start at root of tree */
	do
	{
		if(++bpos > 7)
		{
			if((curin = getc(fin)) == ERROR)
				return ERROR;
			bpos = 0;
			/* move a level deeper in tree */
			i = Utility.dnode[i].children[1 & curin];
		} else
			i = Utility.dnode[i].children[1 & (curin >>= 1)];
	} while(i >= 0);

	/* Decode fake node index to original data value */
	i = -(i + 1);
	/* Decode special endfile token to normal EOF */
	i = (i == SPEOF) ? CPMEOF : i;
	return i;
}


/****************************************************************************
FUNCTION:
	put char to console.  Characters are checked for rxnono and not output
	if in this string.  Tabs are expanded and ^S is tested for.

CALLING PARAMETERS:
	c:
		character to output to console.
===========================================================================*/
char putcty(c)
char c;
{
	if(index(c, rxnono))
		return FALSE;
	while (!COREADY)
		;

	switch(c)
	{
			/* expand tabs */
		case '\t':
			c=' ';		
			while (++Ttycol % tabstop)
				CONOUT(' ');		
			break;
		case '\r':
		case '\n':
			Ttycol=0;
			break;
		default:
			++Ttycol;
	}
	CONOUT(c);

	if(CIREADY)
	{
		c = (char)CICHAR;
			/* pause on ^S */
		if (c==xoff)
		{
			while (!CIREADY)
				;
			return FALSE;
		}
		else
			return c;
	}
	else
		return FALSE;
} /* putcty */


/****************************************************************************
FUNCTION:
	get char from console.

CALLING PARAMETERS:
	none
===========================================================================*/
char getcty()
{
	while (!CIREADY)
		;
	return (char)(CICHAR&KBMASK);
} /* getcty */


/****************************************************************************
FUNCTION:
	display a file to the crt.  This routine will open and close the file.
	if USQ is set, squeezed files may be typed.

CALLING PARAMETERS:
	f_buf:
		pointer to a structure containing information about file.  This
		is not used, but supplied by expand.
	*pathname:
		pointer to directory path name
===========================================================================*/
listfile(f_buf,pathname)
struct find_buf *f_buf;
char *pathname;
{
	int c;

		/* reset tab stop counter */
	Ttycol = 0;
	closetx(TRUE);

		/* open file. open will not compute or display file size */
	if(opentx(NULL,NULL,pathname)==ERROR)
		return ERROR;
#ifdef XMODEM
	sendbyte(022);		/* bracket with ^R and ^T for squelch */
#endif
#ifdef USQ
	c = getw(fin);
	if(c==SQMAGIC)
		c=unsqueeze();

		/* wasn't a squeezed file, display the first 2 chars fetched */
	putcty(c);
	putcty(c>>8);
#endif
	while( ((c=getc(fin)) != EOF) && (c != CPMEOF) )
	{
			/* returns false if no key pressed */
		if( !(c=putcty(c)&0x7f))
			continue;

			/* ^C=abort, ^X=next file */
		if(c==ETX || c==CAN)
				break;
	}
	if(c != CPMEOF)
		putchar('\n');

	/* record complete xmsn if terminated by (CPM)EOF */
	closetx(c != CPMEOF);
#ifdef XMODEM
	sendbyte(024);	/* squelch in case user downloading */
#endif		
	/* cancel rest of files if ^C */
	if(c==ETX)
		return ERROR;
	else
		return OK;
} /* listfile */


/****************************************************************************
FUNCTION:
	get a word from a buffer input file by 2 consective calls to getc
	This call does not exist in lattice C.

CALLING PARAMETERS:
	fin:
		pointer to FILE variable.
===========================================================================*/
getw(fp)
FILE *fp;
{
	union
	{
		char cb[2];
		int cw;
	} cword;

	cword.cb[0] = (char)getc(fp);
	cword.cb[1] = (char)getc(fp);
	return cword.cw;
} /* getw */


/****************************************************************************
FUNCTION:
	fill buf with count chars padding with ^Z for CPM

CALLING PARAMETERS:
	buf:
		pointer to buffer
	count:
		number of bytes to pad buffer with
===========================================================================*/
filbuf(buf, count)
char *buf;
{
	register c, m;
	m=count;
	while((c=getc(fin))!=EOF)
	{
		*buf++ =c;
		if(--m == 0)
			break;
	}
	if(m==count)
		return 0;
	else
		while(--m>=0)
			*buf++ = 032;
	return count;
} /* filbuf */


/****************************************************************************
FUNCTION:
	compare two strings of characters.  If equal return true, else false

CALLING PARAMETERS:
	s:
		pointer to first string
	p:
		pointer to second string
===========================================================================*/
cmdeq(s,p)
char *s, *p;
{
	while(*p)
		if(*s++ != *p++)
			return FALSE;
	return TRUE;
} /* cmdeq */


/****************************************************************************
FUNCTION:
	local tell-it-like-it-is string write function with no kbd check

CALLING PARAMETERS:
	*s:
		pointer to string to place on screen
===========================================================================*/
lputs(s)
char *s;
{
	while(*s)
	{
#ifdef COREADY
		while (!COREADY)
			;
#endif
		CONOUT(*s++);
	}
} /* lputs */
