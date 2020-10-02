/*	QUICK:	Quicksort module for MicroSPELL 1.0
		Spell Checker and Corrector

		(C)opyright May 1987 by Daniel Lawrence
		All Rights Reserved
*/

#include	<stdio.h>
#include	"dopt.h"
#include	"dstruct.h"
#include	"ddef.h"

possort()	/* sort the current wordlist on position order */

{
	int cmppos();
	static WORD lastpos =		/* infinite key */
		{32767, 32767, 32767, 0xFF};

	/* set comparison routines to check positions */
	comp = &cmppos;

	/* make sure there is a "infinite" key at the end */
	sword[numwords] = &lastpos;

	/* and sort them */
	quick(0, numwords - 1);
}

wordsort()	/* sort the current wordlist on word order */

{
	int cmpnam();
	static WORD lastalpha =		/* infinite key */
		{0, 0, 0, 0xFF};
	/* this is the rest of the lastalpha key....make sure it is
	   right after last alpha */

	/* set comparison routines to check names */
	comp = &cmpnam;

	/* make sure there is a "infinite" key at the end */
	sword[numwords] = &lastalpha;

	/* and sort them */
	quick(0, numwords - 1);
}

comsort()	/* sort the common word list alphabetically */

{
	/* make sure there is a "infinite" key at the end */
	cword[numfiltr] = hivalue;

	/* and sort them */
	uquick(0, numfiltr - 1);
}

quick(lower, upper)	/* quicksort part of the word list */

int lower;	/* lowermost element to sort */
int upper;	/* uppermost element to sort */

{
	int testkey;	/* key used to compare items */
	int i, j;	/* quicksort partitioning pointers */
	WORD *t;	/* temp pointer into word list */

	/* if we have less than 9 items...bubble sort them */
	if ((upper - lower) < 9) {
		slow(lower, upper);
		return;
	}

	/* swap the middle item for the first...*/
	testkey = (upper - lower) / 2 + lower;
	t = sword[testkey];
	sword[testkey] = sword[lower];
	sword[lower] = t;	

	/* set up the pointers and prepare to partition */
	testkey = lower;
	i = lower;
	j = upper + 1;

	/* and partition the subfile */
	while (i < j) {

		/* scan to the right */
		i++;
		while ((*comp)(i, testkey) < 0)
			i++;

		/* scan to the left */
		j--;
		while ((*comp)(j, testkey) > 0)
			j--;

		/* switch em if appropriate */
		if (i < j) {
			t = sword[i];
			sword[i] = sword[j];
			sword[j] = t;
		}
	}
	/* and now swap the testkey between the two partitions */
	t = sword[j];
	sword[j] = sword[testkey];
	sword[testkey] = t;

	/* and sort the resulting partitions */
	quick(lower, j - 1);
	quick(j + 1, upper);
}

slow(lower, upper)	/* do a straight bubble sort on a small subfile */

int lower;	/* lower limit */
int upper;	/* upper limit */

{
	int i, j;	/* ptrs in sort */
	WORD *t;	/* temp value for an exchange */
	int swapflag;	/* was there a swap? */

	/* make sure this is needed */
	if (upper <= lower)
		return;

	/* sort the suckers */
	for (i = upper; i > lower; i--) {
		swapflag = FALSE;
		for (j = lower; j < i; j++)
			if ((*comp)(j, j+1) > 0) {
				swapflag = TRUE;
				t = sword[j+1];
				sword[j+1] = sword[j];
				sword[j] = t;
			}
		if (swapflag == FALSE)
			return;
	}
}

int cmpnam(n1, n2)	/* compare the text of two source words */

int n1, n2;		/* pointer to words to compare */

{
	return(strcmp(sword[n1]->w_text, sword[n2]->w_text));
}

int cmppos(n1, n2)	/* compare the position of two source words */

int n1, n2;		/* pointer to words to compare */

{
	WORD *p1, *p2;	/* ptr to the two words in question */

	/* grab the two word pointers */
	p1 = sword[n1];
	p2 = sword[n2];

	if (p1->w_file > p2->w_file)
		return(1);

	if (p1->w_file < p2->w_file)
		return(-1);

	if (p1->w_line > p2->w_line)
		return(1);

	if (p1->w_line < p2->w_line)
		return(-1);

	if (p1->w_col > p2->w_col)
		return(1);

	if (p1->w_col < p2->w_col)
		return(-1);

	return(0);
}

uquick(lower, upper)	/* quicksort part of the common word list */

int lower;	/* lowermost element to sort */
int upper;	/* uppermost element to sort */

{
	int testkey;	/* key used to compare items */
	int i, j;	/* quicksort partitioning pointers */
	char *t;	/* temp pointer into common word list */

	/* if we have less than 9 items...bubble sort them */
	if ((upper - lower) < 9) {
		uslow(lower, upper);
		return;
	}

	/* swap the middle item for the first...*/
	testkey = (upper - lower) / 2 + lower;
	t = cword[testkey];
	cword[testkey] = cword[lower];
	cword[lower] = t;	

	/* set up the pointers and prepare to partition */
	testkey = lower;
	i = lower;
	j = upper + 1;

	/* and partition the subfile */
	while (i < j) {

		/* scan to the right */
		i++;
		while (strcmp(cword[i], cword[testkey]) < 0)
			i++;

		/* scan to the left */
		j--;
		while (strcmp(cword[j], cword[testkey]) > 0)
			j--;

		/* switch em if appropriate */
		if (i < j) {
			t = cword[i];
			cword[i] = cword[j];
			cword[j] = t;
		}
	}
	/* and now swap the testkey between the two partitions */
	t = cword[j];
	cword[j] = cword[testkey];
	cword[testkey] = t;

	/* and sort the resulting partitions */
	uquick(lower, j - 1);
	uquick(j + 1, upper);
}

uslow(lower, upper)	/* do a straight bubble sort on a small subfile */

int lower;	/* lower limit */
int upper;	/* upper limit */

{
	int i, j;	/* ptrs in sort */
	char *t;	/* temp value for an exchange */
	int swapflag;	/* did a swap occur? */

	/* make sure this is needed */
	if (upper <= lower)
		return;

	/* sort the suckers */
	for (i = upper; i > lower; i--) {
		swapflag = FALSE;
		for (j = lower; j < i; j++)
			if (strcmp(cword[j], cword[j+1]) > 0) {
				swapflag = TRUE;
				t = cword[j+1];
				cword[j+1] = cword[j];
				cword[j] = t;
			}
		if (swapflag == FALSE)
			return;
	}
}

#if	EBCDIC
/*	if we are using the EBCDIC character set, spell, to operate
	properly in regards to uppercase words in the merge() phase
	MUST have uppercase sort higher then lower. To effect this,
	we slip in the replacement to the library strcmp() function.
*/

strcmp(s, t)

char *s, *t;	/* two strings to compare with reversed case */

{
	register char s1, t1;	/* current chars in s and t */
	register int cresult;	/* result of current copmparison */

	/* scan through the strings */
	while (*s) {
		/* record the current characters */
		s1 = *s;
		t1 = *t;

		/* swap the case on them */
		if (isletter(s1))
			s1 ^= 0x40;
		if (isletter(t1))
			t1 ^= 0x40;

		/* compare them and return value if unequal */
		cresult = s1 - t1;
		if (cresult != 0)
			return(cresult);

		/* advance to the next charector */
		++s;
		++t;
	}

	/* if there is anything left of t it is greater */
	if (*t)
		return(-1);

	/* they are the same! */
	return(0);
}
#endif
