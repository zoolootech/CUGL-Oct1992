/*
%CC1 $1.C -X -e5000
%CLINK $1 DIO -S
%DELETE	$1.CRL
*/
/*******************************************************************
*			     CB					   *
********************************************************************
*		  COPYRIGHT 1983 EUGENE	H. MALLORY		   *
*******************************************************************/

#include <bdscio.h>
#include <dio.h>

#define	flush /**/
#define	AND &&
#define	OR ||
#define	EQUAL ==
#define	NOTEQUAL !=
#define	NOT !

#define	LOOPTYPE 1
#define	IFTYPE 2
FUNCTION char *do_comment();
FUNCTION char *do_begin();
FUNCTION char *do_end();
FUNCTION char *do_case();
FUNCTION char *do_default();
FUNCTION char *do_semi();
FUNCTION char *do_paren();
FUNCTION char *do_quote();
FUNCTION char *do_string();
FUNCTION char *do_next_word();
FUNCTION char inline[MAXLINE];
int indent,nlflag,step;
int verb, verbx, verbstk[20];

main(argc,argv)
char **argv;
int argc;

  BEGIN
    char c;
    int	ii,jj,optionerr;
    char *ss;
    char *stgptr;
    dioinit(&argc,argv);

/*********************************************************************
*		   ARGUMENT PROCESSING				     *
*********************************************************************/

    step = 2;
    optionerr =	FALSE;
    for	(ii=argc-1;ii>0;ii--)
    if (argv[ii][0] == '-')
      THEN
	for (ss	= &argv[ii][1];	*ss != '\0';)
	  LOOP
	    switch (toupper(*ss++))
	      BEGIN
	      case 'I':
		step = atoi(ss);
		break;
	      case 'H':
		optionerr = TRUE;
		break;
	      default:
		fprintf(STDERR,"CB: Illegal option %c.\n",*--ss);
		ss++;
		optionerr = TRUE;
		break;
	      END
	    while (isdigit(*ss)) 
	      LOOP
		ss++;
	      ENDLOOP
	  ENDLOOP
	for (jj=ii;jj<(argc-1);jj++) argv[jj] =	argv[jj+1];
	argc--;
      ENDIF

    if (optionerr) 
      THEN
	fprintf(STDERR,"CB: Formats a C	Program	with structured	indentation\n");
	fprintf(STDERR,"CB: -In	 n is indent step, default is 2\n");
	fprintf(STDERR,"Ex. A:CB <T-UNFORM.C >T-FORMAT.C\n");
	exit(0);
      ENDIF
/********************************************************************
*		      END OF ARGUMENT PROCESSING		    *
********************************************************************/

    indent = 0;	 	/* start indent at 0 */
    nlflag = 0;	 	/* set	to bol */
    verbx = 0;		/* set verb stack empty	*/
    while (!getstring(inline))
      LOOP
	stgptr = inline;
	while (*stgptr EQUAL ' ' OR *stgptr EQUAL '\t')	
	  LOOP
	    stgptr++;
	  ENDLOOP

	if (*stgptr EQUAL '\n')	
	  THEN
	    nlflag = -1;
	  ENDIF


	if (is_begin(stgptr)) ;
	else if	(is_loop(stgptr)) verb = LOOPTYPE;
	else if	(is_if(stgptr))	verb = IFTYPE;
	else verb = 0;
	while (*stgptr NOTEQUAL	'\n')
	  LOOP
	    if (*stgptr	EQUAL ';') stgptr = do_semi(stgptr);
	    else if (is_comment(stgptr)) stgptr	= do_comment(stgptr);
	    else if (is_begin(stgptr)) stgptr =	do_begin(stgptr);
	    else if (is_default(stgptr)) stgptr	= do_default(stgptr);
	    else if (is_case(stgptr)) stgptr = do_case(stgptr);
	    else if (is_end(stgptr)) stgptr = do_end(stgptr);
	    else if (is_paren(stgptr)) stgptr =	do_paren(stgptr);
	    else if (is_quote(stgptr)) stgptr =	do_quote(stgptr);
	    else if (is_string(stgptr))	stgptr = do_string(stgptr);
	    else stgptr	= do_next_word(stgptr);
	  ENDLOOP
	newline();
      ENDLOOP
    if (verbx NOTEQUAL 0) error("CB: Too few ENDs.");
    dioflush();
  END

FUNCTION is_comment(string)
char string[];
  BEGIN
    if ((string[0] EQUAL '/') AND (string[1] EQUAL '*')) return	TRUE;
    return FALSE;
  END

FUNCTION is_quote(string)
char string[];
  BEGIN
    if (string[0] EQUAL	'\'') return TRUE;
    return FALSE;
  END

FUNCTION is_paren(string)
char string[];
  BEGIN
    if (string[0] EQUAL	'(') return TRUE;
    return FALSE;
  END

FUNCTION is_string(string)
char string[];
  BEGIN
    if (string[0] EQUAL	'"') return TRUE;
    return FALSE;
  END

FUNCTION is_begin(string)
char string[];
  BEGIN
    if (string[0] EQUAL	'{') return TRUE;
    if (strmatch(string,"BEGIN")) return TRUE;
    if (strmatch(string,"LOOP")) return	TRUE;
    if (strmatch(string,"THEN")) return	TRUE;
    return FALSE;
  END

FUNCTION is_end(string)
char string[];
  BEGIN
    if (string[0] EQUAL	'}') return TRUE;
    if (strmatch(string,"ENDLOOP")) return TRUE;
    if (strmatch(string,"ENDIF")) return TRUE;
    if (strmatch(string,"END"))	return TRUE;
    return FALSE;
  END

FUNCTION is_case(string)
char string[];
  BEGIN
    if (strmatch(string,"CASE")) return	TRUE;
    return FALSE;
  END

FUNCTION is_default(string)
char string[];
  BEGIN
    if (strmatch(string,"default:")) return TRUE;
    return FALSE;
  END

FUNCTION is_if(string)
char string[];
  BEGIN
    if (strmatch(string,"IF")) return TRUE;
    if (strmatch(string,"else")) return	TRUE;
    return FALSE;
  END

FUNCTION is_loop(string)
char string[];
  BEGIN
    if (strmatch(string,"WHILE")) return TRUE;
    if (strmatch(string,"DO")) return TRUE;
    if (strmatch(string,"FOR"))	return TRUE;
    return FALSE;
  END

FUNCTION pushv()
  BEGIN
    verbstk[verbx++] = verb;
  END

FUNCTION pullv()
  BEGIN
    verb = verbstk[--verbx];
    if (verbx <	0) error("CB: Too many ENDs.");
    return verb;
  END


FUNCTION strmatch(s1,s2)
char *s1, *s2;
  BEGIN
    while (*s2)	
      LOOP
	if (toupper(*s1++) NOTEQUAL *s2++) 
	  THEN
	    return FALSE;
	  ENDIF
      ENDLOOP
    if (isalpha(*s1)) return FALSE;
    if (isdigit(*s1)) return FALSE;
    if (*s1 EQUAL '_') return FALSE;
    if (*s1 EQUAL ':') return FALSE;
    if (*s1 EQUAL ';') return FALSE;
    return TRUE;
  END

FUNCTION newline()
  BEGIN
    if (nlflag < 0)
      THEN
	putchar('\n');
	nlflag = 0;
      ENDIF
  END

FUNCTION char *do_comment(string)
char *string;
  BEGIN
    if (string NOTEQUAL	inline)	tabin();
    putchar(*string++);
    putchar(*string++);
    nlflag = -1;
    while (TRUE)
      LOOP
	if (*string EQUAL '\n')	
	  THEN
	    if (getstring(inline))
	      THEN
		error("CB: EOF encountered in comment.");
	      ENDIF
	    putchar('\n');
	    string = inline;
	    continue;
	  ENDIF
	if (*string NOTEQUAL '*') 
	  THEN
	    if (is_comment(string)) 
	      THEN
		string = do_comment(string);
	      ENDIF
	    else 
	      THEN
		putchar(*string++);
	      ENDIF
	    continue;
	  ENDIF
	else 
	  THEN
	    if (string[1] EQUAL	'/') /*	comment	end */
	      THEN
		putchar(*string++);
		putchar(*string++);
		return string;
	      ENDIF
	    else 
	    putchar(*string++);
	  ENDIF
      ENDLOOP
  END

FUNCTION char *do_string(string)
char *string;
  BEGIN
    tabin();
    nlflag = -1;
    putchar(*string++);
    while (*string NOTEQUAL '"')
      LOOP
	putchar(*string);
	if (*string EQUAL '\n')	error("CB: Double quote	unmatched at EOL.");
	if (*string++ EQUAL '\\')
	putchar(*string++);
      ENDLOOP
    putchar(*string++);
    return string;
  END

FUNCTION char *do_quote(string)
char *string;
  BEGIN
    tabin();
    nlflag = -1;
    putchar(*string++);
    while (*string NOTEQUAL '\'') 
      LOOP
	putchar(*string);
	if (*string EQUAL '\n')	error("CB: Single quote	unmatched at EOL.");
	if (*string++ EQUAL '\\') putchar(*string++);
      ENDLOOP
    putchar(*string++);
    return string;
  END

FUNCTION char *do_paren(string)
char *string;
  BEGIN
    int	isave;
    indent += step;
    tabin();
    nlflag = -1;
    putchar(*string++);
    while (*string NOTEQUAL ')') 
      LOOP
	if (*string EQUAL '\n')	
	  THEN
	    if (getstring(inline))
	      THEN
		error("CB: EOF encountered in parens.");
	      ENDIF
	    nlflag = -1;
	    newline();
	    tabin();
	    nlflag = -1;
	    string = inline;
	    while (*string EQUAL ' ' OR	*string	EQUAL '\t') 
	      LOOP
		string++;
	      ENDLOOP
	    continue;
	  ENDIF
	if (*string EQUAL '(') string =	do_paren(string);
	else if	(*string EQUAL '\'') string = do_quote(string);
	else if	(*string EQUAL '"') string = do_string(string);
	else putchar(*string++);
      ENDLOOP
    putchar(*string++);
    indent -= step;
    return string;
  END

FUNCTION char *do_begin(string)
char *string;
  BEGIN
    pushv();
    indent += step;
    newline();
    tabin();
    nlflag = -1;
    flush
    if (strmatch(string,"BEGIN")) 
      THEN
	if (verb EQUAL LOOPTYPE) printf("LOOP");
	else if	(verb EQUAL IFTYPE) printf("THEN");
	else printf("BEGIN");
	string+=5;
      ENDIF
    if (strmatch(string,"THEN")) 
      THEN
	printf("THEN");
	string+=4;
      ENDIF
    if (strmatch(string,"LOOP")) 
      THEN
	printf("LOOP");
	string+=4;
      ENDIF
    if (*string	EQUAL '{') 
      THEN
	if (verb EQUAL LOOPTYPE) printf("LOOP");
	else if	(verb EQUAL IFTYPE) printf("THEN");
	else printf("BEGIN");
	string+=1;
      ENDIF
    indent += step;
    newline();
    while (*string EQUAL ' ' OR	*string	EQUAL '\t') 
      LOOP
	string++;
      ENDLOOP
    return string;
  END

FUNCTION char *do_end(string)
char *string;
  BEGIN
    pullv();
    indent -= step;
    newline();
    tabin();
    nlflag = -1;
    flush
    if (strmatch(string,"ENDIF")) 
      THEN
	printf("ENDIF");
	string+=5;
      ENDIF
    if (strmatch(string,"ENDLOOP")) 
      THEN
	printf("ENDLOOP");
	string+=7;
      ENDIF
    if (strmatch(string,"END"))	
      THEN
	if (verb EQUAL LOOPTYPE) printf("ENDLOOP");
	else if	(verb EQUAL IFTYPE) printf("ENDIF");
	else printf("END");
	string+=3;
      ENDIF
    if (*string	EQUAL '}') 
      THEN
	if (verb EQUAL LOOPTYPE) printf("ENDLOOP");
	else if	(verb EQUAL IFTYPE) printf("ENDIF");
	else printf("END");
	string+=1;
      ENDIF
    indent -= step;
    newline();
    while (*string EQUAL ' ' OR	*string	EQUAL '\t') 
      LOOP
	string++;
      ENDLOOP
    return string;
  END

FUNCTION char *do_case(string)
char *string;
  BEGIN
    indent -= step;
    newline();
    tabin();
    nlflag = -1;
    flush
    printf("CASE");
    string += 4;
    indent += step;
    return string;
  END

FUNCTION char *do_default(string)
char *string;
  BEGIN
    indent -= step;
    newline();
    tabin();
    nlflag = -1;
    flush
    printf("default:");
    string+=8;
    indent += step;
    return string;
  END

FUNCTION char *do_semi(string)
char *string;
  BEGIN
    tabin();
    nlflag = -1;
    putchar(*string++);
    while (*string EQUAL ' ' OR	*string	EQUAL '\t') 
      LOOP
	string++;
      ENDLOOP
    if (is_begin(string)) error("CB: BEGIN follows ;.");
    else if (is_loop(string)) verb = LOOPTYPE;
    else if (is_if(string)) verb = IFTYPE;
    else verb =	0;
    if (*string	NOTEQUAL '\n' AND NOT is_comment(string))
      THEN
	newline();
      ENDIF
    if (is_comment(string)) 
      THEN
	putchar(' ');
	putchar(' ');
	putchar('\t');
      ENDIF
    return string;
  END

FUNCTION char *do_next_word(string)
char *string;
  BEGIN
    tabin();
    nlflag = -1;
    while (TRUE)
      LOOP
	if (*string EQUAL '\n')	return string;
	if (*string EQUAL ' ' OR *string EQUAL '\t') break;
	if (*string EQUAL ';') return string;
	if (*string EQUAL '}') return string;
	if (*string EQUAL '{') return string;
	if (*string EQUAL '\'')	return string;
	if (*string EQUAL '"') return string;
	if (*string EQUAL '(') return string;
	if (is_comment(string))	return string;
	putchar(*string++);
      ENDLOOP
    putchar(' ');
    while (*string EQUAL ' ' OR	*string	EQUAL '\t') 
      LOOP
	string++;
      ENDLOOP
    return string;
  END

FUNCTION tabin()
  BEGIN
    if (nlflag >= 0) 
      THEN
	while (nlflag+8	<= indent)
	  LOOP
	    putchar('\t');
	    nlflag+=8;
	  ENDLOOP
	while (nlflag++	< indent) 
	  LOOP
	    putchar(' ');
	  ENDLOOP
      ENDIF
  END
