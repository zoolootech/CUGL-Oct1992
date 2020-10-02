/*********************************************************************\
** .---------------------------------------------------------------. **
** |                                                               | **
** |                                                               | **
** |         Copyright (c) 1981, 1982, 1983 by Eric Martz.         | **
** |                                                               | **
** |                                                               | **
** |       Permission is hereby granted to use this source         | **
** |       code only for non-profit purposes. Publication of       | **
** |       all or any part of this source code, as well as         | **
** |       use for business purposes is forbidden without          | **
** |       written permission of the author and copyright          | **
** |       holder:                                                 | **
** |                                                               | **
** |                          Eric Martz                           | **
** |                         POWER  TOOLS                          | **
** |                    48 Hunter's Hill Circle                    | **
** |                      Amherst MA 01002 USA                     | **
** |                                                               | **
** |                                                               | **
** `---------------------------------------------------------------' **
\*********************************************************************/

#include "rap.h"
	
/*****************************************************************
	this function decodes the command type, returning the type, or
		UNKNOWN.
*******************************************************************/
int comtyp (line)
	char *line;
	{
	char let1, let2;

	let1  =  toupper( line[1] );
	let2  =  toupper( line[2] );

	if ( let1 EQ 'A' && let2 EQ 'R')	return( AR );
	if ( let1 EQ 'B' && let2 EQ 'D')	return( BD );
	if ( let1 EQ 'B' && let2 EQ 'L')	return( BL );
	if ( let1 EQ 'B' && let2 EQ 'O')	return( BO );
	if ( let1 EQ 'B' && let2 EQ 'P')	return( BP );
	if ( let1 EQ 'B' && let2 EQ 'R')	return( BR );
	if ( let1 EQ 'C' && let2 EQ 'C')	return( CC );
	if ( let1 EQ 'C' && let2 EQ 'E')	return( CE );
	if ( let1 EQ 'C' && let2 EQ 'F')	return( CF );
	if ( let1 EQ 'D' && let2 EQ '1')	return( D1 );
	if ( let1 EQ 'D' && let2 EQ '2')	return( D2 );
#ifdef DEBUG
	if ( let1 EQ 'D' && let2 EQ 'E')	return( DEBUG );
#endif
	if ( let1 EQ 'E' && let2 EQ 'C')	return( EC );
	if ( let1 EQ 'E' && let2 EQ 'D')	return( ED );
	if ( let1 EQ 'F' && let2 EQ 'I')	return( FI );
	if ( let1 EQ 'F' && let2 EQ 'O')	return( FO );
	if ( let1 EQ 'F' && let2 EQ 'N')	return( FN );
	if ( let1 EQ 'H' && let2 EQ 'E')	return( HE );
	if ( let1 EQ 'I' && let2 EQ '@')	return( IAT);
	if ( let1 EQ 'I' && let2 EQ 'C')	return( IC );
	if ( let1 EQ 'I' && let2 EQ 'G')	return( IG );
	if ( let1 EQ 'I' && let2 EQ 'N')	return( IN );
	if ( let1 EQ 'I' && let2 EQ 'X')	return( IX );
	if ( let1 EQ 'J' && let2 EQ 'U')	return( JU );
	if ( let1 EQ 'L' && let2 EQ 'F')	return( LF );
	if ( let1 EQ 'L' && let2 EQ 'S')	return( LS );
	
	if ( let1 EQ 'M') {
	     if (let2 EQ '1')			return( M1 );
		if (let2 EQ '2')			return( M2 );
		if (let2 EQ '3')			return( M3 );
		if (let2 EQ '4')			return( M4 );
		if (let2 EQ 'O')			return( MO );
	}

	if ( let1 EQ 'N' && let2 EQ 'E')	return( NEED );
	if ( let1 EQ 'N' && let2 EQ 'F')	return( NF );
	if ( let1 EQ 'N' && let2 EQ 'J')	return( NJ );
	if ( let1 EQ 'N' && let2 EQ 'X')	return( NX );

	if ( let1 EQ 'O' && let2 EQ 'U')	return( OU );
	if ( let1 EQ 'P' && let2 EQ '#')	return( PNO );
	if ( let1 EQ 'P' && let2 EQ 'D')	return( PD );
	if ( let1 EQ 'P' && let2 EQ 'I')	return( PI );
	if ( let1 EQ 'P' && let2 EQ 'L')	return( PL );
	if ( let1 EQ 'R' && let2 EQ 'E')	return( RE );
	if ( let1 EQ 'R' && let2 EQ 'M')	return( RM );
	if ( let1 EQ 'R' && let2 EQ 'F')	return( RF );
	if ( let1 EQ 'S' && let2 EQ 'A')	return( SA );
	if ( let1 EQ 'S' && let2 EQ 'P')	return( SP );
	if ( let1 EQ 'S' && let2 EQ 'S')	return( SS );
	if ( let1 EQ 'T' && let2 EQ 'A')	return( TA );
	if ( let1 EQ 'T' && let2 EQ 'C')	return( TC );
	if ( let1 EQ 'T' && let2 EQ 'I')	return( TI );
	if ( let1 EQ 'U' && let2 EQ 'L')	return( UL );

	if ( let1 EQ Pre_ss_delim &&
		let2 EQ Pre_ss_delim)	return( PRE_SS );
	if ( let1 EQ Post_ss_delim &&
		let2 EQ Post_ss_delim)	return( POST_SS );

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

	for (i = 0;
		(local[i] EQ ' ' || local[i] EQ '\t' || local[i] EQ '\n');
		i++);

	for (j = 0; (string[j] = local[i])  NE  '\0' ; i++, j++ )	;

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

	k  =  strlen (string);
	ptr  =  &string[ k-1 ];	/* char before terminating nul */

	while (*ptr EQ BLANK || *ptr EQ TAB || *ptr EQ NEWLINE)	
		*ptr--   =  '\0';
}
	
/*******************************************************************
	performs the formatting command returned by comtyp - sets global
	  variables ( indenting, underlining, etc. )
*******************************************************************/

command (line, nx, valid)
	char *line, *nx;
	int *valid;
	{
	int c_type;	/* command type  */
	int arg_val;	/* argument value, if any */
	char arg_sign;	/* relative (+ or -) or absolute */
	int i;

	c_type  =  comtyp (line);
#ifdef DEBUG
	if (Debug) fprintf(STDERR,"COMMAND %d\n",c_type);
#endif

	if (c_type  EQ  UNKNOWN)	/* IGNORE ALIEN ORDERS */
		{
		fprintf(STDERR,
			"UNKNOWN COMMAND at line %d: <%s>\n",In_linecnt,line);
		*valid = NO;
		return(0);
	}
	*valid = YES;

	arg_val  =  get_val ( line, &arg_sign );
#ifdef DEBUG
	if (Debug) fprintf(STDERR,
		"get_val returned arg_val = %d, arg_sign =  %c   \n",
		arg_val, arg_sign);
#endif
	switch (c_type) {

		case AR : /* assign value to arabic {#{ */
			set(&Arabic, arg_val, arg_sign, 1, 0, 32000);
			break;

		case TC: /* table of contents */
			if (Tabcont) extract(line+4);
			break;
			
		case IX: /* index */
			if (Index) extract(line+4);
			break;
			
		case PD: /* print draft */
			set(&Printdraft, arg_val, arg_sign, 1, 0, 1);
			break;
			
		case BD: /* begin draft */
			Indraft = YES;
			break;
			
		case ED: /* end draft */
			Indraft = NO;
			break;

		case FI :	/* filled lines  */
			brk(CR);	/* flush out last unfilled */
			Fill  =  YES;
			Tival = Pival;
			break;

		case NEED: /* need arg_val lines */
			if ((arg_val + Lineno - 1) > Bottom) space(HUGE);
			break;

		case NF :	/* non-filled lines */
			brk(CR);	/* flush out */
			Fill  =  NO;
			Tival = Inval;
			break;	/* down and cry */

		case BR :	/* just cause a break */
			brk(CR);
			Tival = Pival;
			break;

		case LS :	/* set line spacing value */
			set (&Lsval, arg_val, arg_sign, 1, 1, HUGE );
			break;

		case TI :	/* set temporary left indent */
			brk(CR);
			set ( &Tival, arg_val, arg_sign, 5, 0, Rmval );
			break;

		case PI : /* set paragraph indent */
			set (&Pival, arg_val, arg_sign, Inval+5, -HUGE, HUGE);
			brk(CR);
			if (Fill) Tival = Pival;
			break;

		case IN :	/* set left indent */
			brk(CR);
			set ( &Inval, arg_val, arg_sign, 0, 0, Rmval-1 );
			Pival = Inval;
			if (Fill) Tival = Pival;
			else Tival = Inval;
			break;

		case RM:	/* set right margin */
			set ( &Rmval, arg_val, arg_sign, 78, Tival+1, HUGE );
			break;

		case RF:	/* restore font to primary/default */
			if ( /*Sanders AND*/ Font_o)
				printf("%s%d%s", Font_o, Font, Font_c);
			break;

		case M1:	/* set topmost margin */
			set ( &M1val, arg_val, arg_sign, 2, 0, HUGE);
			break;

		case M2:	/* set second top margin */
			set ( &M2val, arg_val, arg_sign, 2, 0, HUGE);
			break;

		case M3:	/* set first bottom margin */
			set ( &M3val, arg_val, arg_sign, 2, 0, HUGE);
			break;

		case M4:	/* set bottom-most margin */
			set ( &M4val, arg_val, arg_sign, 2, 0, HUGE);
			break;

		case MO:	/* restore mode on sanders */
			brk(CR);
			/*if (Sanders)*/ restore_mode();/*??does nothing if Print_at*/
			break;

		case CE :	/* center next arg_val lines */
			brk(CR);
			set ( &Ce_input, arg_val, arg_sign, 1, 0, HUGE);
			break;
#ifdef DEBUG
		case DEBUG : /* Turn on Debug */
			Debug = YES;
			checkbig("Debug ON\n","");
			break;
#endif
		case EC : /* expand control character flag */
			set(&Exp_ctrl_chars,
				arg_val, arg_sign, YES, NO, YES);
			break;

		case UL :	/* underline next arg_val lines */
			set ( &Ul_input, arg_val, arg_sign, 1, 1, HUGE );
			break;
			
		case BO :
			set ( &Bo_input, arg_val, arg_sign, 1, 1, HUGE );
			break;

		case HE :	/* get header title for pages */
			gettl ( line, Header );
			break;

		case FO :	/* get footer title for pages */
			gettl ( line, Footer );
			break;

		case SP :	/* space down arg_val blank lines */
			set (&Spval, arg_val, arg_sign, 1, 0, HUGE);
			space ( Spval );
			break;

		case BP :	/* set pageno arg_val - begin page */
			if ( Lineno > 0 OR Outbuf[0])	space (HUGE);
			set ( &Curpag, arg_val, arg_sign, Curpag+1, -HUGE, HUGE);
			Newpag  =  Curpag;
			break;

		case PNO: /* assign new value to page number
				without forcing page break */
			set (&Curpag, arg_val, arg_sign, 1, 0, 32000);
			Newpag = Curpag + 1;
			break;

		case PL :	/* set page length */
			set (&Plval, arg_val, arg_sign, 66,
				M1val+M2val+M3val+M4val+1, HUGE);
			Bottom  =  Plval - M3val - M4val;
			break;

		case TA : /* set tab modulus */
			set (&Tabval, arg_val, arg_sign, 5, 1, HUGE);
			break;

		case JU : /* Justify */
			set(&Justify, arg_val, arg_sign, 1, 0, 2);
			/*if (Sanders)*/ restore_mode();
			break;

		case NJ : /* no justify */
			if (Justify EQ YES) brk(CR);
			Justify = NO;
			break;

		case NX : /* next input file */
			gettl (line, nx);
			break;
	
		case FN : /* select primary font */
			set(&Font, arg_val, arg_sign, 0, 0, 16);
			if (/*Sanders AND*/ Font_o)
				printf("%s%d%s", Font_o, arg_val, Font_c);
			break;

		case SA : /* Sanders printer? 0=no, 1=yes */
			set(&Sanders, arg_val, arg_sign, YES, 0, 1);
			break;

		case PRE_SS : /* {{ delimited ss, 0=no, 1=yes */
			set(&Pre_ss, arg_val, arg_sign, 1, 0, 1);
			break;

		case POST_SS : /* }} delimited ss, 0=no, 1=yes */
/*			if (!Post_ss AND arg_val) brk(CR); */
			set(&Post_ss, arg_val, arg_sign, 1, 0, 1);
			break;
		
		case RE : /* read sub input file */
			read_command(line);
/*			if (New_portion) find_portion();*/
			break;
		
		case SS : /* input ss pairs */
			if (!arg_val) {
				fprintf(STDERR,
"Error: '.ss 0' without preceding 'ss 1' at line %d\n", In_linecnt);
				break;
			}
			getpairs(Fpin, ((Fp_main EQ Fpin)? Fn_main: Fn_read));
			break;

		case OU : /* set output flag */
				switch (arg_val) {
				case 1:
					Screen_size = SCREEN_SIZE;
					_Outflag |= arg_val;
					break;
				case 2:
				case 4:
				case 8:
					_Outflag |= arg_val;
					if (!(_Outflag & 1)) Screen_size = 0;
					break;
				case 12:
				case 14:
				case 16:
				case 18:
				case 20:
				case 22:
				case 24:
				case 26:
				case 28:
				case 30:
				case 40:
				case 54:
				case 60:
					_Outflag |= 1;
					Screen_size = arg_val;
					break;
				default:
					fprintf(STDERR,"Invalid '.ou' argument\n");
					exit(0);
			}
			break;

		case D1 :
			if (strlen(line) >= 5) Pre_ss_delim = line[4];
			else Pre_ss_delim = '{';
			break;

		case D2 :
			if (strlen(line) >= 5) Post_ss_delim = line[4];
			else Post_ss_delim = '}';
			break;

		case IAT: /* Ignore @ as first char in input line */
			set(&Ignore_at, arg_val, arg_sign, 1, 0, 1);
			break;

		case IC : /* Ignore commands */
			set(&i, arg_val, arg_sign, 1, 0, 1);
			return(i);

		case IG : /* Ignore input line */
			break;
			
		case CF : /* continuous forms */
			set(&Con_forms, arg_val, arg_sign, 1, 0, 1);
			break;
			
		case BL : /* blank out character */
			if (strlen(line) >= 5) Blank_c = line[4];
			else Blank_c = ' ';
			break;

		case CC : /* compress control characters */
			set(&Comp_carat, arg_val, arg_sign, 1, 0, 1);
			break;
			
		case LF : /* on: NL -> CR+LF; off: NL -> CR only, and
				CR -> RLF, CR */
			set(&i, arg_val, arg_sign, 1, 0, 1);
			i? Suppress_lf = NO: Suppress_lf = YES; /* invert logic */
			break;
	}
	return(0);
}

/******************************************************************

	gets the number ( if any ) associated with any command 

*******************************************************************/
int get_val ( line, typ )
	char *line, *typ;
	{
	int i;
	char local[ MAXLINE ];

	strcpy (local, line);	/* local copy */
	if (local[3] EQ NULL) return(NO_VAL);

	/* skip over the command line */
	for (i = 1;
		local[i] NE ' ' && local[i] NE '\t' && local[i] NE '\n';
		i++ )
	;

	skip_blanks (&local[i]);	/* find the number */
	if (local[i] EQ NULL) return(NO_VAL);
	*typ  =  local[i];	/* relative or absolute */

	if ( *typ EQ '+' || *typ EQ '-' )	i++;
	else if ( !isdigit( *typ ) )	return( NO_VAL );
	ati(&i,local+i);
	return(i);
}
/*****************************************************************
	 sets a global parameter like line-spacing, underlining, etc.
	 Also checks that the new value is within the range of that 
	 parameter.  Assigns the default for that parameter if no value
	  is specified.
******************************************************************/

set ( param, val, arg_sign, defval, minval, maxval )
	int *param, val, defval, minval, maxval;
	char arg_sign;
	{
	if (val  EQ  NO_VAL )		/* defaulted */
		*param  =  defval;
	else if (arg_sign  EQ  '+')	/* relative + */
		*param +=  val;
	else if (arg_sign  EQ  '-')	/* relative - */
		*param -=  val;
	else	*param  =  val;		/* absolute */

	*param  =  min (*param,maxval);
	*param  =  max (*param, minval);
#ifdef DEBUG
	if (Debug) fprintf(STDERR,"SET *param  =  %d\n", *param);
#endif
	return;
}

/**********************************************************
			centers a line of text
**********************************************************/
center (line)
	char *line;
	{
	Tival  =  max(( Rmval+Tival-truelen(line))/2, 0 );
	return;
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
#ifdef DEBUG
	if (Debug) fprintf(STDERR,
"spread:line = <%s>, nextra = %d, no_words = %d\n",
		line, nextra, no_words);
#endif
	if (nextra <=  0 || no_words <=  1)	return;

	Dir  =  !(Dir);
	nholes  =  no_words - 1;
	trunc_bl (line);
	i  =  strlen(line) - 1 ; /* last character of string */
	j  =  min (MAXLINE - 2,  i + nextra);/* last position in output */
	line[j+1]  =  '\0';
#ifdef DEBUG
	if (Debug) fprintf(STDERR,
		"Dir = %d, nholes = %d, i = %d, j = %d\n",
		Dir, nholes, i, j);
#endif
	for ( ; i<j ; i--, j-- )
	      { line[j]  =  line[i];
#ifdef DEBUG
		if (Debug) fprintf(STDERR,
			"[%c i = %d j = %d] ", line[j], i, j);
#endif
		if ( line[i]  EQ  BLANK)
		      { if (Dir  EQ  0)
				nblanks  =  (nextra - 1)/nholes   +   1;
			else
				nblanks  =  nextra/nholes;
			nextra  =  nextra - nblanks;
			nholes  =  nholes - 1;
#ifdef DEBUG
			if (Debug) fprintf(STDERR,
				"nblanks = %d, nextra = %d\n",
				nblanks, nextra);
#endif
			for ( ; nblanks > 0;  nblanks-- )
				line[--j]  =  BLANK;
		      }
	      }
#ifdef DEBUG
	if (Debug) fprintf(STDERR,"after spreading, line is:\n<%s>\n", line);
#endif
	return;
	}

/*****************************************************************/
extract(buf)
	char *buf;
	{
	char number[8];
	
	sprintf(number, "%d", Curpag);
	substitute(buf, "#", number);
	substitute(buf, "$F", (Fpin EQ Fp_main? Fn_main: Fn_read));

	puts(buf);
	putchar(NEWLINE);
}
/*****************************************************************
	END OF RAP1.C
*****************************************************************/
(Debug) fprintf(STDERR,
			"