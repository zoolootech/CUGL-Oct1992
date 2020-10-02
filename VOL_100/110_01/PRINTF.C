/*
	from STDLIB2.C for BDS C 1.41


	This version of _spr adds the '0' padding option (see K&R, section
	7.3) to the printf, etc. functions.
	If a leading 0 is given in the field length for numeric conversions
	the leading spaces are changed to leading 0s. Useful when two
	numbers are to be concatenated for printing, e.g., split octal
	notation: "017.002".
	One side effect of this is that a right justified character or
	string is also allowed to have leading 0s, but K&R's description
	doesn't appear to disallow it, so...
*/

_spr(line,fmt)
char *line, **fmt;
{
	char _uspr(), c, base, *sptr, *format;
	char wbuf[MAXLINE], *wptr, pf, ljflag;

/*
	the first modification: define a variable to hold the
	padding character.
*/
	char padchr;

	int width, precision,  *args;

	format = *fmt++;    /* fmt first points to the format string	*/
	args = fmt;	    /* now fmt points to the first arg value	*/

	while (c = *format++)
	  if (c == '%') {
	    wptr = wbuf;
	    precision = 6;
	    ljflag = pf = 0;

	    if (*format == '-') {
		    format++;
		    ljflag++;
	     }
/*
	here's the additional logic: simply test if the character
	following the '%' or '-' is a '0', and set padchr to
	'0' if true, otherwise ' '.
*/
	    else if