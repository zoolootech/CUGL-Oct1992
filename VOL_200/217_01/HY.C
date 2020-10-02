/*
 * hy.c
 *
 * Mark words for hyphenation.
 * Output is one word per line.
 * May be used as a filter.
 *
 * Not too useful unless a decent exception list is added
 * to the hyphenation processing.
 *
 * Bob Denny
 * 28-Mar-82
 */

/*)BUILD	$(PROGRAM)	= hy
		$(FILES)	= { hy hyphen digram suffix }
		$(TKBOPTIONS)	= {
			STACK	= 256
			TASK	= ...HYP
			ACTFIL	= 3
			UNITS	= 3
		}
*/

#include <stdio.h>
#include <ctype.h>

#define	EOS	0

/*
 * Hyphenation marker character
 */
#define HYCH '-'

char *wdstart, *wdend;			/* Shared with hyphen()		*/
char *hyptr[16];			/* Hyphenation locations	*/

static char intext[133];		/* Input text line		*/
static char iwbuf[32];			/* Input word buffer		*/
static char owbuf[48];			/* Marked output word		*/

main()
{
	int		i;		/* Hyphen buffer index		*/
	register char	*tp;		/* Text pointer			*/
	register char	*wp;		/* Input word pointer		*/
	register char	*op;		/* Output word pointer		*/

	while((tp = gets(intext)) != NULL) {
	    while (*tp != EOS) {
		while (isspace(*tp))	/* Skip over whitespace		*/
		    tp++;		/* between words		*/
		if (*tp == EOS)
		    break;
		for (wp = iwbuf; (*wp++ = *tp) != EOS && !isspace(*tp); tp++)
			;
		*--wp = EOS;		/* Terminate word		*/
		if (wp == iwbuf)	/* Ignore null words		*/
		    continue;
	        hyphen(iwbuf);		/* Get hyphenation pointers	*/
		i = 0;			/* Start with 1st pointer	*/
		op = owbuf;		/* Initialize output pointer	*/
		wp = iwbuf;
		while ((*op++ = tolower(*wp)) != EOS) {
		    wp++;		/* (no side effect in tolower)	*/
		    if (hyptr[i] == wp) {
			/*
			 * Mark hyphenation point
			 */
			*op++ = HYCH;
			i++;		/* Next hyphenation pointer	*/
		    }
		}
		puts(owbuf);            /* Write the marked word	*/
	    }
	}
}