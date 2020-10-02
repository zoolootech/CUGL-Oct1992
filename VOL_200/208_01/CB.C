/*
NAME
	cb - C program beautifier
SYNOPSIS
	cb
DESCRIPTION
	`cb' places a copy of the C program from the standard input
	on the standard output with spacing and indentation that displays
	the structure of the program.
BUG
	almost ignorant of the C syntax.
AUTHOR
	William C. Colley, III swiped from the CIPG's UNIX system
	and modified to run under BDS C around July 1980.
	restored to Portable C by Yoshimasa Tsuji on 18 June 1987
DIAGNOSTICS
	there used to be none.
	the pairing of braces, parentheses, quotes and comments is checked.
	when a closing curly brace is found at column 1, the pairing
	  balance must be zero. otherwise a warning message is printed on
	  the standard error file.
	nested comments will invoke an error message.
	preprocessor directive('#') must be at column 1.
	funny characters are detected.
	warning messages will not be issued too often because "include" files
	  or preprocessor conditionals can contain unpaired parentheses etc.
BUG FIX
	EOF within comments used to be caught in an endless loop.
	preprocessor statements were not allowed to continue to the next line.
	many other bugs of the old version have been killed.
RESTRICTION
	trouble may be caused if the input file ends with a '\\','\n',EOF
	sequence.
	not fit for non-C format (e.g. "#define begin {" ).
*/

#include <stdio.h>
#include <ctype.h>

typedef int bool;

#define MAXCURL 20		/* {} may nest this deep */
#define MAXLEVL (MAXCURL/2)	/* `if' etc may nest this deep */
#define MAXLEN 200	/*line buffer length*/
#define MAXWARN 4	/*not more than MAXWARN will be printed*/

int clevel;	/* curly brace count */
int spflg[MAXCURL][MAXLEVL];
int sind[MAXCURL][MAXLEVL];
int iflev;		/* `if' nesting depth */
int ifflg = -1;
int level;
int ind[MAXLEVL];
int paren;	/* parenthesis count */
int pflg[MAXLEVL];
int pchar;
bool aflg;	/*flag to print a few more spaces after tabbing*/
int stabs[MAXCURL][MAXLEVL];

char string[MAXLEN];
char *cptr= string;
bool sflg = 1;	/* print the beginning part of a line */
int peek;	/* I like 0 better than EOF */
int tabs;	/* indent count	*/
int lineno = 1;
int column;
int errcount;


main() {
static int slevel[MAXLEVL] = { 0 };
static int siflev[MAXLEVL] = { 0 };
static int sifflg[MAXLEVL] = { 0 };
int lchar;
register bool qflg = 0;
static char *wif[] = { "if",0 };
static char *welse[] = { "else",0 };
static char *wfor[] = { "for", 0 };
static char *wds[] = { "case","default",0 };
register int c, cc;

static char unmatched[] = "%d: unmatched () before %s\n";

	while((c = getchr()) != EOF){
		switch(c){
		case '\\':
		case '@':
		case '$':
			fprintf(stderr,"%d: illegal char %c\n",lineno, c);
		default:
			*cptr++ = c;
			if(c != ',')lchar = c;
			continue;
		case ' ':
		case '\t':
			if(lookup(welse) == 1){
				gotelse();
				if(sflg == 0 || cptr > string) *cptr++ = c;
				putst();
				sflg = 0;
				continue;
			}
			if(sflg == 0 || cptr > string) *cptr++ = c;
			continue;
		case '\n':
		{
			register bool eflg;
			if((eflg = lookup(welse)) == 1)
				gotelse();
			putst();
			putchar('\n');
			sflg++;
			if(eflg == 1){
				pflg[level]++;
				tabs++;
			}
			else if(pchar == lchar)
				aflg++;
			continue;
			}
		case '{':
			if(paren!= 0 && ++errcount <= MAXWARN)
				fprintf(stderr,unmatched,lineno,"{"),
				paren=0;
			if(lookup(welse) == 1)gotelse();
			siflev[clevel] = iflev;
			sifflg[clevel] = ifflg;
			iflev = ifflg = 0;
			if( ++clevel > MAXCURL)
				fprintf(stderr,"%d: {} nested too deeply\n",lineno),
				exit(1);
			if(sflg != 0 && pflg[level] != 0){
				pflg[level]--;
				tabs--;
			}
			*cptr++ = c;
			putst();
			getnl();
			putst();
			putchar('\n');
			tabs++;
			sflg++;
			if(pflg[level] > 0){
				ind[level] = 1;
				if(++level > MAXLEVL)
					fprintf(stderr,"%d: nesting too deep\n",lineno),
					exit(1);
				slevel[level] = clevel;
			}
			continue;
		case '}':
			if(paren != 0 && ++errcount <= MAXWARN)
				fprintf(stderr,unmatched,lineno,"}"),
				paren=0;/*recover*/
			if(--clevel < 0 && ++errcount <= MAXWARN)
				fprintf(stderr,"%d: too many }\n",lineno);
			if(column == 1 && clevel != 0 && ++errcount <= MAXWARN)
				fprintf(stderr,"%d: unmatched {}\n",lineno);
			if((iflev = siflev[clevel]-1) < 0)iflev = 0;
			ifflg = sifflg[clevel];
			putst();
			ptabs(--tabs);
			putchar(c);
			if((peek = getchr()) == ';'){
				putchar(';');
				peek = 0;
			}
			getnl();
			putst();
			putchar('\n');
			sflg++;

			if(clevel < slevel[level] && level > 0)
				level--;
			if(ind[level] != 0){
				tabs -= pflg[level];
				ind[level] = pflg[level] = 0;
			}
			continue;
		case '"':
		case '\'':
			*cptr++ = c;
			while((cc = getchr()) != c){
				*cptr++ = cc;
				if(cc == '\\')
					*cptr++ = getchr();
				if(cc == '\n'){
					fprintf(stderr,
					"%d: %c crossed line\n",lineno,c);
					putst();
					sflg++;break;/*bug tolerant*/
				}
			}
			*cptr++ = cc;
			if(getnl() == 1){
				lchar = cc;
				peek = '\n';
			}
			continue;
		case ';':
			*cptr++ = c;
			putst();
			if(pflg[level] > 0 && ind[level] == 0){
				tabs -= pflg[level];
				pflg[level] = 0;
			}
			getnl();
			putst();
			putchar('\n');
			sflg++;
			if(iflev > 0)
				if(ifflg == 1){
					iflev--;
					ifflg = 0;
				}
				else iflev = 0;
			continue;
		case '?':
			qflg = 1;
			*cptr++ = c;
			continue;
		case ':':
			*cptr++ = c;
			/* "A? B:C */
			if(qflg == 1){
				qflg = 0;
				continue;
			}
			/* bit field or label*/
			if(paren != 0 && ++errcount <= MAXWARN)
				fprintf(stderr,unmatched,lineno,":"),
				paren = 0;
			if(lookup(wds) == 0){
				sflg = 0;
				putst();
			}
			else{
				tabs--;
				putst();
				tabs++;
			}
			if((peek = getchr()) == ';'){
				putchar(';');
				peek = 0;
			}
			getnl();
			putst();
			putchar('\n');
			sflg++;
			continue;
		case '/':
			if(pchar == '*' && ++errcount <= MAXWARN)
				fprintf(stderr,"%d: nested comments\n",
					lineno);
			*cptr++ = c;
			if((peek = getchr()) != '*')continue;
			*cptr++ = peek;
			peek = 0;
			comment();
			continue;
		case ')':
			paren--;
			*cptr++ = c;
			putst();
			if(getnl() == 1){
				peek = '\n';
				if(paren != 0)aflg++;
				else if(tabs > 0){
					pflg[level]++;
					tabs++;
					ind[level] = 0;
				}
			}
			continue;
		case '#':
			if(column != 1)
				fprintf(stderr,"%d: '#' at illegal column\n",lineno);
			*cptr++ = c;
			while((c = getchr()) != '\n') {
				/* the last character before EOF
				 * is always a newline and it is not preceded
				 * by an escape character.
				 */
				*cptr++ = c;
				if(c == '\\')
					*cptr++ = getchr();
			}
			*cptr++ = c;
			sflg = 0;
			putst();
			sflg++;
			continue;
		case '(':
			*cptr++ = c;
			paren++;
			if(lookup(wfor) == 1){
				register int ct;
				while((c = getst()) != ';');
				for(ct=0;;) {
					if ((c = getst()) == ')'){
						if(ct==0)break;
						ct--;
						continue;
					}
					if(c == '(') ct++;
				}
				paren--;
				putst();
				if(getnl() == 1){
					peek = '\n';
					pflg[level]++;
					tabs++;
					ind[level] = 0;
				}
				continue;
			}
			if(lookup(wif) == 1){
				putst();
				stabs[clevel][iflev] = tabs;
				spflg[clevel][iflev] = pflg[level];
				sind[clevel][iflev] = ind[level];
				iflev++;
				ifflg = 1;
			}
		}
	}
	if(clevel != 0 && ++errcount <= MAXWARN)
		fprintf(stderr,"unmatched {} at end\n");
	exit(errcount ==0 ? 0 :1);
}

ptabs(tabbs)
register int tabbs;
{
	while(--tabbs >= 0)putchar('\t');
}

getchr() {
static int lastchar = 0;
register int c;
	if (peek == 0) {
		if(lastchar != ' ' && lastchar != '\t')
			pchar = lastchar;
		if((c = getchar()) == '\n')lineno++,column = 0;
		else if(c != '\t' && (c < 040 || c >= 0177) && c != EOF)
			fprintf(stderr,"%d: funny character '\\%03o'\n",lineno, c);
		else column++;
	} else {
		c = peek; peek = 0;
	}
	return(lastchar = c);
}

putst() {
	if(cptr > string){
		if(sflg != 0){
			ptabs(tabs);
			sflg = 0;
			if(aflg != 0){
				aflg = 0;
				if(tabs > 0)printf("    ");
			}
		}
		*cptr = '\0';
		if(cptr > string + MAXLEN)
			fprintf(stderr,"%d: line too long\n",lineno),
			exit(1);
		printf("%s",string);
		cptr = string;
	}
	else if(sflg != 0)
			aflg= sflg = 0;
}

bool
lookup(tab)
register char **tab;
{
	register char r;
	register char *strptr, *sptr, *tptr;

	if(cptr <= string)return(0);
	sptr = string -1;
	while(*++sptr == ' ' || *sptr == '\t');
	while(tptr = *tab++) {
		strptr=sptr -1; /*dont optimise*/
		while((r = *tptr++) == *++strptr && r != '\0');
		if(r == '\0' && !isalnum(*strptr) &&  *strptr != '_')
			return(1);
	}
	return(0);
}
/*getst returns the first normal character after a parenthesis */
getst(){
	register int ch, cc;

	for(;;) {
		*cptr++ = ch = getchr();
		if(ch == '\'' || ch == '"'){
			while((*cptr++ = cc= getchr()) != ch)
				if(cc == '\\') {
					*cptr++ = getchr();
					putst(); /*trim*/
				}
				else if(cc== '\n') {
					fprintf(stderr,
					"%d: %c crossed line\n",lineno,ch);
					break; /*bug tolerant*/
				}
			continue;
		}
		if(ch == '\n') {
			putst();
			aflg++;
			continue;
		}
		return(ch);
	}
}

gotelse()
{
	tabs = stabs[clevel][iflev];
	pflg[level] = spflg[clevel][iflev];
	ind[level] = sind[clevel][iflev];
	ifflg = 1;
}
getnl()
{
	while((peek = getchr()) == '\t' || peek == ' '){
		*cptr++ = peek;
		peek = 0;
	}
	if((peek = getchr()) == '/'){
		peek = 0;
		*cptr++ = '/';
		if((peek = getchr()) == '*'){
			*cptr++ = '*';
			peek = 0;
			comment();
		}
	}
	if((peek = getchr()) == '\n'){
		peek = 0;
		return(1);
	}
	return(0);
}
comment()
{
register int c;
register int line;
	line= lineno;
	for(;;) {
		if((*cptr++ = c = getchr()) != '*') {
			if(c == '\n') putst(), sflg++;
			else if(c== EOF) {
				fprintf(stderr,"%d: unmatched comment\n",line);
				exit(1);
			}
			continue;
		}
		while ((*cptr++ = c = getchr()) == '*')
			;
		if(c == '/')break;
	}
}
