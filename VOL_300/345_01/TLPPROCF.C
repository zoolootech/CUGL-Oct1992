/* TLPPROCF.C - "The Last Cross-referencer" - TLP Process Files routines*/
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
			TLP Process Files routines";

KEYWORDS:	Utility, Cross-reference, C, Pascal, Apple, Macintosh, APW, Aztec;
SYSTEM:		Macintosh MPW, v3.0;
FILENAME:	TLPPROCF.C;
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

#define 	PAGE_TITLE		"List"


/*--------------------- external declarations --------------------*/

#include	"tlc.ext"

extern FILE *	open_text_file();
extern VOID 	close_text_file(),
				do_emphasis(),
				undo_emphasis(),
				do_form_feed(),
				indent(),
				check_user_abort(),
				new_page();
extern boolean	is_resword();
extern long		get_freemem();


/*------------------------ static variables -----------------------*/

static	pos_int			out_line_number;
static	pos_int			block_level;
static	boolean 		saw_id;
static	pos_int			prev_ref_kind;/* messy way to do it..*/


/*===============[ setup_listing_stuff ]================*/

static VOID setup_listing_stuff()

	{ /* setup_listing_stuff() */
debug(puts("setup_listing_stuff:");)
	} /* setup_listing_stuff() */



/*===============[ print_listing_line ]================*/

static VOID print_listing_line()

	{ /* print_listing_line() */

	if (out_line_number >= parm_rec.bot_line || out_line_number == 0)
		new_page(parm_rec.do_listing, PAGE_TITLE, &out_page_number,
				&out_line_number, file_rec.curr_file->file_name);
	if (ok_to_print())
		{
debug(printf("prntLstLn():outLn#=%d botLn=%d pg#=%d\n",\
out_line_number,parm_rec.bot_line, out_page_number);)
		indent();
		fprintf(out_file,"%4d | %s", file_rec.curr_file->num_lines, curr_line);
		fputc('\n', out_file);
		}
	out_line_number++;
	} /* print_listing_line() */



/*===============[ finish_listing_stuff ]================*/

static VOID finish_listing_stuff()

	{ /* finish_listing_stuff() */
	if (ok_to_print() && !parm_rec.do_xref)
		{
		do_form_feed(out_file, out_line_number);
		}
	} /* finish_listing_stuff() */



/*==================[ expand_tabs ]=====================*/

static VOID expand_tabs()

	{ /* expand_tabs() */
	short 		count;
	short 		index,
				tab_fill;
	char *		line_ptr;
/*
debug(puts("expand_tabs:");)
*/
	index = 0;
	line_ptr = curr_line;
	while (*line_ptr)
		{
		if (*line_ptr == '\t')	/* horizontal tab (HT)? */
			{
			/* calc. # of spaces to replace tab with */
			tab_fill = parm_rec.tab_width - (index % parm_rec.tab_width);

			/* if more than 1 space needed for tab, move the string */
			/* and trailing null (\0) over							*/
			if (tab_fill > 1)
				{
/****************MOVE(line_ptr+tab_fill-1, line, strlen(line_ptr)+1); -- below is less risky */
				for (count=strlen(line_ptr); count>=0; count--)
					line_ptr[count-1+tab_fill] = line_ptr[count];
				}

			/* pad with spaces (& cover up tab as last char) */
/***********FILL(line_ptr, tab_fill, ' '); -- below is less risky */
			for (count=0; count<tab_fill; count++)
				line_ptr[count] = ' ';

			/* point to next char. after tab expansion */
			line_ptr	+= tab_fill;
			index		+= tab_fill;
			}
		else
			{
			/* no conversion, skip this character */
			line_ptr++;
			index++;
			}
		} /* while */
	} /* expand_tabs() */



/*=================[ do_directives ]====================*/

static boolean do_directives()

	{ /* do_directives() */

debug(puts("do_directives:");)

return(FALSE); /* DEBUG..*/
	} /* do_directives() */



/*==================[ fill_ref_recs ]===================*/

static VOID fill_ref_recs(sym_ptr, line_ptr, ref_kind)
sym_list_type*	sym_ptr;
line_list_type* line_ptr;
pos_int 		ref_kind;

	{ /* fill_ref_recs() */

	/* fill symbol record */

	/* NOTE: the symbol's name is VERY temporarily stored in token.tok_string*/
	/* and is "malloc'ed" & copied into heap later by add_sym_rec() if needed*/
	sym_ptr->sym_name		= token.tok_string;
	sym_ptr->sym_type		= token.tok_type;

	/* fill line record */

	line_ptr->line_number	= file_rec.curr_file->num_lines;
	line_ptr->file_ptr		= file_rec.curr_file;
	line_ptr->reference_type= ref_kind;
	} /* fill_ref_recs() */



/*===================[ add_sym_rec ]====================*/

static sym_list_type* add_sym_rec(sym_ptr)
sym_list_type*	sym_ptr;

	{ /* add_sym_rec() */
	sym_list_type*	new_sym_ptr;
	char*			new_sym_name;
/*
debug(puts("add_sym_rec:");)
*/
	/* create a new symbol record & move info into it */
	new_sym_ptr = (sym_list_type*)malloc(sizeof(sym_list_type));

debug(printf("add_sym_rec:malloc(new_sym_ptr)-$%06lx[$%x]\n",\
(long)new_sym_ptr,sizeof(sym_list_type));)

	if (new_sym_ptr==NULL)
		{
		fprintf(stderr,"Error! out of memory on line #%u in file '%s'\n",
				file_rec.curr_file->num_lines+1, file_rec.curr_file->file_name);
		}
	else
		{
#ifdef STRUCT_ASSIGN
		*new_sym_ptr = *sym_ptr;
#else
		MOVEBYTE(sym_ptr, new_sym_ptr, sizeof(sym_list_type));
#endif
		new_sym_ptr->line_list	= NULL;
		new_sym_ptr->line_tail	= NULL;
		new_sym_ptr->left		= NULL;
		new_sym_ptr->right		= NULL;
		}

	/* create a place for the symbol name & move from temp area */

	if (new_sym_ptr!=NULL)
		{
		new_sym_name = malloc(strlen(new_sym_ptr->sym_name)+1);

debug(printf("add_line_rec:malloc(new_sym_name)-$%06lx[$%x]\n",\
(long)new_sym_name,strlen(new_sym_ptr->sym_name)+1);)

		if (new_sym_name==NULL)
			{
			fprintf(stderr,"Error! out of memory on line #%u in file '%s'\n",
					file_rec.curr_file->num_lines+1, file_rec.curr_file->file_name);
			}
		else
			{
			strcpy(new_sym_name, sym_ptr->sym_name);
			new_sym_ptr->sym_name = new_sym_name;
			}
		}

	/* return the pointer to the new record, or NULL if out of mem */

	return(new_sym_ptr);

	} /* add_sym_rec() */


/*==================[ add_line_rec ]====================*/

static int add_line_rec(sym_ptr, line_ptr)
sym_list_type*	sym_ptr;
line_list_type* line_ptr;

	{ /* add_line_rec() */
	line_list_type* new_line_ptr;

debug(puts("add_line_rec:");)

	/* create new line ref. record on heap */
	new_line_ptr = (line_list_type*)malloc(sizeof(line_list_type));

debug(printf("add_line_rec:malloc(new_line_ptr)-$%06lx[$%x]\n",\
(long)new_line_ptr,sizeof(line_list_type));)

	if (new_line_ptr==NULL)
		{
		fprintf(stderr,"Error! out of memory on line #%u in file '%s'\n",
				file_rec.curr_file->num_lines+1, file_rec.curr_file->file_name);
		return(BAD_EXIT);
		}
#ifdef STRUCT_ASSIGN
	*new_line_ptr = *line_ptr;
#else
	MOVEBYTE(line_ptr, new_line_ptr, sizeof(line_list_type));
#endif

	/* add line ref. record to symbol's list */
	new_line_ptr->next = NULL;
	if (sym_ptr->line_list == NULL)
		{ /* first on list */
debug(puts("  1st linerec on list");)
		sym_ptr->line_list = new_line_ptr;
		sym_ptr->line_tail = new_line_ptr;
		}
	else
		{
debug(puts("  next linerec on list");)
		sym_ptr->line_tail->next = new_line_ptr;
		sym_ptr->line_tail = new_line_ptr;
		}

	/* all ok */
	return(GOOD_EXIT);

	} /* add_line_rec() */



/*==================[ add_reference ]===================*/

static int add_reference(sym_ptr, line_ptr)
sym_list_type*	sym_ptr;
line_list_type* line_ptr;

	{ /* add_reference() */
	boolean 		found;
	int				direction;
	sym_list_type*	search_ptr;
	sym_list_type*	trail_ptr;

	/* search for symbol or place to add symbol in symbol table */

/*debug(puts("add_ref:");)*/

	search_ptr = symbol_table.symbol_list;
	found = FALSE;
	trail_ptr = NULL; /* this will always point one above search_ptr(parent)*/
	while (!found && search_ptr!=NULL)
		{
		trail_ptr = search_ptr;
		direction = strncmp(search_ptr->sym_name, sym_ptr->sym_name,
							parm_rec.symbol_size);
		if (direction < 0)
			direction = -1;
		else if (direction > 0)
			direction = 1;
/*
debug(printf("add_ref:strncmp'%s'<%d>'%s'\n",\
search_ptr->sym_name,direction,sym_ptr->sym_name);)
*/
		switch (direction)
			{
			case  1: /* too big, descend left */
					search_ptr = search_ptr->left;
					break;
			case  0: /* equal! got it. */
					found = TRUE;
					break;
			case -1: /* too small, descend right */
			default:
					search_ptr = search_ptr->right;
					break;
			} /*switch*/
		} /*while*/

	/* if not found, add new symbol record at trail_ptr */

	if (!found)
		{
		search_ptr = add_sym_rec(sym_ptr);
		if (search_ptr==NULL)
			return(BAD_EXIT); /* out of memory */
		if (trail_ptr==NULL)
			{ /* first on the list */
/*debug(puts(" 1st symrec on list");)*/
			symbol_table.symbol_list = search_ptr;
			}
		else
			{ /* add to existing list */
/*debug(puts(" next symrec on list");)	 */
			if (direction==1)
				trail_ptr->left = search_ptr; /* add as left node */
			else
				trail_ptr->right= search_ptr; /* add as left node */
			}
		}

	/* now add line ref record to this symbol record */

	return( add_line_rec(search_ptr, line_ptr) );

	} /* add_reference() */



/*====================[ parse_line ]====================*/

static VOID parse_line(found_include)
boolean * found_include; /* NOTE: not used yet - not doing includes */

	{ /* parse_line */
	int 			error;
	pos_int			ref_kind;
	sym_list_type	temp_sym_rec;
	line_list_type	temp_line_rec;

	do	{
		error = get_token();
		if (error)
			{ /* handle bad token or Eoln */
			if (error != TERR_EOLN)
				{ /* bad stuff in text file.. */
debug(puts("UNKNOWN TOKEN");)
				}
			}
		else
			{ /* handle next valid token */
/*
debug(printf("parseLine():token=%d'%s'\n",\
token.tok_type,token.tok_string);)
*/
			switch (token.tok_type)
				{
				case TOK_ID:
						if (is_resword(token.tok_string))
							{ /* handle reserved words.. */
							if (strcmp(token.tok_string, "function") == 0)
								prev_ref_kind = REF_FUNCHEAD;
							else
							if (strcmp(token.tok_string, "procedure") == 0)
								prev_ref_kind = REF_PROCHEAD;
							else
							if (strcmp(token.tok_string, "unit") == 0)
								prev_ref_kind = REF_UNITHEAD;
							else
								prev_ref_kind = REF_BLANK;
							}
						else
							{ /* not reserved, add ref. to sym. table */
							fill_ref_recs(&temp_sym_rec,
										  &temp_line_rec,
										  prev_ref_kind);
							error = add_reference(&temp_sym_rec,
												 &temp_line_rec);
							prev_ref_kind = REF_BLANK;
							}
						break;

				case TOK_NCONST:
						if (parm_rec.xref_nums)
							{
							ref_kind = REF_BLANK;
							fill_ref_recs(&temp_sym_rec,
										  &temp_line_rec,
										  ref_kind);
							error = add_reference(&temp_sym_rec,
												 &temp_line_rec);
							}
						prev_ref_kind = REF_BLANK;
						break;

				case TOK_CCONST:
				case TOK_SCONST:
						if (parm_rec.xref_strings)
							{
							ref_kind = REF_BLANK;
							fill_ref_recs(&temp_sym_rec,
										  &temp_line_rec,
										  ref_kind);
							error = add_reference(&temp_sym_rec,
												 &temp_line_rec);
							}
						prev_ref_kind = REF_BLANK;
						break;

				default: /* ignore anything else.. */
					prev_ref_kind = REF_BLANK;
					break;

				} /*switch*/
			} /*else valid token*/
		} while (token.tok_type != TERR_EOLN && !error);
	} /* parse_line */



/*=================[ process_a_file ]===================*/

static boolean process_a_file()

	{ /* process_a_file() */
	int 	error;
	boolean found_include;
	FILE *	fp;

	if (verbose)
		{
		putc('\n', stderr);
		fprintf(stderr, memdispFmt, get_freemem());
		fprintf(stderr, "Processing file '%s'\n", file_rec.curr_file->file_name);
		fflush(stderr);
		}

	block_level 	= 0;
	saw_id			= FALSE;
	prev_ref_kind	= REF_BLANK;

	fp = open_text_file(file_rec.curr_file->file_name, "r", &error);
	if (error)
		{
		fprintf(out_file, "Error %d/$%x opening file '%s'\n",
				error, error, file_rec.curr_file->file_name);
		fprintf(stderr, "Error %d/$%x opening file '%s'\n",
				error, error, file_rec.curr_file->file_name);
		fflush(stderr);
		return(TRUE);
		}
	else
		{ /*opened ok*/
		file_rec.curr_file->num_lines = 0;
		found_include = FALSE;
		while (!feof(fp) && !error)
			{
			/* check for user abort.. */
			check_user_abort();

			/* read next line in file */
			errno = 0;
			if (fgets(curr_line, LINE_SIZE, fp) != NULL)
				{
				if (curr_line[strlen(curr_line)-1] == '\n')
					curr_line[strlen(curr_line)-1] = 0; /* remove trailing \n */
				file_rec.curr_file->num_lines++;
				expand_tabs();

debug(printf("process_a_file:<%d>'%s'\n",file_rec.curr_file->num_lines,curr_line););

				/* if noisy, list file's current line number to screen */
				if (verbose > 1)
					{
					if (file_rec.curr_file->num_lines % 10 == 1)
						{
						if (file_rec.curr_file->num_lines % 100 == 1)
							{
							putc('\n', stderr);
							fprintf(stderr, memdispFmt, get_freemem());
							}
						fprintf(stderr,"%5d",file_rec.curr_file->num_lines-1);
						fflush(stderr);
						}
					}

/* don't parse if just listing (but need to later!) DEBUG */
				if (parm_rec.do_xref)
					parse_line(&found_include);

				/* if we are listing, print this line */
/*
debug(printf("process_a_file:if(%d) then print'%s'\n",parm_rec.do_listing,curr_line););
*/
				if (parm_rec.do_listing)
					print_listing_line();

				/* see if this line contains a NewPage command string */
				if (parm_rec.paging_string != NULL)
					if (strcmp(parm_rec.paging_string, curr_line)==0)
						new_page(parm_rec.do_listing,
								PAGE_TITLE,
								&out_page_number,
								&out_line_number,
								file_rec.curr_file->file_name);

				/* handle include file */
				if (found_include && parm_rec.do_includes)
					{ /* push current file & enter include file */
/*----------------- later ---------------
					found_include = FALSE;
					push-current-file()
					open-include-file()
					include-level++
					init file stuff
					if (!top-of-page)
						new-page()
----------------------------------------*/
					}
				} /*if fgets()*/
			else
				{ /* error or eof */
				if (!feof(fp))
					{
					error = errno;
					fprintf(out_file,
							"Error %d/$%x reading line #%u in file '%s'\n",
							error, error, file_rec.curr_file->num_lines+1,
							file_rec.curr_file->file_name);
					fprintf(stderr,
							"Error %d/$%x reading line #%u in file '%s'\n",
							error, error, file_rec.curr_file->num_lines+1,
							file_rec.curr_file->file_name);
					fflush(stderr);
					return(TRUE);
					}
				} /* else error or eof */
/*----------------- later ---------------
			if (eof(fp) && !error)
				{
				if include-level > 0
					{
					pop-previous-file()
					seek back to it's next line
					}
				}
----------------------------------------*/
			} /*while*/
		close_text_file(fp, file_rec.curr_file->file_name);
		} /*opened ok*/
		
		if (verbose)
			putc('\n', stderr);
			
	return(FALSE); /* no abort */
	} /* process_a_file() */



/*=================[ process_files ]===================*/
VOID process_files()

	{ /* process_files() */
	boolean abort;

debug(puts("process_files:");)

	init_scanner(parm_rec.do_case_sens, parm_rec.do_underscores, FALSE);
	file_rec.curr_file = file_rec.file_list;
	abort			 = FALSE;
	out_page_number  = 0;
	if (parm_rec.do_listing)
		setup_listing_stuff();
	while (file_rec.curr_file!=NULL && !abort)
		{
		out_line_number = 0;
		abort = process_a_file();

/*IGNORE MISSING FILES FOR NOW..DEBUG*/ abort=FALSE;
		file_rec.curr_file = file_rec.curr_file->next;
		}
		
	if (parm_rec.do_listing)
		finish_listing_stuff();
		
	} /* process_files() */
