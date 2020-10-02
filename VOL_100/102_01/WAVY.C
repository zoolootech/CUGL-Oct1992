/*
	Wavy Lines for the H19/Z19/H89/Z89
	Written by Leor Zolman, 11/81

	This program is configured for the H19 terminal, but may be used
	on ANY cursor-addressable terminal by:

		a) modifying the first four #define lines to suit your
		   particular terminal, and
		b) modifying the cursor addressing sequence (commented)
		   in the program to work for your terminal.

	For best effect, compile with:
	  A>cc1 wavy.c -e2000 -o <cr>
*/

/* The following four #define statements need customizing for your terminal: */

#define INIT	"\33E\33F\33x5"	/* clear screen, enter graphics mode,	*/
				/* and turn off the cursor		*/
#define UNINIT	"\33E\33G\33y5"	/* clear screen, exit graphics mode,	*/
				/* and turn cursor back on		*/

#define SLASH 'x'		/* these are special characters in H19	*/
#define BACKSLASH 'y'		/* graphics mode. If you don't have an  */
				/* H19, make these simply '/' and '\\'	*/


#define MAXL 200		/* maximum number of lines at one time */

int direc[MAXL], di;
char column[MAXL], co;
char i;
char nlines;

main()
{
top:	puts(UNINIT);
	srand1("How many wavy lines (1-200, q to quit) ? ");
	if (!scanf("%d",&nlines) || nlines < 1 || nlines > MAXL) exit();
	puts(INIT);

	di = rand() % 2 * 2 - 1;

	for (i=0; i<nlines; i++)	/* initialize lines */
	{
		column[i] = rand() % 80;	/* start out at random spot */
		direc[i] = di;			/* going in same direction */
	}

	while (1)
	{
	   putch('\n');
	   if (kbhit() && getchar()) goto top;	/* if key hit, stop	*/

	   for (i = 0; i < nlines; i++)
	   {	
		di = direc[i];
		co = column[i];

		putch('\33');		/* position cursor at row 23, 	*/
		putch('Y');		/* 		      column co	*/
		putch(' '+23);		/*   MODIFY THIS SECTION FOR	*/
		putch(' '+co);		/*     NON-H19 TERMINALS!	*/

		putch ((di < 0) ? SLASH : BACKSLASH);

		if ((di > 0) ? (rand() % (79 - co)) : (rand() % (1 + co)))
			column[i] += di;	/* either keep going, or */
		else
			direc[i] = -di;		/* change direction	*/
	   }
	}
}
