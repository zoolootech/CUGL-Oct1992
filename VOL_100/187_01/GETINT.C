/*@*****************************************************/
/*@                                                    */
/*@ getint - read in two binary bytes and treat them   */
/*@        as a reverse 2-byte number.                 */
/*@                                                    */
/*@   Usage:     getint(fp);                           */
/*@       where fp is a file handle.                   */
/*@     Returns an int which is the binary value.      */
/*@                                                    */
/*@*****************************************************/

#include "stdio.h"

/*******************************************************/

int getint(fp)
FILE	   *fp;       /* File to process	    */
/*
 * Read two binary bytes and treat them as reverse 2-byte number.
 */
{
	char c;

	c = fgetc(fp);   
	return(c+(256*fgetc(fp)));
}


