/*
       HEADER:  150.??;
        TITLE:  ROFF - "RUNOFF" Text Formatter Program;
      VERSION:  5.1;
         DATE:  08/28/1982;
  DESCRIPTION:  "This is the second ROFF source file.  See ROFF1.C for the
                 complete description.";
     KEYWORDS:  ROFF, Text Formatter, Software Tools;
       SYSTEM:  any system with C compiler;
     FILENAME:  ROFF2.C;
     SEE-ALSO:  ROFF.DOC, ROFF.H, ROFF.HE, ROFF1.C, ROFF.EXE;
      AUTHORS:  modified "for IBM-PC" by M. S. Zachmann;
    COMPILERS:  BDS-C originally, IBM C compiler unknown;
   REFERENCES:  AUTHORS: M. S. Zachmann; TITLE: "ROFF";
                CITATION: "PC-SIG Disk 50 or 142";
                AUTHORS: Kernighan and Plauger; TITLE: "Software Tools";
                CITATION: "available in many computer stores" 
       ENDREF;
*/
/* 7 MAY 81 */

#include "stdio.h"
#include "ctype.h"
#include "roff.he"

/****************************************************************
handles case of leading blanks or tabs; empty lines 
******************************************************************/

leadbl (line)
char *line;
{
	int i, j;


	brk();

	/* find first non-blank */
	for( i=0; line[i] == BLANK; i++ );

	if ( line[i] != NEWLINE ) 
		TIVAL = i;
	while (line[i] == TAB)
	{ TIVAL += 8;
		i++;
	}

	/* move line to left */
	for (j=0; (line[j] = line[i]) != '\0'; j++, i++ );


	return;
}



/*******************************************************************
get indices of first and last occurrences of c in string
********************************************************************/
int find_char (string, c, first, last)
char *string, c;
int *first, *last;
{
	int i, j, k;

	*first = -1;
	*last = -1;
	for (i=0; string[i] != '\0'; i++)
	{
		if (string[i] == c)
		{ *last = i;
			if (*first == -1)	*first = i;
		}
	}
	return;
}

/***************************************************************
	replace c1 in string with c2
****************************************************************/
replace_char (string, c1, c2)
char *string, c1, c2;
{
	int i;

	for (i=0; string[i] != '\0'; i++)
		if (string[i] == c1)	string[i] = c2;

	return;
}


/********************************************************************
		produces n empty lines
*********************************************************************/
skip (n)
int n;
{
	int i;

	if DEBUG fprintf(stderr,"\n	SKIP %d line(s)", n);

	for ( i=0; i<n; i++)
	{ if DEBUG putchar('!');
		putchar(NEWLINE);
	}

	return;
}


/******************************************************************
	indents the proper number of spaces
*******************************************************************/
indent(val)
int val;
{
	int i;

	if DEBUG fprintf(stderr,"\n	INDENT %d spaces(s)",val);

	for ( i=0; i<val; i++ )		putchar( BLANK );


}



/*******************************************************************
		puts out page header
*******************************************************************/
phead()
{


	CURPAG = NEWPAG;
	NEWPAG++;

	if ( M1VAL > 0 )
	{ skip ( M1VAL - 1 );
		puttl ( HEADER, CURPAG );
	}

	skip ( M2VAL );

	LINENO = M1VAL + M2VAL + 1;
	if DEBUG fprintf(stderr,"\nLINENO=%d", LINENO);
	return;
}


/*********************************************************************
		puts out page footer
*********************************************************************/
pfoot()
{


	skip ( M3VAL );
	if ( M4VAL > 0 )
	{ puttl ( FOOTER, CURPAG );
		skip ( M4VAL - 1 );
	}
	return;

}


/*******************************************************************
	put out title line with optional page no.
*******************************************************************/
puttl ( title_str, pageno )
char *title_str;
int pageno;
{
	int i;


	for ( i=0; title_str[i] != '\0'; i++ )
		if ( title_str[i] == NUMSIGN )
			putdec ( pageno, 1 );	/* print pageno, width >= 1 */
	else
		putchar( title_str[i]);

	return;
}

/*******************************************************************
	put out num in string of width >= w
******************************************************************/
putdec ( num, w )
int num;
int w;
{
	int i, nd;
	char chars[10];


	nd = itoc ( num, chars, 10 );
	for ( i=nd + 1; i<=w; i++ )
		putchar(BLANK);
	for ( i=0;i<=nd; i++)
		putchar( chars[i] );

	return;
}

/*******************************************************************
	convert int num to char string in numstr
*********************************************************************/
itoc ( num, numstr, size )
int num;
char *numstr;
int size;	/* largest size of numstr */
{
	int absnum, i, j, k, d;


	absnum = abs(num);
	numstr[0] = '\0';
	i = 0;

	do
	{ i++;
		d = absnum % 10;
		numstr[i] = d + '0';
		absnum = absnum/10;
	} while ( absnum != 0 && i<size );

	if ( num < 0 && i<size )
	{ i++;
		numstr[i] = '-';
	}

	for ( j=0; j<i; j++ )
	{ k = numstr[i];
		numstr[i] = numstr[j];
		numstr[j] = k;
		i--;
	}

	return ( strlen(numstr) );
}

/********************************************************************
	copy title from com_line to ttl
**********************************************************************/
gettl ( com_line, ttl )
char *com_line, *ttl;
{
	int i;
	char local[ MAXLINE ];


	if DEBUG fprintf(stderr,"\n\nGETTL command line= <%s>", com_line);

	i=0;
	while ( com_line[i]!= ' ' && com_line[i]!='\t' && com_line[i]!='\n')
		i++;

	strcpy ( local, com_line );
	skip_blanks (&local[i]);

	/* strip quote if found */
	if ( local[i]==SQUOTE || local[i]==DQUOTE)	i++;

	strcpy ( ttl, &local[i] );
	if DEBUG fprintf(stderr,"\ntitle = <%s>", ttl);

	return;
}


/******************************************************************
	space n lines or to bottom of the page
*******************************************************************/
space (n)
int n;
{

	if DEBUG fprintf(stderr,"\nSPACE %d line(s), LINENO= %d", n, LINENO);

	brk();	/* flush out last unfilled line */
	if (LINENO > BOTTOM)	return;	/* end of page */

	if ( LINENO == 0 )	/* top of page */
		phead();

	skip( min( n, BOTTOM+1-LINENO ));	/* can't skip past bottom  */
	LINENO = LINENO + n;	/* obvious */

	if DEBUG fprintf(stderr,"\n	LINENO = %d", LINENO);
	if (LINENO > BOTTOM)
	{
		pfoot();	/* print footer if bottom */
	}

	return;
}


/*******************************************************
	yet ANOTHER version of text !!! no. 59,999,999
get it right this time Kath dear
*******************************************************/
text (line)
char *line;
{
	char wrdbuf [MAXLINE];
	int i, j, k;
	char *p1, *p2;

	if DEBUG fprintf(stderr,"\n\nTEXT:<%s>", line);
	if (line[0] == BLANK || line[0] == NEWLINE || line[0] == TAB)
		leadbl (line);
	if (ULVAL > 0)	/* set high bits of all non-white space chars */
	{
		ULVAL--;
		p1 = p2 = line;
		while (*p2)
		{ if (*p2 == TAB || *p2 == BLANK || *p2 == NEWLINE)
				*p1++ = *p2++;
			else 
				*p1++ = *p2++ | 0x80;
		}
	}
	if (CEVAL > 0)
	{ center (line);
		put (line);
		CEVAL--;
	}
	else if ( line[0] == NEWLINE || FILL == NO )
		put (line);
	else while (WE_HAVE_A_WORD == getwrd (line, wrdbuf))
			putwrd (wrdbuf);
	return;
}

/***********************************************************
	put out a line of text with correct indentation
	underlining if specified
************************************************************/
put (line)
char *line;
{
	int i, j, k;

	if DEBUG fprintf(stderr,"\n\nPUT<%s>",line);
	if (LINENO == 0 || LINENO > BOTTOM )	phead();
	indent (TIVAL);
	putline (line);
	TIVAL = INVAL;
	skip (min(LSVAL-1, BOTTOM-LINENO));
	LINENO = LINENO + LSVAL;
	if DEBUG fprintf(stderr,"\nLINENO=%d,  LSVAL=%d",LINENO, LSVAL);
	if (LINENO > BOTTOM)	pfoot();
	return;
}
/***********************************************************
concatenates the word onto the end of OUTBUF for filled text
************************************************************/
putwrd (wrdbuf)
char *wrdbuf;
{
	int i, j, k;
	char s[MAXLINE], ch;
	int line_len, outw, new_out_width, wid;
	int nextra;

	if DEBUG fprintf(stderr,"\nwrdbuf = <%s>",wrdbuf);
	skip_blanks (wrdbuf);
	trunc_bl (wrdbuf);
	wid = strlen (wrdbuf);
	if DEBUG fprintf(stderr,"\nwid = %d",wid);

	line_len = RMVAL - TIVAL;
	outw = strlen (OUTBUF);
	new_out_width = wid + outw + 1;	/* one for blank */
	if DEBUG fprintf(stderr,"\nnew_out_width=%d, outw=%d, wid=%d, line_len=%d",
	  new_out_width,    outw,    wid,    line_len   );
	if (new_out_width > min(line_len, MAXLINE-1))
	{ nextra = min(line_len, MAXLINE-1) -outw + 1;
		spread (OUTBUF, nextra, OUTWRDS);
		brk();
	}

	strcat (OUTBUF, wrdbuf);
	strcat (OUTBUF, " ");
	if DEBUG fprintf(stderr,"\nPUTWRD:OUTBUF=<%s>",OUTBUF);
	OUTWRDS++;
	return;
}

/**************************************************************
	returns no. of ch in string
***************************************************************/
int count_char (string, ch)
char *string, ch;
{
	int n;
	char *p;

	p = string;
	n=0;
	while (*p)	if (*p++ == ch)	n++;
	return (n);
}

/***********************************************************
	remove all occurrences of ch; first, last, and no.
	of occurrences returned in parameters
************************************************************/
remove_char (string, ch, first, last, n_ch)
char *string, ch;
int *first, *last, *n_ch;
{
	char *p1, *p2;
	find_char (string, ch, first, last);
	*n_ch = count_char (string, ch);
	p1=p2=string;
	while (*p1 = *p2++)	if (*p1 != ch)	p1++;
	return;
}

/**************************************************************
	a new putline routine; sends line to current output
***************************************************************/
putline (line)
char *line;
{
	int i;

	if (ULVAL >=0)
	{ replace_char (line, NEWLINE, CR);
		fputs (line, stdout);
		indent (TIVAL);
		for (i=0; line[i] != '\0'; i++)
			if (line[i] & 0x80)	putchar(UNDERLINE);
		else putchar(line[i]);
		putchar(CR);
		putchar(NEWLINE);
		if (ULVAL==0)	ULVAL--;
	}
	else fputs (line, stdout);
	return;
}


/* extras needed by roff */

movwrd(in,out)
char *in, *out;
	{
  char *tmp;
  tmp = out;
if(! *in) { *out = '\0' ; return(0) ; }
while (*in && !isspace(*out = *in))
	{ out++; in++; }
 *out = '\0';
  return( out != tmp );
	}


#define TRUE 1
#define FALSE 0

/* can be used for at most one character ptr argument */
 errprnt(cs, arg)
 char *cs, *arg;
	{
 fprintf( stderr, cs, arg);
  fflush(stderr);
	}



