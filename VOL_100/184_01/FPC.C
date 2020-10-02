/* fpc.c--tests conversion to/from amd9511 fpp format 

Used to verify format conversion routines used in FLTLB.
See accompanying documentation.  Other useful information
is in my article "Faster floating point math," which appeared
on pp.46-54 of the Nov/Dec 1985 issue of Micro/Systems Journal.
*/

#include fprintf.h
#include c80def.h

#define MESSAGE "\nFloating-point format conversion program")
#define DASHES "\n\n----------------------------------------")

#define MAX 6	/* how many different values to feed 
		     to the format conversion routines */

/* replace the next two functions with 
     your own format conversion routines as needed */
extern long c2amd();	/* link .rel file containing these */
extern float amd2c();	/*   to fpc.rel */

main()
{
	int i;
	static int sx=10;
	float x;
	static float dx=1.0;

	printf(MESSAGE);
	printf(DASHES);

	/* a geometric series of positive values */
	for (i = 0, x = 100.0; i<=MAX; x /= sx, i++) showbits(x);
	printf(DASHES);

	/* a geometric series of negative values */
	for (i = 0, x = -100.0; i<=MAX; x /= sx, i++) showbits(x);
	printf(DASHES);

	/* a linear series of positive and negative values */
	for (i = -MAX, x=(float)(-MAX); i<=MAX; x += dx, i++) showbits(x);
}


/* show bit patterns used by C80 and AMD FPP floating point formats 
     to represent the float n */
showbits(n)
float n;
{
	int i;
	union {
		float f;
		long l;
	} x,z;	/* unions are easier to use here than pointers */
	long y;

	x.f=n;
	/* show C80's preconversion bit pattern */
	printf("\n\nx = %e,\t\tBCDE = ",x.f);
	prntlong(x.l);

/* eliminate next few lines if you just want to check the format
     used by your version of C */
	/* show AMD formatted data */
	y=c2amd(x.f);
	printf("\n\t\t\t\t AMD = ");
	prntlong(y);

	/* convert back to C80's format */
	z.f=amd2c(y);
	printf("\nz = %e,\t\tBCDE = ",z.f);
	prntlong(z.l);
/* end of C80 -> AMD -> C80 format conversion test */
}


/* print bit pattern of a long, starting with the high-order bit
     of the most significant byte, working down from left to right */
prntlong(k)
long k;
{
	int i;
	union {
		long l;
		char b[4];
	} datum;

	datum.l=k;
	for (i=3; i>=0; i--) {
		prntbyt(datum.b[i]);
		printf(" ");
	}
}


/* print the bit pattern for a byte from left to right, 
     high order bit first (does the dirty work for prntlong) */
prntbyte(i)
int i;
{
	int j;
	char bit;

	for (j=0x80; j>0; j=j>>1) {
		if (i & j) bit='1';
		else bit='0';
		printf("%c",bit);
	}
}
