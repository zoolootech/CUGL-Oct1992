/* CTRLSHOR.C    VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:36:26 PM */
/*
%CC1 $1.C -X -E5000
%CLINK $1 DIO -S
%DELETE $1.CRL 
*/
/* 
Description:

File filter:
	strips high bits
	displays control char (not cr, lf, tab) as "^char".
	
Use to find and display hidden control characters in wordprocessor files, 
	eg wordstar files.

Operates under DIO, for saving output or other redirection of it.

Adapted from Van Nuys Toolkit program CLEAN, by Eugene H. Mallory.

By J.A. Rupley, Tucson, Arizona
Coded for BDS C compiler, version 1.50a
*/
/*********************************************************************
*			CTRLSHOW  by J.A. Rupley, from		     *
**********************************************************************
*                              CLEAN                                 *
**********************************************************************
*                  COPYRIGHT 1983 EUGENE H. MALLORY                  *
*********************************************************************/
#include "BDSCIO.H"
#include "DIO.H"

main(argc, argv)

char **argv;
int argc;

{
	int c, oldc, indicate, col, blankc;
	dioinit(&argc, argv);
	col = 1;
	blankc = oldc = 0;
	/*********************************************************************
	***               ARGUMENT PROCESSING                              ***
	*********************************************************************/

	int ii, jj, optionerr;
	char *ss;
	optionerr = 0;
	indicate = FALSE;

	for (ii = argc - 1; ii > 0; ii--)
		if (argv[ii][0] == '-')
		{
			for (ss = &argv[ii][1]; *ss != '\0';)
			{
				switch (toupper(*ss++))
				{
				case 'H' :
					optionerr = TRUE;
					break;
				default :
					typef("CLEAN: Illegal option %c.\n"
					    , *--ss);
					ss++;
					optionerr = TRUE;
					break;
				}
				while (isdigit(*ss))
					ss++;
			}
			for (jj = ii; jj < (argc - 1); jj++)
				argv[jj] = argv[jj + 1];
			argc--;
		}

	if (optionerr)
	{
		typef("CTRLSHOW: has no legal options\n");
		typef("usage: ctrlshow  <source_file  >destination_file\n");
		typef("		strips high bits\n");
		typef("		converts (ctrl-char) to (^)(char)\n");
		typef("		for use in display of ctrl chararacters\n");
		exit(0);
	}

	/*********************************************************************
	***                  END OF ARGUMENT PROCESSING                    ***
	*********************************************************************/

	while ((c = getchar()) != EOF)
	{
		c &= 0x7f;

		if (c != ' ' && c != '\n' && c != '\r')
			while (blankc > 0 && col < 130)
			{
				putchar(' ');
				blankc--;
				col++;
			}

		if (c == ' ')
			blankc++;
		else
		    if (c == '\t')
			do
			    putchar(' ');
		while (++col % 8 != 1)
			;
		else
		    if (c == '\n')
		{
			blankc = 0;
			putchar(c);
			col = 1;
		}
		else
			if (c == '\r')
			{
				putchar(c);
				blankc = 0;
				col = 1;
			}
			else
				if (c < 0x20)
				{
					putchar('^');
					putchar(c + 0x40);
				}
		else
		    {
			putchar(c);
			col++;
		}
		if (col >= 130)
		{
			putchar('\n');
			col = 1;
			c = '\n';
			blankc = 0;
		}
		oldc = c;
	}
	if (oldc != '\n')
		putchar('\n');
	dioflush();
}
