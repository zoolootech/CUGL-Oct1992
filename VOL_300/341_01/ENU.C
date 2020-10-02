/*
HEADER:       enu.c		
TITLE:        Convert between eccentric and tru anomaly. 		
VERSION:      1.0	
DESCRIPTION:  This routine converts eccentric to true anomaly and vice
              versa.  To convert eccentric to true anomaly, inputs
              are eccentric anomaly, eccentricity, and the input switch
              is set to 1.  To convert true to eccentric anomaly, inputs
              are true anomaly, eccentricity, and the input switch is set
              to zero.  Input and output anomalies are in degrees.
KEYWORDS:     Astrodynamics, orbital mechanics, Kepler's equation
SYSTEM:       MS-DOS, PC-DOS (Coded with Ver. 3.3, but should be version
              independent) 		
FILENAME:     enu.c	
WARNINGS:	  This routine was coded for educational purposes, using
              the method given in the reference.  No attempt has been made
              to insure optimal numerical stability or optimal convergence
              rate.
SEE-ALSO:     ---	
AUTHORS:      Rodney Long
              19003 Swan Drive
              Gaithersburg, MD 20879 	
COMPILERS:    Microsoft C 5.1	
REFERENCE:    Bate, Mueller, White: Fundamentals of Astrodynamics
*/

#include <stdio.h>
#include <float.h>
#include <math.h>
#include "orbcons.h"

double enu();

main()

{
  double x,w;
  double e;
  int i;

  x = 302.4241917;
  e = .00006769996759;

  printf("Inputs: \n");
  printf("    x = %25.16lf\n",x);
  printf("    e = %25.16lf\n",e);
  printf("    i = 1\n");
  w = enu(x,e,1);

  printf("output true anomaly      = %25.16lf \n",w);

  x = w;
  printf("Inputs: \n");
  printf("    x = %25.16lf\n",x);
  printf("    e = %25.16lf\n",e);
  printf("    i = 0\n");

  w = enu(x,e,0);

  printf("output eccentric anomaly = %25.16lf \n",w);

}
  

double enu(double x, double e, int i)

{

  double cosy,y;

  x = x * DTR;
  if (i) {
    // Convert eccen to true anomaly
    cosy = (e - cos(x))/(e*cos(x) - 1);
  } else {
    // Convert true to eccen anomaly
    cosy = (e + cos(x))/(1+e*cos(x));
  }
  y = acos(cosy);	 // y is 0 to pi
  if (x > PI)
    y = TWOPI - y;
  return(y*RTD);

}
