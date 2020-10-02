/******************************************************************************/
/*                                                                            */
/*  Program:     PAL, version 1.01                                            */
/*                                                                            */
/*  Description: This program gives the user the ability to change the        */
/*               palettes on the IBM's Enhanced Graphic Adapter.              */
/*                                                                            */
/*  Author:      Kent Cedola                                                  */
/*               2015 Meadow Lake Court, Norfolk VA, 23518                    */
/*                                                                            */
/*  Language:    Microsoft's C, V3.0                                          */
/*                                                                            */
/*  Date Coded:  August 28, 1985.                                             */
/*                                                                            */
/******************************************************************************/

#include <stdio.h>
#include <mcega.h>

#define UP_ARROW    (256 + 72)
#define DOWN_ARROW  (256 + 80)
#define LEFT_ARROW  (256 + 75)
#define RIGHT_ARROW (256 + 77)

main()
{

  register unsigned x,y;

  int      ch,i,x1,y1;

  unsigned pal[8][2];
  char     buf[16];

/*
    Initialize EGA graphic library and check for the present of an EGA.
*/

  gpparms();

  if (GDTYPE == 4)                      /* Give monochrome user bad news */
    {
    perror("Sorry, must have a Color Display not monochrome!");
    exit(1);
    }
  else if (GDTYPE != 5)                 /* Tell non-EGA users no can run */
    {
    perror("Enhanced Color Adapter and Display not found!");
    exit(2);
    };

  if (GDMEMORY == 64)                   /* We need lots of EGA memory   */
    {
    printf("This program will work much better with 128k+ EGA memory!");
    printf("    Hit any key to continue!");
    getch();
    };

  gpinit();                             /* We are now in graphic mode!  */

/*
    Setup screen
*/

  gpcolor(CYAN);
  gotoxy( 2, 1); gprintf("KC-PAL 2.00");
  gotoxy(26, 1); gprintf("Set the Palettes of IBM's EGA");
  gotoxy(67, 1); gprintf("KC-GRAPHICS");

  for (y = 0; y < 2; y++)
    for (x = 0; x < 8; x++)
      {
      gpcolor(y*8+x);
      gpmove(x*72+32,139-y*61);
      gpbox(x*72+103,199-y*61);

      if ((pal[x][y] = gprdpal(y*8+x)) == -1)
        {
        pal[x][y] = y * 56 + x;
        gppal(y*8+x,y*56+x);
        }
      gpcolor(CYAN);
      gotoxy(x*9+6,15-y*11);
      sprintf(buf,"C# %2d",pal[x][y]);
      gprintf(buf);
      };

  gpcolor(GREEN);
  gpmove( 0,   0);
  gprect(639,349);
  gpmove( 4,   3);
  gprect(635, 38);
  gpmove( 4,  41);
  gprect(635,346);
  gpmove( 31, 77);
  gprect(608,200);

  gpcolor(WHITE);

  gotoxy(18,17);
  gprintf("Palette Selected XX, Color XX, RGB = (X,X,X).");

  gotoxy(10,19);
gprintf("Use the arrow keys to select a palette.  Use +, -, R, G, B, or");
  gotoxy(06,20);
gprintf("numeric keys to change the current color. Hit the SPACE BAR to reset");
  gotoxy(06,21);
gprintf("the palettes to the their default values. Use the program EGASAV.COM");
  gotoxy(06,22);
gprintf("to retain changes while using other programs. Hit the 'ESC' key to");
  gotoxy(06,23);
gprintf("exit. Send comments (SASE) to 2015 Meadow Lake Ct., Norfolk VA 23518");

/*
    Start the program logic here
*/

  x = 0;
  y = 0;

  xoropt(x,y);
  newcolor(x,y,pal[x][y]);

  while (1)
    {
    gpcolor(2);

    if ((ch = getch()) == 0)
      ch = 0x100 | getch();

    switch (ch)
      {
      case UP_ARROW:
        xoropt(x,y);
        y = (y + 1) % 2;
        xoropt(x,y);
        break;

      case LEFT_ARROW:
        xoropt(x,y);
        x = --x % 8;
        xoropt(x,y);
        break;

      case RIGHT_ARROW:
        xoropt(x,y);
        x = ++x % 8;
        xoropt(x,y);
        break;

      case DOWN_ARROW:
        xoropt(x,y);
        y = (y + 1) % 2;
        xoropt(x,y);
        break;

      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        pal[x][y] = (pal[x][y] * 10) % 100 + (ch - '0');
        if (pal[x][y] > 63)
          pal[x][y] %= 10;
        break;

      case 'R':
      case 'r':
        i = (pal[x][y] >> 4 & 2 | pal[x][y] >> 2 & 1) + 1 & 3;
        pal[x][y] = (pal[x][y] & 0x1B) | ((i & 2) << 4 | (i & 1) << 2);
        break;

      case 'G':
      case 'g':
        i = (pal[x][y] >> 3 & 2 | pal[x][y] >> 1 & 1) + 1 & 3;
        pal[x][y] = (pal[x][y] & 0x2D) | ((i & 2) << 3 | (i & 1) << 1);
        break;

      case 'B':
      case 'b':
        i = (pal[x][y] >> 2 & 2 | pal[x][y] & 1) + 1 & 3;
        pal[x][y] = (pal[x][y] & 0x36) | ((i & 2) << 2 | i & 1);
        break;

      case '+':
        pal[x][y] = (pal[x][y] + 1) % 64;
        break;

      case '-':
        pal[x][y] = (pal[x][y] - 1) % 64;
        break;

      case ' ':
        for (y1 = 0; y1 < 2; y1++)
          for (x1 = 0; x1 < 8; x1++)
            {
            pal[x1][y1] = y1 * 56 + x1;
            if (x != x1 || y != y1)
              {
              gppal(y1*8+x1,y1*56+x1);
              gpcolor(CYAN);
              gotoxy(x1*9+9,15-y1*11);
              sprintf(buf,"%2d",pal[x1][y1]);
              gprintf(buf);
              }
            };

        break;

      case 27:
        gpterm();

        for (y = 0; y < 2; y++)
          for (x = 0; x < 8; x++)
            gppal(y*8+x,pal[x][y]);

        exit();
          break;

      default:
        putch('\007');
        break;
      }
    newcolor(x,y,pal[x][y]);
    }
}

xoropt(x,y)
  unsigned x,y;
{
  int x1,y1,x2;

  x1 = x * 72 + 40;
  x2 = x1 + 56;

  y1 = 210 - y * 154;

  gpcolor(GREEN);
  gpmerge(3);

  gpmove(x1,y1);
  gpbox(x2,y1+13);

  gpmerge(0);
}

newcolor(x,y,c)
  unsigned x,y,c;
{
  char buf[16];

  xoropt(x,y);

  gpcolor(CYAN);

  sprintf(buf,"%2d",c);
  gotoxy(x*9+9,15-y*11);
  gprintf(buf);
  gotoxy(45,17);
  gprintf(buf);

  sprintf(buf,"%2d",y*8+x);
  gotoxy(35,17);
  gprintf(buf);

  gotoxy(56,17);
  sprintf(buf,"%d",c >> 4 & 2 | c >> 2 & 1);
  gprintf(buf);

  gotoxy(58,17);
  sprintf(buf,"%d",c >> 3 & 2 | c >> 1 & 1);
  gprintf(buf);

  gotoxy(60,17);
  sprintf(buf,"%d",c >> 2 & 2 | c & 1);
  gprintf(buf);

  gppal(y*8+x,c);

  gpcolor(GREEN);
  xoropt(x,y);
}
