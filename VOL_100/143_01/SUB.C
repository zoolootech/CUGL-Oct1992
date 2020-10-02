/*
%CC1 $1.C -X -E5000
%CLINK $1 DIO -S
%DELETE	$1.CRL 
*/
/*********************************************************************
*			      SUB				     *
**********************************************************************
*		   COPYRIGHT 1983 EUGENE H. MALLORY		     *
*********************************************************************/
#include "BDSCIO.H"
#include "DIO.H"

#define	AND &&
#define	OR ||
#define	EQUAL ==
#define	NOTEQUAL !=
#define	NOT !
#define	found 0x80
#define	notfound 0x81
#define	match 0x82
#define	notmatch 0x83
#define	notbegin 0xFF
#define	notend 0x8A
#define	skipany	0x86
#define	skipone	0x87
#define	lineend	0x0A
#define	linestart 0x7F
#define	anyalpha 0x12
#define	anynumber 0x14
	  
char string[MAXLINE];
char temps[MAXLINE];
char pattern[MAXLINE];
char replace[MAXLINE];
char anystr[MAXLINE];
int i,j,firstpat,nextpat,start;
    
main(argc,argv)
char **argv;
int argc;

BEGIN
    char c;
    int	ii,jj,optionerr;
    char *ss;

    dioinit(&argc,argv);

/*********************************************************************
***		  ARGUMENT PROCESSING				   ***
*********************************************************************/
	optionerr = FALSE;
	
	for (ii=argc-1;ii>0;ii--)
	    if (argv[ii][0] == '-')
		BEGIN
		    for	(ss = &argv[ii][1]; *ss	!= '\0';)
			BEGIN
			    switch (toupper(*ss++))
				BEGIN
				    case 'H':
					optionerr = TRUE;
					break;
				    default:
					typef("SUB: Illegal option %c.\n"
						,*--ss);
					ss++;
					optionerr = TRUE;
					break;
				END
			    while (isdigit(*ss)) ss++;
			END
		    for	(jj=ii;jj<(argc-1);jj++) argv[jj] = argv[jj+1];
		    argc--;
		END
	

	if (optionerr) 
	    BEGIN
		typef("SUB: Substitutes	Pat2 for Pat1: ONLY option is -H.\n");
		typef("Meta characters are:.\n");
		typef("* - Any character string.\n");
		typef("? - Any single character.\n");
		typef("_ - A space.\n");
		typef("\\ - A literal character	follows.\n");
		typef("{ - Beginning of	line.\n");
		typef("} - End of line.\n");
		typef("@ - Any alphabetic character.\n");
		typef("# - Any numeric character.\n");
		typef("! - NOT the character that follows.\n");
		exit(0);
	    END
    
/*********************************************************************
***		     END OF ARGUMENT PROCESSING			   ***
*********************************************************************/
    
    firstpat = nextpat = 0;
    if (argc < 2) error("SUB: Needs a pattern argument.");
    setuppat(argv[1],pattern);
    if (argc >=3)
	BEGIN
	    replace[0] = 0;
	    for(i=2;i<argc;i++)
	    BEGIN
		setuprep(argv[i],string);
		strcat(replace,string);
		if (i != argc-1) strcat(replace," ");
	    END
	END
    else
	BEGIN
	    replace[0] = 0;
	END
    
    while (NOT(getstring(&string[1])))
	BEGIN
	    string[0] =	linestart;
	    start = 0;
findpat:		    
	    if (find(start) EQUAL found)
		BEGIN
/*   printf("Firstpat is %d, nextpat is	%d.\n",firstpat,nextpat);   */
		    for	(i = 0;i<firstpat;i++)
			temps[i] = string[i];
		    temps[i] = 0;
		    j=0;
		    for	(i=firstpat;i<nextpat;i++)
			 anystr[j++] = string[i];
		    anystr[j] =	0;
/* printf("replacement str is:%s\n",anystr); */
		    catstr(temps,replace,anystr);
		    start = strlen(temps);
		    if (start>0	&& pattern[0] EQUAL notbegin) start--;
		    strcat(temps,&string[nextpat]);
		    if (string[nextpat]	NOTEQUAL 0)
			BEGIN
			    strcpy(string,temps);
			    goto findpat;
			END
		    else
			BEGIN
			    for	(i=0;temps[i];i++)
				BEGIN
				    c =	temps[i];
				    if (c NOTEQUAL linestart
					AND
					c NOTEQUAL lineend)
					    putchar(c);
				END
			    putchar('\n');
			END
		END
	    else
		BEGIN
			for (i=0;string[i];i++)
			    BEGIN
				c = string[i];
				if (c NOTEQUAL linestart
				    AND
				    c NOTEQUAL lineend)
					putchar(c);
			    END
			putchar('\n');
		END
	END
    dioflush();	
END

find(first)
int first;
    BEGIN
	i=0;
	j=first;
	firstpat = first;
loop:
	switch(search())
	    BEGIN
		case match:
		    if (i  EQUAL 0) firstpat = j;
		    i++;
		    j++;
		    if (pattern[i]  EQUAL 0)
			BEGIN
			    nextpat	= j;
			    if (pattern[0] EQUAL notbegin)
				firstpat++;
			    if (pattern[strlen(pattern)-1] EQUAL notend)
				nextpat--;
			    return found;
			END
		    if (string[j] EQUAL	0) return notfound;
		    goto loop;
		    
		case notmatch:
		    firstpat++;
		    j =	firstpat;
		    if (string[j] EQUAL	0) return notfound;
		    i =	0;
		    goto loop;
		    
		case found:
		    nextpat = j;
		    if (pattern[0] EQUAL notbegin)
			firstpat++;
		    if (pattern[strlen(pattern)-1] EQUAL notend)
			nextpat--;
		    return found;
		    
		case notfound:
		    return notfound;
	    END
END

search()
BEGIN
    int	c,isave;
    switch (pattern[i])
    BEGIN
	case 0:
	    return found;
	case anyalpha:
	    if (isalpha(string[j]))
		    return match;
	    else return	notmatch;
	case anynumber:
	    if (isdigit(string[j]))
		    return match;
	    else return	notmatch;
	case anyalpha|0x80:
	    if (!isalpha(string[j])
	       AND j NOTEQUAL 0
	       AND string[j] NOTEQUAL lineend)
		   return match;
	    else return	notmatch;
	case anynumber|0x80:
	    if (!isdigit(string[j])
	       AND j NOTEQUAL 0
	       AND string[j] NOTEQUAL lineend)
		   return match;
	    else return	notmatch;
	case skipany:
	    i++;
	    if (pattern[i] EQUAL 0)
		BEGIN
		    j =	strlen(string);
		    return found;
		END
	    isave = i;
loop:	    i =	isave;
loop2:	    switch(search())
		BEGIN
		    case match:
			i++;
			if (pattern[i] EQUAL 0)
			    BEGIN
			       i--;
			       return match;
			    END
			j++;
			if (string[j] EQUAL 0)
			    BEGIN
			       i--;
			       j--;
			       return notfound;
			    END
			goto loop2;
		    case notmatch:
			j++;
			if (string[j] EQUAL 0)
			    BEGIN
				j--;
				return notmatch;
			    END
			goto loop;
		    case found:
			return found;
		    case notfound:
			return notfound;
		    END
	case skipone:
	    if (string[j] EQUAL	'\n') return notfound;
	    if (j EQUAL	0) return notmatch;
	    i++;
	    j++;
	    if (string[j] EQUAL	0) return notfound;
		switch(search())
		    BEGIN
			case match:
			    return match;
			case notmatch:
			    j--;
			    return notmatch;
			case found:
			    return found;
			case notfound:
			    return notfound;
		    END
	default:
	    c =	 pattern[i];
	    if (c & 0x80)
		BEGIN
		if ((c&0x7f) NOTEQUAL toupper(string[j])
		    AND	j NOTEQUAL 0
		    AND	string[j] NOTEQUAL lineend)
		    return match;
		else return notmatch;
		END
	    else
		BEGIN
		if (c EQUAL toupper(string[j]))
		    return match;
		else return notmatch;
		END
    END
END

setuppat(pat,str)
char *pat;
char *str;
BEGIN
int pi,c;
pi = 0;
while (c = pat[pi++])
    BEGIN
	switch (c)
	    BEGIN
		case '*':
		    *str++ =  skipany;
		    if (pat[pi]	EQUAL '!'
			&&
			pat[pi+1] EQUAL	'}')
			    error("SUB:	*!} not	allowed.");
		    break;
		case '?':
		    *str++ = skipone;
		    break;
		case '}':
		    *str++ = lineend;
		    if (pat[pi]	NOTEQUAL 0)
			error("SUB: } must be last.");
		    break;
		case '{':
		   *str++ = linestart;
		   if (pi NOTEQUAL 1)
			error("SUB: { must be first.");
		   break;
		case '@':
		   *str++ = anyalpha;
		   break;
		case '#':
		   *str++ = anynumber;
		   break;
		case '_':
		    *str++ = ' ';
		    break;
		case '\\':
		    c =	pat[pi++];
		    if (c EQUAL	0 OR c EQUAL '\n')
	error("SUB: Bad	pattern, \\ requires character following.");
		    *str++ = c;
		    break;
		case '!':
		    c =	pat[pi++];
		    if (c < 0x20)
			error("SUB: Bad	pattern	after !.");
		    switch(c)
			BEGIN
			    case '*':
			    case '?':
			    case '!':
			    case 0:
			    error ("SUB: Bad pattern after !.");
			    case '}':
				*str++ = notend;
				if (pat[pi] NOTEQUAL 0)
				    error("SUB:	!} must	be last.");
				if (pi EQUAL 2)
				    error("SUB:	No pattern before !}.");
				break;
			    case '{':
				*str++ = notbegin;
				if (pi NOTEQUAL	2)
				    error("SUB:	!{ must	be first.");
				if (pat[pi] EQUAL 0)
				    error("SUB:	No pattern after !{.");
				if (pat[pi] EQUAL '*')
				    error("SUB:	!{* not	allowed.");
				if (pat[pi] EQUAL '!'
				    &&
				    pat[pi+1] EQUAL '}')
				    error("SUB:	!{!} not allowed.");
				break;
			    case '@':
				*str++ = anyalpha|0x80;
				break;
			    case '#':
				*str++ = anynumber|0x80;
				break;
			    case '_':
				*str++ = ' '|0x80;
				break;
			    case '\\':
				c = pat[pi++];
				if (c EQUAL 0 OR c EQUAL '\n' OR c < 0x20)
			    error("SUB:	Bad pattern after !\\.");
				*str++ = c|0x80;
				break;
			    default:
				*str++ = c|0x80;
			END
		    break;
	       default:
		   *str++ = c;
	    END
    END
    *str = 0;
END

setuprep(pat,str)
char *pat;
char *str;
BEGIN
int c;
while (c = *pat++)
    BEGIN
	switch (c)
	    BEGIN
		case '_':
		    *str++ = ' ';
		    break;
		case '*':
		    *str++ = skipany;
		    break;
		case '\\':
		    c =	*pat++;
		    if (c == 0 || c == '\n')
	error("SUB: Bad	replace, \\ requires character following.");
		    *str++ = c;
		    break;
		case '~':
		    c =	*pat++;
		    if (!isalpha(c))
	error("SUB: Bad	replace, ~ requires a following	alpha.");
		    *str++ = tolower(c);
		    break;
	       default:
		   *str++ = c;
	    END
    END
    *str = 0;
END

catstr(target,source,any)
char *target,*source,*any;
BEGIN
    char *tempany;
    while (*target) target++;
    while (*source)
	BEGIN
	    if (*source	!= skipany)
		*target++ = *source++;
	    else
		BEGIN
		    tempany = any;
		    while (*tempany) *target++ = *tempany++;
		    source++;
		END
	END
    *target = 0;
END
