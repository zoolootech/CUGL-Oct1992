/*
	calc.c---very long integer calculator in reverse polish
	notational form.  It is to some small extent programable,
	in that you may write a string to any one of 10 arrays
	which I've called scripts.  These are executed by way of
	the command 'exe' which requires a single numeric operand
	as a prefix.  You might think of this as a simple version
	of a HP11c handheld with a very large LCD display!  I suppose
	this program should have a manual, but since I only intended
	it to be a demo for vli.crl and math.crl, I didn't bother 
	and now...well like topsy, it sort of grew out of hand.
	I hope that if you are interested, the code should be clear enough to
	explain all.  It also should be easy to extend...some things
	I had thought to add were better (less dangerous!) recursion, and
	maybe the ability to read/write a set of scripts from/to a
	text file.  Then later, conditionals based on boolean evaluation
	of top-of-stack...jumps and script calls.  Then it could be
	extended to allow rational numbers, complex too for that matter.
	After a while, it might	evolve into a small language.  Who knows!

	by Hugh S. Myers

	build:
		n>cc calc
		n>clink calc vli math cio

	3/20/84
	4/9/84

	commands:

		'|'		print '\n' (allows embedded newlines in 
				scripts for display)
		^Z		clearscreen on Televideo terminals (see CRT
				note below)
		'='		print top of stack (tos)
		'clr'		clear stack
		'pop'		next on stack (nos) becomes tos
		'xch'		swap tos and nos
		'clt'		set tos to zero
		op 'pgm'	do a getline and place result in script[op]
		op 'exe'	retrieve script[op] and execute it as
				though it were from the terminal
		1 'sho'		display number stack on screen
		2 'sho'		display variable list on screen
		3 'sho'		display	script list on screen
		op1 op2 'sto'	store op2 to variable[op1]
		op 'rcl'	push variable[op] on tos
		op 'fac'	push factorial(op) on tos
		op 'sqrt'	push square-root(op) on tos
		op 'neg' || '~'		push negative(op) on tos 
		op 'sqr'	push square(op) on tos
		op 'abs'	push absolute-value(op) on tos
		op1 op2 'add' || '+'	tos = op1 + op2
		op1 op2 'sub' || '-'	tos = op1 - op2
		op1 op2 'mul' || '*'	tos = op1 * op2
		op1 op2 'div' || '/'	tos = op1 / op2
		op1 op2 'pow' || '^'	tos = op1 ** op2
		op1 op2 'mod' || '#'	tos = remainder op1 / op2
		op1 op2 'min'	tos = minimum op1, op2
		op1 op2 'max'	tos = maximum op1, op2



	CRT notes...

	This was developed on a TVI 950...so if you have one (or
	an emulation) fine.  If not change the defines for TVI950
	and CLEARSCREEN...

*/

#include <bdscio.h>
#define TVI950 1	/* terminal type for specific flashy stuff */
			/* use #undef for vanilla version */
#define MAX 256		/* number of columns of string arrays */
#define DEPTH 10	/* number of rows of string arrays */
#define SETIO 0x12	/* for cio.crl */
#define ESC 27
#define CLEARSCREEN 26
#define CR 13
#define BS 8
#define DEL 127
#define LIMIT 20	/* nesting level allowed for script recursion */
#define streq(s1,s2) !strcmp(s1,s2) /* reverse the logic for clarity */

/*
	globals
*/

char script[DEPTH][MAX];
char stack[DEPTH][MAX];
char number[MAX];
char var[DEPTH][MAX];
char line[MAX], keyword[MAX];
int i, lastch, errnumb, deep;
char flagged;

main()
{
	int j;

	for(j=0;j<DEPTH;j++)
		strcpy(var[j],"0");
	i = 0; /* stack pointer */
	strcpy(stack[i],"empty stack");
	strcpy(number,"");
	strcpy(keyword,"");
	ttymode(SETIO);
	flagged = FALSE;

	puts("c calculator\n");
	prompt();
	forever {
		parse(getchar());
		if(deep)
			deep = 0;
	}
}

parse(ch)
char ch;
{
	char xscript[MAX], temp[MAX];
	int l, n, key;

	forever {
		if(ch == '\n' || ch == CR || ch == '|') {
			putchar('\n');
			prompt();
			return OK;
		}
		if(ch == CLEARSCREEN) {
			putchar(ch);
			prompt();
			return OK;
		}
		if(ch < ' ' || ch > '~')
			return OK;
		else
			putchar(ch);
		if(isdigit(ch)) {
			sstrcat(number,ch);
			return OK;
		}
		if(!isspace(ch)) {
			sstrcat(keyword,ch);
			return OK;
		}
		if(!isnull(number)) {
			pushnumber();
			return OK;
		}
		else {
			if(isnull(keyword)) /* allows repeat spaces */
				return OK;
			key = isop(keyword);
			strcpy(keyword,"");
			if(!key) {
				calcerror(3);
				return FAIL;
			}
			switch(key) { 
				case 1: /* '=' sign */
					rprintf(stack[i]);
					putchar(' ');
					return OK;
				case 8: /* "clr" */
					i=0;
					return OK;
			}
			if(i<1) {
				calcerror(1);
				return FAIL;
			}
			switch(key) {
				case 2: /* "pgm" */
					i--;
					n=atoi(stack[i+1]);
					if(n<0 || n>9) {
						calcerror(5);
						return FAIL;
					}
					getline(script[n],MAX);
					prompt();
					return OK;
				case 3: /* "exe" */
					i--;
					deep++;
					if(deep > LIMIT) {
						calcerror(4);
						return FAIL;
					}
					n=atoi(stack[i+1]);
					if(n<0 || n>9) {
						calcerror(5);
						return FAIL;
					}
					strcpy(xscript,script[n]);
					for(l=0;l<strlen(xscript);l++)
						parse(xscript[l]);
					return OK;
				case 4: /* "pop" */
					i--;
					return OK;
				case 5: /* "fac" or '!' */
					strcpy(stack[i],facl(stack[i]));
					return OK;
				case 6: /* "sqrt" */
					strcpy(stack[i],sqrtl(stack[i]));
					return OK;
				case 23: /* "sqr" */
					strcpy(stack[i],mull(stack[i],stack[i]));
					return OK;
				case 7: /* "neg" or '~' */
					strcpy(stack[i],smull(stack[i],-1));
					return OK;
				case 9: /* "clt" */
					strcpy(stack[i],"0");
					return OK;
				case 10: /* "abs" */
					strcpy(stack[i],absl(stack[i]));
					return OK;
				case 11: /* "rcl" */
					n=atoi(stack[i]);
					if(n<0 || n>9) {
						calcerror(5);
						i--;
						return FAIL;
					}
					strcpy(stack[i],var[n]);
					return OK;
				case 12: /* "sho" */
					i--;
					n=atoi(stack[i+1]);
					if(n<0 || n>3) {
						calcerror(5);
						return FAIL;
					}
					switch(n) {
						case 1: /* show number stack */
							if(i == 0) {
								rprintf(stack[i]);
								putchar(' ');
								return OK;
							}
							putchar('\n');
							for(l=i;l>0;l--) {
								sprintf(line,"s(%d) %s",l,stack[l]);
								rprintf(line);
								putchar('\n');
							}
							break;
						case 2: /* show variable list */
							putchar('\n');
							for(l=0;l<DEPTH;l++) {
								sprintf(line,"v(%d) %s",l,var[l]);
								rprintf(line);
								putchar('\n');
							}
							break;
						case 3: /* show program list */
							putchar('\n');
							for(l=0;l<DEPTH;l++) {
								sprintf(line,"p(%d) %s",l,script[l]);
								rprintf(line);
								putchar('\n');
							}
							break;
					}
					prompt();
					return OK;
			}
			if(i<2) {
				calcerror(2);
				return FAIL;
			}
			switch(key) {
				case 13: /* "pow" or '^' */
					strcpy(stack[i-1],powl(stack[i-1],stack[i]));
					break;
				case 14: /* "add" or '+' */
					strcpy(stack[i-1],addl(stack[i-1],stack[i]));
					break;
				case 15: /* "sub" or '-' */
					strcpy(stack[i-1],subl(stack[i-1],stack[i]));
					break;
				case 16: /* "mul" or '*' */
					strcpy(stack[i-1],mull(stack[i-1],stack[i]));
					break;
				case 17: /* "div" or '/' */
					strcpy(stack[i-1],divl(stack[i-1],stack[i]));
					break;
				case 18: /* "mod" or '#' */
					strcpy(stack[i-1],modl(stack[i-1],stack[i]));
					break;
				case 19: /* "min" */
					strcpy(stack[i-1],minl(stack[i-1],stack[i]));
					break;
				case 20: /* "max" */
					strcpy(stack[i-1],maxl(stack[i-1],stack[i]));
					break;
				case 21: /* "sto" */
					n=atoi(stack[i-1]);
					if(n<0 || n>9) {
						calcerror(5);
						i-=2;
						return FAIL;
					}
					strcpy(var[n],stack[i]);
					i--;
					break;
				case 22: /* "xch" */
					strcpy(temp,stack[i-1]);
					strcpy(stack[i-1],stack[i]);
					strcpy(stack[i],temp);
					return OK;
			}
			i--;
			return OK;
		}
	}
}

isop(s)
char *s;
{

/*
	no operands required at this level.
*/

	if(iseq(s,"equ") || iseq(s,"="))
		return 1;

/*
	beyond here there must be at least one operand, ie.; i must be > 0.
*/

	if(iseq(s,"pgm"))
		return 2;
	if(iseq(s,"exe"))
		return 3;
	if(iseq(s,"pop"))
		return 4;
	if(iseq(s,"fac") || iseq(s,"!"))
		return 5;
	if(iseq(s,"sqrt"))
		return 6;
	if(iseq(s,"neg") || iseq(s,"~"))
		return 7;
	if(iseq(s,"clr"))
		return 8;
	if(iseq(s,"clt"))
		return 9;
	if(iseq(s,"abs"))
		return 10;
	if(iseq(s,"rcl"))
		return 11;
	if(iseq(s,"sho"))
		return 12;

/*
	beyond here there must be at least two operands, ie. i must be > 1.
*/

	if(iseq(s,"pow") || iseq(s,"^"))
		return 13;
	if(iseq(s,"add") || iseq(s,"+"))
		return 14;
	if(iseq(s,"sub") || iseq(s,"-"))
		return 15;
	if(iseq(s,"mul") || iseq(s,"*"))
		return 16;
	if(iseq(s,"div") || iseq(s,"/"))
		return 17;
	if(iseq(s,"mod") || iseq(s,"#"))
		return 18;
	if(iseq(s,"min"))
		return 19;
	if(iseq(s,"max"))
		return 20;
	if(iseq(s,"sto"))
		return 21;
	if(iseq(s,"xch"))
		return 22;
	if(iseq(s,"sqr"))
		return 23;
	else
		return FALSE;
}
isnull(s)
char *s;
{
	return strlen(s)? FALSE: TRUE;
}

char *sstrcat(s,ch)
char ch, s[];
{
	int i;

	i = strlen(s);
	s[i] = ch;
	s[i+1] = 0;
	return s;
}

rprintf(s)
char *s;
{

#ifdef TVI950 /* if terminal is a televideo 950 */

	putchar(ESC);
	printf("G4%s",s);
	putchar(ESC);
	puts("G0");

#else

	puts(s);

#endif

}

satoi(c)
char c;
{
	return isdigit(c)? c-48: 0;
}

prompt()
{
	printf("[%d] ",i);
}

erase()
{
	putchar(BS);
	putchar(' ');
	putchar(BS);
}

pushnumber()
{
	i++;
	strcpy(stack[i],number);
	strcpy(number,"");
}

calcerror(n)
int n;
{
	switch(n) {
		case 1:
			printf(" missing valid operand\n");
			break;
		case 2:
			printf(" missing second operand\n");
			break;
		case 3:
			printf(" unknow entry\n");
			break;
		case 4:
			printf(" script nesting too deep\n");
			break;
		case 5:
			printf(" operand out of range\n");
			break;
	}
	errnumb = n;
	prompt();
}

iseq(s1,s2)
char *s1, *s2;
{
	if(streq(s1,s2))
		return TRUE;
	lcase(s1);
	if(streq(s1,s2))
		return TRUE;
	else
		return FALSE;
}

lcase(s)
char s[];
{
	int l;

	for(l=0;l<strlen(s);l++)
		s[l] = tolower(s[l]);
}