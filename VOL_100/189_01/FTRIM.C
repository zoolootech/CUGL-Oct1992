#include <stdio.h>
#define MAXLINE 256

char *strcpy();

trim_fspec(t,s,n)
char *s, *t;
int n;
{
    char *p, *rindex();
    
    if (strlen(s) > n)
        if ((p = rindex(s,'\\')) != NULL)
            s = p + 1;
        else if ((p = rindex(s,'/')) != NULL)
            s = p + 1;
            
    return strcpy(t,s);
}
`�����                               