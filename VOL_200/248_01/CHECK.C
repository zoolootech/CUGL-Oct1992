/*	CHECK:	Dictionary scan module for MicroSPELL 1.0
		Spell Checker and Corrector

		(C)opyright May 1987 by Daniel Lawrence
		All Rights Reserved

*/

#include	<stdio.h>
#include	"dopt.h"
#include	"dstruct.h"
#include	"ddef.h"

check()

{
	int i;		/* index into the word list */

	/* sort the source words */
	if (swdebug)
		printf("[%u words being checked...\n", numwords);
	wordsort();
	if (swdebug)
		printf("  sorted...");

	/* merge them against the uppercase part of the dictionary */
	umerge();
	if (swdebug)
		printf("Uppercase checked\n");


	/* lowercase the source word list */
	wordlow();

	/* resort them */
	wordsort();
	if (swdebug)
		printf("  sorted...");

	/* merge them against the rest the dictionary */
	merge();
	if (swdebug)
		printf("lowercase checked\n");

	/* sort them back into position order */
	possort();
	if (swdebug) {
		printf("  sorted in position order...");
		printf("%u mismatched words]\n", badwords);
	}

	/* dump the list to disk */
	dumplist();

	/* lastly, de-allocate the word list */
	while (numwords > 0)
		free(sword[--numwords]);
}

int merge()		/* do a merge run against the main dictionary	*/

{
	register int cindex;	/* current word index */
	register char *cword;	/* ptr to current word */
	register int cmp;	/* result of current comparision */
	register int mismatch;	/* number of mismatched words */
	char mword[NSTRING];	/* current dictionary word */

	/* start with a LOW VALUES dictionary word */
	mword[0] = 0;
	mismatch = 0;

	strcpy(mword, nxtmword());

	for (cindex = 0; cindex < numwords; cindex++) {

		/* get the current word */
		cword = sword[cindex]->w_text;

		/* scan the dictionary for a match */
		cmp = strcmp(cword, mword);
		while (cmp > 0) {
			strcpy(mword, nxtmword());
			cmp = strcmp(cword, mword);
		}

		/* if this word is not matched..... */
		if (cmp != 0) {
			if (mismatch == cindex)
				++mismatch;
			else {
				sword[mismatch++] = sword[cindex];
				sword[cindex] = NULL;
			}
		} else {
			free(sword[cindex]);
			sword[cindex] = NULL;
		}
	}
	numwords = mismatch;
	badwords += mismatch;
	mclose();
}

/* do a merge run against the upper case entries in the main dictionary	*/

int umerge()

{
	register int cindex;	/* current word index */
	register char *cword;	/* ptr to current word */
	register int cmp;	/* result of current comparision */
	register int mismatch;	/* number of mismatched words */
	char mword[NSTRING];	/* current dictionary word */

	if (mopen() == FALSE)
		exit(EXMDICT);

	mword[0] = 0;		/* start with a LOW VALUES dictionary word */
	mismatch = 0;

	strcpy(mword, nxtmword());

	for (cindex = 0; cindex < numwords; cindex++) {

		/* get the current word */
		cword = sword[cindex]->w_text;

		/* scan the dictionary for a match */
		cmp = strcmp(cword, mword);
		while (cmp > 0 && mword[0] != SEPCHAR) {
			strcpy(mword, nxtmword());
			cmp = strcmp(cword, mword);
		}

		/* if we reach the end of the capitals...stop */
		if (mword[0] == SEPCHAR)
			break;

		/* if this word is not matched..... */
		if (cmp != 0) {
			if (mismatch == cindex)
				++mismatch;
			else {
				sword[mismatch++] = sword[cindex];
				sword[cindex] = NULL;
			}
		} else {
			free(sword[cindex]);
			sword[cindex] = NULL;
		}
	}

	/* move the rest of the source words down in the list */
	if (cindex < numwords && mismatch != cindex) {
		for (; cindex < numwords; cindex++) {
			sword[mismatch++] = sword[cindex];
			sword[cindex] = NULL;
		}
		numwords = mismatch;
	}

}

wordlow()	/* lowercase all the words */

{
	register cindex;		/* index into the word list */
	register char *ptr;		/* ptr to current word */

	for (cindex = 0; cindex < numwords; cindex++) {
		ptr = sword[cindex]->w_text;
		while (*ptr) {
			if (isupper(*ptr))
				*ptr += ('a' - 'A');
			++ptr;
		}
	}
}

dumplist()

{
	int i;

	/* if there is a user dictionary, dump its name out */
	if (*userlist)
		if (swwords)
			fprintf(outfile, "USER LIST: %s\n", userlist);
		else
			fprintf(outfile, "-3\n%s\n", userlist);

	for (i = 0; i < numwords; i++) {

		/* if we are in a new file... write a file header */
		if (outnum != sword[i]->w_file) {
			outnum = sword[i]->w_file;
			if (swwords)
				fprintf(outfile, "FILE: %s\n", splname[outnum]);
			else
				fprintf(outfile, "-1\n%s\n", splname[outnum]);
		}

		/* write the row and column number of the current word */
		if (swwords)
			fprintf(outfile, "%s\n", sword[i]->w_text);
		else
			fprintf(outfile, "%u\n%u\n",
				sword[i]->w_line, sword[i]->w_col);
	}
}
