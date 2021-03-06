/*
HEADER:		;
TITLE:		Expression evaluator;
VERSION:	1.0;

DESCRIPTION:	"Auxiliary file for BC.C.  May be useful in other C programs.
		Evaluates arithmetic and logic expressions.  Takes a string
		expression as argument, and returns the value of the
		expression, an integer, 0 through 65535.";

KEYWORDS:	Arithmetic, logical, expression, evaluator, calculator;
SYSTEM:		CP/M-80;
FILENAME:	BCANLYZ.C;
WARNINGS:	"Limited to integers, 0 to 65535.";
SEE-ALSO:	BC.C, BC.DOC, BCDEF.H, BCREVIEW.DOC;
AUTHORS:	Jan Larsson;
COMPILERS:	BDS C;
*/
/*********************************************************************

Delimiters: \0, \n, \f, ,, ), ;
Operators: +, -, *, /, <, >, %, &, |, ^, mod, and, or, xor, shl, shr
Unary operators: +, -, ~, not

	       (c) 1981 Jan Larsson

**********************************************************************/

#include "bcdef.h"

unsigned anlyz( s )
char *s ;
{
	int nesting, *depth ;
	char *p ;
	int n, left, right ;
	unsigned result ;

	depth = &nesting ;
	*depth = 0 ;
	p = s ; right = left = 0 ;
	while(*p != EOL){
		if(*p == LPAREN)left++ ;
		if(*p == RPAREN)right++ ;
		p++ ;
		}
	if(right > left)
		printf("%4u: Missing left parenthesis in expression.\n",line());
	else if(right < left)
		printf("%4u: Missing right parenthesis in expression.\n",line());
	else return( anlyz2( s ) );
	return( FALSE );
}




/*
      O.k. evaluate a simple expression  <value> <operator> <value> .
      If a parenthesis occurs anlyz2 does a recursive call on itself
      to evaluate it first. No operator precedence is implemented, just
      simple left --> right evaluation. 
*/

unsigned anlyz2( s, depth )
char *s ;
int *depth ;
{
	char error, op, *skipsp(), *skipexpr() ;
	char *skiptok(), *skipop(), gettop() ;
	char unary, isunary() ;
	unsigned rval, lval, gettok() ;

	error = FALSE ;
	*depth++ ;
	s = skipsp( s );

	if(eoe(*s)){aerror(0);*depth--;return(0);}
	while(!eoe(*s)){
	    unary = isunary( s );
	    if(unary != FALSE)s = skipop( s );
	    if(*s == LPAREN){ lval = anlyz2( s+1 );
s = skipexpr( s );}
	    else { lval = gettok( s ); s = skiptok( s ); }
	    if(unary != FALSE){
		switch (unary) {
		    case PLUS     : break ;
		    case MINUS    : lval = - lval ; break ;
		    case NOT      : lval = ~ lval ; break ;
		    case SNOT     : lval = ~ lval ; break ;
		    }
		unary = FALSE ;
		}
	    if(!eoe(*s)){
	      for(;;){
		if(eoe(*s)){ aerror( 1 ) ; break ;}
		op = gettop( s ); s = skipop( s );
		if(eoe(*s)){ aerror(2); break ;}
		unary = isunary( s );
		if(unary != FALSE)s = skipop( s );
		if(*s == LPAREN){ rval = anlyz2( s+1 ); s = skipexpr( s );}
		else { rval = gettok( s ); s = skiptok( s );}
		if(unary != FALSE){
		    switch (unary) {
			case MINUS   : rval = - rval ; break ;
			case SNOT    : rval = ~ rval ; break ;
			case PLUS     : break ;
			case NOT      : rval = ~ rval ; break ;
			}
		    unary = FALSE ;
		    }
		lval = calc( lval, op, rval );
		if(eoe(*s))break;
		}
	    }
	}
	*depth-- ;
	return( lval );
}






/*
	Return true if c is an end_of_expression delmiter, else false
*/

eoe( c )
char c ;
{
	switch (c) {
		case NEWLINE   :
		case FORMFEED  :
		case EOL       :
		case RPAREN    :
		case COMMA     :
		case SEMICOLON : return( TRUE );
		default        : return( FALSE );
	}
}



/*
	Return TRUE if c is a space character (to be ignored), else FALSE
*/

spc( c )
char c ;
{
	switch (c) {
		case SPACE    :
		case COLON    :
		case TAB      :
		case SPACE    : return( TRUE );
		default       : return( FALSE );
		}
}




/*
	Return TRUE if c is a legal operator, else FALSE
*/

op( c )
char c ;
{
	switch (c) {
		case PLUS      :
		case MINUS     :
		case MULT      :
		case DIV       :
		case NOT       :
		case XOR       :
		case AND       :
		case SHL       :
		case SHR       :
		case MOD       :
		case OR        : return( TRUE );
		default        : return( FALSE );
		}
}






/*
  	Her we take a pointer as argument, increments it until it
	finds the next non-space character, then return pointer value.
*/

char *skipsp( s )
char *s ;
{
	while(spc( *s ) && !eoe(*s))s++ ;
	return( s );
}




/*
	Skiptok takes a pointer as argument, returns a pointer to the first
	position after the current token (numeric constant, symbol etc.)
*/

char *skiptok( s )
char *s ;
{
	s = skipsp( s );
	while(!spc( *s ) && !op( *s ) && !eoe( *s ) && *s != LPAREN)s++ ;
	s = skipsp( s );
	return( s );
}



/*
	Skipop takes the pointer argument, skips the current operator
	and returns pointer value.
*/

char *skipop( s )
char *s ;
{
	s = skipsp( s );
	if(op( *s ))s++ ;
	else s = skiptok( s );
	s = skipsp( s );
	return( s );
}



/*
	Skipexpr increments pointer S to the end of the current simple
	expression, if a nested expresssion turns up, a recursive call
	is made to Skipexpr.
*/

char *skipexpr( s )
char *s ;
{
	if(*s == LPAREN)s++ ;
	while(!eoe( *s )){
		if(*s == LPAREN)s = skipexpr( s );
		else s++ ;
		}
	if(*s == RPAREN)s++ ;
	s = skipsp( s );
	return( s );
}



/*
	This procedures calculates a simple expression and returns the 
	value.
*/

unsigned calc( val1, op, val2 )
unsigned val1, val2 ;
char op ;
{
	switch (op) {
	    case PLUS      : return( val1 + val2 );
	    case MINUS     : return( val1 - val2 );
	    case MULT      : return( val1 * val2 );
	    case DIV       : return( val1 / val2 );
	    case OR        : return( val1 | val2 );
	    case AND       : return( val1 & val2 );
	    case XOR       : return( val1 ^ val2 );
	    case MOD       : return( val1 % val2 );
	    case SHL       : return( val1 << val2 );
	    case SHR       : return( val1 >> val2 );
	    default        : aerror(3);return( 0 );
	    }
}





/*
	Here we return a single byte representation of the current operator,
	this is used in the switch statement in CALC.
*/

char gettop( s )
char *s ;
{
	char operator[80] ;

	s = skipsp( s );
	if(op( *s ))return( *s );
	else {
		token( s, operator );
		if(strcmp( operator, TXAND ) == 0)return( AND );
		if(strcmp( operator, TXXOR ) == 0)return( XOR );
		if(strcmp( operator, TXSHL ) == 0)return( SHL );
		if(strcmp( operator, TXSHR ) == 0)return( SHR );
		if(strcmp( operator, TXMOD ) == 0)return( MOD );
		if(strcmp( operator, TXOR  ) == 0)return( OR  );
	     }
	aerror(4);
	return( FALSE );
}




/*
	Copy next token from line S to buffer TO
*/

token( s, to )
char *s, *to ;
{
	char *p, c ;

	s = skipsp( s );
	p = s ; s++ ;
	while(!spc( *s ) && !op( *s ) && !eoe( *s ) && *s != LPAREN)s++ ;
	c = *s ; *s = EOL ; 
	strcpy( to, p );
	*s = c ;
}




pack( s )
char *s ;
{
	char c, *p ;

	for( c = 0 ; c < 7 ; c++ ){
		if(*s == EOL)break ;
		s++ ;
		}
	if(*s++ != EOL){
		p = s ;
		while(*p != EOL)p++ ;
		p-- ; *s++ = *p ;
		*s = EOL ;
		}
}




/*
	Return TRUE if next token is a unary operator, else FALSE.
*/

char isunary( s )
char *s ;
{
	char opr[80] ;

	s = skipsp( s );
	switch (*s) {
	    case MINUS   :
	    case PLUS    :
	    case NOT     : return( *s );
	    }
	token( s, opr );
	if(strcmp( opr, TXNOT ) == 0)return( SNOT );
	else return( FALSE );
}




/*
	Return the binary value of the token at S, this may be a numeric
	constant or a symbol in the symbol table.
*/

unsigned gettok( s )
char *s ;
{
	char operand[80] ;

	s = skipsp( s );
	token( s, operand );
	if(isdigit(operand[0]))return( cbin( operand ) );
	else if(operand[0] == ASCII)return( operand[1] );
	else { pack( operand );
	       return( lookup( operand ) );
             }
}



unsigned lookup( s )
char *s ;
{
	return( 0 );
}




scp( to, from )

char *to, *from ;

{

char c ;

c = *from++ ;
while(c != EOL)
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
while(*p != EOL)p++ ;
p-- ;

while(*p == ' ' || *p == EOL || *p == '\t')p-- ;
*p = tolower( *p );

switch ( *p ) {
	case BINARY  : base = 2 ; break ;
	case OCTAL   : base = 8 ; break ;
	case OQTAL   : base = 8 ; break ;
	case DECIMAL : base = 10 ; break ;
	case HEX     : base = 16 ; break ;
	default  : base = DEFAULTBASE ;
	}

if(isalpha( *p )) *p = EOL ;

for(i = 0 ; str[i] == ' ' || str[i] == '\n' || str[i] == '\t' ; i++ );

sign =  1 ;
if(str[i] == PLUS || str[i] == MINUS   )
	sign = (str[i++] == PLUS) ? 1 : -1 ;

for(n = 0 ; str[i] >= '0' && str[i] <= b2asc( base - 1 ) ; i++ )
	n = base * n + asc2b( str[i] );

if(str[i] > b2asc(base - 1))aerror(5);
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

if(sign < 0)s[i++] = MINUS    ;

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


aerror( type )
char type ;
{
	switch (type) {
	    case 0 : printf("%4u: Expecting operand/expression, returned zero.\n",
			line()); break ;
	    case 1 : printf("%4u: Missing operator in expression.\n",line()); break ;
	    case 2 : printf("%4u: Missing an operand after operator.\n",line()); break ;
	    case 3 : printf("%4u: Illegal operator in 'calc'.\n",line()); break ;
	    case 4 : printf("%4u: Illegal operator.\n",line()); break ;
	    case 5 : printf("%4u: Illegal character in numeric constant.\n",line());
			break ;
	    default : printf("%4u: Internal error in 'anlyz', type = %u.\n",line(),type);
	    }
}



line(){return(1);}
