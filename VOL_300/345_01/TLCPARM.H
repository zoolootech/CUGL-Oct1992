/* TLCPARM.H - included by tlcsetp.c - Parameter consts/values			*/
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
			Included by tlcsetp.c - Parameter consts/values";

KEYWORDS:	Utility, Cross-reference, C, Pascal, Apple, Macintosh, APW, Aztec;
SYSTEM:		Macintosh MPW, v3.0;
FILENAME:	TLCPARM.H;
WARNINGS:	"Has not yet been ported to MS-DOS.
			Shareware, $20 Check/Money Order suggested.";

SEE-ALSO:	README.TLC,TLCHELP.DOC,TLPHELP.DOC;
AUTHORS:	Eduard Schwan;
COMPILERS:	AZTEC C65 v3.2b, APPLEIIGS APW C v1.0, APPLE MACINTOSH MPW C v3.0;
*/

/*
symbolic lookup values for parms. these must be unique, and must be
between 1 and 255 (saved in parm_table[] array in tlc.setp.c).
*/

	/* GENERAL */

#define 	DO_INCLUDES 		1
#define 	DO_LISTING			2
#define 	DO_XREF 			3
#define 	DO_STATS			4
#define 	OUT_FNAME			5

	/* PRINTER */

#define 	PAGE_LINES			20
#define 	PAGE_COLUMNS		21
#define 	TOP_LINE			22
#define 	BOT_LINE			23
#define 	LEFT_COLUMN 		24
#define 	RIGHT_COLUMN		25
#define 	USER_HEADING		26
#define 	DO_SINGLE_SHTS		27
#define 	PRINTER_INIT		28
#define 	HAS_FORM_FEED		29
#define 	FIRST_PAGE			30
#define 	LAST_PAGE			31

#define 	EMPH1_LEADIN		50
#define 	EMPH2_LEADIN		51
#define 	EMPH3_LEADIN		52
#define 	EMPH1_LEADOUT		53
#define 	EMPH2_LEADOUT		54
#define 	EMPH3_LEADOUT		55

	/* LISTING */

#define 	TAB_WIDTH			100
#define 	EMPH_COMMENTS		101
#define 	EMPH_HEADING		102
#define 	EMPH_RESERVED		103
#define 	EMPH_FUNCHEADS		104
#define 	PAGING_STRING		105

	/* XREF */

#define 	SYMBOL_SIZE 		200
#define 	XREF_NUMS			201
#define 	XREF_STRINGS		202
#define 	DO_CONDITIONALS 	203
#define 	DO_CASE_SENS		204
#define 	DO_XCASE_SENS		205
#define 	DO_UNDERSCORES		206
#define 	EMPH_SYMBOLS		207

	/* STATS */
