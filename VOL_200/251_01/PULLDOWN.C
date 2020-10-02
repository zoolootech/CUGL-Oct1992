/*   COPYRIGHT (C) 1986 */
/*   BY JAMES L PINSON  */
/*  ALL RIGHTS RESERVED */


/* Compiled with Lattice C V2.14 */
/* Computer: IBM PC JR           */
/* Text editor: Sidekick         */
/* Last revision 3/16/1987       */



#include  "stdio.h"
#include  "ctype.h"

#define void int


#define BLACK   0        /*  THESE ARE FOR COLOR CARDS */
#define BLUE    1
#define GREEN   2
#define CYAN    3
#define RED     4
#define MAGENTA 5
#define BROWN   6
#define WHITE   7
#define L_BLUE  9     /* LIGHT-BLUE FOREGROUND ONLY */
#define L_GREEN 10    /* LIGHT-GREEN FOREGROUND ONLY */
#define YELLOW  14
#define IWHITE  15    /* INTENSE-WHITE FOREGROUND ONLY*/



#define UNDERLINE 1     /* THESE ARE FOR MONOCHROME CARDS */
#define NORMAL    7
#define HI_INTEN  15
#define REVERSE   112

#define TRUE  1
#define FALSE 0

unsigned int page;          /* extern decl. for functions*/
unsigned int attribute;
unsigned int mon_type;
char wrt_meth= 'f';



#define  NU_MAIN 5        /* number of main menu options */
#define  NU_SUB  5        /* number of sub menu options  */

 struct menu_str{        /* change this if you need more options */
    char *head;
    char *body[NU_SUB];
    void (*fun1)();
    void (*fun2)();
    void (*fun3)();
    void (*fun4)();
    void (*fun5)();
  } ;


main(argc,argv)
     int argc;
     char **argv;
{
extern unsigned int page;
extern unsigned int attribute;
extern unsigned int mon_type;

char ch,ext;

int i,hi_attr,nor_attr;

int demo();
int help();

 static  struct menu_str m_menu [NU_MAIN]={

     "   File   ",   /*  The first menu option       */
       "   Dir      ",   /*  Menu sub options */
       "   Load     ",
       "   Save     ",
       "   dElete   ",
       "   Path     ",
                demo,   /* The functions each sub-option call */
                demo,
                demo,    /* these all call the same fake function */
                demo,
                demo,

       "   fiNd   ",           /* The second menu option */
         " All-words  ",
         " First-word ",
         "\0",
         "\0",           /* space filler for unused option names */
         "\0",
                demo,
                demo,
                0,       /* unused function pointer */
                0,
                0,

       "   Configure   ",    /* The third option */
         " Modem ",
         " Screen ",
         " Printer ",
         "\0",
         "\0",
                demo,
                demo,
                demo,
                0,
                0,

       "        Output       ",           /* The fourth menu option */
         "  Screen  ",
         "  Printer ",
         "  Disk    ",
         "  Modem   ",
         "\0",
                demo,
                demo,
                demo,
                demo,
                0,


           " Help ",                       /* The fifth option */
           " Instant help (really works) ",
           "\0",
           "\0",
           "\0",
           "\0",
                help,
                0,
                0,
                0,
                0,
};


/* was a slow write requested? */
if (tolower (*argv[1])== 's') wrt_meth = 's';

page=0;


 mon_type =(what_mon());

  if (mon_type==1){                     /* FIND OUT IF YOU HAVE A COLOR CARD */
    hi_attr=  set_color(BLACK,CYAN);    /* AND SET ATTRIBUTES ACCORDINGLY */
    nor_attr= set_color(WHITE,BLACK);
  }
    else{
     hi_attr = REVERSE;
     nor_attr = NORMAL;
    }

 attribute = nor_attr;
 cursor(0);               /* hide cursor */
 win_save('s');
 cls();

 if (mon_type==1) make_help();

 make_inst();          /* SHOW INSTRUCTIONS */

 menu(m_menu,NU_MAIN,NU_SUB,hi_attr,nor_attr);

 win_save('r');        /* restore text display*/
 cursor(1);            /* restore cursor      */
}


int menu (m_menu,nu_main,nu_sub,hi_attr,nor_attr)
struct menu_str m_menu[];
int nu_main,nu_sub,hi_attr, nor_attr;

{
extern unsigned int page;
extern unsigned int attribute;
extern unsigned int mon_type;

int i,j,k,cur_x,cur_y,cur_opt,found,expert=1;
char ch, ext, ltr;

   ch= ' ';ext=' '; cur_opt=0; found =0;

if (mon_type==1) attribute = set_color(YELLOW,BLACK);
else attribute = nor_attr;

make_window(1,1,78,1,1);


  for(;;){               /* endless loop */

        for(i=0;i<nu_main;i++){
          j=0;
          while(ltr = m_menu[i].head[j++]){
            if ( ch==ltr && ch != ' '){
             found= TRUE;
             cur_opt = i;
            }
          }
        }
          if (ch==13){
           found = TRUE;
           expert = FALSE;
          }

                   ch=' ';
                   cur_x=2;cur_y=2;


               for(i=0;i< nu_main;i++){
                   if(i == cur_opt) attribute= hi_attr;
                    else attribute= nor_attr;
                     print(cur_x,cur_y,m_menu[i].head);

                 cur_x= cur_x+strlen(m_menu[i].head)+3;
                }

           if (!expert) found = TRUE;

           if (found){

             ext =(pull_down(m_menu,nu_sub,cur_opt)); /* pull-down options */
             if (ext == 27) expert = TRUE;
             if (ext == 'r' || ext == 'l') expert = FALSE;
             if (ext=='r') cur_opt = cur_opt+1;
             if (ext =='l') cur_opt = cur_opt -1;
             ch= ' ';
             ext= ' ';

            }

           if(!found){
            ch=' ';
            get_key(&ch,&ext);

            ch=toupper(ch);

          }

           if (ch==27) return;

           if (ext =='r' || ext == 'l') expert = 0;
           if (ext == 'r')  cur_opt = cur_opt +1;
           if (ext == 'l')  cur_opt = cur_opt -1;
           if (cur_opt >= nu_main) cur_opt =0;
           if (cur_opt < 0) cur_opt = nu_main-1;
           ext=' ';
           found=0;

  } /* end for(;;) */

}   /* end function */



int pull_down(m_menu,nu_sub,position)
struct menu_str m_menu[];
int position;

{
extern unsigned int page;
extern unsigned int attribute;
char ch=' ',ltr;
int ext=' ',hi_attr,nor_attr;
int i,j,tx,ty,start,width,nu_opt,cur_opt=0, found= FALSE;

nu_opt = nu_sub;

/* nu_sub = number of possible pull-down options */
/* find out how many are in use */

for(i=0;i<nu_opt;i++){
  if (m_menu[position].body[i][0] == '\0'){
   nu_opt = i;
   break;
  }

}


  if (mon_type==1){
    hi_attr=  set_color(BLACK,CYAN);
    nor_attr= set_color(WHITE,BLACK);
  }
    else{
     hi_attr = REVERSE;
     nor_attr = NORMAL;
    }

 attribute = nor_attr;

start=2;       /* figure where to draw pull-down box */
              /* 2 is column to start 1st box */
              /* add up length of menu heads */

  for(i=0; i< position; i++)  start= start+strlen(m_menu[i].head)+3;


  width=0;      /* figure max length of window */

  for (i=0;i< nu_opt;i++){
   if (strlen(m_menu[position].body[i]) > width){
    width= strlen(m_menu[position].body[i]);
   }
  }

                         /* move box to left if          */
                         /* it will spill off right side */

 if(start+width+1>80) start = 80-width-2;


  win_save ('s');

 if (mon_type ==1) attribute = set_color (YELLOW,BLACK);

 make_window(start++,3,width,nu_opt,0); /*make a window */
 attribute = nor_attr;

tx=start;ty=4;                       /* reposition for writing */

  for(;;){
         for(i=0;i< nu_opt;i++){
           if(i == cur_opt) attribute= hi_attr;
            else attribute= nor_attr;
            print(tx,ty++,m_menu[position].body[i]);
         };

         attribute = nor_attr;

          if(found ) {
             win_save('r');     /* remove box */

                /* if you want more than 5 menu options */
                /* change this next switch statement    */

             switch (cur_opt){       /* call function */
              case 0: (*m_menu[position].fun1)() ;break;
              case 1: (*m_menu[position].fun2)() ;break;
              case 2: (*m_menu[position].fun3)() ;break;
              case 3: (*m_menu[position].fun4)() ;break;
              case 4: (*m_menu[position].fun5)() ;break;

             }

          /* found = FALSE; */
          if (kbhit()) getch();  /* make sure keyboard buffer is clear */
           return(' ');
          }

        tx=start;ty=4;
       get_key(&ch,&ext); ch=toupper(ch);  /* get a character */
           if (ext == 'd')  cur_opt = cur_opt +1;
           if (ext == 'u')  cur_opt = cur_opt -1;
           if (cur_opt >= nu_opt) cur_opt =0;
           if (cur_opt < 0) cur_opt = nu_opt-1;

          if (ch== 13) found = TRUE;

        for(i=0;i<nu_opt;i++){   /* does it match an option? */
         j=0;
         while(ltr = m_menu[position].body[i][j++]){
            if ( ch==ltr){
             cur_opt = i;
             found = TRUE;
            }
         }
        }

           if (ext=='l'|| ext=='r') break;
           if (ch==27){          /* EXIT IF ESCAPE KEY */
            ext = ch;
            break;
           }
           ext=' ';ch=' ';
  } /* end for(;;)*/
  win_save('r');
  return (ext);
}



void make_help()
{

extern unsigned int page,attribute;

 page=1;


  print(1,1,"HELLO - THIS IS A SAMPLE OF AN INSTANT HELP SCREEN.");
  print(10,5,"THIS SCREEN WAS PRINTED TO THE SECOND PAGE OF GRAPHICS");
  print(10,7,"WHILE YOU WERE LOOKING AT THE MAIN MENU.");
  print(10,9,"THIS HELP SCREEN CAN BE LEFT UNDISTURBED");
  print(10,11,"AND REDISPLAYED AT ANY TIME.");
  print (1,20,"PLEASE TOUCH ANY KEY TO RETURN TO THE MAIN MENU.");


 page=0;

}


void make_inst()
{
 extern unsigned int attribute;

  if (mon_type==1)
   attribute=  set_color(GREEN,BLACK);
    else
     attribute = NORMAL;

 print (1,4,"INSTRUCTIONS:");
 print (1,6,"EXPERT MODE: Select by touching the key which represents each option.");
 print (15,7,"(the capital letter)");

 print (1,10, "ASSIST MODE: Pull-down menu by touching 'enter' or a cursor key.");
 print (14,11,"Select by highlighting with cursor keys- then touch return");
 print (14,13,"Return to Expert mode by touching 'escape'");

 print (1,15,"EXIT:        Touch 'Escape' while in expert mode.");
}


void mono_help()
{

  attribute=NORMAL;
  win_save('s');

  clear_window(1,4,80,21);
  print(1,7,"THIS IS A DEMONSTRATION OF A HELP SCREEN");
  print(1,9,"THIS TEXT WAS WRITTEN BY MEANS OF DIRECT MEMORY ADDRESS");
  print(1,10,"THE ORIGINAL SCREEN HAS BEEN SAVED AND WILL BE RESTORED ");
  print(1,11,"WHEN YOU EXIT THIS 'HELP' SCREEN");
  print(1,14,"PLEASE TOUCH ANY KEY TO CONTINUE");

   getch();
  win_save('r');
}



int demo()
{
int hit;
win_save('s');
make_window(20,10,40,5,1);

print(21,11,"Put your favorite routine here ");
print (21,14,"touch any key to return to menu");
getch(hit);

win_save('r');
}


int help()
{
 if (mon_type==1){     /*IF COLOR CARD FLIP PAGE TO */
    page=1;            /*SHOW TEXT ELSE WRITE TO CURRENT*/
    d_page();getch();  /* SCREEN*/
    page=0;d_page();

 } else mono_help();

}



                    /* SCREEN-FUNCTION LIBRAY */



/* DECLARE THE EXTERN VARIABLES        */
/* PAGE,ATTRIBUTE AND MON_TYPE         */
/* (MONITOR TYPE)                      */
/* IN YOUR MAIN PROGRAM                */





/*** GOTOXY ***/                        /* PUTS CURSOR AT X,Y POSITION */
void gotoxy(x,y)                        /* ON SELECTED PAGE            */
unsigned int x,y;                       /* 1,1 IS UPPER LEFT CORNER    */

{
   extern unsigned int page;

   struct { unsigned int ax,bx,cx,dx,si,di,ds,es; } regist;

   if (x<1 || x>80) return;
   if (y<1 || y>25) return;
   x--;y--;                  /* DOS starts co-ordinates at 0,0 */

   regist.ax = 0x0200;
   regist.dx = (y<<8) | x ;
   regist.bx = page<<8;  /*page number*/

   int86(0x10, &regist,&regist);
}

/*** WHEREXY ***/        /* RETURNS THE X,Y POSITION OF CURSOR */
int wherexy(x,y)
int *x,*y;


{
   extern unsigned int page;

   struct { unsigned int ax,bx,cx,dx,si,di,ds,es; } regist;


   regist.ax = 0x0300;

   regist.bx = page<<8;  /*page number*/

   int86(0x10, &regist,&regist);
   *x=( (regist.dx & 0x00ff)+1);
   *y= ( ( (regist.dx & 0xff00)>>8)+1);

}

/*** d_page ***/                 /* DISPLAYS THE PAGE INDICATED  */
void d_page()                    /* BY EXTER VAR PAGE            */
                                 /* USE ONLY WITH COLOR CARD     */

{
   extern unsigned int page;
   struct { unsigned int ax,bx,cx,dx,si,di,ds,es; } regist;
   regist.ax = (0x0500|page);
   int86(0x10, &regist,&regist);
}




/*** WIN_SAVE ***/                  /* SAVES OR RESTORES PRIMARY  */
void win_save(action)               /* DISPLAY SCREEN.            */
int action;                         /* (PAGE 0 FOR COLOR DISPLAY) */
                                    /* 's' = SAVE                 */
{                                   /* 'r' = RESTORE              */
 extern unsigned int page;          /*  SAVES CURSOR POSITION TOO */
 extern unsigned int mon_type;      /*  MAY MAKE SNOW ON CGA      */
 int position;
 static int ptr;

static struct {
         int x;
         int y;
         unsigned int buffer [4000];
       } window[2];


   if (mon_type==1) position=0xb800;  /* COLOR CARD */
    else position=0xb000;             /* MONOCHROME */

   if (action=='s') {                 /* SAVE */
    if (ptr>1){
     ptr=2;
     return(0);
    }
                          /* peek is a lattice function */
                          /* could use pointer in larger */
                          /* memory model                */
    peek(position,0x00,&window[ptr].buffer,4000);  /* SAVE SCREEN     */
    wherexy(&window[ptr].x,&window[ptr].y);        /* SAVE CURSOR LOC */
    ptr++;

   }

   if (action=='r') {                 /* RESTORE */
    if(ptr <1){
      ptr = 0;
      return(0);
    }
    ptr-- ;

    poke(position,0x00,&window[ptr].buffer,4000); /* RESTORE SCREEN */
    gotoxy(window[ptr].x, window[ptr].y);         /* RESTORE CURSOR */



   }

}



/*** SET_COLOR ***/                     /* CALL WITH FORGROUND  */
                                        /* AND BACKGROUND COLOR.*/
int set_color(foreground, background,)  /* RETURNS ATTRIBUTE.   */
  int foreground,background;
{
  return(background<<4|foreground);

}



/*** CLEAR_WINDOW ***/                  /* CALL WITH X,Y OF UPPER LEFT   */
void clear_window(x,y,width,height )    /* CORNER OF WINDOW AREA.        */
unsigned int x,y,width,height;          /* CLEARS DOWN AND TO RIGHT      */
                                        /* FOR WIDTH AND HEIGHT.         */
{                                       /* CLEARED WITH ACTIVE ATTRIBUTE */
   extern unsigned int page;            /* USE ON DISPLAYED PAGE ONLY!   */
   extern unsigned int attribute;
   struct { unsigned int ax,bx,cx,dx,si,di,ds,es; } regist;
   x--;y--;
   regist.ax = 0x0600;
   regist.cx = (y<<8) | x ;
   regist.dx =(y+height-1) <<8 |x + width-1 ;
   regist.bx =(attribute<<8);
   int86(0x10, &regist,&regist);
}



/*** box ***/

void box (x,y,width,height,type)    /* type 0 = pull-down box */
int x,y,width,height,type;          /* type 1 = regular box   */

{
  int i,j,ctr,u_right,u_left;
  char string[82];

  if(type==0){                  /* following sets corners */
   u_left  =   '\xc2';
   u_right =  '\xc2';
  }

  if (type == 1){
   u_left = '\xda';
   u_right = '\xbf';
  }

     string[0]= u_left;
     for(i=1;i<=width;i++) string[i]='\xc4';
     string[i++]=u_right; string[i]='\0';
     print(x,y++,&string[0]);

       for (i=0; i<height;i++){
        print(x,y,"\xb3");
        print(x+width+1,y++,"\xb3");
      }

          string[0]='\xc0';
          for(i=1;i<=width;i++) string[i]='\xc4';
          string[i++]='\xd9';string[i]='\0';
          print(x,y++,&string[0]);
}


int what_mon()          /* RETURNS A 1 IF COLOR CARD PRESENT */
{                       /* RETURNS A 0 IF MONOCHROME CARD    */

char mode;        /* CHAR DEFINES AN 8 BIT INTEGER */

    peek(0x0040,0x0049,&mode,1);

    if (mode==7) return(0);
    else return(1);
}


void cls()           /* SAME AS DOS CLS */
{
clear_window(1,1,80,25);
gotoxy(1,1);
}


void make_window(x,y,width,height,type)  /* DRAWS AND CLEARS A BOX */
unsigned int x,y,width,height,type;

{
  box(x++,y++,width,height,type);       /* DRAW BOX       */
  clear_window(x,y,width,height);       /* CLEAR INTERIOR */
}



int cursor(size)    /* SETS CURSOR SIZE */
int size;           /* 0= no cursor, 1 = normal, 2= big cursor */
{

 struct { int ax,bx,cx,dx,si,di,ds,es; } regist;

 regist.ax= 0x0100;

    if (mon_type == 1 ){                /* COLOR */
     if (size == 0) regist.cx = 0x0f0f;
     if (size == 1) regist.cx = 0x0607;
     if (size == 2) regist.cx = 0x0107;
    }

    if (mon_type == 0 ){                /* MONOCHROME */
     if (size == 0) regist.cx= 0x0f0f;
     if (size == 1) regist.cx = 0x0c0d;
     if (size == 2) regist.cx = 0x010d;
    }

   int86(0x10,&regist,&regist);

}



/*** PRINT ***/

void print(x,y,str)          /* A SWITCHER- ROUTES TO FAST_WRITE */
 unsigned int x,y;           /* OR TO DOS_PRT DEPENDING ON ARGV  */
 char *str;                  /* PASSED TO PROGRAM AND SORED IN   */
{                            /* EXTERN CHAR WR_METH              */
 extern char wrt_meth;

if (wrt_meth == 'f')                       /* FAST (DIRECT POKING) */
  if (wrt_meth == 'f') fast_write(x,y,str);

  if (wrt_meth == 's') {     /* SLOW (DOS METHOD) */
   gotoxy(x,y);
   dos_prt(str);
  }
}

/*** DOS_PRT ***/           /*  ASKS DOS TO WRITE A STRING WITH ATTRIBUTE   */
                            /*  DEFINED. AN ALTERNATIVE TO FAST WRITE IN    */
void dos_prt(str)           /*  THAT IT IS "WELL BEHAVED"(GOES THROUGH DOS) */
                            /*  SPECIFY PAGE AND SET CURSOR POSITION BEFORE */
char *str;                  /*  CALLING   */
{
   extern unsigned int page,attribute;
   unsigned int x,y;
   int c;

   struct { unsigned int ax,bx,cx,dx,si,di,ds,es; } regist;
   wherexy(&x,&y);

   while (*str) {                      /* WHILE NOT EOF LET DOS WRITE CHAR */
      if (x>80){
         x=1;
         y=y+1;
       }
      if (y>25) break;
      gotoxy(x++,y);
      regist.bx = (page<<8|attribute);

      regist.cx = 1;
      regist.ax = 0x0900| *str++;
      int86(0x10, &regist, &regist);

  }
  gotoxy(x,y);             /* put cursor at end of string */
}

/***FAST_WRITE***/               /* DIRECTLY POKES STRING AT X,Y POSITION */
void fast_write(x,y,string)      /* USES ATTRIBUTE AND PAGE.              */
int x,y;                         /* MAY CAUSE SNOW ON SOME GRAPHIC CARDS  */
char *string;

{
 extern unsigned int page;
 extern unsigned int attribute;
 extern unsigned int mon_type;   /* MONITOR TYPE */

 int position,offset,orig;

    if (page <=3 && page >=0) offset = 4000*page+96*page;
      orig = offset;
      offset=offset+((y-1)*160)+(2*(x-1));
      position =0xb800;

    if (mon_type ==0) position=0xb000;

   while(*string){
        poke(position,offset,string++,1);        /* POKE CHARACTER */
        poke(position,offset+1,&attribute,1);    /* POKE ATTRIBUTE */
        offset=offset+2;

   }
         offset = offset- orig;                    /* FIGURE WHERE I AM */
         x= ((offset% 160)/2)+1 ;y= offset/160+1 ; /* AND MOVE CURSOR   */
         gotoxy (x,y);

}

/*** get_key ***/             /* READ A CHAR             */
                              /* RETURN CHARACTER IN CH  */
int get_key(ch,ext)           /* IF IT IS A FUNCTION KEY */
char *ch;                     /* RETURN FOLLOWING IN EXT */
int  *ext;                    /* UP-ARROW    = 'U'       */
                              /* DOWN-ARROW  = 'D'       */
{                             /* RIGHT-ARROW = 'R'       */
  *ch=getch();                /* LEFT-ARROW  = 'L'       */

    if(!*ch){
      *ext=getch();

       switch (*ext){
         case 'H' :*ext ='u';break; /*  up    */
         case 'P' :*ext ='d';break; /*  down    */
         case 'M' :*ext ='r';break; /*  right   */
         case 'K' :*ext ='l';break; /*  left    */
         case 'G' :*ext ='h';break; /*  home    */
         case 'O' :*ext ='e';break; /*  end     */
         case 'R' :*ext ='I';break; /*  insert  */
         case 'S' :*ext ='D';break; /*  delete  */
       }
    }
}

