/* strings.h  for 6809 */

#code

strcmp(s, t)                   /* if s > t, return > 0                 */
char *s, *t;                   /* if s = t, return   0                 */
{                              /* if s < t, return < 0                 */
  while( *s == *t++)
    if( !*s++) return 0;
  return *s-t[-1];
}

strrev(str)        /* reverse characters in s */
char *str;
{
  char *head, *tail, temp;
  int count;

  head = str;
  tail = str;
  count = 0;
  while( *tail != 0x00 ) {  tail++; count++; }
  tail--;
  count = count >> 1;
  while( count-- )
  {
    temp = *head;
    *head = *tail;
    head++;
    *tail = temp;
    tail--;
  }
}

strcat(s, t)             /* concats t onto s */
char *s, *t;
{
  while( *s++ );
  for( --s; *s++ = *t++; );
  *s = 0x00;
}

strcpy(dst, src)         /* copies src to dst                    */
char *dst, *src;
{
  while( *dst++ = *src++ );
  *dst = 0x00;
}


stricmp(s1, s2)         /* compare s2 against s1, ignore case, return 0 if equal */
char *s1, *s2;
{
        char t1, t2;

        while( *s1 || *s2 )
        {
                t1 = *s1 & 0xdf;
                t2 = *s2 & 0xdf;
                if( t1 != t2 ) return 1;
                s1++; s2++;
        }
        return 0;
}

strncmp(s1, s2, count)  /* compare count characters of s1 and s2 */
char *s1, *s2;
int count;
{
  while( --count )
  {
    if( *s1++ != *s2++ )
        return 1;
  }
  return 0;
}

strncat(s1, s2, count)          /* copies s2 onto end of s1 */
char *s1, *s2;
int count;
{
        while( *s1 != 0x00 )          /* find end of s1 */
                s1++;

        while( (*s2) && (count != 0) )
        {
                *s1++ = *s2++;
                count--;
        }
        *s1 = 0x00;
}


strncpy(s2, s1, count)   /* copy count characters from s1 to s2  */
char *s1, *s2;
int count;
{
  while ( *s2 != 0x00 )
        s2++;

  while( (*s2++ = *s1++) && --count)
    ;
  *s2 = '\0';
}

strlen(s)                  /* return length of s                   */
char *s;
{
  int l;
  l = 0;
  while( *s++ ) ++l;
  return l;
}

