

  /*  CLOGS.C       *****
A group of programs in C, using the BDS-C floating point package
as modified by LCC (FLOAT+44.*) and depending on the ability to
insert null characters in a string available in BDS-C V 1.44.
Four functions are handled:
	log10, exp10, expe, pi
In addition, there is a service function exprange() which returns
a false (00) if the exponent of a floating point variable is reaching
near the end of the usable range.
These are discussed in detail in CLOGS.DOC

L. C. Calhoun
257 South Broadway
Lebanon, Ohio 45036  513 day 433 7510 nite 932 4541

  */


char *pi(result)

 /* result is a standard character array char result[5]; in calling
program as used for floating point variables.  The return is a pointer
to result, and the value of pi stored in the result array in floating
point */

char *result;
{
 char *piconst, *fpasg();

 piconst = "\171\356\207\144\2";
 return (fpasg(result,piconst) );
}

char *expe(result,xin)

 /* computes the base of the natural log "e" raised to the "x'th"
    power.  Checks are made for out of range values and result is
    defaulted to 0, 1, or a large number as appropriate.  There are
    no error flags.  The arguments are floating point character
    arrays char result[5], x[5]; in calling program.  Return is
    a pointer to result, and "e to the x" stored in result.
 */

char *result, *xin;
{
	char *zero, *one, *large, *coef[7], *eghty6, *meghty6;
	char intres[5], xint[5], x[5];
	char  *fpmult(), *fpasg(), *fpdiv(), *fpchs();
	int signx, index, bigexp, smallexp, zeroin;
	int exprange();

	bigexp = smallexp = zeroin = 0;

	zero ="\0\0\0\0\0";
	one = "\0\0\0\100\1";
	large = "\0\0\0\100\175";
	eghty6 = "\0\0\0\126\7";
	meghty6 = "\0\0\0\252\7";

	coef[0] = "\0\0\0\100\1";
	coef[1] = "\140\326\377\177\376";
	coef[2] = "\130\373\3\100\374";
	coef[3] = "\200\1\352\124\370";
	coef[4] = "\351\253\362\131\364";
	coef[5] = "\21\213\32\133\357";
	coef[6] = "\371\330\260\134\354";

  /* preserve input datum */
	fpasg(x,xin);

 /* check for sign */
	if (x[3] < 128)   /* positive */
	   {signx = 1;
	     }
	else
	     {signx = 0;
	      fpchs(x,x);
	     }

   /* check for zero and out of range of fp var */

   /* check for zero and very small numbers */
	if ( ((x[4]>127) && (x[4]<226)) || ( (x[4]==0) &&
		(x[3]==0) && (x[2]==0) && (x[1]==0) && (x[0]==0) ) )
	    zeroin = 1;

   /* check for very small exponent */
	if ( fpcomp(xin,meghty6) < 0)
	    smallexp = 1;

  /*  check for very large exponent */
	if ( fpcomp(x,eghty6) > 0 )
	    bigexp = 1;

  /*  now if small number or zero, result is one */
  /*  now if big number and positive, result is large number */
  /*  now if big number and negative, result is zero */

	if (zeroin) return (fpasg(result,one) );
	if (smallexp) return (fpasg(result,zero) );
	if (bigexp) return (fpasg(result,large) );

 /*  all exceptions taken care of, so evaluate rest  */
	    fpasg(result,one);
	    fpasg(xint,x);
	    index = 1;
	    while ( (index<=6) && exprange(xint) )
	       {
	       fpmult(intres,coef[index],xint);
		fpadd(result,result,intres);
		fpmult(xint,xint,x);
		index++;
	       }
  /* now do the square square */
	fpmult(result,result,result);
	fpmult(result,result,result);

  /* now treat sign appropriately */
	if (signx) return (result);
	else
	   {fpdiv(result,one,result);
	    return (result);
	   }
}

char *exp10(result,xin)

/* similar to expe, except result returned is 10 raised to the x
power    the antilogarithm to base 10  */

char *result, *xin;
{
	char *zero, *ten, *one, *large, *thty8;
	char xint[5], *coef[7], intres[5], tenfac[5], x[5];
	int index, bigexp, smallexp, signx, tenpower;
	int exprange();

	bigexp = smallexp = 0;

	zero ="\0\0\0\0\0";
	one = "\0\0\0\100\1";
	large = "\0\0\0\100\175";
	ten = "\0\0\0\120\4";
	thty8 = "\0\0\0\114\6";

	coef[1] = "\65\264\256\111\1";
	coef[2] = "\0\14\330\124\0";
	coef[3] = "\0\46\354\100\377";
	coef[4] = "\24\140\107\115\375";
	coef[5] = "\242\304\361\155\372";
	coef[6] = "\361\143\246\134\371";

 /* preserve input datum */
	fpasg(x,xin);

 /* check for sign */
	if (x[3] < 128)   /* positive */
	   signx = 1;
	else     /* negative */
	   {signx = 0;
	    fpchs(x,x);
	   }

  /* check for very small or large numbers, check by exponent size */
  /* check for zero or small */
	if ( ((x[4]>127) && (x[4]<226)) || ( (x[4]==0) &&
		(x[3]==0) && (x[2]==0) && (x[1]==0) && (x[0]==0) ) )
	    smallexp = 1;
   /* check for big number */
	if ( fpcomp(x,thty8) > 0)
	    bigexp = 1;

 /* if value is small or zero, return 1 as with expe */
 /* if value is large and positive, return a large number */
 /* if value is large and negative, return zero */

	if (smallexp) return (fpasg(result,one) );

	if(bigexp && signx) return (fpasg(result,large) );

	if(bigexp && !signx) return (fpasg(result,zero) );

 /* now reduce range of x to between zero and one */

	tenpower = ftoit(x);
	itof(tenfac,tenpower);
	fpsub(x,x,tenfac);
	fpasg(tenfac,one);
	while (tenpower)
	    {fpmult(tenfac,tenfac,ten);
	     tenpower--;
	    }
 /* now evaluate series  */
	fpasg(result,one);
	fpasg(xint,x);
	index = 1;

		while ( (index <= 6) && exprange(xint) )
	       {fpmult(intres,coef[index],xint);
		fpadd(result,result,intres);
		fpmult(xint,xint,x);
		index += 1;
	       }

 /* now square result (note error in referenced article) */
	fpmult(result,result,result);

 /* now check sign and make proper return */
	fpmult(result,result,tenfac);  /* scale back up */

	if (signx) return (result);

	else return ( fpdiv(result,one,result) );
}


char *log10(result,sign,xin)

 /* computes briggsian logarithm of x which is a char[5]
	floating point number.  Return is logarithm in result[5],
	and sign pointed to by sign.  The logarithm is taken
	of the magnitude, and sign information preserved
	as required by sign.
 */
char *result, *xin;
int *sign;
{
	char *zero, *ten, *one, *large;
	char *sqrtten, x[5];
	char gamma[5], gamnum[5], gamden[5], *coef[5];
	char *half;
	char intres[5], gamint[5];
	int tenpower;
	int index, bigexp, smallexp, signx;
	int exprange();

	bigexp = smallexp = 0;

	zero ="\0\0\0\0\0";
	one = "\0\0\0\100\1";
	large = "\0\0\0\114\6";
	ten = "\0\0\0\120\4";
	half = "\0\0\0\100\0";
	sqrtten = "\304\145\61\145\2";

	coef[0] = "\362\6\56\157\0";
	coef[1] = "\30\346\21\112\377";
	coef[2] = "\100\55\344\132\376";
	coef[3] = "\106\73\244\140\375";
	coef[4] = "\174\5\367\141\376";

 /*  preserve input variable */
	fpasg(x,xin);

 /* check for sign */
	if (x[3] < 128)   /* positive */
	   signx = 1;
	else     /* negative */
	   {signx = -1;
	    fpchs(x,x);
	   }

  /* check for very small or large numbers, check by exponent size */
  /* check for zero or small */
	if ( ((x[4]>127) && (x[4]<209)) || ( (x[4]==0) &&
		(x[3]==0) && (x[2]==0) && (x[1]==0) && (x[0]==0) ) )
	    smallexp = 1;
   /* check for big number */
	if ( (x[4]  >47) && (x[4] < 128) )
	    bigexp = 1;

 /* if very small, return - a large number
    if very large, return + a large number  */

	if (smallexp)
	    {*sign = signx;
	     return (fpchs(result,large) );
	    }
	if (bigexp)
	    {*sign = signx;
	     return (fpasg(result,large) );
	    }
  /*  now bring into range 1 <= x < 10 */
	tenpower = 0;
	while ( fpcomp(x,ten) >= 0)
	       {tenpower++;
		fpdiv(x,x,ten);
	       }
	while ( fpcomp(x,one) < 0)
	       {tenpower--;
		fpmult(x,x,ten);
	       }

  /* now if exactly one, no need to evaluate */
	fpsub(gamnum,x,one);
	if (((gamnum[4]>127)&&(gamnum[4]<209)) || ((gamnum[0]==0) &&
	   (gamnum[1]==0) && (gamnum[2] == 0) && (gamnum[3] == 0) ) )
	       {*sign = signx;
		itof(result,tenpower);
		return (result);
	       }
 /* now compute gamma  for series  */

	fpsub(gamnum,x,sqrtten);
  /* now check for size of numerator */
	if (((gamnum[4]>127)&&(gamnum[4]<209)) || ((gamnum[0]==0) &&
	   (gamnum[1]==0) && (gamnum[2] == 0) && (gamnum[3] == 0) ) )
	       {itof(result,tenpower);
		fpadd(result,result,half);
		*sign = signx;
		return (result);
	       }
	fpadd(gamden,x,sqrtten);
	fpdiv(gamma,gamnum,gamden);

 /* now set up for series (use gamnum as gamma squared) */
	fpmult(gamnum,gamma,gamma);
	fpasg(gamint,gamma);
	index = 0;
	fpasg(result,half);

 /* now do series evaluation */
	while ( (index <= 4) && exprange(gamint) )
	       {fpmult(intres,coef[index],gamint);
		fpadd(result,result,intres);
		fpmult(gamint,gamint,gamnum);
		index++;
	       }

 /* now do correction for range reduction */
	if (tenpower != 0)
	      {itof(intres,tenpower);
		fpadd(result,result,intres);
	      }

 /* now clean up and return */

	*sign = signx;
	return (result);
}

int exprange(x)

 /* The input argument is a floating point function from BDS C which
consists of an array of 5 character data.  The function returns
a 1 if the exponent is in the range of - 47 to + 47.  Outside this
range a value of 0 (false) is returned.  This is a range of ten to
plus or minus 14 power */
char *x;

{ if ( ((x[4]<128) && (x[4]>47) ) || ((x[4]>127) && (x[4] < 209) ) )
	return (0);
  else return (1);
}
r+sÃÙ%! 	n}·Ê7&!’ 	^#Vr+s!  ÍÒ7&!ž 	^#Vr+së6 Ã&ÃM&!ž 	^#Vr+sÕ`inëásÃc&!ž 	^#Vr+sÕ`inëásÃ¸!ž 	~#fo6 ë!š 9ùëÁÉÃ&Ã/Å!  9ùDMÍn}·Ê©&! 	^#Vr+sën& åÍ|&ÑÃ‡&ÁÉÃ±&ÃÄ*Å!  9ùDM! 	~#foå! 	~#foå!  åÍ®&ÑÑÑ! 	~#foÁÉÁÉÅ!  9ùDM! 	n& 0 ÍùÍ¯Ú'! 	

 /* TESTCLOG.C ****** testing program for
    CLOGS.C testing pi, expe, exp10, and log10   functions
    calls for entry of a number, multiplies it by pi,
    obtains log10 of number and of pi times
    obtains exp10 of two logs
    obtains exp10 and expe of number and pi product
  */

 main()
{
	char rlog[5], rlogpi[5], xin[5], x[5];
	char rpi[5], rexp10[5], rexp10pi[5];
	char rexpe[5], rexpepi[5], rexlog[5];
	char rexlogpi[5];
	char *log10(), *expe(), *pi(), *exp10();
	char stringin[80], pival[5];
	 int *sign1, *sign2;

while(1)
	{printf("\n Enter input value ");
	 gets(stringin);
	 atof(xin,stringin);
	 fpasg(x,xin);
	 pi(pival);
	 fpmult(rpi,pival,xin);
	 printf("\n Input %e pi %e times pi %e",x,pival,rpi);
	 expe(rexpe,x);
	 expe(rexpepi,rpi);
	 printf("\n expe %e expe of pi times %e",rexpe,rexpepi);
	 fpasg(x,xin);
	 exp10(rexp10,x);
	 exp10(rexp10pi,rpi);
	 printf("\n exp10 %e exp10 of pi times %e",rexp10,rexp10pi);
	 fpasg(x,xin);
	 log10(rlog,sign1,x);
	 log10(rlogpi,sign2,rpi);
	 printf("\n log10 %f sign %d log10 of pi %f sign %d",
		rlog,*sign1,rlogpi,*sign2);
	 fpasg(x,xin);
	 exp10(rexlog,rlog);
	 exp10(rexlogpi,rlogpi);
	 if(*sign1 < 0) fpchs(rexlog,rexlog);
	 if(*sign2 < 0) fpchs(rexlogpi,rexlogpi);
	 printf("\n In %e PIVAL %e unlog %e unlogpi %e \n",
	    x, rpi, rexlog, rexlogpi);
	}
}
;
{
	char *zero, *one, *large, *coef[7], *eghty6, *meghty6;
	char intres[5], xint[5], x[5];
	char  *fpmult(), *fpasg(), *fpd





                      NOTES ON "LOG" FUNCTIONS FOR BDS - C


                                  Introduction


      These "log" functions were developed so that I could do some auxiliary

work on scaling and curve generation for a graphics package I'm now doing in

BDS-C.  They seem to work OK in my setup which is now:  

       o  Altair 8800b, 64K CPM 2.2 BDS-C vers 1.44 

       o  Tarbell SSSD 4 8 inch disk 

       o  Scion Microangelo 

       o  LSI ADM3A 

This package was developed by:  

L. C. Calhoun PE 

257 South Broadway 

Lebanon, Ohio 45036 

<513> 932-4541/433-7510 

                       SPECIAL NOTE ON VERSION OF BDS - C


      The CLOGS programs have been written to take advantage of the ability to

insert '\0' into string constants which make it possible to use string constant

as pseudo-static floating point constants.  



      These programs are written in BDS-C using the floating point package

modified to add truncation and magnitude functions.  This package is called

"FLOAT+44".  The following functions are mechanized:  

     char *expe(result,x) 

     char *result, *x;   /* usual [5] char arrays for fp */ 

          The program returns the base of natural logs "e" raised to the power

          given in x.  The program is limited to work within the bounds of the





                                        1









          floating point variable.  The function returns the pointer to the

          result.  Values outside floating point bounds for the result are set

          to either zero or a very large number on the order of 2e38.  No error

          flags exist or are set.  

     char *exp10(result,x) 

     char *result, *x;   /* as with expe */ 

          Identical to expe, except the base of briggs logs "10" is raised to

          the power indicated by x.  

     char *log10(result,sign,x) 

     char *result, *x;    /*as with expe */ 

     int *sign; 

     char *angle, *datum;    

          This returns the logarithm to the base 10 in result of the value in x.

          x is unchanged.  Logarithms are computed of the magnitude of x, and

          negative x values return a -1 in sign.  Positive x values produce a 1

          in sign.  Very large or small values are returned for out   of range

          data.  There is no over/underflow indication.  Also returns pointer to

          result.  

                                     Method


      The methods used are outlined in "Functional Approximations" by Fred

Ruckdeschel; page 34 ff in BYTE for November 1978.  Note the corrections in the

January 1979 issue.  A number of references are given in that article, and are

recommended reading.  There is an error in Ruckdeschel' article, in Table 3b.

The term to the right of the equals should all be enclosed in brackets and

squared.  Refer to equation 4.2.47 in Ruckdeshel' reference 6.  The following

service function is used:  

     int exprange(x) 





                                        2









     char *x; 

          This is used in the series evaluations.  Input is a pointer to the

          BDS-C type floating point variable.  It returns a 1 (true) if -47 <=

          exponent <= 47.  Outside of the range of exponent a 0 (false) is

          returned.  The exponent is a power of 2, so the effective range is

          about 1.4e14 to 7.e-15 for exponent in decimals.  The function is used

          to avoid a series computation overflowing the exponent which wraps a

          small exponent into a large one and vice-versa.  



      A number of checks are made for very large and very small data, to protect

the evaluation from the underflow and overflow failures of the floating point

package.  I have used TESTCLOG to evaluate over a wide range of variables, and I

think! I got all the gotchas.  It looks as though the package has (except at

infinity) about an absolute accuracy of .00001.  You will note that I use the

properties of the floating point numbers to do magnitude and sign checks.  Lots

faster than using fpcomp().  There is another program included, COEFSTAT, which

I used to derive the five octal equivalents for the pseudo-static terms in the

series evaluations, etc.  This will only work with BDS C V 1.44 (and later, I

hope) which allows insertion of nulls ('\0') in string constants.  The string

constants are used as pseudo-static floating point constants..and work very

well.  



                        Components of the CLOGS Package
       1. CLOGS.DOC             This documentation file 

       2. CLOGS.C               Source for trig package 

       3. COEFSTAT.C            Source for coeficient determing program 

       4. TESTCLOG.C            Source for trig function testing program 







                                        3



power);
	fpsub(x,x,tenfac);
	fpasg(tenfac,one);
	while (tenpower)
	    Software contributions are received for inclusion into the
  library with the understanding that the contributor is
  authorized to make the material available to others for their
  individual, non-commercial use.  The Users Group makes no
  representations as to the utility of the material in the
  library for any purpose.  Contributions should be submitted
  on 8" single density diskettes in CP/M file form.  Please
  cross reference any rewrites or bug-fixes to prior
  distributions

National CP/M Users Group - Program Submission Form

Submission Date: 27 July 1981 

Files names:	FLOAT+44.*  Files pertaining to a modified
				floating point package for
				BDS-C.  These files upgrade
				my previous FLOATXT.*
				submission.  Files are
		FLOAT+44.C  Source, includes fixed "z" in _spr
		FLOAT+44.CRL compiled version with V 1.44
		FLOAT+44.DOC Documentation from FLOAT.DOC, updated
		NEWFLVAL.C   Source of test program for float pkg
		NEWFLVAL.CRL compiled version with V 1.44 & FLOAT+44
		NEWFLVAL.COM linked version with FLOAT+44 for test

		COEFSTAT.C   Source of program to derive octal string
				equivalent of fp constants
		COEFSTAT.CRL compiled version with V 1.44

		CTRIG.C      Source of updated trignometric functions
				