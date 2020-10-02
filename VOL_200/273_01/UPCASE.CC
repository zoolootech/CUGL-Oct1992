#include <ctype.h>
upcase(char *str)
/* This will convert the string pointed to by *str to uppercase */
{
   while (*str) {
      if (islower(*str))
         *str = toupper(*str);
      str++; }
   return(0);
}
