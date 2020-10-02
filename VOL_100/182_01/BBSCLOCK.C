/*
    bbsclock.c
*/

#include <time.h>

/* copy time into arg sting in form (HH:MM:SS xM) */
gettime(_ttime)
char    *_ttime;
    {
    long tloc ; char tchar ; int hour ;
    struct tm *localtime() , *tadr ;
    tloc = time((long *) 0) ;       /* get time to tloc */
    tadr = localtime (&tloc) ;
    tchar = 'A' ; hour = tadr->tm_hour ;
    if(hour > 12) { hour -= 12 ; tchar = 'P' ; }
    sprintf(_ttime,"%.2d:%.2d:%.2d %cM",hour,tadr->tm_min,
        tadr->tm_sec,tchar) ;
    }   

getdate(_mm,_dd,_yy,_month,_day,_year,_date,_week)
char    *_mm,       /* 2 digit */
    *_dd,       /* 2 digit */
    *_yy,       /* 2 digit */
    *_month,    /* long */
    *_day,      /* long */
    *_year,     /* long */
    *_date,     /* long month day, year */
    *_week;     /* day of week */
    {
    long tvar ;
    struct tm *localtime() , *tadr ;

    time(&tvar) ;           /* get time to tvar */
    tadr = localtime (&tvar) ;

    switch (tadr->tm_wday)
        {
        case 0:
            strcpy(_week,"Sunday ");
            break;
        case 1:
            strcpy(_week,"Monday ");
            break;
        case 2:
            strcpy(_week,"Tuesday ");
            break;
        case 3:
            strcpy(_week,"Wednesday ");
            break;
        case 4:
            strcpy(_week,"Thursday ");
            break;
        case 5:
            strcpy(_week,"Friday ");
            break;
        case 6:
            strcpy(_week,"Saturday ");
            break;
        default:
            strcpy(_week,"Unknown ");
            break;
        }

    switch (tadr->tm_mon)
        {
        case 0:
            strcpy(_month,"January ");  
            break;
        case 1:
            strcpy(_month,"February "); 
            break;
        case 2:
            strcpy(_month,"March ");    
            break;
        case 3:
            strcpy(_month,"April ");
            break;
        case 4:
            strcpy(_month,"May ");  
            break;
        case 5:
            strcpy(_month,"June ");
            break;
        case 6:
            strcpy(_month,"July "); 
            break;
        case 7:
            strcpy(_month,"August");
            break;
        case 8:
            strcpy(_month,"September ");
            break;
        case 9:
            strcpy(_month,"October ");
            break;
        case 10:
            strcpy(_month,"November ");
            break;
        case 11:
            strcpy(_month,"December ");
            break;
        default:
            strcpy(_month,"Unknown ");
            break;
        }

    sprintf(  _mm,  "%.2d",tadr->tm_mon ) ;
    sprintf(  _dd,  "%.2d",tadr->tm_mday) ;
    sprintf(  _yy,  "%.2d",tadr->tm_year) ;
    sprintf( _day,  "%.2d",tadr->tm_mday) ;
    sprintf(_year,"19%.2d",tadr->tm_year) ;

    strcpy(_date,    "");   /* clear date */
    strcat(_date,_month);   /* then concat the month,etc. */
    strcat(_date,  _day);
    strcat(_date,  ", ");
    strcat(_date, _year);
    }