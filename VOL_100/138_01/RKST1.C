/*
	program:	rkst1.c
	created:	03-Nov-83
	by:		A. Skjellum

	modified:	14-Nov-83
	by:		M. J. Roberts

	Copyright 1983, 1984 (c) California Institute of Technology.
	All rights Reserved.  This program may be freely distributed
	for all non-commercial purposes but may not be sold.

	purpose:	illustrate the use of rk4n program

	uses:		rk4n() (rks.c)
			
	summary:
		integrates the differential equation:

		y'(t) + y(t) = t + 1
		y(0) = 5.0.

		for which the exact solution:

		y(t) = t + 5exp(-t) is known.
	
		Integrates the same equation as rktest1 
		but using the more general equation solver, 
		rk4n().  This run is exactly the same as for 
		rktest1, except that, rather than trying to solve 
		exactly the same equation, we will solve a 
		"system" of one differential equation.

*/

/* constants */

#define SYSIZE	1	/* number of functions in system */
#define	YZERO	5.0	/* initial value for y */
#define	TSTART	0.0	/* starting time for integration */
#define	TEND	10.0	/* ending time for integration */
#define	STEPS	50	/* 50 steps in integration */

/* variables external to all functions */

double wvalue[STEPS+1][SYSIZE];
double yarray[STEPS+1][SYSIZE];
double tarray[STEPS];
	/* integrated solution stored here */


/* subroutines: */

/* exact(): returns exact solution value, given t */

double exact(t)
double t;
{
	extern double exp();	/* exponential function */

	return((t + YZERO*exp(-t)));
}

/* fn(j,i,t,y): return f(t,y) given t,y values */

double fn(j,i,t,y)
int j;
int i;
double t;
double (*y)();
{
	double a,b;		/*  temporary storage space  */

	/*
		differential equation is y' + y = t + 1

		therefore, y' = t + 1 - y.

	*/

	a = (*y)(0,j,i);	/*  calculate function
				Note that the ZERO was passed so as to
				allow the function to know which argument
				we are talking about -- in this case,
				we only need one argument evaluated, so
				pass it 0 to indicate the first (zeroeth,
				actually) argument is to be calculated.  */

	b = t + 1.0 - a;	/*  and figure out the rest of it  */
	return(b);

}



/* store():  the routine to store away the W values for later reference */

double store(row, col, value)
int row, col;		/*  location to store the value  */
double value;		/*  the actual value to store  */
{
	wvalue[row][col]=value;
	return (value);
}

/* source():  return the  W value referenced by input parameters  */

double source(row,col)
int row, col;		/*  location to look up */

{
	return (wvalue[row][col]);
}

/* solutn(): print solution step at console */


solutn(j,i)
int j,i;	/* element numbers  */
{
	double time;
	double ex;
	double approx;

	time   = tarray[j];
	ex     = exact(time);
	approx = source(j,i);

	printf("t = %7.3e, y = %7.3e, y_exact = %7.3e, diff = %7.3e\n",
		time,approx,ex,approx - ex);
}

/* main program: */

main()
{
	/* external declarations */

	double store(), source();

	double fn();	/* ensure that this is typed as double */

	/* local variables: */

	register int i,j;


	double init[1];		/* initial condition matrix */

	/* begin code: */

 	printf("\n\nrktest1.c	as of 03-Nov-83\n\n");
	printf("Integrates: y' + y = 1 + t     for\n\n");
	printf("t = %7.3e to %7.3e, with %u steps\n\n",
		TSTART,TEND,STEPS);

	/*
		integrate the answer from t = 0 to t = 10 sec 
		STEPS points.
	*/

	init[0] = YZERO;	/* set up initial condition matrix */

	rk4n(fn,source,store,SYSIZE,TSTART,TEND,STEPS,init,tarray,yarray);
			/* compute the answers for 1 function */

	/*  Print out solution  */

	for(j=0;j<STEPS;j++)	/* print solution */
		solutn(j,0);


	printf("\n\nEnd of execution\n\n");
}
