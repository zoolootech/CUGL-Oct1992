/*
**  bg1.c  - globals and utilities
*/

#include "backgmmn.h"
#include <alloc.h>
#include <stdarg.h>




/* Utilities */




unsigned acg() {  /* additive congruential generator for pseudo-random numbers */
static unsigned arg[] = {
          4292,     60,     4947,     3972,     4489,
          1917,     3916,   7579,     3048,     6856,
          1832,     7589,   1798,     4954,     2880,
          5142,     5187,   3045,     1529,     3110,
          4333,     167,    5556,     7237,     5906,
          5419,     6632,   5833,     3760,     1081,
          1434,     80,     6212,     344,      7303,
          3044,     7675,   5420,     457,      3434,
          2657,     700,    6777,     4436,     620,
          2129,     629,    3550,     1639,     4546,
          1220,     6469,   862,      3280,     4664
     };
static int rp1 = 0, rp2 = 32;

     rp1++; 
     rp2++;  
     rp1 %= 55; 
     rp2 %= 55;
     arg[rp1] += arg[rp2];
     return ( arg[rp1] );

} /* end: acg */




int contains( p, q ) char *p, *q;  /* TRUE iff some char from q is in p */
{
     char *k;
     k = strpbrk( p,q );
     return (k != NULL);
}






/*
**  special sound effects for backgammon
*/




void voice( n ) int n;
{
     switch (n)
     {
          case 0: { blopbloop(); break; }
          case 1: { bopbleet();  break; }
          case 2: { thurb();     break; }
          default:  bopbleet();
     } 
}



void beep()
{
     voice ( level );
}



/* 
** normal set of playing tokens
*/

char *get2tkn() 
{
    return ("^1()^");
}

char *get1tkn() 
{
    return ("^2[]^"); 
}

char *get3tkn() 
{
    return ("^2ßß^");
}

char *get4tkn() 
{
    return ("^1ğğ^"); 
}




/*
** hilited alternate set of playing tokens
*/

char *get6tkn() 
{
    return ("^2()^");
}

char *get5tkn() 
{
    return ("^1[]^"); 
}

char *get7tkn() 
{
    return ("^1ÜÜ^");
}

char *get8tkn() 
{
    return ("^2ğğ^"); 
}



/*
**  Gammon IV version, logo, title
*/

void msg( x,y,p ) int x,y; char *p;  /* variable args, works like wprintf */
{
     va_list arg_ptr;
     char *buff;

     gotoxy( x,y ); 
     buff = malloc( 128 );
     va_start(arg_ptr, p);
     vsprintf( buff, p, arg_ptr );
     va_end( arg_ptr );
     wputs( buff );
     free( buff );
     clr_eol();
     on_cursor(); /* expect user response */
}


void title()
{
   clr_screen();
   setwindow( 13,3,65,18 );
   clrwindow();
   gotoxy(0,15);
   wprintf("%s\n\n",logoname);
   wputs("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
   wputs("³°²°²°²°²°²°²°²°²°²°²°²°²°²°²°²°²°²°²°²°²°²°²°²°²³\n");
   wputs("ÔÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¸°³\n");
   wputs("ÚÄ¿ Û   ÛÛÛ ÛÛ Û ÛÛ  Û   Û ÛÛÛ Û  ÛÛÛ ÛÛÛ ÛÛ tmÔÍ¾\n");
   wputs("³²ÀÄÛÄÄÄÄÄÛÄÛÄÛÛÄÛÄÛÄÛÄÄÄÄÄÛÄÄÄÛÄÄÄÛÄÄÛÛÄÄÛÄÛÄÄÄÄ¿\n");
   wputs("ÔÍÍÍÛÍÍÍÛÛÛÍÛÍÍÛÍÛÛÍÍÛÍÍÍÛÍÛÍÛÍÛÛÛÍÛÍÍÛÍÍÍÛÛÛÍÍ¸²³\n");
   wputs("ÚÄ¿ ÛÛÛ ÛÛÛ Û  Û Û   ÛÛÛ Û ÛÛÛ Û Û Û  ÛÛÛ Û  Û ÔÍ¾\n");
   wputs("³°ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
   wputs("³²°²°²°²°²°²°²°²°²°²°²°²°²°²°²°²°²°²°²°²°²°²°²°²°³\n");
   wputs("ÔÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¾\n\n");
   wputs("Copyright (c) 1986 by The LampLighter Software Co.^");
   sleep(1);
   fullscreen();
}


/* 
**  bg2.c  - globals, chooseplayer()
*/




/*======================================================================

  Choose Player -- A little scenario, in which to select level of play

========================================================================*/

void dissemble() {
static int line;
static char *story[] =
{
 "You are a passenger in the \"Orion\" bound for STAR WELL, a slightly disreputable\n",
 "planetoid in the Flammarion Rift, where you have a scheduled layover of several\n",
 "hours.  You enter the Casino there, desperately bored.  Your attention is \n",
 "immediately drawn to an unusual trio.  Your instinct for good company (they \n",
 "are playing Backgammon) leads you easily into a round of introductions:\n",
 "\n",
 "^íLOUISA PARINI^ -- A young woman who gives you the uncomfortable feeling that she\n",
 "is even younger than she looks.  In fact, she is the offspring of a clan of\n",
 "noted interstellar jewel thieves and con artists, on her way to a famous\n",
 "girl's finishing school on Nashua.  She has larceny in her soul, but she is on\n",
 "holiday.  Do not underestimate her.  (But you will, of course.  You must.)\n",
 "\n",
 "^íANTHONY VILLIERS^ -- A mysterious young fop with impeccable manners, and (you\n",
 "notice) an even more impeccable dueling saber at his side.  There is something\n",
 "between Louisa and him.  His conversation is light, witty and just slightly\n",
 "cynical, but you are not wrong to conclude that this is someone you can trust.\n",
 "\n",
 "^íTORVE THE TROG^ --  This blue-eyed, golden-furred entity is a member of the most\n",
 "dangerous and unpredictable race in the galaxy (aside from humans).  You are \n",
 "amazed that this one is allowed to travel. (In fact, Torve's papers were forged\n",
 "by a member of Louisa's family and procured for him by Villiers).  Torve is\n",
 "lost in some inner rapture, emitting soft \"Thurb\"-like noises.\n"
};

     clr_screen();
     for ( line = 0; line < 22; line++ )
     {
          wprintf( story[ line ] );
     }
     wprintf ( "(Hit any key to continue)");
     keyin( acg );
     for ( line = 0; line < 25; line++ ) 
     {
          wink('\b');
     }
     clr_eol();
    
     wprintf("\nYou suggest a friendly game of backgammon, at small stakes, and your\n");
     wprintf("companions agree instantly.");
     beep();
     wprintf("  Who will be your opponent?  (L, V, or T) ");

} /* end: dissemble */



char *chooseplayer() {
int ch; char *q;

     dissemble();
loo: ch = keyin( acg );
     ch = toupper(ch);
     switch (ch) {
          case 'A':
          case 'V': { level = 0; break; }
          case 'L':
          case 'P': { level = 1; break; }
          case 'T': { level = 2; break; }
          default:  goto loo;
     }
     q = backtalk[ level ];
     clr_screen(); draw_board(q);
     return(q);

} /* end: chooseplayer */





/* eof: bg1.c */

