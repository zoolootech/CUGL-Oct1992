 

#include "ciao.h"
#include "keys.h"

/*
**  clairol()  - modify screen colors or monochrome attributes "on the fly"
**               popout window, updates previous screen on exit
**               to reflect the newly selected values
**
**               returns on Control-C, resets to SIG_DFL, restores screen
**	4/23/88, dco -- removed signal code, doesn't belong in service routine!
*/

     /* Note:  Users who select a PLAIN screen create ambiguity */
     /*        between the various message levels.  The NEXT    */
     /*        time clairol() is called, the ambiguous colors   */
     /*        default to the lower message level.  For example */
     /*        if Normal and Bold have the same value on entry, */
     /*        and the user changes the value of EITHER, BOTH   */
     /*        Normal and Bold bytes in the screen buffer will  */
     /*        be reset to the NEW NORMAL value.  This is       */
     /*        because INFORMATION HAS BEEN LOST concerning the */
     /*        meaning of the message levels, in the step where */
     /*        user selected a PLAIN screen.                    */
     /*                                                         */
     /*        You should test the ambiguity flag on exit from  */
     /*        clairol(), and take whatever corrective action   */
     /*        is needed thereafter.                            */
     /*                                                         */
     /*        In cases where your program cannot tolerate the  */
     /*        ambiguity a user MIGHT create (the possibility   */
     /*        is there!), you should use video registers 4     */
     /*        through 15, which clairol() does not alter, and  */
     /*        to which users have no access; or else simply    */
     /*        don't call clairol()...!  Clairol() gives the    */
     /*        user permission to change things you may not     */
     /*        want to be changed, in other words.              */


#define MONOCHROME 7

int ambiguity = 0;

static void test_ambiguity()
{
     static int i, j;

     for ( ambiguity = i = 0; i < 3; i++ )
     {
         for ( j = i + 1; j < 4; j++ )
         {
             ambiguity |= vid[i] == vid[j]; 
         }
     }
}



/*
static jmp_buf remark;
*/
static int ch, tx, ty, which, eaSave, edSave, icon, flipflop,
           nrmSave, bldSave, empSave, ntcSave;


/*
static void ControlC()
{
      longjmp(remark,-1); 
}
*/


static void valSave()
{
      test_ambiguity();         /* detect plain screen */

      nrmSave = vid[0] & 0xFF;  /* normal       Vid_init() defines only these */
      bldSave = vid[1] & 0xFF;  /* bold         four values for color text.   */
      empSave = vid[2] & 0xFF;  /* emphasis     Programs may alter all 16     */
      ntcSave = vid[3] & 0xFF;  /* notice       values of vid[], of course.   */

      eaSave  = vid[10] & 0xFF;
      edSave  = vid[13] & 0xFF;
      vid[10] = 0x0F;
      vid[13] = 0x07;
}


static void reset()
{
      setwindow( tx, ty, tx + 3, ty + 5 );
      wputs("^í");
      clrwindow();
      setwindow( tx, ty, tx + 14, ty + 5 );
      gotoxy(4,1);
      wputs("^0Normal    ^í");
      gotoxy(4,2);
      wputs("^1Bold      ^í");
      gotoxy(4,3);
      wputs("^2Emphasis  ^í");
      gotoxy(4,4);
      wputs("^3Attention!^í");
}


static void position( line ) int line;
{
      reset();
      gotoxy( 1,0 + line );
      wink( 24 );
      gotoxy( 2,1 + line );
      wink( 26 );
      gotoxy( 1,2 + line );
      wink( 25 );
      gotoxy( 0,1 + line );
      wink( 27 );
      gotoxy( 1,1 + line );

      wprintf("^ê%c^í",icon);;
}


static void normPos()
{
      position(0);
}


static void boldPos()
{
      position(1);
}


static void emphPos()
{
      position(2);
}


static void notePos()
{
      position(3);
}


static void pause()
{
      static int n;
      for (n = 380; n; n--) if (kbhit()) break;
}


static void waitforuser()
{
      int flag;
      flag = 0;
      while (1)
      {
          icon = flag? 'K':'?'; normPos(); pause();
          icon = flag? 'E':'A'; boldPos(); pause();
          icon = flag? 'Y':'N'; emphPos(); pause();
          icon = flag?  19:'Y'; notePos(); pause();
          if (kbhit()) break;
          else flag ^= 1;
      }
      while (kbhit()) getch();
      icon = '?';
}


static void help()
{
      int k;
      static char *p, *msg[] = { "re", "ob", "in" };

      if (video.mode == MONOCHROME) p = msg[0]; /* absolute */
      else p = flipflop? msg[1]: msg[2];

      wputs("^í");
      clrwindow();
      wputs("^êF10^í  help\n");
      wprintf("^ê%c%c%c%c^í select\n",27,24,25,26);
      wputs("^êF1^í   bright\n");
      wputs("^êF2^í   blink\n");
      wprintf("^êF9^í   %sverse\n", p);
      wputs("^êESC^í  done!");
      for (k = 20; k; k-- ) pause();
      if (kbhit()) ch = keyin( screenwait ); else ch = '?';
      clrwindow();
}




/*  The Monochrome Adapter case is simple... */


static void mono()
{
      int ix, hibyte, lobyte, hibit, lobit;

      ix = which;
      lobyte = vid[ ix ] & 0x07;  /* foreground color? */

      /* preserve status of blink and bright bits */

      lobit  = vid[ ix ] & 0x08;  /* intensity bit?    */
      hibit  = vid[ ix ] & 0x80;  /* blink bit?        */

      hibyte = 0;
      if (lobyte == 0) /* turn off reverse, turn on underline */
      {
             lobyte = 0x01;
      }
      else if (lobyte == 1) /* turn off underline, turn on normal */
      {
             lobyte = 0x07;
      }
      else /* turn off normal, turn on reverse */
      {
             lobyte = 0x00;
             hibyte = 0x70;
      }
 
      vid[ ix ] = hibyte | hibit | lobyte | lobit;
}



/* The Color/Graphics Adapter case is even simpler...! */


static void cgaright()
{
      int ix, hibyte, lobyte, hibit, lobit;

      ix = which;
      lobyte = vid[ ix ] & 0x07;  /* foreground color? */
      hibyte = vid[ ix ] & 0x70;  /* background color? */

      /* preserve status of blink and bright bits */

      lobit  = vid[ ix ] & 0x08;  /* intensity bit?    */
      hibit  = vid[ ix ] & 0x80;  /* blink bit?        */

      if (flipflop)
      {
          hibyte += 0x10;
          if (hibyte > 0x70) hibyte = 0;
      }
      else
      {
          ++lobyte;
          if (lobyte > 7) lobyte = 0;
      }
      vid[ ix ] = hibyte | hibit | lobyte | lobit;
}


static void cgaleft()
{
      int ix, hibyte, lobyte, hibit, lobit;

      ix = which;
      lobyte = vid[ ix ] & 0x07;  /* foreground color? */
      hibyte = vid[ ix ] & 0x70;  /* background color? */

      /* preserve status of blink and bright bits */

      lobit  = vid[ ix ] & 0x08;  /* intensity bit?    */
      hibit  = vid[ ix ] & 0x80;  /* blink bit?        */

      if (flipflop)
      {
          hibyte -= 0x10;
          if (hibyte < 0) hibyte = 0x70;
      }
      else
      {
          --lobyte;
          if (lobyte < 0) lobyte = 7;
      }
      vid[ ix ] = hibyte | hibit | lobyte | lobit;
}




static void inquire()
{
      unsigned bl, bt, flip;

      flipflop = which = 0;
      while (ch != 27)
      {
            position( which );
            ch = keyin( screenwait );
            switch ( ch )
            {
               case  ESC:
               {
                    return;
               }
               case F1:    /* F1?  bright */
               {
                    vid[ which ] ^= 0x08;  /* toggle intensity bit */
                    break;
               }
               case F2:    /* F2?  blink */
               {
                    vid[ which ] ^= 0x80;  /* toggle blink bit */
                    break;
               }
               case F9:    /* F9?  reverse, or obverse/inverse */
               {
                    if (video.mode == MONOCHROME)
                    {
                         flip = vid[ which ] & 0x07;  /* read foreground bits */
                         bl   = vid[ which ] & 0x80;  /* save blink */
                         bt   = vid[ which ] & 0x08;  /* save bright */
                         if (flip)
                         {
                            vid[ which ] = 0x70 | bl | bt;
                         }
                         else
                         {
                            vid[ which ] = 0x07 | bl | bt;
                         }
                    }
                    else
                    {
                         flipflop ^= 1;  /* foreground or background */
                    }
                    icon = (flipflop? 'b':'?');
                    break;
               }
               case F10:   /* F10? */
               {
                    help();
                    break;
               }
               case UP:    /* up arrow?    */
               {
                    if (--which < 0) which = 3;
                    break;
               }
               case LF:    /* left arrow?  */
               {
                    if (video.mode == MONOCHROME) mono();
                    else cgaleft();
                    break;
               }
               case RT:    /* right arrow? */
               {
                    if (video.mode == MONOCHROME) mono();
                    else cgaright();
                    break;
               }
               case DN:    /* down arrow?  */
               {
                    if (++which > 3) which = 0;
                    break;
               }
            }
      }
}


static void set_hues_at( x,y ) int x,y;
{
      tx = x;
      ty = y;
      wputs("^í");
      windowbox( tx, ty, tx + 14, ty + 5 );
      waitforuser(); 
      help();
      inquire();
}


static void setnewcolors( p ) char far *p;
{
     static int n, test;

     /* on entry, p points to first char in saved screen buffer */

     n = 2000;
     while (n--)
     {
           p++;                   /* advance to attribute byte */
           test = *p & 0xFF;      /* read old attribute, kill sign extension */
           if (test == nrmSave)   /* update new attribute as indicated */
           {
                *p = vid[0];
           }
           else if (test == bldSave)
           {
                *p = vid[1];
           }
           else if (test == empSave)
           {
                *p = vid[2];
           }
           else if (test == ntcSave)
           {
                *p = vid[3];
           }
           p++;                   /* advance to next character */  
     }
}


void clairol()
{
      char far *screen;
      union REGS x;

      valSave();
      screen = savescreen( &x );      /* get copy of original screen */
      hidecursor();

      /*
	  signal(SIGINT,ControlC);
      if (setjmp(remark) != 0 ) 
      {
           vid[0] = nrmSave;          *//* recover from user bailout *//*
           vid[1] = bldSave;
           vid[2] = empSave;
           vid[3] = ntcSave;
      }
      else 
	  */
	  set_hues_at( 32,8 );

      /* 
	  signal(SIGINT,SIG_DFL);
      while (kbhit()) getch();
	  *
	  /
      vid[10] = eaSave;
      vid[13] = edSave;
      wputs("^0");                  /* set video attribute to new normal */
      setnewcolors( screen );       /* update copy of original screen    */
      restorescreen( screen, &x );  /* restore updated original screen   */
}


/* eof: clairol.c */

