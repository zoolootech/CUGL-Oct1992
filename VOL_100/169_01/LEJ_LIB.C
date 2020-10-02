/* Larry Jordan's sample c program functions */

#include <stdio.h>				/* std I/O library on c disk */

#define EOF -1					/* logical ^Z EOF ends copy */
#define YES 1
#define NO 0
#define YES 1
#define NO 0
#define NL '\n'
#define EOS '\0'
#define MAXLINE 1024
#define FF '\012'                               /* ASCII Form feed */
#define DIGMAX 10

int inword;
int nl;
int nw;
int nc;
int ndigits[DIGMAX];
int c;




/* ---------------------------------------------------------------------- */


main()				      /*				    */
{

	int len;			/* current line length */
	int max;			/* max length so far */
	char line[MAXLINE];
	char save[MAXLINE];
	char lineupper[MAXLINE];
	char linelower[MAXLINE];

	max = 0;

	printf("\n");
	printf("\n");
	printf("Jordan's function test ...\n");
	printf("\n");
	printf("Input text for test.\n");
	printf("\n");


	while ((len = getline(line, MAXLINE)) > 0)   /* getline defined below */
		printf("\nLine = %s\n", line);



}



/* ---------------------------------------------------------------------- */


redirect()				       /* redirect keybd to CRT */
{
	int c;

	printf("\n");
	printf("\n");
	printf("Jordan's redirect function...\n");
	printf("\n");
	printf("Input numbers, space characters, etc...\n");
	printf("?");


	while ((c = getchar()) != EOF)	       /* get char until EOF found */
		putchar(c);		       /* send char to scrn: */
}


/* ---------------------------------------------------------------------- */



wordcnt()			      /* count lines, words, chars in input */
{

	double c, nc, nl, nw;			      /* counters are float */
	int inword;				      /* logical is interger */

	inword = NO;
	nl = nw = nc = 0.0;			      /* initialize variables */

	printf("\n");
	printf("\n");
	printf("Jordan's word count function ...\n");
	printf("\n");
	printf("Input text for character, word and line count search.\n");
	printf("\n");


	while ((c = getchar()) != EOF) {
		nc = nc + 1;

		if (c == '\n')                         /* if c=LF then inc nl */
		     nl = nl + 1;

		if (c == ' ' || c == '\n' || c == '\t')  /* if char is space, */
		     inword = NO;			/* LF or TAB then it */
							/* cannot be a word */

		else if (inword == NO) {		/* otherwise it is a */
		     inword = YES;			/* word */
		     nw = nw + 1;
		}
	}
	printf("\n");
	printf("\n");
	printf("Number of characters = ");
	printf("%.0f\n",nc);                       /* prt float # with no dec */
	printf("Number of words = ");
	printf("%.0f\n",nw);
	printf("Number of lines = ");
	printf("%.0f\n",nl);

	printf("\n");
}


/* ---------------------------------------------------------------------- */


maxline()			/* gets maximum length line in file */
{
	int len;			/* current line length */
	int max;			/* max length so far */
	char line[MAXLINE];
	char save[MAXLINE];

	max = 0;

	printf("\n");
	printf("\n");
	printf("Jordan's get maximum length line function ...\n");
	printf("\n");
	printf("Input text for maximum line length search.\n");
	printf("\n");


	while ((len = getline(line, MAXLINE)) > 0)   /* getline defined below */
		if (len > max){
			max = len;
			copy(line, save);	       /* function from below */
		}
	if (max > 0)
	    printf("%s", save);
}


/* ---------------------------------------------------------------------- */


getline(s, lim) 		/* getline into s, return length of s */
char s[];
int lim;
{
	int c,i;

	for (i = 0; i < lim-1 && (c = getchar()) != EOF && c != '\n'; ++i)
		s[i] = c;
	if (c == '\n'){
		s[i] = c;
		++i;
	}
	s[i] = '\0';
	return(i);
}



/* ---------------------------------------------------------------------- */


copy(s1, s2)			/* copy s1 to s2; assume s2 big enough */
char s1[], s2[];
{
	int i;

	i = 0;
	while (s2[i] = s1[i] != EOS)
		++i;
}




/* ---------------------------------------------------------------------- */


power(x, n)			    /* raise x to n-th power; n > 0 */
int x, n;
{
	int i, p;

	p = 1;
	for (i=1; i<=n; ++i)
	    p=p*x;
	return(p);
}



/* ---------------------------------------------------------------------- */


digicnt()				 /* count digits, spaces, others */
{
	int c, i, nspaces, nother;	 /* integers */
	int ndigits[DIGMAX];		 /* integer array */

	nspaces = nother = 0;
	for (i= 0; i < 10; ++i)
		ndigits[i] = 0; 	 /* initialize arry to 0 */


	printf("\n");
	printf("\n");
	printf("Jordan's number counter...\n");
	printf("\n");
	printf("Input numbers, space characters, etc...\n");
	printf("?");

	while ((c = getchar()) != EOF)
		if (c >= '0' && c <= '9')
		      ++ndigits[c];
		else if (c == ' ' || c == '\n' || c == '\t')
			++nspaces;
		else
			++nother;



	printf("\n");
	printf("\n");
	printf("Jordan's number counter results...\n");
	printf("\n");
	printf("Number of digits = ");
	for (i=0; i < 10; ++i)
	       printf(" %d", ndigits[i]);
	printf("\nNumber of spaces = %d,  Other = %d\n",
	       nspaces, nother);
	printf("\n");
}



/* ---------------------------------------------------------------------- */

strlen(s)				/*  return length of string s */
char s[];
{
	int i;

	i = 0;
	while (s[i] != '\0')
		++i;
	return(i);
}


/* ---------------------------------------------------------------------- */

atoi(s) 			/* convert numeric string into numeric equiv */
char s[];
{

	int i;
	int n;		      /* need double precision for no > 32K */

	n = 0;
	for (i = 0; s[i] >= '0' && s[i] <= '9'; ++i)
	      n = 10 * n + s[i] - '0';

	return(n);
}


/* ---------------------------------------------------------------------- */




isdigit(c)			/* test character to see if it is a digit */
int c;
{
	int isdigit;

	return(isdigit = c >= '0' && c<= '9');  /* isdigit true = 1 */
						/* isdigit false = 0 */

}


/* ---------------------------------------------------------------------- */
