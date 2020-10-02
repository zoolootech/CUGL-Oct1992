/*
	CPP V5 -- global variable definitions.

	Source:  glb.c
	Started: April 2, 1986
	Version: January 20, 1988; May 21, 1988

	Written by Edward K. Ream.
	This software is in the public domain.

	See the read.me file for disclaimer and other information.
*/
#define no_extern 1
#include "cpp.h"

/*
	========= increase stack size ========
*/
#ifdef TURBOC
extern unsigned _stklen = 0xf000;
#endif

/*
	========= global variables =========
*/

/*
	The variable ch should be allocated a register if possible.
*/
char 	ch = '@';		/* The NEXT character.			*/

/*
	User options.
*/
bool	nest_flag = FALSE;	/* TRUE = allow nested comments.	*/
bool	com_flag = FALSE;	/* TRUE = leave comments in output.	*/
bool	slc_flag = TRUE;	/* TRUE = allow single-line comments.	*/

/*
	----- PREPROCESSOR AND TOKENS -----
*/
bool	m_flag	= FALSE;	/* TRUE if expanding a macro.	*/
char	macro_buf[MAX_RTEXT];	/* Final macro buffer.		*/
char *	p_rescan;		/* Pointer into rescan_buf[].	*/

long	t_errcount = 0;		/* Number of errors seen.		*/

/*
	The code assumes that no token or string will ever be longer than
	MAX_SYMBOL.  If that ever is not so the program may crash.  Thus,
	MAX_SYMBOL should be very large -- say 1000 or more.
*/
en_tokens
	token = NULL_TOK;	/* The token itself or it's class.	*/
char	t_symbol [MAX_SYMBOL];	/* The spelling of the token.		*/
int	t_length;		/* The length of the token (in the text)*/
long	t_value;		/* Value of integer constants.		*/
int	t_subtype;		/* The subtype of token class.		*/

/*
	The following globals are set ONLY by the system module.
	They are picked up and used by the preprocessor and the
	parser.
*/
char *	t_file    = NULL;	/* Name of current input file.	*/
int	t_line    = 0;		/* Line number within file.	*/
int	t_inlevel = -1;		/* Current input file level.	*/

/*
	Globals for use internally to the token routines.
*/
bool	t_ifstack [MAX_IF];	/* Stack for nexted #if's	*/
int	t_iflevel  = 0;		/* Nesting depth of #if's	*/

/*
	Defines for the path table.
	This table is used to search for #include files.
*/
char *	paths [MAX_PATHS];	/* Pointers to path names.	*/
int	n_paths = 0;		/* Number of paths defined.	*/
