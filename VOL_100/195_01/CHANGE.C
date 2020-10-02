/* [CHANGE.C of JUGPDS Vol.18]
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

/* change - change "from" into "to" 

	Usage: change [-l][-v] from [to] <infile [>outfile]

where
	-l: line # 
	-v: 

*/

#include "stdio.h"
#include "def.h"
#include <dio.h>

main(argc, argv)
int	argc;
char	**argv;

{
	char	lin[MAXLINE], pat[MAXPAT], new[MAXLINE], sub[10][MAXLINE];
	char	cmd[2][MAXLINE];
	char	opt_l, opt_v, *ap;
	int	i, k, lastm, m, lno, maxsub, nl, nx;

	dioinit(&argc, argv);
	opt_l = opt_v = OFF;
	nl = 1; i = 0;
	while (--argc > 0) {
		if ((*++argv)[0] == '-') /* check option */
			for (ap = argv[0]+1; *ap != '\0'; ap++) {
				if (tolower(*ap) == 'l') {
					opt_l = ON;
					nl = 0;
					for (; isdigit(nx = *(ap+1)); ap++)
						nl = nl*10 + nx - '0';
					nl = (nl == 0 ? 1 : nl);
					}
				else if (tolower(*ap) == 'v')
					opt_v = ON;
				else
					fprintf(STDERR,
					"[-%c]:Illigal option\n",tolower(*ap));
				}
		else if (i < 2)
			strcpy(cmd[i++], *argv);
		else
			remark("Too many arguments!\n");
		}
	if (i == 0)
		error("Usage: change [-l][-v] from [to] <infile [>outfile]\n");
	if ((maxsub = getpat(cmd[0], pat)) == ERROR)
		error("Illegal from pattern.\n");
	if (i > 1) {
		if (getsub(cmd[1], sub[0]) == ERROR)
			error("Illegal to pattern.\n");
		}
	else
		sub[0][0] = EOS;
	printf("Changed to "); puts(&sub[0][0]); printf("\n");
	lno = 0;
	while (getlin(lin, MAXLINE) > 0) {
		lno += nl;	k = 0;	lastm =	-1;
		for (i = 0; lin[i] != EOS; ) {
			m = amatch(lin, i, pat, sub, maxsub);
			if (m >= 0 && lastm != m) { /* replace matched text */
				catsub(lin, i, m, new, &k, MAXLINE, sub);
				lastm = m;
				}
			if (m == ERROR || m == i) /* no match or null match */
				addset(lin[i++], new, &k, MAXLINE);
			else			/* skip matched text */
				i = m;
			}
		if (k >= MAXLINE) {
			new[MAXLINE] = EOS;
			fprintf(STDERR, "line truncated.\n:%s", new);
			printf("        %s\n", new);
			}
		else
			new[k] = EOS;
		if (lastm >= 0)
			if (opt_l == ON)
				fprintf(STDERR, "%6d: %s", lno, new);
			else
				fprintf(STDERR, "%s", new);
		if (opt_v == ON)
			if (opt_l == ON)
				printf("%6d: %s", lno, new);
			else
				printf("%s", new );
		}
	dioflush();
}


/* catsub - add replacement text to end of new */
catsub(lin, from, to, new, k, maxnew, sub)
int	from, to, *k, maxnew;
char	lin[], new[], sub[10][MAXPAT];

{
	int	i, j, ns;

	for (i = 0; sub[0][i] != EOS && *k < maxnew; i++) {
		if (sub[0][i] == DITTO) {
			for (j = from; j < to && *k < maxnew; j++)
				new[(*k)++] = lin[j];
			}
		else if (sub[0][i] == SUBSTRG) {
			ns = sub[0][++i] - '0';
			for (j = 0; sub[ns][j] != EOS && *k < maxnew; j++)
				new[(*k)++] = sub[ns][j];
			}
		else if (*k >= maxnew)
			break;
		else
			new[(*k)++] = sub[0][i];
		}
}

/* getsub - get substitution pattern into sub */
getsub(arg, sub)
char	*arg, *sub;

{
	return(maksub(arg, 0, EOS, sub));
}

/* maksub -make substitution string in sub */
maksub(arg, from, delim, sub)
char	delim, arg[], *sub;
int	from;

{
	int	i, j;

	for (i = from; arg[i] != delim && arg[i] != EOS; i++)
		if (arg[i] == AND)
			*sub++ = DITTO;
		else if (arg[i] == ESCAPE && (isdigit(arg[i+1]))) {
			*sub++ = SUBSTRG;
			*sub++ = arg[++i];
			}
		else
			*sub++ = esc(arg, &i);
	*sub = EOS;
	if (arg[i] != delim)
		return ERROR;
	if (strlen(sub) > MAXLINE)
		return ERROR;
	return(i);
}


/* getpat - convert argument into pattern */
getpat(arg, pat)
char	*arg, *pat;

{
	return(makpat(arg, 0, EOS, pat));
}

/* amatch (non-recursive) - look for match starting at lin(from) */
amatch(lin, from, pat, sub, maxsub)
char	lin[], pat[], sub[10][MAXPAT];
int	from, maxsub;

{
	int	i, j, offset, stack, bgn[10], lst[10], l;

	stack = 0;
	offset = from;
	for (j = 0; pat[j] != EOS; j += patsiz(pat, j))
		if (pat[j] == BPAT )
			bgn[pat[j+1]-'0'] = offset;
		else if (pat[j] == EPAT)
			lst[pat[j+1]-'0'] = offset-1;
		else if (pat[j] == CLOSURE) {
			stack = j;
			j += CLOSIZE;
			for (i = offset; lin[i] != EOS; )
				if (omatch(lin, &i, pat, j) == NO)
					break;
			pat[stack + COUNT] = i - offset;
			pat[stack + START] = offset;
			offset = i;
			}
		else if (omatch(lin, &offset, pat, j) == NO) {
			for ( ; stack > 0; stack = pat[stack + PREVCL] ) {
				if (pat[stack + COUNT] > 0)
					break;
				}
			if (stack <= 0)
				return(ERROR);
			pat[stack + COUNT]--;
			j = stack + CLOSIZE;
			offset = pat[stack + START] + pat[stack + COUNT];
			}
	for (l = 1; l <= maxsub; l++)
		copysub(lin, bgn[l], lst[l], sub[l]);
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
			  error("Error in patsiz: can't happen.\n");
	}
}


/* omatch - try to match a single pattern at pat(j) */
omatch(lin, i, pat, j)
char	lin[], pat[];
int	*i, j;

{
	int	bump;
	char	c;

	c = lin[*i];
	if (c == EOS)
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
		case ANY :  if (c != NEWLINE)
				bump = 1;
			    break;
		case EOL :  if (c == NEWLINE)
				bump = 0;
			    break;
		case CCL :  if (locate(c, pat, j+1) == YES)
				bump = 1;
			    break;
		case NCCL:  if (c != NEWLINE && locate(c, pat, j+1) == NO)
				bump = 1;
			    break;
		default  :  remark("Error in omatch: can't happen.\n");
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

/* copysub - copy line to sub from b to e */ 
copysub(lin, b, e, sub)
char	lin[], *sub;
int	b, e;
{
	for( ; b <= e; b++)
		*sub++ = lin[b];
	*sub = EOS;
	return;
}
