/* PERCOLAR.C    VERS:- 01.00  DATE:- 10/06/86  TIME:- 05:23:14 PM */
/*
%CC1 $1.C -X -O -E 7000
%CLINK $1 DIO -F FLOAT -S
%DELETE $1.CRL 
*/
/* 
Description:

Simulation of percolation on a two-dimensional square lattice, 
Option to block one or more regions of the lattice to prevent sites 
being filled (approximately circular blocked region = 1,3,5,3,1).
The algorithm is not efficient (300 seconds/1000 site 2-d sheet).

A percolative process is characterized by a percolation transition, a phase
change at which long-range connectivity (e.g., a gel phase; conduction 
across the sample; etc) suddenly appears.  Connectivity is defined by the
extent of a cluster of filled sites adjacent (ie, connected) in the lattice.
Long-range connectivity is equivalent to a cluster of infinite extent.

The percolation model applies to stochastic processes taking place 
in a spatially disordered system.  
Examples: 
	gelation; 
	flow in a porous medium; 
	dilute magnets; 
	conductivity of conductor-insulator composite materials.

For a discussion of percolation, see R. Zallen, "The Physics of Amorphous
Solids", John Wiley, New York, 1983.

This program finds the fractional occupancy of the model lattice at the 
percolation transition.  Lattice sites are filled randomly.  When each filled
site is added to the lattice, connectivity is tested along one dimension
of the lattice (ie, between one pair of arbitrarily chosen sides of the 
lattice).  The point of the percolation transition is the point at which
connectivity between sides first appears.

Because of the stochastic character of percolation, a simulation consists
of the averaging of NRPT determinations of the percolation transition.

The program is set for looping over the simulation, with change in
control parameters at each loop.

Requires sqrt() function.

By J.A. Rupley, Tucson, Arizona
Coded for BDS C compiler, version 1.50a
*/

#include "jario.h"
#include "bdscio.h"
#include "dio.h"

/*CHANGE TO FALSE IF SYSTEM NOT CONFIGURED FOR HAYES MODEM AS PUNCH-READER*/
#define HAYES_MODEM FALSE

#DEFINE ARRAY_DIM 75
#DEFINE NROW  10
#DEFINE NCOL  10
#DEFINE NRPT  20
#DEFINE NBLOCKED 0
#DEFINE NO_PRINT 2

#DEFINE PTS_BLOCKED 13		/*reset if change blocking pattern*/

#DEFINE FILLED '+'
#DEFINE BLOCKED 'B'
#DEFINE TRAIL 'O'
#DEFINE BLANK ' '

#DEFINE ROW_PATTERN 2;
#DEFINE COL_PATTERN 7;
#DEFINE CIRCLE TRUE;
#DEFINE BAR !CIRCLE;

int sum[ARRAY_DIM];
char mast_array[ARRAY_DIM][ARRAY_DIM];
char work_array[ARRAY_DIM][ARRAY_DIM];

int cnt_crit[100], cnt_blocked[100];

char del_i[8], del_j[8];
char test_block[30];
char str_buf[80];
char title[160];

int xnrpt, xnrow, xncol;
int nrpt, nrpt_1;
int nsites, nsites_blocked;
int no_print;
int nblocked;
int circle, bar, row_pattern, col_pattern;

char i_last, j_last, k_last;

int block_sum, crit_sum;
char fcrit_avg[5], fcrit_var[5], fcrit_1var[5];
char fract_avg[5], fract_var[5], fract_1var[5];
char fbloc_avg[5], fbloc_var[5], fbloc_1var[5];
char frat1_avg[5], frat1_var[5], frat1_1var[5];
char frat2_avg[5], frat2_var[5], frat2_1var[5];
char funblocked[5], fnrpt[5], fnrpt_1[5], ftemp[5], fsites[5], fsqd[5];

char i, j, k, ii, jj, kk, ll;
int count;
char pspecial;
int s1, s2, s3;
char clock_in[12];

/*page eject*/


main(argc, argv)
int argc;
char **argv;
{
	dioinit(&argc, argv);
	/*PSPECIAL - LOOP OVER VARIED PARMS*/
	/*SETUP*/
	for (pspecial = 0; pspecial < 100; pspecial++)
	{
		switch (pspecial)
		{
		case 0 :        /*case 0 used for setup*/
			setup();
			/*insert title(up to 2 lines)*/
			strcpy(title,
			"SIMULATION OF PERCOLATION ON A TWO-DIMENSIONAL SQUARE LATTICE"
			    );
			strcat(title,
			""
			    );
			printf("\n%s\n", title);
			read_variables();        /*********optionally delete*/
			continue;
		case 1 :        /*case 1 to 99 = specials*/
			circle = FALSE;
			bar = !circle;
			break;
		default :
			dioflush();
			exit();
		}

		nsites = xnrow * xncol;
		header_disp();

		/*MAIN LOOP*/
		/*repeat determination of critical count */
		/*to obtain the average*/
		for (nrpt = 0; nrpt < xnrpt; nrpt++)
		{
			/*initialize mast_array */
			/*with optional blocking of sites*/
			init_array();

			/*SUB LOOP*/
			/*start of cycle of filling sites,*/
			/*until have connection top-to-bottom*/
			/*the critical count is the number filled*/
			/*at the point of first connection*/
			while (TRUE)
			{
				/*find unfilled site, by random search*/
				i = (nrand(1) % xnrow) + 1;
				j = (nrand(1) % xncol) + 1;
				if (mast_array[i][j] != BLANK)
					continue;
				/*add new site to the master array*/
				i_last = i;
				j_last = j;
				mast_array[i][j] = FILLED;
				if (exit_test())
					array_disp();
				/*if connection not possible,*/
				/*go fill another site*/
				if ((cnt_crit[nrpt] = sum_filled(mast_array)) == 0)
					continue;

				/*test for connection*/
				/*if no connection, go fill another site*/
				copy_array(mast_array, work_array);
				if (!work_test())
					continue;

				/*EXIT SUBLOOP*/
				break;
			}

			/*display mast_array and work_array,*/
			/*side by side*/
			/*display blocked sites, if present*/
			/*display cnt_crit values and their*/
			/*average and variance*/
			if (nrpt < no_print)
			{
				array_disp();
				crit_calc();
				crit_disp();
			}
			else
				printf("working on cycle %d\n", (nrpt + 1));
		}
		/*ALL DONE--TERMINATION OF MAIN LOOP*/
		prog_exit();
	}
	/*END OF PSPECIAL LOOP*/
}
/* END OF MAIN				*/
/*page eject*/


int read_variables()
{
	printf
	    ("\nDo you want to change values of program variables? (y/cr=>default): ");
	getline(str_buf, 2);
	if ((str_buf[0] != 0x79) && (str_buf[0] != 0x59))
	{
		printf("\n");
		return;
	}
	printf
	    ("\nNew title? \n*:");
	getline(str_buf, 79);
	if (strlen(str_buf))
	{
		strcpy(title, str_buf);
		printf("     Another title line?\n*:");
		getline(str_buf, 79);
		if (strlen(str_buf))
			strcat(title, "\n");
		strcat(title, str_buf);
	}
	printf("\n");
	read_val(
	"How many array rows? (value/cr=>no change = %d): ",
	&xnrow);
	printf("\n");
	read_val(
	"How many array cols? (value/cr=>no change = %d): ",
	&xncol);
	printf("\n");
	read_val(
	"How many blocked regions? (value/cr=>no change = %d): ",
	&nblocked);
	printf("\n");
	read_val(
	"How many repeat determinations? (value/cr=>no change = %d): ",
	&xnrpt);
	printf("\n");
	read_val(
	"How many cycles do you want printed? (value/cr=>no change = %d): ",
	&no_print);

	printf("\n\n");
	return;
}
/*END OF READ_VARIABLES			*/



int read_val(string, val)
char *string;
int *val;
{
	while (TRUE)
	{
		printf(string, *val);
		if (strlen(gets(str_buf)) == 0)
			return;
		*val = atoi(str_buf);
	}
	return;
}
/*END OF READ_VAL			*/



int setup()
{
	/* set default values of xnrow, etc*/
	xnrow = NROW;
	xncol = NCOL;
	xnrpt = NRPT;
	nblocked = NBLOCKED;
	no_print = NO_PRINT;
	nsites_blocked = PTS_BLOCKED;
	row_pattern = ROW_PATTERN;
	col_pattern = COL_PATTERN;
	circle = CIRCLE;
	bar = BAR;

	/*establish kernel for random number generator*/
	read_clock();
	nrand(-1, s1, s2, s3);

	/*fill arrays for choosing step (square lattice) in work_test()*/
	initb(del_i, "-1,0,1,0,-1,0,1,0");
	initb(del_j, "0,-1,0,1,0,-1,0,1");

	/*fill array used to set block of points*/
	/*to be excluded from the fitting*/
	initb(&test_block[0], "0,0,1,0,0");
	initb(&test_block[5], "0,1,1,1,0");
	initb(&test_block[10], "1,1,1,1,1");
	initb(&test_block[15], "0,1,1,1,0");
	initb(&test_block[20], "0,0,1,0,0");
	for (i = 0; i < 25; i++)
		if (test_block[i])
			test_block[i] = BLOCKED;
		else
			test_block[i] = BLANK;

	return;
}
/*END OF SETUP				*/



#if HAYES_MODEM
int read_clock()
/*set kernel for random number generator based on read of time/date/day*/
{
	/* Clock_in gives hhmmssx	*/
	hayes_chrono("RT\r", clock_in);
	s1 = atoi(&clock_in[2]);
	printf("\nTime, date and day =  %s ", clock_in);

	/* Clock_in gives yymmdd	*/
	hayes_chrono("RD\r", clock_in);
	s2 = atoi(&clock_in[2]);
	printf("%s ", clock_in);

	/* Clock_in gives  d = acsii #	*/
	hayes_chrono("RW\r", clock_in);
	s3 = atoi(clock_in);
	printf("%s \n", clock_in);

	/*set generator*/

	return;
}
/*END OF READ_CLOCK			*/



#define PUNCH 6
#define READER 7
#define CR 0x0d

int hayes_chrono(command, buffer)
/* Send command to clock and return with string*/
char *command, *buffer;
{
	strcpy(str_buf, "AT");
	strcat(str_buf, command);        /* Form command */

	for (i = 0; str_buf[i] != NULL; bios(PUNCH, str_buf[i++]))
		;

	for (i = 0; i < 12; i++)
		if ((str_buf[i] = bios(READER, 0)) == CR)
			break;
	str_buf[i] = '\0';

	strcpy(buffer, str_buf);
	return;
}
/* END OF HAYES_CHRONO	*/



#else
void read_clock()
{
	printf("\n\nPlease hit three keys, not too quickly, to seed random number generator\n\n");
	s1 = s2 = s3 = 0;
	while (!bios(2, 0))
		s1++;
	bios(3, 0);
	while (!bios(2, 0))
		s2++;
	bios(3, 0);
	while (!bios(2, 0))
		s3++;
	while (bios(2, 0))
		bios(3, 0);
}
/* END OF "READ_CLOCK" FOR SYSTEM WITHOUT HAYES MODEM*/
#endif



int init_array()
{
	/*loop until success or 100 times*/
	ll = 100;
	while (ll--)
	{
		/*zero arrays*/
		for (i = 0; i < ARRAY_DIM; i++)
		{
			sum[i] = 0;
			for (j = 0; j < ARRAY_DIM; j++)
				mast_array[i][j] = work_array[i][j] = BLANK;
		}

		/*exclude block of sites*/
		/*count excluded sites*/
		cnt_crit[nrpt] = cnt_blocked[nrpt] = 0;
		if (!nblocked)
			return;
		for (k = 0; k < nblocked; k++)
		{
			while (bar)
			{
				i = (nrand(1) % xnrow) + 1;
				j = (nrand(1) % xncol) + 1;
				if ((i > 0) && (i < (xnrow - row_pattern + 2))
				    && (j > 0)
				    && (j < (xncol - col_pattern + 2)))
				{
					for (ii = (i); ii < (i + row_pattern); ii++)
						for (jj = (j); jj < (j + col_pattern); jj++)
						{
							if (mast_array[ii][jj] != BLOCKED)
							{
								mast_array[ii][jj] = BLOCKED;
								cnt_blocked[nrpt]++;
							}
						}
					break;
				}
			}
			while (circle)
			{
				i = (nrand(1) % xnrow) + 1;
				j = (nrand(1) % xncol) + 1;
				if ((i > 2) && (i < xnrow - 1)
				    && (j > 2) && (j < xncol - 1))
				{
					kk = 0;
					for (ii = (i - 2); ii < (i + 3); ii++)
						for (jj = (j - 2); jj < (j + 3); kk++, jj++)
							if ((mast_array[ii][jj] != BLOCKED)
							    && (test_block[kk] == BLOCKED))
							{
								mast_array[ii][jj] = BLOCKED;
								cnt_blocked[nrpt]++;
							}
					break;
				}
			}
		}
		/*test for impermeable barrier to connection*/
		for (i = 1; i < (xnrow + 1); i++)
			for (j = 1; j < (xncol + 1); j++)
				if (mast_array[i][j] != BLOCKED)
					work_array[i][j] = FILLED;
		if (work_test())
			return;
	}
	printf("\nFailure in array initialization\n");
	prog_exit();
	dioflush();
	exit();
}
/*END OF INIT_ARRAY			*/



int exit_test()
{
	if (kbhit())
	{
		getline(str_buf, 2);
		printf("\ntype x or X to exit or any other character to continue: ");
		getline(str_buf, 2);
		if ((str_buf[0] == 0x58) || (str_buf[0] == 0x78))
		{
			prog_exit();
			dioflush();
			exit();
		}
		return TRUE;
	}
	return FALSE;
}
/*END OF EXIT_TEST			*/



int prog_exit()
{
	header_disp();
	nrpt = nrpt - 1;
	crit_calc();
	crit_disp();
	printf("\n\n\n\f");
	return;
}
/*END OF PROG_EXIT			*/



int header_disp()
{
	printf("\n\n%s\n", title);
#if HAYES_MODEM
	read_clock();
#endif
	printf
	    ("\nSimulation of percolation on a two-dimensional square lattice.");
	printf
	    ("\nThe array size is %d rows by %d columns, %d sites total.",
	xnrow, xncol, nsites);
	printf
	    ("\nThere are %d regions of %d sites blocked from filling.\n",
	nblocked, nsites_blocked);
	return;
}
/*END OF HEADER_DISP			*/



int array_disp()
/*display master array and working array side-by-side, if possible*/
/*display blocked sites, in mast_array*/
/*display trail of the work_test pointer, in work_array*/
/*display coordinates of last site filled*/
{
	printf("\n");
	for (i = 1; i < (xnrow + 1); i++)
	{
		printf("\n");
		for (j = 1; j < (xncol + 1); j++)
			printf("%c", mast_array[i][j]);
		if (xncol < 38)
		{
			printf("   ");
			for (j = 1; j < (xncol + 1); j++)
				printf("%c", work_array[i][j]);
		}
	}
	if (xncol > 37)
	{
		printf("\n\n\n");
		for (i = 1; i < (xnrow + 1); i++)
		{
			printf("\n");
			for (j = 1; j < (xncol + 1); j++)
				printf("%c", work_array[i][j]);
		}
	}
	printf("\n\n nrpt=%d   cnt_crit[nrpt]=%d   ",
	nrpt, cnt_crit[nrpt]);
	printf("          i_last=%d   j_last=%d\n", i_last, j_last);
	return;
}
/* END OF ARRAY_DISP			*/



int crit_disp()
/*display critical counts and fractions--averages and variances*/
{
	char fpno1[5], fpno2[5];
	int sign;
	char *sqrt();

	if (nblocked)
	{
		printf("\nList of all counts of blocked sites\n");
		for (i = 0; i < (nrpt + 1); i++)
			printf("%4d%c", cnt_blocked[i],
			((i % 10) == 9 || i == nrpt) ? '\n' : ' ');
	}

	printf
	    ("\nList of all counts of filled sites at the critical point\n");
	for (i = 0; i < (nrpt + 1); i++)
		printf("%4d%c", cnt_crit[i],
		((i % 10) == 9 || i == nrpt) ? '\n' : ' ');

	printf(
	"\n\n   critical count =%10.2f     sample_sd=%10.2f   avg_sd=%10.2f",
	fcrit_avg,
	sqrt(fpno1, &sign, fcrit_var),
	sqrt(fpno2, &sign, fcrit_1var));
	printf(
	"\n   critical/total =%10.5f     sample_sd=%10.7f   avg_sd=%10.7f",
	fract_avg,
	sqrt(fpno1, &sign, fract_var),
	sqrt(fpno2, &sign, fract_1var));

	if (nblocked)
	{
		printf(
		"\n\n   blocked sites  =%10.2f     sample_sd=%10.2f   avg_sd=%10.2f",
		fbloc_avg,
		sqrt(fpno1, &sign, fbloc_var),
		sqrt(fpno2, &sign, fbloc_1var));
		printf(
		"\n   blocked/total  =%10.5f     sample_sd=%10.7f   avg_sd=%10.7f",
		frat1_avg,
		sqrt(fpno1, &sign, frat1_var),
		sqrt(fpno2, &sign, frat1_1var));
		printf(
		"\n   critical/unblkd=%10.5f     sample_sd=%10.7f   avg_sd=%10.7f",
		frat2_avg,
		sqrt(fpno1, &sign, frat2_var),
		sqrt(fpno2, &sign, frat2_1var));
	}
	printf("\n\n\n");
	return;
}
/* END OF CRIT_DISP			*/



int crit_calc()
/*calculate averages and variances of critical counts and fractions*/
{
	nrpt_1 = nrpt + 1;
	itof(fnrpt, nrpt);
	itof(fnrpt_1, nrpt_1);
	itof(fsites, nsites);
	fpmult(fsqd, fsites, fsites);

	crit_sum = 0;
	for (i = 0; i < nrpt + 1; i++)
		crit_sum = crit_sum + cnt_crit[i];

	itof(fcrit_avg, crit_sum);
	fpdiv(fcrit_avg, fcrit_avg, fnrpt_1);
	atof(fcrit_var, "0");
	if (nrpt > 0)
		for (i = 0; i < (nrpt + 1); i++)
		{
			itof(ftemp, cnt_crit[i]);
			fpsub(ftemp, ftemp, fcrit_avg);
			fpmult(ftemp, ftemp, ftemp);
			fpadd(fcrit_var, ftemp, fcrit_var);
		}
	fpdiv(fcrit_var, fcrit_var, fnrpt);
	fpdiv(fcrit_1var, fcrit_var, fnrpt_1);
	fpdiv(fract_avg, fcrit_avg, fsites);
	fpdiv(fract_var, fcrit_var, fsqd);
	fpdiv(fract_1var, fcrit_1var, fsqd);

	/*calculations if blocked regions present*/
	if (!nblocked)
		return;
	block_sum = 0;
	for (i = 0; (i < (nrpt + 1)) && (cnt_blocked[i] != 0); i++)
		block_sum = cnt_blocked[i] + block_sum;

	itof(fbloc_avg, block_sum);
	fpdiv(fbloc_avg, fbloc_avg, fnrpt_1);
	atof(fbloc_var, "0");
	if (nrpt > 0)
		for (i = 0; i < (nrpt + 1); i++)
		{
			itof(ftemp, cnt_blocked[i]);
			fpsub(ftemp, ftemp, fbloc_avg);
			fpmult(ftemp, ftemp, ftemp);
			fpadd(fbloc_var, ftemp, fbloc_var);
		}
	fpdiv(fbloc_var, fbloc_var, fnrpt);
	fpdiv(fbloc_1var, fbloc_var, fnrpt_1);

	fpdiv(frat1_avg, fbloc_avg, fsites);

	fpdiv(frat1_var, fbloc_var, fsqd);
	fpdiv(frat1_1var, fbloc_1var, fsqd);

	fpsub(funblocked, fsites, fbloc_avg);
	fpdiv(frat2_avg, fcrit_avg, funblocked);

	fpmult(frat2_var, frat2_avg, frat2_avg);
	fpmult(frat2_var, frat2_var, fbloc_var);
	fpadd(frat2_var, frat2_var, fcrit_var);
	fpdiv(frat2_var, frat2_var, funblocked);
	fpdiv(frat2_var, frat2_var, funblocked);

	fpmult(frat2_1var, frat2_avg, frat2_avg);
	fpmult(frat2_1var, frat2_1var, fbloc_1var);
	fpadd(frat2_1var, frat2_1var, fcrit_1var);
	fpdiv(frat2_1var, frat2_1var, funblocked);
	fpdiv(frat2_1var, frat2_1var, funblocked);

	return;
}
/*END OF CRIT_CALC			*/


int sum_filled(array)
char array[ARRAY_DIM][ARRAY_DIM];
/*sum filled sites of array, by row*/
/*return 0 if any row is zero (which means there can be no connection
	between top and bottom edges (rows))*/
/*else return count of all filled sites in array*/
{
	count = 0;
	for (i = 1; i < (xnrow + 1); i++)
	{
		sum[i] = 0;
		for (j = 1; j < (xncol + 1); j++)
			if (array[i][j] == FILLED)
				sum[i] = sum[i] + 1;
		if (sum[i] == 0)
			return 0;
		count = count + sum[i];
	}
	return count;
}
/* END OF SUM_FILLED			*/



int copy_array(source, dest)
char source[ARRAY_DIM][ARRAY_DIM], dest[ARRAY_DIM][ARRAY_DIM];
/*copy only filled sites, blank others, ie the blocked sites*/
{
	for (i = 0; i < (xnrow + 2); i++)
		for (j = 0; j < (xncol + 2); j++)
			if (source[i][j] == FILLED)
				dest[i][j] = FILLED;
			else
				dest[i][j] = BLANK;
	return;
}
/*END OF COPY_ARRAY			*/



int work_test()
/*move a pointer around the outside left edge of each sub-pattern*/
/*starting at a filled site on the top row*/
/*return true if reach bottom row, return false if no connection*/
/*the arrays i_del and j_del hold a sequence of trial moves*/
/*that maintains the pointer on the outside left edge of a pattern*/
/*the pointer should either reach the bottom row or return to the top row*/
/*if it returns to the top row, the search along the top edge continues*/
/*mark trail of pointer with a string of TRAIL values*/
{
	for (j = 1; j < (xncol + 1); j++)
	{
		if (work_array[1][j] == BLANK || work_array[2][j] == BLANK)
			continue;
		k = 2;
		i = 2;
		work_array[1][j] = TRAIL;
		work_array[2][j] = TRAIL;
		while (i > 1)
		{
			if ((k_last = k % 4) == 0)
				k_last = 4;
			for (k = (k_last - 1); k < (k_last + 3); k++)
			{
				ii = i + del_i[k];
				jj = j + del_j[k];
				if (work_array[ii][jj] != BLANK)
				{
					i = ii;
					j = jj;
					work_array[i][j] = TRAIL;
					break;
				}
			}
			if (i == xnrow)
				return TRUE;
		}
	}
	return FALSE;
}
/*END OF WORK_TEST			*/



