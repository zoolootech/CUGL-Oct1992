/*
*                          E D I P A L . C
*
*                 written by marwan el-augi  14-oct-1990 v1.0
*
*
*                 EGA/VGA palette editor for specific purpose
*
*     Copyright (c) 1990 by Marwan EL AUGI, MEASoftlines,France.
*     Copyright (c) 1987,1988  by Borland International
*         for some parts of the graphics routines.
*
*   No part (partial or total) may be modified without mentioning
*   the above copyrights provided the fact that you don't either
*   change the original name of the software (edipal.*).
*
*   Despite these restrictions you can copy and distribute the software
*   knowing that it is available in shareware form which is a user
*   supported way of selling , you benefit from the try-before-buy
*   concept. So if you find that it is useful to you or that the source
*   is valuable to you, please send a contribution of 5 US$ to the address
*   below.
*
*   To help improve it send any comment to :
*
*
*	      Mr. Marwan EL-AUGI
*	      Res. PROBY  BAT.A5  App.527
*	      115, imp. Villehardouin
*	      F-34090  MONTPELLIER
*			   FRANCE
*
*
*/

/*
HEADER:		;
TITLE:          EDIPAL
VERSION:        1.0;

DESCRIPTION:    "Lets you view, edit and save an EGA color palette."

                 The whole program is in graphics mode and will
                 run with EGA and mostly with VGA. You can edit
                 modify any of the 16 colors of the palette
                 (any to all at once) choosing the new color from
                 the 64 colors set of any EGA compatible card.



KEYWORDS:       EGA_colors,editor, palette;
SYSTEM:         DOS 3.21 and higher
FILENAME:       EDIPAL.C & OUT.C (provided with companion compiled
                                  .exe files)
WARNINGS:       needs a TurboC to compile (copyrighted to Borland
                                            International)
                Some routines are copyrighted to Borland International
                these routines are mentioned in the source file
                and must not be copied without clear mention
                in source and executable of the following statement :
                "Copyright (c) 1987,1988 Borland International"



SEE-ALSO:       README.DOC, OUT.C, ega.pal,
AUTHOR :        Marwan EL-AUGI
COMPILERS:      TurboC 2.0 (should compile with v1.5);

*/

/* include files */

#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

/* some #defines'  */

#define ESC	 0x1b			/*  define ESC key  */
#define TRUE	 1			/*  handy constants */
#define FALSE    0
#define END      0
#define CONT     1
#define SAVE     3
#define ABNORMAL 1
#define ERROR    2
#define NORMAL   0

/*  message in a msg...   form used in edipal.c */

char *msgcop1  = "Copyright (c) 1990 by Marwan El-Augi, MEASoftlines, France";
char *msgcop2  = "Copyright (c) 1987,1988 by Borland International";
char *msggiv   = "Give the color number   :";
char *msgerr1  = "Abnormal program termination";
char *msgbye   = "Good Bye from EDIPAL by MEASoftlines...";

char *msgstd   = "ESC aborts or press a key to continue...";
char *msgent   = "Esc aborts or press a key to begin process...";
char *msgentry = "Enter the colors now or press ESC to change palette";
char *msginp1  = "Invalid entry, only 0 to 6 are allowed : try again...";
char *msginp2  = "Invalid entry, only 0 to 9 are allowed : try again...";
char *msgcont  = "ESC saves your palette or press a key to continue editing";
char *msgsav   = "Press Y/y for saving new palette or any key aborts";
char *msgsure  = "Are you sure  ? ESC aborts or any key for yes";
char *msgext   = "Press ESC for exiting";

char *msginit  = "The initial palette configuration";
char *msgnew   = "The modified palette configuration";
char *msgend1  = "The Quit/Save ending screen showing your palette";
char *msged    = "Now you can change the palette settings";
char *msgset   = "Now the new palette is set as you see it on the screen";
char *msgbeg[] = {"Graphics device       :",
		  "Graphics mode         :",
		  "Screen resolution     :",
		  "Available colors      :",
		  "Actual color          :",
		  "Current fill color    :"};


/*  some useful global variables */

int	graphdriver;		    /*  graph device driver ega, vga... */
int 	graphmode;         	    /*  graphics mode value             */
int	MaxX, MaxY;		    /*  maximal screen resolution       */
int	Maxcolors;		    /*  maximal number of colors avail. */
int	errorcode;		    /*  graphics errors code variables  */
struct  palettetype  oldpal,newpal; /*  palette variables               */


/*
*     function prototypes
*/

void init(void);		    /* graphics initialization function    */
void graphstat(void);		    /* prints report of the actual config. */
void editpal(void);                 /* EGA palette editing                 */
void colorprint(char *msg);         /* print the palette                   */
void mainscreen(char *header);      /* formats a screen and writes on it   */
void drawborder(void);              /* draws a border                      */
void pause(char *header, int flag); /* pauses and waits for a key          */
void bye(void);                     /* exits the program giving a status   */
void end(int status);               /* the real exit routine               */
void save(void);                    /* save the palette config             */
void statlin(char *msg);            /* prints a status line                */
void getcolors(void);               /* gets the new numbers of the palette */
char input1(void);                  /* gets the  first color digit         */
char input2(void);                  /* gets the second color digit         */



/*
*
*    main() : Main function
*
*/

void main(void)
{

	init();		      /* init graphics mode */
	graphstat();          /* report mode and configuration settings */
	colorprint(msginit);
	pause(msgstd,END);

	editpal();            /* actual main function of modifying palet.*/

	end(NORMAL);

}

/*
*
*    functions library
*
*/


/*   init() : initialization function  */

void init(void)
{

   graphdriver = DETECT;      /* autodetection of driver requested ega...*/

   initgraph( &graphdriver, &graphmode, "");    /* init graph mode      */
   errorcode = graphresult();                   /* exit if an error     */
   if( errorcode != grOk )                      /* occured at init time */
     {
	printf(" Graphics System error : %s\n",grapherrormsg( errorcode));
	end(ERROR);
     }

   getpalette( &oldpal );       	/* save actual palette      */
   getpalette( &newpal );               /* initialize both palettes */
				 /* newpal = oldpal is another solution */

   Maxcolors = getmaxcolor() + 1;       /* maximal number of colors */

   MaxX = getmaxx();                    /* get size of screen*/
   MaxY = getmaxy();


}



/*   graphstat() : actual configuration function */

void graphstat(void)
{

   struct fillsettingstype  fillinfo;

   char *driver, *mode;
   char xval[3], yval[3], Max[3], filcol[3], curcol[3];
   int height , width;
   int i,j;

   getfillsettings( &fillinfo );

   mainscreen( "Actual Graphics System configuration" );
   settextjustify(LEFT_TEXT, TOP_TEXT);

   driver = getdrivername();
   mode   = getmodename(graphmode);       /* get current settings */

   height = textheight("H");
   width  = textwidth( "W");

   MaxX = getmaxx(); itoa(MaxX, xval, 10);
   MaxY = getmaxy(); itoa(MaxY, yval, 10);

   itoa(Maxcolors, Max, 10);
   itoa(getcolor(), curcol, 10);
   itoa(fillinfo.color, filcol, 10);


/* prints the configuration  */

      for (i=2,j=0; i <= 12; i+=2,j++)
	  {
	    outtextxy(25, i * height, msgbeg[j]);
	   }
      outtextxy(27 * width, 2 * height, driver );
      outtextxy(27 * width, 4 * height, mode);
      outtextxy(27 * width, 6 * height, "( 0 , 0 ,     ,     )");
	 outtextxy(37 * width, 6 * height,  xval );
	 outtextxy(43 * width, 6 * height,  yval );
      outtextxy(27 * width,  8 * height, Max );
      outtextxy(27 * width, 10 * height, curcol);
      outtextxy(27 * width, 12 * height, filcol);

      outtextxy(25, 38 * height, msgcop1);
      outtextxy(25, 39 * height, msgcop2);

   pause(msgstd, END);

}





/*   colorprint() : prints the window with the colors based on palette */

void colorprint(char *msg)
{
    struct viewporttype  vp;
    struct palettetype   curpal;

    int color, height, width;
    int x, y, i, j;

    char colnum[3];
    signed char actcol;

    mainscreen(msg);

    color = 1;
    getviewsettings( &vp );		/* Get the current window size	*/
    width  = 2 * ( (vp.right+1) / 16 ); /* Get box dimensions	   */
    height = 2 * ( (vp.bottom-10) / 10 );

    x = width / 2;
    y = height / 2;	/* Leave 1/2 box border 	*/

    for( j=0 ; j<3 ; ++j )		/* Row loop	*/
       {
	 for( i=0 ; i<5 ; ++i )		/* Column loop	*/
	    {
	      setfillstyle(SOLID_FILL, color);	/* Set to solid fill in color*/
	      setcolor( color );		/* Set the same border color*/

	      bar( x, y, x+width, y+height );        /* Draw the rectangle*/
	      rectangle( x, y, x+width, y+height );  /* outline the rectangle*/

	      if( color == BLACK )		/* If box was black...	*/
		{
		 setcolor( WHITE );	           /* Set drawing color to white*/
		 rectangle( x, y, x+width, y+height ); /* Outline black in white*/
		}



	      getpalette(&curpal);
	      actcol = curpal.colors[color];

	      sprintf(colnum, "%d", actcol);
	      outtextxy( x+(width/2), y+height+4, colnum );

	      color = ++color % Maxcolors;  /* Advance to the next color*/
	      x += (width / 2) * 3;	    /* move the column base 	*/

	    }				/* End of Column loop		*/

	 y += (height / 2) * 3;		/* move the row base		*/
	 x = width / 2;			/* reset column base		*/

       }				/* End of Row loop		*/

}



/*   mainscreen() : prints the main screen */

void mainscreen(char *header)
{
   int height;

   cleardevice();       		/* clear graphics screen */
   setcolor(Maxcolors - 1);             /* set color to white    */
   setviewport( 0, 0, MaxX, MaxY, 1);   /* viewport = fullscreen */

   height = textheight("H");             /* basic text height     */

   settextstyle( DEFAULT_FONT, HORIZ_DIR, 1);
   settextjustify( CENTER_TEXT, TOP_TEXT);;
   outtextxy( MaxX/2, 2, header);
   setviewport( 0, height+4, MaxX, MaxY-(height+4), 1);
   drawborder();
   setviewport( 1, height+5, MaxX-1, MaxY-(height+5), 1);

}


/*  drawborder() : prints the viewport */

void drawborder(void)
{
   struct  viewporttype  vp ;

   setcolor( Maxcolors - 1 );          /* set color to white  */

   setlinestyle( SOLID_LINE, 0, NORM_WIDTH );

   getviewsettings( &vp );
   rectangle( 0, 0, vp.right-vp.left, vp.bottom-vp.top );

}


/*  pause() : pauses and waits for a key */

void pause(char *header, int flag)
{

   int c;

   statlin(header);

   c = getch();                 /* reads kbd input */

   if( ESC == c)
     {
       switch(flag)
	 {
	   case CONT : return;
	   case END  : end(NORMAL);
	   case SAVE : end(SAVE);

	   default   : return;
	 }
     }

   if( 0 == c)
      {

	c = getch();

      }

   cleardevice();		/* clear the screen */

}




/*  statlin() : display a status line at the bottom of the screen */

void statlin( char *msg)
{
   int height;

   setviewport( 0, 0, MaxX, MaxY, 1 );
   setcolor(Maxcolors - 1);
   settextstyle( DEFAULT_FONT, HORIZ_DIR, 1 );
   settextjustify( CENTER_TEXT, TOP_TEXT );
   setlinestyle( SOLID_LINE, 0, NORM_WIDTH);
   setfillstyle( EMPTY_FILL, 0 );

   height = textheight( "H" );

   bar( 0, MaxY-(height + 4), MaxX, MaxY );
   rectangle( 0, MaxY-(height+4), MaxX, MaxY );
   outtextxy( MaxX/2, MaxY-(height+2), msg);
   setviewport( 1, height+5, MaxX-1, MaxY-(height+5), 1);

}

/*   editpal() : palette edition function  */

void editpal(void)
{
   int c;


   mainscreen(msged);
   statlin(msgent);

   c = getch();
   while ( ESC != c)
     {
	getcolors();
	colorprint(msgnew);
	statlin(msgcont);
	c = getch();
	switch(c)
	  {
	    case ESC : save();
	    default  : break;
	  }


	mainscreen(msged);
	statlin(msgent);
	c = getch();

     }

   end(NORMAL);

}



/* getcolors(); function to get the colors numbers */

void getcolors(void)
{
   int   i, y, val;
   int height, width;

   char c, d;


   char *value;
   char string1[1], string2[1];

   value = (char *) malloc(1);

   statlin(msgentry);

   height = textheight( "H");
   width  = textwidth(  "W");

   for( i = 1; i <= Maxcolors - 2; i++)
      {
	itoa( i, value, 10);
	outtextxy( 15 * width, 2 * i * height, msggiv );
	outtextxy( 25 * width, y = 2 * i * height, value);

	c = input1();
	if(c != ESC)
	  {
	    sprintf(string1, "%c", c);
	    outtextxy(29 * width, y, string1);
	  }
	  else
	   {
	      break;
	   }

	d = input2();
	if( d != ESC)
	  {
	    sprintf(string2, "%c",d);
	    outtextxy(30 * width, y, string2);
	   }
	  else
	   {

	     break;
	   }


	val = atoi(string1) * 10 + atoi(string2);
	setpalette(i, val);

      }

}

char input1(void)
{
  char car;

  car = getch();
  if( ESC == car)
    {
       return(car);
    }
    else
     {
	  while(car < '0' || car > '6')
	    {
	       statlin(msginp1);
	       car = getch();

	     }
	  statlin(msgentry);

     }

   return(car);
}

char input2(void)
{
  char car;

  car = getch();
  if( ESC == car)
    {
       return(car);
    }
    else
     {
	  while(car < '0' || car > '9')
	    {
	       statlin(msginp2);
	       car = getch();

	     }

	  statlin(msgentry);

     }
   return(car);
}

/*  bye() : exits the program */

void bye(void)
{

     getpalette(&newpal);
     colorprint(msgset);
     pause(msgext,SAVE);



}

/*   end(); the real ending routine */

void end(int status)
{



     switch(status)
	{
	  case ERROR    : closegraph();                       break;
	  case ABNORMAL : closegraph();clrscr();printf("\n%s\n",msgerr1);  break;
	  case NORMAL   : closegraph();clrscr();              break;
	  case SAVE     : clrscr();puts(msgbye);exit(status);
	  default       : closegraph();clrscr();              break;

	}


				/* Some housekeeping    */
      puts(msgbye);             /* before               */
      exit(status);             /* quitting             */
}


/*   save();   the save routine    */

void save(void)
{

    char c;

    colorprint(msgend1);
    statlin(msgsav);

    c = getch();

    switch(c)
     {
       case 'y'  :
       case 'Y'  : colorprint(msgend1);statlin(msgsure);
		   c = getch();
		   if( ESC == c)
		    {
		      return;
		    }
		    else bye();break;

       case ESC  :
       default   : return;
     }

}
