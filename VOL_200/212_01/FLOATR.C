/* FLOATR.C    VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:36:35 PM */
/*
%CC1 $1.C 
*/
/* 
Description:

Library of floating point functions (FLOAT.C, by Bob Mathias)
	with additions (FLOAT+44.C, by L.C. Calhoun)
	with added trig functions (CTRIG.C, by L.C. Calhoun)
	with added log functions (CLOGS.C, by L.C. Calhoun)
	with additional function(s) (by J.A. Rupley)

FLOAT.C from BDS C distribution package, version 1.5
FLOAT+44.C, CTRIG.C, AND CLOGS.C are from cug disk "functions III".

Note: the v1.5 FLOAT.C has an updated version of _spr;
replaced v1.44 std functions by v1.5 std functions

By J.A. Rupley, Tucson, Arizona
Coded for BDS C compiler, version 1.50a
*/


	/* page eject */

/* summary of functions 

(from FLOAT, by Mathias)
int fpcomp(op1,op2)			return = (1,0,-1) || (op1 >,=,< op2)
char *fpadd(result,op1,op2)		result = op1 + op2
char *fpsub(result,op1,op2)		result = op1 - op2
char *fpmult(result,op1,op2)		result = op1*op2
char *fpdiv(result,op1,op2)		result = op1/op2
char *itof(result,int#)			result = float(int#)
char *atof(result,string#)		result = float(string#)
char *ftoa(result,fpno)			result = string(fpno)
char *itoa(result,int#)			result = string(int#)
void printf(format)			standard C print function
void _spr(line,&format)			float formatting function

(from FLOAT+44, by Calhoun)
char *fpmag(result,fpno)		result = abs(fpno)
char *fpchs(result,fpno)		result = - fpno
char *fpasg(result,fpno)		result = fpno
int ftoit(fpno)				return = truncate then to integer
int ftoir(fpno)				return = round then to integer

(from CLOGS, by Calhoun)
int exprange(fpno)			service function
char *pi(result)			result = float(pi)
char *expe(result,fpno)			result = e^(fpno)
char *exp10(result,fpno)		result = 10^(fpno)
char *log10(result,&int,fpno)		result = log10(abs(fpno)) //
							*int = sgn(fpno)

(from CTRIG, by Calhoun)
char *degtorad(result,fpno)		result = rad(fpno-degrees)
char *radtodeg(result,fpno)		result = deg(fpno-radians)
char *sinev(result,fpno)		service function
char *sine(result,fpno)			result = sin(fpno-radians)
char *cosine(result,fpno)		result = cos(fpno-radians)
char *tangent(result,fpno)		result = tangent(fpno-radians)
char *atanev(result,fpno)		service function
char *arctan(result,fpno)		result = arctan-radians(fpno)
char *arctan2(result,quadrant,opside,adjside)
		result = arctan-radians(opside/adjside) // quadrant = 1 to 4
	
(additions, by Rupley)
char *sqrt(result,&int,fpno)		result = sqrt(abs(fpno)) //
							*int = sgn(fpno)
*/


	/* page eject */

/* FLOAT.C ***************/

/*
	Floating point package support routines

	Note the "fp" library function, available in DEFF2.CRL,
	is used extensively by all the floating point number
	crunching functions.

	-------------------------------------------------------------
	Usage: After compiling your program, link with this library
	by typing:

	A>clink <your program files> -f float <cr>
	-------------------------------------------------------------

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
*/

#include <bdscio.h>

#define NORM_CODE	0
#define ADD_CODE	1
#define SUB_CODE	2
#define MULT_CODE	3
#define DIV_CODE	4
#define FTOA_CODE	5

fpcomp(op1, op2)
char *op1, *op2;
{
	char work[5];
	fpsub(work, op1, op2);
	if (work[3] > 127)
		return (-1);
	if (work[0] + work[1] + work[2] + work[3])
		return (1);
	return (0);
}

fpnorm(op1) char *op1;
{
	fp(NORM_CODE, op1, op1);
	return (op1);
}

fpadd(result, op1, op2)
char *result, *op1, *op2;
{
	fp(ADD_CODE, result, op1, op2);
	return (result);
}

fpsub(result, op2, op1)
char *result, *op1, *op2;
{
	fp(SUB_CODE, result, op1, op2);
	return (result);
}

fpmult(result, op1, op2)
char *result, *op1, *op2;
{
	fp(MULT_CODE, result, op1, op2);
	return (result);
}

fpdiv(result, op1, op2)
char *result, *op1, *op2;
{
	fp(DIV_CODE, result, op1, op2);
	return (result);
}

atof(fpno, s)
char fpno[5], *s;
{
	char *fpnorm(), work[5], ZERO[5], FP_10[5];
	int sign_boolean, power;

	initb(FP_10, "0,0,0,80,4");
	setmem(fpno, 5, 0);
	sign_boolean = power = 0;

	while (*s == ' ' || *s == '\t')
		++s;
	if (*s == '-')
	{
		sign_boolean = 1;
		++s;
	}
	for (; isdigit(*s); ++s)
	{
		fpmult(fpno, fpno, FP_10);
		work[0] = *s - '0';
		work[1] = work[2] = work[3] = 0;
		work[4] = 31;
		fpadd(fpno, fpno, fpnorm(work));
	}
	if (*s == '.')
	{
		++s;
		for (; isdigit(*s); --power, ++s)
		{
			fpmult(fpno, fpno, FP_10);
			work[0] = *s - '0';
			work[1] = work[2] = work[3] = 0;
			work[4] = 31;
			fpadd(fpno, fpno, fpnorm(work));
		}
	}
	if (toupper(*s) == 'E')
	{
		++s;
		power += atoi(s);
	}
	if (power > 0)
		for (; power != 0; --power)
			fpmult(fpno, fpno, FP_10);
	else
	    if (power < 0)
		for (; power != 0; ++power)
			fpdiv(fpno, fpno, FP_10);
	if (sign_boolean)
	{
		setmem(ZERO, 5, 0);
		fpsub(fpno, ZERO, fpno);
	}
	return (fpno);
}
ftoa(result, op1)
char *result, *op1;
{
	fp(FTOA_CODE, result, op1);
	return (result);
}

itof(op1, n)
char *op1;
int n;
{
	int *p;
	p = op1;
	p[0] = 0;
	p[1] = n;
	op1[4] = 15;
	fp(NORM_CODE, op1, op1);
	return op1;
}

itoa(str, n)
char *str;
{
	char *sptr;
	sptr = str;
	if (n < 0)
	{
		*sptr++ = '-';
		n = -n;
	}
	_uspr(&sptr, n, 10);
	*sptr = '\0';
	return str;
}

/*
	This is the special formatting function, which supports the
	"e" and "f" conversions as well as the normal "d", "s", etc.
	When using "e" or "f" format, the corresponding argument in
	the argument list should be a pointer to one of the five-byte
	strings used as floating point numbers by the floating point
	functions. Note that you don't need to ever use the "ftoa"
	function when using this special printf/sprintf combination;
	to achieve the same result as ftoa, a simple "%e" format
	conversion will do the trick. "%f" is used to eliminate the
	scientific notation and set the precision. The only [known]
	difference between the "e" and "f" conversions as used here
	and the ones described in the Kernighan & Ritchie book is that
	ROUNDING does not take place in this version...e.g., printing
	a floating point number which happens to equal exactly 3.999
	using a "%5.2f" format conversion will produce " 3.99" instead
	of " 4.00".
*/

printf(format)
char *format;
{
	int putchar();
	_spr(&format, &putchar);        /* use "_spr" to form the output */
}


_spr(fmt, putcf, arg1)
int (*putcf)();
char **fmt;
{
	char _uspr(), c, base, *sptr, *format;
	char wbuf[MAXLINE], *wptr, pf, ljflag, zfflag;
	int width, precision, exp, *args;

	format = *fmt++;        /* fmt first points to the format string */
	args = fmt;        /* now fmt points to the first arg value */
	while (c = *format++)
		if (c == '%')
		{
			wptr = wbuf;
			precision = 6;
			ljflag = pf = zfflag = 0;

			if (*format == '-')
			{
				format++;
				ljflag++;
			}

			if (*format == '0')
				zfflag++;        /* test for zero fill */

			width = isdigit(*format) ? _gv2(&format) : 0;

			if ((c = *format++) == '.')
			{
				precision = _gv2(&format);
				pf++;
				c = *format++;
			}

			switch (toupper(c))
			{
			case 'E' :
				if (precision > 7)
					precision = 7;
				ftoa(wbuf, *args++);
				strcpy(wbuf + precision + 3, wbuf + 10);
				width -= strlen(wbuf);
				goto pad2;

			case 'F' :
				ftoa(&wbuf[60], *args++);
				sptr = &wbuf[60];
				while (*sptr++ != 'E')
					;
				exp = atoi(sptr);
				sptr = &wbuf[60];
				if (*sptr == ' ')
					sptr++;
				if (*sptr == '-')
				{
					*wptr++ = '-';
					sptr++;
					width--;
				}
				sptr += 2;

				if (exp < 1)
				{
					*wptr++ = '0';
					width--;
				}

				pf = 7;
				while (exp > 0 && pf)
				{
					*wptr++ = *sptr++;
					pf--;
					exp--;
					width--;
				}

				while (exp > 0)
				{
					*wptr++ = '0';
					exp--;
					width--;
				}

				*wptr++ = '.';
				width--;

				while (exp < 0 && precision)
				{
					*wptr++ = '0';
					exp++;
					precision--;
					width--;
				}

				while (precision && pf)
				{
					*wptr++ = *sptr++;
					pf--;
					precision--;
					width--;
				}

				while (precision > 0)
				{
					*wptr++ = '0';
					precision--;
					width--;
				}

				goto pad;


			case 'D' :
				if (*args < 0)
				{
					*wptr++ = '-';
					*args = -*args;
					width--;
				}
			case 'U' :
				base = 10;
				goto val;

			case 'X' :
				base = 16;
				goto val;

			case 'O' :
				base = 8;

val :
				width -= _uspr(&wptr, *args++, base);
				goto pad;

			case 'C' :
				*wptr++ = *args++;
				width--;
				goto pad;

			case 'S' :
				if (!pf)
					precision = 200;
				sptr = *args++;
				while (*sptr && precision)
				{
					*wptr++ = *sptr++;
					precision--;
					width--;
				}

pad :
				*wptr = '\0';
pad2 :
				wptr = wbuf;
				if (!ljflag)
					while (width-- > 0)
						if ((*putcf)(zfflag ? '0' : ' ', arg1) == ERROR)
							return ERROR;

				while (*wptr)
					if ((*putcf)(*wptr++, arg1) == ERROR)
						return ERROR;

				if (ljflag)
					while (width-- > 0)
						if ((*putcf)(' ', arg1) == ERROR)
							return ERROR;
				break;

			default :
				if ((*putcf)(c, arg1) == ERROR)
					return ERROR;
			}
		}
	else
	    if ((*putcf)(c, arg1) == ERROR)
		return ERROR;
}


	/* page eject */

/* FLOAT+44.C ************/
/*

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

#define EXPON_SIGN    0x80   /* break point for exponent sign */

/*  NEW FUNCTIONS ADDED */
/*  new function to convert floating to truncated integer */
ftoit(fpno)
char *fpno;
{
	char *ftoa(), temp[20], temp2[20], *sptr, *wptr;
	int atoi(), exp, pf;
	wptr = &temp2;
	ftoa(temp, fpno);
	sptr = &temp;
	while (*sptr++ != 'E')
		;
	exp = atoi(sptr);
	sptr = &temp;
	if (*sptr == ' ')
		sptr++;
	if (*sptr == '-')
	{
		*wptr++ = '-';
		sptr++;
	}
	sptr += 2;

	if (exp < 1)
		*wptr++ = '0';

	pf = 7;
	while (exp > 0 && pf)
	{
		*wptr++ = *sptr++;
		pf--;
		exp--;
	}
	while (exp > 0)
	{
		*wptr++ = '0';
		exp--;
	}
	*wptr++ = '.';
	/* foregoing lifted from _spr with F format  */
	return (atoi(temp2));
}

/* new function to round, then convert to integer */
ftoir(fpno)
char *fpno;
{
	char *fpsub(), *fpadd();
	char *atof(), rnd[5], res[5];
	int ftoit();
	atof(rnd, "0.5");
	if (fpno[3] > 127)
		fpsub(res, fpno, rnd);
	else
	    fpadd(res, fpno, rnd);
	return (ftoit(res));
}

/* new function to produce the unsigned magnitude of a fp no. */
char *fpmag(result, fpno)
char *result, *fpno;
{
	char *fpchs();
	if (fpno[3] < 128)
		return (fpasg(result, fpno));
	else
	    return (fpchs(result, fpno));
}

/* new function to change sign of floating point number */
char *fpchs(result, fpno)
char *result, *fpno;
{
	char *fpsub(), *ZERO;
	ZERO = "\0\0\0\0\0";
	return (fpsub(result, ZERO, fpno));
}

/* a new function to assign <in essence> the value of one
   floating point number to another  */
char *fpasg(result, fpno)
char *result, *fpno;
{
	int index;
	for (index = 0; index <= 4; index++)
		result[index] = fpno[index];
	return (result);
}


	/* page eject */

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
	return (fpasg(result, piconst));
}

char *expe(result, xin)

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
	char *fpmult(), *fpasg(), *fpdiv(), *fpchs();
	int signx, index, bigexp, smallexp, zeroin;
	int exprange();

	bigexp = smallexp = zeroin = 0;

	zero = "\0\0\0\0\0";
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
	fpasg(x, xin);

	/* check for sign */
	if (x[3] < 128)        /* positive */
	{
		signx = 1;
	}
	else
	    {
		signx = 0;
		fpchs(x, x);
	}

	/* check for zero and out of range of fp var */

	/* check for zero and very small numbers */
	if (((x[4] > 127) && (x[4] < 226)) || ((x[4] == 0) &&
	    (x[3] == 0) && (x[2] == 0) && (x[1] == 0) && (x[0] == 0)))
		zeroin = 1;

	/* check for very small exponent */
	if (fpcomp(xin, meghty6) < 0)
		smallexp = 1;

	/*  check for very large exponent */
	if (fpcomp(x, eghty6) > 0)
		bigexp = 1;

	/*  now if small number or zero, result is one */
	/*  now if big number and positive, result is large number */
	/*  now if big number and negative, result is zero */

	if (zeroin)
		return (fpasg(result, one));
	if (smallexp)
		return (fpasg(result, zero));
	if (bigexp)
		return (fpasg(result, large));

	/*  all exceptions taken care of, so evaluate rest  */
	fpasg(result, one);
	fpasg(xint, x);
	index = 1;
	while ((index <= 6) && exprange(xint))
	{
		fpmult(intres, coef[index], xint);
		fpadd(result, result, intres);
		fpmult(xint, xint, x);
		index++;
	}
	/* now do the square square */
	fpmult(result, result, result);
	fpmult(result, result, result);

	/* now treat sign appropriately */
	if (signx)
		return (result);
	else
	    {
		fpdiv(result, one, result);
		return (result);
	}
}

char *exp10(result, xin)

/* similar to expe, except result returned is 10 raised to the x
power    the antilogarithm to base 10  */

char *result, *xin;
{
	char *zero, *ten, *one, *large, *thty8;
	char xint[5], *coef[7], intres[5], tenfac[5], x[5];
	int index, bigexp, smallexp, signx, tenpower;
	int exprange();

	bigexp = smallexp = 0;

	zero = "\0\0\0\0\0";
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
	fpasg(x, xin);

	/* check for sign */
	if (x[3] < 128)        /* positive */
		signx = 1;
	else        /* negative */
	{
		signx = 0;
		fpchs(x, x);
	}

	/* check for very small or large numbers, check by exponent size */
	/* check for zero or small */
	if (((x[4] > 127) && (x[4] < 226)) || ((x[4] == 0) &&
	    (x[3] == 0) && (x[2] == 0) && (x[1] == 0) && (x[0] == 0)))
		smallexp = 1;
	/* check for big number */
	if (fpcomp(x, thty8) > 0)
		bigexp = 1;

	/* if value is small or zero, return 1 as with expe */
	/* if value is large and positive, return a large number */
	/* if value is large and negative, return zero */

	if (smallexp)
		return (fpasg(result, one));

	if (bigexp && signx)
		return(fpasg(result, large));

	if (bigexp && !signx)
		return(fpasg(result, zero));

	/* now reduce range of x to between zero and one */

	tenpower = ftoit(x);
	itof(tenfac, tenpower);
	fpsub(x, x, tenfac);
	fpasg(tenfac, one);
	while (tenpower)
	{
		fpmult(tenfac, tenfac, ten);
		tenpower--;
	}
	/* now evaluate series  */
	fpasg(result, one);
	fpasg(xint, x);
	index = 1;

	while ((index <= 6) && exprange(xint))
	{
		fpmult(intres, coef[index], xint);
		fpadd(result, result, intres);
		fpmult(xint, xint, x);
		index += 1;
	}

	/* now square result (note error in referenced article) */
	fpmult(result, result, result);

	/* now check sign and make proper return */
	fpmult(result, result, tenfac);        /* scale back up */

	if (signx)
		return (result);

	else
	    return (fpdiv(result, one, result));
}


char *log10(result, sign, xin)

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

	zero = "\0\0\0\0\0";
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
	fpasg(x, xin);

	/* check for sign */
	if (x[3] < 128)        /* positive */
		signx = 1;
	else        /* negative */
	{
		signx = -1;
		fpchs(x, x);
	}

	/* check for very small or large numbers, check by exponent size */
	/* check for zero or small */
	if (((x[4] > 127) && (x[4] < 209)) || ((x[4] == 0) &&
	    (x[3] == 0) && (x[2] == 0) && (x[1] == 0) && (x[0] == 0)))
		smallexp = 1;
	/* check for big number */
	if ((x[4] > 47) && (x[4] < 128))
		bigexp = 1;

	/* if very small, return - a large number
	    if very large, return + a large number  */

	if (smallexp)
	{
		*sign = signx;
		return (fpchs(result, large));
	}
	if (bigexp)
	{
		*sign = signx;
		return (fpasg(result, large));
	}
	/*  now bring into range 1 <= x < 10 */
	tenpower = 0;
	while (fpcomp(x, ten) >= 0)
	{
		tenpower++;
		fpdiv(x, x, ten);
	}
	while (fpcomp(x, one) < 0)
	{
		tenpower--;
		fpmult(x, x, ten);
	}

	/* now if exactly one, no need to evaluate */
	fpsub(gamnum, x, one);
	if (((gamnum[4] > 127) && (gamnum[4] < 209)) || ((gamnum[0] == 0) &&
	    (gamnum[1] == 0) && (gamnum[2] == 0) && (gamnum[3] == 0)))
	{
		*sign = signx;
		itof(result, tenpower);
		return (result);
	}
	/* now compute gamma  for series  */

	fpsub(gamnum, x, sqrtten);
	/* now check for size of numerator */
	if (((gamnum[4] > 127) && (gamnum[4] < 209)) || ((gamnum[0] == 0) &&
	    (gamnum[1] == 0) && (gamnum[2] == 0) && (gamnum[3] == 0)))
	{
		itof(result, tenpower);
		fpadd(result, result, half);
		*sign = signx;
		return (result);
	}
	fpadd(gamden, x, sqrtten);
	fpdiv(gamma, gamnum, gamden);

	/* now set up for series (use gamnum as gamma squared) */
	fpmult(gamnum, gamma, gamma);
	fpasg(gamint, gamma);
	index = 0;
	fpasg(result, half);

	/* now do series evaluation */
	while ((index <= 4) && exprange(gamint))
	{
		fpmult(intres, coef[index], gamint);
		fpadd(result, result, intres);
		fpmult(gamint, gamint, gamnum);
		index++;
	}

	/* now do correction for range reduction */
	if (tenpower != 0)
	{
		itof(intres, tenpower);
		fpadd(result, result, intres);
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

{
	if (((x[4] < 128) && (x[4] > 47)) || ((x[4] > 127) && (x[4] < 209)))
		return (0);
	else
	    return (1);
}


	/* page eject */

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

char *degtorad(rad, deg)        /*obvious arguments in 5 char fp */
char *rad, *deg;
{
	char *fpmult(), *radindeg;
	radindeg = "\71\36\175\107\373";
	fpmult(rad, deg, radindeg);
	return (rad);
}

char *radtodeg(deg, rad)        /* 5 char fp arguments */
char *deg, *rad;
{
	char *fpmult(), *deginrad;
	deginrad = "\12\162\227\162\6";
	fpmult(deg, rad, deginrad);
	return (deg);
}

/* service function sinev which evaluates when range of angle
reduced to plus or minus pi/2 (90 deg) */

char *sinev(result, angle)

char *result, angle[5];
{
	char *fpmult(), x[5], xsq[5];
	char *coef[5], termreslt[5];
	char *fpadd(), *fpasg();
	int index;

	/*  use the exponent part of the floating point
		    to check for threat of underflow  use small
		    angle approximation if appropriate  */
	if ((angle[4] > 128) && (angle[4] < 226))
	{
		fpasg(result, angle);
		return (result);
	}
	/* solution to fpmult underflow problem */

	/* series coef are 1., -.1666666, .008333026, -.0001980742,
		.000002601887  determined from coefset program */
	coef[0] = "\0\0\0\100\1";
	coef[1] = "\157\252\252\252\376";
	coef[2] = "\271\242\103\104\372";
	coef[3] = "\320\352\46\230\364";
	coef[4] = "\246\15\116\127\356";
	fpasg(x, angle);
	fpmult(xsq, x, x);
	setmem(result, 5, 0);
	/* to this point the coef have been initialized, the angle
	    copied to x, x squared computed, and the result initialized */

	/* now to do the polynomial approximation */
	index = 0;
	while ((index <= 4) && ((x[4] > 194) || (x[4] < 64)))
		/* use index for loop, and exponent of x to avoid underflow
			   problems */
	{
		fpmult(termreslt, coef[index], x);
		fpadd(result, result, termreslt);
		index++;
		fpmult(x, x, xsq);
	}
	return (result);
}

/* here is sine(result,angle) with angle in radians */

char *sine(result, angle)

char *result, *angle;
{
	char *fpmult(), *twopi, *halfpi;
	char *mtwopi, *mhalfpi, *fpasg(), *fpchs();
	char *pi, *sinev(), *fpadd();
	char y[5], *fpsub();
	int fpcomp(), compar;
	int signsine;
	/* some initialization required here */
	signsine = 1;
	twopi = "\171\356\207\144\3";
	halfpi = "\171\356\207\144\1";
	pi = "\171\356\207\144\2";
	mtwopi = "\207\21\170\233\3";
	mhalfpi = "\207\21\170\233\1";
	fpasg(y, angle);
	while (fpcomp(y, twopi) >= 0)
	{
		fpsub(y, y, twopi);
	}
	while (fpcomp(y, mtwopi) <= 0)
	{
		fpadd(y, y, twopi);
	}
	if (fpcomp(y, halfpi) > 0)
	{
		signsine *= -1;
		fpsub(y, y, pi);
	}
	if (fpcomp(y, mhalfpi) < 0)
	{
		signsine *= -1;
		fpadd(y, y, pi);
	}
	sinev(result, y);
	if (signsine > 0)
		return(result);
	/* minus so need to change sign */
	else
	    return (fpchs(result, result));
}

/* cosine(result,angle) with angle in radians  - uses sine */

char *cosine(result, angle)

char *result, *angle;
{
	char *sine(), *fpsub(), *halfpi, y[5];
	halfpi = "\171\356\207\144\1";
	fpsub(y, halfpi, angle);
	sine(result, y);
	return (result);
}

/* tangent(result,angle) with angle in radians, returns very 
large number for divide by zero condition */

char *tangent(result, angle)
char *result, angle[5];
{
	char *sine(), *cosine(), *fpdiv(), zero[5];
	char sresult[5], cresult[5], intres[5], big[5];
	char *fpmult(), *fpmag();

	sine(sresult, angle);
	cosine(cresult, angle);
	/* check magnitude of denominator :*/
	/* check magnitude of denominator using exponent */
	if ((cresult[4] > 128) && (cresult[4] < 132))
	{
		initb(big, "30,207,228,127,128");        /*big number */
		if (sresult[3] > 127)        /*use mantissa sign */
			return (fpchs(result, big));
		else
		    return (fpasg(result, big));
	}
	/* check for small angle, use small angle approx to
		   avoid underflow */
	if ((angle[4] < 226) && (angle[4] > 128))
		return (fpasg(result, angle));
	else
	    return (fpdiv(result, sresult, cresult));
}

/* atanev(result,x) evaluates arctangent for 0 <= x < infinity,
	result in radians */

char *atanev(result, x)

char result[5], x[5];
{
	char *coef[8], *piover4, y[5];
	int index;
	char *fpadd(), *fpmult(), *atof(), ysq[5], *one;
	char yterm[5], termreslt[5], *fpasg();

	piover4 = "\171\356\207\144\0";
	one = "\0\0\0\100\1";

	/* small angle approximation */
	if ((x[4] > 128) && (x[4] < 226))
		/* use fp exponent to check size, use small angle */
		return (fpasg(result, x));
	else
	    fpasg(result, piover4);

	/* check for argument near one */
	fpsub(yterm, x, one);
	/* if it is close to one, as checked by exponent,
		   return the result pi/4  */
	if ((yterm[4] > 128) && (yterm[4] < 243))
		return (result);


	coef[0] = "\270\376\377\177\0";
	coef[1] = "\277\357\254\252\377";
	coef[2] = "\106\126\40\146\376";
	coef[3] = "\146\313\311\270\376";
	coef[4] = "\34\360\273\142\375";
	coef[5] = "\206\30\177\215\374";
	coef[6] = "\154\54\213\131\373";
	coef[7] = "\67\12\224\275\371";

	fpadd(termreslt, x, one);
	fpdiv(y, yterm, termreslt);

	fpmult(ysq, y, y);
	/* do poly evaluation */
	index = 0;
	/* poly evaluation checked by index limit, and check of
		variables for under/over flow */
	while ((index <= 7) && ((y[4] < 100) || (y[4] > 140)))
	{
		fpmult(termreslt, coef[index], y);
		fpadd(result, result, termreslt);
		index++;
		fpmult(y, y, ysq);
	}

	return (result);
}

/* arctan(result,angle) is floating point arctangent evaluation */

char *arctan(result, x)
char result[5], x[5];

{
	char *atanev(), *fpasg(), y[5];
	char *fpchs(), *halfpi;
	int index;
	halfpi = "\171\356\207\144\1";
	/* check exponent for very large argument */
	if ((x[4] > 100) && (x[4] <= 128))
	{
		fpasg(result, halfpi);
	}
	else        /* go through evaluation */
	{
		fpmag(y, x);
		atanev(result, y);
	}

	if (x[3] > 127)
	{
		return (fpchs(result, result));
	}
	else
	    return (result);
}


char *arctan2(result, quadrant, opside, adjside)

char result[5], opside[5], adjside[5];
int *quadrant;

{
	char x[5], *fpmag(), *fpchs();
	*arctan();
	int opsign, adjsign;
	char *zero, *halfpi;

	halfpi = "\171\356\207\144\1";

	zero = "\0\0\0\0\0";

	opsign = fpcomp(opside, zero);
	adjsign = fpcomp(adjside, zero);

	if ((adjsign == 0) && (opsign == 0))
	{
		*quadrant = 0;
		fpasg(result, zero);
		return (result);
	}

	if (((adjside[4] > 128) && (adjside[4] < 226)) || (adjsign == 0))
		fpasg(result, halfpi);
	else
	    {
		fpdiv(x, opside, adjside);
		fpmag(x, x);
		arctan(result, x);
	}

	if ((adjsign == 0) && (opsign > 0))
	{
		*quadrant = 1;
		return (result);
	}
	if ((adjsign == 0) && (opsign < 0))
	{
		*quadrant = 4;
		fpchs(result, result);
		return (result);
	}
	if ((adjsign > 0) && (opsign == 0))
	{
		*quadrant = 1;
		return (result);
	}
	if ((adjsign < 0) && (opsign == 0))
	{
		*quadrant = 2;
		fpchs(result, result);
		return (result);
	}
	if ((adjsign > 0) && (opsign > 0))
	{
		*quadrant = 1;
		return (result);
	}
	if ((adjsign > 0) && (opsign < 0))
	{
		*quadrant = 4;
		fpchs(result, result);
		return (result);
	}
	if ((adjsign < 0) && (opsign > 0))
	{
		*quadrant = 2;
		fpchs(result, result);
		return (result);
	}
	if ((adjsign < 0) && (opsign < 0))
	{
		*quadrant = 3;
		return (result);
	}
}


	/* page eject */

/*additional functions for floating point operations*/

char *sqrt(result, sign, op1)
char *result, *op1;
int *sign;
{
	char *atof(), *log10(), *fpdiv(), *exp10();
	char two[5];

	atof(two, "2");
	log10(result, sign, op1);
	fpdiv(result, result, two);
	exp10(result, result);

	return (result);
}


