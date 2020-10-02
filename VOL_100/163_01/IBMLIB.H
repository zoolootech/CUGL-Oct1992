/*	IBM.LIB special 8086 PC compatible functions	       */
/*	1985 agassiz computer consultants		       */

extern int
	   inp(),	/*	inp(port)		       */
	   outp(),	/*	outp(port,byte) 	       */
	   pokec(),	/*	pokec(seg,off,byte)	       */
	   tone(),	/*	tone(freq,duration)	       */
	   sign(),	/*	sign(I) 		       */
	   fraction(),	/*	fraction(I,factor,divisor)     */
	   seed(),	/*	seed(I) 		       */
	   randint(),	/*	randint()  +-MAXINT	       */
	   irand(),	/*	irand(Om)    < Om	       */
	   beep(),	/*	beep(pips)		       */
	   locate(),	/*	locate(x,y) true if on screen  */
	   pos(),	/*	pos(p,s) *p position in *s     */
	   strappend(), /*	append(*a,*b) b onto a	       */
	   strmatch(),	/*	match(*a,*b) a begins b        */
	   replace();	/*	replace(*L,*a,*b) a -> b in L  */

