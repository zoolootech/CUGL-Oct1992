/*
HEADER:        ;
TITLE: trigger.c   
       trigonometric implementations of the Borland Graphics Interface;
VERSION:  1.0;

DESCRIPTION: horsing around with trig functions and the Borland Graphics
   Interface (BGI).  This program was created and run on a Compaq Plus
   and an IBM AT with a CGA card using Borland's TurboC 2.0.  Routines
   and variables with a single, initial capital letter (i.e. MainWindow())
   are taken directly from the demo program accompanying TurboC 2.0.
   The file CGA.BGI is likewise directly from the TurboC disk.  In theory
   by including a .BGI file for every conceivable combination of video
   cards and allowing the program to auto detect the card one could make it
   universally adaptable.  In this case, however, the medium resolution
   mode of the CGA card is forced.  To allow for other cards, one could
   initiate the sequence--
           GraphDriver = DETECT;
           initgraph( &GraphDriver, &GraphMode, "" );
   but this would require all of the *.BGI files to be available on disk
   or compiled within the program.
   In addition several of the Borland routines are specific to IBM and
   close compatibles so that the program is not very portable as written.
   The Borland file 'graphics.h' contains the definitions for many of the
   global variables specific for their functions (e.g. DETECT).

KEYWORDS: trig functions, BGI, Borland Graphics Interface, TurboC;
SYSTEM:   MSDOS;
FILENAME: TRIGGER.C;
WARNINGS: Expects CGA card, will not run on monochrome
          must have CGA.BGI on same directory or path to it;
SEE-ALSO: TRIGGER.EXE;
AUTHORS:  Henry Pollock;
COMPILERS:  TurboC 2.0;
*/

/*
--  TRIGGER.C  horsing around with trig functions and the Borland Graphics
--             Interface (BGI).
--                               Henry Pollock
--                               Essex Center Drive
--                               Peabody, MA 01960
--                               7/15/89
*/







#include <conio.h>
#include <dos.h>
#include <graphics.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define CTR_X_ORIGIN X_OFFSET + (MaxX - X_OFFSET)/2
#define CTR_Y_ORIGIN Y_OFFSET + (MaxY - Y_OFFSET)/2
#define ESC    0x1b           /* Define the escape key */
#define FALSE  0              /* Define some handy constants     */
#define OFF    0              /* Define some handy constants     */
#define ON     1              /* Define some handy constants     */
#define PI     3.14159        /* Define a value for PI */
#define SW_X_ORIGIN X_OFFSET
#define SW_Y_ORIGIN MaxY - Y_OFFSET
#define TRUE   1              /* Define some handy constants     */





double AspectRatio;      /* Aspect ratio of a pixel on the screen*/
int    ErrorCode;        /* Reports any graphics errors          */
int    GraphDriver;      /* The Graphics device driver      */
int    GraphMode;        /* The Graphics mode value         */
int    MaxColors;        /* The maximum # of colors available    */
int    MaxX, MaxY;       /* The maximum resolution of the screen */
int    X_OFFSET = 0;
int    Y_OFFSET = 0;


/*Routines from Borland Demo Disk */
int  gprintf(int *xloc, int *yloc, char *fmt, ... );
void changetextstyle(int font, int direction, int charsize);
void DrawBorder(void);
void Initialize(void);
void MainWindow(char *header);
void StatusLine(char *msg);

/*Trig routines*/

int menu();
int scale_x(float *, float *);
int scale_y(float *, float *);
void asteroid();
void autoloid();
void circloid();
void cycloids();
void ctr_graph_plot();
void draw_circleoid(int *, int *, float *);
void get_params2(float *power);
void get_params8(char *cycloid, int *cusps);
void draw_roulette(char *cycloid, int *cusps);
void lissajous();
void logo();
void spiral();
void sw_graph_plot();
void tschirnhausen();
void roulettes();







void changetextstyle(int font, int direction, int charsize)
{
    int ErrorCode;

    graphresult();              /* clear error code      */
    settextstyle(font, direction, charsize);
    ErrorCode = graphresult();       /* check result          */
    if( ErrorCode != grOk ){         /* if error occured      */
        closegraph();
        printf(" Graphics System Error: %s\n", grapherrormsg( ErrorCode ) );
        exit( 1 );
    }
}


int gprintf( int *xloc, int *yloc, char *fmt, ... )
{
    va_list  argptr;            /* Argument list pointer */
    char str[140];              /* Buffer to build sting into */
    int cnt;                    /* Result of SPRINTF for return */

    va_start( argptr, format );      /* Initialize va_ functions   */

    cnt = vsprintf( str, fmt, argptr );   /* prints string to buffer    */
    outtextxy( *xloc, *yloc, str );  /* Send string in graphics mode */
    *yloc += textheight( "H" ) + 2;       /* Advance to next line         */

    va_end( argptr );           /* Close va_ functions        */

    return( cnt );              /* Return the conversion count     */

}

void DrawBorder(void)
{
    struct viewporttype vp;

    setcolor( MaxColors - 1 );       /* Set current color to white */

    setlinestyle( SOLID_LINE, 0, NORM_WIDTH );

    getviewsettings( &vp );
    rectangle( 0, 0, vp.right-vp.left, vp.bottom-vp.top );

}


void MainWindow( char *header )
{
    int height;

    cleardevice();              /* Clear graphics screen */
    setcolor( MaxColors - 1 );       /* Set current color to white */
    setviewport( 0, 0, MaxX, MaxY, 1 );   /* Open port to full screen   */

    height = textheight( "H" );           /* Get basic text height        */

    changetextstyle( DEFAULT_FONT, HORIZ_DIR, 1 );
    settextjustify( CENTER_TEXT, TOP_TEXT );
    outtextxy( MaxX/2, 2, header );
    setviewport( 0, height+4, MaxX, MaxY-(height+4), 1 );
    DrawBorder();
    setviewport( 1, height+5, MaxX-1, MaxY-(height+5), 1 );

}


void Initialize(void)
{
    int xasp, yasp;             /* Used to read the aspect ratio*/

    GraphDriver = CGA;         /* force cga                  */
    GraphMode   = CGAC1;       /* cyan,magenta,white         */
    initgraph( &GraphDriver, &GraphMode, "" );
    ErrorCode = graphresult();       /* Read result of initialization*/
    if( ErrorCode != grOk ){         /* Error occured during init  */
        printf(" Graphics System Error: %s\n", grapherrormsg( ErrorCode ) );
        exit( 1 );
    }

    MaxColors = getmaxcolor() + 1;   /* Read maximum number of colors*/

    MaxX = getmaxx();
    MaxY = getmaxy();           /* Read size of screen        */

    getaspectratio( &xasp, &yasp );  /* read the hardware aspect   */
    AspectRatio = (double)xasp / (double)yasp; /* Get correction factor */

}

void StatusLine( char *msg )
{
    int height;
    setviewport( 0, 0, MaxX, MaxY, 1 );   /* Open port to full screen   */
    setcolor( MaxColors - 1 );       /* Set current color to white */
    changetextstyle( DEFAULT_FONT, HORIZ_DIR, 1 );
    settextjustify( CENTER_TEXT, TOP_TEXT );
    setlinestyle( SOLID_LINE, 0, NORM_WIDTH );
    setfillstyle( EMPTY_FILL, 0 );
    height = textheight( "H" );           /* Detemine current height      */
    bar( 0, MaxY-(height+4), MaxX, MaxY );
    rectangle( 0, MaxY-(height+4), MaxX, MaxY );
    outtextxy( MaxX/2, MaxY-(height+2), msg );
    setviewport( 1, height+5, MaxX-1, MaxY-(height+5), 1 );

}

/*#################################################*/

void main()  {
    int menu_nbr;
    char again = 'y';
    Initialize();
    while  (again == 'y')   {
        restorecrtmode();
        clrscr();
        logo();
        menu_nbr = menu();
        switch (menu_nbr) {
        case 1:
            autoloid();
            break;
        case 2:
            circloid();
            break;
        case 3:
            asteroid();
            break;
        case 4:
            spiral();
            break;
        case 5:
            lissajous();
            break;
        case 6:
            tschirnhausen();
            break;
        case 7:
             cycloids();
             break;
        case 8:
             roulettes();
             break;
        case 9:
            again = 'n';
            closegraph();
        }
    }
}
/*#################################################*/


void logo()
{
 printf("\nษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป"
        "\nบ serious software from                                     บ"
        "\nบ                                                           บ"
        "\nบ         I A T R O G E N I C   S O F T W A R E             บ"
        "\nบ             Essex Center Drive - Suite 205                บ"
        "\nบ            Peabody, Massachusetts 01960-2972              บ"
        "\nบ                                                           บ"
        "\nบ             you know its serious ....  when its iatrogenicบ"
        "\nศอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ");
}


int menu()   {
    int x,y,z;
    char zz[2];
 printf("\n    ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ"
        "\n    ณ  TRIGONOMETRIC CURVES culled from            ณ"
        "\n    ณ                    VARIOUS SOURCES           ณ"
        "\n    ณ                                              ณ"
        "\n    ณ  1. CIRCLES                                  ณ"
        "\n    ณ  2. MAKE A CIRCLEOID                         ณ"
        "\n    ณ  3. ASTEROID AND OTHERS                      ณ"
        "\n    ณ  4. SPIRALS                                  ณ"
        "\n    ณ  5. LISSAJOUS or BOWDITCH CURVE              ณ"
        "\n    ณ  6. TSCHIRNHAUSEN'S CUBIC or l'HOPITAL'S     ณ"
        "\n    ณ     or TRISECTRIX of CATALAN                 ณ"
        "\n    ณ  7. CYCLOIDS                                 ณ"
        "\n    ณ  8. MAKE A CYCLOID                           ณ"
        "\n    ณ  9. QUIT                                     ณ"
        "\n    ณ                   which choice ?             ณ"
        "\n    ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู");

    x = wherex() - 13;
    y = wherey() - 1;
     while  ( (atoi(zz) <1 ) || ( atoi(zz) > 9) )  {
        gotoxy(x,y);
        printf(" \b");
          scanf("%1s",zz);
    }
    z = atoi(zz);
    return(z);
}


void sw_graph_plot()          {
    setcolor(MaxColors-3);
    setlinestyle(SOLID_LINE,0,THICK_WIDTH);
    moveto(SW_X_ORIGIN, SW_Y_ORIGIN);
    lineto(SW_X_ORIGIN,Y_OFFSET);
    moveto(SW_X_ORIGIN, SW_Y_ORIGIN);
    lineto(MaxX, SW_Y_ORIGIN);
    setlinestyle(SOLID_LINE,0,NORM_WIDTH);
}

void ctr_graph_plot()         {
    setcolor(MaxColors-3);
    setlinestyle(SOLID_LINE,0,THICK_WIDTH);
    moveto(CTR_X_ORIGIN, 0);
    lineto(CTR_X_ORIGIN, MaxY);
    moveto(X_OFFSET, CTR_Y_ORIGIN);
    lineto(MaxX, CTR_Y_ORIGIN);
    setlinestyle(SOLID_LINE,0,NORM_WIDTH);
}


void lissajous()         {
    double t;
    float x,y,a,b,c,d,f;
    int x_plot,y_plot,color,mode,again,x_out,y_out;
    mode = getgraphmode();
    setgraphmode( mode );
    MainWindow( "Bowditch Sine Curves" );
    StatusLine( "Return quits" );
    ctr_graph_plot();
    settextjustify(LEFT_TEXT, TOP_TEXT);
    x_out = 10;
    y_out = 10;
    gprintf(&x_out,&y_out,"Equation:      x = sin(c*t),                y = sin(t) ");
    gprintf(&x_out,&y_out,"for t = 1 to 100");
    again = 'y';
    while ( again == 'y' || again == 'Y') {
        color = 0 ;
        d = 0;
        c = .2;
        for (c = .25; c <= 1.0; c += .25)  {
            if( kbhit())   break;
            color +=1 ;
            if (color == MaxColors) color = 1;
            gprintf(&x_out,&y_out,"c = %.2f",c);
            for( t = 0; t <= 100; t+=.1) {
                if( kbhit())   break;
                x = ((MaxX-X_OFFSET)/3) * (sin((c*t) + d));
                y = ((MaxY-Y_OFFSET)/3) * (sin(t));
                x_plot = (int)x + CTR_X_ORIGIN;
                y_plot = (int)y + CTR_Y_ORIGIN;
                putpixel(x_plot, y_plot,color);
            }
        }
        gprintf(&x_out,&y_out,"all done");

        again = getche() ;
    }
}



void tschirnhausen()          {
    float x,y,a,t,f;
    int x_out,y_out,x_plot,y_plot,x_range,y_range,again,mode;

    a = 1;
    x_range = 60;
    y_range = 140;

    mode = getgraphmode();
    setgraphmode( mode );
    MainWindow( "Family of Tschirnhausen Curves" );
    StatusLine( "Return quits" );
    settextjustify(LEFT_TEXT, TOP_TEXT);
    x_out = 10;
    y_out = 10;
    gprintf(&x_out,&y_out,"Equation:      x = 3a(t^2 - 3)               y = at(t^2 - 3)");
    gprintf(&x_out,&y_out,"for t from -4.4 to +4.4");
    again = 'y';
    while ( again == 'y' || again == 'Y') {
        ctr_graph_plot();
        for( a = .5; a <=5; a+=.5) {
            gprintf(&x_out,&y_out,"a = %.1f",a);
            for (t = -4.4; t <= 4.4; t+=.02)  {
                if  ( kbhit())   break;
                x = 3 * a * ((t * t) - 3);
                y = a * t * ((t * t) - 3);

                x_plot = (int)x * (180/x_range) + CTR_X_ORIGIN;
                y_plot = (int)y * (200/y_range) + CTR_Y_ORIGIN;

                putpixel(x_plot, y_plot, MaxColors-2);
            }
        }
        gprintf(&x_out,&y_out,"all done");
        again = getche() ;
    }
}




void spiral()          {

    double x,y,r,rr,a,aa,theta,m;
    int x_plot,y_plot,x_range,y_range,mode,again,color;
    mode = getgraphmode();
    setgraphmode( mode );
    settextjustify(LEFT_TEXT, TOP_TEXT);
    ctr_graph_plot();
    again = 'y';
    while ( again == 'y' || again == 'Y') {

        x_range = 30;  /* screen is 320 x 200   so 150 % x 20 & 32 */
        y_range = 48;
        color = 1;
        MainWindow( " x = 0cos(0), y = 0sin(0)" );
        StatusLine( "Archemedian: Return Advances" );
        color += 1;
        if (color == MaxColors) color = 1;
        for ( theta = 0; theta <= 8*PI; theta += .05) {
            if  ( kbhit())   break;
            x = theta * cos(theta);
            y = theta * sin(theta);

            x_plot = (int)(x * (180/x_range)) + CTR_X_ORIGIN;
            y_plot = (int)(y * (200/y_range)) + CTR_Y_ORIGIN;
            putpixel(x_plot, y_plot, color);
        }
        again = getch();

        MainWindow( " x =(e^0)cos(0), y = (e^0)sin(0)" );
        StatusLine( "Logarithmic: Return Advances" );
        color += 1;
        if (color == MaxColors) color = 1;

        for ( theta = 0; theta <= 8*PI; theta += .05) {
            if  ( kbhit())   break;
            x = (exp(theta*.25)*cos(theta))/10;
            y = (exp(theta*.25)*sin(theta))/10;

            x_plot = (int)(x * (180/x_range)) + CTR_X_ORIGIN;
            y_plot = (int)(y * (200/y_range)) + CTR_Y_ORIGIN;

            putpixel(x_plot, y_plot, color);
        }
        again = getch();

        MainWindow( " x = (1/0)cos(0), y = (1/0)sin(0)" );
        StatusLine( "Hyperbolic: Return Advances" );
        color += 1;
        if (color == MaxColors) color = 1;
        for ( theta = .05; theta <= 8*PI; theta += .05) {
            if  ( kbhit())   break;
            x = (30/theta) * cos(theta);
            y = (50/theta) * sin(theta);

            x_plot = (int)(x * (180/x_range)) + CTR_X_ORIGIN;
            y_plot = (int)(y * (200/y_range)) + CTR_Y_ORIGIN;

            putpixel(x_plot, y_plot, color);
        }
        again = getch();
    }

}


void autoloid()          {
    float power;
    int mode, x_out, y_out, again;
    mode = getgraphmode();
    setgraphmode( mode );
    MainWindow( "Modifications of a Circle" );
    StatusLine( "Return quits" );
    settextjustify(LEFT_TEXT, TOP_TEXT);
    ctr_graph_plot();
    again = 'y';
    while ( again == 'y' || again == 'Y') {
        for(power = .5; power <= 3; power = power + .5) {
            if  ( kbhit())   break;
            draw_circleoid(&x_out, &y_out, &power);
            if (power == 3)  {
                setcolor( MaxColors - 1 );
                gprintf(&x_out,&y_out,"all done");
            }
        }
     again = getche() ;
     }
}



void draw_circleoid(int *x_out, int *y_out, float *power)  {
    float radius, x_range, y_range, x, y_pos, y_neg;
    int x_plot, y_plot, x_plot_old, y_plot_old, x_scaled, y_scaled, color;


    radius =30;
    x_range = radius * 3;
    y_range = radius * 3;
    *x_out = 10;
    *y_out = 10;
    *y_out +=  (textheight( "H" )+2) ;   /* advance prior to back up    */
    *y_out -=  (textheight( "H" )+2) ;   /* back up to prior line       */
    setcolor(OFF);
    gprintf(x_out,y_out,"A^%2.1lf+B^%2.1lf=C^%2.1lf", *power-.5,*power-.5,*power-.5);
    *y_out -=  (textheight( "H" )+2) ;   /* back up to prior line       */
    setcolor( MaxColors - 1 );        /* Set current color to white */
    gprintf(x_out,y_out,"A^%2.1lf+B^%2.1lf=C^%2.1lf",*power,*power,*power);
    x_plot_old = CTR_X_ORIGIN;
    y_pos = radius;
    y_plot_old = CTR_Y_ORIGIN - scale_y(&y_pos, &y_range);

    setcolor( MaxColors - 2 );
    for ( x = 1; x <= radius ; x++) {
        if  ( kbhit())   break;
        y_pos = pow ( (  (pow(radius,*power))-(pow(x,*power))  ), 1/(*power));
        x_scaled = scale_x(&x, &x_range) ;
        y_scaled = scale_y(&y_pos, &y_range);
        x_plot = CTR_X_ORIGIN + x_scaled ;
        y_plot = CTR_Y_ORIGIN - y_scaled;

        line(x_plot_old,y_plot_old,x_plot,y_plot);
        x_plot_old = x_plot;
        y_plot_old = y_plot;
    }

    for ( x = radius; x >= 1 ; x--) {
        if  ( kbhit())   break;
        y_pos = pow ( (  (pow(radius,*power))-(pow(x,*power))  ), 1/(*power));
        x_scaled = scale_x(&x, &x_range) ;
        y_scaled = scale_y(&y_pos, &y_range);
        x_plot = CTR_X_ORIGIN + x_scaled ;
        y_plot = CTR_Y_ORIGIN + y_scaled;
        line(x_plot_old,y_plot_old,x_plot,y_plot);
        x_plot_old = x_plot;
        y_plot_old = y_plot;
    }

    for ( x = 1; x <= radius ; x++) {
        if  ( kbhit())   break;
        y_pos = pow ( (  (pow(radius,*power))-(pow(x,*power))  ), 1/(*power));
        x_scaled = scale_x(&x, &x_range) ;
        y_scaled = scale_y(&y_pos, &y_range);
        x_plot = CTR_X_ORIGIN - x_scaled ;
        y_plot = CTR_Y_ORIGIN + y_scaled;
        line(x_plot_old,y_plot_old,x_plot,y_plot);
        x_plot_old = x_plot;
        y_plot_old = y_plot;
    }

    for ( x = radius; x >= 1 ; x--) {
        if  ( kbhit())   break;
        y_pos = pow ( (  (pow(radius,*power))-(pow(x,*power))  ), 1/(*power));
        x_scaled = scale_x(&x, &x_range) ;
        y_scaled = scale_y(&y_pos, &y_range);
        x_plot = (int)CTR_X_ORIGIN - x_scaled ;
        y_plot = (int)CTR_Y_ORIGIN - y_scaled;
        line(x_plot_old,y_plot_old,x_plot,y_plot);
        x_plot_old = x_plot;
        y_plot_old = y_plot;
    }


}



scale_x(float *formula_x, float *x_range)
{
    float x;
    x = ( *formula_x / *x_range ) *  (MaxX - X_OFFSET) * 3/4;
    return (int)x;
}




scale_y(float *formula_y, float *y_range)
{
    float y;
    y = (*formula_y / *y_range) * ( MaxY - Y_OFFSET) ;
    return (int)y;
}

void get_params8(char *cycloid, int *cusps)  {
    char ch ;
    restorecrtmode();
    clrscr();
    gotoxy(0,4);
    printf
("The common cycloid was traced by a peripheral point on a rolling circle.\n"
"The prolate cycloid was traced by a point within the rolling circle\n"
"If one rolls the circle around another, fixed, circle instead of\n"
"along a straight line; then the point on the rim of the rolling\n"
"circle traces an epicycloid, while the one within the circle traces a\n"
"hypocycloid.  The size of the rolling circle in comparison to the\n"
"fixed circle determines the number of cusps; and this determines\n"
"the shape of the figure.  These figures are called roulettes, \n"
"and they include the asteroid and the nephroid which \n"
"were drawn above by entirely different means.  An epicycloid with 1 \n"
"cusp is a cardioid, a hypocycloid with 4 cusps is an asteroid.");
    ch =  'x';
    *cycloid  = 'x';
    *cusps    =  0;
    do         {
        while (!( ((*cycloid=='E') || (*cycloid=='e'))
                ||((*cycloid=='H')||(*cycloid=='h'))))  {
            gotoxy(30,15);
            clreol();
            printf("Either Epicycloid or Hypocycloid > 0");
            gotoxy(30,16);
            clreol();
            printf("Enter an 'e' or an 'h' ?  ");
            scanf("%c",cycloid);
        }

        while (!((*cusps > 0 ))) {
            gotoxy(30,18);
            clreol();
            printf("The number of cusps must be > 0");
            gotoxy(30,19);
            clreol();
            printf("What should the number be?  ");
            scanf("%d",cusps);
        }

        while (!( ((ch=='y') || (ch=='Y'))||((ch=='n')||(ch=='N'))))  {
            gotoxy(30,21);
            clreol();
            ((*cycloid=='E') || (*cycloid=='e'))
            ? printf("Epicycloid")
            :   printf("Hypocycloid");
            gotoxy(30,22);
            clreol();
            printf("Of %d cusps",*cusps);
            gotoxy(30,23);
            clreol();
            printf("Is this ok (y/n) ?");
            ch=getch();
        }
        if ( (ch=='y')||(ch=='Y')  )
            break;
        else  {
                ch =  'x';
                *cycloid  = 'x';
                *cusps    =  0;
        }
    }
    while (!(OFF==ON));
    clrscr();
}


void get_params2(float *power)
{
    char ch = 'x';
    restorecrtmode();
    clrscr();
    gotoxy(0,4);
    printf
        ("To make a graph of a circle:  Radius squared = x squared + y squared\n"
        "         or  R^2  =  X^2   +  Y^2 \n"
        "However, when the exponent is other than 2 distortions occur.\n"
        "You may enter a positive number for the exponent and observe that,\n"
        "the closer the exponent is to zero, the more nearly crossed lines are drawn\n"
        "while the larger the exponent, the more nearly tha graph approaches a square\n"
        "In all cases the 'radius' is constant and represents the furthest distance\n"
        "from the center");

    *power  = -1;
    do         {
        while (!((*power > 0 ))) {
            gotoxy(30,15);
            clreol();
            printf("The power must be > 0");
            gotoxy(30,16);
            clreol();
            printf("What should the power be?  ");
            scanf("%f",power);
        }

        while (!( ((ch=='y') || (ch=='Y'))||((ch=='n')||(ch=='N'))))  {
            gotoxy(30,20);
            clreol();
            printf("The power  is: %f", *power );
            gotoxy(30,21);
            clreol();
            printf("Is this ok (y/n) ?");
            ch=getch();
        }
        if ( (ch=='y')||(ch=='Y')  )
            break;
        else  {
            ch = 'x';
            *power  = -1;
        }
    }
    while (1);
    clrscr();
}


void circloid()          {
    float power;
    int mode, x_out, y_out, again;
    mode = getgraphmode();
    setgraphmode( mode );
    get_params2(&power);
    setgraphmode( mode );
    MainWindow( "Modifications of a Circle" );
    StatusLine( "Return quits" );
    settextjustify(LEFT_TEXT, TOP_TEXT);
    ctr_graph_plot();
    again = 'y';
    while ( again == 'y' || again == 'Y') {
        draw_circleoid(&x_out, &y_out, &power);
        if  ( kbhit())   break;
        again = getche() ;
    }
}



void asteroid()          {

    double z,zz,dblradius;
    int mode,again,x1,x2,y1,y2,x_cusp,y_cusp,radius,color;

    mode = getgraphmode();
    setgraphmode( mode );

    again = 'y';
    while ( again == 'y' || again == 'Y') {
        if  ( kbhit())   break;

        MainWindow( "Asteroid" );
        StatusLine( "Return Advances" );
          color=MaxColors-3;
        setcolor(color);
        radius = 80;
        for(z=0;z<=radius;z+=4){
            if  ( kbhit())   break;
            x1 = CTR_X_ORIGIN - z;
            x2 = CTR_X_ORIGIN + z;
            y1 = CTR_Y_ORIGIN - (CTR_Y_ORIGIN  - z);
            y2 = CTR_Y_ORIGIN + (CTR_Y_ORIGIN  - z);
            line(x1, CTR_Y_ORIGIN, CTR_X_ORIGIN, y1);
            line(x1, CTR_Y_ORIGIN, CTR_X_ORIGIN, y2);
            line(x2, CTR_Y_ORIGIN, CTR_X_ORIGIN, y1);
            line(x2, CTR_Y_ORIGIN, CTR_X_ORIGIN, y2);
        }
        again = getche() ;

        MainWindow( "Nephroid" );
          StatusLine( "Return Advances" );
        dblradius = 50;
          color =  MaxColors - 2;
          setcolor(color);
          circle(CTR_X_ORIGIN, CTR_Y_ORIGIN, (int)dblradius);
          color =  MaxColors - 3;
          setcolor(color);
        for(z=0;z <= PI/2; z+=.15){
            x1 = CTR_X_ORIGIN + (int)(dblradius * (cos(z)));
            x2 = CTR_X_ORIGIN - (int)(dblradius * (cos(z)));
            y1 = CTR_Y_ORIGIN - (int)(dblradius * (sin(z)));
            y2 = CTR_Y_ORIGIN + (int)(dblradius * (sin(z)));
            circle(x1, y1, (int)(dblradius * (cos(z))));
            circle(x2, y1, (int)(dblradius * (cos(z))));
            circle(x2, y2, (int)(dblradius * (cos(z))));
            circle(x1, y2, (int)(dblradius * (cos(z))));
        }
        again = getch();


        MainWindow( "Cardioid" );
        StatusLine( "Return Advances" );
        dblradius = 25;
          color =  MaxColors - 2;
          setcolor(color);
          circle(CTR_X_ORIGIN, CTR_Y_ORIGIN, (int)dblradius);
          color =  MaxColors - 3;
        setcolor(color);
        x_cusp = CTR_X_ORIGIN - (int)dblradius;
        y_cusp = CTR_Y_ORIGIN;
        for(z=0;z <= PI/2; z+=.15){
            x1 = CTR_X_ORIGIN + (int)(dblradius * (cos(z)));
            x2 = CTR_X_ORIGIN - (int)(dblradius * (cos(z)));
            y1 = CTR_Y_ORIGIN - (int)(dblradius * (sin(z)));
            y2 = CTR_Y_ORIGIN + (int)(dblradius * (sin(z)));
            circle(x1, y1,
            (int)sqrt(((abs(x1)-abs(x_cusp))*(abs(x1)-abs(x_cusp))) +
                ((abs(y1)-abs(y_cusp))*(abs(y1)-abs(y_cusp)))));
            circle(x2, y1,
            (int)sqrt(((abs(x2)-abs(x_cusp))*(abs(x2)-abs(x_cusp))) +
                ((abs(y1)-abs(y_cusp))*(abs(y1)-abs(y_cusp)))));
            circle(x2, y2,
            (int)sqrt(((abs(x2)-abs(x_cusp))*(abs(x2)-abs(x_cusp))) +
                ((abs(y2)-abs(y_cusp))*(abs(y2)-abs(y_cusp)))));
            circle(x1, y2,
            (int)sqrt(((abs(x1)-abs(x_cusp))*(abs(x1)-abs(x_cusp))) +
                ((abs(y2)-abs(y_cusp))*(abs(y2)-abs(y_cusp)))));
        }
        again = getch();

        MainWindow( "Limacon" );
        StatusLine( "Return Advances" );
        dblradius = 25;
          color =  MaxColors - 2;
          setcolor(color);
          circle(CTR_X_ORIGIN, CTR_Y_ORIGIN, (int)dblradius);
          color =  MaxColors - 3;
          setcolor(color);
        x_cusp = CTR_X_ORIGIN - 2*(int)dblradius ;
        y_cusp = CTR_Y_ORIGIN;
        for(z=0;z <= PI/2; z+=.15){
            x1 = CTR_X_ORIGIN + (int)(dblradius * (cos(z)));
            x2 = CTR_X_ORIGIN - (int)(dblradius * (cos(z)));
            y1 = CTR_Y_ORIGIN - (int)(dblradius * (sin(z)));
            y2 = CTR_Y_ORIGIN + (int)(dblradius * (sin(z)));
            circle(x1, y1,
            (int)sqrt(((abs(x1)-abs(x_cusp))*(abs(x1)-abs(x_cusp))) +
                ((abs(y1)-abs(y_cusp))*(abs(y1)-abs(y_cusp)))));
            circle(x2, y1,
            (int)sqrt(((abs(x2)-abs(x_cusp))*(abs(x2)-abs(x_cusp))) +
                ((abs(y1)-abs(y_cusp))*(abs(y1)-abs(y_cusp)))));
            circle(x2, y2,
            (int)sqrt(((abs(x2)-abs(x_cusp))*(abs(x2)-abs(x_cusp))) +
                ((abs(y2)-abs(y_cusp))*(abs(y2)-abs(y_cusp)))));
            circle(x1, y2,
            (int)sqrt(((abs(x1)-abs(x_cusp))*(abs(x1)-abs(x_cusp))) +
                ((abs(y2)-abs(y_cusp))*(abs(y2)-abs(y_cusp)))));
        }

        again = getch();

    }
}


void cycloids()      {

int again,color,radius,displacement,startangle,
     endangle,mode,x_offset,y_offset,delay,i;
struct arccoordstype arcinfo;
    mode = getgraphmode();
    setgraphmode( mode );

delay = 10000;
radius = 20;
X_OFFSET = 10;  /* offset for abcissa  */
x_offset = 5;   /* offset along abcissa */
Y_OFFSET = 50;
y_offset = 2;
    again = 'y';
    while ( again == 'y' || again == 'Y') {
       startangle = 341;
       endangle   = 340;
        MainWindow( "Common Cycloid" );
        StatusLine( "Return Advances" );
        sw_graph_plot();
           for (displacement = 0; displacement <=270; displacement += 10) {
                if  ( kbhit())   break;
               color = MaxColors - 2;
                setcolor(color);
           arc(X_OFFSET + x_offset + radius + displacement,
                   SW_Y_ORIGIN - radius - y_offset,
                   startangle -= 18,
                   endangle -= 18,
                   radius);
           getarccoords(&arcinfo);
        setcolor( MaxColors - 1 );  /* Set current color to white */
           line(    X_OFFSET + x_offset + radius + displacement,
                SW_Y_ORIGIN - radius - y_offset,
                arcinfo.xstart,
                arcinfo.ystart);
           for(i=0 ; i<delay; i++) {}
                   color = OFF;
                setcolor(color);
           line(    X_OFFSET + x_offset + radius + displacement,
                SW_Y_ORIGIN - radius - y_offset,
                arcinfo.xstart,
                arcinfo.ystart);
               arc(X_OFFSET + x_offset + radius + displacement,
                    SW_Y_ORIGIN - radius - y_offset,
                   startangle,
                   endangle,
                    radius);
            setcolor( MaxColors - 1 );  /* Set current color to white */
           arc(X_OFFSET + x_offset + radius + displacement,
                   SW_Y_ORIGIN - radius - y_offset,
                   endangle -1,
                   startangle,      /* put point                      */
                   radius + 1);     /* outside circle won't get erased*/

    }
       startangle = 341;
       endangle   = 340;
            setcolor( MaxColors - 1 );  /* Set current color to white */
           for (displacement = 0; displacement <=270; displacement += 10) {
                if  ( kbhit())   break;
           arc(X_OFFSET + x_offset + radius + displacement,
                   SW_Y_ORIGIN - radius - y_offset,
                   endangle -= 18,
                   startangle-= 18,
                   radius + 1);
        }

                again = getche() ;

        MainWindow( "Prolate Cycloid" );
        StatusLine( "Return Advances" );
        x_offset = radius/2 + 5;   /* offset along abcissa */
        y_offset = radius/2 + 2;
       startangle = 341;
       endangle   = 340;
        sw_graph_plot();
           for (displacement = 0; displacement <=270; displacement += 10) {
                if  ( kbhit())   break;
               color = MaxColors - 2;
                setcolor(color);
           arc(X_OFFSET + x_offset + radius + displacement,
                   SW_Y_ORIGIN - radius - y_offset,
                   startangle -= 18,
                   endangle -= 18,
                   radius);
               color = OFF;
                setcolor(color);
           arc(X_OFFSET + x_offset + radius + displacement,
                   SW_Y_ORIGIN - radius - y_offset,
                   startangle -= 18,
                   endangle -= 18,
                   radius + radius/2);
           getarccoords(&arcinfo);
        setcolor( MaxColors - 1 );  /* Set current color to white */
           line(X_OFFSET + x_offset + radius + displacement,
                SW_Y_ORIGIN - radius - y_offset,
                arcinfo.xstart,
                arcinfo.ystart);
           for(i=0 ; i<delay; i++) {}
               color = OFF;
                setcolor(color);
           line(X_OFFSET + x_offset + radius + displacement,
                SW_Y_ORIGIN - radius - y_offset,
                arcinfo.xstart,
                arcinfo.ystart);
           arc(X_OFFSET + x_offset + radius + displacement,
                SW_Y_ORIGIN - radius - y_offset,
                startangle,
                endangle,
                radius);
        setcolor( MaxColors - 1 );  /* Set current color to white */
           arc(X_OFFSET + x_offset + radius + displacement,
                SW_Y_ORIGIN - radius - y_offset,
                endangle -1,
                startangle,      /* put point                      */
                radius + radius/2);     /* outside circle won't get erased*/

    }
       startangle = 341;
       endangle   = 340;
            setcolor( OFF);
           for (displacement = 0; displacement <=270; displacement += 10) {
                if  ( kbhit())   break;
           arc(X_OFFSET + x_offset + radius + displacement,
                   SW_Y_ORIGIN - radius - y_offset,
                   endangle -= 36 ,
                   startangle-= 36,
                   radius + radius/2);
           getarccoords(&arcinfo);
            putpixel(arcinfo.xstart,
                arcinfo.ystart,
                MaxColors-1);
        }
        again = getch();

        MainWindow( "Curtate Cycloid" );
        StatusLine( "Return Advances" );
        x_offset = 5;   /* offset along abcissa */
        y_offset = 2;
       startangle = 341;
       endangle   = 340;
        sw_graph_plot();
           for (displacement = 0; displacement <=270; displacement += 10) {
                if  ( kbhit())   break;
               color = MaxColors - 2;
                setcolor(color);
           arc(X_OFFSET + x_offset + radius + displacement,
                   SW_Y_ORIGIN - radius - y_offset,
                   startangle -= 18,
                   endangle -= 18,
                   radius);
               color = OFF;
                setcolor(color);
           arc(X_OFFSET + x_offset + radius + displacement,
                   SW_Y_ORIGIN - radius - y_offset,
                   startangle -= 18,
                   endangle -= 18,
                   radius/2);
           getarccoords(&arcinfo);
        setcolor( MaxColors - 1 );  /* Set current color to white */
           line(X_OFFSET + x_offset + radius + displacement,
                SW_Y_ORIGIN - radius - y_offset,
                arcinfo.xstart,
                arcinfo.ystart);
           for(i=0 ; i<delay; i++) {}
               color = OFF;
                setcolor(color);
           line(X_OFFSET + x_offset + radius + displacement,
                SW_Y_ORIGIN - radius - y_offset,
                arcinfo.xstart,
                arcinfo.ystart);
           arc(X_OFFSET + x_offset + radius + displacement,
                SW_Y_ORIGIN - radius - y_offset,
                startangle,
                endangle,
                radius);
        setcolor( MaxColors - 1 );  /* Set current color to white */
           arc(X_OFFSET + x_offset + radius + displacement,
                SW_Y_ORIGIN - radius - y_offset,
                endangle -1,
                startangle,      /* put point                      */
                radius/2);     /* outside circle won't get erased*/

    }
       startangle = 341;
       endangle   = 340;
            setcolor( OFF);
           for (displacement = 0; displacement <=270; displacement += 10) {
                if  ( kbhit())   break;
           arc(X_OFFSET + x_offset + radius + displacement,
                   SW_Y_ORIGIN - radius - y_offset,
                   endangle -= 36 ,
                   startangle-= 36,
                   radius/2);
           getarccoords(&arcinfo);
            putpixel(arcinfo.xstart,
                arcinfo.ystart,
                MaxColors-1);
        }
        again = getch();
    }
X_OFFSET = 0;
Y_OFFSET = 0;
}


void roulettes()          {
    char cycloid;
    int cusps,mode, x_out, y_out, again;
    mode = getgraphmode();
    setgraphmode( mode );
    again = 'y';
    while ( again == 'y' || again == 'Y') {
    get_params8(&cycloid, &cusps);
    setgraphmode( mode );
    MainWindow( "Roulettes" );
    StatusLine( "Return quits / 'y' Again" );
    settextjustify(LEFT_TEXT, TOP_TEXT);
    ctr_graph_plot();
    draw_roulette(&cycloid, &cusps);
    again = getche() ;
    }
}


void draw_roulette(char *cycloid, int *cusps)  {
    double t;
    float radius, x_range, y_range, x_pos, y_pos, y_neg;
    int x_plot, y_plot, x_plot_old, y_plot_old, x_scaled, y_scaled,
    x_out, y_out, color, size;


    radius = 5;
    x_out = 70;
    y_out = 10;
    x_range = radius * 15;
    y_range = radius * 15;

    x_plot_old = 0;
    y_plot_old = 0;
    size = 24/(*cusps);
    if ( size < 1) size = 1;
    if ((*cycloid=='E') || (*cycloid=='e'))
        if(size > 5) size = 6;
    setcolor( MaxColors - 1 );
      if ((*cycloid=='E') || (*cycloid=='e')) {
        if (*cusps == 1) {
            gprintf(&x_out,&y_out,"Epicycloid  %d cusp",*cusps);
        }
        else {
            gprintf(&x_out,&y_out," Epicycloid  %d cusps",*cusps);
        }
      *cusps += 1;
      }
      else  {
         if (*cusps == 1) {
            gprintf(&x_out,&y_out,"Hypocycloid  %d cusp",*cusps);
         }
        else {
            gprintf(&x_out,&y_out,"Hypocycloid  %d cusps",*cusps);
        }
        *cusps -= 1;
      }
    for ( t = 0; t <= 2*PI ; t += .02) {
      if  ( kbhit())   break;
      if ((*cycloid=='E') || (*cycloid=='e')) {
        x_pos = (*cusps * size * (float)cos(t)) - (size * (float)cos(*cusps * t));
        y_pos = (*cusps * size * (float)sin(t)) - (size * (float)sin(*cusps * t));
      }
      else {
        x_pos = (*cusps * size * (float)cos(t)) + (size * (float)cos(*cusps * t));
        y_pos = (*cusps * size * (float)sin(t)) - (size * (float)sin(*cusps * t));
     }
        x_scaled = scale_x(&x_pos, &x_range) ;
        y_scaled = scale_y(&y_pos, &y_range);
        x_plot = CTR_X_ORIGIN + x_scaled ;
        y_plot = CTR_Y_ORIGIN - y_scaled;
    setcolor( MaxColors - 2 );
    if (!(x_plot_old == 0)) line(x_plot_old,y_plot_old,x_plot,y_plot);
        x_plot_old = x_plot;
        y_plot_old = y_plot;
    }
}