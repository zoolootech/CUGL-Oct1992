

 /*  **** CTRIG.C *****
A group of programs in C, using the BDS-C floating point package,
as modified by L. C. Calhoun called FLOATXT, which compute some
commonly used transcendental functions - to wit*
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
	char *fpmult(),radindeg[5];
	initb (radindeg,"217,27,125,71,251");
	fpmult(rad,deg,radindeg);
	return (rad);
}

char *radtodeg(deg,rad) /* 5 char fp arguments */
char *deg, *rad;
{
	char *fpmult(),deginrad[5];
	initb (deginrad,"10,114,151,114,6");
	fpmult(deg,rad,deginrad);
	return (deg);
}

/* service function sinev which evaluates when range of angle
reduced to plus or minus pi/2 (90 deg) */

char *sinev(result,angle)

char *result, angle[5];
{
	char *fpmult(),x[5],xsq[5];
	char coef[5][5],termreslt[5];
	char *fpadd(),*fpasg();
	int index;

	/*  use the exponent part of the floating point
	    to check for threat of underflow  use small
	    angle approximation if appropriate  */
	if ( (angle[4] > 128) && (angle[4] < 226) )
	   {fpasg(result,angle);	   return (result);
	  }   /* solution to fpmult underflow problem */

/* series coef are 1., -.1666666, .008333026, -.0001980742,
	.000002601887  determined from coefset program */
	initb(coef[0],"0,0,0,64,1");
	initb(coef[1],"111,170,170,170,254");
	initb(coef[2],"185,162,67,68,250");
	initb(coef[3],"208,234,38,152,244");
	initb(coef[4],"166,13,78,87,238");
	fpasg(x,angle);
	fpmult(xsq,x,x);
	setmem(result,5,0);
 /* to this point the coef have been initialized, the angle
    copied to x, x squared computed, and the result initialized */

/* now to do the polynomial approximation */
	index = 0;
	while ( (index <= 4) && ( (x[4] > 194) || (x[4] < 64) ) )
	/* use index for loop, and exponent of x to avoid underflow
	   problems */
	{fpmult(termreslt,coef[index],x);
	fpadd(result,result,termreslt);
	 index++;
	 fpmult(x,x,xsq);
	}
	return (result);
}

 /* here is sine(result,angle) with angle in radians */

char *sine(result,angle)

char *result, *angle;
{
	char *fpmult(),twopi[5],halfpi[5];
	char mtwopi[5],mhalfpi[5],*fpasg(),*fpchs();
	char pi[5],*sinev(),*fpadd();
	char y[5],*fpsub();
	int fpcomp(), compar;
	int signsine;
 /* some initialization required here */
	signsine = 1;
	initb(twopi,"121,238,135,100,3");
	initb(halfpi,"121,238,135,100,1");
	initb(pi,"244,239,135,100,2");
	initb(mtwopi,"135,17,120,155,3");
	initb(mhalfpi,"135,17,120,155,1");
	fpasg(y,angle);
	while (fpcomp(y,twopi) >= 0)
	   {fpsub(y,y,twopi);
	   }
	while (fpcomp(y,mtwopi)<= 0)
	   {fpadd(y,y,twopi);
	   }
	if(fpcomp(y,halfpi) > 0)
	   {signsine *=-1; fpsub (y,y,pi);
	   }
	if(fpcomp(y,mhalfpi)<  0)
	   {signsine *=-1; fpadd (y,y,pi);
	   }
	sinev(result,y);
	if (signsine > 0) return (result);
 /* minus so need to change sign */
	else return ( fpchs(result,result) );
}

 /* cosine(result,angle) with angle in radians  - uses sine */

char *cosine(result,angle)

char *result, *angle;
{
	char *sine(),*fpsub(),halfpi[5],y[5];
	initb(halfpi,"121,238,135,100,1");
	fpsub(y,halfpi,angle);
	sine(result,y);
	return (result);
}

/* tangent(result,angle) with angle in radians, returns very 
large number for divide by zero condition */

char *tangent(result,angle)
char *result, angle[5];
{
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
}

/* atanev(result,x) evaluates arctangent for 0 <= x < infinity,
	result in radians */

char *atanev(result,x)

char  result[5], x[5];
{
	char coef[8][5],piover4[5],y[5];
	int index;
	char *fpadd(),*fpmult(),*atof(),ysq[5],one[5];
	char yterm[5],termreslt[5],*fpasg();

	initb(piover4,"121,238,135,100,0");
	initb(one,"0,0,0,64,1");

	/* small angle approximation */
	if ( (x[4] > 128) && (x[4] < 226) )
	/* use fp exponent to check size, use small angle */
	 return ( fpasg(result,x) );
	else
	fpasg(result,piover4);

	/* check for argument near one */
	fpsub(yterm,x,one);
	/* if it is close to one, as checked by exponent,
	   return the result pi/4  */
	if ( (yterm[4] > 128) && (yterm[4] < 243) )
	   return (result);


	initb(coef[0],"184,254,255,127,0");
	initb(coef[1],"191,239,172,170,255");
	initb(coef[2],"70,86,32,102,254");
	initb(coef[3],"102,203,201,184,254");
	initb(coef[4],"28,240,187,98,253");
	initb(coef[5],"134,24,127,141,252");
	initb(coef[6],"108,44,139,89,251");
	initb(coef[7],"55,10,148,189,249");
	fpadd(termreslt,x,one);
	fpdiv(y,yterm,termreslt);

	fpmult(ysq,y,y);
/* do poly evaluation */
	index = 0;
	/* poly evaluation checked by index limit, and check of
	variables for under/over flow */
	while ( (index <= 7) && ( (y[4]<100) || (y[4]>140) ) )
	 {fpmult(termreslt,coef[index],y);
	  fpadd(result,result,termreslt);
	  index++;
	  fpmult(y,y,ysq);
	 }

	return (result);
}

/* arctan(result,angle) is floating point arctangent evaluation */

arctan(result,x)
char result[5], x[5];

{
	char *atanev(),*fpasg(),y[5];
	char *fpchs(),halfpi[5];
	int index;
	/* check exponent for very large argument */
	if ( (x[4] > 100) && (x[4] <= 128) )
	   {initb(halfpi,"121,238,135,100,1");
	    fpasg(result,halfpi);
	   }
	else  /* go through evaluation */
	  {fpmag(y,x);
	   atanev(result,y);
	  }

	if ( x[3] > 127 )
	  {return ( fpchs(result,result) );}
	else return (result);
}


