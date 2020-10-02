#include "stdio.h"

/*@*****************************************************/
/*@                                                    */
/*@   center - center a given string on a given length.*/
/*@            Trims trailing blanks before centering. */
/*@                                                    */
/*@    Usage:  center(string, length);                 */
/*@       NOTE: The string is output to STDOUT.        */
/*@                                                    */
/*@*****************************************************/

center(line, linlen)
int linlen;
char *line;
{
	char *cp, *wp;
	int countr, len;
	char c;

	len = strlen(trim(line));	/* get non-blank length */

     /* output leading blanks */
	for (countr=1; countr < (linlen - len)/2; countr++)
		putc(' ', stdout);

     /* output non-blank chars */
	while (c = *cp++)
		putc(c, stdout);

	putc('\n', stdout);     /* end of record */

}
