/* HEADER: xxx.yy;
   TITLE: ONED;
   DATE: 4/30/85;
   DESCRIPTION: "One dimensional cellular automata simulation";
   VERSION: 1.0;
   KEYWORDS: Life, cellular automata, games;
   FILENAME: ONED.C;
   CRC: xxxx;
   SYSTEM: CP/M, Osborne 1;
   COMPILERS: C/80;
   AUTHORS: David L. Fox;
   REFERENCES: AUTHORS: A. K. Dewdney;
         TITLE: "Computer Recreations";
         CITATION: "Scientific American, 252, 18-30(May 1985).";
         AUTHORS: S. Wolfram;
         TITLE: "Computer Software in Science and Mathematics";
         CITATION: "Scientific American, 251, 188-203(September 1984).";
         AUTHORS: S. Wolfram;
         TITLE: "";
         CITATION: "Physical Review Letters, 54, #8, 735-739(1985)."
      ENDREF
*/
/* oned.c -- One dimensional cellular automata simulator.

	Copyright 1985 by David L. Fox.
	All rights reserved.
	Permission granted for unlimited
	personal and non-commercial use
	provided that this notice is included.
*/

#include	<stdio.h>
#include	<printf.h>

#define	MAXST	10	/* Maximum number of different states. */
#define	MAXNB	10	/* Maximum number of neighboors (to one side). */
#define	UNIVSIZ	250
#define	SCRNSIZ	80	/* Number of characters in one line of display. */

/* Osborne 1 graphic characters to represent states. */
char stsym[] = {' ', '\0', '\026', '\015', '\014', '5', '6', '7', '8', '9'};
/* Osborne 1 screen controls (emulate Televideo 912 and 920. */
#define	UPLINE	"\013"		/* Move cursor up one line. */
#define	ERALINE	"\033T"		/* Erase from cursor to end of line. */
#define	GPHST	"\033g"		/* Start display of graphics characters. */
#define	GPHEND	"\033G"		/* End display of graphics characters. */

char pgmname[] = "ONED";
char copyright[] = "(c) Copyright 1985 David L. Fox";
char version[] = "1.0";
char univ[2][UNIVSIZ]	/* Arrays for current and next generations. */
	= {0,0,0,0,1,0,0,1,1,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,1,1,0,0,1};
int gen = 0;		/* Generation index in univ. */
int origin = 0;		/* Origin of displayed region. */

extern int (*CtlB)();	/* C/80 vectors here when ^B is pressed. */
int restart();		/* Called on ^B interrupt. */
char env[6];		/* Use	#include <setjmp.h>
				jmp_buf env;
			   with a fully UNIX compatable library. */

main(argc, argv)
int argc;
char **argv;
{	int ngen;	/* Number of generations. */
	int i, j;
	char line[MAXSTR], *s;
	static struct automaton {
		int nstate;	/* Number of possible states for each cell. */
		int neighb;	/* Number of neighbors on one side. */
		int nexts[(MAXST - 1)*(2*MAXNB + 1) + 1];	/* Next state function. */
	} ca = {2, 2, {0,0,1,0,1,0},};		/* r=2, s=2, code 20 automation. */
	char *fgets();

	printf("%s  Version %s\n%s\n", pgmname, version, copyright);
	CtlB = restart;
	setjmp(env);		/* Return here after interrupt. */
	while ((printf(": "), s = fgets(line, MAXSTR, stdin)) != NULL) {
		switch(*s) {
		case 'a':	/* Modify automaton. */
			adisp(&ca);
			break;
		case 'd':	/* Show current state of universe. */
			printf(UPLINE);
			dispst(univ[gen]);
			break;
		case 'e':	/* Edit current state of universe. */
			ued();
			break;
		case 'n': ++s;	/* Do n generations. */
			printf(UPLINE);
			ngen = getval(&s);
			evolve(ngen, &ca);
			break;
		case 'o':	/* Shift origin. */
			switch(*++s) {
			case '=': origin = (getval(&s) + UNIVSIZ) % UNIVSIZ; break;
			case '+': ++s; origin += getval(&s) % UNIVSIZ; break;
			case '-': ++s; origin -= (getval(&s) - UNIVSIZ) % UNIVSIZ; break;
			}
			break;
		case '\n':
			printf(UPLINE);
			evolve(1, &ca); 
			break;
		case 'r':
			++s;
			if ((ca.neighb = getval(&s)) > MAXNB)
				ca.neighb = MAXNB;
			break;
		case 's':
			++s;
			if ((ca.nstate = getval(&s)) > MAXST)
				ca.nstate = MAXST;
			break;
		case 't':
			if (*++s == '=') {	/* Define transition table by t=code */
				i = getval(&s);
				for (j = 0; j <= (ca.nstate - 1)*(2*ca.neighb + 1) + 1; ++j) {
					ca.nexts[j] = i % ca.nstate;
					i /= ca.nstate;
				}
			}
			else {	/* Set members of transition table individually: txx=yy */
				do {
					if ((i = getval(&s)) > (ca.nstate-1)*(2*ca.neighb+1)) {
						printf("total too large\n");
						i = getval(&s);
						continue;
						}
					if ((j = getval(&s)) < ca.nstate)
						ca.nexts[i] = j;
					else
						printf("illegal state\n");
				} while (*s);
			}
			break;
		case 'x': exit();
		default: printf("Unknown command: %s\n", s);
		case '?': printf("Command summary\n\n  ?\t\tDisplay this message.\n");
			printf("  a\t\tDisplay automaton\n");
			printf("  d\t\tDisplay state of universe.\n");
			printf("  e\t\tEdit current state.\n");
			printf("  n=xx\t\tCompute xx generations.\n");
			printf("  o=xx\t\tSet display origin to xx.\n");
			printf("  o+xx\t\tIncrease display origin to curent value + xx.\n");
			printf("  o-xx\t\tDecrease display origin to curent value - xx.\n");
			printf("  r=xx\t\tSet size of neighborhood.\n");
			printf("  s=xx\t\tSet number of states.\n");
			printf("  t=code\tTransition table given by code.\n");
			printf("  txx=yy\tSet next state = yy for total xx.\n");
			printf("  x\t\tExit to CP/M.\n");
		}
	}
}

/* getval -- Convert command string to a integer,
	     on exit strp contains pointer to next character to be processed. */
getval(strp)
char **strp;
{	char *s;
	int n;

	for (s = *strp; *s == '=' || *s == ',' || isspace(*s); ++s)
		;	/* Skip separators. */
	n = atoi(s);
	while (isdigit(*s))
		++s;
	*strp = s;
	return(n);
}

/* evolve -- Compute next n generations of cellular automaton. */
evolve(n, ca)
int n;
struct automation *ca;
{	int i;

	for (i=0; i < n; ++i) {
		dogener(univ, ca);
		gen = 1-gen;
		dispst(univ[gen]);
	}
}

/* dogener -- Compute next generation. */
dogener(univ, ca)
char univ[2][UNIVSIZ];
struct automaton *ca;
{	static int i, j, total;		/* Static for speed. */
	static char *uvp;

	CtlCk();	/* C/80 routine to check for control characters. */
	uvp = univ[gen];
	for (i=0; i < UNIVSIZ; ++i) {
		total = uvp[i];
		for (j=1; j <= ca->neighb; ++j) {
			total += ((i - j < 0) ? uvp[i-j+UNIVSIZ] : uvp[i-j]) +
			       ((i + j >= UNIVSIZ) ? uvp[i+j-UNIVSIZ] : uvp[i+j]);
		}
		univ[1-gen][i] = ca->nexts[total];
	}
}

/* dispst -- Display state of the universe. */
dispst(univ)
char univ[UNIVSIZ];
{	int i;
	char c;

	printf("%s", GPHST);	/* Enter graphic mode. */
	for (i=origin; i < origin+SCRNSIZ; ++i) {
		putchar(stsym[univ[i % UNIVSIZ]]);
		if (c = bdos(6, 0xff)) {	/* Check for a character from keyboard. */
			switch(c) {
			case '\014': case '\04':	/* Move display to right. */
				origin = (origin + 1)%UNIVSIZ;
				break;
			case '\b': case '\023':	/* Move display to left. */
				origin = (origin - 1 + UNIVSIZ)%UNIVSIZ;
				break;
			}
		}
	}
	printf("%s\n", GPHEND);	/* Exit graphic mode, new line. */
}

/* ued -- Edit the current state of the universe. */
ued()
{	int curpos, i, c;
	extern char Cmode;

	Cmode = 0;	/* Set char at a time input for C/80. */
	curpos = 0;
	printf("%s%s",UPLINE, UPLINE);	/* Move up two lines. */
	dispst(univ[gen]);
	while (1) {
		putchar('\r');		/* Return to left margin. */
		for (i=0; i < curpos; ++i)
			putchar(' ');
		printf("^%s\b",ERALINE);	/* Display cursor, erase to end of line. */
		switch(c = getchar()) {
		case '\b':
		case '\023':
			curpos = (curpos - 1 + UNIVSIZ)%UNIVSIZ;
			break;
		case '\014':
		case '\04':
			curpos = (curpos + 1)%UNIVSIZ;
			break;
		case ' ':	/* Special case: blank = state 0. */
			c = '0';
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			univ[gen][(curpos+origin)%UNIVSIZ] = c - '0';
			printf("\r%s",UPLINE);
			dispst(univ[gen]);
			break;
		case 'z':	/* Zap universe. */
			printf("\nZero entire universe (y/n)? ");
			if ((c = toupper(getchar())) == 'Y')
				for (i=0; i < UNIVSIZ; ++i)
					univ[gen][i] = 0;
			printf("\r%s%s",UPLINE, UPLINE);
			dispst(univ[gen]);
			printf("\n%s%s",ERALINE, UPLINE);	/* Erase message. */
			break;
		case '\n':
			printf(UPLINE);
		default:
			printf("\r%s", ERALINE);	/* Erase cursor line. */
			Cmode = 1;	/* Restore line at a time input for C/80. */
			return;
		}
	}
}

/* adisp -- Display properties of automaton. */
adisp(ca)
struct automaton *ca;
{	int i;

	printf("origin=%d\n", origin);
	printf("states=%d\nneighboors=%d\ntotal\tnext state\n", ca->nstate, ca->neighb);
	for (i = 0; i < (ca->nstate - 1)*(2*ca->neighb + 1) + 1; ++i) {
		printf("%d\t%d\n",i, ca->nexts[i]);
	}
}

/* restart -- Control B vectors through CtlB to here. */
restart()
{	longjmp(env, 1);	/* Restart program. */
}

#include	"b:onedlib.c"
#include	<stdlib.c>
