
/* tm.c -- emulate time and date functions found in C Food Smorgasbord
**         cf_time(p,m), cf_date(p,m) return ptr to '\0' terminator of p
**         caller must provide ptr to storage, and mode
**         these correspond to C.F.S. fns time() and date(), w/ same args
**         
**         February 20, 1988, d.c.oshel
**         for Microsoft C ver. 5.00, and PC/MS-DOS 2.x
*/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <dos.h>

#define DOS_INT 0x21
#define BCD(Q) (((Q / 10) << 4) | (Q % 10))



char *MON_table[] = {  /* 1..12 */
  NULL,
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

char *Month_table[] = { /* 1..12 */
  NULL,
  "January", "February", "March", "April", "May", "June",
  "July", "August", "September", "October", "November", "December"
};


char *Day_table[] = {  /* 1..7 */
    NULL,
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
    "Saturday"
};




void get_system_time( int *hr, int *min, int *sec, int *hnd )
{
      union REGS tm;

      tm.h.ah = 0x2C;  /* DOS get time function */

      int86( DOS_INT, &tm, &tm );

      *hr  = tm.h.ch;  /* 0..23 */
      *min = tm.h.cl;  /* 0..59 */
      *sec = tm.h.dh;  /* 0..59 */
      *hnd = tm.h.dl;  /* 0..99 */
}

void get_system_date( int *year, int *month, int *day, int *dayofweek )
{
      union REGS tm;

      tm.h.ah = 0x2A;  /* DOS get date function */

      int86( DOS_INT, &tm, &tm );

      *year      = tm.x.cx;  /* 1980..2099 */
      *month     = tm.h.dh;  /* 1..12 */
      *day       = tm.h.dl;  /* 1..31 */
      *dayofweek = tm.h.al;  /* 0..6, where 0 is Sunday, 6 is Saturday */
}


char *cf_time( char *str, int mode )  
/* fn returns pointer to \0 terminator of str */
{
      char *p, *pm;
      int hh, mm, ss, dd;

      get_system_time( &hh, &mm, &ss, &dd );

      pm = "PM";
      if ( mode > 5 )
      {
        if ( hh < 12 )
        {
             pm = "AM";
        }
        else 
        {
             hh -= 12;
        }
        if ( hh == 0 ) hh = 12;  /* noon or midnight */
      }

      switch (mode)
      {
      case 0: 
        p = str;
        *p++ = BCD( hh );
        *p++ = BCD( mm );
        *p++ = BCD( ss );
        *p++ = BCD( dd );
        goto zoo; /* exit pointing to next byte, per definition */
        break;
      case 1:
        sprintf( str, "%02d%02d%02d", hh, mm, ss);
        break;
      case 2:            
        sprintf( str, "%02d:%02d:%02d", hh, mm, ss);
        break;
      case 3:
        sprintf( str, "%02d%02d%02d%02d", hh, mm, ss, dd);
        break;
      case 4:
        sprintf( str, "%02d:%02d:%02d.%02d", hh, mm, ss, dd);
        break;
      case 5:
        sprintf( str, "%02d:%02d", hh, mm);
        break;
      case 6:
        sprintf( str, "%02d:%02d:%02d %s", hh, mm, ss, pm );
        break;
      case 7:
        sprintf( str, "%02d:%02d %s", hh, mm, pm );
        break;
      default:
        p = NULL;
        goto zoo;
      }

      p = strrchr( str, '\0' );
zoo:  return (p);
}


char *cf_date( char *str, int mode ) 
/* fn returns pointer to \0 terminator of str */
{
      char *p; 
      int yr, mo, da, dw;

      get_system_date( &yr, &mo, &da, &dw );

      switch (mode)
      {
      case 0:
        p = str;
        yr %= 100;
        *p++ = BCD( yr );
        *p++ = BCD( mo );
        *p++ = BCD( da );
        goto zoo;  /* exit pointing to next byte, per definition */
        break;
      case 1:
        sprintf( str, "%02d%02d%02d", (yr % 100), mo, da );
        break;
      case 2:
        sprintf( str, "%02d/%02d/%02d", mo, da, (yr % 100));
        break;
      case 3:
        sprintf( str, "%02d-%02d-%02d", mo, da, (yr % 100));
        break;
      case 4:
        sprintf( str, "%s %02d, %d", MON_table[ mo ], da, yr );
        break;
      case 5:
        /* Smorgasbord would have it as June 01, 1988
           but we have June 1, 1988
           */
        sprintf( str, "%s %d, %d", Month_table[ mo ], da, yr );
        break;
      case 6:
        sprintf( str, "%02d %s %02d", da, MON_table[ mo ], (yr % 100));
        break;
      case 7:
        sprintf( str, "%02d %s %d", da, MON_table[ mo ], yr );
        break;
      default:
        p = NULL;
        goto zoo;
      }

      p = strrchr( str, '\0' );
zoo:  return (p);
}


/* ----- validate the above (works like a charm!) -------------------------
main()
{
   char *p, buf[80];
   int i;

   *//* first test ... check bcd options 0 *//*
   cf_date( buf,0 );
   printf("Option 0.  (BCD) Date = (%02x %02x %02x), ", 
           (unsigned)(buf[0] & 0xFF), buf[1], buf[2]);
   cf_time( buf,0 );
   printf("Time = (%02x %02x %02x %02x).\n", 
           buf[0], buf[1], buf[2], (unsigned)(buf[3] & 0xFF));


   *//* second test ... check options 1..7 *//*
   for ( i = 1; i < 8; i++ )
   {
     p = cf_date( buf,i );
     *p++ = ',';
     *p++ = ' ';
     cf_time( p,i );
     printf("Option %d.  Today is %s.\n", i, buf);
   }

}
--------------------------------------------------------------------- */

/* return a point to system date string, like "09/10/88" */

char *sysdate( void )
{
       static char date[] = "00/00/00";
       cf_date( date, 2 );
       return (date);
}


/* return a point to system time string, like "09:10:48" (24-hr) */

char *systime( void )
{
       static char time[] = "00:00:00";
       cf_time( time, 2 );
       return (time);
}


