/*
**	name:		matches.c
**	purpose:	Words that almost match pattern searching
**	source:		Computer Language - November 1986
**			by Dave Taylor
**	coded:		1988.11.07 - Roberto Artigas Jr
**	NOTES:		This set of routines comes from a number of
**			sources, all I did is integrate them into a
**			subroutine that uses all the methods for
**			text matches.
*/
#define	DEBUG	0	/* 0 = NO debug, 1 = YES debug */
#define	TEST	0	/* 0 = library, 1 = mainline test */

#include	<stdio.h>
#include	<stdlib.h>
#include	<ctype.h>
#include	<string.h>

#ifdef	tolower
#undef	tolower
#endif
#define	tolower(c)	(isupper(c) ? c - 'A' + 'a' : c)
#ifdef	min
#undef	min
#endif
#define	min(a,b)	((a) < (b) ? (a) : (b))

/*
**	Defined constants
*/
#define	WORD_LENGTH	40
#define	THRESHOLD	0.500	/* PF474 */
#define	THRESHOLD2	0.750	/* GESTALT */
#define	SOUNDEX_LENGTH	4	/* soundex */
#ifndef	TRUE
#define	TRUE		1
#define	FALSE		0
#endif
/*
**	For the soundex algorithm: 'x' means 'delete this letter'
*/
#define	index_of(ch)	(ch - 'A')
#define	IGNORE		'x'
static	char soundex_mappings[] = { "x123x12xx22455x12623x1x2x2" };
/*
**	Routine prototypes
*/
char	*remove_duplicate_letters();
char	*translate_case();
char	*soundex_translation_of();
char	*reverse();
double	pf474_algorithm();


/*
**	name:		reverse
**	purpose:	Reverse the word and return it
*/
char	*reverse(word)
char	*word;
{
	static char	buffer[WORD_LENGTH];
	register int	i, j = 0;
	for (i=strlen(word)-1; i > -1; i--)
		buffer[j++] = word[i];
	buffer[j] = '\0';
	return((char *)buffer);
}


/*
**	name:		translate_case
**	purpose:	Destructively translate the word to all lowercase
**			returning both the altered word and a pointer to
**			the altered word
*/
char	*translate_case(word)
char	*word;
{
	register int	index;
	for (index = 0; index < strlen(word); index++)
		word[index] = tolower(word[index]);
	return((char *)word);
}

/*
**	name:		remove_duplicate_letters
**	purpose:	This routine removes all multiply occurring
**			letters in the given word and returns a
**			'sanitized' version of the word. For example,
**			given 'missionary' it returns 'misionary'.
*/
char	*remove_duplicate_letters(word)
char	*word;
{
	register int	index, bindex = 0;
	static char	buffer[WORD_LENGTH] = { "" };
	buffer[bindex++] = word[0];
	for (index = 1; index < strlen(word); index++)
		if (word[index-1] != word[index])
			buffer[bindex++] = word[index];
	buffer[bindex] = 0;
	return((char *)buffer);
}


/*
**	name:		check_transpositions
**	purpose:	Transposes character pairs in word to see if
**			any match the given patter. Returns TRUE if
**			so, FALSE if not
*/
int	check_transpositions(word,pattern)
char	*word;
char	*pattern;
{
	register int	index;
	char		spare_character;
	for (index=1; index < strlen(word); index++) {
		spare_character = word[index];
		word[index] = word[index-1];
		word[index-1] = spare_character;
		if (strcmp(word,pattern) == 0)
			return(TRUE);
		word[index-1] = word[index];
		word[index] = spare_character;
	}
	return(FALSE);
}

/*
**	name:		allow_a_missing_character
**	purpose:	This routine returns TRUE if word is the
**			same as pattern, just missing a single letter.
**	1988.10.07	Roberto Artigas Jr - If word and pattern
**			is a length of one and not an exact match exit
**			saying that it is false
*/
int	allow_a_missing_character(word, pattern)
char	*word;
char	*pattern;
{
	register int	index, windex = 0;
	if ((strlen(word)==1) && (strlen(pattern)==1) && (*word != *pattern))
		return(FALSE);
	if (strlen(word) + 1 != strlen(pattern))
		return(FALSE);
	for (index=0, windex=0; index < strlen(pattern); index++)
		if (word[windex] == pattern[index])
			windex++;
		else if (windex < index)
			return(FALSE);
	if (word[windex] == pattern[index])
	 	return(TRUE);
	else
		return(FALSE);
}

/*
**	name:		allow_an_incorrect_character
**	purpose:	This routine returns TRUE if word is the same
**			as pattern, just with a single incorrect letter.
**	1988.10.07	Roberto Artigas Jr - If word and pattern
**			is a length of one and not an exact match exit
**			saying that it is false
*/
int	allow_an_incorrect_character(word,pattern)
char	*word;
char	*pattern;
{
	register int	index, windex;
	register int	already_seen_bad_letter = FALSE;
	if (strlen(word) != strlen(pattern))
		return(FALSE);
	if ((strlen(word)==1) && (strlen(pattern)==1) && (*word != *pattern))
		return(FALSE);
	for (index=0, windex=0; index < strlen(pattern); index++) {
		if (word[windex] == pattern[index])
			windex++;
		else if (already_seen_bad_letter)
			return(FALSE);
		else {
			already_seen_bad_letter++;
			windex++;
		}
	}
	return(TRUE);
}

/*
**	name:		soundex_translation_of
**	purpose:	Implement soundex algorithm tranlation
*/
char	*soundex_translation_of(buffer)
char	*buffer;
{
	static char	result[WORD_LENGTH];
	register int	i, j, ch, newch;
	result[0] = toupper(buffer[0]);
	result[1] = result[2] = result[3] = '0';
	result[4] = '\0';
	for (i=1, j=1; i < strlen(buffer) && j < SOUNDEX_LENGTH; i++) {
		ch = toupper(buffer[i]);
		if ((newch = soundex_mappings[index_of(ch)]) != IGNORE)
			if (result[j-1] != (char)newch)
				if (!(j == 1 && ch == result[0]))
					result[j++] = (char)newch;
	}
	return((char *)result);
}

/*
**	name:		soundex
**	puspose:	Implements matching based on the soundex
**			algorithm - this simple part gets the two
**			words 'Soundex'ized then returns a nonzero
**			if the two match on this technique.
*/
int	soundex(word,pattern)
char	*word;
char	*pattern;
{
	char		word2[WORD_LENGTH];
	char		pattern2[WORD_LENGTH];
	strcpy(word2,soundex_translation_of(word));
	strcpy(pattern2,soundex_translation_of(pattern2));
	return((strcmp(word2,pattern2)==0));
}

/*
**	name:		proximity_value
**	purpose:	This actually implements the algorithm used
**			in the PF474 pattern matching chip from
**			Proximity technologies, returning the number
**			of matches encountered.
*/
int	proximity_value(word,pattern)
char	*word;
char	*pattern;
{
	int	matches = 0, count = 1;
	int	iteration, index, len;
	if (strcmp(word,pattern) == 0) {
		for (count=0; count <= strlen(word); count++)
			matches += (2*count);
		return(matches);
	}
 	len = min(strlen(word),strlen(pattern)) + 1;
	count =1;
	do {
	for (iteration=1; iteration<len; iteration++)
		for (index=0; index<iteration; index++)
			if (tolower(word[index]) == tolower(pattern[index]))
				matches++;
		strcpy(word,reverse(word));
		strcpy(pattern,reverse(pattern));
	} while (count++ < 2);
	return(matches);
}

/*
**	name:		pf474_algorithm
**	purpose:	This is the top-level PF474 pattern matching
**			routines. It implements the equation we are
**			using
*/
double	pf474_algorithm(word,pattern)
char	*word;
char	*pattern;
{
	if (strlen(word) == 0 && strlen(pattern) == 0)
		return((double)1.0);
	return((double) (proximity_value(word,pattern) /
		(proximity_value(word,word)+proximity_value(pattern,pattern))));
}

/*
**	name:		GCsubstr
**	purpose:	Recursive greatest common sub-string
*/
int	GCsubstr(st1, end1, st2, end2)
char	*st1;
char	*end1;
char	*st2;
char	*end2;
{
	register char	*a1, *b1, *s1, *a2, *b2, *s2;
	short		max, i;

	if (end1 <= st1)		/* s1 empty */
	  return (0);
	if (end2 <= st2)		/* s2 empty */
	  return (0);
	if (end1 == st1 + 1)		/* s1 has one char */
	  if (end2 == st2 + 1)		/* and s2 has one char */
	    return (0);			/* They cannot be equal */

	max = 0;
	b1 = end1; b2 = end2;

	for (a1 = st1; a1 < b1; a1++)
	   for (a2 = st2; a2 < b2; a2++)
	      if (*a1 == *a2)
	        {			/* How long common sub-string? */
		for (i = 1; a1[i] && (a1[i] == a2[i]); i++)
		   ;
		if (i > max)
		  {
		  max = i; s1 = a1; s2 = a2;
		  b1 = end1 - max; b2 = end2 - max;
		  }
		}
	if (! max)
	  return (0);

	max += GCsubstr(s1 + max, end1, s2 + max, end2);	/* RHS */
	max += GCsubstr(st1, s1, st2, s2);			/* LHS */

	return(max);
}

/*
**	name:		simil
**	purpose:	Best guess matches
**	author:		Ratcliff/Obershelp pattern matching
**	source:		Pattern matching by Gestalt
**			by John W. Ratcliff
**			Dr. Dobb's Journal #141 - July 1988
*/
double	simil(s1,s2)
char	*s1;
char	*s2;
{
	short	l1;
	short	l2;

	l1 = strlen(s1);
	l2 = strlen(s2);

	if ( l1 == 1)			/* Check for easiest case */
	  if (l2 == 1)
	    if (*s1 == *s2)
	       return(100.0);

	return(2.0 * GCsubstr(s1, s1 + l1, s2, s2 + l2) / (l1 + l2));
}

/*
**	name:		matches
**	purpose:	Apply all pattern matching techniques to get
**			a list of possible words to substitute
*/
int	matches(word,pattern)
char	*word;
char	*pattern;
{
	static	double	pf474_value;
	static	double	percnt;
	static	int	pword;		/* Difference after drop duplicates */
	static	int	ppatt;		/* Difference after drop duplicates */
	static	int	i;		/* For loops */

	if (strcmp(word,pattern) == 0) {
#if	TEST 
		printf("MATCHED by comparison (EXACT)\n");
#endif
		return(1);
	}
	translate_case(word);
	translate_case(pattern);
	if (strcmp(word,pattern) == 0) {
#if	TEST 
		printf("MATCHED by translation of case\n");
#endif
		return(2);
	}
/*
**	We remove all duplicate characters and count how many we have
**	removed.  This will allow some measure of control with eccessive
**	matches
*/
	pword = strlen(word);
	ppatt = strlen(pattern);
	strcpy(word,remove_duplicate_letters(word));
	strcpy(pattern,remove_duplicate_letters(pattern));
	pword -=strlen(word);
	ppatt -=strlen(pattern);
	if (strcmp(word,pattern)==0) {
#if	TEST 
		printf("MATCHED by removal of duplicate letters\n");
#endif
		return(3);
	}
	if (check_transpositions(word,pattern)) {
#if	TEST 
		printf("MATCHED by transposition of letters\n");
#endif
		return(4);
	}
	if (allow_a_missing_character(word,pattern)) {
		if (!(abs((pword-ppatt)+(strlen(word)-strlen(pattern)))>1)) {
#if	TEST 
			printf("MATCHED by allowance of a missing character\n");
#endif
			return(5);
		}
	}
	if (allow_an_incorrect_character(word,pattern)) {
#if	TEST 
		printf("MATCHED by allowing an incorrect character\n");
#endif
		return(6);
	}
	if (soundex(word,pattern)) {
#if	TEST 
		printf("MATCHED by using the Soundex algorithm\n");
#endif
		return(7);
	}
	if ((pf474_value = pf474_algorithm(word,pattern)) > THRESHOLD) {
#if	TEST 
		printf("MATCHED (%g) by using PF474 algorithm\n",
			pf474_value);
#endif
		return(8);
	}
/*
**	Matching by gestalt does not seem to be a good way to match in
**	a spell checker type of application. All sort of weird things
**	match that have no relation to the word we are trying to spell
**	However - this seems to be a good method for badly misspelled
**	words. Perhaps a matching by GESTALT with some sort of inteligent
**	weeding out would be a good solution. Also it might be possible
**	that for a minimum length with a high enough percentage threshhold
**	might work for the longer words, BUT what about the shorter ones.
*/
	if ((percnt = simil(word,pattern)) > THRESHOLD2) {
#if	TEST
		printf("MATCHED (%g) by using GESTALT algorithm\n",
			percnt);
#endif
		return(9);
	}
	return(0);	/* No MATCH found */
}


#if	TEST
/*
**	name:		main
**	purpose:	Test algorithms that match
*/
int	main(argc,argv)
int	argc;
char	*argv[];
{
	char	save_word[WORD_LENGTH];
	char	save_pattern[WORD_LENGTH];
	char	word[WORD_LENGTH];
	char	pattern[WORD_LENGTH];
	int	result;

	printf("Enter the pattern to match against: ");
	fgets(save_pattern,WORD_LENGTH,stdin);
	while (1) {
		printf("\n\nPattern is : %s",save_pattern,pattern);
		printf("Enter word or \"quit\" to quit : ");
		fgets(save_word,WORD_LENGTH,stdin);
		if (strcmp(save_word,"quit\n")==0) break;
		if (strcmp(save_word,"QUIT\n")==0) break;
		strcpy(word,save_word);
		strcpy(pattern,save_pattern);
		result = matches(word,pattern);
		if (result) {
			printf("Matched by algorithm %d\n",result);
			printf("Original pattern : %s",save_pattern);
			printf("Original word    : %s",save_word);
			printf("   Final pattern : %s",pattern);
			printf("   Final word    : %s",word);
		}
		else printf("Couln't match word to pattern\n");
	}
	return(0);
}
#endif
