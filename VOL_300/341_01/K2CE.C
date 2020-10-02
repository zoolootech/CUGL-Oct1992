/*
HEADER:       k2ce.c		
TITLE:        Keplerian-to-Cartesian conversion routine 		
VERSION:      1.0	
DESCRIPTION:  This routine converts the six classical Keplerian orbital
              elements to Cartesian xyz position and velocity elements.
              Required inputs are the six Keplerian elements plus mu for
              the central body.  	
KEYWORDS:     Astrodynamics, orbital mechanics, Keplerian elements,
              Cartesian coordinates	
SYSTEM:       MS-DOS, PC-DOS (Coded with Ver. 3.3, but should be version
              independent) 		
FILENAME:     k2ce.c	
UNITS:        I believe that the code is not dependent on any particular
              set of units.  See orbcons.h for the value of earth mu
              I use.
WARNINGS:	  This version is for elliptical input only.
              Input elements should have a > 0 and e < 1.
              This routine was coded for educational purposes, following
              the development in the reference.  No attempt has been
              made to provide conversions which are optimally numerically
              stable.            
SEE-ALSO:     c2ke.c	
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

double sk();

main()
{
  double c[6],k[6],mu;
  char filename[31];
  FILE *fp;
  int rc;

  printf("Enter input file name: \n");
  gets(filename);


  fp = fopen(filename,"r");
  printf("filename = %s\n",filename);
  if ( fp == NULL) {
    perror("Open error \n");
    exit(0);
  }
  fscanf(fp,"%lf %lf %lf",&k[0],&k[1],&k[2]);
  fscanf(fp,"%lf %lf %lf",&k[3],&k[4],&k[5]);
  fscanf(fp,"%lf",&mu); 
  printf("Input elements (a,e,i,lan,w,m): \n");
  printf("%25.16lf %25.16lf %25.16lf\n",k[0],k[1],k[2]); 
  printf("%25.16lf %25.16lf %25.16lf\n",k[3],k[4],k[5]);
  printf("Input mu: %25.16lf\n",mu);


  k[2] *= DTR;
  k[3] *= DTR;
  k[4] *= DTR;
  k[5] *= DTR;

  rc = k2c(k,mu,c);

  if (rc == -1 ) {
    printf("Input orbit is not elliptic. \n");
  } else {
    printf("Output pos/vel: \n");
    printf("%25.16lf %25.16lf %25.16lf\n",c[0],c[1],c[2]); 
    printf("%25.16lf %25.16lf %25.16lf\n",c[3],c[4],c[5]);
  }
}

//  This routine converts classical Keplerian elements to
//    Cartesian position and velocity.

//  Input : k[6] -- a, e, i, lan, w, m	; 
//          mu   -- mu of central body
//  Output: c[6] -- x, y, z, xdot, ydot, zdot  

  k2c (double *k, double mu, double *c)
{   
		int max = 10;
      double tol = .5e-16;
      double r11, r21, r31;
      double r12, r22, r32;
      double cee, see;
      double ci, cw, clan; 
      double si, sw, slan; 
      

      double a, e, ee, e0, i, lan, m, w;
      double xp,yp,xpd,ypd, r;

      // Get input vector elements into local variables
      //   with logical names.
      a   = k[0];       // Semi-major axis.
      e   = k[1];       // Eccentricity.
      i   = k[2];       // Inclination.  (rad)
      lan = k[3];       // Longitude of ascending node. (rad)
      w   = k[4];       // Argument of perigee. (rad)  
		m   = k[5]*RTD;   // Mean anomaly. (deg)

      // If orbit is elliptical, proceed.  If not
      //  print warning and halt.
		if ( a > 0  & e < 1  ) { 
        // Orbit is elliptical.
      
        e0 = m;                   // Make Kepler starting iterate
                                  //  = input mean anomaly.
        // Call sk() to solve Kepler.
        ee = sk(m,e0,e,max,tol);
											 
        ee  = ee * DTR;           // Eccen anomaly is output in
                                  //   degrees by sk(); convert it
                                  //   to radians.          
        cee = cos(ee);            // Cos and sin of
        see = sin(ee);            //  eccen anomaly output
                                  //  by Kepler solution.

        r    = a * (1 - e * cee);        // Compute orbit radius at
                                         //   current position. 
      
        xp   = a * (cee - e);            // Compute perifocal
        yp   = a * (sqrt(1-e*e) * see);  //   (x,y) of position.

        xpd  = (-sqrt(mu*a)*see)/r;         // Compute perifocal
        ypd  = sqrt(mu/(a * (1-e*e) ) ) *  // (x,y) of velocity. 
                (e + (e - cee)/(e * cee - 1) );

		} else { 
        printf("Input orbit is not elliptic. \n");
        return(-1);

      }

      // Now compute remaining pos/vel coordinates.

      ci   =  cos(i);     // Cos, sin
      si   =  sin(i);     //   of inclination.
      clan =  cos(lan);   // Cos, sin
      slan =  sin(lan);   //   of longitude of ascending node.
      cw   =  cos(w);     // Cos, sin
      sw   =  sin(w);     //   of arg of perigee.

      // Compute matrix to convert from perifocal
      //   to geocentric-equatorial coordinates.
      r11   =  cw * clan - sw * slan * ci;
      r21   =  cw * slan + sw * clan * ci;
      r31   =  sw * si;
      r12   = -sw * clan - cw * slan * ci;
      r22   = -sw * slan + cw * clan * ci;
      r32   =  cw * si; 


      // Now apply the matrix to compute
      //   the output position and velocity.

      c[0] = r11 * xp  + r12 * yp;
      c[1] = r21 * xp  + r22 * yp;
      c[2] = r31 * xp  + r32 * yp;
      c[3] = r11 * xpd + r12 * ypd;
      c[4] = r21 * xpd + r22 * ypd;
      c[5] = r31 * xpd + r32 * ypd;

      return(0);

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