/* text formatter transcribed from the book Software Tools */

#include <ctype.h>
#include <stdio.h>

#define PAGEWIDTH 60    /* default page width */
#define PAGELEN    66        /* default page length */
#define MAXLINE 255
#define PAGECHAR '#'    /* page number escape char */
#define HUGE 32700
#define YES    1
#define NO    0
#define OK    1

#define max(a,b) (a >= b) ? a : b
#define min(a,b) (a <= b) ? a : b
#define abs(x)   (x >= 0) ? x : -x

int    fill = YES,                /* in fill mode if YES */
       lsval = 1,                /* current line spacing */
       inval = 0,                /* current indent; >= 0 */
       rmval = PAGEWIDTH,            /* current right margin */
       tival = 0,                /* current temporary indent */
       ceval = 0,                /* number of lines to center */
       ulval = 0,                /* number of lines to underline */
       curpag = 0,                /* current output page number */
       newpag = 1,                /* next output page number */
       lineno = 0,                /* next line to be printed */
       plval = PAGELEN,        /* page length in lines */
       m1val = 2,                /* top margin, including header */
       m2val = 3,                /* margin after header */
       m3val = 2,                /* margin after last text line */
       m4val = 3,                /* bottom margin, including footer */
       bottom,                    /* last live line on page: */
                            /*  = plval - m3val - m4val */
       outp,                    /* index into outbuf */
       outw,                    /* width of text in outbuf */
       outwds,                    /* number of words in outbuf */
     dir;                    /* directions flag */

char header[MAXLINE],          /* top of page title */
     footer[MAXLINE],          /* bottom of page title */
     outbuf[MAXLINE];        /* lines to be filled go here */

main(argc,argv)
int argc;
char *argv[];
{
    register i;

    init();

    for (i = 1; i < argc; ++i)
        if (freopen(argv[i],"r",stdin) == NULL)
            fprintf(stderr,"can't open %s\n",argv[i]);
        else
            roff();
}


/* initialize header, footer, and line-count invariant */

init()
{
    bottom = plval - m3val - m4val; /* invariant */
    strcpy(header,"\n");
    strcpy(footer,"\n");
}


/* format current file */

roff()
{
    char inbuf[MAXLINE];

    while (fgets(inbuf,MAXLINE-1,stdin) != NULL)
        if (inbuf[0] == '.')
            command(inbuf);
        else
            text(inbuf);

    if (lineno > 0)
        space(HUGE);
}


/* perform formatting command */

command(buf)
char *buf;
{
    int val, spval;
    char *argtyp;

    val = getval(buf,argtyp);
    if (lookup(buf,"bp"))
    {
        if (lineno > 0)
            space(HUGE);
        set(&curpag,val,argtyp,curpag+1,-HUGE,HUGE);
        newpag = curpag;
    }
    else if (lookup(buf,"br"))
        brk();
    else if (lookup(buf,"ce"))
    {
        brk();
        set(&ceval,val,argtyp,1,0,HUGE);
    }
    else if (lookup(buf,"fi"))
    {
        brk();
        fill = YES;
    }
    else if (lookup(buf,"fo"))
         strcpy(footer,buf+3);
    else if (lookup(buf,"he"))
        strcpy(header,buf+3);
    else if (lookup(buf,"in"))
    {
        set(&inval,val,argtyp,0,0,rmval-1);
        tival = inval;
    }
    else if (lookup(buf,"ls"))
        set(&lsval,val,argtyp,1,1,HUGE);
    else if (lookup(buf,"nf"))
    {
        brk();
        fill = NO;
    }
    else if (lookup(buf,"pl"))
    {
        set(&plval,val,argtyp,PAGELEN,m1val+m2val+m3val+m4val+1,HUGE);
        bottom=plval-m3val-m4val;
    }
    else if (lookup(buf,"rm"))
        set(&rmval,val,argtyp,PAGEWIDTH,tival+1,HUGE);
    else if (lookup(buf,"sp"))
    {
        set(&spval,val,argtyp,1,0,HUGE);
        space(spval);
    }
    else if (lookup(buf,"ti"))
    {
        brk();
        set(&tival,val,argtyp,0,0,rmval);
    }
    else if (lookup(buf,"ul"))
        set(&ulval,val,argtyp,0,1,HUGE);
    else
        return;    /* ignore unknown commands */
}


/* lookup routine for commands */

lookup(buf,string)
char *buf, *string;
{
    return (buf[1] == string[0]) && (buf[2] == string[1]);
}


/* evaluate optional numeric argument */

getval(buf,argtyp)
char *buf, *argtyp;
{
    int i;

    i = 3;
    /* ..find argument.. */
    while (buf[i] == ' ' || buf[i] == '\t')
        ++i;
    *argtyp = buf[i];
    if (*argtyp == '+' || *argtyp=='-')
        i++;

    return(atoi(buf+i));
}


/* set parameter and check range */

set(param,val,argtyp,defval,minval,maxval)
int *param,val,defval,minval,maxval;
char *argtyp;
{
    if (*argtyp == '\n')
        *param = defval;
    else if (*argtyp == '+')
        *param += val;
    else if (*argtyp == '-')
        *param -= val;
    else
        *param = val;

    *param = min(*param,maxval);
    *param = max(*param,minval);
}

/* process text lines */

text(inbuf)
char *inbuf;
{
    char wrdbuf[MAXLINE];
    int i;

    if (isspace(*inbuf))
        leadbl(inbuf);    /* go left. set tival */

    if (ulval > 0)
    {
        underl(inbuf,wrdbuf,MAXLINE);
        ulval--;
    }
    if (ceval > 0)
    {
        center(inbuf);
        put(inbuf);
        ceval--;
    }
    else if (*inbuf == '\n')
        put(inbuf);
    else if (fill == NO)
        put(inbuf);
    else
    {
        i = 0;
        while (getwrd(inbuf,&i,wrdbuf) > 0)
            putwrd(wrdbuf);
    }
}


/* delete leading blanks.  Set tival */

leadbl(buf)
char *buf;
{
    register i,j;

    brk();

    /* find first non blank */
    i = 0;
    while (buf[i] == ' ')
        i++;

    if (buf[i] != '\n')
        tival += i;

    /* move line to left */
    j = 0;
    while ((buf[j++] = buf[i++]) != '\0') ;
}


/* put out line with proper spacing and indenting */

put(buf)
char *buf;
{
    register i;

    if ((lineno == 0) || (lineno > bottom))
        phead();

    i = 1;
    while (i++ <= tival)
        putchar(' ');

    tival = inval;    /* tival good for one line only */
    fputs(buf,stdout);
    skip(min(lsval-1,bottom-lineno));
    lineno += lsval;
    if (lineno > bottom)
        pfoot();
}


/* put out page header */

phead()
{
    curpag = newpag++;
    if (m1val > 0)
    {
        skip(m1val-1);
        puttl(header,curpag);
    }
    skip(m2val);
    lineno = m1val+m2val+1;
}


/* put out page footer */

pfoot()
{
    skip(m3val);
    if (m4val > 0)
    {
        puttl(footer,curpag);
        skip(m4val-1);
    }
}


/* put out title line with optional page number */

puttl(buf,pageno)
char *buf;
int pageno;
{
    register i;

    for (i = 0; buf[i] != '\n' && buf[i] != '\0'; i++)
        if (buf[i] == PAGECHAR)
            printf("%*d",1,pageno);
        else
            putchar(buf[i]);
    putchar('\n');
}


/* space n lines or to bottom of page */

space(n)
int n;
{
    brk();
    if (lineno > bottom)
        return;

    if (lineno == 0)
        phead();

    skip(min(n,bottom+1-lineno));
    lineno += n;
    if (lineno > bottom)
        pfoot();
}


/* output n blank lines */

skip(n)
register n;
{
    while ((n--) > 0)
        putchar('\n');
}


/* get non-blank word from in[i] into out.
 * increment *i.
 */

getwrd(in,ii,out)
char *in, *out;
int *ii;
{
    register i, j;

    i = *ii;
    while ((in[i]  == ' ') || (in[i] == '\t'))
        i++;

    j = 0;
    while ( (in[i] != '\0') && !isspace(in[i]) )
        out[j++] = in[i++];

    out[j] = '\0';
    *ii = i;        /* return index in ii */
    return(j);    /* return length of word */
}


/* put a word in outbuf; includes margin justification */

putwrd(wrdbuf)
char *wrdbuf;
{
    int last, llval, w, nextra;

    w = width(wrdbuf);
    /* new end of wrdbuf */
    last = strlen(wrdbuf)+outp+1;
    llval = rmval-tival;
    if ( (outp > 0) && ((outw+w>llval) || (last>=MAXLINE)) )
    {
        /* too big */
        last -= outp;    /* remember end of wrdbuf */
        nextra = llval-outw+1;
        spread(outbuf,outp-1,nextra,outwds);
        if (nextra > 0 && outwds > 1)
            outp += nextra;
        brk();    /* flush previous line */
    }
    strcpy(outbuf+outp,wrdbuf);
    outp = last;
    outbuf[outp-1] = ' ';    /* blank between words */
    outw += w+1;            /* 1 extra for blank */
    outwds++;
}


/* spread words to justify right margin */

spread(buf,outp,ne,outwds)
char *buf;
int outp, ne, outwds;
{
    register nholes, i, j, nb;

    if (ne <= 0 || outwds <= 1)
        return;

    dir = 1-dir;    /* reverse direction */
    nholes = outwds-1;
    i = outp-1;
    /* leave room for NEWLINE, EOS */
    j = min(MAXLINE-2,i+ne);
    while (i < j)
    {
        buf[j] = buf[i];
        if (buf[i] == ' ')
        {
            /* compute # of blanks */
            if (dir == 0)
                nb = (ne-1)/nholes+1;
            else
                nb = ne/nholes;
            ne -= nb;
            nholes--;
            /* put blanks in buffer */
            while (nb-- > 0)
                buf[--j] = ' ';
        }
        i--;
        j--;
    }
}


/* compute width of character string */

width(buf)
char *buf;
{
    register i, val;

    for (val = i = 0; buf[i] != '\0'; i++)
        if (buf[i] == '\b')
            val--;
        else if (buf[i] != '\n')
            val++;
    return(val);
}


/* end current filled line */

brk()
{
    if (outp > 0)
    {
        outbuf[outp-1] = '\n';
        outbuf[outp] = '\0';
        put(outbuf);
    }
    outp = outw = outwds = 0;
}


/* center a line by setting tival */

center(buf)
char *buf;
{
    tival = max((rmval+tival-width(buf))/2,0);
}


/* underline a line */

underl(buf,tbuf,size)
char *buf, *tbuf;
int size;
{
    int i, j,
        k, k1;    /* how many chars in last word */

    i = j = k = 0;
    while (j < size-2)
    {
        if (isspace(buf[i]))
        {
            k1 = k;
            while (k-- > 0)
            {
                if (j >= size-2)
                    break;
                tbuf[j++] = '\b';
            }
            k = k1;
            while (k-- > 0) {
                if (j >= size-2)
                    break;
                tbuf[j++] = '_';
            }
        }
        if (buf[i] == '\n')
            break;
        else if (j >= size-2)
            break;
        else
        {
            if (buf[i] != ' ' && buf[i] != '\t')
                k++;
            else
                k = 0;
            tbuf[j++] = buf[i++];
        }
    }
    tbuf[j++] = '\n';
    tbuf[j] = '\0';
    strcpy(buf,tbuf);
}
       while (k-- > 0) {
                if (j >= size-2)
             