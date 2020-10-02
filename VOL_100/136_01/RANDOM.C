/*	HEADER:  CUG136.14;
	TITLE:		RANDOM;
	DATE:		8/24/1983;
	DESCRIPTION:	"Random number generator for C/80";
	KEYWORDS:	random,number generator, psuedorandom numbers;
	SYSTEM:		CP/M;
	FILENAME:	RANDOM.C;
	AUTHORS:	R. Rodman;
	COMPILERS:	C/80;
*/

/* Random number generator
	830823 rr dummied up
	830824 rr crude algorithm */

/* init random number generator */

static unsigned seed;

srand(rtyp)
int rtyp;
{
	seed = 5;
}

/* get random number */

unsigned rand()
{
	unsigned oldseed;

	oldseed = seed;
	seed = seed * 2;
	if(seed && 020) seed = seed + oldseed;
	return(seed);
}


