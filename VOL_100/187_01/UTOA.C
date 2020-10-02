/*@*****************************************************/
/*@                                                    */
/*@ utoa - convert unsigned integer to ascii.          */
/*@        Very useful to prevent loading of printf    */
/*@        and the 2-8K of follow-ons.                 */
/*@                                                    */
/*@   Usage:     utoa(num, buffer);                    */
/*@       where num is an unsigned integer.            */
/*@             buffer is a char area large enough to  */
/*@                to hold the resulting string.       */
/*@                                                    */
/*@   Returns a pointer to the buffer.  This allows    */
/*@       nesting of puts(utoa(n, buffer)); variety.   */
/*@                                                    */
/*@*****************************************************/

utoa(n,s)		/* convert n to characters in s */
char s[];
unsigned n;
{
	int i;

	i = 0;
	do {				/* generate digits in reverse order */
		s[i++] = n % 10 + '0';          /* get next digit */
	} while ((n /= 10) > 0);		/* delete it */
	s[i] = '\0';
	reverse(s);
	return s;
}
