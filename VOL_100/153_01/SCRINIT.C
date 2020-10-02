/*                          *** scrinit.c ***                        */
/*                                                                   */
/* IBM - PC microsoft "C"                                            */
/*                                                                   */
/* Function to initialize the screen and keyboard control arrays.    */
/* Currently is only a dummy function getting its information from   */
/* a header file (scontrol.h).                                       */
/*                                                                   */
/* Written by L. Cuthbertson, April 1984.                            */
/*                                                                   */
/*********************************************************************/
/*                                                                   */

#include "scontrol.h"

scrinit()
{
	return(0);
}

	static int monthd[] = {31,28,31,30,31,30,31,31,30,31,30,31};
	int i;
	int leapd,iyr,