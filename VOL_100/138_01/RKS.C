/*
	rk4n.c				created: 07-Nov-83
	authors:			A. Skjellum, 
					M. Roberts

	updated:			14-Nov-83
					by MJR

	Copyright 1983, 1984 (c) California Institute of Technology.
	All rights Reserved.  This program may be freely distributed
	for all non-commercial purposes but may not be sold.

	Purpose:
	integrate M first order differential equations

	y'[i] = f[i](t;y[j=1...M])

	M equations.

	algorithm:	see Burden, Faires, Reynolds, p. 239-240
			also see p. 205

		1. interval t = [a,b]
		2. choose N > 0 as as partition of interval (N steps)
		3. define step size h = (b-a)/N.
		4. Initial conditions: (denote w[i,j] as approxes to y's)

			w[i,0] = alpha[i]
			means: ith w at time zero is set to initial value
			       alpha[i].

		5. Computing the w[i,j+1] from w[i,j] is done as follows:

			loop over i = 1 to M

			    compute k1[i] = h*f[i](t,w[1,j],...,w[M,j])

			end of loop

			loop over i = 1 to M

			    compute k2[i] = h*f[i](t+h/2,w[1,j]+.5*k1[1],...,
					    w[M,j] + .5*k1[M])

			end of loop

			loop over i = 1 to M

			    compute k3[i] =h*f[i](t+h/2,w[1,j]+.5*k2[1],...,
					   w[M,j] + .5*k2[M])

			end of loop

			loop over i = 1 to M

			    compute k4[i] =h*f[i](t+h,w[1,j]+k3[1],...,
					   w[M,j] + k3[M])

			end of loop

			loop over i = 1 to M

			    w[i,j+1] = w[i,j] +
				       {k1[i] + 2*k2[i] + 2*k3[i] + k4[i]}/6
                                                                              
                        end of loop                       

*/

double	(*rk_function)();
double	(*rk_source)();
double	(*rk_store)();
double	(*rk_kuttas)[4];
double	(*rk_comp[4])();		/* tells us how to form k's */

/* functions called indirectly by k_calc()  */

double rk_1(n,j,i)	/* to provide compatibility with calling */
int n;
int j;
int i;
{
	return ((*rk_source)(j,n));
}

double rk_23(n,k,j,i)  /*  N is in { 0...M } = argument number.
			   Since we have one argument to FN per equation,
			   N will indicate which we are currently being
		 	   asked to provide.  Same goes for other RK_x
			   functions below.  */
int n;
int k;
int j;
int i;
{
	return((*rk_source)(j,n) + .5*rk_kuttas[n][k]);
}

double rk_2(n,j,i)
int n;
int j;
int i;
{
	return(rk_23(n,0,j,i));
}

double rk_3(n,j,i)
int n;
int j;		
int i;
{
	return(rk_23(n,1,j,i));
}

double rk_4(n,j,i)
int n;
int j;
int i;
{
	return((*rk_source)(j,n) + rk_kuttas[n][2]);
}


/*  Here's the integrator!!! */


double rk4n(function,wsource,wstore,m,a,b,n,alpha,t,kuttas)

double (*function)();	/* pointer to function which returns deriv info */
double (*wsource)();	/* source of w[i,j] values */
double (*wstore)();	/* function which stores w[i,j] values for us */
int m;			/* number of equations */
double a;		/* start of interval */
double b;		/* end   of interval */
int n;			/* number of points */
double alpha[];		/* array of initial values */
double t[];		/* array where we store times */
double kuttas[][4];	/* n x 4 kuttas (k1,k2,k3,k4 i=1,...m) */
{

	register int i; /* looping variable */
	register int j;	/* looping variables */
	double time;
	double h = (b-a)/(double)n;	/* step size */

	double rk_k2(),rk_k3(),rk_k4();	/* include for emphasis */

	rk_function = function;

	rk_kuttas   = kuttas;
	rk_source   = wsource;
	rk_store    = wstore;
	
	rk_comp[0]  = rk_1;
	rk_comp[1]  = rk_2;
	rk_comp[2]  = rk_3;
	rk_comp[3]  = rk_4;
	
	/*  First assign initial values  */

	for (i = 0;i < m;i++)
		(*rk_store)(0,i,alpha[i]);

	/*  Now loop through the necessary loop, calculating
		each K value for each equation in each time step. */


	for(j = 0;j < n;j++)	/* n time steps */
	{
		time = a + h*(double)j;  /* compute time */
		t[j] = time;		 /* also save it */

		rk4_1n(m,j,time,h);
	}
}

/* rk4_1n(): compute one solution step for n equations */

rk4_1n(m,j,time,h)
int m;
int j;				/* time step we're working on */
double time;
double h;
{
	register int k;	/* k calculation loop */
	register int i;	/* m equations loop */
	double value;	/* temporary */

	for(k=0;k < 4;k++)	/* k compute loop */
		k_calc(m,k,j,time,h);

	for(i=0;i<m;i++)	/* compute new w[i,j]'s j fixed here */
	{
		value = (*rk_source)(j,i) + .166666667*(rk_kuttas[i][0] +
			2.0*(rk_kuttas[i][1] + rk_kuttas[i][2])
			    + rk_kuttas[i][3]);
				/* value for w[i,j] */
		(*rk_store)(j+1,i,value); /* save this hard got number */

	} 
}

/* k_calc(): compute rk coefficients for fixed j */

k_calc(m,k,j,time,h)
int m;
int k;
int j;
double time;
double h;
{
	register int i;
	double tval;

	switch(k)
	{
		case 0:
			tval = time;
			break;
		case 1:
		case 2:
			tval = time + .5*h;
			break;
		case 3:
			tval = time + h;
			break;
	}

	for(i=0;i<m;i++)		/*  used to be 1;<=m  */
	{
		rk_kuttas[i][k] = h*(*rk_function)(j,i,tval,rk_comp[k]);
	}
}

