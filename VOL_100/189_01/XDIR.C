 /* xdir.c:     fill structures with extended file info */

#include <stdio.h>
#include <ctype.h>
#include <dos.h>
#include <xdir.h>

static struct DTA dta;
static struct reg r;

int ffirst(), fnext(), xdir(), getdrv();
char *strlower(), *rindex(), *index();

int ffirst(s,attr)
char *s;
unsigned attr;
{
    /* ..Set DTA.. */
    r.r_ax = SETDTA;
    ptoreg(dsreg,r.r_dx,r.r_ds,&dta);
    intcall(&r,&r,DOSINT);

    /* ..Get first match.. */
    r.r_ax = NFFIRST;
    r.r_cx = attr;
    ptoreg(dsreg,r.r_dx,r.r_ds,s);
    intcall(&r,&r,DOSINT);

    return (r.r_flags & F_CF) == 0;
}

int fnext()
{
    /* ..Find next match.. */
    r.r_ax = NFNEXT;
    intcall(&r,&r,DOSINT);

    return (r.r_flags & F_CF) == 0;
}

int xdir(argc,argv,xfiles,maxf,find_attr)
int argc, maxf, find_attr;
char *argv[];
struct file_info *xfiles[];
{
    register i, j;
    int status;
    char path[64];

    i = 0;      /* ..File count.. */
    for (j = 1; j < argc; ++j)
    {
        getpath(argv[j],path);
        status = ffirst(argv[j],find_attr);
        /* ..Non-existent files are ignored.. */
        for ( ; status && i < maxf; status = fnext(), ++i)
        {
            xfiles[i] = (struct file_info *) malloc(sizeof(struct file_info));
            if (xfiles[i] == NULL)
            {
                fputs("not enough memory\n",stderr);
                exit(1);
            }
            xfiles[i]->name = (char *) malloc(strlen(dta.name)+
                                              strlen(path)+1);
            if (xfiles[i]->name == NULL)
            {
                fputs("not enough memory\n",stderr);
                exit(1);
            }
            strcpy(xfiles[i]->name,path);
            strcat(xfiles[i]->name,strlower(dta.name,dta.name));
            xfiles[i]->attr = dta.attribute;
            xfiles[i]->size = dta.size;
            xfiles[i]->mon = dta.month;
            xfiles[i]->day = dta.day;
            xfiles[i]->year = dta.year + 80;
            xfiles[i]->hour = dta.hour;
            xfiles[i]->min = dta.min;        
        }
    }
    return i;
}

getpath(f,p)
register char *f, *p;
{
    register char *ptr;
    int drive;
    char dir[64];

    /* ..Get drive.. */
    if ((ptr = index(f,':')) != NULL)
    {
        drive = tolower(*f) - 'a';  /* ..Drive ID.. */
        *p++ = tolower(*f++);       /* ..Drive letter.. */
        *p++ = *f++;                /* ':' */
    }
    else
    {
        drive = getdrv();           /* ..Default drive ID.. */
        *p++ = 'a' + drive;
        *p++ = ':';
    }


    /* ..Get directory.. */
    if (*f != '\\' && *f != '/')
    {
        /* ..Get default directory (starting from root).. */
        getdir(drive+1,dir);
        *p++ = '\\';
        ptr = dir;
        while (*ptr)
            *p++ = tolower(*ptr++);
	if (strlen(dir) > 0)
            *p++ = '\\';
    }

    /* ..Copy specified directory.. */
    if ((ptr = rindex(f,'\\')) != NULL)
        while (f <= ptr)
            *p++ = tolower(*f++);
    else if ((ptr = rindex(f,'/')) != NULL)
        while (f <= ptr)
            *p++ = tolower(*f++);

    *p = '\0';
}

int getdrv()
{
    r.r_ax = GETDISK;
    intcall(&r,&r,DOSINT);
    return r.r_ax & 0x00ff;
}

getdir(drive,dir)
int drive;
char *dir;
{
    r.r_ax = GETCDIR;
    r.r_dx = drive;
    ptoreg(dsreg,r.r_si,r.r_ds,dir);
    intcall(&r,&r,DOSINT);
}
    B~L n"  FCOMP   C             ��}.   XDIR    C             �}3 �  