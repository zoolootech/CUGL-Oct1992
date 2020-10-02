#include "stdio.h"

itoa(n,s)
char s[];
int n;
{
 int i = 0, sign;

 if ( (sign = n) < 0 )  n = -n;

 do { s[i++] = n%10 + '0'; 
    } while ( (n /= 10) > 0 );

 if ( sign < 0 )  s[i++] = '-';
 s[i] = '\0';
 reverse(s);
}

ltoa(n,s)
char s[];
long n;
{
 int i = 0;
 long sign;

 if ( (sign = n) < 0 )  n = -n;

 do { s[i++] = n%10 + '0'; 
    } while ( (n /= 10) > 0 );

 if ( sign < 0 )  s[i++] = '-';
 s[i] = '\0';
 reverse(s);
}

reverse(s)
char s[];
{
 int c, i, j;

 for ( i=0, j = strlen(s)-1;   i < j;   i++, j-- )
   {  c    = s[i];
      s[i] = s[j];
      s[j] = c;
   }
}

fstrn(s1,s2)
char s1[], s2[];
{
int i, k = 0, l1, l2;
short f1 = FALSE, f2 = TRUE;

if ( (l1 = strlen(s1)) < (l2 = strlen(s2)) ) return(0);

if ( !strncmp(s1,s2,l2) ) return(1);

while ( k < l1 )
{
  for (i = k; i < l1; i++) 
    {  if ( s1[i] == s2[0] )
        {  f1 = TRUE;
           k = i;
           break;  }
    }

  if ( !f1 ) return(0);
  if ( (l1 - k) < l2 ) return(0);

  for (i = 0; i < l2; i++)
    {  if ( s1[i+k] != s2[i] ) f2 = FALSE; }

  if ( f2 ) return(1);
  
k++;
f1 = FALSE;
f2 = TRUE;
}

return(0);

}
