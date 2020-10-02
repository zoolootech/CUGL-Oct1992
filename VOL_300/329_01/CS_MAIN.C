/* cs_main.c */
/*************************************************************************
This programme is shareware.
If you found this programme useful then send me a colourful postcard
with the words "Happy Birthday" (or equivalent) so that it arrives
at my address around the first of January 1991. After that there
is no restriction.

Henri de Feraudy
27 rue Chef de Ville
92140 Clamart 
France
**************************************************************************/
/*************************************************************************
 22/10/90, changed file names
 22/10/90, bug fix: added rule to deal with hexa numbers in csubst.l -HdeF
 22/10/90, added types to symbols with corresponding effect on csubst.l
	and sorting and printing -HdeF
 22/10/90  added the truncated output feature -HdeF
 23/10/90 bugfix in output of extracted names if no macros defined -HdeF
 29/10/90 bugfix in read_comment for comments with odd number of stars
 10/11/90 bugfix in sort_and_print concerning p_next_symbol
 29/11/90 removed unnecessary redefinition of NULL in csubst.h
**************************************************************************/
#include <stdio.h>
#include <assert.h>
#include "csubst.h"

extern void fatal(), fatal1(), trunc_error();
extern FILE *yyin;/* this is defined in the lex-generated file */

int Mode;		/* indicates what feature is being used */
static char *Workfile = NULL; /* this is for a file of filenames */
static char *Ignore_file = NULL; /* file of identifiers to ignore */
static FILE *Wfp = NULL;      /* this is the workfile pointer */
int Truncation = 0;		/* this says how much of 
				   identifiers we 
				   truncate in output
				 */
/* 
   The following are used to communicate argc,argv and an index
   to the function next_yyin() which is needed by   yylex()
 */
static int Arg_index, Arg_count;
static char **ArgV;

void main(argc, argv)
int argc;
char *argv[];
{
    extern FILE *try_r_open();
    int i;
    char *subst_filename = NULL;
    if(argc == 1)
    {
	puts("Usage :\ncsubst subst_file source_files ... (applies substitutions)");
	puts("or csubst substfile -F workfile (contains filenames)");
	puts("or csubst -S subst_file source_files.");
	puts("or csubst -X source_files (extracts names and strings)");
	puts("or csubst -F workfile -X (extracts names and strings)");
	puts("(In substitution mode all is sent to stdout.)");
	puts("or csubst -nTruncation -Mignore_file source_file");
	puts("  -in this mode all is sent to stdout, identifiers are truncated.");
	exit(1);
    }

    Mode = READ_SUBSTS; /* default mode */

/*  First, a loop that looks for options */
    for(i = 1; i < argc; i++)
    {
	if(*argv[i] == '-')
	{
	    switch(argv[i][1])
	    {
	    case 'n': /* use next argument as Truncation for printing */
		Mode = PRINT_TRUNCATED;
		if(argv[i][2])/* argument is right next to option, no space */
		{
		    if(EOF == sscanf(&argv[i][2], "%d", &Truncation))
			trunc_error();
		    if(!Truncation)
			trunc_error();
		    break;
		}
		i++;
		if(i == argc)
		    fatal("missing truncation parametrer");
		else
		    if(EOF == sscanf(argv[i], "%d", &Truncation))
			trunc_error();
		    if(!Truncation)
			trunc_error();
		break;

	    case 'M': /* use next argument as file of names not to be truncated */
	    case 'm':
		Mode = READ_IGNORES;
		if(argv[i][2])/* argument is right next to option, no space */
		{
		    Ignore_file = &argv[i][2];
		    break;
		}
		i++;
		if(i == argc)
		    fatal("missing Ignore_file");
		else
		    Ignore_file = argv[i];
		break;

	    case 'F': /* use next argument as "workfile" */
	    case 'f':
		if(argv[i][2])/* argument is right next to option, no space */
		{
		    Workfile = &argv[i][2];
		    break;
		}
		i++;
		if(i == argc)
		    fatal("missing workfile");
		else
		    Workfile = argv[i];
		break;

	    case 'S': /* use next  argument as subst file */
	    case 's':
		if(argv[i][2])/* argument is right next to option, no space */
		{
		    subst_filename = &argv[i][2];
		    break;
		}
		i++;
		if(i == argc)
		    fatal("missing substitutions file");
		else
		    subst_filename = argv[i];
		break;

	    case 'X': /* extract  names from arguments */
	    case 'x': 
		Mode = EXTRACT_SYMBOLS;
		break;

	    default:
		fatal("Unrecognised option");
	    }      /* end switch */
	}   /* end if */
    }/* end for */

    if(Workfile != NULL)
	Wfp = try_r_open(Workfile);
    else
    {
	Arg_index = 1;
	Arg_count = argc;
	ArgV = argv;
    }

    if(Mode == READ_IGNORES)
	{
        init_extract(); /* and use that hash table to store ignorable identifiers */
	if(Truncation == 0)
	    fatal("Cannot have -M option without -n option");
	yyin = try_r_open(Ignore_file);
	yylex(); /* next_yyin() switches input file in the middle */
	return;
    }
    else
    if(Mode == PRINT_TRUNCATED)
      {
      if(!next_yyin())
	return;
      yylex();
      return;
      }


    if(Mode == EXTRACT_SYMBOLS) /* i.e. extracting names */
    {
	if(subst_filename != NULL)
	    fatal("incompatible options: -s and -X");

	init_extract();
	if(!next_yyin())/* sets initial yyin */
	    return;
	yylex();
	sort_and_print();
	return;
    }
    else
	if(Mode == READ_SUBSTS ){
	if(subst_filename == NULL){
/* take the first argument to be the substitution file */
	    if(argv[1][0] == '-')
		fatal("Missing substitution file");
	    subst_filename = argv[1];
	    Arg_index = 2;
	}

	yyin = try_r_open(subst_filename);
	init_csubst();
	yylex();
	exit(0);
    }
}

/* This tries to open the file as an input file and brings the programme
   down if this is not possible 
 */
FILE *try_r_open(filename)
char *filename;
{
    FILE *ifp;

    ifp = fopen(filename, "r");
    if(ifp == NULL){
	fatal1("could not open %s\n", filename);
	return NULL; /* for lint */
    }
    else
	return ifp;
}

/* Issues an error message and forces the programe to abort  */
void fatal(msg)
char *msg;
{  
    fprintf(stderr,"%s\n", msg);
    exit(1);
}

/* This is a small variant of the above - see how it is used */
void fatal1(fmt, str)
char *fmt, *str;
{      
    fprintf(stderr, fmt, str);
    fputc('\n',stderr);
    exit(1);
}     

void trunc_error()
{
    fatal("argument after -n should be positive integer");
}

/* This function is called by yywrap inside yylex() when 
   yyin reaches the end of the file. It makes yyin point to
   the next file to read if possible and returns 1 iff
   such a file existed. Does not handle wildcards yet.
   This function is too complicated (it's been modified too many 
   times)
 */
int next_yyin()
{	 
    char source_filename[256];

    if((yyin != NULL) && (yyin != stdin))
	fclose(yyin);

    if(Mode == READ_SUBSTS)
    {
	Mode = APPLY_SUBSTS;
    }
    else
    if(Mode == READ_IGNORES)
    {
	 Mode = PRINT_TRUNCATED;
    }
    if(Workfile != NULL)
    {
	if (EOF != fscanf(Wfp, "%s", source_filename))
	{
	    yyin = try_r_open(source_filename);
	    return 1;
	}
	else
	    return 0;
    }	   
    else while (Arg_index  < Arg_count && ( ArgV [Arg_index][0] == '-'))
	{/* skip options */
	    if( ArgV [Arg_index][1] == 'X' &&  ArgV [Arg_index][2] != '\0')
	    { /* the filename is in close contact with the option */
		yyin = try_r_open(& ArgV [Arg_index][2]);
		Arg_index++;
		return 1;
	    }
	    else
		Arg_index ++;
	}
    if(Arg_index == Arg_count)
	return 0;
    else
	yyin = try_r_open(ArgV[Arg_index]);
    Arg_index++;
    return 1;

}
/* end of file */

