/*
** strncmp(string_1, string_2, max)
** char *string_1, *string_2;
** int max;
**
** Return values:
** A negative, 0 or positive integer indicating
** whether string_1 is lexicodraphically less than,
** equal to, or greater than string_2 (up to max
** length).
*/
#include stdio.h

strncmp(string_1, string_2, max)
char *string_1, *string_2; int max; {

  while(max--) {
   if(*string_1 != *string_2) return(*string_1 - *string_2);
   ++string_1;
   ++string_2;
   if(*string_1 == NULL) return(NULL);
  }
  return(0);
}
