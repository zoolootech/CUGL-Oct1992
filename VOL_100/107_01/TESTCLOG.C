

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
				working with BDS-C V 1.44 and FLOAT+44
				Update of CTRIG previously submitted
				to BDS-C UG.
		CTRIG.CRL    compiled vers