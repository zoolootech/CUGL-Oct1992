/* TLP.C - "The Last Pascal Cross-referencer" - Pascal Main routine		*/
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
			Pascal Main routine";

KEYWORDS:	Utility, Cross-reference, C, Pascal, Apple, Macintosh, APW, Aztec;
SYSTEM:		Macintosh MPW, v3.0;
FILENAME:	TLP.C;
WARNINGS:	"Has not yet been ported to MS-DOS.
			Shareware, $20 Check/Money Order suggested.";

SEE-ALSO:	README.TLC,TLCHELP.DOC,TLPHELP.DOC;
AUTHORS:	Eduard Schwan;
COMPILERS:	AZTEC C65 v3.2b, APPLEIIGS APW C v1.0, APPLE MACINTOSH MPW C v3.0;
*/


/*-------------------------- include files ---------------------------*/

#include	<stdio.h>
#include	"tlc.h"


/*------------------------ external declarations ----------------------*/

extern	VOID
				handle_cmd_line(),
				handle_resw_file(),
				handle_parm_file(),
				process_files(),
				print_xref(),
				print_stats();
extern	int		close_text_file();
extern	FILE *	open_text_file();


/*-------------------------- static variables ------------------------*/


/*-------------------------- global variables ------------------------*/

char		ref_chars[MAX_REF_CHARS+1] = " fpu";

emph_rec_type		emph_array[EMPH_MAX]=
	{
	{NULL, NULL},
	{NULL, NULL},
	{NULL, NULL}
	};

file_rec_type		file_rec=
	{
	/*	num_in_fnames	*/		0,
	/*	file_list		*/		NULL,
	/*	curr_file		*/		NULL,
	/*	parm_fname		*/		"TLPParm.Dat",
	/*	resw_fname		*/		"TLPResw.Dat"
	};

parm_rec_type		parm_rec=
	{

/* GENERAL */
	/*	do_includes 	*/		FALSE,
	/*	do_listing		*/		FALSE,
	/*	do_xref 		*/		FALSE,
	/*	do_stats		*/		FALSE,
	/*	out_fname		*/		"TLP.Out",

/* PRINTER */
	/*	page_lines		*/		66,
	/*	page_columns	*/		80,
	/*	top_line		*/		1,
	/*	bot_line		*/		56,
	/*	left_column 	*/		1,
	/*	right_column	*/		80,
	/*	user_heading	*/		NULL,
	/*	do_single_shts	*/		FALSE,
	/*	printer_init	*/		NULL,
	/*	has_form_feed	*/		TRUE,
	/*	first_page		*/		1,
	/*	last_page		*/		9999,

/* LISTING */
	/*	tab_width		*/		4,
	/*	emph_comments	*/		EMPH_NONE,
	/*	emph_heading	*/		EMPH_NONE,
	/*	emph_reserved	*/		EMPH_NONE,
	/*	emph_funcheads	*/		EMPH_NONE,
	/*	paging_string	*/		NULL,

/* XREF */
	/*	symbol_size 	*/		MAX_SYM_SIZE,
	/*	xref_nums		*/		FALSE,
	/*	xref_strings	*/		FALSE,
	/*	do_conditionals */		FALSE,
	/*	do_case_sens	*/		FALSE,
	/*	do_xcase_sens	*/		FALSE,
	/*	do_underscores	*/		FALSE,
	/*	emph_symbols	*/		EMPH_NONE

/* STATS */

	};

resw_rec_type		resw_rec=
	{
	/*	num_reswords	*/		0,
	/*	resw_list		*/		NULL
	};

sym_table_type		symbol_table=
	{
	/*	symbol_list 	*/		NULL
	};

stat_rec_type		stat_rec=
	{
	/*	total_lines 	*/		0
	};

token_rec_type		token=
	{
	/*	tok_type		*/		-1, /* init to bad value */
	/*	tok_string		*/		"",
	/*	tok_value		*/		0L,
	/*	tok_column		*/		0
	};

line_type			curr_line		= "";
pos_int 			out_page_number = 0;
pos_int 			verbose 		= 0;
int 				enable_debugs	= FALSE;
FILE*				out_file;



/*===================[ main ]=====================*/

main(argc,argv)
int 	argc;
char	**argv;

	{ /* main() */
	int		error;
	char	curr_time_str[30];

	printf("\nThe Last Pascal Cross-referencer (TLP) version:%s\n",TLC_VERSION);
	printf("Copyright 1987-1989 by Eduard Schwan [esp]\n\n");
	fflush(stdout);

	get_time(curr_time_str);
	fprintf(stderr, "\nStarting at > %s\n",curr_time_str);
	fflush(stderr);

	handle_cmd_line(argc, argv);
	handle_resw_file();
	handle_parm_file();
	out_file = open_text_file(parm_rec.out_fname, "w+", &error);
	if (error)
		{
		fprintf(stderr,"Error #%d/$%x opening output file '%s'\n",
				error,error,parm_rec.out_fname);
		exit(BAD_EXIT);
		}
	else
		{
		if (parm_rec.printer_init != NULL)
			fputs(parm_rec.printer_init, out_file);
		process_files();
		if (parm_rec.do_xref)
			print_xref();
		if (parm_rec.do_stats)
			print_stats();
		error = close_text_file(out_file, parm_rec.out_fname);
		if (error)
			{
			fprintf(stderr,"Error #%d/$%x closing output file '%s'\n",
					error,error,parm_rec.out_fname);
			exit(BAD_EXIT);
			}

		get_time(curr_time_str);
		fprintf(stderr, "\nFinished at > %s\n",curr_time_str);
		fflush(stderr);

		}
	} /* main() */
