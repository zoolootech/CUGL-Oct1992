/*@*****************************************************/
/*@                                                    */
/*@ mak_dec - insert a decimal point in a numeric      */
/*@        string.  If the string is not long          */
/*@        enough, leading zeros will be inserted.     */
/*@                                                    */
/*@   Usage:     mak_dec(string, places);              */
/*@       where string is a string of digits.          */
/*@             places is the number of places after   */
/*@                the decimal point.                  */
/*@                                                    */
/*@    Returns a pointer to the string.                */
/*@                                                    */
/*@    NOTE:  The string area must be long enough to   */
/*@         hold the original string, the decimal      */
/*@         point, and any added leading zeros.  It    */
/*@         may be wise to copy the string to a large  */
/*@         string buffer before use.                  */
/*@                                                    */
/*@         A typical use might be:                    */
/*@           puts(mak_dec(itoa(n_max, strbuf), 2));   */
/*@                                                    */
/*@*****************************************************/

char *mak_dec(s, n)
char *s;
int n;
{
	int len, i;
	char *save;

	save = s;
	len = strlen(s);		/* find end of it */

	if (len <= n) {
		for (i=len; i >= 0; i--)	/* make room for place holder zeros */
			s[i+n-len+1] = s[i];
		for (i=0; i < n-len+1; i++)
			s[i] = '0';			/* add zero place holders */
		len = n +1;
	}

	for (i=len; i >= (len - n); i--) {
		if (s[i] == ' ')
			s[i] = '0';		/* add trailing zeros */
		s[i+1] = s[i];		/* make room for decimal */
	}

	s[len-n] = '.';			/* add decimal */
	if (s[len-n-1] == ' ')
		s[len-n-1] = '0';	/* add leading zero before decimal, if necessary */
	
	return save;			/* make avail as stacked function */
}