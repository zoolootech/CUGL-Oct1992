
/*
**  bg3.c -- gameplan
*/

#include "backgmmn.h"


/*=============================================*/
/*     M Y   M O V E                           */
/*=============================================*/


static naked() {  /* am I leaving too many blots? */
static int i, clink;
     i = 24;  clink = 0;
     while (i) {
          if (point[i].stones == 1 && point[i].owner == ME) clink++;
          i--;
     }
     return (clink > 2);
} /* end: naked */


static yourfolly() { /* look for lotsa blots in your inner table */
static int i, clink;
     i = 18; clink = 0;
     while (i < 25) {
          if (point[i].owner == YU && point[i].stones == 1) 
               clink++; 
          i++;
     }
     return (clink >= 3);
} /* end: yourfolly */


static goodboard() { /* look for four made points near my inner table */
static int i, clank, clink;
     i = 9; clank = 0;
     while (i > 3) {
          if (point[i].owner == ME && point[i].stones > 1) clank++;
          i--;
     }
     if (clank > 4) return (TRUE);  /* bar is nearly blocked */

     i = 6; clank = clink = 0;
     while (i) {
          if (point[i].owner != ME) 
               ;
          else if (point[i].stones == 1) clink++; else clank++;
          i--;
     }
     return (clank > 3 && clink < 2);
} /* end: goodboard */


static scanahead( from ) int from; {
static int count;

     count = 0;
     while (--from > 0) {
          ++count;
          if ( point[ from ].owner == YU ) return (count);
     }
     return (7);

} /* end: scanahead */




/*------------------------------------------------------------*/
/* MATCHUP                                                    */
/*                                                            */
/* 2-stone functions that force the choice of the next move.  */
/* These are the HEART and SOUL of this backgammon algorithm! */
/*------------------------------------------------------------*/

static setpend( from, to ) int from, to; {
     pending.fr   = from;
     pending.to   = to;
     pending.flag = TRUE;
} /* end: setpend */


static natural(f1,t1,f2,t2) int f1,t1,f2,t2; {
     clrpend();
     if (point[t2].stones == 1 && t1 == f2) setpend(f2,t2);
     return (pending.flag);
} /* end: natural */


static matchup( test4 ) int (* test4)(); {
static int i, j, ti, tj;

     if ( pending.flag ) return (FALSE); /* this is probably redundant */

     for (i = 1; i < 26; i++) {
          ti = list[0][i];
          if ( ti == ERROR ) goto zoo;
          for (j = 1; j < 26; j++) {
               tj = list[1][j];
               if ( tj == ERROR ) goto voo;
               if ( (* test4)( i, ti, j, tj ) ) {
                    lurch( i, ti, 0);
                    return (TRUE);
               }
             voo: ;
          }
        zoo: ;
     }
     return (FALSE);

} /* end: matchup */


static matchhi( test4 ) int (* test4)(); {
static int i, j, ti, tj;

     if ( pending.flag ) return (FALSE); /* this is probably redundant */

     for (i = 1; i < 26; i++) {
          ti = list[1][i];
          if ( ti == ERROR ) goto zoo;
          for (j = 1; j < 26; j++) {
               tj = list[0][j];
               if ( tj == ERROR ) goto voo;
               if ( (* test4)( i, ti, j, tj ) ) {
                    lurch( i, ti, 1);
                    return (TRUE);
               }
             voo: ;
          }
        zoo: ;
     }
     return (FALSE);

} /* end: matchhi */



/*--------------------------------------------------------*/
/* CLOCKWISE and COUNTERCLOCK                             */
/*                                                        */
/* the rest of these are single-stone decisions based on  */
/* rules of thumb, board-scanning functions               */
/*--------------------------------------------------------*/

static plainstupid( from ) int from; { /* don't break a safe point */
     return (from < 13 && (point[from].stones == 2 && scanahead(from) < 7));
} /* end: plainstupid */


static unwise( innertablept ) int innertablept; {
     /* if it's a hit, just for god's sake don't put him on the bar!! */
     if ( innertablept < 7 ) {
          if (point[ innertablept ].owner == YU ||
              point[ YRBAR ].stones > 0) 
               return (TRUE);
     }
     return(FALSE);
} /* end: unwise */



static covermine( from, to ) int from, to; {
     if ( from < 8 ) return(FALSE);
     return ( (point[ to ].stones == 1) && (point[ to ].owner == ME) );
} /* end: covermine */


static idareyou( from, to ) int from, to; {
     if (unwise( to )) return (FALSE);
     if ( (point[ from ].stones != 2)
          && (point[ to ].stones < 2)
          && (scanahead( to ) > 6) ) return ( TRUE );
     else return (FALSE);
} /* end: idareyou */


static hitandrun( from, to ) int from, to; {
     return ( point[ to ].owner == YU );
} /* end: hitandrun */


static dbuild( from, to ) int from, to; {
static int diceleft;
     diceleft = (myturns? 2 + movesleft: movesleft);
     if (diceleft > 1) {
          /* can't possibly be only one stone on from point */
          /* or kamikaze would have covered it on last move */
          return ( point[to].stones == 0 );
     }
     return (FALSE);
} /* end: dbuild */


static kamikaze( from, to ) int from, to; { 
/* cover my distant blot, or razzle-dazzle 'em with the long doubles hit */
static int j, k, diceleft;
     
     k = from; 
     j = from - to;
     diceleft = myturns * movesleft;  /* NB: 2*2 == 2+2, "fourtunately" */
     while ( diceleft-- ) {   /* predicting where doubles land is easy! */
          k -= j;
          if (k < 1) return (FALSE); /* out of bounds */
          if ( point[ k ].stones == 0 ) continue;        /* simplify */
          if ( point[ k ].stones == 1 )   /* found my blot or yours? */
               return (TRUE);
          else if ( point[k].owner == YU )   /* found your blockade? */
               return (FALSE); 
          else continue;        /* found my safe point, so ignore it */
     }
     return (FALSE);

} /* end: kamikaze */


static hittite( from, to ) int from, to; {
     return (hitandrun(from,to) && to > 9);
} /* end: hittite */


static safehit( from, to ) int from, to; {
     return (hittite(from,to) && idareyou(from,to));
} /* end: safehit */


static foolsdemise( from, to ) int from, to; { 
        /* annihilate orphaned blots in enemy's inner, my outer table */
     return (to > 17 && point[to].owner == YU);
} /* end: foolsdemise */


static landonme( from, to ) int from, to; {
     if ( plainstupid(from) ) return (FALSE);
     if ( loneranger(from,to) ) {
          if (from < 19 && to > 6) return(TRUE);
     }
     else return ( point[ to ].owner == ME && point[to].stones < 4);
} /* end: landonme */



/* these evaluations have meaning only in the endgame */


static nobackgammon( from, to ) int from, to; { /* endgame */
     return (from > 19);
} /* end: nobackgammon */


static crosstable( from, to ) int from, to; { 
     /* always move a table ahead if possible, in the endgame */
     if (from < 7) return (FALSE);
     if (from > 18 && to <= 18) return (TRUE);
     if (from > 12 && to <= 12) return (TRUE);
     if (from >  6 && to <=  6) return (TRUE);
     return (FALSE);
} /* end: crosstable */


static fiftytworule( from, to ) int from, to; { /* endgame */
static int p;
     if (from < 7) return (FALSE);   /* not in inner table! */
     p = from % 6;
     if (p == 0) return (TRUE);            /* improve the six */
     if (p != 5) return ( (to % 6) < 3 );    /* best improve under five */
} /*  end: fiftytworule */




/* these evaluations are universally applicable, last resort moves */


static gohome( from, to ) int from, to; { /* always go home if you can */
     return (to == MYHOME);
} /* end: gohome */


static scatter( from, to ) int from, to; {  /* scatter, esp. in the endgame */
     if (plainstupid(from) || unwise(to)) return (FALSE);
     return ( point[ from ].stones > 1 && point[ to ].stones == 0 );
} /* end: scatter */


static runnerup( from, to ) int from, to; {
     if (from < 10 || from > 18) return (FALSE);
     return (TRUE);
} /* end: runnerup */


static loneranger( from, to ) int from, to; {
     return( point[ from ].stones == 1 );
} /* end: loneranger */


static run( dummy1, dummy2 ) int dummy1, dummy2; { /* MUST move something! */
     return (TRUE);
} /* end: run */





/* clockwise and counterclock make a 1-stone choice on rules of thumb */


static counterclock( test ) int (* test)(); {
static int i,j;

     for (i = 0; i < 2; i++) {
          for (j = 1; j < 25; j++) {
               if ( list[i][j] == ERROR ) continue;
               if ( (* test)( j, list[i][j] ) ) {
                    lurch( j, list[i][j], i);
                    return ( TRUE );
     }     }     }
     return (FALSE);

} /* end: counterclock */


static clockwise( test ) int (* test)(); {
static int i,j;

     for (i = 0; i < 2; i++) {
          for (j = 25; j > 0; j--) {
               if ( list[i][j] == ERROR ) continue;
               if ( (* test)( j, list[i][j] ) ) {
                    lurch( j, list[i][j], i);
                    return ( TRUE );
     }     }     }
     return (FALSE);

} /* end: clockwise */





/*-------------------------------------------*/
/*  Make Prime                               */
/*-------------------------------------------*/

static int prmchk;


static buildprime( f1,t1,f2,t2 ) int f1,t1,f2,t2; {
     clrpend();
     /* check for the doubles bug */
     if ((dice[0] == dice[1]) && (point[f1].stones < 2)) return(FALSE);

     /* look for the combination */
     if ( t1 == prmchk && t2 == prmchk) setpend(f2,t2);

     /* stick like glue to a made point, but doubles may move forward */
     if (dice[0] != dice[1]) { 
          if ((f2 < 8) && (point[f2].stones == 2)) clrpend();
          if ((f1 < 8) && (point[f1].stones == 2)) clrpend();
     }

     return(pending.flag);

} /* end: buildprime */


static makeprime() {
static int i, tab[] = { ERROR,1,2,3,20,22,24,9,4,6,8,5,7 };
     i = 12;
     while (i) {
          prmchk = tab[i]; 
          i--;
          if ( point[ prmchk ].stones > 1 ) continue;
          else if ( matchup( buildprime ) ) return(TRUE);
     }
     return(FALSE);
} /* end: makeprime */


static coverprime( from, to ) int from, to; {
     return (((to == prmchk) && 
          (point[prmchk].owner == ME)) &&
          (point[from].stones != 2));
} /* coverprime */


static cleanup() {
static int i, tab[] = { ERROR,1,2,3,20,22,24,9,4,6,8,5,7 };
     i = 12;
     while (i) {
          prmchk = tab[i]; 
          i--;
          if ( point[ prmchk ].stones != 1 ) continue;
          else if ( counterclock( coverprime ) ) return(TRUE);
     }
     return(FALSE);
} /* end: cleanup */


/*-------------------------------------*/
/* Walking Prime                       */
/*-------------------------------------*/

static swivelhips( from, to ) int from, to; {
     return ( from > prmchk );
} /* end: swivelhips */


static slink( from, to ) int from, to; {
     return( (from > prmchk) && (point[to].stones == 1) );
} /* end: slink */


static weasel() {
     if ( clockwise( slink ) )
          return(TRUE);
     if ( counterclock( swivelhips ) ) 
          return(TRUE);
     if ( clockwise( run ) )
          return(TRUE);
} /* end: weasel */

static ihaveprime( from )  int from; {
static int i, to, ez;
     ez = 0;
     for (i = 0; i < 6; i++) {
          to = from - i;
          if ((point[to].owner == ME) && (point[to].stones > 1)) ez++;
     }
     return (ez > 4);
} /* end: ihaveprime */


static walkingprime() { 
/* looks for the walking prime anywhere in the front tables */
/* then tries to bring up a runner from behind the prime,   */
/* ensuring that a back stone WILL move before a front one  */
static int i;
     i = 12;
     while (i > 5) {
          if ( ihaveprime(i) ) {
               prmchk = i;
               if ( weasel() ) return (TRUE);
          }
          i--;
     }
     return(FALSE);
} /* end: walkingprime */


/*---------- Book Moves ----------*/
/* only valid if my move is first */
/*--------------------------------*/

static zip(a,b,c,d) int a,b,c,d; {
     lurch(a,b,0);
     lurch(c,d,0);
     movesleft = 0; return( TRUE );
} /* end: zip */


static zoom( a,b,c,d,e,f,g,h ) int a,b,c,d,e,f,g,h; {
     myturns = 0; zip(a,b,c,d); zip(e,f,g,h); return( TRUE );
} /* end: zoom */


static book() {
int a,b;
     if (!firstmove) return (FALSE);
     firstmove = FALSE;
     a = min(dice[0],dice[1]);
     b = max(dice[0],dice[1]);
     switch (level) {
          case 0: { return ( book0(a,b) ); break; }
          case 1: { return ( book1(a,b) ); break; }
          case 2: { return ( book2(a,b) ); break; }
     }
}

static book0( a,b ) int a, b; { 
     switch (a) {
          case 1: { switch (b) {
                    case 1: return ( zoom(8,7,8,7,6,5,6,5) );
                    case 2: return ( zip(24,23,13,11) );
                    case 3: return ( zip(8,5,6,5) );
                    case 4: return ( zip(24,23,13,9) );
                    case 5: return ( zip(24,23,13,8) );
                    case 6: return ( zip(13,7,8,7) );
                 }
               break; }
          case 2: { switch (b) {
                    case 2: return ( zoom(6,4,6,4,13,11,13,11) );
                    case 3: return ( zip(13,11,13,10) );
                    case 4: return ( zip(8,4,6,4) );
                    case 5: return ( zip(13,8,13,11) );
                    case 6: return ( zip(24,18,13,11) );
                 }
               break; }
          case 3: { switch (b) {
                    case 3: return ( zoom(13,10,13,10,10,7,10,7) );
                    case 4: return ( zip(13,10,13,9) );
                    case 5: return ( zip(13,10,13,8) );
                    case 6: return ( zip(24,18,13,10) );
                 }
               break; }
          case 4: { switch (b) {
                    case 4: return ( zoom(13,9,13,9,24,20,24,20) );
                    case 5: return ( zip(13,8,13,9) );
                    case 6: return ( zip(24,18,18,14) );
                 }
               break; }
          case 5: { switch (b) {
                    case 5: return ( zoom(13,8,13,8,8,3,8,3) );
                    case 6: return ( zip(24,18,18,13) );
                 }
               break; }
          case 6: { return ( zoom(13,7,13,7,24,18,24,18) );
               break; }
     }

} /* end: book0 */

static book1( a,b ) int a, b; { /* mostly follows Becker */
     switch (a) {
          case 1: { switch (b) {
                    case 1: return ( zoom(8,7,8,7,6,5,6,5) );
                    case 2: return ( zip(13,11,6,5) );
                    case 3: return ( zip(8,5,6,5) );
                    case 4: return ( zip(13,9,6,5) );
                    case 5: return ( zip(13,8,6,5) );
                    case 6: return ( zip(13,7,8,7) );
                 }
               break; }
          case 2: { switch (b) {
                    case 2: return ( zoom(6,4,6,4,13,11,13,11) );
                    case 3: return ( zip(13,11,13,10) );
                    case 4: return ( zip(8,4,6,4) );
                    case 5: return ( zip(13,8,13,11) );
                    case 6: return ( zip(13,7,7,5) );
                 }
               break; }
          case 3: { switch (b) {
                    case 3: return ( zoom(13,10,13,10,8,5,8,5) );
                    case 4: return ( zip(13,10,13,9) );
                    case 5: return ( zip(13,8,8,5) );
                    case 6: return ( zip(13,7,13,10) );
                 }
               break; }
          case 4: { switch (b) {
                    case 4: return ( zoom(13,9,13,9,9,5,9,5) );
                    case 5: return ( zip(13,8,13,9) );
                    case 6: return ( zip(13,7,13,9) );
                 }
               break; }
          case 5: { switch (b) {
                    case 5: return ( zoom(13,8,13,8,8,3,8,3) );
                    case 6: return ( zip(13,7,13,8) );
                 }
               break; }
          case 6: { return ( zoom(13,7,13,7,24,18,24,18) );
               break; }
     }

} /* end: book1 */

static book2( a,b ) int a, b; { /* mostly follows Becker */
     switch (a) {
          case 1: { switch (b) {
                    case 1: return ( zoom(8,7,8,7,6,5,6,5) );
                    case 2: return ( zip(13,11,24,23) );
                    case 3: return ( zip(8,5,6,5) );
                    case 4: return ( zip(13,9,24,23) );
                    case 5: return ( zip(13,8,24,23) );
                    case 6: return ( zip(13,7,8,7) );
                 }
               break; }
          case 2: { switch (b) {
                    case 2: return ( zoom(6,4,6,4,24,23,24,23) );
                    case 3: return ( zip(13,11,13,10) );
                    case 4: return ( zip(8,4,6,4) );
                    case 5: return ( zip(13,8,13,11) );
                    case 6: return ( zip(13,7,13,11) );
                 }
               break; }
          case 3: { switch (b) {
                    case 3: return ( zoom(13,10,13,10,10,7,10,7) );
                    case 4: return ( zip(13,10,13,9) );
                    case 5: return ( zip(13,8,8,5) );
                    case 6: return ( zip(13,7,13,10) );
                 }
               break; }
          case 4: { switch (b) {
                    case 4: return ( zoom(13,9,13,9,9,5,9,5) );
                    case 5: return ( zip(13,8,13,9) );
                    case 6: return ( zip(13,7,13,9) );
                 }
               break; }
          case 5: { switch (b) {
                    case 5: return ( zoom(13,8,13,8,8,3,8,3) );
                    case 6: return ( zip(13,7,13,8) );
                 }
               break; }
          case 6: { return ( zoom(13,7,13,7,24,18,24,18) );
               break; }
     }

} /* end: book2 */


/*====== MyMove ======*/

static torve() {
     if ( makeprime() ) {  /* this will use doubles, if it can */
               return;
     }
     else if ( walkingprime() ) {  /* i have six prime points, so run!!! */
               return;
     }
     else if ( dice[0] == dice[1] ) {           /* this is too easy! */
          if ( counterclock( kamikaze ) )
               return;
          if ( counterclock( dbuild ) ) /* claim new turf */
               return;
          if ( clockwise( run ) )
               return;
     }
     else if ( cleanup() ) {  /* cover my single blot on prime points */
               return;
     }
     else if ( bearoff() ) { /* I'm ready, but you're in the back game! */
          if ( counterclock( gohome ) )
               return;
          if ( clockwise( run ) )
               return; 
     }
     else {
          if ( clockwise( hitandrun ) )
               return;
          if ( matchup( natural ) )
               return; 
          if ( clockwise( landonme ) )
               return;
          if ( counterclock( runnerup ) )
               return;
          if ( clockwise( scatter ) )
               return;
          if ( clockwise( run ) )
               return;
     }

} /* end: torve */



static villiers() {
     if ( makeprime() ) {  /* this will use doubles, if it can */
               return;
     }
     else if ( walkingprime() ) {  /* i have six prime points, so run!!! */
               return;
     }
     else if ( dice[0] == dice[1] ) {           /* this is too easy! */
          if ( counterclock( kamikaze ) )
               return;
          if ( counterclock( dbuild ) ) /* claim new turf */
               return;
          if ( clockwise( run ) )
               return;
     }
     else if ( cleanup() ) {  /* cover my single blot on prime points */
               return;
     }
     else if ( bearoff() ) { /* I'm ready, but you're in the back game! */
          if ( counterclock( gohome ) )
               return;
          if ( clockwise( run ) )
               return; 
     }
     else {                             
          if ( clockwise( foolsdemise ) )
               return;
          if ( clockwise( idareyou ) ) 
               return;
          if ( counterclock( covermine ) )
               return;
          if ( matchup( natural ) )
               return;
          if ( clockwise( landonme ) )
               return;
          if ( clockwise( runnerup ) )
               return;
          if ( clockwise( scatter ) )
               return;
          if ( clockwise( run ) )
               return;
     }

} /* end: villiers */



static louisa() {
     if ( makeprime() ) {  /* this will use doubles, if it can */
               return;
     }
     else if ( walkingprime() ) {  /* i have six prime points, so run!!! */
               return;
     }
     else if ( dice[0] == dice[1] ) {           /* this is too easy! */
          if ( counterclock( kamikaze ) )
               return;
          if ( counterclock( dbuild ) ) /* claim new turf */
               return;
          if ( clockwise( run ) )
               return;
     }
     else if ( cleanup() ) {  /* cover my single blot on prime points */
               return;
     }
     else if ( bearoff() ) { /* I'm ready, but you're in the back game! */
          if ( counterclock( gohome ) )
               return;
          if ( clockwise( run ) )
               return; 
     }
     else if ( (!naked() && goodboard()) || yourfolly() ) { 
          if ( clockwise( hitandrun ) )
               return;
          if ( matchup( natural ) )
               return; 
          if ( clockwise( landonme ) )
               return;
          if ( counterclock( runnerup ) )
               return;
          if ( clockwise( scatter ) )
               return;
          if ( clockwise( run ) )
               return;
     }
     else {                             
          if ( clockwise( foolsdemise ) )
               return;
          if ( clockwise( idareyou ) ) 
               return;
          if ( counterclock( covermine ) )
               return;
          if ( matchup( natural ) )
               return;
          if ( clockwise( landonme ) )
               return;
          if ( clockwise( runnerup ) )
               return;
          if ( clockwise( scatter ) )
               return;
          if ( clockwise( run ) )
               return;
     }
} /* end: louisa */


mymove() {
int i, d;

     if ( nomove() ) {
          if (lookforit && (dice[0] != dice[1])) {
               lookforit = FALSE;
               wprintf("\b... ");
               if (tone) { beep(); beep(); }
               switch (level) {
                    case 0: { wprintf("Blocked!");  break; }
                    case 1: { wprintf("Well, no!"); break; }
                    case 2: { wprintf("Thurb!");    break; }
               }
               sleep(1);
               restoreboard();
               update(); 

               /* put the high die in list zero */
               d = dice[0]; dice[0] = dice[1]; dice[1] = d;

               cantuse = ERROR; movesleft = 2; myturns = 1;
               switch (level) {
                    case 0: { setchat("I move");  break; }
                    case 1: { setchat("Let's try"); break; }
                    case 2: { setchat("Move is"); break; }
               }
               msg(5,22,chatter);
               prmchk = 12;
               weasel();
               /* the rules say, use both dice if you can, or  */
               /* the highest if one or the other but not both */
          }
          else {
               lookforit = TRUE;
               strcat(chatter," and now I'm blocked ");
               myturns = movesleft = 0;
     }     }
     else if ( book() ) {
          return;
     }
     else if ( pending.flag ) {
          lurch( pending.fr, pending.to, 1 );
          clrpend();
     }
     else if ( endgame() ) {
          if ( clockwise( gohome ) )
               return;
          if ( clockwise( nobackgammon ) ) /* no excuse! */
               return;
          if ( clockwise( crosstable ) )
               return;
          if ( clockwise( fiftytworule ) )
               return;
          if ( clockwise( scatter ) )
               return;
          if ( clockwise( run ) ) 
               return;
     }
     else if ( point[ MYBAR ].stones > 0 ) {         /* I'm on the bar! */
          if ( clockwise( hitandrun ) )       /* wreak havoc, please */
               return;
          if ( clockwise( run ) )     /* note: uses low die first */
               return;
     }
     else switch (level) {
          case 0: { villiers(); break; }
          case 1: { louisa(); break; }
          case 2: { torve(); break; }
     }


} /* end: mymove */


/* eof: bg3.c */

