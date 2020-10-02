/* keyin.c */

#include "ciao.h"
#include <dos.h>
#include <stdarg.h>




/*=============================*/
/* keyin( screenwait )         */
/*                             */
/* screen goes blank after 8   */
/* minutes of inactivity       */
/*=============================*/


void screenwait( void )
{
     extern int vid_attr;

     static int screentime = 0;
     char far *p;
     union REGS x;
     int h,m,s,d;

     acg();  /* bump the random number generator whilst here */

     if ( !screentime )
     {
         get_system_time( &h, &m, &s, &d );
         screentime = (h * 60) + m;
         screentime += 8;  /* eight minutes from now */
     }

     get_system_time( &h, &m, &s, &d );
     h = (h * 60) + m;
     if ( h > screentime ) /* blank screen after eight minutes of inactivity */
     {
         screentime = 0;
         p = savescreen( &x );
		 fullscreen();				/* ensure entire screen is blanked out */
         vid_attr = 7;              /* ensure screen is BLACK  */
         setcursize( 32,32 );       /* and cursor is INVISIBLE */
         gotoxy(0,0);
         rptchar(' ',2000);
         while ( !kbhit() )
         ;
         while ( kbhit() ) getch();
         restorescreen( p, &x );  /* put screen back when user hits a key */
     }
     if ( kbhit() ) screentime = 0;  /* keyin will find the waiting char */
}


/* additive congruential generator for pseudo-random numbers
** see Knuth, Art of Computer Programming, Vol. 2, p.27
*/

#define ACG_SIZE 55  /* this is a *MAGIC* number, do not modify! */

static unsigned acgnum[ACG_SIZE] = {
     44292,    60,     4947,     3972,     4489,
     1917,     23916,  7579,     3048,     56856,
     11832,    7589,   1798,     4954,     2880,
     5142,     5187,   3045,     31529,    3110,
     4333,     167,    5556,     7237,     5906,
     5419,     56632,  15833,    3760,     1081,
     1434,     80,     6212,     344,      7303,
     3044,     7675,   5420,     19457,    33434,
     2657,     700,    6777,     4436,     620,
     42129,    629,    23550,    1639,     4546,
     1220,     6469,   862,      3280,     4664
};

unsigned acg( void )
{

     static int rp1 = 54, rp2 = 23;  /* these are *MAGIC* numbers! */

     rp1++;
     rp2++;
     rp1 %= ACG_SIZE;
     rp2 %= ACG_SIZE;
     acgnum[rp1] += acgnum[rp2];
     return ( acgnum[rp1] );

}


void init_acg( void )  /* used when can't get a keypress first */
{
      unsigned h,m,s,d;
      get_system_time( &h, &m, &s, &d );
      srand( d );
      for ( h = 0; h < ACG_SIZE; h++ )
          acgnum[h] += rand();
}


/*=============================*/
/* keyin()                     */
/*                             */
/* extended ASCII chars are    */
/* returned << 8 to high byte; */
/* see "keys.h" for definition */
/*=============================*/


int keyin( void (* wait)( void ) )
{
     int ch;

     do
	 { 
     	(* wait)();   /* while waiting for input */
     } 
     while ( !kbhit() );

	 /* This appears to be the best algorithm I can find; signal is
	    still a problem, but the keyboard does not inexplicably lock up
		when using this method; extended ascii are defined in <keys.h>.
		*/
     ch = (getch() & 0xFF);
     return ((ch == 0)? ((getch() & 0xFF) << 8): ch);
}


int keyshift_status( void )
{
	union REGS x;
	x.h.ah = 2;
	int86( 22, &x,&x );
	return (x.h.al & 0xff);
}


void flush_keys( void )
{
	while (kbhit()) getch();
}




/* message() has variable args, works like wprintf with gotoxy
*/

void message( int x, int y, char *p, ...)
{
     va_list arg_ptr;
     char *buff;

     gotoxy( x,y ); wputs("^0"); clreol();  /* NOTE: Clears line! */
     buff = malloc( 128 );
     va_start(arg_ptr, p);
     vsprintf( buff, p, arg_ptr );
     va_end( arg_ptr );
     wputs( buff );
     free( buff );
}



/*
**  noop definition for #defines in debug test files
*/

void noop()
{
     ;
}



void bell() { putch('\a'); }


unsigned int sleep( int tenths )
{
     unsigned h1, h2, m1, m2, s1, s2, d1, d2;

     tenths *= 10;  /* convert to hundredths, e.g. 20 * 10 = 200 hundredths */

     get_system_time( &h1, &m1, &s1, &d1 );
     h1 = (100 * s1) + d1;  /* e.g., 5910 hundredths of a second */
     do
     {
	 	  if (kbhit()) 
          {
				return ( keyin( noop ) );  /* return user's keystroke */
		  }
		  get_system_time( &h2, &m2, &s2, &d2 );
          if (s2 < s1) s2 += 60;
          h2 = ((100 * s2) + d2) - h1; /* e.g., 6030 - 5910 = 120 hundredths */
     }
     while ( h2 < tenths );  /* e.g., while 120 < 200 */
	 return (0);
}





int getkey()
{
      int n;
	  n = keyin(screenwait);
      HIclack();
      return (n);
}



