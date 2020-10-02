/*
HEADER:       twobdy.c		
TITLE:        Two-body, Keplerian Orbit Propagator which uses
              universal variables formulation. 		
VERSION:      1.0	
DESCRIPTION:  This routine propagates Cartesian position and velocity
              for a specified time interval.  It will optionally
              output partial derivative data.
              Required inputs are the initial Cartesian position and velocity,
              the time interval to propagate, mu for the central body,
              an initial guess for the solution to the generalized Kepler
              equation, and a switch to indicate whether partial derivative
              data is being requested.  
KEYWORDS:     Astrodynamics, orbital mechanics, orbit propagation,
              universal variables, Goodyear method, Cartesian coordinates	
SYSTEM:       MS-DOS, PC-DOS (Coded with Ver. 3.3, but should be version
              independent) 		
FILENAME:     twobdy.c	
UNITS:        I believe that the code is not dependent on any particular
              set of units.  See orbcons.h for the value of earth mu
              I use.
SEE-ALSO:     k2ce.c
WARNINGS:     This code was coded for educational purposes.  No attempt
              has been made to provide for optimal numerical processing.
              For a detailed reference to the algorithm, see the second
              reference.
AUTHORS:      Rodney Long
              19003 Swan Drive
              Gaithersburg, MD 20879 	
COMPILERS:    Microsoft C 5.1	
REFERENCE:    Bate, Mueller, White: Fundamentals of Astrodynamics
              Goodyear, W.H.,"A General Method for the
              Computation of Cartesian Coordinates and
              Partial Derivatives of the Two-Body Problem",
              NASA Contractor Report NASA CR 522.

*/
#include <io.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include "orbcons.h"


struct input {
  double s0[6];         // s0[6]: Pos/vel at time t0
  double tau;           // Time interval from t0 to t
  double mu;            // Mu from diff. eq. of motion
  double psi;  			// Initial approx for sol. of
                        //  Kepler's equation.
  int    auxind;        // 0=output pos/vel and final psi.
                        //  1=output auxiliary data.
} ;

struct output {
  double psi;        	// Final sol. to Kepler's equation.
  double s[6];	         // s[6]: Pos/vel at time t.
} ;

struct aux_output {
  double p[6][6];		   // p[6][6]: Partial derivs. of
                        //   s with respect to s0.
  double pi[6][6];   	// pi[6][6]: Inverse of p. 
  double pmu[6];			// pmu[6]: Partial derivs. of
                        //   s with respect to mu.
  double p0mu[6];			// p0mu[6]: Partial derivs. of
                        //   s0 with respect to mu.
  double acc[3];			// acc[3]: Acceleration at time t.
  double acc0[3];			// acc0[3]: Accel. at time t0.
  double r;    			// Orbit radius at time t.
  double r0; 	      	// Orbit radius at time t0.
}



main()
{
  int i,n;
  char filename[31];
  FILE *fp;


  struct input in;
  struct output out;
  struct aux_output aux;
  
  printf("Enter input file name: \n");
  gets(filename);


  fp = fopen(filename,"r");
  printf("filename = %s\n",filename);
  if ( fp == NULL) {
    perror("Open error \n");
    exit(0);
  }
  fscanf(fp,"%lf %lf %lf",&(in.s0[0]),&(in.s0[1]),&(in.s0[2]));
  fscanf(fp,"%lf %lf %lf",&(in.s0[3]),&(in.s0[4]),&(in.s0[5]));
  fscanf(fp,"%lf",&(in.tau)); 
  fscanf(fp,"%lf",&(in.mu)); 
  fscanf(fp,"%lf",&(in.psi)); 
  fscanf(fp,"%d",&(in.auxind));

  printf("%25.16lf %25.16lf %25.16lf\n",in.s0[0],in.s0[1],in.s0[2]);
  printf("%25.16lf %25.16lf %25.16lf\n",in.s0[3],in.s0[4],in.s0[5]);
  printf("%25.16lf\n",in.tau); 
  printf("%25.16lf\n",in.mu); 
  printf("%25.16lf\n",in.psi); 
  printf("%d\n",in.auxind);


  // For the sample orbit print out the pos and velocity
  //   at each quarter-orbit point.
  // The orbit period has been computed from
  //   period = ( TWOPI/sqrt(mu) ) * a**1.5.
  // The number given below is one quarter of the orbit
  //   period for the sample case, in seconds. 

  // NOTE THAT FOR THIS SPECIAL TEST, in.tau from the
  //  input file is overwritten.  In the normal case, you
  //  would remove the "for" loop below and just leave the
  //  call to twobdy() and the print of the outputs.  Note
  //  that if you want the partial derivative output, you must
  //  add print statements to output them.
 
  for (i=0; i<4; i++) {
    in.tau = (i+1)*1576.780105;    
    twobdy(&in,&out,&aux);

  printf("Output pos/vel: \n");
  printf("%25.16lf %25.16lf %25.16lf\n",out.s[0],out.s[1],out.s[2]); 
  printf("%25.16lf %25.16lf %25.16lf\n",out.s[3],out.s[4],out.s[5]);
  }
}

twobdy(struct input *in, struct output *out, struct aux_output *aux)


//  twobdy:  A function to compute the position and
//           velocity of an orbiting body under
//           two-body motion, given the position and
//           velocity at time t0, the elapsed time tau
//           at which to compute the solution, the value
//           for mu for the two bodies, and an initial
//           approximation psi of the solution to
//           the generalized Kepler's equation.
//
//  References:
//    (1)  Goodyear, W.H.,"A General Method for the
//         Computation of Cartesian Coordinates and
//         Partial Derivatives of the Two-Body Problem",
//         NASA Contractor Report NASA CR 522.
//    (2)  Bate, Mueller, White, Fundamentals of
//         Astrodynamics. 

{

#define SQ(x) (x*x)
#define LARGENUMBER 1.e+300


// Additional variables
   int    its;                      // Iteration counter
   double a, ap;		    // Argument in series
                                    //   expansion.
   double alpha, sig0, u;           // Temporary quantities
   double c0, c1, c2, c3, c4, c5x3, // Series coefficents
          s1, s2, s3;             
   double psin, psip;               // Acceptance bounds for
                                    //   sol. psi to Kep. eq.    
   double dtau,dtaun, dtaup;        // Residuals in Newton
                                    //  method for solving
				    //  Kep. eq. 
   double fm1, g;                   // f,g &
   double fd, gdm1;	            //   fdot, gdot express.

   double acc[3] , acc0[3] ,
          mu     ,                  // Local copies of i/o
          p[6][6], pi[6][6],
  	       psi    ,                  //   variables
          r      , r0      ,
          s[6]   , s0[6]   ,
          tau;          

   int loopexit;                    // Flag to exit main
                                    //   program loop.

   int i,j,m;

// Copy the input pos, vel, tau, psi into
//  local variables.
   for (i=0; i<6 ; i++)
     s0[i] = in->s0[i];
	mu  = in->mu;
   tau = in->tau;
   psi = in->psi;

  
//       Compute initial orbit radius
    r0    = sqrt(SQ(s0[0]) + SQ(s0[1]) + SQ(s0[2]));
//       Compute other intermediate quantities.
    sig0  = s0[0]*s0[3] + s0[1]*s0[4] + s0[2]*s0[5];
    alpha = SQ(s0[3]) + SQ(s0[4]) + SQ(s0[5]) - 2*mu/r0;
//       Initialize series mod count m to zero
    m = 0;


//---Establish initial psi and initial acceptance bounds
//---for psi.											 
    if (tau == 0) {	          // If input tau = 0, set
      psi = 0;	                  //  output psi to 0; else
    } else {                      //  initialize acceptance
      if (tau < 0) {	   	  //  bounds for psi, and
          psin  = -LARGENUMBER;	  //  initialize Newton
 		    psip  = 0;    //  method iteration 
          dtaun = psin;	          //  residuals.
          dtaup = -tau;
        } else {                  //tau > 0
          psin = 0;
          psip = LARGENUMBER;
          dtaun = -tau;
          dtaup = psip;
        }

        // If the input psi lies between bounds
        //  psin and psip, use it as a first approx.
        //  to a solution of Kepler's equation.
        // If not, we adjust the input psi before using
        //  it to solve Kepler.

        if (!( (psi > psin) && (psi < psip)) ) {
          // Try Newton's method for initial psi set equal to zero  
          psi = tau/r0;
          // Set psi = tau if Newton's method fails
          if (psi <= psin || psi >= psip)
            psi = tau;
        }
     }
//---
    loopexit = 0;
    its      = 0;
//-----  BEGINNING OF LOOP FOR SOLVING GENERALIZED KEP. EQ.
    do {
      its++;
      a = alpha * psi * psi;
      if (fabs(a) > 1) {
        ap = a;              // Save original value of a
        // Iterate until abs(a) <= 1.
        while (fabs(a) > 1) {
          m++;              // Keep track of number of
                            //  times reduce a.
          a *= 0.25;
        }
      }
     // Now compute "C series" terms:
     //   Note that they are functions of psi.
     c5x3 = (1+(1+(1+(1+(1+(1+(1+a/342)*a/272)*a/210)*a/156)
            *a/110)*a/72)*a/42)/40;
     c4   = (1+(1+(1+(1+(1+(1+(1+a/306)*a/240)*a/182)*a/132)
            *a/90)*a/56)*a/30)/24;
     c3 = (.5 + a*c5x3)/3;
     c2 = (.5 + a*c4);
     c1 = 1 + a*c3;
     c0 = 1 + a*c2;

     //   If we reduced "a" above, we have to adjust the
     //     C terms just computed.
     if (m>0) {
       while (m>0) {
         c1 = c1*c0;
         c0 = 2*c0*c0 - 1;
         m--;
        }

       c2 = (c0 -  1)/ap;
       c3 = (c1 -  1)/ap;
       c4 = (c2 - .5)/ap;
       c5x3 = (3 * c3-.5)/ap;
     }

     // Now use the C terms to compute the "S series" terms.
     s1 = c1 * psi;
     s2 = c2 * psi * psi;
     s3 = c3 * psi * psi * psi;

     g    = r0*s1 + sig0*s2;   // Compute g; used later to
                               //   get position.

     // Compute dtau, the function to be zeroed by the
     //   Newton method.
     dtau = (g + mu*s3) - tau;
     // r is the derivative of dtau with respect to psi.
     r    = fabs(r0*c0 + (sig0*s1 + mu*s2));

     // Check for convergence of iteration and
     //  reset bounds for psi.
      if (dtau == 0) {
        loopexit = 1;
        continue;            // Get out of loop if dtau==0. 
      } 
      else if (dtau < 0) {
        psin  = psi;
        dtaun = dtau;
      }
      else {                  // dtau > 0
        psip = psi;
        dtaup = dtau;
      }
      
      // This is the Newton step:
      //   x(n+1) = x(n) - y(n)/(dy/dx).
      psi = psi - dtau/r;


      // Accept psi for further iterations if it is
      // between bounds psin and psip.
      if ( (psi > psin) && (psi < psip) ) continue;

//--  -- -- Begin modifications to Newton method.
      // Try incrementing bound with dtau nearest zero by
      // the ratio 4*dtau/tau.
      if ( fabs(dtaun) < fabs(dtaup) )
        psi = psin * (1 - (4*dtaun)/tau);
      if ( fabs(dtaup) < fabs(dtaun) )
        psi = psip * (1 - (4*dtaup)/tau);
      if ( (psi > psin) && (psi < psip) ) continue;

      // Try doubling bound closest to zero.
      if (tau > 0)
        psi = psin + psin; 
		if (tau < 0)
        psi = psip + psip;
      if ( (psi > psin) && (psi < psip) ) continue;

      // Try interpolation between bounds.
      psi = psin + (psip - psin) * (-dtaun/(dtaup - dtaun));
      if ( (psi > psin) && (psi < psip) ) continue;

      // Try halving between bounds.
      psi = psin + (psip - psin) * .5;
      if ( (psi > psin) && (psi < psip) ) continue;
//-- -- --
      // If we still are not between bounds, we've improved
      //  the estimate of psi as much as possible.
      loopexit = 1;
      
    } while ( loopexit == 0 && its <= 10);
//-----  END OF LOOP FOR SOLVING GENERALIZED KEPLER EQ.

    // Compute remaining three of four functions fm1, g, fd,
    //   gdm1
    fm1  = -mu * s2 / r0;
    fd   = -mu * s1 / ( r0 * r);
    gdm1 = -mu * s2 / r;
 
    // Compute output solution for time t = t0 + tau
    for (i=0;i<3;i++) {
      // Position solution	at time t
      out->s[i]  = s[i] = s0[i] + (fm1 * s0[i] + g * s0[i+3]);
      // Velocity solution at time t
      out->s[i+3] = s[i+3] = (fd * s0[i] + gdm1 * s0[i+3])
                                              + s0[i+3];
      // Generalized eccentric anomaly for time t
      in->psi = psi;
    }
    // Output additional outputs if requested by user.
    if (in->auxind) {
      // Acceleration solution at time t
      aux->acc[i]  = acc[i]  = -mu * s[i]  / (r*r*r);
      // Acceleration solution at original time t0
      aux->acc0[i] = acc0[i] = -mu * s0[i] / (r*r*r);
		// Orbit radii at t and at t0
      aux->r0 = r0;
      aux->r  = r;

      // Partial derivative computations

      // Compute coefficients for state partials
      u   = s2*tau + mu*(c4 - c5x3) * psi*psi*psi*psi*psi;
      p[0][0] = -(fd*s1 + fm1/r0)/r0;
      p[0][1] = -fd*s2;
      p[1][0] =  fm1*s1/r0;
      p[1][1] =  fm1*s2;
      p[0][2] =  p[0][1];
      p[0][3] = -gdm1*s2;
      p[1][2] =  p[1][1];
      p[1][3] =  g*s2;
      p[2][0] = -fd*(c0/(r0*r) + 1/(r*r) + 1/(r0*r0));
      p[2][1] = -(fd*s1 + gdm1/r)/r;
      p[3][0] = -p[0][0];
      p[3][1] = -p[0][1];
      p[2][2] =  p[2][1];
      p[2][3] = -gdm1 + s1/r;
      p[3][2] = -p[0][1];
      p[3][3] = -p[0][3];

      // Compute coefficients for mu partials
      p[0][4] = -s1 / (r0*r);
      p[1][4] =  s2 / r0;
      p[2][4] =  u  / r0 - s3;
      p[0][5] = -p[0][4];
      p[1][5] =  s2 / r;
      p[2][5] = -u  / r + s3;

      // Compute mu partials
	   for (i=0;i<3;i++) {
        aux->pmu[i]    = -s[i]  * p[1][4]
                              + s[i+2]  *  p[2][4];
        aux->pmu[i+2]  =  s[i]  * p[0][4]
                              + s[i+2]  *  p[1][4]
                              + acc[i]  *  p[2][4];
        aux->p0mu[i]   = -s0[i] * p[1][5]
                              + s0[i+2] *  p[2][5];
        aux->p0mu[i+2] =  s0[i] * p[0][5]
                              + s0[i+2] *  p[1][5]
                              + acc0[i] *  p[2][5];

        // Matrix accumulations for state partials

        for (j=0;i<4;i++) {
          pi[j][i]   = p[j][0] * s0[i]
                               + p[j][1] * s0[i+2];
          pi[j][i+2] = p[j][2] * s0[i]
                               + p[j][3] * s0[i+2];
        }
      }

      for (i=0;i<3;i++) {
       for (j=0;j<3;j++) {
         p[i][j]   = s[i] * pi[0][j]   + s[i+2] * pi[1][j]
                            + u * s[i+2] * acc0[j];
         p[i][j+3] = s[i] * pi[0][j+2] + s[i+2] * pi[1][j+2]
                          - u * s[i+2] * s0[j+2];
         p[i+2][j] = s[i] * pi[2][j] + s[i+2] * pi[3][j] 
                            + u * acc[i] * acc0[j];
         p[i+2][j+2] = s[i] * pi[2][j+2] + s[i+2] * pi[3][j+2]
                           -u * acc[i] * s0[j+3];
       }

       p[i][i]      = p[i][i]     + fm1 + 1;
 	    p[i][i+2]    = p[i][i+2]   + g;
       p[i+2][i]    = p[i+2][i]   + fd;
       p[i+2][i+2]  = p[i+2][i+2] + gdm1 + 1;
     }

     // Transpositions for inverse state partials
     for (i=0;i<3;i++) {
       for (j=0;j<3;j++) {
         pi[j+2][i+2]   =  p[i][j];
         pi[j+2][i]     = -p[i+2][j];
         pi[j][i+2]     = -p[i][j+2];
         pi[j][i]       =  p[i+2][j+2];
       }
     }

     // Output state partials and inverse state partials.
     for (i=0;i<6;i++) {
       for (j=0;j<6;j++) {
         aux->p[i][j]  = p[i][j];
         aux->pi[i][j] = pi[i][j];
       }
     }
   } 

   return(0);
} 

