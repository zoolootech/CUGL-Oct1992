/*                LONG INTEGER PACKAGE

	  The functions below, together with the Z-80 assembly
	language coded routine li, constitute a package to
	provide long integer capability to BDS C via function
	calls similar to those provided by Bob Mathias'
	floating point package.

	  Note that, while the functions below are portable,
	the basic workhorse function in the backage, li, will
	only run on a Z-80 processor.  Further, li assumes that
	the BDS C version 1.4x stack conventions are being used.

	  The Long Integer Package and the function li were
	programmed by Paul J. Gans, Department of Chemistry,
	New York University, New York, NY  10003.  Phone
	(212) 598-2515 9 am to 5 pm.

	  Please let me know about bugs, problems, improvements,
	or what you will.  That's the only way things will
	get better.

	  BDS C is copyright 1980 by Leor Zolman.

	  Version record:

	Version 1.0	March 6, 1981		Original version.

	ver. 1.0a	9/12/82	by SP	ltou() and utol() added,
					bugs in ltoa(), ltoi(), fixed
					

	ver  1.1	who's gonna rewrite li() (in longcode.mac) in
			casm for the 8080?

	  The Long Integer Package and the function li are
	copyright (c) 1981 by Paul J. Gans.


*/

#define   ADD     0
#define   SUB     1
#define   MUL     2
#define   DIV     3
#define   MOD     4

/*   ladd adds the addend and augend to produce a 32 bit
   signed result in sum.  A pointer to sum is returned.  */

char *ladd(sum,addend,augend)
char sum[], addend[], augend[];
{
	char *li();

	return (li(ADD,sum,addend,augend));
}

/*   lsub subtracts subtrahend from minuend and places the
   32 bit signed difference in difference.  A pointer to
   difference is returned.                              */

char *lsub(difference,minuend,subtrahend)
char difference[], minuend[], subtrahend[];
{
	char *li();

	return (li(SUB,difference,minuend,subtrahend));
}

/*   lmul multiplies plier and plicand together and places
   the signed low order 32 bit product in product.  A
   pointer to product is returned.                      */

char *lmul(product,plier,plicand)
char product[], plier[], plicand[];
{
	char *li();

	return (li(MUL,product,plier,plicand));
}

/*   ldiv divides dividend by divisor and places the signed
   low order 32 bits of the quotient in quotient.  A
   pointer to quotient is returned.                   */

char *ldiv(quotient,dividend,divisor)
char quotient[], dividend[], divisor[];
{
	char *li();

	return (li(DIV,quotient,dividend,divisor));
}

/*   lmod places the 31 bit positive remainder resulting
   from the division of dividend by divisor into residue.
   A pointer to residue is returned.                   */

char *lmod(residue,dividend,divisor)
char residue[], dividend[], divisor[];
{
	char *li();

	return (li(MOD,residue,dividend,divisor));
}

/*   lneg places the 32 bit signed negative of orignum in
   negnum and returns a pointer to it.                  */

char *lneg(negnum,orignum)
char negnum[], orignum[];
{
	char *li(), work[4], i;

	for (i=0; i<4; ++i) work[i]=0;
	return (li(SUB,negnum,work,orignum));
}

/*   itol converts an integer to long form.  A pointer
   to the longform is returned.                       */

char *itol(longform,i)
char longform[];
int i;
{
	longform[3]=longform[2]=0;
	if (i<0) longform[3]=longform[2]=0xff;
	longform[1]=i>>8;
	longform[0]=i&0xff;
	return longform;
}

/*   ltoi returns a properly signed integer containing the
   low order 15 bits of precision of the long integer
	longint.  The value of this integer is placed in i and
   is returned by this function.                         */

/* note, bug here, value isn't placed in 'i' of calling function,
	fix by passing ADDRESS of 'i'.  also, left shifting a char
	8 times empties it, not the intended action.  substituted 
	appropriate code.  9/82 SP
*/

ltoi(i,longint)
int *i;				/* i is a pointer to int */
char *longint;
{
	char work[4];

	movmem(longint, work, 4);
	if (work[3] > 127) lneg(work, work);	/* convert from 2s compliment? */
	*i = ((work[1] & 0x7f) * 256) + work[0];
	if (longint[3] > 127) *i = -(*i);
	return *i;
}

/*   atol converts the ASCII string s to a long integer
   placed in longint and returns a pointer to longint.
   Acceptable format for atol is:  any amount of white
   space followed by an optional sign followed by a string
   of decimal digits.  The first non-decimal digit stops
   the scan.  The method used is an adaptation of that
   presented in Kernighan and Ritchie.                */

char *atol(longint,s)
char longint[], s[];
{
	char work[4], ten[4];
	int i, j, n, sign;

	/* form a constant.. */

	ten[0]=0x0a; ten[1]=ten[2]=ten[3]=0;
	/*   first skip white space... */

	for (i=0; s[i]==' ' || s[i]=='\n' || s[i]=='\t'; i++);

	/*   now get the sign... */

	sign=1;
	if (s[i]=='+' || s[i]=='-')
		sign=(s[i++]=='+') ? 1 : -1;

	/*   and convert the string straightforwardly... */

	for (j=0; j<4; ++j) longint[j]=0;
	while (s[i]>='0' && s[i]<='9') {
		for (j=0; j<4; ++j) work[j]=0;
		work[0]=s[i++]-'0';
		lmul(longint,ten,longint);
		ladd(longint,longint,work);
	}

	/*   not forgetting the original sign... */

	if (sign==-1) lneg(longint,longint);
	return longint;
}

/*   ltoa converts the long integer longint to an ASCII
   string which is placed in s.  If longint is negative a
   minus sign is prefixes the string.  The algorithm used
   is an adaptation of itoa presented in Kernighan and
   Ritchie.                                          */

/* note, this had a bug in that it destroyed the contents of
	longint 9/82  SP */

ltoa(s,longint)
char *s, *longint;
{
	char c, work[4], work2[4], ten[4];
	int i, j, k, sign;

	ten[0]=0x0a; ten[1]=ten[2]=ten[3]=0;
	movmem(longint, work2, 4);					/* save contents of longint */
	sign=1;

	/*   check the sign... */

	if (longint[3]>127) {
		sign=-1;
		lneg(work2,work2);
	}

	/*   convert, generating digits backwards in s[]... */

	i=0;
	do {
		lmod(work,work2,ten);
		s[i++]=work[0]+'0';
		ldiv(work2,work2,ten);
	} while ((work2[0]|work2[1]|work2[2]|
		work2[3])!=0);

	/*   postfix a minus sign if needed... */

	if (sign==-1) s[i++]='-';
	s[i]='\0';

	/*   and turn the whole thing around in place. */

	k=strlen(s);
	for (i=0, j=k-1; i<j; i++, j--) {
		c=s[i]; s[i]=s[j]; s[j]=c;
	}

	return s;
}

/********************************************************

	following added for cnode use:

*/

/*   utol converts an unsigned to long form.  A pointer
   to the longform is returned.                       */

char *
utol(longform, u)
char *longform;
unsigned u;
{
	longform[3] = longform[2] = 0;
	longform[1] = u >> 8;
	longform[0] = u & 0xff;
	return longform;
}

/*   ltou returns an unsigned integer containing the
	low order 16 bits of precision of the long integer
	longform.  The value of this integer is placed in u and
	is returned by this function.  It assumes that long is
	positive.
*/

ltou(u, longform)
unsigned *u;
char *longform;
{
	return (*u = (longform[1] * 256) + longform[0]);
}
0]|work2[1]|work2[2]|
		work2[3])!=0);

	/*   postfix a 