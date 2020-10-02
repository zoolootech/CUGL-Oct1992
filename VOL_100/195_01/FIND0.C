/* [FIND0.C of JUGPDS Vol.18]
*****************************************************************
*								*
*	Written by  Hakuo Katayose (JUG-CP/M No.179)		*
*		    49-114 Kawauchi-Sanjuunin-machi		*
*		    Sendai, Miyagi 980                          *
*		    Phone: 0222-61-3219				*
*								*
*	Edited & tested by Y. Monma (JUG-C/M Disk Editor)       * 
*								*
*****************************************************************
*/

/* find - find patterns in text */

#include "stdio.h"
#include "def.h"
#include <dio.h>

main(argc, argv)
int	argc;
char	**argv;

{
	char	lin[MAXLINE], pat[MAXPAT];
	char	opt_l, *ap, *cp;
	int	i, lno, nl, nx;

	dioinit(&argc, argv);
	if (argc < 2) {
		error("Usage: find0 pattern <infile >outfile\n");
		exit();
		}
	opt_l = OFF;
	nl = 1; i = 0;
	i = 0;
	while (--argc > 0) {
		if ((*++argv)[0] == '-')
			for (ap = argv[0]+1; *ap != '\0'; ap++) {
				if (tolower(*ap) == 'l') {
					opt_l = ON;
					nl = 0;
					for(; isdigit(nx = *(ap+1)); ap++)
						nl = nl*10 + nx - '0';
					nl = (nl == 0 ? 1 : nl);
					}
				else
					fprintf(STDERR,
					"[-%c]:Illigal option\n",tolower(*ap));
				}
		cp = *argv;
		}
	if (getpat(cp, pat) == ERROR)
		exit(fprintf(STDERR, "Illegal pattarn.\n"));
	lno = 0;
	while (getlin(lin, MAXLINE) > 0) {
		lno += nl;
		if (match(lin, pat) == YES)
			if (opt_l == ON)
				printf("%6d: %s", lno, lin);
			else
				printf("%s", lin);
		}
	dioflush();
}

/* getpat - convert argument into pattern */
getpat(arg, pat)
char	*arg, *pat;
{
	return(makpat(arg, 0, '\0', pat));
}

/* match - find match anywhere on line */
match(lin, pat)
char	lin[], *pat;
{
	int i;


	for (i = 0; lin[i] != '\0'; i++)
		if (amatch(lin, i, pat) >= 0)
			return (YES);
	return(NO);
}

/* amatch (non-recursive) - look for stating at lin(from) */
amatch(lin, from, pat)
char	lin[], pat[];
int	from;

{
	int	i, j, offset, stack;

	stack = 0;
	offset = from;
	for (j = 0; pat[j] != '\0'; j += patsiz(pat, j))
		if (pat[j] == CLOSURE) {
			stack = j;
			j += CLOSIZE;
			for (i = offset; lin[i] != '\0'; )
				if (omatch(lin, &i, pat, j) == NO)
					break;
			pat[stack + COUNT] = i - offset;
			pat[stack + START] = offset;
			offset = i;
			}
		else if (omatch(lin, &offset, pat, j) == NO) {
			for (; stack > 0; stack = pat[stack + PREVCL]) {
				if (pat[stack + COUNT] > 0)
					break;
				}
			if (stack <= 0)
				return(-1);
			pat[stack + COUNT]--;
			j = stack + CLOSIZE;
			offset = pat[stack + START] + pat[stack + COUNT];
			}
	return(offset);
}

/* patsiz - returns size of pattern entry at pat(n) */
patsiz(pat, n)
char	pat[];
int	n;

{
	switch (pat[n]) {
		case BPAT :
		case EPAT :
		case CHAR :
			  return(2);
		case BOL :
		case EOL :
		case ANY :
			  return(1);
		case CCL :
		case NCCL:
			  return (pat[n+1] + 2);
		case CLOSURE :
			  return CLOSIZE;
		default  :
			  exit(fprintf(STDERR, "Error in patsiz: can't happen.\n"));
	}
}

/* omatch - try to match a single pattern at pat(j) */
omatch(lin, i, pat, j)
char	lin[], pat[];
int	*i, j;

{
	int	bump;
	char	c;

	c = lin[ *i ];
	if (c == '\0')
		return(NO);
	bump = -1;
	switch (pat[j]) {
		case BPAT :
		case EPAT : return(YES);
		case CHAR : if (c == pat[j+1])
				bump= 1;
			    break;
		case BOL :  if (*i == 0)
				bump = 0;
			    break;
		case ANY :  if (c != '\n')
				bump = 1;
			    break;
		case EOL :  if (c == '\n')
				bump = 0;
			    break;
		case CCL :  if (locate(c, pat, j+1) == YES)
				bump = 1;
			    break;
		case NCCL:  if (c != '\n' && locate(c, pat, j+1) == NO)
				bump = 1;
			    break;
		default  :  fprintf(STDERR, "Error in omatch: can't happen.\n");
	}
	if (bump >= 0) {
		*i += bump;
		return(YES);
		}
	return(NO);
}

/* locate - look for c in char class at pat(offset) */
locate(c, pat, offset)
char	c, pat[];
int	offset;

{
	int	i;

	for (i = offset + pat[offset]; i > offset; i--)
		if (c == pat[i])  return YES;
	return NO;
}
