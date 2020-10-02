/* ..make a global argv[] by parsing command line.. */

#include <stdio.h>
#include <dos.h>
#include <ctype.h>

#define MAXARGS 30
#define START 1
#define NORMAL 2
#define QUOTE 3

int argc;
extern unsigned _pspbase;
char *cl, tail[0x80], **argv, *getarg(), *argbuf[MAXARGS];

makeargv()
{
    /* ..initialize argc/argv[].. */
    argc = 0;
    argv = argbuf;
    argv[0] = "";

    /* ..get command line from DOS.. */
    _copy(PTR(tail),0x80,_pspbase,0x80);
    cl = tail + 1;
    cl[*tail] = '\0';

    /* ..get parsed arguments.. */
    while (argv[++argc] = getarg()) ;
}

/* ..parse arguments.. */
char *getarg()
{
    static char arg[0x80], *q;
    int state;
    register i;

    for (i = 0, state = START ; *cl ; ++cl)
        switch(state)
        {
            case START:
                if (*cl == '\\')
                    if (*++cl)
                    {
                        arg[i++] = *cl;
                        state = NORMAL;
                    }
                    else
                        goto done;
                else if (*cl == '\"')
                    state = QUOTE;
                else if (!isspace(*cl))
                {
                    --cl;
                    state = NORMAL;
                }
                break;
            case NORMAL:
                if (isspace(*cl))
                    goto done;
                if (*cl == '\\' && *++cl == '\0')
                    goto done;
                arg[i++] = *cl;
                break;
            case QUOTE:
                if (*cl == '\"')
                {
                    ++cl;
                    goto done;
                }
                if (*cl == '\\' && *++cl == '\0')
                    goto done;
                arg[i++] = *cl;
                break;
        }

done:
    /* ..ran out?.. */
    if (i == 0)
        return NULL;

    /* ..get some memory and store this arg.. */
    arg[i] = '\0';
    q = (char *) malloc(i);
    strcpy(q,arg);
    return q;
}
me,current_target);
        filecopy(xfiles[i]->name,current_target);
    }
}


,n+1);
    