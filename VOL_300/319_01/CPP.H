/*
	CPP V5 -- Main header file.

	Source:  cpp.h
	Started: September 26, 1987
	Version: May 31, 1988; July 21, 1988

	Written by Edward K. Ream.
	This software is in the public domain.

	See the read.me file for disclaimer and other information.
*/

/*
	Define which compiler will be used to compile the program.
	Typically, a command line option will be used to define these 
	constants.

	TURBOC		use Turbo C compiler.
	MICRO_SOFT	use version 4.0 of MicroSoft compiler on MS DOS.
*/

/*
	Compile-time options:

	DEBUG		Make run-time checks.
	STD_DIR		Name of first standard directory.
*/

#define DEBUG 1

/*
	Define possible values for ch.
*/
#define END_FILE 0x1a

/*
	Define flags which will be found in macro replacement texts.
	These flags MUST NOT BE NEGATIVE, so as to fit in a proper char.
*/
#define ARG_FLAG 1
#define POUND_FLAG 2
#define CONCAT_FLAG 3
#define EORT 4
#define EXPAND_OFF 5
#define ARG_OFFSET '0'

/*
	Miscellaneous global constants.
*/
#define ZERO	(0)
#define TRUE	(1)
#define FALSE	(0)
#define ERROR	(-1)
#define BAD_EXIT 1

#define INT_DIGITS 20		/* used by utl.c and other routines */
#define LONG_DIGITS 40		/* used by utl.c and other routines */
typedef int	bool;

#ifdef MICRO_SOFT
#define FAR far
#else
#define FAR
#endif

/*
	Include subsidiary header files.
	SL.H MUST be included even if SHERLOCK.C is not linked in.
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <process.h>

#include <sl.h>

#include "enum.h"
#include "glb.h"
#include "tmp.h"

/*
	----------    PREPROCESSOR    ----------
*/
#define ishex(c) (isdigit(c) || (c >= 'A' && (c <= 'F' || (c>='a' && c<='f')))
#define isid1(c) (isalpha(c) || c == '_')
#define isid2(c) (isalpha(c) || isdigit(c) || c == '_')

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#define TRACETOK(name) TRACEPB(name, printf("(%s)\n", pr_tok()))
#define TRACECH(name)  TRACEPB(name, printf("(ch: %s)\n", pr_ch(ch)))
#define is(tok) (token == tok)
