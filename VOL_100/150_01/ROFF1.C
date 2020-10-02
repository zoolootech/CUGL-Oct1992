/*
       HEADER:  150.??;
        TITLE:  ROFF - "RUNOFF" Text Formatter Program;
      VERSION:  5.1;
         DATE:  08/28/1982;
  DESCRIPTION:  "A Text formatter based upon the one in Kernighan and Plauger's
                 book SOFTWARE TOOLS.  Contains provisions for right justifi-
                 cation, page headings and footings, variable margins, inden-
                 tation, page numbers in header text, centered text, underlin-
                 ing, and page lengths.  Easy to use.  Processes plain ASCII
                 text from almost any editor, and formats it to to drive most  
                 printers in straightforward fashion.  No provisions for fancy
                 fonts, proportional spacing, etc.  DOC file contains good but 
                 brief tutorial; sufficient for most semi-experienced users.";
     KEYWORDS:  ROFF, Text Formatter, Software Tools;
       SYSTEM:  any system with C compiler;
     FILENAME:  ROFF1.C;
     SEE-ALSO:  ROFF.DOC, ROFF.H, ROFF.HE, ROFF2.C, ROFF.EXE;
      AUTHORS:  modified "for IBM-PC" by M. S. Zachmann;
    COMPILERS:  BDS-C originally, IBM C compiler unknown;
   REFERENCES:  AUTHORS: M. S. Zachmann; TITLE: "ROFF";
                CITATION: "PC-SIG Disk 50 or 142";
                AUTHORS: Kernighan and Plauger; TITLE: "Software Tools";
                CITATION: "available in many computer stores" 
       ENDREF;
*/
#define FALSE 0
#define TRUE 1

/* 7 MAY 81 */
/* based on the formatter presented in Kernighan and Plauger's
SOFTWARE TOOLS, modified for BDS C */



#include "stdio.h"
#include "ctype.h"
#include "ROFF.H"


main(argc, argv)
int argc;
char **argv;
{
   char filename[20];
   char line[MAXLINE];
   struct _iobuf *bufr;


	errprnt("\nVersion 5.1\n");

	debug = FALSE;
	if (argc == 1)
	{ errprnt("\nUSAGE:  roff >destination origin_file \n");
	  errprnt("Where origin_file is any valid text file name\n");
	  errprnt("And destination is a filename, or LST: (printer)\n");
	  errprnt("\tor left out (to console)\n");
		exit();
	}

	argv++;
	while ( --argc > 0 )
	{ strcpy (filename, *argv++);		/* get next argument */
		errprnt("\nNow processing <%s>",filename );
		if (filename[0] == '-' && filename[1]=='d')
		{
			debug=TRUE;
			continue;
		}


		if ((bufr = fopen(filename, "r")) == NULL)
		{ errprnt("\ncan not open <%s>", filename );
			continue;
		}

		init_defaults();

		while ( fgets( line,MAXLINE, bufr ) != 0 ) /*until EOF or CPMEOF */
		{ if (line[0] == COMMAND )
				comand (line);
			else
				text (line);

		}
		if (LINENO > 0  || OUTBUF[0] !='\0')
			space (HUGE);		/* flush last output */
		errprnt("\nDone processing <%s>", filename );

	} 	/* end while (--argc > 0 ) */
	exit(0);
}		/* end main()	       */


/********************************************************************
initializes the global variables governing the execution of the
 format commands.
********************************************************************/
init_defaults()
{

	FILL = FI_DEF;	/* yes we want filled lines */
	LSVAL = LS_DEF;	/* line spacing = 1 */
	INVAL = IN_DEF;	/* left margin indent  0 */
	RMVAL = RM_DEF;	/* right margin = page width  */
	TIVAL = TI_DEF;	/* left margin temporary indent    0 */
	CEVAL = CE_DEF;	/* next n lines to be centered -  0  */
	ULVAL = UL_DEF;	/* next n lines to be underlined -  0  */

	PAGESTOP = FALSE;
	PLVAL = PAGELEN;
	M1VAL = M1_DEF;		/* top and bottom margins */
	M2VAL = M2_DEF;
	M3VAL = M3_DEF;
	M4VAL = M4_DEF;
	CURPAG = 0;
	NEWPAG = 1;
	LINENO = 0;
	BOTTOM = PLVAL - M3VAL - M4VAL;

	OUTWRDS = 0;	/* no. of words in outbuf */
	OUTBUF [0] = '\0';
	DIR = 0;

	strcpy ( HEADER, "\n" );
	strcpy ( FOOTER, "\n" );
}


/*****************************************************************
this function decodes the command type, returning the type, or
	UNKNOWN.
*******************************************************************/
int comtyp (line)
char *line;
{
	char let1, let2;

	let1 = toupper( line[1] );
	let2 = toupper( line[2] );

	if ( let1=='F' && let2=='I')	return( FI );
	if ( let1=='F' && let2=='O')	return( FO );
	if ( let1=='T' && let2=='I')	return( TI );
	if ( let1=='B' && let2=='P')	return( BP );
	if ( let1=='B' && let2=='R')	return( BR );
	if ( let1=='C' && let2=='E')	return( CE );
	if ( let1=='H' && let2=='E')	return( HE );
	if ( let1=='I' && let2=='N')	return( IN );
	if ( let1=='L' && let2=='S')	return( LS );
	if ( let1=='N' && let2=='F')	return( NF );
	if ( let1=='P' && let2=='L')	return( PL );
	if ( let1=='R' && let2=='M')	return( RM );
	if ( let1=='S' && let2=='P')	return( SP );
	if ( let1=='U' && let2=='L')	return( UL );
	if ( let1=='M')
	{ if (let2=='1')		return( M1 );
		if (let2=='2')		return( M2 );
		if (let2=='3')		return( M3 );
		if (let2=='4')		return( M4 );
	}

	return( UNKNOWN );		/* no match */

}


/*********************************************************************
Skips white-space characters at the beginning of a string.
*********************************************************************/
skip_blanks ( string )
char *string;
{
	char local[ MAXLINE ];
	int i, j, k;



	strcpy ( local, string );

	for (i=0; local[i]==' ' || local[i]=='\t' || local[i]=='\n' ; i++);

	for (j=0; (string[j]=local[i]) != '\0' ; i++, j++ )	;

	return;
}


/*********************************************************************

Truncates white-space characters at the end of a string.

*********************************************************************/
trunc_bl (string)
char *string;
{
	char *ptr;
	int k;

	k = strlen (string);
	ptr = &string[ k-1 ];	/* char before terminating nul */

	while (*ptr==BLANK || *ptr==TAB || *ptr==NEWLINE)	
		*ptr--  = '\0';
}


/*******************************************************************
performs the formatting command returned by comtyp - sets global
  variables ( indenting, underlining, etc. )
*******************************************************************/

comand ( line )
char *line;
{
	int c_type;	/* command type  */
	int arg_val;	/* argument value, if any */
	char arg_typ;	/* relative (+ or -) or absolute */
	int i;

	c_type = comtyp (line);
	if DEBUG errprnt("\n\nCOMAND %d",c_type);

	if (c_type == UNKNOWN)	/* IGNORE ALIEN ORDERS */
	{
		errprnt( "UNKNOWN COMMAND: <%s>\n", line);
		return;
	}

	arg_val = get_val ( line, &arg_typ );
	if DEBUG errprnt( " \n	get_val returned arg_val=%d, arg_typ= %c   ",
	  arg_val,    arg_typ   );
	if DEBUG errprnt("\n\n");

	switch (c_type)
	{
	case FI :	/* filled lines  */
		brk();	/* flush out last unfilled */
		FILL = YES;
		break;

	case NF :	/* non-filled lines */
		brk();	/* flush out */
		FILL = NO;
		break;	/* down and cry */

	case BR :	/* just cause a break */
		brk();
		break;

	case LS :	/* set line spacing value */
		set (&LSVAL, arg_val, arg_typ, LS_DEF, 1, HUGE );
		break;

	case TI :	/* set temporary left indent */
		brk();
		set ( &TIVAL, arg_val, arg_typ, TI_DEF, 0, RMVAL );
		break;

	case IN :	/* set left indent */
		set ( &INVAL, arg_val, arg_typ, IN_DEF, 0, RMVAL-1 );
		TIVAL = INVAL;
		break;

	case RM:	/* set right margin */
		set ( &RMVAL, arg_val, arg_typ, RM_DEF, TIVAL+1, HUGE );
		break;
	case M1:	/* set topmost margin */
		set ( &M1VAL, arg_val, arg_typ, M1_DEF, 0, HUGE);
		break;

	case M2:	/* set second top margin */
		set ( &M2VAL, arg_val, arg_typ, M2_DEF, 0, HUGE);
		break;

	case M3:	/* set first bottom margin */
		set ( &M3VAL, arg_val, arg_typ, M3_DEF, 0, HUGE);
		break;

	case M4:	/* set bottom-most margin */
		set ( &M4VAL, arg_val, arg_typ, M4_DEF, 0, HUGE);
		break;

	case CE :	/* center next arg_val lines */
		brk();
		set ( &CEVAL, arg_val, arg_typ, CE_DEF, 0, HUGE);
		break;

	case UL :	/* underline next arg_val lines */
		set ( &ULVAL, arg_val, arg_typ, UL_DEF, 1, HUGE );
		break;

	case HE :	/* get header title for pages */
		gettl ( line, HEADER );
		break;

	case FO :	/* get footer title for pages */
		gettl ( line, FOOTER );
		break;

	case SP :	/* space down arg_val blank lines */
		set (&SPVAL, arg_val, arg_typ, 1, 0, HUGE);
		space ( SPVAL );
		break;

	case BP :	/* set pageno arg_val - begin page */
		if ( LINENO > 0 )	space (HUGE);
		set ( &CURPAG, arg_val, arg_typ, CURPAG+1, -HUGE, HUGE);
		NEWPAG = CURPAG;
		break;

	case PL :	/* set page length */
		set (&PLVAL, arg_val, arg_typ, PAGELEN,
		  M1VAL+M2VAL+M3VAL+M4VAL+1, HUGE);
		BOTTOM = PLVAL - M3VAL - M4VAL;
		break;


	}

	return;
}

/******************************************************************

gets the number ( if any ) associated with any command 

*******************************************************************/
int get_val ( line, typ )
char *line, *typ;
{
	int i,j;
	char local[ MAXLINE ];

	strcpy (local, line);	/* local copy */

/* skip over the command line */
	for (i=1; local[i]!=' ' && local[i]!='\t' && local[i]!='\n' ; i++ )
		;

	skip_blanks (&local[i]);	/* find the number */
	*typ = local[i];	/* relative or absolute */

	if ( *typ=='+' || *typ=='-' )	i++;
	else if ( !isdigit( *typ ) )	return( NO_VAL );

	stcd_i( &local[i], &j);
	return(j);

}


/*****************************************************************
 sets a global parameter like line-spacing, underlining, etc.
 Also checks that the new value is within the range of that 
 parameter.  Assigns the default for that parameter if no value
  is specified.
******************************************************************/

set ( param, val, arg_typ, defval, minval, maxval )
int *param, val, defval, minval, maxval;
char arg_typ;
{
	if (val == NO_VAL )		/* defaulted */
		*param = defval;
	else if (arg_typ == '+')	/* relative + */
		*param += val;
	else if (arg_typ == '-')	/* relative - */
		*param -= val;
	else	*param = val;		/* absolute */

	*param = min(*param,maxval);
	*param = max(*param, minval);

	if DEBUG errprnt("\n	SET *param = %d", *param);
	return;
}



/**********************************************************
		centers a line of text
**********************************************************/
center (line)
char *line;
{
	TIVAL = max(( RMVAL+TIVAL-strlen(line))/2, 0 );
	return;
}


/**************************************************************
	get next word from input line
*************************************************************/
int getwrd (in,  out )
char *in, *out;
{
	int i, j, n;

/* find first non-blank */
	skip_blanks (in);
	replace_char ( in, TAB, BLANK); /* replace tabs w/space */

/* scan off a word */
	if ( 1 == (n = movwrd( in, out)))
	{ if DEBUG errprnt("\nafter sscanf out = <%s>",out);
		j = strlen (out);
		replace_char (out, NEWLINE, BLANK);
		for (i=0; i<j; i++ )	in[i] = BLANK;	/* blank out word in input */
	}
	if DEBUG errprnt("\ngetwrd will return %d", n);
	return(n);	/* WE_HAVE_A_WORD = 1 */
}




/*****************************************************************
	distibute words evenly across a line
*****************************************************************/
spread ( line, nextra, no_words)
char *line;
int nextra;	/* no. extra places left in line */
int no_words;   /* no. words in the line         */
{
	int i, j, nblanks, nholes;

	if (nextra <= 0 || no_words <= 1)	return;

	DIR = !(DIR);
	nholes = no_words - 1;
	trunc_bl (line);
	i = strlen(line) - 1 ; /* last character of string */
	j = min(MAXLINE - 2,  i + nextra);	/* last  position in output */
	line[j+1] = '\0';

	for ( ; i<j ; i--, j-- )
	{ line[j] = line[i];
		if DEBUG errprnt("[%c i=%d j=%d] ", line[j], i, j);
		if ( line[i] == BLANK)
		{ if (DIR == 0)
				nblanks = (nextra - 1)/nholes   +   1;
			else
				nblanks = nextra/nholes;
			nextra = nextra - nblanks;
			nholes = nholes - 1;

			if DEBUG errprnt("\nnblanks=%d, nextra=%d",
			  nblanks,    nextra   );
			for ( ; nblanks > 0;  nblanks-- )
				line[--j] = BLANK;
		}
	}
	if DEBUG errprnt("\nafter spreading, line is:\n<%s>", line);
	return;
}


/****************************************************************
	end current filled line 
*****************************************************************/
brk()
{
	int l;
	if DEBUG errprnt("\nbrk: OUTBUF=<%s>", OUTBUF);

	if ( 0!= ( l = strlen (OUTBUF)))
	{ OUTBUF[l] = NEWLINE;
		OUTBUF[l+1] = '\0';
		put (OUTBUF);
	}

	OUTWRDS = 0;
	OUTBUF[0] = '\0';
	return;

}
