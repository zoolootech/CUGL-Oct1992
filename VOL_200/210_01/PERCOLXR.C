/* PERCOLXR.C    VERS:- 01.00  DATE:- 10/06/86  TIME:- 05:24:12 PM */
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

J.A. Rupley, Tucson, Arizona
Coded for the Eco-C Compiler, version 3.40
*/

#include <stdio.h>
#include <ctrlcnst.h>

/*CHANGE TO FALSE IF SYSTEM NOT CONFIGURED FOR HAYES MODEM AS PUNCH-READER*/
#define HAYES_MODEM FALSE

#define ARRAY_DIM 75
#define NROW  10
#define NCOL  10
#define NRPT  20
#define NBLOCKED 0
#define NO_PRINT 20

#define PTS_BLOCKED 13		/*reset if change blocking pattern*/

#define FILLED '+'
#define BLOCKED 'B'
#define TRAIL 'O'
#define BLANK ' '

int sum[ARRAY_DIM];
char mast_array[ARRAY_DIM][ARRAY_DIM];
char work_array[ARRAY_DIM][ARRAY_DIM];

int cnt_crit[100], cnt_blocked[100];

char del_i[] = {
	-1,0,1,0,-1,0,1,0
};
char del_j[] = {
	0,-1,0,1,0,-1,0,1
};
char test_block[] = {
	0,0,1,0,0,
	0,1,1,1,0,
	1,1,1,1,1,
	0,1,1,1,0,
	0,0,1,0,0
};

char str_buf[80];
char title[160];

int xnrpt, xnrow, xncol;
int nrpt, nrpt_1;
int nsites¬ nsite_blocked;
int no_print;
int nblocked;

char i_last, j_last, k_last;

int block_sum, crit_sum;

double fcrt_avg, fcrt_var, fcrt_1var;
double frct_avg, frct_var, frct_1var;
double fblc_avg, fblc_var, fblc_1var;
double frt1_avg, frt1_var, frt1_1var;
double frt2_avg, frt2_var, frt2_1var;
double fnrpt, fnrpt_1, funblocked, fsites; 

char i,j,k,ii,jj,kk,ll;
int count;
int pspecial;
int s1, s2, s3;
char clock_in[12];

void srand(), printf();
int getchar(), strlen(), atoi(), irand(), bios();
char *strcpy(), *strcat();

/*page eject*/


main(argc,argv)
int argc;
char **argv;
{
				/*PSPECIAL - LOOP OVER VARIED PARMS*/
				/*SETUP*/
for (pspecial = 0; pspecial < 100; pspecial++) { 
	switch (pspecial) {
		case 0 :			/*case 0 used for setup*/
			setup();
			strcpy(title,
"SIMULATION OF PERCOLATION ON A TWO-DIMENSIONAL SQUARE LATTICE"
				);
			strcat(title,
""
				);		/*********insert title******/
			printf("\n%s\n", title);
			read_variables();	/*********optionally delete*/
			continue;
		case 1 :			/*case 1 to 99 = specials*/
			break;
		default:
			exit();
	}

nsites = xnrow * xncol;
header_disp();

				/*MAIN LOOP*/
				/*repeat determination of critical count 
					to obtain the average*/
for (nrpt = 0; nrpt < xnrpt; nrpt++) {
				/*initialize mast_array 
					with optional blocking of sites*/
	init_array();

				/*SUB LOOP*/
				/*start of cycle of filling sites, 
					until have connection top-to-bottom*/
				/*the critical count is the number filled 
					at the point of first connection*/
	while(TRUE) {
				/*find unfilled site, by random search*/
		i = (irand() % xnrow) + 1;
		j = (irand() % xncol) + 1;
		if (mast_array[i][j] != BLANK)
			continue;
				/*add new site to the master array*/
		i_last = i;
		j_last = j;
		mast_array[i][j] = FILLED;
		if (exit_test()) array_disp();
				/*if connection not possible, 
					go fill another site*/
		if((cnt_crit[nrpt] = sum_filled(mast_array)) == 0)
			continue;

				/*test for connection*/
				/*if no connection, go fill another site*/
		copy_array(mast_array, work_array);
		if (!work_test())
			continue;

				/*EXIT SUBLOOP*/
		break;
	}

				/*display mast_array and work_array, 
					side by side*/
				/*display blocked sites, if present*/
				/*display cnt_crit values and their 
					average and variance*/
	if (nrpt < no_print) {
		array_disp();
		crit_calc();
		crit_disp();
	}
	else
		printf("working on cycle %d\n", (nrpt+1));
}
				/*ALL DONE--TERMINATION OF MAIN LOOP*/
prog_exit();
}				/*END OF PSPECIAL LOOP*/
}				/* END OF MAIN				*/
/*page eject*/


int read_variables()
{
	printf
("\nDo you want to change values of program variables? (y/cr=>default): ");
	str_buf[0] = getchar();
	if ((str_buf[0] != 0x79) && (str_buf[0] != 0x59)) {
		printf("\n");
		return;
	}
	printf
("\nNew title? \n*:");
	gets(str_buf);
	if (strlen(str_buf)) {
		strcpy(title, str_buf);
		printf("     Another title line?\n*:");
		gets(str_buf);
		if (strlen(str_buf))
			strcat(title,"\n");
			strcat(title, str_buf);
	}
	printf("\n");
	read_num(
"How many array rows? (value/cr=>no change = %d): ", 
		&xnrow);
	printf("\n");
	read_num(
"How many array cols? (value/cr=>no change = %d): ", 
		&xncol);
	printf("\n");
	read_num(
"How many blocked regions? (value/cr=>no change = %d): ", 
		&nblocked);
	printf("\n");
	read_num(
"How many repeat determinations? (value/cr=>no change = %d): ", 
		&xnrpt);
	printf("\n");
		read_num(
"How many cycles do you want printed? (value/cr=>no change = %d): ", 
		&no_print);

	printf("\n\n");
	return;
}				/*END OF READ_VARIABLES			*/



int read_num(string, val)
char *string;
int *val;
{
	while (TRUE) {
		printf(string, *val);
		if (strlen(gets(str_buf)) == 0) return;
		*val = atoi(str_buf);
	}
	return;
}				/*END OF READ_NUM			*/



int setup()
{
	char clock_in[12];
	long temp;

/* set default values of xnrow, etc*/
			xnrow = NROW;
			xncol = NCOL;
			xnrpt = NRPT;
			nblocked = NBLOCKED;
			no_print = NO_PRINT;
			nsite_blocked = PTS_BLOCKED;

/*establish kernel for random number generator*/
	read_clock();
	temp = s1 + s2 + s3;
	printf("\n\n temp = %ld = %d + %d + %d\n\n", temp, s1, s2, s3);
	srand(temp);

/*fill array used to set block of points to be excluded from the fitting*/
	for (i = 0; i < 25; i++)
		if (test_block[i])
			test_block[i] = BLOCKED;
		else
			test_block[i] = BLANK;

	return;
}				/*END OF SETUP				*/



#if HAYES_MODEM
int read_clock()
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

	return;
}				/*END OF READ_CLOCK			*/



int hayes_chrono(command, buffer)
/* Send command to clock and return with string*/
char *command, *buffer;
{
	strcpy(str_buf,"AT");
	strcat(str_buf,command);		/* Form command			*/

	for (i = 0; str_buf[i] != NULL; bios(PUNCH, str_buf[i++]));

	for (i = 0; i < 12; i++)
		if ((str_buf[i] = bios(READER, 0)) == CR)
			break;
	str_buf[i] = '\0';

	strcpy(buffer, str_buf);
	return;
}				/* END OF HAYES_CHRONO			*/


#else
int read_clock()
{
	printf("\n\nPlease hit three keys, not too quickly, to seed random number generator\n\n");
	s1 = s2 = s3 = 0;
	while (KBHIT)
		getchar();
	while (!KBHIT)
		s1++;
	while (KBHIT)
		getchar();
	while (!KBHIT)
		s2++;
	while (KBHIT)
		getchar();
	while (!KBHIT)
		s3++;
	while (KBHIT)
		getchar();
}				/*END OF READ_CLOCK IF NO MODEM IN SYSTEM */
#endif



int init_array()
{
				/*loop until success or 100 times*/
ll = 100;
while (ll--) {
				/*zero arrays*/
	for (i = 0; i < ARRAY_DIM; i++) {
		sum[i] = 0;
		for (j = 0; j < ARRAY_DIM; j++)
			mast_array[i][j] = work_array[i][j] = BLANK;
	}

				/*exclude block of sites*/
				/*count excluded sites*/
	cnt_crit[nrpt] = cnt_blocked[nrpt] = 0;
	if (!nblocked) return;
	for (k = 0; k < nblocked; k++) {
		while (TRUE) {
			i = (irand() % xnrow) + 1;
			j = (irand() % xncol) + 1;
			if ((i > 2) && (i < xnrow - 1) 
					&& (j > 2) && (j < xncol - 1)) {
			    kk = 0;
			    for (ii = (i - 2); ii < (i + 3); ii++)
				for (jj = (j - 2); jj < (j + 3); kk++, jj++)
				    if ((mast_array[ii][jj] != BLOCKED)
					      && (test_block[kk] == BLOCKED)){
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
	iæ (work_test()© 
		return;
}
printf("\nFailure in array initialization\n");
prog_exit();
exit();
}				/*END OF INIT_ARRAY			*/



int exit_test()
{
	if (KBHIT) {
		str_buf[0] = getchar();
printf("\ntype x or X to exit or any other character to continue: ");
		str_buf[0] = getchar();
		if ((str_buf[0] == 0x58) || (str_buf[0] == 0x78)) {
			prog_exit();
			exit();
		}
		return TRUE;
	}
	return FALSE;
}				/*END OF EXIT_TEST			*/



int prog_exit()
{
	header_disp();
	nrpt = nrpt - 1;
	crit_calc();
	crit_disp();
	printf("\n\n\n\f");
	return;
}				/*END OF PROG_EXIT			*/



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
		nblocked, nsite_blocked);
	return;
}				/*END OF HEADER_DISP			*/



int array_disp()
/*display master array and working array side-by-side, if possible*/
/*display blocked sites, in mast_array*/
/*display trail of the work_test pointer, in work_array*/
/*display coordinates of last site filled*/
{
	printf("\n");
	for (i = 1; i < (xnrow + 1); i++) {
		printf("\n");
		for (j = 1; j < (xncol + 1); j++)
			printf("%c", mast_array[i][j]);
		if (xncol < 38) {
			printf("   ");
			for (j = 1; j < (xncol + 1); j++)
				printf("%c", work_array[i][j]);
		}
	}
	if (xncol > 37) {
		printf("\n\n\n");
		for (i = 1; i < (xnrow + 1); i++) {
			printf("\n");
			for (j = 1; j < (xncol + 1); j++)
				printf("%c", work_array[i][j]);
		}
	}
	printf("\n\n nrpt=%d   cnt_crit[nrpt]=%d   ",
		nrpt, cnt_crit[nrpt]);
	printf("          i_last=%d   j_last=%d\n", i_last, j_last);
	return;
}				/* END OF ARRAY_DISP			*/



int crit_disp()
/*display critical counts and fractions--averages and variances*/
{
if (nblocked) {
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
"\n\n   critical count =%10.2f     sample_var=%10.2f   avg_var=%10.2f",
		fcrt_avg, fcrt_var, fcrt_1var);
	printf(
  "\n   critical/total =%10.5f     sample_var=%10.7f   avg_var=%10.7f", 
		frct_avg, frct_var, frct_1var);

if (nblocked) {
	printf(
"\n\n   blocked sites  =%10.2f     sample_var=%10.2f   avg_var=%10.2f", 
		fblc_avg, fblc_var, fblc_1var);
	printf(
  "\n   blocked/total  =%10.5f     sample_var=%10.7f   avg_var=%10.7f", 
		frt1_avg, frt1_var, frt1_1var);
	printf(
  "\n   critical/unblkd=%10.5f     sample_var=%10.7f   avg_var=%10.7f", 
		frt2_avg, frt2_var, frt2_1var);
}
	printf("\n\n\n");
	return;
}	      			/* END OF CRIT_DISP			*/



int crit_calc()
/*calculate averages and variances of critical counts and fractions*/
{
	fnrpt = nrpt;
	fnrpt_1 = nrpt + 1;
	fsites = nsites;
	crit_sum = 0;
	for (i = 0; i < nrpt + 1; i++)
		crit_sum = crit_sum + cnt_crit[i];

	fcrt_avg = (double) crit_sum / fnrpt_1;
	fcrt_var = 0;
	if (nrpt > 0) {
		for (i = 0; i< (nrpt + 1); i++)
			fcrt_var = fcrt_var + ((double) cnt_crit[i] - fcrt_avg) 
					* ((double) cnt_crit[i] - fcrt_avg);
		fcrt_var = fcrt_var / fnrpt;
	}
	fcrt_1var = fcrt_var / fnrpt_1;
	frct_avg = fcrt_avg / fsites;
	frct_var = fcrt_var / (fsites * fsites);
	frct_1var = fcrt_1var / (fsites * fsites);

				/*calculations if blocked regions present*/
	if (!nblocked) return;
	block_sum = 0;
	for (i = 0; (i < (nrpt + 1)) && (cnt_blocked[i] != 0); i++)
		block_sum = cnt_blocked[i] + block_sum;

	fblc_avg = block_sum / fnrpt_1;
	fblc_var = 0;
	if (nrpt > 0) {
		for (i = 0; i< (nrpt + 1); i++)
		    fblc_var = fblc_var + ((double) cnt_blocked[i] - fblc_avg) 
				* ((double) cnt_blocked[i] - fblc_avg);
		fblc_var = fblc_var / fnrpt;
	}
	fblc_1var = fblc_var / fnrpt_1;
	frt1_avg = fblc_avg / fsites;
	frt1_var = fblc_var / (fsites * fsites);
	frt1_1var = fblc_1var / (fsites * fsites);

	funblocked = fsites - fblc_avg;
	frt2_avg = fcrt_avg / funblocked;
	frt2_var = (frt2_avg * frt2_avg * fblc_var + fcrt_var)
			/ funblocked / funblocked;
	frt2_1var = (frt2_avg * frt2_avg * fblc_1var + fcrt_1var)
			/ funblocked / funblocked;
	return;
}				/*END OF CRIT_CALC			*/


int sum_filled(array)
char array[ARRAY_DIM][ARRAY_DIM];
/*sum filled sites of array, by row*/
/*return 0 if any row is zero (which means there can be no connection
	between top and bottom edges (rows))*/
/*else return count of all filled sites in array*/
{
	count = 0;
	for (i = 1; i < (xnrow + 1); i++) {
		sum[i] = 0;
		for (j = 1; j < (xncol + 1); j++)
			if (array[i][j] == FILLED)
				sum[i] = sum[i] + 1;
		if (sum[i] == 0)
			return 0;
		count = count + sum[i];
	}
	return count;
}				/* END OF SUM_FILLED			*/



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
}				/*END OF COPY_ARRAY			*/



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
	for (j = 1; j < (xncol + 1); j++) {
		if (work_array[1][j] == BLANK || work_array[2][j] == BLANK)
			continue;
		k = 2;
		i = 2;
		work_array[1][j] = TRAIL;
		work_array[2][j] = TRAIL;
		while (i > 1) {
			if ((k_last = k % 4) == 0)
				k_last = 4;
			for (k = (k_last - 1); k < (k_last + 3); k++) {
				ii = i + del_i[k];
				jj = j + del_j[k];
				if (work_array[ii][jj] != BLANK) {
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
}				/*END OF WORK_TEST			*/



