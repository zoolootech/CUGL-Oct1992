/*
	Transliteration filter
	Adapted from Software Tools, by Kernighan & Plauger.

	source:  tr.c
	version: February 22, 1983

	Link this program to the directed I/O packages as follows:
	A>clink tr -f dio

	Transliterated from RATFOR to BDS C by:

		Robert Pasky
		36 Wiswall Rd
		Newton, MA 02159
		(617) 964-3641

	Edited by:

		Edward K. Ream
		1850 Summit Ave.
		Madison, WI 53705
		(608) 231 - 2952


	See the file tr.doc for complete documentation.
*/


#include "bdscio.h"
#include "dio.h"

#define MAXSET 129
#define NOTSYM '~'
#define ESCAPE '\\'
#define DASH '-'
#define UPPER 1
#define LOWER 0
#define YES 1
#define NO 2

/* comment out ----- remove this comment to get debugging messages
#define DEBUG 1
----- end comment out */

int casesw;
int ix, jx;

main(argc, argv)
int  argc;
char **argv;
{
	/* Set up the filter. */
	_allocp = NULL;
	dioinit(&argc, argv);
	main1(argc, argv);

	/* Indicate no error in the pipe. */
	dioflush();	
}


/* Main line. */

main1(argc, argv)
int argc;
char *argv[];
{
	int  c;
	char from[MAXSET], to[MAXSET];
	char *s;
	int allbut, collap, i, lastto;

	if (argc < 2) {
		fprintf(STD_ERR,
		"usage: tr <infile [>outfile] from [to]\n");
		dioflush();
		exit();
	}

	casesw = LOWER;
	s = *++argv;

	from[0] = NULL;
	if (s[0] == NOTSYM) {
		allbut = YES;
		if (makset(s, 1, from) == NO) {
			fprintf(STD_ERR, "<from> too large\n");
			dioflush();
			exit();
		}
	}
	else {
		allbut = NO;
		if (makset(s, 0, from) == NO) {
			fprintf(STD_ERR, "<from> too large\n");
			dioflush();
			exit();
		}
	}

#ifdef DEBUG
	fprintf(STD_ERR, "from: <%s>\n", from);
#endif

	casesw = LOWER;
	s = *++argv;

	to[0] = NULL;
	if (argc <= 2 || s[0] == NULL) {
		to[0] = NULL;
	}
	else if (makset(s, 0, to) == NO) {
		fprintf(STD_ERR, "<to> too large\n");
		dioflush();
		exit();	
	}

#ifdef DEBUG
	fprintf(STD_ERR, "to: <%s>\n", to);
#endif

	lastto = strlen(to) - 1;
	if (strlen(from) > (lastto + 1) || allbut == YES) {
		collap = YES;
	}
	else {
		collap = NO;
	}

	for(;;) {
		c = getchar();
		i = xindex(from, c, allbut, lastto);
		if (collap == YES && i >= lastto && lastto >= 0) {
			putchar(to[lastto]);
			do {
				c = getchar();
				i = xindex(from, c, allbut, lastto);
			} while (i >= lastto);
		}
		if (c == CPMEOF || c == EOF) {
			break;
		}
		if (i >= 0 && lastto >= 0) {
			putchar(to[i]);
		}
		else if (i == -1) {
			putchar(c);
		}
	}
}


makset(array, k, set)	/* make the from/to set from array */
int k;
char array[], set[];
{
	ix = k;
	jx = 0;
	filset(NULL, array, set);
	return (addset(NULL, set));
}


addset(c, set)		/* add single code to set */
char c, set[];
{
	if (jx > MAXSET)
		return(NO);
	set[jx++] = c;
	return (YES);

}


filset(delim, array, set)	/* fill set from array */
char delim, array[], set[];
{
	char digits[11];
	char lowalf[27];
	char upalf[27];
	char c;

	strcpy(digits, "0123456789");
	strcpy(lowalf, "abcdefghijlkmnopqrstuvwxyz");
	strcpy(upalf, "ABCDEFGHIJLKMNOPQRSTUVWXYZ");

	for ( ; array[ix] != delim && array[ix] != NULL; ix++) {
		if (array[ix] == ESCAPE) {
			if ((c = esc(array)) != NULL) {
				addset(c, set);
			}
		}
		else if (array[ix] != DASH) {
			if (casesw == LOWER) {
				array[ix] = tolower(array[ix]);
			}
			addset(array[ix], set);
		}
		else if (jx <= 0 || array[ix + 1] == NULL) {
			addset(DASH, set);
		}
		else if (index(digits, set[jx - 1]) >= 0 ) {
			dodash(digits, array, set);
		}
		else if (index(lowalf, set[jx - 1]) >= 0 ) {
			dodash(lowalf, array, set);
		}
		else if (index(upalf, set[jx - 1]) >= 0 ) {
			dodash(upalf, array, set);
		}
		else {
			addset(DASH, set);
		}
	}
}


dodash(valid, array, set)	/* parse intervals */
char valid[], array[], set[];
{
	int k, limit;
	ix++;
	jx--;

	limit = index(valid, esc(array));
	for (k = index(valid, set[jx]); k <= limit; k++) {
		addset(valid[k], set);
	}
}


esc(array)		/* handle escape sequences, returns intended code */
char array[];
{
	int sum;

	if (casesw == LOWER) {
		array[ix] = tolower(array[ix]);
	}
	if (array[ix] != ESCAPE) {
		return (array[ix]);
	}
	else if (array[ix + 1] == NULL) {
		return (ESCAPE);
	}
	else {
		ix++;
		switch(tolower(array[ix])) {
		case 'n':
			return('\n');
		case 't':
			return('\t');
		case 'b':
			return(' ');
		case 'r':
			return('\r');
		case 'l':
			casesw = LOWER;
			return (NULL);
		case 'u':
			casesw = UPPER;
			return (NULL);
		case '0':	case '1':
		case '2':	case '3':
		case '4':	case '5':
		case '6':	case '7':
			sum = array[ix] - '0';
			while (isdigit(array[ix+1])) {
				sum = sum * 8 + array[ix++] - '0';
			}
			return (sum & 0xff);

		default:
			if (casesw == LOWER) {
				array[ix] = tolower(array[ix]);
			}
			return(array[ix]); /* takes care of ESCAPE ESCAPE */
		}
	}
}


xindex(array, c, allbut, lastto)	/* return index of c
					   modified by allbut */
char array[], c;
int allbut, lastto;
{
	int i;

	if (c == NULL) {
		return (-1);
	}
	else if (allbut == NO) {
		return (index(array, c));
	}
	else if (index(array, c) >= 0) {
		return (-1);
	}
	else {
		return (lastto + 1);
	}
}


index(s, c)		/* return index of c in s */
char *s, c;
{
	int i;

	for (i = 0; s[i] != NULL; i++) {
		if(s[i] == c) {
			return (i);
		}
	}
	return (-1);
}
/
char *s,