/*
HEADER:       sk.c		
TITLE:        Solve Kepler's Equation 		
VERSION:      1.0	
DESCRIPTION:  This routine solves Kepler's equation for eccentric
              anomaly.  Iterative solution by Newton's method is used.
              Required inputs are initial guess at eccentric anomaly,
              value of mean anomaly, stopping tolerance, and maximum
              number of iterations.  Input and output anomalies are
              in degrees.
KEYWORDS:     Astrodynamics, orbital mechanics, Kepler's equation
SYSTEM:       MS-DOS, PC-DOS (Coded with Ver. 3.3, but should be version
              independent) 		
FILENAME:     sk.c	
WARNINGS:	  This routine was coded for educational purposes, using
              a standard iteration technique.  No attempt has been made
              to insure optimal numerical stability or optimal convergence
              rate.
SEE-ALSO:     ---	
AUTHORS:      Rodney Long
              19003 Swan Drive
              Gaithersburg, MD 20879 	
COMPILERS:    Microsoft C 5.1	
REFERENCE:    Bate, Mueller, White: Fundamentals of Astrodynamics
              Atkinson: An Introduction to Numerical Analysis
*/


#include <stdio.h>
#include <float.h>
#include <math.h>
#include "orbcons.h"

double sk();

double e[3] = {.1,.9,.999999999999}; //Input orbit eccen

main()

{
  double eout;
  double m, e0, t;
  int i,mi;


   e0   =  280;        // Input guess at eccen anom
   m    =  286.879298; // Input mean anom
   t    = .000001;	  // Input stopping tolerance
   mi   =  15;         // Input maximum iterations

   printf("Kepler's equation solved by Newton method \n");
   printf(" --Beware of e very close to 1!-- \n");
   printf("Initial eccentric anom           = %25.16lf \n",e0);
   printf("Input mean anom                  = %25.16lf \n",m);
   printf("Input maximum its                =      %d \n",mi);
   printf("Input stop tolerance             = %25.16lf \n",t);

   for (i=0;i<1;i++) {
     printf("Input eccentricity               = %25.16lf \n",e[i]);
     eout = sk(m,e0,e[i],mi,t);
     printf("Final eccentric anom             = %25.16lf \n",eout);
     printf("Mean anom computed from solution = %25.16lf \n",
            ((eout*DTR - e[i]*sin(eout*DTR))*RTD));
   }
}
  
double sk(double m,double e0,double e,int mi,double t)
{
  /* Solve Kepler's equation by the Newton method. */
  double en,ep;
  int i;

  e0 = e0 * DTR;
  m  = m  * DTR;

  i  = 0;
  en = e0;
  do {
    i++;
    ep = en;

    // The next line of code is the Newton iteration:
    //   x(n+1) = x(n) - (1/(dy/dx)) * y(n). 

    en = en - (en - e*sin(en) - m)/(1 - e*cos(en))   ;
    en = fmod(en,TWOPI);
    if ( fabs(en-ep) < t) break;
  } while (i <= mi );
  return(en*RTD);
}
    
   
