/* HEADER:       (cat #);
   TITLE:        Tab Extraction Text Filter;
   DATE:         06/07/1989;
   DESCRIPTION:  "Reads an existing text file, and replaces <tab> characters
                 with spaces, so as to retain the original formatting. Note
                 that this is NOT the same as replacing <tab> characters
                 with a fixed number of spaces, which destroys the original
                 formatting. The original tab setting value is passed as a
                 command line parameter.";
   KEYWORDS:     filter, detab, text formatters, file.;
   SYSTEM:       MS-DOS;
   FILENAME:     XTAB.C;
   SEE-ALSO:     itab.c, xitab.txt;
   AUTHOR:       Eric Horner;
   COMPILERS:    Turbo C 2.0;
*/

#include <stdio.h>

	/***** error messages *****/

char *ers[] =
{
	"\7\nUnable to open input file!\n",
	"\7\nUnable to open output file!\n",
	"\7\nUsage is: xtab infile outfile tabs\n\n(tabs = spaces per tab).\n"
};	


main(int argc, char *argv[])
{
    int ch, charcnt, tabs, tabcnt;
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

	while ((ch = fgetc(infile)) != EOF)
	{
	    switch (ch)
	    {
		case '\t': tabcnt = (tabs - charcnt%tabs);
			   charcnt += tabcnt;
			   for(;tabcnt > 0; tabcnt--)
			   fputc('\x20', outfile);	/* spaces */
			   break;
		default:   ++charcnt;
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
	exit(1);
    }

    fclose(infile);
    fclose(outfile);
}
