/*	DSTRUCT:	Structure and preprocesser defined for MicroSPELL 1.0
			Spell Checker and Corrector

			(C)opyright May 1987 by Daniel Lawrence
			All Rights Reserved
*/

#if	V7 | USG | BSD
#define	PATHCHR	':'
#else
#define	PATHCHR	';'
#endif

#define	INTWIDTH	sizeof(int) * 3

/* DIFCASE represents the integer difference between upper
   and lower case letters.  It is an xor-able value, which is
   fortunate, since the relative positions of upper to lower
   case letters is the opposite of ascii in ebcdic.
*/

#ifdef	islower
#undef	islower
#endif

#ifdef	isupper
#undef	isupper
#endif

#if	ASCII

#define	DIFCASE		0x20
#define SEPCHAR         '^'
#define isletter(c)	(('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))
#define islower(c)	('a' <= c && c <= 'z')
#define isupper(c)	('A' <= c && c <= 'Z')
#endif

#if	EBCDIC

#define	DIFCASE		0x40
#define SEPCHAR         '{'
#define isletter(c)	(('a' <= c && 'i' >= c) || ('j' <= c && 'r' >= c) || ('s' <= c && 'z' >= c) || ('A' <= c && 'I' >= c) || ('J' <= c && 'R' >= c) || ('S' <= c && 'Z' >= c))
#define islower(c)	(('a' <= c && 'i' >= c) || ('j' <= c && 'r' >= c) || ('s' <= c && 'z' >= c))
#define isupper(c)	(('A' <= c && 'I' >= c) || ('J' <= c && 'R' >= c) || ('S' <= c && 'Z' >= c))
#endif

/*	Dynamic RAM tracking and reporting redefinitions	*/

#if	RAMSIZE
#define	malloc	allocate
#define	free	release
#endif

/*
	The input document is read in and broken up into words. The
	place each word came from in the origional document is stored
	along with the word.  The following structure is used to store
	these words.
*/

typedef	struct	WORD {
	int w_file;		/* source file of word */
	int w_line;		/* line of word */
	int w_col;		/* column of word */
	char w_text[1];		/* text of current word */
} WORD;
