/*
	xpnd.c	- expand standard input by specified horizontal
		and vertical expansion factors.  Write the results
		to standard output.
	
		In general, this is a pretty worthless program, but if you
		process an ascii picture file such as the peanuts characters
		that are so popular, or even a nude, the result can be pretty
                amazing. A friend of mine and I xpnd'ed a one page nude to be
		eleven feet tall! Enjoy.

		Marty Chamberlain,	1/7/86
		Hdwr: AT&T 3B1
		Opsy: UNIX Version 5, AT&T release v3.0

*/
#include <stdio.h>
#include <ctype.h>

#define maxline 5000 /* maximum line size (5000 is a bit excessive!) */

main(argc,argv)
  int argc;
  char *argv[];
{
	int horzxpnd,vertxpnd;
	int len;
	int i,j,k;
	char line[maxline];

	if(argc!=3)
	  fprintf(stderr,"xpnd...M. Chamberlain 1986:\nUsage: \
xpnd horizontal vertical\n\n\
where horizontal is horizontal expansion factor and\n\
      vertical is vertical expansion factor\n");
	else
	if((atoi(argv[1])<=0) || (atoi(argv[2])<=0))
	  fprintf(stderr,"xpnd...M. Chamberlain 1986:\n\
Arguments must be positive integers\n");
	else
	  {
	  horzxpnd=atoi(argv[1]);
	  vertxpnd=atoi(argv[2]);
	  while ((len=getline(line,maxline))>0)
	    for (i=0;i<=vertxpnd-1;++i) {
	      for (j=0;j<=len-2;++j)
	        for (k=0;k<=horzxpnd-1;++k)
	          putchar(line[j]);
	      putchar('\n');
	    }
	  }
}

getline(s,lim) /* get a line of input from standard input */
char s[];
int lim;
{
	int c,i;
	for (i=0;i<lim-1 && (c=getchar())!=EOF && c!='\n';++i)
	  s[i]=c;
	if (c=='\n') {
	  s[i]=c;
	  ++i;
	}
	s[i]='\0';
	return(i);
}
