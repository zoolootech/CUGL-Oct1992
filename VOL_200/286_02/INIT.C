#include <stdio.h>
#include <gds.h>

GRADinit()
{
    int count;
    long curtime, time();

#ifdef MSC4
    extern int _amblksiz;
    _amblksiz=8000;
#endif
#ifdef JLASER
    if (initjlsr()==ERROR) {
        fprintf(stderr,"Unable to initialize JLASER\n");
        exit(1);
    }
#endif
    count=0;
    curtime=time(NULL);
    while (curtime==time(NULL));
    curtime=time(NULL);
    count++;
    while (curtime==time(NULL)) count++;
    TEN_MS=(int) (333L *count/187);   /* 333 empty loops in IBM PC takes 10ms
                                     IBM PC gives a average of 187 count */
} 

cleanup(exitcode)
{
#ifdef JLASER
    cleanjlsr();
#endif
    if (exitcode)       /* if exitcode is non-zero, error exit */
        exit(exitcode);
}

