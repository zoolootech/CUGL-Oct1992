/* ctype.h    */

#code

isascii(ch)
char ch;
{
        return ( (ch & 0xff) < 0x80 );
}

iscntrl(ch)
char ch;
{
        return( (ch == 0x7f) || ((ch >= 0x00) && (ch <= 0x1f)));
}

isdigit(ch)
char ch;
{
        return( (ch >= 0x30) && (ch <= 0x39 ) );
}


isgraph(ch)
char ch;
{
        return( (ch >= 0x21) && (ch <= 0x7e) );
}

islower(c)
char c;
{
        return ( (c >='a') && (c <='z') );
}

isprint(c)
char c;
{
        return ( (c >= 0x20) && (c <= 0x7e));
}

isspace(c)
char c;
{
        return ( (c == ' ') || ( (c >= 0x09) && (c <= 0x0d)) );
}

isupper(c)
char c;
{
        return ( (c >= 'A') && (c <= 'Z') );
}

isxdigit(c)
char c;
{
   int result;
   result = isdigit(c);
   result = result | ( (c >='A') && (c <='F'));
   result = result | ( (c >='a') && (c <='f'));
   return result;
}

isalpha(c)
char c;
{
        return( islower(c) || isupper(c) );
}

isalnum(c)
char c;
{
        return( islower(c) || isupper(c) || isdigit(c) );
}

ispunct(c)
char c;
{
        return( iscntrl(c) || isspace(c) );
}

toascii(c)
char c;
{
        return( c & 0x7f);
}

tolower(c)
char c;
{
        return( c | 0x20 );
}

toupper(c)
char c;
{
        return( c & 0xdf );
}

/* convert an integer into an ascii string, buffer should be 7 characters long
   and will be null terminated on return */
itoa( value, buffer )
int value;
char *buffer;
{
   int x, y;
   char sign;
   char temp[8];

   for( x = 0; x < 8; x ++ )
     temp[x] = ' ';
   sign = (value >= 0) ? '+' : '-';
   x = 7;
   do {
      temp[x] = (value % 10) + '0';
      value = value / 10;
      x--;
   } while( value > 0 );
   temp[x] = sign;
   y = 0;
   while( x <  8 )
      buffer[y++] = temp[x++];
   buffer[y] = '\0';
}

atoi( s )     /* convert s to integer */
char *s;
{
  int n, sign;
  sign=n=0;
  if(*s == '+' || *s == '-')  /* sign */
    if(*s++ == '-') sign = -sign;
  while(*s)
    n = 10 * n + *s++ - '0';
  if(sign) return(-n);
  return n;
}
