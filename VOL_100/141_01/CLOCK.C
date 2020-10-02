/* clock.c - hack for newclock 80 */
#include <bdscio.h>
#define port 176

main ()	{
	int digit, i ; char *ptr ;
	printf ("\N") ;
	for (;;)	{
		for (i = 12 ; 0 <= i ; --i)	{
			digit =  (0xF & inp (port + i) ) ;
			switch (i)	{
			case 10: case 8: printf ("/") ; break ;
			case  5:         digit &= 0x3 ; break ;
			case  3: case 1: printf (":") ; break ;
			default: ;	}
			if ( i != 6 ) printf ("%01d", digit) ;
			else {	ptr = (3 * digit) +
				"SunMonTueWedThuFriSat" ;
				printf (" %3.3s ", ptr) ;
			} /* day name */
		} /* 13 digits */
	while ( digit == (0xF & inp (port)) )	; /* wait */
	printf ("\r") ; /* backspace */
	} /* forever */
} /* main */


	while ( digit == (0xF & inp (port)) )	; /* wait */
	printf ("\r") ; /* backsp