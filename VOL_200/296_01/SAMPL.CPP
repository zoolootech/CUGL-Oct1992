#include <stdio.h>
typedef struct waldo { int i; float z; } FOOBAR;
double x;

main(int argc,char *argv[])
    {
    float x = 2.0;
    FOOBAR y;

    func1(x,y);
    noargs();
    weird();
    }

double func1(float x,FOOBAR y,FOOBAR z,struct waldo *q,struct waldo r)
    {
    }
int noargs(void)
    {
    }
weird(void)
    {
    /* no args and defaults to int type func */
    }

double * crazy(int ***q)
    {
    /* The func type is on the previous line! */
    }

