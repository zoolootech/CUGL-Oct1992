/* TLCSTAT.C - "The Last Cross-referencer" - Print Stats routines		*/
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
			Print Stats routines";

KEYWORDS:	Utility, Cross-reference, C, Pascal, Apple, Macintosh, APW, Aztec;
SYSTEM:		Macintosh MPW, v3.0;
FILENAME:	TLCSTAT.C;
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

#define 	PAGE_TITLE		"Stats"
#define		stat_reswords	1
#define		stat_fnames		2


/*--------------------- external declarations --------------------*/

#include	"tlc.ext"

extern VOID 	do_form_feed(),
				indent(),
				new_page();
extern long 	get_freemem();


/*------------------------ static variables -----------------------*/

static	pos_int			out_line_number;
static	pos_int			which_stat;


/*===============[ setup_stat_stuff ]================*/

static VOID setup_stat_stuff()

	{ /* setup_stat_stuff() */
	} /* setup_stat_stuff() */



/*===============[ print_stat_line ]================*/

static VOID print_stat_line(line_to_print)
char *	line_to_print;

	{ /* print_stat_line() */
	
debug(printf("prntStatLn():outLn#=%d botLn=%d pg#=%d\n",\
out_line_number,parm_rec.bot_line, out_page_number);)

	if (out_line_number >= parm_rec.bot_line || out_line_number == 0)
		{
		new_page(parm_rec.do_stats, PAGE_TITLE,
				&out_page_number, &out_line_number, NULL);
		if (ok_to_print()
			&& parm_rec.do_stats
			&& (which_stat == stat_fnames))
			{
			indent();
			fprintf(out_file, "-- List of Files --\n\n");
			indent();
			fprintf(out_file, "File# #Lines  FileName\n");
			indent();
			fprintf(out_file, "----- ------- --------\n");
			out_line_number += 4;
			}
		}
	if (ok_to_print() && parm_rec.do_stats)
		{
		indent();
		fprintf(out_file, line_to_print);
		fputc('\n', out_file);
		}
	out_line_number++;
	} /* print_stat_line() */



/*===============[ finish_stat_stuff ]================*/

static VOID finish_stat_stuff()

	{ /* finish_stat_stuff() */
	if (ok_to_print())
		{
		do_form_feed(out_file, out_line_number);
		}
	} /* finish_stat_stuff() */



/*=================[ print_resword_tree ]=================*/

static VOID print_resword_tree(resw_node)
resw_list_type *	resw_node;

	{ /* print_resword_tree() */
	static char	temp_line[30]; /* dont' need this allocated recursively! */
	
	if (resw_node != NULL)
		{
		/* print the lower words */
		print_resword_tree(resw_node->left);
		
		/* print the current node's word */
		sprintf(temp_line, "%8lu:%-16s",
				resw_node->occurrances, resw_node->resword);
		if (strlen(curr_line)+strlen(temp_line) >
			parm_rec.right_column - parm_rec.left_column)
			{ /* won't fit, dump full line first */
			print_stat_line(curr_line);
			curr_line[0] = '\0';
			}
		strcat(curr_line, temp_line);
		
		/* print the higher words */
		print_resword_tree(resw_node->right);
		}
	} /* print_resword_tree() */



/*================[ print_reswords ]=================*/

static VOID print_reswords()

	{ /* print_reswords() */

	if (verbose > 1)
		{
		putc('\n', stderr);
		fprintf(stderr, memdispFmt, get_freemem());
		fputs("-Reserved Word List..", stderr);
		fflush(stderr);
		}

	which_stat = stat_reswords;
	out_line_number = 0;
	print_stat_line("-- Reserved Word Occurrances --");
	print_stat_line("");
	curr_line[0] = '\0';
	print_resword_tree(resw_rec.resw_list);
	/* print any remaining non-full line */
	if (strlen(curr_line) > 0)
		print_stat_line(curr_line);
	} /* print_reswords() */


/*=================[ print_fname_list ]=================*/

static VOID print_fname_list(flist_node, num_fnames)
file_list_type*	flist_node;
pos_int			num_fnames; /* NOTE: so far, only needed for debug */

	{ /* print_fname_list() */

	while (flist_node != NULL)
		{		
debug(printf("prntFnameLst():node=$%lx file#=%d fname='%s'\n",\
flist_node,flist_node->file_num,flist_node->file_name);)
		/* print the current node's info */
		sprintf(curr_line, "%4d  %6u  %s",
				flist_node->file_num,
				flist_node->num_lines,
				flist_node->file_name);
		print_stat_line(curr_line);
	
		/* go to next file name node */
		flist_node = flist_node->next;
		}
	} /* print_fname_list() */



/*================[ print_filenames ]=================*/

static VOID print_filenames()

	{ /* print_filenames() */

	if (verbose > 1)
		{
		putc('\n', stderr);
		fprintf(stderr, memdispFmt, get_freemem());
		fputs("-FileName List..", stderr);
		fflush(stderr);
		}

	which_stat = stat_fnames;
	out_line_number = 0;
	curr_line[0] = '\0';
	print_fname_list(file_rec.file_list, file_rec.num_in_fnames);

	} /* print_filenames() */


/*=================[ print_stats ]===================*/

VOID print_stats()

	{ /* print_stats() */
	
debug(printf("print_stat:\n");)

	if (verbose)
		{
		fprintf(stderr, "\n[%8ld] Doing Statistics..\n", get_freemem());
		fflush(stderr);
		}

	setup_stat_stuff();
	
	print_reswords();
	
	print_filenames();

	finish_stat_stuff();

	} /* print_stats() */
