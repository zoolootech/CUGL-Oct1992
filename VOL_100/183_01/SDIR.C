#include "stdio.h"
#include <search.h>
#include <string.h>

#define  FALSE     0
#define  TRUE      1

int compare();

struct  dirrect {
    char           fname[12];
    unsigned int   ftime;
    unsigned int   fdate;
    long           flen;
} dir_rec[112];
char    lookst[41];
char    volst[14];
char    ds[9],ts[9];
int     count,m;
long    total;

/*     this was the PASCAL definition of the external procedure:
procedure DIRGET(var looks:lookst; var dir_rec:dirrect;
                 var vols:volst; var count:word);extern;
*/


main( argc, argv )
int     argc;
char    **argv;
{
        if (argc == 1){         /* no arguments on command line */
            printf(" Input file specifier : ");
            getnc(40,lookst);   /* get max 40 characters */
        }
        else                    /* Copy command line argument   */
            strcpy(lookst,argv[1]);
        volst[0] = '*';         /* initialize volume string     */
        total = 0;              /*        and total bytes       */
        dirget(&count,volst,&dir_rec[0],lookst);  /* external call */
                                /* if volst has changed there's a label*/
        if (volst[0]!='*') printf(" The diskette is labeled %s\n",volst);
        putchar('\n');          /* newline in any case          */
                                /* go through list and print entries   */

        qsort(dir_rec, count, sizeof(struct dirrect), compare);

        for(m = 0 ; m < count ; ++m ){
                printnme(dir_rec[m].fname);     /* print 12 chars      */
                getdate(dir_rec[m].fdate,ds);   /* convert int to date */
                gettime(dir_rec[m].ftime,ts);   /* convert int to time */
                printf("  %6ld  %s  %s\n",dir_rec[m].flen,ds,ts);
                total += dir_rec[m].flen;       /* get running total   */
        };
        printf("\n  %ld bytes used in %d files.\n",total,count);
}

getdate(sysnumber,pstring)
unsigned int sysnumber;
char *pstring;
{
        int     day,month,year;
        day    = (sysnumber & 31);      /* day is bottom 5 bits        */
        month  = (sysnumber >> 5) & 15; /* month is middle 4 bits      */
        year   = (sysnumber >> 9) + 80; /* year is top 7 bits          */
        pstring[0] = (char)((month / 10) + '0');
        pstring[1] = (char)((month % 10) + '0');
        pstring[2] = '/';
        pstring[3] = (char)((day / 10) + '0');
        pstring[4] = (char)((day % 10) + '0');
        pstring[5] = '/';
        pstring[6] = (char)((year / 10) + '0');
        pstring[7] = (char)((year % 10) + '0');
        pstring[8] = '\0';
}

gettime(sysnumber,pstring)
unsigned int sysnumber;
char *pstring;
{
        int     hour,minute,second;
        second  = (sysnumber & 31) * 2; /* second/2 is bottom 5 bits   */
        minute  = (sysnumber >> 5) & 63;/* minute is middle 6 bits     */
        hour    = (sysnumber >> 11);    /* hour is top 5 bits          */
        pstring[0] = (char)((hour / 10) + '0');
        pstring[1] = (char)((hour % 10) + '0');
        pstring[2] = ':';
        pstring[3] = (char)((minute / 10) + '0');
        pstring[4] = (char)((minute % 10) + '0');
        pstring[5] = ':';
        pstring[6] = (char)((second / 10) + '0');
        pstring[7] = (char)((second % 10) + '0');
        pstring[8] = '\0';
}


printnme(pstring)
char    *pstring;               /* print 12 characters in name         */
{
        int     m;
        for(m=0 ; m<=11 ; putchar(pstring[m++]));
}


getnc(lent,istring)
int     lent;                   /* get maximum LENT characters and put */
char    *istring;               /*     into istring                    */
{
        int     m;
        for(m=0; (m<=lent) && ((istring[m]=getchar())!='\n'); m++);
        istring[m]='\0';
}

int compare(arg1, arg2)
struct dirrect *arg1, *arg2;
{

   return strcmp((*arg1).fname, (*arg2).fname);

}
