/* [FIND1.C of JUGPDS Vol.18]
*****************************************************************
*								*
*       Written by  Hakuo Katayose (JUG-CP/M No.179)		*
*		    49-114 Kawauchi-Sanjuunin-machi		*
*		    Sendai, Miyagi 980                          *
*		    Phone: 0222-61-3219				*
*								*
*       Edited & tested by Y. Monma (JUG-C/M Disk Editor)       * 
*								*
*****************************************************************
*/

/* find1 - find patterns in text */

#include "stdio.h"

#define	BDS		1

/*
#define	DEBUG		1
*/

#ifdef	BDS

#include <dio.h>

#define	stderr		STDERR
#define	stdin		STDIN

#endif


#define	MAXLINE		256
#define	TAB		0x09
#define	YES		0
#define	NO		-1

#define	NOT_CHAR	'!'
#define	NOT		0x00800
#define	NMASK		0x0FEFF
#define	BOL_CHAR	'%'
#define	BOL		0x00125
#define	ANY_CHAR	'?'
#define	ANY		0x0013F
#define	EOL_CHAR	'$'
#define	EOL		0x00124
#define	PCLOSURE_CHAR	'+'
#define	CLOSURE_CHAR	'*'
#define	CLOSURE		0x04000
#define	CMASK		0x0BFFF
#define	CCL_CHAR	'['
#define	CCL		0x02000
#define	NCL		0x01000
#define	ECL_CHAR	']'
#define	GROUP		'-'
#define	UC		'\\'

int	np;
int	spat[16][64];

main(argc, argv)
int	argc;
char *argv[];

{
	char	line[MAXLINE];
	int	pat[256];

#ifdef	DEBUG
	int	i, j;
#endif

#ifdef	BDS
	dioinit(&argc, argv);
#endif

	np = 0;
	if (argc < 2) {
		fprintf(stderr, "Usage: find1 pattern <infile >outfile\n");
		exit();
	}
	makpat(pat, argv[1]);

#ifdef	DEBUG

	printf("PAT[ ]");
	for (j = 0; pat[j] != '\0'; j++)
		printf("%04x", pat[j]);
	putchar('\n');
	for (i = 0; i < np; i++) {
		printf("PAT[%1d]", i);
		for (j = 0; spat[i][j] != '\0'; j++)
			printf("%04x", spat[i][j]);
		putchar('\n');
	}
#endif

	while (getline(line, MAXLINE) > 0)
		if (match(line, pat) >= 0)
			printf("%s", line);
#ifdef	BDS
	dioflush();
#endif

}

/* makpat -make pattern from arg(from), terminates atdelim */
makpat(s, t)
int	*s;
char	*t;

{
	register	char	*tt;

	tt = t;
	if (*t == BOL_CHAR) {
		*s++ = BOL;
		t++;
	}
	for (; *t != '\0'; t++) {
		if (*t == ANY_CHAR)
			*s++ = ANY;
		else if (*t == EOL_CHAR && *(t+1) == '\0')
			*s++ = EOL;
		else if (*t == CLOSURE_CHAR && t > tt)
			*(s-1) |= CLOSURE;
		else if (*t == PCLOSURE_CHAR && t > tt)
			*s++ = *(s-1) | CLOSURE;
		else if (*t == CCL_CHAR && *(t+1) != '\0')
			getccl(np++,&s,&t);
		else if (*t == NOT_CHAR && *t != '\0') {
			t++;
			*s++ = esc(&t) | NOT;
		}
		else
			*s++ = esc(&t) ;
	}
	*s = '\0';
}

/* getccl - expand char class at arg(i) into pat(j) */
getccl(np,x,y)
register int  **x;
register char **y;

{
	char	ss, st, ch;
	int	i;
#define	s	(*x)
#define	t	(*y)

	t++;
	if (*t == NOT_CHAR) {
		*s++ = NCL + np;
		t++;
	}
	else
		*s++ = CCL + np;
	for (i = 0; *t != ECL_CHAR && *t != '\0'; i++, t++) {
		if (*t != GROUP)
			spat[np][i] = esc(y);
		else {
			t++;
			ss = esc(y);
			st = spat[np][i-1];
			if ((isdigit(st) && isdigit(ss) ||
			     islower(st) && islower(ss) ||
			     isupper(st) && isupper(ss)) &&
			     st < ss) {
				i--;
				for (ch = st; ch <= ss; ch++)
					spat[np][i++] = ch;
				i--;
			}
			else {
				spat[np][i++] = GROUP;
				spat[np][i]   = ss;
			}
		}
	}
	spat[np][i] = '\0';
	t--;
	if (*t != '\0')
		t++;

#undef t
#undef s
}

/* esc - map aray(i) into escaped character if appropriate */
esc(x)
register char **x;
{
#define	t	(*x)

	if (*t == '@' && *(t+1) != '\0')
		switch (*++t) {
			case 'n' :
			case 'N' :	return ('\n');
			case 's' :
			case 'S' :	return (' ');
			case 't' :
			case 'T' :	return ('\t');
			default  :	return (*t);
		}
	else if (*t == UC && *(t+1) != '\0')
		return toupper(*++t);
	else
		return tolower(*t);

#undef	s
}


getline(s,lim)
char *s;

{
	register int	c;
	register char	*p;

	p = s;
	while (--lim > 0 && (c = getchar()) != EOF && c != '\n')
		*p++ = c;
	if (c == '\n')
		*p++ = '\n';
	*p = '\0';
	return (p-s);
}

/* match - find character anywhere on line */
match(text,pattarn)
int	*pattarn;
char	*text;

{
	int	i;
	char	*ss;

	for (ss = text; *text != '\0'; text++) {
		if ((i = amatch(text,pattarn,ss)) >= 0)
			return i;
	}
	return -1;
}

/* amatch (non-recursive) - look for match starting at lin(from) */
amatch(text,pattarn,tp)
register char *text, *tp;
register int  *pattarn;
{
	char	*temp, *tmp2;
	int	k, *newpat;

	for (temp = text; *pattarn != '\0';pattarn++) {
		if ((*pattarn & CLOSURE) == CLOSURE) {
			for (tmp2 = temp; *tmp2 != '\0';) {
				if (omatch(&tmp2,*pattarn & CMASK,tp) == NO)
					break;
			}
			for (newpat = pattarn+1; tmp2 >= temp; tmp2--) {
				if ((k = amatch(tmp2,newpat,tp)) >= 0)
					break;
			}
			temp = tmp2 + k;
			break;
		}
		else if (omatch(&temp,*pattarn,tp) == NO)
			return -1;
	}
	return (temp-text);
}

/* omatch - try to match a single pattern at pat(j */
omatch(ln, pattarn, s)
register char **ln, *s;
register int  pattarn;
{
	int	retv;

	if (**ln == '\0')
		retv =  (NO);
	else if (pattarn == ANY && **ln != '\n') {
		(*ln)++;
		retv =  (YES);
		}
	else if (pattarn == BOL && *ln == s)
		retv =  (YES);
	else if (pattarn == EOL && **ln == '\n')
		retv =  (YES);
	else if ((pattarn & NOT) == NOT)
		retv =  (*(*ln)++ == (pattarn & NMASK) ? NO : YES);
	else if ((pattarn & CCL) == CCL)
		retv = iindex(spat[pattarn & 255],*(*ln)++);
	else if ((pattarn & NCL) == NCL)
		retv=(iindex(spat[pattarn & 255],*(*ln)++) == YES ? NO : YES);
	else
		retv =  (*(*ln)++ == pattarn ? YES : NO);
	return retv;
}


iindex(pat,c)
register int *pat;
register char c;

{
	pat--;
	while (*++pat)
		if (*pat == c)
			return YES;
	return NO;
}
