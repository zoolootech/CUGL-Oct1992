/* TLCXREF.C - "The Last Cross-referencer" - Print Xref routines		*/
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
			C Print Xref routines";

KEYWORDS:	Utility, Cross-reference, C, Pascal, Apple, Macintosh, APW, Aztec;
SYSTEM:		Macintosh MPW, v3.0;
FILENAME:	TLCXREF.C;
WARNINGS:	"Has not yet been ported to MS-DOS.
			Shareware, $20 Check/Money Order suggested.";

SEE-ALSO:	README.TLC,TLCHELP.DOC,TLPHELP.DOC;
AUTHORS:	Eduard Schwan;
COMPILERS:	AZTEC C65 v3.2b, APPLEIIGS APW C v1.0, APPLE MACINTOSH MPW C v3.0;
*/


/*------------------------ include files -------------------------*/

#include	<stdio.h>
#include	"tlc.h"


/*------------------------- definitions -------------------------*/

#define 	REF_NUM_SIZE		6	/* width of line# field */
#define		VSYM_PER_LINE		50	/* # of sym chrs dsplyd in -v mode */
#define 	PAGE_TITLE			"Xref"



/*--------------------- external declarations --------------------*/

#include	"tlc.ext"

extern VOID 	do_form_feed(),
				indent(),
				do_emphasis(),
				undo_emphasis(),
				check_user_abort(),
				new_page();
extern long		get_freemem();


/*------------------------ static variables -----------------------*/

static pos_int		out_line_number;
static pos_int		out_column;
static pos_int		out_vsym_count;		/* verbose ABC.. display counter */



/*===============[ setup_xref_stuff ]================*/

static VOID setup_xref_stuff()

	{ /* setup_xref_stuff() */
	new_page(parm_rec.do_xref, PAGE_TITLE, &out_page_number,
			&out_line_number, NULL);
	} /* setup_xref_stuff() */



/*===============[ add_ref_to_line ]=================*/

static VOID add_ref_to_line(new_sym, new_file, sym_ptr, line_ptr)
boolean 		new_sym;
boolean 		new_file;
sym_list_type*	sym_ptr;
line_list_type* line_ptr;

	{ /* add_ref_to_line() */
	pos_int 	local_indent;

/*
debug(printf("addRefToLine:NewSym=%d NewFile=%d\n",new_sym,new_file);)
*/
	/* time for a new line? */
	if (new_sym || new_file ||
		out_column+REF_NUM_SIZE>=parm_rec.right_column)
		{
		/* terminate previous line */
		putc('\n', out_file);
		out_line_number++;

		/* time for a new page heading? */
		if (out_line_number>parm_rec.bot_line ||
			out_line_number>parm_rec.bot_line-4 && new_file) /*will do 4 lines*/
			{									
			new_page(parm_rec.do_xref, PAGE_TITLE, &out_page_number,
					&out_line_number, NULL);
			new_sym = TRUE;
			new_file = TRUE;
			}

		/* time for a new symbol heading? */
		if (new_sym)
			{
			/* skip a line */
			putc('\n', out_file);
			indent();
			out_column = parm_rec.left_column;

			/* write the symbol */
			do_emphasis(parm_rec.emph_symbols);
			fprintf(out_file,"%s",sym_ptr->sym_name);
			undo_emphasis(parm_rec.emph_symbols);
			fputc('\n', out_file);
			out_line_number += 2;
			}

		indent();
		out_column = parm_rec.left_column;

		/* time for new file heading? */
		if (new_file)
			{
			/* filename indent spacing */
			for (local_indent = 0; local_indent < 6; local_indent++)
				{
				putc(' ', out_file);
				out_column++;
				}
			fprintf(out_file,"%s:\n",line_ptr->file_ptr->file_name);
			indent();
			out_column = parm_rec.left_column;
			out_line_number++;
			}

		/* first line # on line indent spacing */
		for (local_indent = 0; local_indent < 6; local_indent++)
			{
			putc(' ', out_file);
			out_column++;
			}
		} /* time for new line */
 
/*debug(fprintf(out_file, "Dx%d: line#%5d%c\n", \
ok_to_print(), line_ptr->line_number, ref_chars[line_ptr->reference_type]);)*/

		fprintf(out_file, "%5d%c",	 /*note:REF_NUM_SIZE*/
			line_ptr->line_number, ref_chars[line_ptr->reference_type]);
	out_column += REF_NUM_SIZE;

	} /* add_ref_to_line() */


/*===============[ finish_xref_stuff ]================*/

static VOID finish_xref_stuff()

	{ /* finish_xref_stuff() */
	putc('\n', out_file);	/* finish previous line */
	out_line_number++;		/* finish previous line */
	do_form_feed(out_file, out_line_number);
	} /* finish_xref_stuff() */



/*===============[ print_sym_tree ]=================*/

static VOID print_sym_tree(sym_node)
sym_list_type*	sym_node;

	{ /* print_sym_tree() */
	static boolean			new_symbol;
	static boolean			new_file;
	static file_list_type*	prev_file_ptr;
	static line_list_type*	line_ptr;

	/* print next non-null node */
	if (sym_node!=NULL)
		{
		/*
		print left subtree first (NOTE: recursion)
		*/
		print_sym_tree(sym_node->left);

		/* see if user wants to abort.. */
		check_user_abort();

		/*
		traverse the reference line # list for this symbol
		*/
debug(printf("print_sym(%s):\n",sym_node->sym_name);)
		if (verbose > 1)
			{
			if (out_vsym_count % VSYM_PER_LINE == 0)
				{
				putc('\n', stderr);
				fprintf(stderr, memdispFmt, get_freemem());
				}
			fprintf(stderr,"%c",sym_node->sym_name[0]);
			fflush(stderr);
			out_vsym_count++;
			}
		prev_file_ptr = NULL;
		new_symbol = TRUE;
		line_ptr   = sym_node->line_list;
		while (line_ptr != NULL)
			{
debug(printf("print_sym:line#%d in file '%s'\n",\
line_ptr->line_number,line_ptr->file_ptr->file_name);)
			new_file = (line_ptr->file_ptr != prev_file_ptr);
			add_ref_to_line(new_symbol, new_file, sym_node, line_ptr);
			prev_file_ptr = line_ptr->file_ptr;
			line_ptr = line_ptr->next;
			new_symbol = FALSE;
			} /*while2*/

		/*
		now print right subtree (NOTE: recursion)
		*/
		print_sym_tree(sym_node->right);
		} /*if*/
	} /* print_sym_tree() */



/*=================[ print_xref ]===================*/

VOID print_xref()

	{ /* print_xref() */
	
debug(puts("print_xref:");)

	if (verbose)
		{
		putc('\n', stderr);
		fprintf(stderr, memdispFmt, get_freemem());
		fputs("Doing Cross-Reference..", stderr);
		fflush(stderr);
		}

	setup_xref_stuff();

	/*
	traverse the symbol list, printing each symbol & its
	associated list of line references.
	*/

	out_vsym_count = 0;
	print_sym_tree(symbol_table.symbol_list);
	
	if (verbose)
		putc('\n', stderr);

	finish_xref_stuff();

	} /* print_xref() */
