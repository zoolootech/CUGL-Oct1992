/* cmath.c 1984 apr 30 pmk - 
	combine fpsqrt + clogs + ctrig 
	move consts to float.h & coef's to coef.h
	speedup inner eval loops */
#include "float.h"
#include "coef.h"

char *fpsqrt (z, xin) char *z, *xin ; {	/* z = sqrt (x)	*/
#define ITS 6	/* iteration count */
	float x, z2, x2 ;	unsigned n ;
	fpasg (z, xin) ; fpasg (x, xin) ; 
	z[4] = (z[4] >> 1) | (0x80 & z[4]) ;
	for (n = ITS ; n ; --n) 	{
		/* Newton - Raphson, ITS iterations */
		fpdiv (z2, x, z) ;	/* z2 = x / z ; */
		fpadd (z, z, z2) ;	/* z += z2 ;    */
		fpmult (z, HALF, z) ;	/* z *= half ;  */
	} /* for ITS */
	return z ;
#undef ITS
} /* fpsqrt */

 /* CTRIG.C *****
	sine, cosine, tangent and arctangent
	convert degrees to radians, convert radians to degrees
These functions are discussed in detail in CTRIG.DOC

L. C. Calhoun
257 South Broadway
Lebanon, Ohio 45036   513 932 4541/433 7510
  */

/* simple ones first converting degrees - radians */

char *degtorad(rad,deg) /*obvious arguments in 5 char fp */
char *rad, *deg;
{
	char *fpmult();
	fpmult(rad,deg,RADINDEG);
	return (rad);
} /* degtorad */

char *radtodeg(deg,rad) /* 5 char fp arguments */
char *deg, *rad;
{
	char *fpmult();
	fpmult(deg,rad,DEGINRAD);
	return (deg);
} /* radtodeg */

/* service function sinev which evaluates when range of angle
reduced to plus or minus pi/2 (90 deg) */
char *sinev(result,x)	char *result, x[5];	{
	char *fpmult(),xsq[5];	char *coef[5];
	char *fpadd(),*fpasg();
	int index;

	/*  use the exponent part of the floating point
	    to check for threat of underflow  use small
	    angle approximation if appropriate  */
	if ( (x[4] > 128) && (x[4] < 226) )
	   {fpasg(result,x);	   return (result);
	  }   /* solution to fpmult underflow problem */

/* series coef are 1., -.1666666, .008333026, -.0001980742,
	.000002601887  determined from coefset program */
	coef[0] = SINC0 ;	coef[1] = SINC1 ;
	coef[2] = SINC2 ;	coef[3] = SINC3 ;
	coef[4] = SINC4 ;	fpmult(xsq,x,x) ;
 /* to this point the coef have been initialized, 
    x squared computed. */

/* now to do the polynomial approximation */
	fpasg (result, coef[4]) ;
	for (index = 3 ; 0 <= index ; --index) {
		fpadd (result, coef[index],
			fpmult (result, result, xsq)) ;
	} /* this loop approx 1/3 faster than original */
	fpmult (result, result, x) ;
	return (result);
} /* sinev */

 /* here is sine(result,angle) with angle in radians */
char *sine(result,angle)	char *result, *angle;	{
	char *fpmult(),*twopi,*halfpi;
	char *mtwopi,*mhalfpi,*fpasg(),*fpchs();
	char *pi,*sinev(),*fpadd();	char y[5],*fpsub();
	int fpcomp(), compar;	int signsine;
 /* some initialization required here */
	signsine = 1;
	twopi	= TWOPI ;	halfpi	= HALFPI ;
	pi	= PI ;		mtwopi	= MTWOPI ;
	mhalfpi	= MHALFPI ;	fpasg(y,angle);
	while (fpcomp(y,twopi) >= 0)
	   fpsub(y,y,twopi);
	while (fpcomp(y,mtwopi)<= 0)
	   fpadd(y,y,twopi);
	if(fpcomp(y,halfpi) > 0)
	   { signsine *=-1; fpsub (y,y,pi);	   }
	if(fpcomp(y,mhalfpi)<  0)
	   { signsine *=-1; fpadd (y,y,pi);	   }
	sinev(result,y);
	if (signsine > 0) return (result);
 /* minus so need to change sign */
	else return ( fpchs(result,result) );
} /* sine */

 /* cosine(result,angle) with angle in radians  - uses sine */
char *cosine(result,angle)	char *result, *angle;	{
	char *sine(),*fpsub(),y[5];
	fpsub(y,HALFPI,angle);
	sine(result,y);
	return (result);
} /* cosine */

/* tangent(result,angle) with angle in radians, returns very 
large number for divide by zero condition */
char *tangent(result,angle)	char *result, angle[5];	{
	char *sine(), *cosine(), *fpdiv(), zero[5];
	char sresult[5], cresult[5], intres[5], big[5];
	char *fpmult(), *fpmag();

	sine(sresult,angle);
	cosine(cresult,angle);
	/* check magnitude of denominator :*/
	/* check magnitude of denominator using exponent */
	if ( (cresult[4] > 128) && (cresult[4] < 132) )
	   {initb(big,"30,207,228,127,128"); /*big number */
	    if ( sresult[3] > 127 ) /*use mantissa sign */
	       return ( fpchs(result,big) );
	    else return ( fpasg(result,big) );
	   }
	/* check for small angle, use small angle approx to
	   avoid underflow */
	if ( (angle[4] < 226) && (angle[4] > 128) )
	   return ( fpasg(result,angle) );
	else
	   return ( fpdiv(result,sresult,cresult) );
} /* tangent */

/* atanev(result,x) evaluates arctangent for 0 <= x < infinity,
	result in radians */
char *atanev(result,x)	char  result[5], x[5];	{
	char *coef[8],y[5];	int index;
	char *fpadd(),*fpmult(),*atof(),ysq[5];
	char yterm[5],termreslt[5],*fpasg();

	/* small angle approximation */
	if ( (x[4] > 128) && (x[4] < 226) )
	/* use fp exponent to check size, use small angle */
	 return ( fpasg(result,x) );

	fpasg(result,PIOVER4);
	/* check for argument near one */
	fpsub(yterm,x,ONE);
	if ( (yterm[4] > 128) && (yterm[4] < 243) )
	   return (result);

	coef[0] = ATNC0 ;	coef[1] = ATNC1 ;
	coef[2] = ATNC2 ;	coef[3] = ATNC3 ;
	coef[4] = ATNC4 ;	coef[5] = ATNC5 ;
	coef[6] = ATNC6 ;	coef[7] = ATNC7 ;

	fpadd(termreslt,x,ONE);
	fpdiv(y,yterm,termreslt);
	fpmult(ysq,y,y);
 /* do poly evaluation, fast loop */
	for (index = 6, fpasg (result, coef[7]) ; 0 <= index ;
		--index)
		fpadd (result, coef[index], 
			fpmult (result, result, ysq)) ;
	fpadd (result, PIOVER4, fpmult (result, result, y) ) ;
	return (result);
} /* atanev */

/* arctan(result,angle) is floating point arctangent evaluation */
char *arctan(result,x)	char result[5], x[5];	{
	char *atanev(),*fpasg(),y[5];
	char *fpchs();	int index;
	/* check exponent for very large argument */
	if ( (x[4] > 100) && (x[4] <= 128) )
	    fpasg(result,HALFPI);
	else  /* go through evaluation */
	  { fpmag(y,x);   atanev(result,y);  }

	if ( x[3] > 127 )
	   return ( fpchs(result,result) ); 
	else return (result);
} /* arctan */

char *arctan2 (result, quadrant, opside, adjside)
char result[5], opside[5], adjside[5];	int *quadrant;	{
	char x[5], *fpmag(), *fpchs(); *arctan();
	int opsign, adjsign;
	char *zero ;	zero = ZERO ;

	opsign = fpcomp(opside,zero);
	adjsign = fpcomp(adjside,zero);

	if((adjsign == 0) && (opsign == 0))
	   { *quadrant = 0;    fpasg(result,zero);
	    return(result);   }

	if ( ( (128 < adjside[4]) && (adjside[4] < 226) ) 
	|| (adjsign == 0) )
	   fpasg(result,HALFPI);
	else
	   { fpdiv(x,opside,adjside);
	    fpmag(x,x);    arctan(result,x);   }

	if ( (adjsign == 0) && (opsign >  0) )
	   { *quadrant = 1;    return(result);   }
	if ( (adjsign == 0) && (opsign <  0) )
	   { *quadrant = 4;    fpchs(result,result);
	    return(result);   }
	if ( (adjsign >  0) && (opsign == 0) )
	   { *quadrant = 1;    return(result);   }
	if ( (adjsign < 0 ) && (opsign == 0) )
	   { *quadrant = 2;    fpchs(result,result);
	    return(result);   }
	if ( (adjsign > 0) && (opsign > 0) )
	   { *quadrant = 1;    return(result);   }
	if ( (adjsign > 0) && (opsign < 0) )
	   { *quadrant = 4;    fpchs(result,result);
	    return(result);   }
	if ( (adjsign < 0) && (opsign > 0) )
	   { *quadrant = 2;    fpchs(result,result);
	    return(result);   }
	if ( (adjsign < 0) && (opsign < 0) )
	   { *quadrant = 3;    return (result);   }
} /* arctan2 */

/*  CLOGS.C       */

char *pi(result)	char *result;	{
	char *fpasg();
	return (fpasg(result,PI) );
} /* pi */

char *expe(result,xin)
 /* computes the base of the natural log "e" raised to the "x'th"
    power.  Checks are made for out of range values and result is
    defaulted to 0, 1, or a large number as appropriate.  There are
    no error flags.  The arguments are floating point character
    arrays char result[5], x[5]; in calling program.  Return is
    a pointer to result, and "e to the x" stored in result.
 */
char *result, *xin ;	{
	char *one, *coef[7], x[5] ;
	char  *fpmult(), *fpasg(), *fpdiv(), *fpchs();
	int signx, index, bigexp, smallexp, zeroin;
	int exprange();

	bigexp = smallexp = zeroin = 0;	one = ONE ;

/* preserve arg before transforms */
	fpasg (x, xin) ;

	coef[0] = EXPC0 ;	coef[1] = EXPC1 ;
	coef[2] = EXPC2 ;	coef[3] = EXPC3 ;
	coef[4] = EXPC4 ;	coef[5] = EXPC5 ;
	coef[6] = EXPC6 ;

 /* check for sign */
	signx = 0 ;
	if (x[3] < 128)  signx = 1; /* positive */
	else fpchs(x,x) ;

   /* check for zero and very small numbers */
	if ( ((127 < x[4]) && (x[4] < 226)) 
	|| ( (x[4]==0) && (x[3]==0) && (x[2]==0) 
		&& (x[1]==0) && (x[0]==0) ) )
	    zeroin = 1;

   /* check for very small exponent */
	if ( fpcomp(x,MEGHTY6) < 0 )
	    smallexp = 1;

  /*  check for very large exponent */
	if ( fpcomp(x,EGHTY6) > 0 )
	    bigexp = 1;

  /*  now if small number or zero, result is one */
  /*  now if big number and positive, result is large number */
  /*  now if big number and negative, result is zero */

	if (zeroin)	return (fpasg(result,one) );
	if (smallexp)	return (fpasg(result,ZERO) );
	if (bigexp)	return (fpasg(result,LARGE) );

 /*  all exceptions taken care of, so evaluate rest  */
	    fpasg(result,one);
	/* fast loop */
	    for (index = 5, fpasg (result, coef[6]); 0<=index ;
		--index)
		fpadd (result, coef[index],
			fpmult (result, result, x) ) ;

  /* now do the square square */
	fpmult(result,result,result);
	fpmult(result,result,result);

  /* now treat sign appropriately */
	if (signx)	return (result);
	else		return fpdiv (result, one, result) ;
} /* expe */

char *exp10(result,xin)
/* similar to expe, except result returned is 10 raised to the
 xth power:   the antilogarithm to base 10  */
char *result, *xin;	{
	char *one ;
	char *coef[7], tenfac[5], x[5] ;
	int index, bigexp, smallexp, signx, tenpower;
	int exprange();
	bigexp = smallexp = 0;	one =  ONE ;

	coef[0] = ONE ;
	coef[1] = E10C1 ;	coef[2] = E10C2 ;
	coef[3] = E10C3 ;	coef[4] = E10C4 ;
	coef[5] = E10C5 ;	coef[6] = E10C6 ;

 /* check for sign */
	signx = 0 ;	fpasg (x, xin) ;
	if (x[3] < 128)	/* positive */   signx = 1;
	else    	/* negative */   fpchs(x,x);

  /* check for very small or large numbers, check by exponent size */
  /* check for zero or small */
	if ( ((127 < x[4]) && (x[4] < 226)) 
	|| ( (x[4]==0) && (x[3]==0) && (x[2]==0) 
		&& (x[1]==0) && (x[0]==0) ) )
	    smallexp = 1;
   /* check for big number */
	if ( fpcomp(x,THTY8) > 0)
	    bigexp = 1;

 /* if value is small or zero, return 1 as with expe */
 /* if value is large and positive, return a large number */
 /* if value is large and negative, return zero */
	if (smallexp)		return (fpasg(result,one) );
	if (bigexp && signx)	return (fpasg(result,LARGE) );
	if (bigexp && !signx)	return (fpasg(result,ZERO) );

 /* now reduce range of x to between zero and one */
	tenpower = ftoit(x);	itof(tenfac,tenpower);
	fpsub(x,x,tenfac);	fpasg(tenfac,one);
	while (tenpower--)
	    fpmult(tenfac,tenfac,TEN);

 /* now evaluate series, fast loop */
	for (fpasg (result, coef[6]), index = 5 ; 0 <= index ;
		--index)
		fpadd (result, coef[index],
			fpmult (result, result, x) ) ;

 /* now square result (note error in referenced article) */
	fpmult(result,result,result);

 /* now check sign and make proper return */
	fpmult(result,result,tenfac);  /* scale back up */
	if (signx)	return (result);
	else		return ( fpdiv(result,one,result) );
} /* exp10 */

char *log10 (result, sign, xin)
 /* computes briggsian logarithm of x which is a char[5]
	floating point number.  Return is logarithm in result[5],
	and sign pointed to by sign.  The logarithm is taken
	of the magnitude, and sign information preserved
	as required by sign. */
char *result, *xin ;	int *sign ;	{
	char *zero, *ten, *one, *thty8, *sqrtten, x[5] ;
	char gamma[5], gamnum[5], gamden[5], *coef[5] ;
	char *half, intres[5] ;
	int tenpower, index, bigexp, smallexp, signx ;
	int exprange() ;

	bigexp = smallexp = 0 ;
	zero = ZERO ;		half = HALF ;	one = ONE ;
	sqrtten = SQRTTEN ;	ten = TEN ;	thty8 = THTY8 ;

	coef[0] = L10C0 ;	coef[1] = L10C1 ;
	coef[2] = L10C2 ;	coef[3] = L10C3 ;
	coef[4] = L10C4 ;

 /*  preserve input variable */
	fpasg (x, xin) ;

 /* check for sign */
	signx = -1 ;
	if (x[3] < 128)   /* positive */   signx = 1 ;
	else     /* negative */		   fpchs (x, x) ;
	*sign = signx ;

  /* check for very small or large numbers, check by exponent size */
  /* check for zero or small */
	if ( ( (127 < x[4]) && (x[4] < 209) ) 
	|| ( (x[4]==0) && (x[3]==0) && (x[2]==0) 
		&& (x[1]==0) && (x[0]==0) ) )
	    smallexp = 1 ;
   /* check for big number */
	if ( (47 < x[4]) && (x[4] < 128) )
	    bigexp = 1 ;

 /* if very small, return - a large number
    if very large, return + a large number  */
	if ( smallexp )	return ( fpchs (result, thty8) ) ;
	if ( bigexp )	return ( fpasg (result, thty8) ) ;

  /*  bring into range 1 <= x < 10 */
	tenpower = 0 ;
	while ( fpcomp (x, ten) >= 0 )
		{ tenpower++ ;	fpdiv (x, x, ten) ; }
	while ( fpcomp (x, one) < 0 )
		{ tenpower-- ;	fpmult (x, x, ten) ; }

  /* if exactly one, no need to evaluate */
	fpsub (gamnum, x, one) ;
	if ( ( (127 < gamnum[4]) && (gamnum[4] < 209) ) 
	|| ( (gamnum[0]==0) && (gamnum[1]==0) && 
		(gamnum[2] == 0) && (gamnum[3] == 0) ) )
		return ( itof (result, tenpower) ) ;
 /* compute gamma for series  */

	fpsub (gamnum, x, sqrtten) ;
  /* check for size of numerator */
	if ( ( (127 < gamnum[4]) && (gamnum[4] < 209) ) 
	|| ( (gamnum[0]==0) && (gamnum[1]==0) 
		&& (gamnum[2] == 0) && (gamnum[3] == 0) ) )
		{ itof (result, tenpower) ;
		return ( fpadd (result, result, half) ) ; }
	fpadd (gamden, x, sqrtten) ;
	fpdiv (gamma, gamnum, gamden) ;

 /* set up for series (use gamnum as gamma squared) */
	fpmult (gamnum, gamma, gamma) ;

 /* do series evaluation */
	for (fpasg (result, coef[4]), index = 3 ; 0 <= index ;
		--index)
		fpadd (result, coef[index],
			fpmult (result, result, gamnum) ) ;
	fpadd (result, half, 
		fpmult (result, result, gamma) ) ;

 /* do correction for range reduction */
	if ( tenpower != 0 )
		fpadd (result, result, 
			itof (intres, tenpower) ) ;

 /* return */
	return (result) ;
} /* log10 */

int exprange(x)
 /* The input argument is a floating point value from BDS C 
which consists of an array of 5 character data.  The function 
returns a 1 if the exponent is in the range of - 47 to + 47.  
Outside this range a value of 0 (false) is returned.  This is 
a range of ten to plus or minus 14 power */
char *x; { 
  if ( ((x[4]<128) && (x[4]>47) ) 
  || ((x[4]>127) && (x[4] < 209) ) )
	return (0);
  else return (1);
} /* exprange */

dd (gamden, x, sqrtten) ;
	fpdiv (gamma, gamnum, gamden) ;
