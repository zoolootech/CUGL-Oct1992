

/*
	Floating point package support routines
	  modified two times by L. C. Calhoun see notes below

	Note the "fp" library function, available in DEFF2.CRL,
	is used extensively by all the floating point number
	crunching functions.

	(see FLOAT.DOC for details...)
	(see FLOAT+44.DOC for details of revised version)

	NEW FEATURE: a special "printf" function has been included
		     in this source file for use with floating point
		     operands, in addition to the normal types. The
		     printf presented here will take precedence over
		     the DEFF.CRL version when "float" is specified
		     on the CLINK command line at linkage time.
		     Note that the "fp" function, needed by most of
		     the functions in this file, resides in DEFF2.CRL
		     and will be automatically collected by CLINK.

	All functions here written by Bob Mathias, except printf and
	_spr (written by Leor Zolman.)

	New Functions Added	fpmag converts to floating magnitude
			      fpchs changes sign of floating point no
			      fpasg provides assignment of fl pt no
			      ftoit converts fl pt no to trunc. int.
			      ftoir converts fl pt no to rounded int.

                written by L. C. Calhoun
	Second Revision by L. C. Calhoun  
				Modify the _spr to use the z option
				  as in STDLIB2
				Modify the program set to utilize the
				  V 1.44 zero insert string variable

*/

#include "bdscio.h"

#define NORM_CODE	0
#define ADD_CODE	1
#define SUB_CODE	2
#define MULT_CODE	3
#define DIV_CODE	4
#define FTOA_CODE	5
#define EXPON_SIGN    0x80   /* break point for exponent sign */

fpcomp(op1,op2)
	char *op1,*op2;
{
	char work[5];
	fpsub(work,op1,op2);
	if (work[3] > 127) return (-1);
	if (work[0]+work[1]+work[2]+work[3]) return (1);
	return (0);
}

fpnorm(op1) char *op1;
{	fp(NORM_CODE,op1,op1);return(op1);}

fpadd(result,op1,op2)
	char *result,*op1,*op2;
{	fp(ADD_CODE,result,op1,op2);return(result);}

fpsub(result,op2,op1)
	char *result,*op1,*op2;
	{fp(SUB_CODE,result,op1,op2);return(result);}

fpmult(result,op1,op2)
	char *result,*op1,*op2;
{	fp(MULT_CODE,result,op1,op2);
	return (result);
}

fpdiv(result,op1,op2)
	char *result,*op1,*op2;
{	fp(DIV_CODE,result,op1,op2);return(result);}

atof(fpno,s)
	char fpno[5],*s;
{
	char *fpnorm(),work[5],*ZERO,*FP_10;
	int sign_boolean,power;

	FP_10 = "\0\0\0\120\4"; /* use as static variable */
	ZERO = "\0\0\0\0\0";
	setmem(fpno,5,0);
	sign_boolean=power=0;

	while (*s==' ' || *s=='\t') ++s;
	if (*s=='-'){sign_boolean=1;++s;}
	for (;isdigit(*s);++s){
		fpmult(fpno,fpno,FP_10);
		work[0]=*s-'0';
		work[1]=work[2]=work[3]=0;work[4]=31;
		fpadd(fpno,fpno,fpnorm(work));
	}
	if (*s=='.'){
		++s;
		for (;isdigit(*s);--power,++s){
			fpmult(fpno,fpno,FP_10);
			work[0]=*s-'0';
			work[1]=work[2]=work[3]=0;work[4]=31;
			fpadd(fpno,fpno,fpnorm(work));
		}
	}
	if (toupper(*s) == 'E') {++s; power += atoi(s); }
	if (power>0)
		for (;power!=0;--power) fpmult(fpno,fpno,FP_10);
	else
	if (power<0)
		for (;power!=0;++power) fpdiv(fpno,fpno,FP_10);
	if (sign_boolean){
		fpsub(fpno,ZERO,fpno);
	}
	return(fpno);
}
ftoa(result,op1)
	char *result,*op1;
{	fp(FTOA_CODE,result,op1);return(result);}

itof(op1,n)
char *op1;
int n;
{
	char temp[20];
	return atof(op1, itoa(temp,n));
}

itoa(str,n)
char *str;
{
	char *sptr;
	sptr = str;
	if (n<0) { *sptr++ = '-'; n = -n; }
	_uspr(&sptr, n, 10);
	*sptr = '\0';
	return str;
}


/*
	The short "printf" function given here is exactly the
	same as the one in the library, but it needs to be placed
	here so that the special "_spr" is used instead of the
	normal one in DEFF.CRL. The way the linker works is that
	a function is not linked in UNTIL IT IS REFERENCED...so
	if the definition of "printf" were not placed here in this
	file, "_spr" would not be referenced at all
	until the "printf" from DEFF.CRL got yanked in, at which time
	"_spr" would ALSO be taken from DEFF.CRL and cause the
	floating point "_spr" options to not be recognized.

	In other words, if "printf" were not given explicitly here,
	the WRONG _spr would end up being used.
*/


printf(