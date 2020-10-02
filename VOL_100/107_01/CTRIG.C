

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

 *** revision 13 July 1981 to 1) refine precision of constants,
 especially pi related values  2) to utilize string constants
 as pseudo static numeric constants.  THIS WILL ONLY WORK WITH
 BDS C V 1.44 (and hopefully later).
 3) to add ARCTAN2() function to identify quadrant

  */

/* simple ones first converting degrees - radians */

char *degtorad(rad,deg) /*obvious arguments in 5 char fp */
char *rad, *deg;
{
	char *fpmult(),*radindeg;
	radindeg = "\71\36\175\107\373";
	fpmult(rad,deg,radindeg);
	return (rad);
}

char *radtodeg(deg,rad) /* 5 char fp arguments */
char *deg, *rad;
{
	char *fpmult(), *deginrad;
	deginrad = "\12\162\227\162\6";
	fpmult(deg,rad,deginrad);
	return (deg);
}

/* service function sinev which evaluates when range of angle
reduced to plus or minus pi/2 (90 deg) */

char *sinev(result,angle)

char *result, angle[5];
{
	char *fpmult(),x[5],xsq[5];
	char *coef[5],termreslt[5];
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
	coef[0] = "\0\0\0\100\1";
	coef[1] = "\157\252\252\252\376";
	coef[2] = "\271\242\103\104\372";
	coef[3] = "\320\352\46\230\364";
	coef[4] = "\246\15\116\127\356";
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
	char *fpmult(),*twopi,*halfpi;
	char *mtwopi,*mhalfpi,*fpasg(),*fpchs();
	char *pi,*sinev(),*fpadd();
	char y[5],*fpsub();
	int fpcomp(), compar;
	int signsine;
 /* some initialization required here */
	signsine = 1;
	twopi	= "\171\356\207\144\3";
	halfpi	= "\171\356\207\144\1";
	pi	= "\171