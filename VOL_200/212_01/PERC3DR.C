/* PERC3DR.C    VERS:- 01.00  DATE:- 10/06/86  TIME:- 05:23:48 PM */
/*
%CC1 $1.C -X -O -E 7000
%CLINK $1 DIO -F FLOAT -S
%DELETE $1.CRL 
*/
/* 
Description:

Simulation of percolation on a three-dimensional simple cubic lattice.
Option to wrap in one dimension, ie to simulate cylindrical shells.
The algorithm is fairly efficient (20 seconds/1000 site 2-d sheet).

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
/*CHANGE TO TRUE FOR DEBUG OUTPUT*/
#define DEBUG FALSE

#define NSITES 6400
#define NROW  10
#define NCOL  10
#define NLEVEL 10
#define NRPT  20
#define NO_PRINT 2
#define NCLUSTERS (NSITES / 8)
#define WRAP_COL FALSE

int mast_array[NSITES];
int top[NCLUSTERS], bottom[NCLUSTERS];
int cnt_crit[100];

char str_buf[80];
char title[160];

int xnrpt, xnrow, xncol, xnlevel;
int nrpt, nrpt_1;
int nsites;
int no_print;
int wrap_col;

int crit_sum;
char fcrit_avg[5], fcrit_var[5], fcrit_1var[5];
char fract_avg[5], fract_var[5], fract_1var[5];
char fnrpt[5], fnrpt_1[5], ftemp[5], fsites[5], fsqd[5];

int i, j, k, ii, jj, kk, ll;
int i_new, j_new, k_new;
int i_old, j_old, k_old;
int new_index, old_index;
int new, old;
int cnt_cluster, ncluster;
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
		case 0 :        /*case 0 used -ONLY- for setup*/
			setup();
			/*insert title(up to 2 lines)*/
			strcpy(title,
			"SIMULATION OF PERCOLATION ON A THREE-DIMENSIONAL SIMPLE CUBIC LATTICE"
			    );
			strcat(title,
			""
			    );
			printf("\n%s\n", title);
			read_variables();
			set_more();
			continue;
		case 1 :        /*case 1 to 99 = specials*/
			break;
		default :
			dioflush();
			exit();
		}

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
				i_new = (nrand(1) % xnrow);
				j_new = (nrand(1) % xncol);
				k_new = (nrand(1) % xnlevel);
				new_index = i_new * xncol * xnlevel
				    + j_new * xnlevel + k_new;
				if (mast_array[new_index])
					continue;
				/*add new site to the master array*/
				/*exit subloop if connection made*/
				if (add_site())
					break;
#if !DEBUG
				if (exit_test())
					array_disp();
#endif
			}

			/*display cnt_crit values and their*/
			/*average and variance*/
			if (nrpt < no_print)
			{
				printf("\n\n");
				array_disp();
				crit_disp();
			}
			else
				typef("working on cycle %d\n", (nrpt + 1));
#if DEBUG
			exit_test();
#endif
		}
		/*ALL DONE--TERMINATION OF MAIN LOOP*/
		prog_exit();
	}
	/*END OF PSPECIAL LOOP*/
}
/* END OF MAIN				*/
/*page eject*/


void read_variables()
{
	printf("\nDo you want to change values of program variables? (y/cr=>default): ");
	getline(str_buf, 2);
	if ((str_buf[0] != 0x79) && (str_buf[0] != 0x59))
	{
		printf("\n");
		return;
	}
	printf("\nNew title? \n*:");
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
	readval("How many array rows? (value/cr=>no change = %d): ", &xnrow);
	printf("\n");
	readval("How many array cols? (value/cr=>no change = %d): ", &xncol);
	printf("\n");
	readval("How many array levels? (value/cr=>no change = %d): ", &xnlevel);
	printf("\n");
	readval("Wrap left column to right column (1 = yes)? (value/cr=>no change = %d): ", &wrap_col);
	printf("\n");
	readval("How many repeat determinations? (value/cr=>no change = %d): ", &xnrpt);
	printf("\n");
	readval("How many cycles do you want printed? (value/cr=>no change = %d): ", &no_print);
	printf("\n\n");
}
/*END OF READ_VARIABLES			*/



void readval(string, val)
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
}
/*END OF READVAL			*/



void setup()
{
	/* set default values of xnrow, etc*/
	xnrow = NROW;
	xncol = NCOL;
	xnlevel = NLEVEL;
	xnrpt = NRPT;
	no_print = NO_PRINT;
	wrap_col = WRAP_COL;

	/*establish kernel for random number generator*/
	read_clock();
	nrand(-1, s1, s2, s3);
}
/*END OF SETUP				*/



void set_more()
{
	if ((nsites = xnrow * xncol * xnlevel) > NSITES)
	{
		printf("\n\nArray too large.  Error exit.");
		dioflush();
		exit();
	}
}
/*END OF SET_MORE				*/


#if HAYES_MODEM
void read_clock()
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
}
/*END OF READ_CLOCK			*/



#define PUNCH 6
#define READER 7
#define CR 0x0d

void hayes_chrono(command, buffer)
/* Send command to clock and return with string*/
char *command, *buffer;
{
	strcpy(str_buf, "AT");
	strcat(str_buf, command);
	for (i = 0; str_buf[i] != NULL; bios(PUNCH, str_buf[i++]))
		;
	for (i = 0; i < 12; i++)
		if ((str_buf[i] = bios(READER, 0)) == CR)
			break;
	str_buf[i] = '\0';
	strcpy(buffer, str_buf);
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
/*END OF "READ_CLOCK" FOR SYSTEM WITH NO CLOCK */
#endif



int exit_test()
{
	int t;

#if !DEBUG
	if (bios(2, 0))
#endif
	{
		while (bios(2, 0))
			bios(3, 0);
		printf("\ntype x or X to exit or any other character to continue: ");
		if (((t = bios(3, 0)) == 0x58) || (t == 0x78))
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



void prog_exit()
{
	header_disp();
	nrpt = nrpt - 1;
	crit_disp();
	printf("\n\n\n\f");
}
/*END OF PROG_EXIT			*/



void header_disp()
{
	printf("\n\n%s\n", title);
#if HAYES_MODEM
	read_clock();
#endif
	printf("\nSimulation of percolation on a three-dimensional simple cubic lattice.");
	printf("\nThe array size is %d rows by %d columns by %d levels, %d sites total.",
		xnrow, xncol, xnlevel, nsites);
	printf("\nThe test is for connection between top and bottom levels.");
	if (wrap_col)
	{
		printf("\nWrap left to right column.");
		printf("\nCylindrical shell: diameter = columns; thickness = rows; height = levels.\n\n");
	}
	else
	    {
		printf("\nWrap off.");
		printf("\nPlate: width = columns; thickness = rows; height = levels.\n\n");
	}
}
/*END OF HEADER_DISP			*/



void crit_disp()
/*display critical counts and fractions--averages and variances*/
{
	char fpno1[5], fpno2[5];
	int sign;
	char *sqrt();

	crit_calc();

	printf("\nList of all counts of filled sites at the critical point\n");
	for (i = 0; i < (nrpt + 1); i++)
		printf("%4d%c", cnt_crit[i], ((i % 10) == 9 || i == nrpt) ? '\n' : ' ');
	printf("\n\n   critical count =%10.2f     sample_sd=%10.2f   avg_sd=%10.2f", fcrit_avg, sqrt(fpno1, &sign, fcrit_var), sqrt(fpno2, &sign, fcrit_1var));
	printf("\n   critical/total =%10.5f     sample_sd=%10.7f   avg_sd=%10.7f", fract_avg, sqrt(fpno1, &sign, fract_var), sqrt(fpno2, &sign, fract_1var));
	printf("\n\n\n");
}
/* END OF CRIT_DISP			*/



void crit_calc()
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

}
/*END OF CRIT_CALC			*/


void init_array()
{
	/*zero arrays*/
	ll = nsites;
	while (ll--)
		mast_array[ll] = 0;
	cnt_cluster = 0;
	ncluster = 0;
	cnt_crit[nrpt] = 0;
}
/*END OF INIT_ARRAY			*/



int add_site()
{
	int wrap_flag;
	cnt_crit[nrpt]++;
	new = 0;
	for (i = 0; i < 6; i++)
	{
		i_old = i_new;
		j_old = j_new;
		k_old = k_new;
		wrap_flag = 0;
		switch (i)
		{
		case 0 :
			i_old = max(0, i_new - 1);
			break;
		case 1 :
			i_old = min(xnrow - 1, i_new + 1);
			break;
		case 2 :
			if ((j_old = (j_new - 1)) < 0)
			{
				wrap_flag = 1;
				if (wrap_col)
					j_old = xncol - 1;
				else
				    j_old = 0;
			}
			break;
		case 3 :
			if ((j_old = (j_new + 1)) >= xncol)
			{
				wrap_flag = 1;
				if (wrap_col)
					j_old = 0;
				else
				    j_old = xncol - 1;
			}
			break;
		case 4 :
			k_old = max(0, k_new - 1);
			break;
		case 5 :
			k_old = min(xnlevel - 1, k_new + 1);
			break;
		}
		old_index = i_old * xncol * xnlevel
		    + j_old * xnlevel + k_old;
		old = mast_array[old_index];
#if DEBUG
		printf("wrap = %d %d old_point= %2d %2d %2d  old_index=%-3d old=%-3d new_index=%-3d new=%-3d\n", wrap_col, wrap_flag, i_old, j_old, k_old, old_index, old, new_index, new);
#endif
		if (!old)
			continue;
		if (!new)
			new = mast_array[new_index] = old;
		else
			if (new != old)
				combine_cluster();
	}
	if (!new)
	{
		if (ncluster++ == NCLUSTERS)
		{
			printf("\n\nToo many clusters.  Error exit\n");
			prog_exit();
			dioflush();
			exit();
		}
		cnt_cluster++;
		new = mast_array[new_index] = ncluster;
		top[new] = bottom[new] = 0;
	}
	if (k_new == 0)
		top[new] = 1;
	if (k_new == (xnlevel - 1))
		bottom[new] = 1;
#if DEBUG
	array_disp();
#endif
	if (top[new] && bottom[new])
		return 1;
	else
	    return 0;
}
/*END OF ADD_SITE			*/



void combine_cluster()
{
	cnt_cluster--;
	ll = nsites;
	while (ll--)
		if (mast_array[ll] != old)
			continue;
	else
	    mast_array[ll] = new;
	if (top[old])
		top[new] = 1;
	if (bottom[old])
		bottom[new] = 1;
}
/*END OF COMBINE_CLUSTER		*/



void array_disp()
{
	int temp_index;
	char temp;

	printf("\ncoordinates of last point = %d %d %d    point = %d of %d sites.\n", i_new, j_new, k_new, cnt_crit[nrpt], nsites);
	printf("cluster = %d  of  (%d - %d) clusters.\n", new, ncluster, (ncluster - cnt_cluster));
	printf("topflag = %d, bottomflag = %d        wrap_col = %d (1 => true)\n", top[new], bottom[new], wrap_col);
	if (((3 + xncol) * xnlevel) >= 79)
		return;
	for (i = 0; i < xnrow; i++)
	{
		printf("\n");
		for (k = 0; k < xnlevel; k++)
		{
			printf(" | ");
			for (j = 0; j < xncol; j++)
			{
				temp_index = 
				    i * xncol * xnlevel + j * xnlevel + k;
				temp = 0x40 + mast_array[temp_index];
				printf("%c", (temp == 0x40 ? '-' : temp));
			}
		}
	}
	printf("\n\n");
}
/*END OF ARRAY_DISP			*/


