/*	file atof.c	*/
/*	convert ASCII string to float	*/
#asm
	NAME	('ATOF')
#endasm

extern	float	fmten(),fdten();
static float	val, power,xpow,xval;
static int	c,sign,exp,xsign;
static	char	*pc0;
float atof(s)	/* convert string s to float */
char	s[];
{
	pc0 = &s[0];
	while((c = *pc0) == ' ' || c == '\n' || c == '\t')
		pc0++;		/* skip white space */
	xsign =1;
	xval = exp = 0;
	comlup(s);
	sign = xsign;
	xpow = exp = 1;
	if(c == '.')
		comlup(s);
	val= xval;
	power = xpow;
	xsign = 1;
	if(c == 'e' || c == 'E'){
		xval = exp = 0;
		xpow = xsign = 1;
		comlup(s);
		exp = xval * xsign;
		if(xsign == 1) {
			while(exp-- > 0) {
				power = fdten(power);
			}
		}
		else {
			while(exp++ < 0) {
				power =fmten(power);
			}
		}
	}
	return(sign * val / power);
}
comlup()	/* common loop */
{
	if((c = *pc0) == '+' || c == '-') {		/* sign */
		++pc0;
		xsign = (c == '+') ? 1 : -1;
	}
	while(( c = *pc0++) >= '0' && c <= '9') {
		xval = fmten(xval) + (c - '0');
		xpow = fmten(xpow);
	}
}
