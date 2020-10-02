#include <stdio.h>
typedef struct waldo { int i; float z; } FOOBAR;
double x;

extern something();   /* should be elided by the tool */
char *strcat();

main(argc,argv)
    int argc;
    char *argv[];
    {
    float x = 2.0;
    FOOBAR y;
    extern double waldo();

    func1(x,y);
    noargs();
    weird();
    }

char *strcmp();

double func1(x,y,z,q,r)
    float x;
    FOOBAR y,z;
    struct waldo *q,r;
    {
    }

int noargs()
    {
    }

weird()
    {
    /* no args and defaults to int type func */
    }

double *
crazy(q)
    int ***q;
    {
    /* The func type is on the previous line! */
    }

