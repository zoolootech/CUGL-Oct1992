/*
HEADER
 */
/* Reform paragraphs.  Dot commands in the text are interpreted and
 * left in the result.  Adapted from CUG .. 3/15/87 G. Osborn.
 *
 * All automatic pagination output is suppressed until a .he command
 * is processed.  The text portion of the .he command may be null.
 *
 * dot commands to not contribute to the line count if left
 * in the output.
 */

#include <ctype.h>
#include <stdio.h>
#include "ged.h"



#define PAGEWIDTH 72    /* default page width */
#define PAGELINES  66   /* default page length */
#define PAGECHAR '#'    /* page number escape char */

static int fill;                 /* in fill mode if YES */
static int lsval;                /* current line spacing */
static int inval;                /* current indent; >= 0 */
static int rmval;                /* current right margin */
static int ceval;                /* number of lines to center */
static int ulval;                /* number of lines to underline */
static int curpag;               /* current output page number */
static int newpag;               /* next output page number */
static int pageline;             /* next line to be printed */
static int plval;                /* page length in lines */
static int m1val;                /* top margin, including header */
static int m2val;                /* margin after header */
static int m3val;                /* margin after last text line */
static int m4val;                /* bottom margin, including footer */
static int bottom;               /* last live line on page: */
static int dir;                  /* directions flag */
static char *header;             /* top of page title */
static char *footer;             /* bottom of page title */
static char *outbuf;             /* lines to be filled go here */
static int inline, outline;
static int outp;
static int para1;                 /* excess paragraph indentation of 1st line of paragraph */
static int paraphase;

/* ----------------------------- */
/* keep dot commands if keep not 0.
 */
roff(keep)
int keep;
{
    register i;
    char inbuf[LLIM];
    int c, ncline, msgline, lastlsav;
    char *sp;
    char *getline();
    char temp[81];
    char buf1[LLIM];
    char buf2[LLIM];
    char buf3[LLIM];


/* the large buffers are automatic so that they do not occupy permanent
 * storage.  The pointers to them are global within this file.
 */
    header = &buf1[0];
    footer = &buf2[0];
    outbuf = &buf3[0];

/* don't retain memory of state after previous call */
    fill = YES;
    lsval = 1;
    inval = 0;
    rmval = PAGEWIDTH;
    ceval = 0;
    ulval = 0;
    curpag = 0;
    newpag = 1;
    pageline = 0;
    plval = PAGELINES;
    m1val = 2;
    m2val = 3;
    m3val = 2;
    m4val = 3;
    bottom = plval - m3val - m4val; /* invariant */
    header[0] = '\0';
    footer[0] = '\0';


/* Old lines are deleted in blocks of 100 because deltp is slow for large
 * documents.  Its execution time is independent of the number of
 * lines deleted.
 */
    inline = 1;
    lastlsav = lastl;
    msgline = 0;
    outline = lastl;
    outbuf[0] = '\0';
    outp = 0;
    paraphase = 0;    /* expecting first line of paragraph */

    while (inline <= lastlsav) {
        strcpy(inbuf,getline(inline));
        if ( (inline + msgline ) == 1 || (inline % 100) == 0) {
            sprintf(temp,"Reforming per dot commands.  Line %d", inline+msgline);
            putmess(temp);
        }
        if ((inline % 100) == 0) {
            deltp(1,100);
            inline -= 100;
            lastlsav -= 100;
            outline -= 100;
            msgline += 100;
        }
        if (inline + msgline == cline)
            ncline = outline - lastlsav;
        inline++;

/* it is necessary to flush the output buffer before dot commands
 * if they are left in the output.
 */
        if (inbuf[0] == '.' && isalpha(inbuf[1]) ) {
            if (keep) {
                putwrd("");   /* flush the output buffer */
                inject(outline++,inbuf);  /* leave the dot commands in for further editing */
            }
            command(inbuf);
        }
        else {
            proctext(inbuf);
        }
    }
    putwrd("");   /* round it out */
    space(10000);

    if (lastlsav > 0)
        deltp(1,lastlsav);  /* delete the last of the old */

    plast = -1;
    moveline (ncline-cline);  /* go back to the original point, which is not now at the same line number */
    blankedmess = YES;
    return;
}





/* perform formatting command */

command(buf)
char *buf;
{
    int val, spval;
    char *argtyp;

    val = getval(buf,argtyp);  /* get numerical argument and type */
/* begin page */
    if (lookup(buf, "bp")) {
        if (pageline > 0)
            space(10000);   /* maxint overflows */
        set(&curpag, val, argtyp, curpag+1, -MAXINT, MAXINT);
        newpag = curpag;
    }
/* break */
    else if (lookup(buf,"br"))
        putwrd("");
/* center */
    else if (lookup(buf,"ce")) {
        putwrd("");
        set(&ceval,val,argtyp,1,0,MAXINT);
    }
/* fill */
    else if (lookup(buf,"fi")) {
        putwrd("");
        fill = YES;
    }
/* footnote */
    else if (lookup(buf,"fo")) {
        putwrd("");
        strcpy(footer,buf+3);
    }
/* header */
    else if (lookup(buf,"he")) {
        putwrd("");
        strcpy(header,buf+3);
        if (header[0] == '\0')
            strcpy(header," ");   /* activate pagination */
    }
/* indent */
    else if (lookup(buf,"in")) {
        set(&inval, val, argtyp, 0, 0, rmval-1);
    }
    else if (lookup(buf,"ls"))
        set(&lsval,val,argtyp,1,1,MAXINT);

/* need at least n lines before new page */
    else if (lookup(buf, "ne")) {
        if (val + pageline - 1  > bottom)
            space(10000);
    }
    else if (lookup(buf,"nf")) {
        putwrd("");  /* runout */
        fill = NO;
    }
    else if (lookup(buf,"pl")) {
        set(&plval, val, argtyp, PAGELINES, m1val+m2val+m3val+m4val+1, MAXINT);
        bottom=plval-m3val-m4val;
    }
    else if (lookup(buf,"rm")) {
        putwrd("");
        set(&rmval, val, argtyp, PAGEWIDTH, inval+1, MAXINT);
    }
    else if (lookup(buf,"sp")) {
        set(&spval,val,argtyp,1,0,MAXINT);
        putwrd("");
        space(spval);
    }
    else if (lookup(buf,"ul"))
        set(&ulval,val,argtyp,0,1,MAXINT);
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
    if (*argtyp == '\0')
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

/* ------------- reformat text lines ------------------- */

proctext(inbuf)
char *inbuf;
{
    char wrdbuf[LLIM];
    char *nl;
    int save, inpoint;
    int i, j;


    for (inpoint = 0; inbuf[inpoint] == ' '; inpoint++)
        ;
    if (fill) {

/* determine additional indentation for first line of paragraph, if any */
        if (inbuf[0] == '\0') {
            paraphase = 0;    /* looking for 1st line of paaragraph */
        }
        else if (paraphase == 0) {
            paraphase = 1;
            if (inline <= lastl) {   /* use preceeding value if last line */
                nl = getline(inline);   /* next line */
                for (j = 0; nl[j] == ' '; j++)
                    ;
                para1 = max(inpoint - j, 0);
            }
        }
    }
    if (ulval > 0) {
        underl(inbuf, wrdbuf, LLIM);
        ulval--;
    }
    if (ceval > 0) {   /* nofill lines can be centered */
        paraphase = 0;
        putwrd("");
        save = inval;
        inval = max( inval + (rmval - inval)/2 - width(inbuf+inpoint)/2 -1, 0);
        put(inbuf+inpoint);
        inval = save;
        ceval--;
        return;
    }
    if (!fill) {
        paraphase = 0;
        putwrd("");
        put(inbuf);
    }
    else if (inbuf[0] == '\0') {
        putwrd("");
        put(inbuf);
    }
    else {
        while (getwrd(inbuf, &inpoint, wrdbuf) > 0)
            putwrd(wrdbuf);
    }
    return;
}


/* put out line with proper spacing and indenting */

put(buf)
char *buf;
{
    register i, j;
    char temp[LLIM];

    if (pageline == 0 || pageline > bottom)
        if (header[0] != '\0')
            phead();

    i = 0;
    while (i < inval)   /* indent */
        temp[i++] = ' ';

/* optional extra indentation for first line of paragraph.
 */
    if (paraphase == 1) {
        for (j = 0; j < para1; j++)
            temp[i++] = ' ';  /* i continued from preceeding loop */
        paraphase = 2;
    }

    strcpy(temp + i, buf);
    if (strlen(temp) >= LLIM-1)
        cerr(130);
    inject(outline++, temp);

    if (header[0] != '\0')
        skip(min(lsval-1, bottom-pageline));
    else
        skip(lsval-1);

    pageline += lsval;
    if (pageline > bottom && header[0] != '\0')
        pfoot();
    return;
}


/* put out page header */

phead()
{
    curpag = newpag++;
    if (m1val > 0) {
        skip(m1val-1);
        puttl(header,curpag);
    }
    skip(m2val);
    pageline = m1val + m2val + 1;
    return;
}


/* put out page footer */

pfoot()
{
    skip(m3val);
    if (m4val > 0) {
        puttl(footer,curpag);
        skip(m4val-1);
    }
}


/* put out title line with optional page number */

puttl(buf,pageno)
char *buf;
int pageno;
{
    char tbuf[LLIM];
    int i, j;

    j = 0;
    for (i = 0; buf[i] != '\0'; i++) {
        if (buf[i] == PAGECHAR) {
            j += sprintf(tbuf+j,"%*d",1,pageno);
        }
        else {
            tbuf[j++] = buf[i];
        }
    }
    tbuf[j] = '\0';
    inject(outline++, tbuf);
    return;
}


/* space n lines, or to bottom of page for large n */

space(n)
int n;
{
    putwrd("");
    if (pageline > bottom)
        return;

    if (pageline == 0)
        phead();

    skip(min(n,bottom+1-pageline));
    pageline += n;
    if (pageline > bottom)
        pfoot();
}


/* output n blank lines */

skip(n)
int n;
{
    while ((n--) > 0)
        inject(outline++, "");
    return;
}


/* get non-blank word from in[i] into out.
 * increment *i.
 */

getwrd(in, inx, out)
char *in, *out;
int *inx;
{
    register i, j;
    char c;

    i = *inx;
    while ((in[i]  == ' ') || (in[i] == '\t'))
        i++;

    j = 0;
    while ( ( c = (out[j++] = in[i++]))  > ' ' || c == '\b')
        ;
    out[--j] = '\0';
    *inx = i-1;        /* return new index  */
    return(j);    /* return length of word */
}


/* put a word in outbuf; includes margin justification */

putwrd(wrdbuf)
char *wrdbuf;
{
    static int outwds, outw;
    int sw, w, nextra, i;
    char cl;

    if (wrdbuf[0] == '\0') {      /* runout */
        if (outp > 0) {
            outbuf[outp] = '\0';
            put(outbuf);
        }
        outp = outw = outwds = 0;
        outbuf[0] = '\0';
    }
    else {
        w = width(wrdbuf);
        sw = strlen(wrdbuf);

/* two spaces between sentences */
        if ( (outp >= 3) && (outbuf[outp-1] == ' ') ) {
            cl = outbuf[outp-2];
            if ( (cl == ':') || (cl == '.') || (cl == '?') || (cl == '!') || (cl == ';') ) {
                if (isupper(wrdbuf[0])) {
                    outbuf[outp++] = ' ';
                    outw += 1;
                }
            }
        }
        i = inval;
        if (paraphase == 1)
            i += para1;
        if ( outp > 0 && (outw + w + i) > rmval || sw + outp + 1 >= LLIM ) {
/* too big */
            nextra = rmval-i-outp+1;       /* no. spaces to be inserted */
            for (i=0; i < nextra; i++)
                outbuf[outp++]=' ';
/*+++        spread(outbuf,outp-1,nextra,outwds); */
            if (nextra > 0 && outwds > 1)
/*+++     outp += nextra*/
                ;
            outbuf[outp] = '\0';
            put(outbuf);    /* write the filled line */
            outp = outw = outwds = 0;
            outbuf[0] = '\0';
        }
        strcpy(outbuf+outp, wrdbuf);
        outp += sw;
        outw += w;
        outbuf[outp++] = ' ';    /* blank between words */
        outw += 1;
        outwds++;
    }
    return;
}

/* --------- */

/* spread words to justify right margin */
/* an attempt at proportional spacing */

spread(buf, outp1, ne, outwds)
char *buf;
int outp1, ne, outwds;
{
    register nholes, i, j, nb;
    if (ne <= 0 || outwds <= 1)
        return;

    dir = 1-dir;    /* reverse direction */
    nholes = outwds-1;
    i = outp1-1;
/* leave room for NEWLINE, EOS */
    j = min(LLIM-2, i+ne);
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
    return;
}


/* compute printed width of character string */

width(buf)
char *buf;
{
    register i, val;

    for (val = i = 0; (buf[i]); i++)
        if (buf[i] == '\b')
            val--;
        else
            val++;
    return(val);
}




/* underline a line */

underl(buf,tbuf,size)
char *buf, *tbuf;
int size;
{
    int i, j,
    k, k1;    /* how many chars in last word */

    i = j = k = 0;
    while (j < size-2) {
        if (isspace(buf[i]) || buf[i] == '\0') {
            k1 = k;
            while (k-- > 0) {
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
        if (buf[i] == '\0')
            break;
        else if (j >= size-2)
            break;
        else {
            if (buf[i] != ' ' && buf[i] != '\t')
                k++;
            else
                k = 0;
            tbuf[j++] = buf[i++];
        }
    }
    tbuf[j++] = '\0';
    strcpy(buf,tbuf);
    return;
}
/* ----------------- help display ---------------- */
helpdot()
{
    cleareop(1);
    putstr("*.bp [p]  begin new page\n .br  break\n .ce [l]  center\n");
    putstr(" .fi  fill paragraphs\n*.fo [text#]  footer\n*.he [text#]  header\n");
    putstr(" .in c  indent\n*.ls l  line spacing\n*.ne l  need l for entity\n .nf   no fill\n");
    putstr(" .pl l  lines/page\n .rm c  right margin\n*.sp l  space l lines\n");
    putstr("*.ul [l]  underline\n\n");
    putstr("[] optional.  +n, -n relative to existing value. l = line, c = col, p = page\n");
    putstr(".he activates pagination.  # becomes page no.  * irreversible.\n");
    putstr("-----------------------------------------");
    topline = 19;
    calp();
    putpage();
    return;
}

