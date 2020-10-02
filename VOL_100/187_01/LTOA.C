/*@*****************************************************/
/*@                                                    */
/*@ ltoa - convert long to ascii.                      */
/*@        Very useful to prevent loading of printf    */
/*@        and the 2-8K of follow-ons.                 */
/*@                                                    */
/*@   Usage:     ltoa(num, buffer);                    */
/*@       where num is a long.                         */
/*@             buffer is a char area large enough to  */
/*@                to hold the resulting string.       */
/*@                                                    */
/*@   Returns a pointer to the buffer.  This allows    */
/*@       nesting of puts(ltoa(n, buffer)); variety.   */
/*@                                                    */
/*@*****************************************************/

char *ltoa(n,s)		/* convert n to characters in s */
char s[];
long n;
{
	long sign;
	int i;

	if ((sign = n) < 0)  /* record sign */
		n = -n; 		/* make positive */
	i = 0;
	do {				/* generate digits in reverse order */
		s[i++] = n % 10 + '0';          /* get next digit */
	} while ((n /= 10) > 0);		/* delete it */
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s);
	return s;
}
