/*
HEADER:         CUGXXX;
TITLE:          Mathematical subroutines;
DATE:           11-1-85;
DESCRIPTION:    Mathematical subroutines;
KEYWORDS:       Mathematics, Math functions;  
FILENAME:       MATHMAX.H;
WARNINGS:       None;
AUTHORS:        Max R. D�rsteler;
COMPILER:       Lattice C, Microsoft C;
REFERENCES:     US-DISK 1307;
ENDREF
*/
/*
 *	S T A N D A R D   H E A D E R	F I L E
 *
 *		M A T H   P A C K A G E
 */

	/* Functions */

extern double fabs(), floor(), ceil(), fmod();
extern double sqrt(), exp(), exp2(), log(), log10(), pow(), pw10();

extern double sin(), cos(), tan(), asin(), acos(), atan(), atan2();

extern double gamma();

struct complex {     /* structure for complex numbers */
	double rl;
	double im;
};
	/* Usefull constants */

#define BLKSIZE 512
#define MAXINT	32767
#define BIG	72057594037927936.		/* Maximum precision of DOUBLE */
#define HUGE	1.701411733192644270e38 	/* Largest DOUBLE */
#define LOGHUGE 39				/* 10^39 --> Largest power */
#define LN2	0.69314718055994530941
#define LN10	2.30258509299404568401
#define E	2.7182818284590452353602874
#define SQRT2	1.41421356237309504880
#define HALFSQRT2	.70710678118654752440
#define PI	3.141592653589793238462643
#define QUARTPI 0.78539816339744830962
#define HALFPI	1.57079632679489661923
#define TWOPI	6.28318530717958647692
#define RADPDEG 0.01745329251994329576

		/* macros */

#define sqr(x)	((x) * (x))
#define sgn(x)	((x) < 0 ? -1 : 1)
#define xswap(a,b) a ^= b, b ^= a, a ^= b
#define swap(a,b,c) c = a, a = b, b = c
#define max(x,y)	((x) >= (y) ? (x) : (y))
#define min(x,y)	((x) <= (y) ? (x) : (y))
#define abs(x)		((x)<0? -(x):(x))	/* Integer Absolute value */
#define fabs(x) 	((x)<0.? -(x):(x))	/* Floating absolute value */
#define mod(x,y)	((x)%(y))		/* Integer modulo */
#define logE(x) 	(log(x))		/* Natural log */
#define ln(x)		(log(x))		/* Natural log */
#define cos(x)		(sin(x+HALFPI)) 	/* Cosinus [radians] */
#define tan(x)		(sin(x)/cos(x)) 	/* Tangens [radians] */

/* struct to define low and high bytes of a word. */
struct lohi {
	char lo, hi;
} ;


/* struct to define low and high words of a long int */



struct llohi {
	int lhi, llo;
} ;
