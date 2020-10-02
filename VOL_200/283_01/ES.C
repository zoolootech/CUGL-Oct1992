

/* es.c -- display/edit program screens, save as compilable C source text
**         d.c.oshel, 4/21/88
*/

#include "ciao.h"
#include "keys.h"
#include <dos.h>
#include <signal.h>


static int screen_number = 0;
static char far *video_RAM;
static char *sname;

/* control-c mesmerizer */
static jmp_buf isis;
void iris( void )
{
	longjmp(isis,-1);
}


extern unsigned read_keyboard( void (*fn) (void) );  /* Assembler! */

/*
unsigned read_keyboard( void (*fn) (void) );
{
	union REGS rx;
	while (!kbhit())
		(*fn)();
	rx.h.ah = 0;
	int86(22,&rx,&rx); *//* damn int86 does NOT ignore Control-C! *//*
	if (rx.h.al)
		rx.h.ah = 0;
	return (rx.x.ax);
}
*/

int dialogue( void )
{
     char far *p;
     union REGS x;
     int c;

     p = savescreen( &x );

     defcursor();
     wputs("^2");
     windowbox( 21,1,59,8  );
     fullscreen();
     gotoxy(23,0);
     wprintf("^0µ^1 %s ^0Æ^2",sname);
     setwindow( 21,1,59,8 );
     gotoxy( 0,1 );
     wputs("       ^1R^2eset screen\n");
     wputs("       ^1B^2lank screen (absolute!)\n");
     wputs("       ^1C^2 text file, save screen to\n");
     wputs("       ^1I^2mage  file, save screen to\n");
     wputs("       ^1N^2ext screen, quit editing\n\n");
     wputs("       Please select: ");
     flush_keys();
     c = keyin( screenwait );

     restorescreen(p,&x);
     return (c);
}

static char namebuffer[80];

int ask( void )
{
     char far *p;
     union REGS x;
     int c;
     char buffer[80];

     p = savescreen( &x );
     defcursor();
     wputs("^2");
     windowbox( 21,12,59,14  );
     sprintf(buffer,"^0  Save %s? ^1Y^0/N  \b\b\b\b\b",namebuffer);
     c = ((59-21) - (strlen(buffer) - 11))/2;
     if (c < 1) c = 1;
     gotoxy( c,1 );
     wputs(buffer);
     bopbleet();
     flush_keys();
     do
     {
        c = keyin( screenwait );
        c = isascii(c)? toupper(c) : c;
        if (c == CR) c = 'Y';
     }
     while ( c != 'Y' && c != 'N' );

     restorescreen(p,&x);
     return ( c == 'Y' );
}


int ask2( void )
{
     char far *p;
     union REGS x;
     int c;
     char buffer[80];

     p = savescreen( &x );
     defcursor();
     wputs("^2");
     windowbox( 21,12,59,14  );
     sprintf(buffer,"^0  Save %s? ^1Y^0/N  \b\b\b\b\b",sname);
     c = ((59-21) - (strlen(buffer) - 11))/2;
     if (c < 1) c = 1;
     gotoxy( c,1 );
     wputs(buffer);
     bopbleet();
     flush_keys();
     do
     {
        c = keyin( screenwait );
        c = isascii(c)? toupper(c) : c;
        if (c == CR) c = 'Y';
     }
     while ( c != 'Y' && c != 'N' );

     restorescreen(p,&x);
     return ( c == 'Y' );
}


void report( void )
{
     char far *p;
     union REGS xr;
     int x,y,x1,x2,y1,y2;

     x = col;
     y = row;
     p = savescreen(&xr);
     wputs("^3");
     rptchar('\xfb',1);
     hidecursor();
     x1 = 1;  
     y1 = 1; 
     if ( ((y >= 0) && (y <= 4)) && ((x >=  0) && (x <= 22)) )
     {
         x1 = 58;
     }
     x2 = x1 + 20;
     y2 = y1 + 2;
     windowbox( x1,y1,x2,y2  );
     gotoxy (1,1);
     wprintf("^1 Column %2d, Row %2d ^0", x,y );
     flush_keys();
     keyin( screenwait );
     restorescreen(p,&xr);
}

void readscreen( void )
{
     FILE *fp;
     register int i;
     union REGS x;

     x.h.ah = 0x0F;
     int86( 0x10, &x, &x );
 
     /* do we have mono adapter or cga/ega? */

     video_RAM = (x.h.al == 7)? ((char far *)( 0xB0000000L )) :
                                ((char far *)( 0xB8000000L ));

     if ((fp=fopen(sname,"rb")) == NULL) {
          gotoxy(0,0);
          rptchar(' ',2000);
          windowbox( 0,0,79,24 );
          fullscreen();
     }
     else
     {
          for ( i = 0; i < 4000; i++ )
          {
              video_RAM[i] = fgetc(fp);
              if (feof(fp))
              {
                 video_RAM[i] = 7;
              }
              if (ferror(fp))
              {
                 gotoxy(0,24);
                 printf("READSCREEN COULD NOT READ \"%s\"\n",sname);
                 thurb();
                 exit(1);
              }
          }
          fclose(fp);
     }
}
 

void title( void )
{
     char far *p;
     union REGS x;

     p = savescreen( &x );
     hidecursor();
     windowbox( 21,1,59,8  );
     fullscreen();
     gotoxy(23,0);
     wprintf("^0µ^1 %s ^0Æ",strupr(sname));
     setwindow( 21,1,59,8 );
     gotoxy( 0,7 );
     wputs("^2 Screens-to-C v.1.1, by David C. Oshel ^0\n");
     wputs("       ^1MicroConsulting Services^0\n");
     wputs("           ^11219 Harding Ave.^0\n");
     wputs("           ^1Ames, Iowa 50010^0\n");
     wputs("            ^1(515) 232-8679^0\n\n");
     wputs("  Press ^1any key^0 to begin, ^1Esc^0 to quit^0");
     flush_keys();
     keyin( screenwait );

     restorescreen(p,&x);
}


void doit( void )
{
     void saveit( void );
     void saveit0( void );
     char far *p;
     union REGS x;
     unsigned int c,d;

     col = 39;
     row = 12;
     setcursize(0,rasterl);
     wputs("^0");
     p = savescreen(&x);
     while (1)
     {
           gotoxy(col,row);
           c = read_keyboard( screenwait );

           switch (c)
           {
           case ESC:
                d = dialogue();
                switch (d)
                {
                case 'b':
                case 'B':
                     restorescreen(p,&x);
                     gotoxy(0,0);
                     rptchar(' ',2000);
                     windowbox( 0,0,79,24 );
                     fullscreen();
                     col = 39;
                     row = 12;
                     setcursize(0,rasterl);
                     wputs("^0");
                     p = savescreen(&x);
                     bopbleet();
                     break;
                case 'r':
                case 'R':
                     restorescreen(p,&x);
                     p = savescreen(&x);
                     bopbleet();
                     break;
                case 'c':
                case 'C':
                     saveit();
                case 'i':
                case 'I':
                     saveit0();
                case 'n':
                case 'N':
                case 'q':
                case 'Q':
                case 'x':
                case 'X':
                     _ffree(p);
                     defcursor();
                     goto zoo;
                     break;
                default:
                     break;
                }
                break;
           case F1:
                wputs("^1");
                break;
           case F2:
                wputs("^2");
                break;
           case F3:
                wputs("^3");
                break;
           case F4:
                wputs("^4");
                break;
           case F5:
                wputs("^5");
                break;
           case F6:
                wputs("^6");
                break;
           case F7:
                wputs("^7");
                break;
           case F8:
                wputs("^8");
                break;
           case F9:
                wputs("^9");
                break;
           case F10:
                wputs("^0");
                break;
           case Alt_F1:
                rptchar('É',1);
                ++col;
                if ( col > 79 ) col = 0;
                break;
           case Alt_F2:
                rptchar('»',1);
                ++row;
                if ( row > 24 ) row = 0;
                break;
           case Alt_F3:
                rptchar('Í',1);
                --col;
                if ( col < 0 ) col = 79;
                break;
           case Alt_F4:
                rptchar('Í',1);
                ++col;
                if ( col > 79 ) col = 0;
                break;
           case Alt_F5:
                rptchar('º',1);
                --row;
                if ( row < 0 ) row = 24;
                break;
           case Alt_F6:
                rptchar('º',1);
                ++row;
                if ( row > 24 ) row = 0;
                break;
           case Alt_F7:
                rptchar('Ç',1);
                --row;
                if ( row < 0 ) row = 24;
                break;
           case Alt_F8:
                rptchar('¶',1);
                ++row;
                if ( row > 24 ) row = 0;
                break;
           case Alt_F9:
                rptchar('È',1);
                --row;
                if ( row < 0 ) row = 24; 
                break;
           case Alt_F10:
                rptchar('¼',1);
                --col;
                if ( col < 0 ) col = 79;
                break;
           case Shift_F1:
                rptchar('Ú',1);
                ++col;
                if ( col > 79 ) col = 0;
                break;
           case Shift_F2:
                rptchar('¿',1);
                ++row;
                if ( row > 24 ) row = 0;
                break;
           case Shift_F3:
                rptchar('Ä',1);
                --col;
                if ( col < 0 ) col = 79;
                break;
           case Shift_F4:
                rptchar('Ä',1);
                ++col;
                if ( col > 79 ) col = 0;
                break;
           case Shift_F5:
                rptchar('³',1);
                --row;
                if ( row < 0 ) row = 24;
                break;
           case Shift_F6:
                rptchar('³',1);
                ++row;
                if ( row > 24 ) row = 0;
                break;
           case Shift_F7:
                rptchar('Æ',1);
                --row;
                if ( row < 0 ) row = 24;
                break;
           case Shift_F8:
                rptchar('µ',1);
                ++row;
                if ( row > 24 ) row = 0;
                break;
           case Shift_F9:
                rptchar('À',1);
                --row;
                if ( row < 0 ) row = 24; 
                break;
           case Shift_F10:
                rptchar('Ù',1);
                --col;
                if ( col < 0 ) col = 79;
                break;
           case UP:
                --row;
                if ( row < 0 ) row = 24;
                break;
           case DN:
                ++row;
                if ( row > 24 ) row = 0;
                break;
           case BS:
           case LF:
                --col;
                if ( col < 0 ) col = 79;
                break;
           case RT:
                ++col; 
                if ( col > 79 ) col = 0;
                break;
           case CR:
                wputs("^0");
                col = 0;
                ++row;
                if ( row > 24 ) row = 0;
                break;
           case END:
           case PGUP: /* do nothing */
           case PGDN:
                break;
           case HOME:
                report();
                break;
           default:
                rptchar(c,1);
                ++col;
                if ( col > 79 ) col = 0;
                break;
           }
     }
     zoo:
     ;
}


void editscreen( void )
{
     readscreen();
     title();
     doit();
}


void writescreen( void )
{
     FILE *fp;
     register int i,j;

     for ( i = 0; isascii(sname[i]); i++ )
     {
         namebuffer[i] = tolower((sname[i]));
         if ( sname[i] == '.' )
         {
            namebuffer[i] = '\0';
            break;
         }
     }
     strcat(namebuffer,".c");
     if ((fp=fopen(namebuffer,"w")) == NULL) {
          gotoxy(0,24);
          printf("SAVESCREEN COULD NOT OPEN \"%s\"\n",sname);
          thurb();
          exit(1);
     }
     namebuffer[i] = '\0';

     fprintf(fp,"\n\n/* Screen Array */\n\nchar %s_screen[4000]={",namebuffer);

     for ( i = 0; i < 4000; i++ )
     {
         if ( (i % 20) == 0 )
            fprintf(fp,"\n");
         j = video_RAM[i];
         j &= 0xFF; 
         fprintf( fp, "%d,", j );

         if (ferror(fp))
         {
            gotoxy(0,24);
            printf("SAVESCREEN COULD NOT WRITE TO \"%s\"\n",sname);
            thurb();
            exit(1);
         }
     }
     fprintf(fp,"\n}; /* end: %s_screen */\n\n",namebuffer);
     fclose(fp);
}


void saveit( void )
{
     int i;
     for ( i = 0; isascii(sname[i]); i++ )
     {
         namebuffer[i] = tolower((sname[i]));
         if ( sname[i] == '.' )
         {
            namebuffer[i] = '\0';
            break;
         }
     }
     strcat(namebuffer,".c");
     if ( ask() ) 
        writescreen();
}


void writeoldway( void )
{
     FILE *fp;
     register int i;

     if ((fp=fopen(sname,"wb")) == NULL) {
          gotoxy(0,24);
          printf("SAVESCREEN COULD NOT OPEN \"%s\"\n",sname);
          thurb();
          exit(1);
     }

     for ( i = 0; i < 4000; i++ )
     {
         fputc( video_RAM[i], fp );
         if (ferror(fp))
         {
            gotoxy(0,24);
            printf("SAVESCREEN COULD NOT WRITE TO \"%s\"\n",sname);
            thurb();
            exit(1);
         }
     }
     fclose(fp);
}


void saveit0( void )
{
     if ( ask2() ) 
        writeoldway();
}

main( int argc, char **argv )
{
     char far *p;
     union REGS x;
	 int i;

     vid_init(0);
     p = savescreen(&x);
	 if ( setjmp(isis) ) goto Zoo;
	 signal(SIGINT,iris);
     if ( argc < 2 )
     {
        restorescreen(p,&x);
        wprintf("\n^2   Usage: C>EDITSCRN [ [*.SCN] | [filename] ... ]  ^0\n\n");
        wprintf("^1F1 .. F10^0, set screen attribute (F10 is normal)    \n");
        wprintf("^1Shift-F1 .. Shift-F10^0, box characters (single line)\n");
        wprintf("^1Alt-F1 .. Alt-F10^0, box characters (double line)    \n");
        wprintf("Press ^1Home^0 to report current cursor location       \n");
        wprintf("Press ^1Esc^0 when done editing                        \n\n");
        goto Zp;
     }
     else for ( i = 1; i < argc; i++ )
     {
        sname = argv[ i ];
        editscreen();
     }
Zoo: restorescreen(p,&x);
Zp:  vid_exit();
	 signal(SIGINT,SIG_DFL);
     exit (0);
}

