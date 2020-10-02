/*	Command utilities	*/

/*	This package contains a variety of functions useful for writing
	command processors, including a standard command line argument
	processor.

	Kevin Kenny
	729-A E. Cochise Dr.
	Phoenix, Arizona   85020.

	This code is in the public domain.	*/

/*	Functions included:

abbrev		Test for a valid abbreviation.
initv		Initialize an integer vector.
patmat		Match a name with a pattern; optionally, substitute in a
		second pattern.
procarg		Process an argument from command line.
showsynt	Display the correct syntax for a command.

*/

#include <bdscio.h>
#include <cmdutil.h>

/*	ABBREV - Test for a valid abbreviation */

abbrev (text, pattern)
    char * text;		/* Text to test */
    char * pattern;		/* Pattern to match it against; lowercase
					letters are optional */
    {
	while (*pattern != '\0') {
		if (islower (*pattern)) {	/* Optional character */
			if (tolower (*text) == *pattern++
			 && abbrev (text + 1, pattern))
				return (TRUE);
			}
		else {				/* Required character */
			if (toupper (*text++) != *pattern++)
				return (FALSE);
			}
		}
	return (*text == '\0');
	}

/*	Initv - initialize a vector	*/

int * initv (vect, endval, contents)
    int *vect;   			/* Vector to initialize */
    int endval;				/* Value flagging end of list */
    int contents;			/* Stuff to put in it */
    {
	int *contp;

	contp = &contents;
	while ((*vect++ = *contp++) != endval);
	}

/*	Patmat - Name pattern matcher 	*/

int patmat (name, pat, equiv, newname)
    char *name, *pat, *equiv, *newname;
     {

	char c, mc, flag;

	mc = *pat++;			/* Get char from pat */
	while (c = *name++) {		/* Corresponding char */
		switch (mc) {

case '?':				/* '?' matches any single char */
			if (equiv) {
				if (_passtxt (&equiv, &newname)) {
					*newname++ = c;
					if (*pat != '?') ++equiv;
					}
				}
			mc = *pat++;
			break;

case '*':				/* '*' matches any string */
			if (equiv)
				flag = _passtxt (&equiv, &newname);
			else
				flag = 0;
			if (patmat (name-1, pat, equiv+flag, newname))
				return (TRUE);
			if (flag) *newname++ = c; 
			break;

default:				/* Anything else matches only itself */
			if (toupper (mc) != toupper (c))
				return (FALSE);
			mc = *pat++;
			break;
			}
		}

	if ((mc != '*' || *pat) && mc) return (FALSE);
	if (equiv) {
		while (_passtxt (&equiv, &newname)) ++equiv;
		*newname = '\0';
		}
	return (TRUE);
	}

char _passtxt (insp, otsp)
    char **insp, **otsp;
    {

	char c;

	while ((c = **insp) && c != '*') {	
		*(*otsp)++ = c;
		++*insp;
		}
	return (c != 0);
	}

/*	Process an argument from command line	*/

int procarg (argc, argv, optable, info) 
    int * argc;			/* Argument count */
    char * * * argv;		/* Argument vector */
    struct option * optable;	/* Option list */
    char * * * info;		/* Returned information */
    {
	int optno;		/* Option number */
        char * argtext;		/* Argument text */
	int parmno;		/* Parameter counter for multi-value
				   options. */
	
	argtext = *(*argv)++;		/* Pick up an argument */
	--*argc;

	if (!_isopt (argtext)) {
		*info = argtext;	/* String, not an option */
		return (-1);
		}

	for (optno = 0; optable -> opt_text != EOF; ++optno, ++optable)
		if (abbrev (argtext + 1, optable -> opt_text)) break;
				/* Search for optable entry */

	if (optable -> opt_text == EOF) {
		fprintf (STD_ERR, "\n-%s: unknown option.", argtext);
		*info = argtext;
		return (-2);		/* Unrecognized option */
		}

	switch (optable -> opt_type) {
	    case NAKED_KWD:
		*info = 0;
		return (optno);
	    case NVAL_KWD:
	    case SVAL_KWD:
		if (*argc == 0 || _isopt (**argv)) {
			fprintf (STD_ERR, "\n-%s option requires a value.", 
				optable -> opt_text);
			*info = argtext;
			return (-2);
			}
		break;
	    case MNVL_KWD:
	    case MSVL_KWD:
		break;
	    default:
		fprintf (STD_ERR, "\nBug: optable badly constructed.");
		exit ();
		}

	switch (optable -> opt_type) {
	    case SVAL_KWD:
		*info = *(*argv)++;	/* Pick up next arg string */
		--*argc;
	    	return (optno);
	    case NVAL_KWD:
		*info = atoi (*(*argv)++);
		--*argc;
		return (optno);
	    default:
		*info = sbrk (*argc + *argc + 2);
		break;
		}

	for (parmno = 0; 
		*argc && !_isopt (argtext = **argv);
		--*argc, ++*argv, ++parmno) {

		if (optable -> opt_type == MSVL_KWD) {
			(*info) [parmno + 1] = argtext;
			}
		else {
			(*info) [parmno + 1] = atoi (argtext);
			}
		}
	(*info) [0] = parmno;
	return (optno);
	}

/* Test if a string is a command option */

int _isopt (s)
    char * s;
    {
	return (*s++ == '-' && isalpha (*s));
	}

/* Display command syntax */

showsyntax (command, optable)
    char * command;
    struct option * optable;
    {
	char * opstr [6];

	initv (opstr, EOF, "", "<s>", "<n>", "<s> <s>...", "<n> <n>...", EOF);

	fprintf (STD_ERR, "\nSyntax: %s", command);

	if (optable -> opt_text == EOF) return;

	fprintf (STD_ERR, " <options>\nOptions:");

	while (optable -> opt_text != EOF) {

		fprintf (STD_ERR, "\n\t-%s %s", optable -> opt_text,
			opstr [optable -> opt_type]);
		++optable;

		}
	}

		return (optno);
	    default:
		*info = sbrk (*argc + *argc + 2);
		break;
		}

	for (parmno = 0;