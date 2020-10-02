/*@*****************************************************/
/*@                                                    */
/*@ itoa - convert integer to ascii. Stolen from K & R */
/*@        Very useful to prevent loading of printf    */
/*@        and the 2-8K of follow-ons.                 */
/*@                                                    */
/*@   Usage:     itoa(num, buffer);                    */
/*@       where num is an integer.                     */
/*@             buffer is a char area large enough to  */
/*@                to hold the resulting string.       */
/*@                                                    */
/*@   Returns a pointer to the buffer.  This allows    */
/*@       nesting of puts(itoa(n, buffer)); variety.   */
/*@                                                    */
/*@*****************************************************/

char *itoa(n,s)		/* convert n to characters in s */
char s[];
int n;
{
	int i,sign;

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
