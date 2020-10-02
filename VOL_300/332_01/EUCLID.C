/*----------------------------------------------------*- Fundamental -*-

Facility:		euclid

File:			euclid.c

Associated files:	- (none)

Description:		This is an implementation of the algorithm
			1.1E given in [1] describing a solution
			(by Euclid) to the problem of finding the
			greatest common	denominator for two given
			integers.

Portability:		Conforms to X/Open Portability Guide, ed. 3,

References:	    [1] Knuth, Donald E:
			The Art of Computer Programming,
			  Vol. 1 - Fundamental Algorithms, ed. 2,
			Addison-Wesley, 1968

		    [2] X/Open Company, Ltd:
			  X/Open Portability Guide, ed. 3 (7 vol)
			Prentice-Hall, 1989

Author:			H. Moran (?)

Editor:			Anders Thulin
			Rydsvagen 288
			S-582 50 Linkoping
			SWEDEN

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

Edit history :

Vers  Ed   Date	       By                Comments
----  ---  ----------  ----------------  -------------------------------
 1.0    0  19xx-xx-xx  H. Moran (?)
 1.1    1  1988-10-25  Anders Thulin 	 :>

    Did not compile under K&R: General cleanup: changed to
    K&R syntax, added include files, some cleanup of logic,
    removed unnecessary variables, added some error checking. 

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*---  Configuration:  -------------------------------------------------

System configuration:
=====================

  ANSI		ANSI C, 
  BSD		BSD Unix, SunOS 3.5
  SV2		AT&T UNIX System V.2, AU/X
  XPG3		X/Open Portability Guide, ed. 3

If you have an ANSI C compiler, define ANSI.  Otherwise, define the
system that best matches your environment.


Program configuration:
======================

  INPUT_LEN	The max length of an input line

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#define	ANSI	1
#define	BSD	0
#define	SV2	0
#define	XPG3	0

#define	INPUT_LEN	160	/* characters */

/* - - End: configuration options - - - - - - - - - - - - - - - - - - - - - */

#if ANSI
# include <stdio.h>
# include <stdlib.h>
#endif

#if BSD
# include <stdio.h>
# define EXIT_SUCCESS	0
  extern long strtol();
#endif

#if SV2
# include <stdio.h>
# define EXIT_SUCCESS	0
  extern long strtol();
#endif

#if XPG3
# include <stdio.h>
# include <stdlib.h>
#endif

/*----------------------------------------------------------------------

Routine:	main

Description:	main program

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

int main()
{
  long r, m, n;
  long temp;
  char string[INPUT_LEN];	/* input string */

  puts("euclid - computes largest common divisor of two integers");

  /*  1.  Job loop:  */

  while (1) {

   /*
    *  1a.  Get the two numbers m and n.
    *
    *  Note that using gets() is in general bad practice - there
    *  is no check that input doesn't overrun the input buffer and
    *  destroys important info.  fgets() is better.
    *
    */

    printf("\nenter first integer  ( > 0) : ");
    m = strtol(gets(string), (char **)0, 10);
    if (m <= 0) break;

    printf("enter second integer ( > 0) : ");
    n = strtol(gets(string), (char **)0, 10);
    if (n <= 0) break;

    /*  1b.  Ensure that m >= n:  */

    if (m < n)  {
      temp = m;
      m = n;
      n = temp;
    }

    /*  1c.  Euclid's algorithm:  */

    r = m % n;
    while (r != 0) {
      m = n;
      n = r;
      r = m % n;
    }

    printf("\nlargest common divisor = %ld\n", n);

  }

  /*  2.  Terminate nicely:  */

  return EXIT_SUCCESS;					
}

