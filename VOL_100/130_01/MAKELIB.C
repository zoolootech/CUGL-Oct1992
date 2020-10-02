


gotoxy( y, x )

char y, x ;
{
unsigned z ;
char *p, a ;

if(x > 15 && y > 47)return( 0 );

p = 0xfc02 ; z = *p++ ; z += (*p << 8) ;
p = z ;
if(*p == 0x7f)*p = ' ';

p = 0xfc04 ;
*p++ = x ;
p++ ;
*p = y ;

p = 0xfc02 ;

if(x == 0){
	*p++ = 0xca + y ;
	*p = 0xfb ;
	z = 0xfbca + y ;
	p = z ; *p = 0x7f ;
        }
else {
	z = 0xf80a ;
        a = 1 ;
	while(a++ < x)z += 64 ;
	z += y ;
	*p++ = z & 0x00ff ;
	*p = z >> 8 ;
	p = z ;
	*p = 0x7f ;
     }

return( 1 );

}



clrplot()
{
char *p ;
putchar( 0x0c ) ; p = 0xfbca ; *p = ' ' ;
}



plot( x, y, c )

char x,y,c ;

{

char a,b,*p ;
unsigned z ;

if(x > 15 || y > 47)return( 0 );

if(x == 0)
{ p = 0xfbca + y ; *p = c ; }
else {
	p = 0xf80a + y ;
	a = 1 ;
	while(a++ < x)p += 64 ;
	*p = c ;
     }

return( 1 );

}



txtplot( string, x, y, ropt )

char *string, x, y, ropt ;

{

char a, *p ;
unsigned z ;

if(x > 15 || y > 47)return( 0 );

if(x == 0)
{ p = 0xfbca + y ; scp( p, string ); }
else {
	p = 0xf80a + y ;
	a = 1 ;
	while(a++ < x)p += 64 ;
	scp( p, string );
     }

return( 1 );

}


setplot( base, xsize, ysize )

char *base, xsize, ysize ;
{
	return(1);
}




scp( to, from )

char *to, *from ;

{

char c ;

c = *from++ ;
while(c != 0)
{ *to++ = c ;
  c = *from++ ;
}

}


reverse( s )

char s[];

{ 

int c, i, j ;

for(i = 0, j = strlen( s ) - 1 ; i < j ; i++, j-- )
{ c = s[i] ; s[i] = s[j] ; s[j] = c ; }

}


b2asc( c )

char c ;

{

c += '0' ;
if(c > '9')c += 39 ;
return( c );

}


asc2b( c )

char c ;

{

c = toupper( c );
if(c > '9')c -= 7 ;
c -= '0' ;
return( c );

}



cbin(str)

char str[];

{

char k, base, *p ;
int i, n, sign ;

p = str ;
while(*p != 0)p++ ;
p-- ;

while(*p == ' ' || *p == 0 || *p == '\t')p-- ;
*p = tolower( *p );

switch ( *p ) {
	case 'b' : base = 2 ; break ;
	case 'o' : base = 8 ; break ;
	case 'q' : base = 8 ; break ;
	case 'd' : base = 10 ; break ;
	case 'h' : base = 16 ; break ;
	default  : base = 10 ;
	}

if(isalpha( *p )) *p = 0 ;

for(i = 0 ; str[i] == ' ' || str[i] == '\n' || str[i] == '\t' ; i++ );

sign =  1 ;
if(str[i] == '+' || str[i] == '-')
	sign = (str[i++] == '+') ? 1 : -1 ;

for(n = 0 ; str[i] >= '0' && str[i] <= b2asc( base - 1 ) ; i++ )
	n = base * n + asc2b( str[i] );

return( n * sign );

}




ctoa( n, s, base )

int n ;
char s[], base ;

{

int i, sign ;

if((sign = n) < 0)n = -n ;

i = 0 ;
do {
	s[i++] = b2asc( n % base );
   } while(( n /= base ) > 0 );

if(sign < 0)s[i++] = '-' ;

s[i] = 0 ;

reverse( s );

}





binout( word, bits )

int word, bits ;

{
 
unsigned x,y,z;

z = 0x8000 ; /* set highest bit */
while(bits-- != 0)z = z >> 1 ;

while(z != 0)
{ if((z & word) == 0)putchar( '0' );
  else putchar( '1' );
  z = z >> 1 ;
}

}





nocur()

{

char *p ;
unsigned z ;

p = 0xfc02 ;
z = *p++ ;
z = z & 0x00ff ;
z += (*p << 8);
p = z ; *p = ' ' ;

}


