
/*
** bg0.c -- contains main() and globals
*

   Program  : PC-Gammon IV
   Computer : 100% IBM PC compatibles
   Language : Microsoft C 4.00
            :
   Compile  : cl -Febackgmmn bg0.c bg1.c bg2.c bg3.c -link ciao
            :
   Files    : backgmmn.h    private declarations file
            : backgmmn.doc  history file
            : bg0.c         main(), global data
            : bg1.c         utilities
            : bg2.c         board & dice manager
            : bg3.c         gameplan & strategy manager
            : ciao.h        declarations for fast RAM version of...
            : ciao.lib      ibm clone i/o module
            : backgmmn.     make file, optional
            :
   Author   : David C. Oshel
            : 1219 Harding Avenue
            : Ames, Iowa 50010
*           : September 8, 1986
**
*/


#include "backgmmn.h"   /* private declarations file */



/* Note:  ALL version info is located here, from now on! 
*/

#define USERMAN "pc-gamn.doc"

char *bgversion = ".70 - June 13, 1987, by David C. Oshel";
                 /*....v....1....v....2....v....3....v...*/
                 /* just 38 characters for version info! */

char *logoname = "PC-GAMMON:  The Peelgrunt Game of Gammon IV v. 7.0";



/* Global Data 
*/ 

int level = 3;

jmp_buf mark;

char *backtalk[] = {
     "VILLIERS:  At your service!",
     "LOUISA:  Delighted!",
     "TORVE:  Is interesting line of occurrence.  Thurb!",
     "Copyright (c) 1985 by The Xochitl Sodality Wonders & Marvels Committee",
     };

int list[2][28];  /* two dice, two lists */

struct board point[28], bdsave[28]; /* 24 points, plus 2 bars, 2 homes */
struct dstruct doubles;
struct pstruct pending;

int dice[2], myscore, yrscore, player, movesleft, cantuse, myturns,
    swapped, tswap, deciding, expert, show, moremsgline, 
    tokenstyle, firstmove, helpdisabled, yrdice, lookforit, startcubevalue;

char *token1, *token2, chatter[80], buzzard[80], window[80];





commands() {

static char ch;

static char *p1 = "^ÌP^lay, ^ÌA^rrange, ^ÌN^ew, ^ÌR^everse, ^ÌS^wap, ^ÌH^%s, ^ÌQ^uit ",
            *p2 = "^ÌU^se %s dice, ^ÌO^pponent, ^ÌX^pert, ^ÌT^one, ^ÌC^ount, ^ÌZ^ero, ^ÌQ^uit ",
            *myline;


     level = MYLEVEL + 1; /* fetch copyright notice */
     setup(); 
     hint();

     while (TRUE) {
          moremsgline = FALSE;  /* show first command line on entry */
          myline = backtalk[ level ];  /* did level change? */
          msg(5,23, myline );
          firstmove = TRUE;
          newboard(); /* note, sets starting cube value to 1 */

          deciding = TRUE;
          while (deciding) {

          /* display command line */

          off_cursor(); 
          if (show) { mytotal(); yrtotal(); }
          else { gotoxy(0,3); wprintf("   "); gotoxy(0,19); wprintf("   "); }
          if (tone) beep();
          if (expert) {
               msg(5,22,"");
               msg(5,23,"Your pleasure? ");
               }
          else {
               if (moremsgline) 
               {
                    msg(5,22,"^");
                    msg(5,23,  "Select:  ");
                    wprintf( p2, (yrdice? "my": "your")); 
               }
               else 
               {
                    msg(5,22,"^");
                    msg(5,23,  "Select:  ");
                    wprintf( p1, (vid_mode == 7? "ilite": "ues"));
               }
          }

          /* get response and do it */


          ch = keyin( acg );
          ch = toupper( ch );

          if (moremsgline && ch < -60) ch += 35;  /* Shift Fn Key */

          switch ( ch ) {
               case -60:    /* F10 */
               case 'Q': {  /* quit play, exit to system */
                    return;
                    }
               case -69:    /* F1 */ 
               case 'P': {  /* play the game as board is arranged */
                    helpdisabled = FALSE;
                    if (level > MYLEVEL ) {
                         myline = chooseplayer();
                         update();
                    }
                    play(); 
                    break;
                    }
               case -68:    /* F2 */
               case 'A': {  /* arrange stones (or cheat?) */
                            /* play is suspended, so don't use the long */
                            /* messages that assist game play */
                    helpdisabled = TRUE;
                    arrange();
                    msg(5,22,"");
                    break;
                    }
               case -67:    /* F3 */
               case 'N': {  /* abandon game without quitting */
                    deciding = FALSE; player = 0;
                    break;
                    }
               case -66:    /* F4 */
               case 'R': {  /* mirror board image */
                    reverse();
                    update();
                    break;
                    }
               case -65:    /* F5 */ 
               case 'S': {  /* SWAP Command - exchange stones */
                    swaptokens();
                    update(); 
                    break;
                    }
               case -64:  /* F6, hue or hilite */ 
               case 'H':
                    {
                    /* Note:  Has the user set up a PLAIN board with no
                    ** distinctions between pieces or playing field, and 
                    ** is now going back to something more distinctive?
                    ** Clairol will set the new colors EVERYWHERE the old
                    ** colors were found, regardless, so we update the
                    ** board if an ambiguity was detected on entry.
                    ** Any problems from there on are the user's own, and
                    ** plainly obvious...!  We COULD of course just update
                    ** the board unconditionally, but that sort of spoils
                    ** the illusion.
                    */
                    clairol();
                    if (ambiguity) update();  /* neat & sweet */
                    break;
                    }
               case -24:
               case -34:    /* Shift, ALT, CTL F1 */
               case -44:
               case 'D': {  /* use my dice or your dice? */
               case 'U':
                    yrdice ^= TRUE;
                    break;
                    }
               case -23:    
               case -33:    /* Shift, ALT, CTL F2 */
               case -43:
               case 'O': {  /* change opponents and skill level */
                    myline = chooseplayer();
                    deciding = FALSE;
                    break;
                    }
               case -22:
               case -32:   /* Shift, ALT, CTL F3 */
               case -42:
               case 'X': { /* expert mode toggle(s) */ 
                    expert ^= TRUE; 
                    break;
                    }
               case -21:
               case -31:   /* Shift, ALT, CTL F4 */
               case -41:
               case 'T': { /* kill the beep */
                    tone ^= TRUE; break;
                    }
               case -20:
               case -30:    /* Shift, ALT, CTL F5 */
               case -40:
               case 'C': {  /* show mytotal, yrtotal counts */
                    show ^= TRUE;
                    break;
                    }
               case -19:
               case -29:   /* Shift, ALT, CTL F6 */
               case -39:
               case 'Z': { /* zero the score */
                    myscore = yrscore = 0; putscore();
                    break;
                    }
               case 27 : {
                    moremsgline = expert = FALSE;
                    break;
                    }
               default:  { moremsgline ^= TRUE; break; }
          }}
     }
} /* end: commands */


main( argc, argv ) int argc; char *argv[];
{
     char far *buffer;
     union REGS rg;

     if (argc > 1)        /* as in, A>backgmmn b&w */
     {
         vid_init( 2 );   /* first, foremost, and forever! */
     }
     else
         vid_init( 3 );   /* first, foremost, and forever! */

     vid[10] = vid[0];    /* set absolute normal, ^Í */
     vid[13] = vid[1];    /* set absolute bold,   ^Ì */

     buffer = savescreen( &rg );

     /* writer's block */
     {
          char ch;

          if ( (access(USERMAN,0)) != -1 )
          { 
               wputs("\n");
               wputs("*** Do you want to read the PC-Gammon User's Manual? ");
               do
               {
                    ch = keyin( noop );
                    ch = toupper(ch);
               }
               while ((ch != 'Y') && (ch != 'N')); 
               wink(ch);
               if (ch == 'Y')
                    typeafile( USERMAN, 1, 0,0, 79,24 );
          }
          else
          {
               windowbox( 25,11,55,15 );
               wprintf( " Ahem...%c\n\n",19);
               wprintf( " The PC-Gammon User's Manual,\n");
               wprintf( " \"%s,\" isn't in this\n",USERMAN);
               wprintf( " directory%c",19);
               sleep(1);
          }
     }

     commands();

     restorescreen( buffer, &rg );
     vid_exit();
     exit(0);
}

/* eof: bg0.c */

