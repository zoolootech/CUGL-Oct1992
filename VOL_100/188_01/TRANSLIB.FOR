/*
HEADER:		;
TITLE:		C elementary transcendentals;
VERSION:	1.0;

DESCRIPTION:   	Source    code    for   all   standard    C                            
               transcendentals

		Employs  ldexp()  and  frexp()  functions;  if                
               suitable  versions  of these are  not  provided                
               by  a given compiler,  the versions provided in                
               source  code  will require  adaptation  to  the                
               double float formats of the compiler.

KEYWORDS:	Transcendentals, library;
SYSTEM:		CP/M-80, V3.1;
FILENAME:	TRANS.C;
WARNINGS:  	frexp()   and  ldexp()  are   implementation                
               dependent.   The  compiler  employed  does  not 
               support    minus   (-)   unary   operators   in                
               initializer  lists,  which are required by  the                
               code.
AUTHORS:	Tim Prince;
COMPILERS:	MIX C, v2.0.1;
*/
	program transt
	real log
	sum=0
	do 1 i=1,1000
	  x=i*.01
1	sum=abs(tan(atan(x))/x-1)+sum
	write(3,2)sum
2	format(' built-in tan(atan) error:',g15.7)
	sum=0
	do 3 i=1,1000
	  x=i*.01
3	sum=abs(exp(log(x))/x-1)+sum
	write(3,4)sum
4	format(' built-in exp(log) error:',g15.7)
	sum=0
	do 5 i=1,1000
	  x=i*.01
5	sum=abs(xtan(xatan(x))/x-1)+sum
	write(3,6)sum
6	format(' tan(atan) error:',g15.7)
	sum=0
	do 7 i=1,1000
	  x=i*.01
7	sum=abs(xexp(xlog(x))/x-1)+sum
	write(3,8)sum
8	format(' exp(log) error:',g15.7)
	end

      function xtan(x)
	xtan=xsin(x)/xcos(x)
	return
      end

      function xcos(x)
	xcos=xsin(1.57079633-x)
	return
      end

      function xsin(x)
	real table(5)
	data pi/3.141592653589793238/,
     &	  table/2.60198e-6,-.0001980746,.0083330258,
     &	    -.16666657,.99999999/
c series to 28 bit accuracy
c nearest multiple of pi
	pim=anint(x/pi)
c get remainder
	xr=x-pi*pim
c sin(x-pim*pi) = (-1)**pim * sin(x)
	hpim=scal2(pim,-1)
c if pim/2 not integer, pim is odd so change sign
	if(aint(hpim).ne.hpim)xr=-xr
c evaluate Horner polynomial, odd terms
	xsin=poly(xr*xr,5,table)*xr
	return
      end

      function scal2(x,i)
c	scal2=x*2**i
	integer*1 ix(4),i
	equivalence(xi,ix(1))
	xi=x
	ix(4)=i+ix(4)
	scal2=xi
	return
      end

      function poly(x,n,table)
	real table(n)
	poly=table(1)
	do 1 i=2,n
1	poly=poly*x+table(i)
	return
      end

      function xatan(x)
	logical invert
	real table(8)
	data table/-.0046930,.0242506,-.0594827,.0991394,
     &	  -.1401932,.1996969,-.3333199,.9999999/
	xr=abs(x)
	invert=xr.gt.1
	if(invert)xr=1/xr
	xr=poly(xr*xr,8,table)*xr
c tan(1/x)= tan(pi/2-x)
	if(invert)xr=1.57079633-xr
	xatan=sign(xr,x)
	return
      end

      function xlog(x)
	xlog=alog2(x)*.693147181
	return
      end

      function alog2(x)
	real table(3)
	integer*1 ix(4),iexp
	equivalence(xr,ix(1))
c polynomial to 24 bit precision
	data table/.5957779,.9615884,2.8853901/
	xr=x
c z'35' is leading 7 bits (minus hidden bit) of sqrt(.5)
c shift so xr close to 1.
c .true. value -1 on this system, bias z'80'
	iexp=(ix(3).le.z'35')+ix(4)-z'80'
c subtract 1 first so no accuracy lost
	xr=scal2(xr,-iexp)-1
	xr=xr/(xr+2)
c polynomial in (xr-1)/(xr+1), .7 < xr < 1.4
	alog2=poly(xr*xr,3,table)*xr+iexp
	return
      end

      function xexp(x)
	xexp=exp2(x*1.442695040888963407)
	return
      end

      function exp2(x)
	integer*1 expn
	real table(7)
c 26 bits precision -.5 < xr < .5 for 2**x
c sinh (|x|<.7) use odd terms (x*1.442695)
	data table/.000154,.00133904,.00961814,.05550358,
     &	  .2402265,.69314718,1./
	if(x.ge.-128)go to 1
	  exp2=0
	  return
1	xr=amin1(x,126.9999)
	expn=anint(xr)
	xr=xr-expn
	exp2=scal2(poly(xr,7,table),expn)
	return
      end
