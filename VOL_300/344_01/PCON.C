/* HEADER:       (cat #);
   TITLE:        Utility to send multiple printer control codes;
   DATE:         06/08/1989;
   DESCRIPTION:  "Sends a series of printer control codes (entered on the
                 command line). Initially set up for Epson type printers,
                 the control codes can be easily modified by changing 2
                 tables in the header file pcon.h. One table is for control 
                 code data, the other is for on-screen help";
   KEYWORDS:     escape sequences, printer control, pcon.;
   SYSTEM:       MS-DOS;
   FILENAME:     PCON.C;
   SEE-ALSO:     pcon.h;
   AUTHOR:       Eric Horner;
   COMPILERS:    Turbo C 2.0;
*/


#include <stdio.h>
#include <string.h>
#include "pcon.h"

main(int argc, char *argv[])
{
    int parm, pcnt, flts, codenum;

    if (argc > (MAXPARMS + 1))
    {
	fprintf(stderr, "%s", ers[1]);
	exit(1);
    }
    if (argc > 1)
    {
	for (parm = 1, pcnt = 0; parm <= (argc - 1); parm++)
	{
            strtoupper(argv[parm]);
	    for (codenum = 0; codenum < MAXCODES; codenum++)
	    {
		if (strcmp(argv[parm], codes[codenum * 2]) == 0)
		{
		    fprintf(stdprn, "%s", codes[(codenum * 2) + 1]);
		    fputc(0, stdprn); /* for sequences awaiting null */
		    pcnt++;
		}
	    }
	}
	if((flts =(argc - pcnt -1)) != 0) fprintf(stderr, "\n%d %s", flts, ers[0]);
    }
    else print_instructions();

    exit(0);
}

	/***** print instructions, if no commands given *****/
void print_instructions()
{
    int i;
    for (i = 0; i < TEXTLEN; ++i)
    {
	fprintf(stderr, "%s", explain[i]);
    }
}

	/***** convert a string to all upper case *****/
void strtoupper(char *strptr)
{
    for (; *strptr != '\0'; strptr++)
	if ((*strptr >= 'a') && (*strptr <= 'z')) *strptr -= 32;
}
