/* HEADER:       (cat #);
   TITLE:        Tab Insertion Text Filter;
   DATE:         06/07/1989;
   DESCRIPTION:  "Reads an existing text file, formatted with only spaces, and
                 produces a new file, formatted with the optimum combination
                 of spaces and tabs of a specified width. The original file can
                 be produced from a file containing tabs (presumably of a
                 different width) using XTAB.COM.";
   KEYWORDS:     filter, detab, text formatters, tabs, file.;
   SYSTEM:       MS-DOS;
   FILENAME:     ITAB.C;
   SEE-ALSO:     xtab.c, xitab.txt;
   AUTHOR:       Eric Horner;
   COMPILERS:    Turbo C 2.0;
*/

#include <stdio.h>

	/***** error messages *****/

char *ers[] =
{
	"\7\nUnable to open input file!\n",
	"\7\nUnable to open output file!\n",
	"\7\nUsage is: itab infile outfile tabs\n\n(tabs = new tab width).\n",
	"\ninfile must contain no tabs, and can be made using xtab.com!\n"
};	

main(int argc, char *argv[])
{
    int ch, charcnt, spccnt, tabs;
    FILE *infile, *outfile;

    if (argc == 4)
    {
	if ((infile = fopen(argv[1], "r")) == 0)
	{
	    printf("%s", ers[0]);
	    fclose(infile);
	    exit(1);
	}
	if ((outfile = fopen(argv[2], "w")) == 0)
	{
	    printf("%s", ers[1]);
	    fclose(infile);
	    fclose(outfile);
	    exit(1);
	}
	tabs = atoi(argv[3]);		/* get number of spaces per tab */
	charcnt = 0;			/* char count within line       */
	spccnt = 0;			/* space count within tab	*/

	while ((ch = fgetc(infile)) != EOF)
	{
	    switch (ch)
	    {
		case '\x20': ++charcnt;
			     ++spccnt;
			     if (charcnt%tabs == 0)
			     {
				spccnt = 0;
				fputc('\t', outfile);
			     }
			     break;
		    default: if (spccnt > 0)
			     {
				 for(;spccnt > 0; --spccnt)
				 fputc('\x20', outfile);
			     }
			     if ((ch != '\0') && (ch != '\n'))
				 ++charcnt;
			     if (ch == '\n')
				 charcnt = 0;
			     fputc((char) ch, outfile);
			     break;
	    };
	}
    }
    else
    {
	printf("%s", ers[2]);
	printf("%s", ers[3]);
	exit(1);
    }
    fclose(infile);
    fclose(outfile);
}
