/* case.c:  convert strings to one case */

#include <stdio.h>
#include <ctype.h>

char *strupper(s,t)
char *s, *t;
{
    register char *p;

    strcpy(s,t);
    for (p = s ; *p ; ++p)
        *p = toupper(*p);

    return s;
}

char *strlower(s,t)
char *s, *t;
{
    register char *p;

    strcpy(s,t);
    for (p = s ; *p ; ++p)
        *p = tolower(*p);

    return s;
}
�                                    '�)�+�-�/���                                                      