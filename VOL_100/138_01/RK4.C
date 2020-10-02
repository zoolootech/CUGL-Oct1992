/*  
	Runge - Kutta order 4 Algorithm

	Creation date:	31-Oct-83
	Author:		Mike Roberts

	Copyright 1983, 1984 (c) California Institute of Technology.
	All rights Reserved.  This program may be freely distributed
	for all non-commercial purposes but may not be sold.

	This algorithm is described in detail on page 205 of
	Burden, Richard L.:  Numerical Analysis.

	To approximate the solution of the initial value problem
		y'=f(t,y),  a<=t<=b, y(a)=alpha,
	at (N+1) equally spaced numbers in the interval [a,b]:
	INPUT endpoints a,b; integerg N; initial condition alpha.
	OUTPUT approximation w to y at the (N+1) values of t.

Step 1:
	Set	h=(b-a)/N;
		t=a;
		w=alpha;
	Output (t,w).
Step 2:
	For i=1,2,...,N do Steps 3-5:

	Step 3:
		Set	K1=hf(t,w);
			K2=hf(t+h/2,w+K1/2);
			K3=hf(t+h/2,w+k2/2);
			K4=hf(t+h,w+K3).
	Step 4:
		Set w=w+(K1+2K2+2K3+K4)/6;	(Compute w[i].)
			t=a+ih.			(Compute t[i].)
	Step 5:
		Output (t,w).

Step 6:
	Stop.


*/


#define FALSE 0
#define TRUE 1

rk4(function,a,b,n,alpha,t,w)

double (*function)();	/* function giving f(t,y) */
double a;		/* beginning of interval */
double b;		/* end of interval */
int n;			/* number of steps in interval */
double alpha;		/* initial condition for y */
double t[];		/* array for returning T[i] values */
double w[];		/* array for returning W[i] values */

{
	register int i;	/* counter for integration steps */
	double h;	/* stepsize */
	double time;
	double yapprox;	/* approximation for y value  */

	/*  STEP 1:  Initialization  */

	h = (b-a) / (double)n ;		/* Compute stepsize */
	time = a;			/* Initialize time */
	yapprox = alpha;		/* Start with the approximation
					   equal to the initial value */
	
	for (i=0; i<n; i++)		/* Main integration loop */
	{
		if(i)	/* if not first time, call the integrator */
			rk4_1(function,h,time,&yapprox);
    	
			/* Pass the function pointer, the h, time, and
				yapprox values, and the pointers to
				the current positions in the T and W
				matrices */

		time = a + h*(double)i;  /* compute time */
		t[i] = time;		 /* also save it */
		w[i] = yapprox;		 /* store value for function */
	}
}

/*  This is the RK4 integrator portion.  It performs one step of the
	integration, and is called on each step from the RK4 loop.
	
	function = pointer to function to integrate
	h = stepsize
	time = current time location
	yapprox = current w (function approximation)

*/

rk4_1(function,h,time,yapprox)

double (*function)();		/* Pointer to function to integrate */
double h;			/* Step size */
double time;			/* Current time step */
double *yapprox;		/* Current approximation of function */

{
	double k1, k2, k3, k4;	/* Temporary values in RK calculation */

	k1 = h * (*function)(time,*yapprox);	/* Evaluate first approx */
	k2 = h * (*function)(time+h/2.0, *yapprox+k1/2.0);
						/* Evaluate second approx */
	k3 = h * (*function)(time+h/2.0, *yapprox+k2/2.0);
						/* And the third */ 
	k4 = h * (*function)(time+h, *yapprox+k3);
						/* And the last one */

	*yapprox += (k1 + 2.0*(k2 + k3) + k4)/6.0; /* new approx */
}
