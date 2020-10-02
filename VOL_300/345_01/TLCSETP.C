/* TLCSETP.C - "The Last Cross-referencer" - Set Parm routine			*/
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
			Set Parm routine";

KEYWORDS:	Utility, Cross-reference, C, Pascal, Apple, Macintosh, APW, Aztec;
SYSTEM:		Macintosh MPW, v3.0;
FILENAME:	TLCSETP.C;
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

#include	"tlcparm.h"


/*--------------------- external declarations --------------------*/

#include	"tlc.ext"

extern VOID 	expand_str_chars();


/*------------------------ static variables -----------------------*/

struct	parmsym_type
	{
	char		*parm_name;
	byte		parm_index;
	};

static	struct parmsym_type parm_table[] =
{

/* GENERAL */

	{"doincludes",			DO_INCLUDES},
	{"dolisting",			DO_LISTING},
	{"doxref",				DO_XREF},
	{"dostats", 			DO_STATS},
	{"outfname",			OUT_FNAME},

/* PRINTER */

	{"pagelines",			PAGE_LINES},
	{"pagecolumns", 		PAGE_COLUMNS},
	{"topline", 			TOP_LINE},
	{"botline", 			BOT_LINE},
	{"leftcolumn",			LEFT_COLUMN},
	{"rightcolumn", 		RIGHT_COLUMN},
	{"userheading", 		USER_HEADING},
	{"dosingleshts",		DO_SINGLE_SHTS},
	{"printerinit", 		PRINTER_INIT},
	{"hasformfeed", 		HAS_FORM_FEED},
	{"firstpage",			FIRST_PAGE},
	{"lastpage",			LAST_PAGE},

	{"emph1leadin", 		EMPH1_LEADIN},	/* printer emphasis strings */
	{"emph2leadin", 		EMPH2_LEADIN},
	{"emph3leadin", 		EMPH3_LEADIN},
	{"emph1leadout",		EMPH1_LEADOUT},
	{"emph2leadout",		EMPH2_LEADOUT},
	{"emph3leadout",		EMPH3_LEADOUT},

/* LISTING */

	{"tabwidth",			TAB_WIDTH},
	{"emphcomments",		EMPH_COMMENTS},
	{"emphheading", 		EMPH_HEADING},
	{"emphreserved",		EMPH_RESERVED},
	{"emphfuncheads",		EMPH_FUNCHEADS},
	{"pagingstring",		PAGING_STRING},

/* XREF */

	{"symbolsize",			SYMBOL_SIZE},
	{"xrefnums",			XREF_NUMS},
	{"xrefstrings", 		XREF_STRINGS},
	{"doconditionals",		DO_CONDITIONALS},
	{"doconditionals",		DO_CONDITIONALS},
	{"docasesens",			DO_CASE_SENS},
	{"doxcasesens", 		DO_XCASE_SENS},
	{"dounderscores",		DO_UNDERSCORES},
	{"emphsymbols", 		EMPH_SYMBOLS},

/* STATS */

/* THE END */

	{NULL,					0}
};


/*=================[ find_parm ]===================*/

static byte find_parm(parm_name)
char	*parm_name;

	{ /* find_parm() */
	struct parmsym_type *ptable_ptr;

	ptable_ptr = parm_table;
	while (ptable_ptr->parm_name != NULL)
		{
/*
debug(printf("find_parm:targ:'%s' curr:'%s'\n",parm_name,ptable_ptr->parm_name);)
*/
		if (strcmp(parm_name, ptable_ptr->parm_name)==0)
			{ /* found a match, return corresponding index */
/*
debug(printf("find_parm:found '%s'[%u]\n",\
			ptable_ptr->parm_name,ptable_ptr->parm_index);)
*/
			return(ptable_ptr->parm_index);
			}
		else
			{ /* keep looking */
			ptable_ptr++;
			}
		} /*while*/
			return(0);
	} /* find_parm() */


/*=================[ bad_type ]===================*/

static VOID bad_type(line_num, parm_name, expected_type)
int 		line_num;
fname_type	parm_name;
int 		expected_type;

	{ /* bad_type() */
	fprintf(stderr, "Expected a");
	switch (expected_type)
		{
		case TOK_ID:
			fprintf(stderr, "n ID");
			break;
		case TOK_NCONST:
			fprintf(stderr, " Number");
			break;
		case TOK_SCONST:
			fprintf(stderr, " String");
			break;
		default:
			fprintf(stderr, " Type %d", expected_type);
		}
	fprintf(stderr, " for parm '%s'\n", parm_name);
	fprintf(stderr, "in line %d of Parm File '%s'\n",
			 line_num, file_rec.parm_fname);
	exit(1);
	} /* bad_type() */



/*=================[ bad_numrange ]===================*/

static VOID bad_numrange(line_num, parm_name, lo_bound, hi_bound)
int 			line_num;
char*			parm_name;
unsigned long	lo_bound;
unsigned long	hi_bound;

	{ /* bad_numrange() */
	fprintf(stderr, "Illegal value for parm '%s'\n", parm_name);
	fprintf(stderr, "in line %d of Parm File '%s'\n",
			line_num, file_rec.parm_fname);
	fprintf(stderr, "Valid range for parm is %lu to %lu\n",
			lo_bound, hi_bound);
	exit(1);
	} /* bad_numrange() */


/*=================[ bad_strlen ]===================*/

static VOID bad_strlen(line_num, parm_name, max_len)
int 		line_num;
char*		parm_name;
pos_int 	max_len;

	{ /* bad_strlen() */
	fprintf(stderr, "String too long for parm '%s'\n", parm_name);
	fprintf(stderr, "in line %d of Parm File '%s'\n",
			line_num, file_rec.parm_fname);
	fprintf(stderr, "Maximum length for parm is %u\n",
			max_len);
	exit(1);
	} /* bad_strlen() */


/*=================[ set_byteval ]===================*/

static VOID set_byteval(line_num, parm_name, parm_fld_ptr, lo_bound, hi_bound)
int 		line_num;
char*		parm_name;
byte*		parm_fld_ptr;
int 		lo_bound;
int 		hi_bound;

	{ /* set_byteval() */
/*
debug(printf("set_byte:'%s'=%ld [%d,%d]\n",parm_name,token.tok_value,lo_bound,hi_bound);)
*/
	if (token.tok_type == TOK_NCONST)
		{ /* right type, check range */
		if (token.tok_value>=lo_bound && token.tok_value<=hi_bound)
			*parm_fld_ptr = (byte)token.tok_value;
		else
			bad_numrange(line_num, parm_name, (long)lo_bound, (long)hi_bound);
		}
	else
		bad_type(line_num, parm_name, TOK_NCONST);
	} /* set_byteval() */


/*=================[ set_intval ]===================*/

static VOID set_intval(line_num, parm_name, parm_fld_ptr, lo_bound, hi_bound)
int 		line_num;
char*		parm_name;
pos_int*	parm_fld_ptr;
pos_int 	lo_bound;
pos_int 	hi_bound;

	{ /* set_intval() */
/*
debug(printf("set_int:'%s'=%ld [%d,%d]\n",parm_name,token.tok_value,lo_bound,hi_bound);)
*/
	if (token.tok_type == TOK_NCONST)
		{ /* right type, check range */
		if (token.tok_value >= lo_bound && token.tok_value <= hi_bound)
			*parm_fld_ptr = (pos_int)token.tok_value;
		else
			bad_numrange(line_num, parm_name, (long)lo_bound, (long)hi_bound);
		}
	else
		bad_type(line_num, parm_name, TOK_NCONST);
	} /* set_intval() */


/*=================[ set_strval ]===================*/

static VOID set_strval(line_num, parm_name, parm_fld_ptr, max_len)
int 		line_num;
char*		parm_name;
char**		parm_fld_ptr;
int 		max_len;

	{ /* set_strval() */
/*
debug(printf("set_str:'%s'='%s' max=%d\n",parm_name,token.tok_string,max_len);)
*/
	if (token.tok_type == TOK_SCONST)
		{ /* right type, check length */
		/* crunch backslash escape sequences into true chars first */
		expand_str_chars(token.tok_string);
		if (strlen(token.tok_string)<=max_len)
			{
			*parm_fld_ptr = malloc(strlen(token.tok_string)+1);
			strcpy(*parm_fld_ptr, token.tok_string);
			}
		else
			{
			bad_strlen(line_num, parm_name, max_len);
			}
		}
	else
		bad_type(line_num, parm_name, TOK_SCONST);
	} /* set_intval() */


/*=================[ set_parm ]===================*/

VOID set_parm(line_num, parm_name)
int 		line_num;
fname_type	parm_name;

	{ /* set_parm() */
	byte	parm_num;

	parm_num = find_parm(parm_name);

/*
debug(printf("set_parm #%d:line %d:'%s'='%s'\n",parm_num,line_num,parm_name,token.tok_string);)
*/
	switch (parm_num)
		{

/* GENERAL */

		case DO_INCLUDES:
			set_byteval(line_num, parm_name,
				&parm_rec.do_includes, FALSE, TRUE);
			break;

		case DO_LISTING:
			set_byteval(line_num, parm_name,
				&parm_rec.do_listing, FALSE, TRUE);
			break;

		case DO_XREF:
			set_byteval(line_num, parm_name,
				&parm_rec.do_xref, FALSE, TRUE);
			break;

		case DO_STATS:
			set_byteval(line_num, parm_name,
				&parm_rec.do_stats, FALSE, TRUE);
			break;

		case OUT_FNAME:
			set_strval(line_num, parm_name,
				&parm_rec.out_fname, 40);
			break;

/* PRINTER */

		case PAGE_LINES:
			set_intval(line_num, parm_name,
				&parm_rec.page_lines, 10, 255);
			break;

		case PAGE_COLUMNS:
			set_intval(line_num, parm_name,
				&parm_rec.page_columns, 10, 255);
			break;

		case TOP_LINE:
			set_intval(line_num, parm_name,
				&parm_rec.top_line, 1, 10);
			break;

		case BOT_LINE:
			set_intval(line_num, parm_name,
				&parm_rec.bot_line, 10, 255);
			break;

		case LEFT_COLUMN:
			set_intval(line_num, parm_name,
				&parm_rec.left_column, 1, 40);
			break;

		case RIGHT_COLUMN:
			set_intval(line_num, parm_name,
				&parm_rec.right_column, 10, 255);
			break;

		case USER_HEADING:
			set_strval(line_num, parm_name,
				&parm_rec.user_heading, 80);
			break;

		case DO_SINGLE_SHTS:
			set_byteval(line_num, parm_name,
				&parm_rec.do_single_shts, FALSE, TRUE);
			break;

		case PRINTER_INIT:
			set_strval(line_num, parm_name,
				&parm_rec.printer_init, 80);
			break;

		case HAS_FORM_FEED:
			set_byteval(line_num, parm_name,
				&parm_rec.has_form_feed, FALSE, TRUE);
			break;

		case FIRST_PAGE:
			set_intval(line_num, parm_name,
				&parm_rec.first_page, 1, parm_rec.last_page);
			break;

		case LAST_PAGE:
			set_intval(line_num, parm_name,
				&parm_rec.last_page, parm_rec.first_page, 9999);
			break;

/* printer emphasis strings */

		case EMPH1_LEADIN:
			set_strval(line_num, parm_name,
				&emph_array[EMPH_ONE-1].emph_leadin, 9);
			break;

		case EMPH2_LEADIN:
			set_strval(line_num, parm_name,
				&emph_array[EMPH_TWO-1].emph_leadin, 9);
			break;

		case EMPH3_LEADIN:
			set_strval(line_num, parm_name,
				&emph_array[EMPH_THREE-1].emph_leadin, 9);
			break;

		case EMPH1_LEADOUT:
			set_strval(line_num, parm_name,
				&emph_array[EMPH_ONE-1].emph_leadout, 9);
			break;

		case EMPH2_LEADOUT:
			set_strval(line_num, parm_name,
				&emph_array[EMPH_TWO-1].emph_leadout, 9);
			break;

		case EMPH3_LEADOUT:
			set_strval(line_num, parm_name,
				&emph_array[EMPH_THREE-1].emph_leadout, 9);
			break;

/* LISTING */

		case TAB_WIDTH:
			set_intval(line_num, parm_name,
				&parm_rec.tab_width, 0, 16);
			break;

		case EMPH_COMMENTS:
			set_byteval(line_num, parm_name,
				&parm_rec.emph_comments, EMPH_NONE, EMPH_MAX);
			break;

		case EMPH_HEADING:
			set_byteval(line_num, parm_name,
				&parm_rec.emph_heading, EMPH_NONE, EMPH_MAX);
			break;

		case EMPH_RESERVED:
			set_byteval(line_num, parm_name,
				&parm_rec.emph_reserved, EMPH_NONE, EMPH_MAX);
			break;

		case EMPH_FUNCHEADS:
			set_byteval(line_num, parm_name,
				&parm_rec.emph_funcheads, EMPH_NONE, EMPH_MAX);
			break;

		case PAGING_STRING:
			set_strval(line_num, parm_name,
				&parm_rec.paging_string, 15);
			break;

/* XREF */

		case SYMBOL_SIZE:
			set_intval(line_num, parm_name,
				&parm_rec.symbol_size, 1, MAX_SYM_SIZE);
			break;

		case XREF_NUMS:
			set_byteval(line_num, parm_name,
				&parm_rec.xref_nums, FALSE, TRUE);
			break;

		case XREF_STRINGS:
			set_byteval(line_num, parm_name,
				&parm_rec.xref_strings, FALSE, TRUE);
			break;

		case DO_CONDITIONALS:
			set_byteval(line_num, parm_name,
				&parm_rec.do_conditionals, FALSE, TRUE);
			break;

		case DO_CASE_SENS:
			set_byteval(line_num, parm_name,
				&parm_rec.do_case_sens, FALSE, TRUE);
			break;

		case DO_XCASE_SENS:
			set_byteval(line_num, parm_name,
				&parm_rec.do_xcase_sens, FALSE, TRUE);
			break;

		case DO_UNDERSCORES:
			set_byteval(line_num, parm_name,
				&parm_rec.do_underscores, FALSE, TRUE);
			break;

		case EMPH_SYMBOLS:
			set_byteval(line_num, parm_name,
				&parm_rec.emph_symbols, EMPH_NONE, EMPH_MAX);
			break;

/* STATS */


/* NONE OF THE ABOVE */

		default: /* unknown parameter */
			fprintf(stderr,
	"WARNING: unknown parm '%s' ignored, line %d, ParmFile '%s'\n",
			parm_name, line_num, file_rec.parm_fname);
			break;

		} /*switch*/

	} /* set_parm() */
