

#include "meta40.h"



/*
	Here we define file i/o and the pack/unpack functions
	CR's are filtered and the ER variable is set/reset in
	the file i/o functions.

									J.L. oct. -81
*/


pack( a, b, c )
unsigned *a ;
char *b, *c ;
{
	*a = (*b << 8) | *c ;
}

unpack( a, b, c )
unsigned *a ;
char *b, *c ;
{
	*c = *a & 0x00ff ;
	*b = *a >> 8 ;
}

xopen( name, buf )
char *name, *buf ;
{
	if(fopen( name, buf ) == ERROR)er = 1 ; else er = 0 ;
	eoflag = FALSE ;
}

xclose( buf )
char *buf ;
{
	fclose( buf );
	er = 0 ;
}

gchar( buf )
char *buf ;
{
	char c ;

	c = 0x0d ; er = 0;
	while(c == 0x0d && er == 0){
		if((c = getc(buf)) == ERROR || c == CPMEOF)er = 1 ; else er = 0 ;
		if(er)eoflag = TRUE ;
		if(printflag && c != CPMEOF && !eoflag)putchar(c);
		}
	return(tolower(c));
}


pchar( c, buf )
char c, *buf ;
{  
    if(spcharflag){
   		spcharflag = FALSE ;
		if(c == 't')c = 9; else if(c == 'n')c = '\n' ;
		}
	else if(c == '\\'){spcharflag = TRUE; er = 0 ; return ;}
	if(c == '\n')if(putc(0x0d,buf) == ERROR)er = 1 ; else er = 0 ;
	if(putc(c,buf) == ERROR)er = 1 ; else er = 0 ;
}


xcreat( name, buf )	
char *name, *buf ;
{
	if(fcreat( name, buf ) == ERROR)er = 1 ; else er = 0 ;
}


newext( s, n )
char *s, *n ;
{
	char *i ;

	i = s ;
	while(*s != NULL && *s != '.')s++;
	if(*s == '.')*s = NULL ;
	strcat( i, n );
}

	
	                                                                                                               