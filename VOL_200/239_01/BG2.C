
/*
**  bg2.c  -- initialization; major utils: play, arrange, etc.
*/

#include "backgmmn.h"


/*==========================================================================

  INITIALIZATION and NEWBOARD commands -- start of a new game, or cold

===========================================================================*/


static char *getAtkn() {
     if (tokenstyle) return( get5tkn() );
     else return( get1tkn() );
}
static char *getBtkn() {
     if (tokenstyle) return( get6tkn() );
     else return( get2tkn() );
}
static char *getCtkn() {
     if (tokenstyle) return( get7tkn() );
     else return( get3tkn() );
}
static char *getDtkn() {
     if (tokenstyle) return( get8tkn() );
     else return( get4tkn() );
}


static wipeout() {
static int i;

     player = 0;
     barcube();
     for (i = 0; i < 28; i++) {
          point[i].stones = point[i].owner = 0;
     }
     update();

} /* end: wipeout */


setup() {
static int i, j, k; char *copyright;

     myscore = yrscore = player = dice[0] = dice[1] = 0;
     tokenstyle = swapped = tswap = 
     expert = helpdisabled = yrdice = FALSE;
     show = moremsgline = tone = TRUE;

     /* please be sure that main has called vid_init()...!!! */

     off_cursor(); 
     title();

     token1 = getAtkn();
     token2 = getBtkn();
     copyright = backtalk[ MYLEVEL + 1 ];
     draw_board( copyright );

     for (i = 0; i < 28; i++) {
          point[i].stones = point[i].owner = 0;
          point[i].x = point[i].y = point[i].lastx = point[i].lasty = 0;
          point[i].cx = point[i].cy = 0;
     }

     k = 68;
     for (i = 1; i < 13; i++ ) { /* establish xy coords for the points */
          j = 25 - i;
          point[i].cx = point[j].cx = point[i].x = point[j].x = k; 
          k -= 5;
          point[i].y = 4;
          point[j].y = 18;
          point[i].cy = 2;
          point[j].cy = 20;
          if (k == 38) k -= 5; /* skip over bar */
     }

     point[MYBAR].x  = point[YRBAR].x  = 38;
     point[MYHOME].x = point[YRHOME].x = 75;

     point[MYBAR].y  = point[MYHOME].y =  5;
     point[YRBAR].y  = point[YRHOME].y = 17;

} /* end: setup */



newboard() {
static int i;  

     startcubevalue = 1;
     wipedice(); wipeout();

     putstone( MYHOME, 15, ME  );
     putstone( YRHOME, 15, YU );

     putstone( YRHOME, 13, YU );
     putstone(  1, 2, YU );

     putstone( YRHOME, 8, YU );
     putstone( 12, 5, YU );

     putstone( YRHOME, 5, YU );
     putstone( 17, 3, YU );

     putstone( YRHOME, 0, 0 ); 
     putstone( 19, 5, YU );

     putstone( MYHOME, 10, ME );
     putstone(  6, 5, ME  );

     putstone( MYHOME,  7, ME );
     putstone(  8, 3, ME  );

     putstone( MYHOME,  2, ME );
     putstone( 13, 5, ME  );

     putstone( MYHOME, 0, 0 );
     putstone( 24, 2, ME  );

} /* end: newboard */


void draw_board( c ) char *c; {
static int line,k;
static char *m = "     ", *picture[] = {
"The Peelgrunt Game of GAMMON IV%s",
/* nice ain't it? */
"…—ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕ∏o’ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕ—ª",
"∫≥ ∞∞   ≤≤   ∞∞   ≤≤   ∞∞   ≤≤   ≥∫≥  ∞∞   ≤≤   ∞∞   ≤≤   ∞∞   ≤≤  ≥∫",
"∫≥                               ≥∫≥                               ≥∫",
"∫≥ ≤≤   ∞∞   ≤≤   ∞∞   ≤≤   ∞∞   ≥∫≥  ≤≤   ∞∞   ≤≤   ∞∞   ≤≤   ∞∞  ≥∫",
"»œÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕæo‘ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕœº"
};

     clr_screen();
     off_cursor();
     msg(5,23, c );
     off_cursor();
     line = 0;
     gotoxy(0,line++); wprintf(m); wprintf(picture[0],bgversion);
     line = 3;
     gotoxy(0,line++); wprintf(m); wprintf(picture[1]);

     for (k = 0; k < 6; k++) { 
          gotoxy( 0,line++ ); wprintf(m); wprintf(picture[2]);
     }
     gotoxy(0,line++); wprintf(m); wprintf(picture[3]);
     gotoxy(0,line++); wprintf(m); wprintf(picture[3]);
     gotoxy(0,line++); wprintf(m); wprintf(picture[3]);

     for (k = 0; k < 6; k++) {
          gotoxy( 0,line++ ); wprintf(m); wprintf(picture[4]);
     }
     gotoxy(0,line); wprintf(m); wprintf(picture[5]);

} /* end: draw_board */





/*=========================================================================

  DICE Commands:  How to roll the dice

===========================================================================*/


haltgame() {
        player = -1;  /* if play resumes, ask whose roll it is */
        msg(5,22,"Ok.");
        jumpjack(); 

} /* end: haltgame */



static peek() {
     if (yrdice) return;  /* you know your own dice, probably...? */
     if (expert) msg(5,22,"");
     else msg(5,22,"The Dice will Rattle until you Roll.  Now on ");
     getdice();
     wprintf("[%d] [%d] ...",dice[0],dice[1]);
} /* end: peek */




static getonedie() {
     return ( (abs(acg()) % 6) + 1 );
} /* end: getonedie */




static fixup() {     /* ensure that the low die is in dice[0] */
int d,e;
     if (player == 0) return;  /* whofirst?  don't mess with the odds */
     d = min(dice[0],dice[1]);
     e = max(dice[0],dice[1]);
     dice[0] = d;
     dice[1] = e;
} /* end: fixup */




static getdice() {
int ch;

     /* if it's MY dice we're using, generate random dice... */
     if (!yrdice) {
          dice[0] = getonedie(); 
          acg();    /* bounce a little for luck */
          dice[1] = getonedie();
     }

     /* but if it's YOUR dice, then get roll from user's real dice... */
     else do
     {
          msg(5,23,"<> ");
          wprintf( "%s roll: ", ((player == ME)? "My": "Your"));
          if (!expert) wprintf("\b\b, using your dice: ");
          wprintf(" First? ");
    
          ch = keyin( acg );
          if (ch == 27) haltgame();
          ch -= '0';
          if (ch < 1 || ch > 6) continue;

          wink( ch + '0');
          dice[0] = ch;

          wprintf("  Second? ");
          ch =  keyin( acg );
          if (ch == 27) haltgame();
          ch -= '0';
          if (ch < 1 || ch > 6) continue;

          wink( ch + '0');
          dice[1] = ch;

          wprintf("  All Ok? ");
          ch = keyin( acg );
          ch = toupper( ch );
          if (ch == 27) haltgame();

     } while ( ch != 'Y' && ch != '\015' );

     fixup();

} /* end: getdice */




static rolldice( who ) int who; {
int waiting,ch;

     setchat("You Roll: ");
     if (!expert) {
          if (!yrdice) strcat(chatter," ^ÌP^eek,"); 
          strcat(chatter," ^ÌD^ouble, ^ÌQ^uit, or <^ÌAnyKey^> to Roll ");
     }
     if (player == YU) {  /* not executed if player == 0, i.e., whofirst */
          waiting = TRUE; 
          while (waiting) {
               msg(5,23,chatter);
               ch = keyin(acg);
               ch = toupper(ch);
               switch ( ch ) {
                    case 27 : { haltgame(); break; }
                    case -69: /* F1 */
                    case 'P': { peek(); break; }
                    case -68: /* F2 */
                    case 'D': { udouble(); break; }
                    case -60: /* F10 */
                    case 'Q': { winner(ME,topstone(YU)); break; }
                    default : waiting = FALSE;
          }     }
          off_cursor(); msg(5,22,""); msg(5,23,"");
     }
     highroller(who);

} /* end: rolldice */




static highroller( who ) int who; {  /* parameter is not redundant */
static int y = 11;
static int xme, xyu;

     /* get the values for two dice, either yours or mine */

     getdice();

     /* display the values of the dice in the board area */

     off_cursor();
     xme = 47; xyu = 12;    /* decide which half to show the values in */ 
     if (point[1].x > 40) { 
          xme = 12; 
          xyu = 47; 
     }
     if (player) {
          gotoxy(xyu,y); blanks(18); /* erase, if not whofirst */
          gotoxy(xme,y); blanks(18);
     }
     if (who == ME) {
          gotoxy(xme,y); wprintf("My");
     }
     else {
          gotoxy(xyu,y); wprintf("Your");
     }
     wprintf(" Roll> [%d] ",dice[0]);
     if (player) wprintf("[%d] ",dice[1]); /* whofirst doesn't show this */

} /* end: highroller */





/*========================================================================

  MAJOR UTILITIES -- Miscellaneous functions sans which the game will not
                     proceed so well as otherwise..........

========================================================================*/

jumpjack() 
{
     longjmp( mark, -1 );
}




static saveboard() {
int i;
     for (i = 0; i < 28; i++) {
          bdsave[i].stones = point[i].stones;
          bdsave[i].owner  = point[i].owner;
     }
} /* end: saveboard */


void restoreboard() {
int i;
     for (i = 0; i < 28; i++) {
          point[i].stones = bdsave[i].stones;
          point[i].owner  = bdsave[i].owner;
     }
} /* end: restoreboard */



swaptokens() {
char *temp;

     swapped ^= TRUE;
     if (swapped) {
          temp = token1;
          token1 = token2;
          token2 = temp;
     }
     else {
          tswap ^= TRUE;
          if (tswap) {
               token1 = getCtkn();
               token2 = getDtkn();
          }
          else {
               token1 = getAtkn();
               token2 = getBtkn();
          }
     }

} /* end: swaptokens */




static nxtyp(i) int i; {
     if (i > 9) return ( i - 1 ); else return ( i + 1 );
}




static isbar(p) int p; {
   return (( p == MYBAR ) || ( p == YRBAR ));
}



static ishome(p) int p; {
   return (( p == MYHOME ) || ( p == YRHOME ));
}


static putstone( pt, cnt, color ) int pt, cnt, color; {
int i, xp, yp, slack;
char *background, *token;


     if (cnt < 1) { /* empty point has neither stones nor owner */
            cnt   = 0; 
            color = 0;  
        }

     point[pt].stones = cnt;    /* number of stones on this point */
     point[pt].owner  = color;  /* and whose they are */

        /* stack stones 5 high in the home tray, 6 high on the points */
     if (isbar(pt) || ishome(pt)) slack = 5; else slack = 6;

        /* locate the base address of the point for animation */
     xp = point[pt].x;  
     yp = point[pt].y; 


        /* decide on the background pattern to be used for empty places */
     if (pt > 12) background = "≤≤  ";  /* was "/\\  ";  */
     else background = "≤≤  ";  /* was "\\/  "; */

     if (point[1].x < 40) {
          if ((pt % 2) == 1) background = "∞∞  ";  /* was "..  "; */
     }
     else if ((pt % 2) == 0) background = "∞∞  ";  /* was "..  "; */

     if (ishome(pt))    background = "    ";
     if (isbar(pt))     background = "≥∫≥ ";

        /* get the token pattern to be used */
     if (color == ME) {
          token = token1;  
     }
     else {
          token = token2;
     }

        /* draw the entire point with token and background patterns */
     off_cursor();

     /* first erase all blots from this point (draw the background) */
          for (i = 0; i < slack; i++) {
               gotoxy(xp,yp);
               wprintf(background);     /* string has point's width */
               point[pt].lastx = 0;  /* future, not implemented */
                        point[pt].lasty = 0;
               yp = nxtyp(yp);
          }

     /* now draw all the blots there are on this point onto the point */
          for (i = 0; i < cnt; i++) {
               xp = point[pt].x + (i / slack);
               if ((i % slack) == 0) yp = point[pt].y;
               gotoxy(xp, yp);
               wprintf(token);
               point[pt].lastx = xp;  /* future, not implemented */
               point[pt].lasty = yp;
               yp = nxtyp(yp);
          }

} /* end: putstone */



static hitblot( from, color ) int from, color; {
static int barpt, addone;

     if (tone) beep();
     barpt = whosebar( color );
     putstone(from, 0, 0);
     addone = point[barpt].stones + 1;
     putstone( barpt, addone, color );

} /* end: hitblot */



static movestone( from, to) int from, to; {
static int opponent, subone, addone;


     opponent = other( player );
     if (point[to].owner == opponent) hitblot(to, opponent);

     subone = point[from].stones - 1;
     addone = point[to].stones + 1;

     putstone(from, subone, player);
     putstone(to, addone, player);

     --movesleft;
     checkwin();  /* never but NEVER let a win go unnoticed! */

} /* end: movestone */



/*========================================================================

  CUBE Commands -- commands related to the cube, doubling, etc.

=========================================================================*/

static notyrcube() {
     gotoxy(75,19); blanks(5);
     gotoxy(75,20); blanks(5);
} /* end: notyrcube */


static notmycube() {
     gotoxy(75,2); blanks(5);
     gotoxy(75,3); blanks(5);
} /* end: notmycube */


static barcube() {
        /* startcubevalue is normally 1, but it may have doubled */
        /* if the opening rolloff for first turn came up doubles */
     doubles.cube = startcubevalue;
     doubles.whosecube = 0;
     notmycube(); notyrcube();
        gotoxy(37,11);
        if (startcubevalue == 1) wprintf("[BAR]");
     else if (doubles.cube < 16) wprintf("[ %d ]",doubles.cube);
     else wprintf("[%03d]",doubles.cube); 
} /* end: barcube */



static notbarcube() {
     gotoxy(37,11); wprintf("^2 BAR ^");
} /* end: notbarcube */


static mycube(value) int value; {
     notbarcube(); gotoxy(75,2); wprintf("^3CUBE^");
     gotoxy(75,3); 
     sprintf(buzzard,"[%d]",value); 
     wprintf("%-5s",buzzard);
     doubles.whosecube = ME;

} /* end: mycube */



static yrcube(value) int value; {
     notbarcube; gotoxy(75,19); wprintf("^3CUBE^");
     gotoxy(75,20); sprintf(buzzard,"[%d]",value); 
     wprintf("%-5s",buzzard);
     doubles.whosecube = YU;

} /* end: yrcube */


static idouble() {
static int ch;

     if (doubles.whosecube == YU || doubles.cube >= 256) {
             ;  /* not mine, or not polite, so do nothing */
        }
     else {
        notbarcube();
        if (tone) beep();
        msg(5,22,"^ÌI double.  Will you accept the cube ");
        wprintf("at %d points? ^",doubles.cube * 2);
        do
        {
             ch = keyin( acg );
             ch = toupper( ch );
             if (ch == 27) winner(ME,0);   /* Ha, fooled you! */
        }
        while ( ch != 'Y' && ch != 'N' );  /* only Y or N this time */
        
        if (ch == 'Y') {
             notmycube();
             doubles.cube *= 2;
             yrcube(doubles.cube);
             off_cursor();
             }
        else winner(ME,0);
        }

} /* end: idouble() */




static backgame() {
int max, barred, count, i;
     if (topstone(YU) < 12 && topstone(ME) > 18) {
          if (mytotal() < yrtotal() + 4) return (TRUE);
          max = barred = count = 0;
          i = 24;
          while (i > 18) {
               if (point[i].owner == ME) {
                    max = i;
                    if (point[i].stones > 1) barred++;
                    count += point[i].stones;
               }
               i--;
          }
          return ((max < topstone(YU) + 1) && (barred > 1 && count < 7));
     }
     else return ( mytotal() < yrtotal() + 24 );

} /* end: backgame */



static cubeval() {
int ineed, yuneed, yrtop, mytop;

     if (endgame()) {

          /* calculate the number of dice that are required to end */
          /* the game, with appropriate fudge factors for position */

          yrtop = topstone(YU);
          mytop = topstone(ME);

          ineed = 15 - point[MYHOME].stones;
          yuneed = 15 - point[YRHOME].stones;

          if (yrtop < 4 && yuneed < 3) return (FALSE); /* obvious */

          /* topstones still running?  use a different method */
          if (mytop > 6 || yrtop > 6) {
               ineed  = 2 * (mytotal() / 8) + 1; /* number of dice */
               yuneed = 2 * (yrtotal() / 8) + 1;
          }

          /* count the stones on point 6 twice, they're losers */
          yuneed += point[6].stones;
          ineed  += point[6].stones;

          /* you doubled, so you have the roll */
          yuneed -= 2;

          /* odd number left? */
          if (ineed  % 2) ineed++;
          if (yuneed % 2) yuneed++;

          /* is the one point empty? */
          if (ineed  > 4 && mytop > 3 && point[24].stones == 0) ineed++;
          if (yuneed > 4 && yrtop > 3 && point[ 1].stones == 0) yuneed++;

          if (mytop < yrtop && ineed < yuneed) return (TRUE);
          if (yrtop < 5 && yuneed < ineed) return (FALSE);
          return ( yuneed >= ineed );
     }
     else return ( backgame() );

} /* end: cubeval */



static testcube() {

     if (cubeval()) {
          msg(5,22,"I accept the cube.");
          notyrcube();
          doubles.cube *= 2;
          mycube(doubles.cube);
     }
     else winner(YU,0);

} /* end: testcube */



static udouble() {

     if (doubles.whosecube == ME) {
          if (tone) beep();
          msg(5,22,"It's MY cube, dummy!"); 
     }
     else testcube();

} /* end: udouble */







/*======================================================================

  ARRANGE Command:  Move stones around in the playing area.  Play will
                    commence with this final arrangement.  Notice, this
                    command allows for cheating because the line input
                    function traps Ctrl-C and executes jumpjack().  Player
                    returns to the command line with the game frozen, may
                    re-arrange as desired, then resume play.  

========================================================================*/


arrange() {  /* update() must follow arrangement */

      if ( setjmp( mark ) != 0 ) update(); 
      else

      {
           moveabout();  /* infinite loop */
      }

} /* end: arrange */



static moveabout() { 

     player = -1;  /* flag to ask who moves first */
     if (!expert)
        msg(5,22,"Type BAR or HOME, or the Number of a Point.");

     while ( TRUE ) { /* exit via jumpjack() by typing ESCape   */
           mytotal();
           yrtotal();
           revise();
        }

}  /* end: moveabout */



static getpt(b,h) int b,h; {
static char ans[6], *p; int x, d1, d2, look, try;
static char *cusswords = "?!LP .,-@#$%^&*()CDFGIJKNPQSTUVWXYZ"; 
                         /* not HOME or BAR */

     on_cursor();
     getms( ans, 5, acg, haltgame);
     p = ans;
     while (*p) { *p = toupper(*p); p++; }
     off_cursor();

     if (!helpdisabled && (contains(ans,cusswords) == TRUE)) {
          x = ERROR;
          msg(5,23,"");
          if (point[ whosebar(player) ].stones > 0) {
               wprintf("You're on the Bar, so let's move that one!  ^3BAR^ ");
               x = b;
          }
          else {
               wprintf("Are you ");
               if (cantuse != 0 && cantuse != 1) wprintf("REALLY ");
               wprintf("blocked?  Try moving From ");
               look = 24;
               while (look > 0) {
                    try = list[0][look];
                    if (try == ERROR) try = list[1][look];
                    if ( try != ERROR ) {
                         wprintf("%d To ",25-look);
                         if (try == YRHOME) wprintf("HOME");
                         else wprintf("%d",25-try);
                         look = 0;
                    }
                    look--;
               }
               sleep(40);
          }
     }
     else if (contains(ans,"B") == TRUE) x = b;
     else if (contains(ans,"H") == TRUE) x = h;
     else {
          x = atoi( ans );
          if (x < 1 || x > 24) x = ERROR;
          else x = 25 - x;  /* translate human to computer view */
     }
     return (x);

} /* end: getpt */


static whoseit(p,a,b) char *p; int a,b; {
int ch;

     if (point[a].stones > 0 && point[b].stones > 0) {
          msg(5,22,"Whose "); wprintf(p); wprintf("?  1 = "); 
          wprintf(token1);
          wprintf("  2 = "); wprintf(token2); wprintf(" ");
     loo: ch = keyin( acg );
          if (!(ch == '1' || ch == '2')) goto loo; 

          msg(5,22,"From "); 
          if (ch == '1') { ch = a; wprintf(token1); }
          else { wprintf(token2); ch = b; }
          wprintf("'s "); wprintf(p);
          return ( ch );
     }
     else if (point[a].stones > 0) return (a);
     else if (point[b].stones > 0) return (b);
     else return (ERROR);

} /* end: whoseit */



static revise() {
     do {
          setchat("Move a Stone From? ");
          msg(5,23,chatter);
     }
     while ( !delightful() );

} /* end: revise */


static delightful() {
static int from, to, fcnt, tcnt, fcolor, tcolor; 

     from = getpt(MYBAR,MYHOME);

     sprintf(buzzard,"%d",abs(25 - from));

     if (from == MYBAR) {
          from = whoseit("bar",MYBAR,YRBAR);
          strcpy(buzzard,"BAR");
     }

     if (from == MYHOME) {
          from = whoseit("home",MYHOME,YRHOME);
          strcpy(buzzard,"HOME");
     }

     if (from == ERROR) {
          msg(5,22,"Hit ESC to quit.");
          return( FALSE );
     }

        /*-----------------------------------------------------*/
        /* establish the color of the stones on the from point */
        /*-----------------------------------------------------*/
     fcolor = point[from].owner;

        /*--------------------------------------------------*/
        /* establish the number of stones on the from point */
        /*--------------------------------------------------*/
        fcnt = point[from].stones;

     if (fcnt == 0) {
          msg(5,22,"What's the point?"); 
                return(FALSE);
     }

     strcat(chatter,buzzard);  /* this avoids use of save_cursor() */
     strcat(chatter," To? ");
     msg(5,23,chatter);

     to = getpt(MYBAR,MYHOME);

     sprintf(buzzard,"%d",abs(25 - to));


     if (to == MYBAR) {
          if (fcolor == YU) to = YRBAR;
          strcpy(buzzard,"BAR");
     }

     if (to == MYHOME) {
          if (fcolor == YU) to = YRHOME;
          strcpy(buzzard,"HOME");
     }
     
     if (to == ERROR) {  
          msg(5,22,"Hit ESC to quit.");
                return(FALSE);
     }

     if (from == to) {
          msg(5,22,"Quite easily done!"); 
                return(FALSE);
     }

     strcat(chatter,buzzard);

        /*---------------------------------------------------*/
        /* establish the color of the stones on the to point */
        /*---------------------------------------------------*/
     tcolor = point[to].owner;

        /*------------------------------------------------*/
        /* establish the number of stones on the to point */
        /*------------------------------------------------*/
     tcnt = point[to].stones;

     if (fcolor == tcolor || tcolor == 0) {
          msg(5,23,chatter);
                --fcnt;
                ++tcnt;
          putstone(from, fcnt, fcolor);  /* one less */
          putstone(to,   tcnt, fcolor);  /* one more */
          msg(5,22,"");
          return (TRUE);
     }
     else {
          msg(5,22,"Evict the other stone");
          if (tcnt > 1) wprintf("s");
          wprintf(" first!");
          return (FALSE);
     }

} /* end: delightful */





/*=======================================================================

   PLAY Command - this is the command that initiates the 2-player game

=========================================================================*/

play() {

     if ( setjmp( mark ) != 0 ) return;
     else
     {
         whofirst(); 
         taketurns();  /* infinite loop here */
     }

} /* end: play */



static whofirst() {
int ch, myval, yrval;

     if (yrdice || (player < 0)) {        /* board has been re-arranged */
          msg(5,23,"Is it my "); wprintf(token1);
          wprintf(" turn or your "); wprintf(token2);
          wprintf(" turn? ");
     loo: ch = keyin( acg );
          ch = toupper(ch);
          if (!(ch == 'M' || ch == 'Y')) goto loo;
          if (ch == 'M') player = YU;  /* player says Me, of course! */
          else player = ME;
          rolldice(player);
     }

     else if (player == 0) {  /* fresh start, roll the dice */
          barcube();
     zoo: msg(5,22,"Tossing for first turn...");
          wipedice();
          rolldice(ME);
          myval = dice[0];
          rolldice(YU);  
          yrval = dice[0];
          if (myval == yrval) {
               if (tone) beep();
               off_cursor();
               gotoxy(37,11); wprintf("[^4   ^]"); sleep(3);
               startcubevalue *= 2;
               if (startcubevalue > 8) startcubevalue = 8;
               doubles.cube = startcubevalue;
               gotoxy(37,11);
               if (doubles.cube < 16) wprintf("[ %d ]",doubles.cube);
               else wprintf("[%03d]",doubles.cube); 
               if (startcubevalue < 9) {
                   msg(5,23,"Double the cube!");
                   sleep(20);
                   }
               goto zoo;
          }
          else if (myval < yrval) player = YU;
          else player = ME;
          dice[0] = max(myval,yrval); dice[1] = min(myval,yrval);
     }
     /* otherwise, continue with last dice rolled as play is resumed */

} /* end: whofirst */




static getmove() {  
static int i, ch, temp, happy;

     cantuse = ERROR;  /* important for human player in tellmove */
     movesleft = 2;
     if (dice[0] == dice[1]) movesleft += 2;
     temp = movesleft;          

     getlist(); saveboard(); lookforit = TRUE;

     if ( nomove() ) {
          msg(5,22,"All "); 
          if (player == ME) wprintf("my"); else wprintf("your");
          wprintf(" moves are blocked!");
          if (player == YU) sleep(20);
          return;
     }


     if (player == ME) {
          /* handle doubles as two consecutive, independent moves */
          setchat("I move"); 
          msg(5,22,chatter);
          if (movesleft == 4) myturns = 2; else myturns = 1;
          clrpend();
          while (myturns > 0) {
               cantuse = ERROR;
               movesleft = 2;
               while (movesleft > 0) { getlist(); mymove(); }
               myturns--;
          }
          msg(5,22,chatter);
          wputs("\b.");
     }
     else {  /* allow the human to take back a bad board position */
          happy = FALSE;
          while (!happy) {
               while (movesleft > 0) { getlist(); yrmove(); }
               msg(5,23,"All ok?  Y/N ");
               do {
                    ch = keyin( acg );
                    ch = toupper(ch);
               } while (ch != 'N' && (ch != 'Y' && ch != '\015'));
               if ( ch == 'N' ) {
                    msg(5,23,"Ok, as it was...");
                    restoreboard();
                    update();
                    movesleft = temp;
                    cantuse = ERROR;
               }
               else happy = TRUE;
               msg(5,22,"");
          }
     }

} /* end: getmove */



static taketurns() {   /* COMPUTER CUBE STRATEGY IS HERE */

     while (TRUE) {  /* NO EXIT!  Only a Win or player ESC can exit */

          getmove();
          player = other(player);
          if ( player == ME ) {
               if ( endgame() ) {
                    if (topstone(ME) < 6 && cubeval()) idouble();
                    else if ( mytotal() < (yrtotal() - 8) ) {
                         idouble();
                    }
               }
               else if ( bearoff() ) idouble();
          }
          rolldice (player); 
     }

} /* end: taketurns */


endgame() { /* Is no strategy required from here on in? */
     return ( (25 - topstone(YU)) > topstone(ME) );
} /* end: endgame */



bearoff() {
     return (topstone(ME) < 7);
} /* end: bearoff */



void clrpend() {
     pending.flag = FALSE;
} /* end: clrpend */



/*=============================================*/
/*     Y O U R   M O V E                       */
/*=============================================*/

static tellmove() {  /* show what the player's current dice are */
int k,n;

     n = movesleft;
     msg(5,22,"You "); 
     if (!expert) { wprintf(token2); wprintf(" "); }
     wprintf("have "); 
     while (n--) {
          if (dice[0] == dice[1]) k = dice[0];  /* doubles? */

          else if (cantuse == 1) k = dice[0];
          else if (cantuse == 0) k = dice[1];
          else if (n == 1) k = dice[1];     /* 2 of 2? */
          else k = dice[0];          /* 1 of 2? */

          wprintf("[%d] ",k);
     }
     wprintf("left");
     if (expert) wprintf("."); else wprintf(", moving from high to low.");

} /* end: tellmove */



static yrmove() {

     if ( nomove() ) {
          msg(5,22,"You have no more moves in this line of play.");
          movesleft = 0;
          return;
     }
     else {
loo:          tellmove();
          if (!getyrmove()) {
                     hint(); 
                     goto loo;
                }
          msg(5,22,"");
     }

} /* end: yrmove */



hint() {
    if (!expert) { 
        msg(5,22,"HELP, BAR and HOME are useful words in this game.");
        sleep(10);
    }
} /* end: hint */


nomove() {
int i,j;
     for (i = 0; i < 2; i++)
          for (j = 0; j < 28; j++)
               if (list[i][j] != ERROR) return (FALSE);
     return (TRUE);
} /* end: nomove */



static getyrmove() {  
int fpoint, tpoint;


     firstmove = FALSE; /* I got it, I got it! */

     msg(5,23,"Move from? ");
     fpoint = getpt(YRBAR,YRHOME);
     if ( fpoint == ERROR ||
          (list[0][fpoint] == ERROR && list[1][fpoint] == ERROR)) {
               return (FALSE);
     }
     wprintf(" To? ");
     tpoint = getpt(YRBAR,YRHOME);
     if ( tpoint == ERROR || 
          (list[0][fpoint] != tpoint && list[1][fpoint] != tpoint)) {
               return (FALSE);
     }

     HIclack(); movestone( fpoint, tpoint ); LOclack();
     if (movesleft < 2) {
          if (list[0][fpoint] == tpoint) cantuse = 0;
          else cantuse = 1;
     }
     return (TRUE);

} /* end: getyrmove */



/*====== Functions That Make The Selected Move ======*/


void setchat( p ) char *p; {
     strcpy(chatter,p);
} /* end: setchat */



static putdice( f,t) int f,t; {
static char q[15];
     if (t == MYHOME) sprintf(q," %d to Home,",25 - f);
     else if (f == MYBAR) sprintf(q," Bar to %d,",25 - t);
     else sprintf(q," %d to %d,",25 - f, 25 - t);
     strcat(chatter,q);
     msg(5,22,chatter);  /* avoid using save_cursor() */
} /* end: putdice */



lurch( f, t, zlist ) int f, t, zlist; {
     LOclack();
     movestone( f, t );   /* move the stone */
     HIclack();
     putdice(f,t);           /* tell user, the action is a bit fast */
     if (movesleft < 2) cantuse = zlist;
} /* end: lurch */





/*==========================================================================

  GETLIST -- Find the possible moves for any particular throw of the dice

===========================================================================*/


static checkpips( whichlist, ptimon, pips, tops ) 
int whichlist, ptimon, pips, tops; {
static int j,k;

     if ( !ptimon ) {
          j = whosebar(player);      /* 0 if you, 25 if me  */
          k = abs(j - pips);
     }
     else if (player == ME) {
          j = ptimon;
          k = j - pips;
          if (k < 1) k = MYHOME;
     }
     else {
          j = 25 - ptimon;
          k = j + pips;
          if (k > 24) k = YRHOME;
     }

     if (point[j].stones > 0 && point[j].owner == player) {

          /* no move to a blocked point */
          if (point[k].owner != player && point[k].stones > 1)
               return;

          /* no move home if i can't bear off yet */
          if (k == whosehome(player) && cantbearoff(j,pips,tops)) 
               return;

          /* no other move is allowed if i'm on the bar */
          if (tops == 25 && j != whosebar(player))
               return;

          /* the move is legal (but maybe not optimal) */
          list [whichlist] [j] = k;
     }

} /* end: checkpips */



static build( whichlist, pips ) int whichlist, pips; {
int i, tops;

     if (whichlist == cantuse) return;
     tops = topstone(player);
     for (i = 0; i < 26; i++) checkpips( whichlist, i, pips, tops );

} /* end: build */



static getlist() {  /* find all legal moves using these dice */
static int i, j;

     for (i = 0; i < 2; i++)  /* initialize the lists */
          for (j = 0; j < 28; j++)
               list [i] [j] = ERROR;

     build( 0, dice[0] ); /* usually the low die  */
     build( 1, dice[1] ); /* usually the high die */

} /* end: getlist */



/*==========================================================================

  EVALUATE UTILITIES -- Functions for legal and/or best play, telling who's
               who, who's ahead, who won, etc. etc.

============================================================================*/



mytotal() {
int i, cnt;

     cnt = 0;
     for (i = 0; i < 26; i++) {
          if (point[i].owner == ME) cnt += point[i].stones * i;
     }
     if (show) {
     gotoxy(0,3); wprintf("^0%03d",cnt); 
     }
     return (cnt);

} /* end: mytotal */


yrtotal() {
int i, cnt;

     cnt = 0;
     for (i = 0; i < 26; i++) {
          if (point[i].owner == YU) cnt += point[i].stones * (25 - i);
     }
     if (show) {
        gotoxy(0,19); wprintf("^0%03d",cnt); 
     }
     return (cnt);

} /* end: yrtotal */



topstone( who )  int who; {
static int i,j;

     if (point[ whosebar( who ) ].stones > 0) i = 25;
     else {
          i = 24;
          while (i > 0) {
               if (who == ME) j = i; else j = 25 - i;
               if (point[j].stones > 0 &&
                   point[j].owner == who) return (i);
               --i;
          }
     }
     return (i);  /* return normalized value, 1 - 25, 0 is home */

}  /* end: topstone */



static cantbearoff( mypt, pips, tops ) int mypt, pips, tops; {

     /* My destination is Home, but can I do it??? */
     if (mypt > 6) mypt = 25 - mypt;  /* normalize inner table */

     /* I can't bear off if there's anybody still not in my inner table */
     if (tops > 6) return (TRUE);

     /* If I'm the highest blot in my own table, I CAN bear off */
     if (tops == mypt) return (FALSE);

     /* If I'm NOT high, I have to have an exact roll to get away with it */
     return ( (mypt != pips) );

} /* end: cantbearoff */



static whosebar( who ) int who; {
     return ( (who == ME? MYBAR: YRBAR) );
}



static whosehome( who ) int who; {
     return ( (who == YU? YRHOME: MYHOME) );
}



static checkwin() {  
     if (mytotal() == 0) winner(ME, topstone(YU));
     if (yrtotal() == 0) winner(YU, topstone(ME));
} /* end: checkwin */


static other( color ) int color; {

     if (color == ME) return (YU); else return (ME);

} /* end: otherplayer */




static winner( who, high ) int who, high; {
int gammon; char ch;

     if (tone) beep();
     if (point[ whosehome( other(who) )].stones > 0) gammon = 1;
     else gammon = 2;               /* nothing off is a gammon! */
     if (high == 0) gammon = 1;          /* someone doubled */
     else if (high > 18) gammon = 3;      /* backgammon! */


     msg(5,22,"");
     if (who == ME) wprintf("I"); else wprintf("You"); wprintf(" win"); 
     switch (gammon) {
          case 1: { wprintf("!"); break; }
          case 2: { wprintf(" a Gammon!"); break; }
          case 3: { wprintf(" a Backgammon!"); break; }
     }
     gammon *= doubles.cube;
     switch (who) {
          case ME: { myscore += gammon; break; }
          case YU: { yrscore += gammon; break; }
     }
     putscore();
     msg(5,23,"Hit ESC to resume play"); 
     loo:  ch = keyin( acg ); if (ch != 27) goto loo;
     player = 0;
     reverse(); deciding = FALSE; jumpjack();

} /* end: winner */


putscore() {
     gotoxy(39,0); clreol(); gotoxy(53,0);
     wprintf("SCORE:  You %d, Me %d",yrscore,myscore);
} /* end: putscore */




/*========================================================================

  REVERSE Command - allow player to take the opponent's viewpoint of the
                    board layout, mirror the board.  Returns board layout
                    to the arranged position as seen from opposite side.

==========================================================================*/

reverse() {
int cnt1, cnt2, cnt3, cnt4;

     off_cursor();
     cnt1 = point[MYHOME].stones;  /* save counts for erase */
     cnt2 = point[YRHOME].stones;
     cnt3 = point[MYBAR].stones;
     cnt4 = point[YRBAR].stones;

     putstone( MYHOME, 0, 0);  /* erase old trays before update */
     putstone( YRHOME, 0, 0);

     halfswap(1); halfswap(13);

     gotoxy(0,11);  wprintf("    ");  /* erase HOME message */
     gotoxy(75,11); wprintf("    ");
     if (point[1].x < 40) {
          point[MYHOME].x = point[YRHOME].x = 0;
     }
     else {
          point[MYHOME].x = point[YRHOME].x = 75;
     }

     point[MYHOME].owner  = point[MYBAR].owner = ME; /* restore counts */
     point[YRHOME].owner  = point[YRBAR].owner = YU;
     point[MYHOME].stones = cnt1;
     point[YRHOME].stones = cnt2;
     point[MYBAR].stones  = cnt3;
     point[YRBAR].stones  = cnt4;

} /* end: reverse */


static
char *charm[] = {
"Gammon IV concept & text graphic rendition by David C. Oshel",
"MidSummer's Day, June 21, 1985",
"To Whomever Destroys This Notice -- Nothing Shall Happen, Forever"
};


static halfswap( n ) int n; {
static int i, j, k, o;

     o = n + 6;
     for (i = n; i < o; i++) {
          j = ((o * 2) - 1) - i;
          k = point[i].x;
          point[i].cx = point[i].x = point[j].x;
          point[j].cx = point[j].x = k;
     }

} /* end: halfswap */






static wipedice() {
static char *s = "                  ";

     off_cursor();
     gotoxy(47,11); wprintf(s);      /* erase dice roll messages */
     gotoxy(12,11); wprintf(s);     

} /* end: wipedice */



update() {
int i, x, c;

     for (i = 1; i < 25; i++) {
          gotoxy(point[i].cx,point[i].cy);
          wprintf("%2d",25 - i);
     }
     if (doubles.whosecube == YU) yrcube(doubles.cube); 
     else if (doubles.whosecube == ME) mycube(doubles.cube);
     else barcube();
     for (i = 0; i < 28; i++) {
          x = point[i].stones;
          c = point[i].owner;
          putstone(i,x,c);
     }
     if (point[1].x < 40) x = 0; else x = 75;
     gotoxy(x,11); wprintf("HOME");

     putscore(); mytotal(); yrtotal();

} /* end: update */


/* eof: bg2.c */


