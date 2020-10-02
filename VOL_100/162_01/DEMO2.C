/*	file	demo2.c	*/
/*	Returns inverse sine, in radians and degrees,
	or and error message if out of range.	*/
/*	Use CTRL-C to exit from demo.	*/
#include	fprintf.h
#define	EOF -1
#define	MAXL	40
extern	float	asin(),fdpi(),atof();
extern	char	*ftoa();
float	rtod();
extern	int	b34arg;
extern	int	portc,portd;

main() {
	static	float	fnuma,fnumb,fnumc;
	static	int	len;
	static	char	line[MAXL];
	portd = 188; portc = 189;	/* must be YOUR hardware ports */
	while(len = getline(line,MAXL) < MAXL) {
		fnuma = atof(line);
		fnumb = asin(fnuma);
		xpand1();	/* automatically external ? */
		if(b34arg == 1)
	     printf("\n%g is out of range for inverse sine operation\n",fnuma);
		else {
		    printf("\nInverse sine of %g is %f radians, ",fnuma,fnumb);
			fnumc = rtod(fnumb);
			printf("or %f degrees.\n",fnumc);
		}
	}
}

getline(s,lim)	/* get line into s, return length */
char	s[];
int	lim;
{
	int	c,i;
	i = 0;
	while(--lim > 0 && (c=getchar()) != EOF && c != '\n')
		s[i++] = c;
	if (c == '\n')
		s[i++] = c;
	s[i] ='\0';
	return(i);
}
float	rtod(fn)	/*	convert Radians TO Degrees	*/
float	fn;
{
	return(fdpi(fn) * 180);
}
