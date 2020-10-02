/*
**	WC.C
**	Written by Leor Zolman, 3/16/82
**
**	Modified for Small-C 2.7		by F.A.Scacchitti
**							9/23/84
**
**
**	Text analysis utility. Given a list of text files, WC prints
**	out total number of characters, words and lines in each file
**	and in all files together. "Words", here, are simply delimited
**	by blanks, tabs or newlines.
**
**	Maximum number of words and lines are each 32767, but the char
**	count is virtually unlimited. ( >4E9 )
**
*/

#include <stdio.h>

FILE ibuf;
int  lo_totchars, hi_totchars, totwords, totlines;

main(argc,argv) int argc; char *argv[]; {

	lo_totchars=hi_totchars=totwords=totlines=0;       

	if(argc == 1)
		{
		prntf("\n\nUsage: wc <file1> <file2> . . . <fileN> <CR>\n\n");
		exit();
		}
	else
		prntf("\n\t\tchars\twords\tlines\n\n");

	while (--argc) 

		dofile(*++argv);	/* process the files */


	prntf("\nTotals:");		/* print the results */

	if (hi_totchars) prntf("\t\t%d%04d",hi_totchars,lo_totchars);

	else prntf("\t\t%d",lo_totchars);

	prntf("\t%d\t%d\n",totwords,totlines);
}

dofile(name)
char *name;
{
	char inword;
	int c;
	int  lo_tch, hi_tch, twords, tlines;

	ibuf = (fopen(name,"R"));
	if (ibuf == NULL)
		{
		prntf("Can't open %s\n",name);
		return;
		}
	prntf("%s:\t",name);
	if (strlen(name) < 7)
		putchar('\t');
	
	inword = lo_tch = hi_tch = twords = tlines = 0;

	while ((c =fgetc(ibuf)) != EOF ) {

		if (++lo_tch == 10000) {
			lo_tch = 0;
			hi_tch++;
		}

		if (isspace(c)) {
			if (inword) {
				inword = 0;
				twords++;
			}
		} else
			if (!inword)
				inword = 1;
		
		if (c == '\n')
			tlines++;
	}

	if (hi_tch) prntf("%d%04d",hi_tch,lo_tch);
	else prntf("%d",lo_tch);
	prntf("\t%d\t%d\n",twords,tlines);

	if ((lo_totchars += lo_tch) >= 10000) {
		lo_totchars -= 10000;
		hi_totchars++;
	}
	hi_totchars += hi_tch;
	totwords += twords;
	totlines += tlines;

fclose(ibuf);
}


