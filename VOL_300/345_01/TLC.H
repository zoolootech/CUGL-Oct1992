/* TLC.H - "The Last Cross-referencer" - Definition include file		*/
/*	Last Modified:	07/07/89											*/

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
			Definition include file";

KEYWORDS:	Utility, Cross-reference, C, Pascal, Apple, Macintosh, APW, Aztec;
SYSTEM:		Macintosh MPW, v3.0;
FILENAME:	TLC.H;
WARNINGS:	"Has not yet been ported to MS-DOS.
			Shareware, $20 Check/Money Order suggested.";

SEE-ALSO:	README.TLC,TLCHELP.DOC,TLPHELP.DOC;
AUTHORS:	Eduard Schwan;
COMPILERS:	AZTEC C65 v3.2b, APPLEIIGS APW C v1.0, APPLE MACINTOSH MPW C v3.0;
*/


/*--- defining "do_dbg" at compile time activates all debug lines ---*/
/*--- then the '-d' option forces display of them while TLC runs  ---*/

/*
#define do_dbg */		/* comment this out for no debugs.. */

#ifdef do_dbg
#define debug(x)	{if(enable_debugs){x;getchar();}}
#else
#define debug(x)
#endif


/*-------------- Compiler-dependent Definitions ---------*/

#ifdef AppleIIgs
/* APW C */
#define FILLCHAR(farea,fsize,fvalue)  memset(farea,fvalue,fsize)
#define MOVEBYTE(msource,mdest,msize) memcpy(mdest,msource,msize)
#else
#ifdef macintosh
/* MPW C */
#define FILLCHAR(farea,fsize,fvalue)  memset(farea,fvalue,fsize)
#define MOVEBYTE(msource,mdest,msize) memcpy(mdest,msource,msize)
#else
/* Aztec C */
#define FILLCHAR(farea,fsize,fvalue)  setmem(farea,fsize,fvalue)
#define MOVEBYTE(msource,mdest,msize) movmem(msource,mdest,msize)
#endif
#endif

/* comment this out if you cannot assign structures */
#define STRUCT_ASSIGN	1

#define VOID	void	/* define VOID to int for non-void compilers */


/*---------------------- definitions --------------------*/

#define 	TLC_VERSION 	"1.01 07/02/89"
#define 	LINE_SIZE		255
#define 	FNAME_SIZE		64
#define 	DATE_SIZE		30
#define 	MAX_SYM_SIZE	32	/* maximum symbol (id) size */

#define 	TRUE			1	/* boolean types are set to these */
#define 	FALSE			0

#define 	BAD_EXIT		99
#define 	GOOD_EXIT		0

#define 	CH_FF			'\014'	/* printer form-feed character */
#define 	CH_COMMENT		'*' 	/* comment char for file list file */
#define 	CH_SPACE		' ' 	/* space character! */

#define 	TOK_ID			1
#define 	TOK_NCONST		2
#define 	TOK_CCONST		3
#define 	TOK_SCONST		4

#define 	TOK_EQUALS		'='
#define 	TOK_LBRACE		'{'
#define 	TOK_RBRACE		'}'
#define 	TOK_LPAREN		'('
#define 	TOK_RPAREN		')'

#define 	TERR_BAD_CHR	-1000 /* get_token() error values */
#define 	TERR_BAD_STR	-1001
#define 	TERR_EOLN		-1010

#define 	REF_BLANK		0	/* reference tags on xref line numbers */
#define 	REF_FUNCHEAD	1
#define 	REF_DEFINE		2
#define 	REF_PROCHEAD	2
#define 	REF_UNITHEAD	3
#define		MAX_REF_CHARS	4

#define 	EMPH_NONE		0	/* Emphasis style values */
#define 	EMPH_ONE		1
#define 	EMPH_TWO		2
#define 	EMPH_THREE		3
#define 	EMPH_MAX		3	/* should be total # of emph values above */

#define 	B_octal 		 8	/* Number bases used */
#define 	B_decimal		10
#define 	B_hex			16

#define		memdispFmt		"[%6lu] "	/* verbose FreeMem display */


/*-------- globally external variables for everyone ---------*/



/*---------------- structure definitions -----------------*/

typedef unsigned char			boolean;
typedef unsigned char			byte;
typedef unsigned short int		pos_int;

typedef char					date_str_type[DATE_SIZE];
typedef char					fname_type[FNAME_SIZE];
typedef char					line_type[LINE_SIZE];


/*------------------ emphasize type record --------------*/
struct	emph_r_type
	{
	char*						emph_leadin;
	char*						emph_leadout;
	};
typedef struct emph_r_type 		emph_rec_type;


/*-------------------- file list record -----------------*/
struct	file_l_type
	{
	pos_int 					file_num;
	pos_int						num_lines;
	char*						file_name;
	struct file_l_type*			next;
	};
typedef struct file_l_type 		file_list_type;


struct	file_r_type
	{
	pos_int 					num_in_fnames;
	file_list_type* 			file_list;
	file_list_type* 			curr_file;
	fname_type					parm_fname;
	fname_type					resw_fname;
	};
typedef struct file_r_type 		file_rec_type;


/*-------------------- parameter record -----------------*/
struct	parm_r_type
	{
			/* GENERAL */
	boolean 					do_includes;
	boolean 					do_listing;
	boolean 					do_xref;
	boolean 					do_stats;
	char*						out_fname;

			/* PRINTER */
	pos_int 					page_lines;
	pos_int 					page_columns;
	pos_int 					top_line;
	pos_int 					bot_line;
	pos_int 					left_column;
	pos_int 					right_column;
	char*						user_heading;
	boolean 					do_single_shts;
	char*						printer_init;
	boolean 					has_form_feed;
	pos_int 					first_page;
	pos_int 					last_page;

			/* LISTING */
	pos_int 					tab_width;
	byte						emph_comments;
	byte						emph_heading;
	byte						emph_reserved;
	byte						emph_funcheads;
	char*						paging_string;

			/* XREF */
	pos_int 					symbol_size;
	boolean 					xref_nums;
	boolean 					xref_strings;
	boolean 					do_conditionals;
	boolean 					do_case_sens;
	boolean 					do_xcase_sens;
	boolean 					do_underscores;
	byte						emph_symbols;

			/* STATS */
	};
typedef struct parm_r_type 		parm_rec_type;


/*------------------- reserved word record --------------*/
struct	resw_l_type
	{
	struct resw_l_type*			left;
	struct resw_l_type*			right;
	char*						resword;
	byte						tok_type;
	unsigned long				occurrances;
	};
typedef struct resw_l_type 		resw_list_type;

struct	resw_r_type
	{
	pos_int 					num_reswords;
	resw_list_type* 			resw_list;
	};
typedef struct resw_r_type 	resw_rec_type;


/*------------------ line list record -------------------*/
struct	line_l_type
	{
	pos_int 					line_number;
	file_list_type* 			file_ptr;
	byte						reference_type;
	struct line_l_type*			next;
	};
typedef struct line_l_type 		line_list_type;


/*------------------ symbol table records ---------------*/
struct	sym_l_type
	{
	struct sym_l_type* 			left;
	struct sym_l_type* 			right;
	char*						sym_name;
	byte						sym_type;
	line_list_type* 			line_list;
	line_list_type* 			line_tail;
	};
typedef struct sym_l_type		sym_list_type;

struct	sym_t_type
	{
	sym_list_type*				symbol_list;
	};
typedef struct sym_t_type		sym_table_type;


/*---------------------- status record ------------------*/
struct	stat_r_type
	{
	pos_int 					total_lines;
	};
typedef struct stat_r_type	 	stat_rec_type;


/*-------------------- token record ---------------------*/
struct	tok_r_type
	{
	pos_int 					tok_type;
	line_type					tok_string;
	long						tok_value;
	pos_int 					tok_column;
	};
typedef struct tok_r_type		token_rec_type;
