	/*	file ftoa.c	*/
	/*	convert float type to ASCII string	*/
#asm
	NAME	('FTOA')
#endasm

#define	BUFL	40
static	char	*pc;
static	int	m[12],chinc,n,stz,d;
static	int	*pm3,*pm2, *pm1, *pm0 = &m[0];
static	int	*rndbak();
extern	float	fmten(),fdten();

char	*
ftoa(fmt,digits,fl,where)
char	fmt;
int	digits;
float	fl;
char	where[];
{

	static	char	c;
	static	int	declim,expint,moc,expd,a;
	static	float	fn,fz;

	pc = &where[0];
	fn = fl;
	declim = digits;
	d = fmt;
	chinc = 0;

    if(fn == fz) {
    	putk('0');
    }
    else {
	moc = stz = 0;
	if(d == 'f' || d == 'F')
		moc = 1;	/* mode of conversion is Float */
	if(d <= 'F')
		stz = 1;	/* suppress trailing zero's */

	if(fn < 0) {
		putk('-');
		fn = -fn;
	}

	expint = 0;
	if(fn > 9) { /* pos expnt, not decimal normal */
		while(expint < 18) {
			if(fn < 10)
				break;
			fn = fdten(fn);
			++expint;
		}
	}
	else if(fn < 1) {  /* neg exponent */
		while(expint > -20) {
			if(fn >= 1)
				break;
			fn = fmten(fn);
			--expint;
		}
	}
	else;	/* fn is decimal normal */

	expd = expint;
	if(moc == 0)	/* exponential mode */
		expint = 0;
	a = 7;
	if((n = (declim + expint + 2)) < a && n > 1)
		a = n;

	*pm0 = '0';
	for(pm1 = pm0 +1; pm1 <= pm0 +(a+2);){ /* mantissa to ASCII */
		*pm1++ =((d = fn) + '0');
		fn = fmten(fn - d);
	}

	while(pm1 >= pm0 + a) {		/* round back */
		pm3 = pm1 = rndbak(pm1,'5','5');
		while((pm2 =rndbak(pm3,'9','0')) >= pm0 && pm2 != pm3)
			pm3 = pm2;
	}

	pm1 = pm0;
	if(*pm0 != '0') {
		++expd;
		if(moc == 1)	/* float format */
			++expint;
	}
	else
		++pm1;
	for(d = 0; chinc < BUFL; ++d, --expint) {
		if(expint >= 0) {
			d = 0;
			putn();
		}
		else {
			if(d == 1)
				putk('.');
			if((a = (d + expd)) < 0 && moc == 1) {
				putk('0');	/* leading zero */
			}
			else if((n = putn()) == 1)
				break;
			else;
		}
		if(d >= declim)
			break;
	}
	if(moc == 0) {
		putk('e');
		if(expd < 0) {
			putk('-');
			expd = -expd;
		}
expd;
#asm
	XCHG
	LXI	H,10
	CALL	c.div##
	PUSH	D
	LXI	B,48
	DAD	B
	PUSH	H
	CALL	putk
	POP	H
	POP	H
	DAD	B
	PUSH	H
	CALL	putk
	POP	H
#endasm
	}
    }
    putk('\0');
    return(--pc);
}

putk(c)
int	c;
{
	if(chinc++ < BUFL)
		*pc++ = c;
}

static	int	*rndbak(ip,hilev,lolev)
int	*ip, hilev, lolev;
{
	static	int	*ips;
	ips = ip;
	if(*ips > hilev) {
		*ips-- = '\0';
		++(*ips);
	}
	else if(*ips <= lolev)
		*ips-- = '\0';
	else;
	return(ips);
}

putn()
{
	if(*pm1 != '\0')
		putk(*pm1++);	/* significant digit */
	else if(stz == 0 || d == 0) {
		putk('0');	/* trailing zero */
		return(0);
	}
	else
		return(1);
}
