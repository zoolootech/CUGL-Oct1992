/* TLCRESW.C - "The Last Cross-referencer" - C Reserved word routines	*/
/*	Last Modified:	07/02/89											*/

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
			C Reserved word routines";

KEYWORDS:	Utility, Cross-reference, C, Pascal, Apple, Macintosh, APW, Aztec;
SYSTEM:		Macintosh MPW, v3.0;
FILENAME:	TLCRESW.C;
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


/*--------------------- external declarations --------------------*/

#include	"tlc.ext"

extern	char*	fgets();
extern	VOID	init_scanner();
extern	int 	get_token();
extern	FILE	*open_text_file();
extern	int 	close_text_file();


/*------------------------ static variables -----------------------*/

static	pos_int line_num;


/*=====================[ out_of_mem ]======================*/

static VOID out_of_mem(resword)
char	*resword;

	{ /* out_of_mem */
	fprintf(stderr,
		"out of memory adding '%s' on line %d of ResWordFile '%s'\n",
		resword, line_num, file_rec.resw_fname);
	exit(BAD_EXIT);
	} /* out_of_mem */


/*====================[ is_resword ]=======================*/

int is_resword(resword)
char *	resword;

	{ /* is_resword() */
	int 				found;
	int 				direction;
	resw_list_type *	resw_ptr;

	found = FALSE;
	resw_ptr = resw_rec.resw_list;
	while (!found && resw_ptr!=NULL)
	 {
		direction = strncmp(resword, resw_ptr->resword,
							 parm_rec.symbol_size);
		if (direction<0)
			direction=-1;
		else if (direction>0)
			direction=1;
/*
debug(printf("is_resw(%s:%d):'%s'=%d\n",resword,parm_rec.symbol_size,resw_ptr->resword,direction);)
*/
		switch (direction)
			{
			case  0: /* found! */
					found = TRUE;/*DEBUG:return token_type in rec later*/
					resw_ptr->occurrances++;
					break;
			case -1: /* too high, go left */
					resw_ptr = resw_ptr->left;
					break;
			case  1: /* too low, go right */
			default:
					resw_ptr = resw_ptr->right;
					break;
			} /*switch*/
		} /*while*/

	return(found);

	} /* is_resword() */



/*====================[ add_resword ]======================*/

static resw_list_type * add_resword(resword, list_node)
char			*resword;
resw_list_type	*list_node;

	{ /* add_resword */

	if (list_node == NULL)
		{ /* end of branch, add new node */
		list_node = (resw_list_type*) malloc(sizeof(resw_list_type));
		if (list_node == NULL)
			{ /* couldn't create record from heap, error exit */
			out_of_mem(resword);
			}
		/*
		initialize new node fields
		*/
		list_node->left 	= NULL;
		list_node->right	= NULL;
		list_node->resword	= malloc(strlen(resword)+1);
		list_node->occurrances = 0;
		if (list_node->resword == NULL)
			{ /* couldn't create record from heap, error exit */
			out_of_mem(resword);
			}
		strcpy(list_node->resword, resword);
		} /* end of branch */
	else
		{ /* traverse left or right sub-tree of this node */
		if (strcmp(resword, list_node->resword) < 0)
			{ /* resword lower, go left */
			list_node->left = add_resword(resword, list_node->left);
			}
		else
			{ /* resword higher, go right */
			list_node->right = add_resword(resword, list_node->right);
			}
		} /* else traverse */
	/*
	return this node to the caller
	*/
	return (list_node);
	} /* add_resword */


/*=================[ handle_resword_file ]===================*/

VOID handle_resw_file()

	{ /* handle_resw_file() */
	int 	error;
	boolean end_of_file;
	FILE	*fp;

	if (verbose)
		{
		fprintf(stderr, "Reading ResWordsFile '%s'\n",
				file_rec.resw_fname);
		fflush(stderr);
		}

	fp = open_text_file(file_rec.resw_fname, "r", &error);
	if (error)
		{
		fprintf(stderr, "Error #%d/$%x opening ResWordsFile '%s'\n",
				error, error, file_rec.resw_fname);
	fflush(stderr);
		exit(BAD_EXIT);
		}

	init_scanner(TRUE, TRUE, TRUE); /* CaseSens=T UseUndersc=T DelQuotes=T */
	line_num = 0;
	end_of_file = feof(fp);
	while (!end_of_file && !error)
		{ /* do each line in the file */
		errno = 0;
		end_of_file = (fgets(curr_line, LINE_SIZE, fp) == NULL);
		line_num++;
		error = errno;
		if (end_of_file)
			error = 0;	/* some O.S.s will set errno too! */
/*
debug(printf("resw line #%d:%s",line_num,curr_line);)
*/
		while (!end_of_file && !error)
			{ /* get words on a line */
			error = get_token(curr_line);
			if (!error)
				{
				if (token.tok_type == TOK_ID)
					{
/*
debug(printf(" add resword '%s'\n",token.tok_string);)
*/
					resw_rec.resw_list =
						add_resword(token.tok_string, resw_rec.resw_list);
					resw_rec.num_reswords++;
					}
				else
					{
					fprintf(stderr,
					"illegal char '%c' in ResWordFile '%s' line %d col %d\n",
					token.tok_string[0], file_rec.resw_fname,
					line_num, token.tok_column);
					fflush(stderr);
					exit(BAD_EXIT);
					}
				} /* if !error */
			} /*while in line*/

			/*
			if a new line is needed, not really an error
			*/
			if (error == TERR_EOLN)
				error = 0;
		} /*while !eof*/

	/*
	handle any fatal errors
	*/
	if (error)
		{
		fprintf(stderr,
			"error # %d/$%x reading line %d of ResWordFile '%s'\n",
			error, error, line_num, file_rec.resw_fname);
		fflush(stderr);
		exit(BAD_EXIT);
		}
	error = close_text_file(fp, file_rec.resw_fname);
	} /* handle_resw_file() */
