/* memory.h   for 6809 card */

#code

memchr(src,chr,len)          /* Searches area pointed to by src of len */
char *src, chr;              /* bytes for chr.                         */
int len;
{
  while( --len >= 0 )
    if( *src++ == chr )
      return src-1;
    return 0;
}

memcmp(lhs, rhs, len)        /* Compares two memory areas              */
char *lhs, *rhs;
int len;
{
  while( --len >= 0 )
    if( *lhs++ != *rhs++ )
       return lhs[-1]-rhs[-1];
    return 0;
}

memcpy(dest,src,len)         /* Copies len bytes from src to dest,     */
char *dest, *src;
int len;
{
  while( --len >= 0 )
     *dest++ = *src++;
}

memmov(dst, src, len)        /*  copies len bytes from src to dst,     */
char *dst, *src;
int len;
{
  while( --len >= 0 )
  {
    *dst = *src;
    dst++;
    src++;
  }
}

memrev(dst, src, len)          /* copies len bytes from src to dst, in */
char *dst, *src;               /* reverse order                        */
int len;
{
  char *dstz;
  char *srcz;
  char t;
  dstz = dst+len;
  srcz = src+len;
  while( srcz > src)
  {
    t = *--srcz;
    *--dstz = *src++;
    *dst++ = t;
  }
}

memset(dst, chr, len)          /* fills dst with chr for len bytes     */
char *dst, chr;
int len;
{
  while( --len >= 0 )  *dst++ = chr;
}

poke(address, value)
char *address;
char value;
{
   *address = value;
}

peek(address)
char *address;
{
/*
   return ( ((*address) & 0xff00) >> 8);
*/
   return *address;
}

setvect(vector,function)
int *vector, *function;
{
   *vector = function;
}

getvect(vector)
int *vector;
{
   return (*vector);
}

