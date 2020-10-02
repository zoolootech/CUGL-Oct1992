/*
	CPP V5 -- global header for variables.

	Source:  glb.h
	Started: June 20, 1986
	Version: May 21, 1988; August 1, 1989

	Written by Edward K. Ream.
	This software is in the public domain.

	See the read.me file for disclaimer and other information.
*/

/*
	Constants describing limits.
*/
#define MAX_SYMBOL	1000	/* Max len of symbols/strings.		*/
#define MAX_FILE_NAME	50	/* Maximum length of file names.	*/
#define MAX_IF		50	/* Max nesting level of #if's.		*/
#define MAX_PATHS	20	/* Max number of -s args.		*/
#define MAX_RTEXT	1000	/* Max chars in replacement string.	*/
#define MAX_ATEXT	1000	/* Max total chars in actual args.	*/
#define MAX_NARGS	50	/* Max number of macro arguments.	*/
#define MAX_MDEPTH	100	/* Max depth of macro expansions.	*/
#define MAX_INCLUDE	MAX_MDEPTH+20

/*
	Exclude the rest if glb.c is the file including this file.
*/
#ifndef no_extern

/*
	============= global variables =============
*/

/*
	The next character of input.
*/
extern char	ch;		/* The NEXT character.			*/


/*
	User Flags.
*/
extern bool	nest_flag;	/* TRUE = allow nested comments.	*/
extern bool	com_flag;	/* TRUE = leave comments in output.	*/
extern bool	slc_flag;	/* TRUE = allow single-line comments.	*/

/*
	Globals for macro expansion.
	p_rescan and m_flag are used by sysnext().
*/
extern bool	m_flag;			/* TRUE if expanding a macro.	*/
extern char	macro_buf[MAX_RTEXT];	/* Final macro buffer.		*/
extern char *	p_rescan;		/* Pointer into rescan_buf[].	*/


/*
	Globals describing tokens.

	The code assumes that no token or string will ever be longer than
	MAX_SYMBOL.  If that ever is not so the program may crash.  Thus,
	MAX_SYMBOL should be very large -- say 1000 or more.
*/
extern en_tokens token;			/* The current token. */
extern int	t_subtype;		/* The subtype of token class.		*/
extern char	t_symbol [MAX_SYMBOL];	/* The spelling of the token.		*/
extern int	t_length;		/* The length of the token (in the text)*/
extern long	t_value;		/* Value of integer constants.		*/

/*
	The following globals are set ONLY by the system module and
	used by the preprocessor.
*/
extern char *	t_file;			/* Name of current input file.	*/
extern int	t_line;			/* Line number within file.	*/
extern int	t_inlevel;		/* Current input file level.	*/

/*
	Globals for use internally to the token routines.
*/
extern bool	t_ifstack [MAX_IF];	/* Stack for nexted #if's	*/
extern int	t_iflevel;		/* Nexting depth of #if's	*/
extern long	t_errcount;		/* Number of errors seen.	*/

/*
	Defines for the path table.
	This table is used to search for #include files.
*/
extern char *	paths [MAX_PATHS];	/* Pointers to path names.	*/
extern int	n_paths;		/* Number of paths defined.	*/

#endif /* no_extern */
