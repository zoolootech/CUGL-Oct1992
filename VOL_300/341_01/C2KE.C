/*
HEADER:       c2ke.c		
TITLE:        Cartesian-to-Keplerian conversion routine 		
VERSION:      1.0	
DESCRIPTION:  This routine converts Cartesian xyz position and velocity
              elements to the six classical Keplerian orbital
              elements.  In addition, this routine outputs the orbit
              period, true and eccentric anomalies, and semi-latus
              rectum.
              Required inputs are the Cartesian position and velocity
              plus mu for the central body.  	
KEYWORDS:     Astrodynamics, orbital mechanics, Keplerian elements,
              Cartesian coordinates	
SYSTEM:       MS-DOS, PC-DOS (Coded with Ver. 3.3, but should be version
              independent) 		
FILENAME:     c2ke.c	
UNITS:        I believe that the code is not dependent on any particular
              set of units.  See orbcons.h for the value of earth mu
              I use.
WARNINGS:	  This routine was coded for educational purposes, using
              conversion techniques given in the reference.  No attempt
              has been made to provide conversions with optimal numerical
              stability, although some simple checks have been included
              to flag exceptional conditions, such as equatorial or circular
              orbits.  Note that not every Keplerian element is defined
              for every type of orbit.  For example, circular orbits have
              no defined argument of periapsis (w); equatorial orbits have
              no defined longitude of the ascending node (lan).
SEE-ALSO:     k2ce.c, orbcons.h	
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

main()
{
  double c[6],k[6],mu,aux[4];
  char filename[31];
  FILE *fp;

  printf("Enter input file name: \n");
  gets(filename);


  fp = fopen(filename,"r");
  printf("filename = %s\n",filename);
  if ( fp == NULL) {
    perror("Open error \n");
    exit(0);
  }
  fscanf(fp,"%lf %lf %lf",&c[0],&c[1],&c[2]);
  fscanf(fp,"%lf %lf %lf",&c[3],&c[4],&c[5]);
  fscanf(fp,"%lf",&mu); 
  printf("Input pos/vel: \n");
  printf("%25.16lf %25.16lf %25.16lf\n",c[0],c[1],c[2]); 
  printf("%25.16lf %25.16lf %25.16lf\n",c[3],c[4],c[5]);
  printf("Input mu: %25.16lf\n",mu);

  c2ke(c,mu,k,aux);

  printf("Output elements (a,e,i,lan,w,m):\n");
  printf("%25.16lf %25.16lf %25.16lf\n",k[0],k[1],k[2]); 
  printf("%25.16lf %25.16lf %25.16lf\n",k[3],k[4],k[5]);
  printf("True anom, eccen anom, semi-latus rectum: \n");
  printf("%25.16lf %25.16lf %25.16lf\n",aux[0],aux[1],aux[2]);
  printf("Period: \n");
  printf("%25.16lf\n",aux[3]);


  
}

//  This routine converts Cartesian position and velocity
//    to classical Keplerian elements, for elliptical
//    orbits.

//  Input : c[6] -- pos/vel; 
//          mu   -- mu of central body
//  Output: k[6] -- a,e,i,lan,w,m
//          ** i,lan,w,m are output in degrees **        


  c2ke(double *c,double mu,double *k, double *aux)
{
		double a,e,i,lan,m,p,w;
      double hmagsq, rmag, vmag, vmagsq, rdv;
      double h1,h2,h3;
      double x,y,z,xd,yd,zd;
      double ee, e1, e2, e3;
      double n1, n2;
      double t, u;
      double cosnu, nde, nu, period;

      x  = c[0];		   // Get input pos
      y  = c[1];
      z  = c[2];
      xd = c[3];        // Get input vel
      yd = c[4];
      zd = c[5];

      rmag   = sqrt(x*x   + y*y   + z*z);   // Get pos/vel mag
      vmagsq = xd*xd + yd*yd + zd*zd;
      vmag   = sqrt(vmagsq);
      rdv    = x*xd + y*yd +z*zd;				// Get pos dot vel



      // Calculate angular momentum vector hbar =
      //   (h1,h2,h3) = pos cross vel.
      //   hbar is orthogonal to the orbit plane.    
      h1 = y*zd - z*yd; 
      h2 = z*xd - x*zd;
      h3 = x*yd - y*xd;
		hmagsq = h1*h1 + h2*h2 + h3*h3;


		// Node vector nbar =
      //   (-h2,h1,0).
      //   nbar is in the orbit plane and points from
      //   orbit focus to the ascending node.
      n1 = -h2;
      n2 =  h1;
 
 		// Calculate eccentricity vector ebar =
      //  (e1,e2,e3).
      //  ebar is in the orbit plane and points from
      //  the orbit focus toward periapis.  The magnitude
      //  of ebar is the eccentricity.
      t = vmagsq - mu/rmag;
      u = 1/mu;
      e1	= u * ( t * x - rdv * xd);
      e2 = u * ( t * y - rdv * yd);
      e3 = u * ( t * z - rdv * zd);


			                // Calculate semi-latus rectum
      p   = hmagsq/mu;
							    // Calculate eccentricity
      e = sqrt(e1*e1 + e2*e2 + e3*e3);
                         // Calculate semi-major axis
      if ( e != 1 ) {
        a = p/(1 - e*e);     
      } else {
        a = 0;
        printf("a is infinite; orbit is parabolic \n");
      }
                         // Calculate inclination
      i    = atan2(sqrt(h1*h1 + h2*h2),h3);
      if (i < 0) i += TWOPI;
                         // Calculate long of asc node
      if (i == 0 ) {
        lan = 0;
        printf("lan is undefined; orbit is equatorial. \n");
      } else {
        // lan is angle between node vector nbar and
        //   the positive x-axis.
        lan = atan2(h1,-h2);
        if (lan < 0 ) lan += TWOPI;
      }

                        // Calculate arg of periapsis
      if ( e != 0 && ( fabs(n1) + fabs(n2) != 0 ) ) {
        nde = n1 * e1 + n2 * e2;
        w = acos(nde/(sqrt(n1*n1 + n2*n2) * e));
        if ( e3 < 0 ) {
          w = TWOPI - w;
        }
      } else {
        printf("e or node vec is zero; arg of perigee is undefined \n");
        w = 0;
      }
       

                        //	Calculate true anomaly
      cosnu = (e1*x + e2*y + e3*z)/(e * rmag);
      nu    = acos(cosnu);
      if (rdv <= 0 ) {
        nu = TWOPI - nu;
      }
      
					        // Calculate eccentric anomaly
      ee = acos((e + cosnu)/(1+ e* cosnu));
      if (rdv < 0) {
        ee = TWOPI - ee;
      } 
                        // Calculate mean anomaly
      m = ee - e*sin(ee);

                        // Calculate period
      period = (TWOPI/sqrt(mu)) * pow(a,1.5);


      // Output results

      k[0] = a;          // Semi-major axis		     
      k[1] = e;          // Eccentricity
      k[2] = i  *RTD;    // Inclination
      k[3] = lan*RTD;    // Long of ascending node
      k[4] = w  *RTD;    // Arg of periapsis
      k[5] = m  *RTD;    // Mean anomaly

      aux[0] = nu *RTD;  // True anomaly
      aux[1] = ee *RTD;  // Eccentric anomaly
      aux[2] = p;        // Semi-latus rectum
      aux[3] = period;   // Period 
      return(0);
}
