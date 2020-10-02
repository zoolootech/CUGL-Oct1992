/* TLCPARM.C - "The Last Cross-referencer" - Parameter file routines	*/
/*	Last Modified:	02/10/89											*/

/*
---------------------------------------------------------------------
Copyright (c) 1987-1989, Eduard Schwan Programs [esp] - All rights reserved
TLC (The Last C-Cross-Referencer) and TLP (same, but for Pascal) are
Cross-Reference Generators crafted and shot into the Public Domain by
Eduard Schwan.  The source code and executable program may be freely
distributed as long as the copyright/author notices remain intact, and
it is not used in part or whole as the basis of a commercial product.
Any comments, bug-fixes, or enhancements are welcome.
Also, if you find TLC and it's source code useful, a contribution of
$20 (check/money order) is encouraged!  Hopefully we will all see more
source code distributed!
	Eduard Schwan, 1112 Oceanic Drive, Encinitas, Calif. 92024
---------------------------------------------------------------------
*/

/*
HEADER:		The Last Cross-Referencer;
TITLE:		TLC/TLP - The Last Cross-Referencer;
VERSION:	1.01;

DESCRIPTION: "TLC/TLP.
			Parameter file routines";

KEYWORDS:	Utility, Cross-reference, C, Pascal, Apple, Macintosh, APW, Aztec;
SYSTEM:		Macintosh MPW, v3.0;
FILENAME:	TLCPARM.C;
WARNINGS:	"Has not yet been ported to MS-DOS.
			Shareware, $20 Check/Money Order suggested.";

SEE-ALSO:	README.TLC,TLCHELP.DOC,TLPHELP.DOC;
AUTHORS:	Eduard Schwan;
COMPILERS:	AZTEC C65 v3.2b, APPLEIIGS APW C v1.0, APPLE MACINTOSH MPW C v3.0;
*/


/*------------------------ include files -------------------------*/

#include	<stdio.h>
#include	<errno.h>
#include	"tlc.h"


/*------------------------- definitions -------------------------*/

#define 	PARMN_SIZE			15
#define 	EXPECT_PARMNAME 	1
#define 	EXPECT_EQUALS		2
#define 	EXPECT_VALUE		3


/*--------------------- external declarations --------------------*/

#include	"tlc.ext"

extern	char*	fgets();
extern	VOID	init_scanner();
extern	int 	get_token();
extern	FILE	*open_text_file();
extern	int 	close_text_file();
extern	VOID	set_parm();


/*------------------------ static variables -----------------------*/

static pos_int	line_num, expecting;
static char 	parm_name[PARMN_SIZE+1];


/*=================[ do_parmname ]===================*/

static VOID do_parmname()

	{ /* do_parmname() */
/*
debug(printf("do_parmname:\n");)
*/
	/*
	save parm_name & advance to next state..
	*/
	if (token.tok_type == TOK_ID)
		{
		/*
		copy parm-name into buffer.  if the name is >= PARMN_SIZE,
		then strncpy() won't copy the null byte at the end, so
		we have to take care of that case.
		*/
		strncpy(parm_name, token.tok_string, PARMN_SIZE);
		parm_name[PARMN_SIZE] = 0;
		expecting = EXPECT_EQUALS;
		}
	else
		{
		fprintf(stderr,
			"expected a parm-name on line %d col %d of ParmFile '%s'\n",
			line_num, token.tok_column, file_rec.parm_fname);
		exit(BAD_EXIT);
		}
	} /* do_parmname() */


/*=================[ do_equals ]===================*/

static VOID do_equals()

	{ /* do_equals() */
/*
debug(printf("do_equals:\n");)
*/
	/*
	just advance to next state..
	*/
	if (token.tok_type == TOK_EQUALS)
		expecting = EXPECT_VALUE;
	else
		{
		fprintf(stderr,
			"expected an '=' on line %d col %d of ParmFile '%s'\n",
			line_num, token.tok_column, file_rec.parm_fname);
		exit(BAD_EXIT);
		}
	} /* do_equals() */


/*=================[ do_value ]===================*/

static VOID do_value()

	{ /* do_value() */
/*
debug(printf("do_value:\n");)
*/

	/*
	set parm_name to value & advance to first state..
	*/
	if (token.tok_type == TOK_SCONST ||
		token.tok_type == TOK_NCONST)
		{
		set_parm(line_num, parm_name);
		expecting = EXPECT_PARMNAME;
		}
	else
		{
		fprintf(stderr,
			"expected a parm-value on line %d col %d of ParmFile '%s'\n",
			line_num, token.tok_column, file_rec.parm_fname);
		exit(BAD_EXIT);
		}
	} /* do_value() */


/*=================[ handle_parm_file ]===================*/

VOID handle_parm_file()

	{ /* handle_parm_file() */
	int 	error;
	boolean end_of_file;
	FILE	*fp;

	if (verbose)	
		fprintf(stderr, "Reading ParmFile '%s'\n", file_rec.parm_fname);

	fp = open_text_file(file_rec.parm_fname, "r", &error);
	if (error)
		{
		fprintf(stderr, "Error #%d/$%x opening ParmFile '%s'\n",
				error, error, file_rec.parm_fname);
		exit(BAD_EXIT);
		}

	init_scanner(FALSE, FALSE, TRUE); /* CaseSens=F UseUndersc=F DelQuotes=T */
	line_num = 0;
	end_of_file = feof(fp);
	while (!end_of_file && !error)
		{ /* do each line in the file */
		/*
		read next line in file
		*/
		errno = 0;
		end_of_file = (fgets(curr_line, LINE_SIZE, fp) == NULL);
debug(printf("%dparm line #%02d:%s",end_of_file,line_num,curr_line);)
		line_num++;
		error = errno;
		if (end_of_file)
			error = 0;
		if (error)
			{
			fprintf(stderr,
			"error # %d/$%x reading line %d of ParmFile '%s'\n",
			error, error, line_num, file_rec.parm_fname);
			exit(BAD_EXIT);
			}

		/*
		handle tokens on this line, note that each line resets the
		state variable, so "parmname = value" cannot wrap across lines.
		*/
		expecting = EXPECT_PARMNAME;
		while (!end_of_file && !error)
			{ /* get parms on a line */
			error = get_token(curr_line);
			if (!error)
				{
				switch (expecting)
					{
					case EXPECT_PARMNAME:
						do_parmname();
						break;
					case EXPECT_EQUALS:
						do_equals();
						break;
					case EXPECT_VALUE:
						do_value();
						break;
					} /*switch*/
				} /* if !error */
			} /*while in line*/

		/*
		if a new line is needed, not really an error
		*/
		if (error)
			{
			if (error == TERR_EOLN)
				error = 0;
			else
				{
				fprintf(stderr,
			"error # %d happened parsing line %d, col %d of ParmFile '%s'\n",
				error, line_num, token.tok_column, file_rec.parm_fname);
				exit(BAD_EXIT);
				}
			}
		} /*while !eof*/

	/*
	handle any fatal errors
	*/
	if (error)
		{
		fprintf(stderr,
			"error # %d/$%x reading line %d of ParmFile '%s'\n",
			error, error, line_num, file_rec.parm_fname);
		exit(BAD_EXIT);
		}
	error = close_text_file(fp, file_rec.parm_fname);
	} /* handle_parm_file() */
