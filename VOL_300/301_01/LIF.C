/*
	HEADER:      CUG301;
	TITLE:       Conway's game of life;
	DATE:        09/30/89;
	DESCRIPTION: "This program is an example of the usage of the pull-down
                      graphics menu system (menu.c/menu.h) on the same disk.";
	KEYWORDS:	   mathematical games, finite automata;
	FILENAME:	   LIF.C;
	WARNINGS:	   "The author claims copyrights and authorizes
                      non-commercial use only.";
	AUTHORS:	   John Muczynski;
	COMPILERS:   Turbo C version 2.0;
*/

/*
	lif.c

	Conway's game of life.

	(uses menu)

    (c) Copyright 1989 by John Muczynski. All Rights Reserved.
*/


/*  	BULLETINS

jsm	09/04/89	known problems:
	1. the box is not being turned off properly.
	2. add a menu so that 1. is a feature after the bug is fixed.
	3. make global commands work inside of box (run/seed/...)
	4. the main computation loop can be made to run faster by using
	   a state table.
	5. if you stamp when no file is active, then it blows up.
	6. if you stamp when part of the box isn't on the plane, then
		it blows up.  (should wrap-around)

FUTURE EXPANSION:
jsm      09/29/89 -Need to check details before executing each function
                   from the user's keys.  For example, the [?][b][r] function
                   will run into never-never land if you execute it before
                   specifing a block.
                  -Need to assign default hot-keys.
                  -Need functions to mirror image the box over either the 
                   x or the y axis.  A second cursor (and box) would also
                   be useful.
                  -Need to take the input for the file name from the MENU
                   buffer so that macros can name files to be loaded and
                   saved.
                  -Need to optimize the run-time loop with a state machine
                   so that it runs faster.

*/


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

#include <bios.h>
#include <graphics.h>



#include "menu.h"


#define SIZE   5
#define COLS  17	/* MAX OF 17 MIN OF  ??1?? */
#define ROWS  60	/* MAX OF 60 MIN OF        */

/* HOW MUCH SPACE TO LEAVE AT LEFT OF SCREEN */
#define COLOFF 11

/* HOW MUCH SPACE TO LEAVE AT TOP OF SCREEN (FOR MENUS) */
#define ROWOFF 30	/* MIN OF 30 OR SO */


void initgr();
void onblk();

typedef int plane[COLS][ROWS];

  plane data1 = {0};
  plane data2 = {0};

  plane *old_ptr = &data1;
  plane *new_ptr = &data2;
  plane *temp_ptr = 0;

#define OLD_DATA (*old_ptr)
#define NEW_DATA (*new_ptr)

void cleardat();

static int oldX=0, oldY=0;
static int curX=0, curY=0;
static int blockX=0, blockY=0, blockon=0;







FILE *infil;
FILE *outfil;


int writeit(int val)
{
  fprintf(outfil,"%c",val*( '*' - '.' ) +'.');
  return(val);
};
void writelf()
{
  fprintf(outfil,"\n");
};



static failflag=0;

/* doesn't use param that is passed */
int readit()
{
  unsigned char ch;
  int val;

  fscanf(infil,"%c",&ch);
  val = (ch!='.');		/* IF ERROR, THEN MAKE IT 'ON' */
  return(val||failflag);
};

void readlf()
{
  char ch;

  fscanf(infil,"%c",&ch);
  if (ch!='\n') failflag=1;
};


  static int temp;
#define swapint(a,b) {temp=a; a=b; b=temp;}




/* do something to the marked block */
void eachblk(funct,functlf)
  int funct(int val);
  void functlf();
{
  int x,y;
  int ex, ey;
  int i,j, mask, oldval,newval;
  int fx,fy;
  int bX,bY;
  failflag=0;

  if (blockon==1) {
    bX=blockX;
    bY=blockY;
  };
  if (blockon==2) {
    bX=oldX-blockX;
    bY=oldY-blockY;
  };


    fx=bX;
    fy=bY;

    ex=oldX;
    ey=oldY;

    if (ex<fx) swapint(ex,fx);
    if (ey<fy) swapint(ey,fy);

    for(j=fy;j<=ey;j++){
      for(i=fx;i<=ex;i++){
	x=i/8;
	mask = (1 << (i%8));
	y=j;

	oldval = ( (NEW_DATA[x][y] & mask) != 0);
	newval = (funct(oldval) != 0);
	if (newval!=oldval) {
	  NEW_DATA[x][y] = NEW_DATA[x][y] ^ mask;
	  if( (OLD_DATA[x][y] & mask) == (NEW_DATA[x][y] & mask) ) {
	    OLD_DATA[x][y] = OLD_DATA[x][y] ^ mask;
	  };
	};

      }; /* end for i */
      functlf();
    }; /* end for j */
};




 static char filnam[80];
 static char *root = "";  /* WAS   "IMAGES\\"   */

/* turn off graphics and get a file name */
void getname(char *filnam)
{
  int len;
  char base[40];

  savescr();

  system("dir /w images");
  printf("\n\n\nEnter the base file name: ");
  fgets(base,40,stdin);

  len = strlen(base);
  if (isspace(base[len-1]) ) len--;
  base[len]='\0';

  strcpy(filnam,root);
  strcat(filnam,base);
  printf("\n using '%s'\n\n",filnam);
};


void readblk()
{
  int i,j;
  char ch;


  getname(filnam);
  infil = fopen(filnam,"r");
  fscanf(infil,"%d %d%c",&i,&j,&ch);


  fclose(infil);
  restorescr();

    blockon=2;
    blockX=1-i;
    blockY=1-j;
  onblk();
};







/* doesn't use param that is passed */
int fillit()
{
  return(1);
};
void filllf()
{
};

void fillblk()
{
  eachblk(fillit,filllf);
};






/* doesn't use param that is passed */
int emptyit()
{
  return(0);
};
void emptylf()
{
};

void emptyblk()
{
  eachblk(emptyit,emptylf);
};






int revit(int it)
{
  return(!it);
};
void revlf()
{
};

void reverseblk()
{
  eachblk(revit,revlf);
};







void enterblk()
{
  int i,j;
  char ch;

  infil = fopen(filnam,"r");
  fscanf(infil,"%d %d%c",&i,&j,&ch);

    blockon=2;
    blockX=1-i;
    blockY=1-j;


  eachblk(readit,readlf);
  fclose(infil);

};



void writeblk()
{
  char filnam[80];
  int x,y;
  int ex, ey;
  int i,j;
  int bX,bY;

  if (blockon==1) {
    bX=blockX;
    bY=blockY;
  };
  if (blockon==2) {
    bX=oldX-blockX;
    bY=oldY-blockY;
  };



    x=bX;
    y=bY;

    ex=oldX;
    ey=oldY;

    if (ex<x) swapint(ex,x);
    if (ey<y) swapint(ey,y);

    i=ex-x+1;
    j=ey-y+1;



  getname(filnam);
  outfil = fopen(filnam,"w");
  fprintf(outfil,"%d %d\n",i,j);

  eachblk(writeit,writelf);
  fclose(outfil);

  restorescr();
};










/* turn the block on or off */
void putblk(col)
 int col;
{
  int x,y;
  int ex, ey;
  int bX,bY;


  if (blockon==1) {
    bX=blockX;
    bY=blockY;
  };
  if (blockon==2) {
    bX=oldX-blockX;
    bY=oldY-blockY;
  };

  if (blockon) {
    x=bX*SIZE+SIZE+1+COLOFF-2;
    y=bY*SIZE+SIZE+1+ROWOFF	-2;

    ex=oldX*SIZE+SIZE+1+COLOFF -2;
    ey=oldY*SIZE+SIZE+1+ROWOFF	    -2;

    if (ex<x) swapint(ex,x);
    if (ey<y) swapint(ey,y);

    setcolor(col);
    rectangle(x,y,ex+SIZE,ey+SIZE);
  };
};

void onblk()
{
  putblk(15);
};

void offblk()
{
  putblk(00);
};



void rim()
{
  int x1,y1;
  int x2,y2;

  x1=8*COLS *SIZE+SIZE+1+COLOFF-2;
  y1=ROWS   *SIZE+SIZE+1+ROWOFF     -2;

  x2=		  SIZE+1+COLOFF-2;
  y2=		  SIZE+1+ROWOFF     -2;

  setcolor(15);
  rectangle(x1+1,y1+1,x2-1,y2-1);
};



void cleardat()
{
  int i,j;


  offblk();
  blockon=0;

  clearviewport();
  rim();

  for(i=0;i<COLS;i=i+2){
    for(j=0;j<ROWS;j=j+3){
      OLD_DATA[i][j] = 0;
      NEW_DATA[i][j] = 0;
    };
  };
};



void randdata()
{
  int i,j;
  for(i=0;i<COLS;i=i+1){		
    for(j=0;j<ROWS;j=j+1){
      OLD_DATA[i][j] = (rand() % 256)& 0xFF;
      NEW_DATA[i][j] = OLD_DATA[i][j] ^ 0xFF;
    };
  };
};







void neighbor(funct)
  int funct(int);
{
  int row_before, row_here, row_after;
  int col_here, shiftsize;
  int bits_before, bits_here, bits_after;
  int col_mod, bitpos;
  int mask;
  int newmod;	/* value of current row,col */

  row_before = ROWS-1;
  row_here = 0;
  row_after = 1;

nextrow:
  /* load up first 9 bits to be the last 1 and the leftmost 8 */
  bits_before = ( (OLD_DATA[0][row_before]) <<1) | ( 1&( (OLD_DATA[COLS-1][row_before])>>7 ) );
  bits_here   = ( (OLD_DATA[0][row_here]) <<1)   | ( 1&( (OLD_DATA[COLS-1][row_here])>>7	 ) );
  bits_after  = ( (OLD_DATA[0][row_after]) <<1)  | ( 1&( (OLD_DATA[COLS-1][row_after])>>7  ) );


  col_here = 0; /* where we just read a byte */
  shiftsize = 9;

  col_mod  = 0; /* where we are changing bits */
  bitpos = 1;
  newmod = OLD_DATA[col_here][row_here];


innerloop:
  mask = ((7&bits_before)<<6) | ((7&bits_here)<<3) | (7&bits_after);
  if (funct(mask)) {
    newmod = newmod ^ bitpos; /* xor */
  };


  bits_before = bits_before/2;
  bits_here   = bits_here/2;
  bits_after  = bits_after/2;
  shiftsize = shiftsize-1;
  bitpos = bitpos<<1;


  if (bitpos==256) {
    bitpos=1;
    NEW_DATA[col_mod][row_here] = newmod;
    col_mod++;
    if (col_mod==COLS) {
      goto quitinner;
    };
    newmod = OLD_DATA[col_mod][row_here];
  };

  if (shiftsize == 3) {
    shiftsize = 11; /* shiftsize+8 = 3+8 = 11 */

    col_here++;
    if (col_here==COLS) col_here=0;

    bits_before = bits_before | (OLD_DATA[col_here][row_before]<<3);
    bits_here	= bits_here   | (OLD_DATA[col_here][row_here]<<3);
    bits_after	= bits_after  | (OLD_DATA[col_here][row_after]<<3);
  };

  goto innerloop;


quitinner:
  row_before++;
  row_here++;
  row_after++;
  if (row_before==ROWS) row_before=0;
  if (row_after==ROWS)	row_after=0;
  if (row_here!=ROWS)  goto nextrow;

};









/* turn cursor pointer on and off */
void putptr(col)
  int col;
{
  int x,y;
  x=oldX*SIZE+SIZE+1+COLOFF;
  y=oldY*SIZE+SIZE+1+ROWOFF;

  putpixel(x,y,col);
  putpixel(x+1,y+1,col);
  putpixel(x+1,y,col);
  putpixel(x,y+1,col);
};

void onptr()
{
  putptr(15);
};

void offptr()
{
  putptr(00);
};




/* update editing pointer */
void updpointer()
{
  static period=0;

  if( (oldX==curX) && (oldY==curY) ){
    period = (period+1) % 64;
    if (period & 32) {
      offptr();
    }else{
      onptr();
    };
  }else{

    offptr();
    offblk();

    curX=(curX+COLS*8) % (COLS*8);
    curY=(curY+ROWS) % ROWS;
    oldX=curX;
    oldY=curY;
    onptr();
    onblk();
  };

};





void eachunit(funct)
  void funct(int old, int new);
{
  int row_here;
  int bits_old;
  int bits_new;
  int col_mod, bitpos;

  row_here = 0;

nextrow:
  col_mod  = 0; /* where we are checking bits */


  bits_old = OLD_DATA[col_mod][row_here];
  bits_new = NEW_DATA[col_mod][row_here];
  bitpos = 1;


innerloop:
  funct(bitpos & bits_old, bitpos & bits_new);
  bitpos = bitpos<<1;

  if (bitpos==256) {
    /* remember that the screen has been updated for this byte */
    OLD_DATA[col_mod][row_here] = NEW_DATA[col_mod][row_here];

    bitpos=1;
    col_mod++;
    if (col_mod==COLS) {
      goto quitinner;
    };
    bits_old = OLD_DATA[col_mod][row_here];
    bits_new = NEW_DATA[col_mod][row_here];
  };

  goto innerloop;


quitinner:
  updpointer();
  row_here++;
  if (row_here!=ROWS)  goto nextrow;

};






void turnon(x,y)
  int x,y;
{
  /* if (! getpixel(x,y) ){ */
    setcolor(15);
    rectangle(x,y,x+SIZE-2,y+SIZE-2);
  /* }; */
};

void turnoff(x,y)
  int x,y;
{
  /* if (getpixel(x,y) ){ */
    setcolor(00);
    rectangle(x,y,x+SIZE-2,y+SIZE-2);
  /* }; */
};



  static dump_col;
  static dump_row;
int dump(int old, int new)
{
  dump_col+=SIZE;
  if (dump_col==COLS*8*SIZE+SIZE+COLOFF) {
    dump_col=SIZE+COLOFF;
    dump_row+=SIZE;
  };

  if (old != new) {
    if (new) {
      turnon(dump_col,dump_row);
    }else{
      turnoff(dump_col,dump_row);
    };
  };

  return(0);
};


void display()
{
  dump_col=COLOFF;
  dump_row=SIZE+ROWOFF;
  eachunit(dump);
};





int change(int dat)
{
  int ct=0; /* count */

  if ((dat&0x100)!=0) ct++;
  if ((dat&0x80)!=0) ct++;
  if ((dat&0x40)!=0) ct++;

  if ((dat&0x20)!=0) ct++;
  if ((dat&0x08)!=0) ct++;

  if ((dat&0x04)!=0) ct++;
  if ((dat&0x02)!=0) ct++;
  if ((dat&0x01)!=0) ct++;

  if ( (dat&0x10)!=0 ) {      /* if currently a 1 */
    if (ct<2) return(1);
    if (ct>3) return(1);
  }else{		    /* if currently a 0 */
    if (ct==3) return(1);
  };
  return(0);
};


void generation()
{
  neighbor(change);
};



/* routine to setup flags to set or clear the cursor's bit */
void m_celltoggle()
{
  int x,y;
  int mask;

  x=oldX /8;
  mask = 1 << (oldX%8);
  y=oldY;

    NEW_DATA[x][y] = NEW_DATA[x][y] ^ mask;
    if( (OLD_DATA[x][y] & mask) == (NEW_DATA[x][y] & mask) ) {
      OLD_DATA[x][y] = OLD_DATA[x][y] ^ mask;
    };
};




static int steps=-1;
static int mod;	/* flag for screen update */




void m_beginblk()
{
   if (blockon){
     offblk();
     blockon=1; /* change from 2 to 1 */
   };
   blockX=curX;
   blockY=curY;
   blockon=1-blockon; /* invert 1 or 2 <--> 0 */
};




void m_unblock()
{

   if (blockon) offblk();
   blockon=0;
   /*
   curX=blockX;     this doesn't work for blockon==2
   curY=blockY;
   */
};

void m_pause()
{
   steps=0;
};

void m_run()
{
  steps=-1;
};


void m_step()
{
  steps=1;
};


void m_fill()
{
   mod=1;
   fillblk();
};


void m_empty()
{
   mod=1;
   emptyblk();
};

void m_reverse()
{
   mod=1;
   reverseblk();
};


/* enter the 'read' file name's data into the cell plane */
void m_usefile()
{
  mod=1;
  enterblk();
};

void m_readdisk()
{
  if (blockon) offblk();
  mod=1;
  readblk();
};


void m_writedisk()
{
  if (blockon) offblk();
   mod=1;
   writeblk();
};

void m_clear()
{
   cleardat();
};


void m_seed()
{
   mod=1;
   cleardat();
   randomize();
   randdata();
};

void m_quit()
{
   closegraph();
   exit(256);
};

void m_go_up()
{
  curY=curY-1;
};

void m_go_down()
{
  curY=curY+1;
};

void m_go_left()
{
  curX=curX-1;
};

void m_go_right()
{
  curX=curX+1;
};







void m_exit()
{    
  closegraph();
  exit(0);
};

void mystep()
{
  if (steps) {
    temp_ptr = old_ptr;
    old_ptr = new_ptr;
    new_ptr = temp_ptr;
    temp_ptr = 0;
    generation();
    if (steps != -1) steps--;
  };
  display();
  updpointer();

};



void user_menus()
{


#define RIGHT 1
#define LEFT 2
#define UP 3
#define DOWN 4
#define QUITTER 5


  hot_key(RIGHT,m_go_right);
  hot_key(LEFT,m_go_left);
  hot_key(UP,m_go_up);
  hot_key(DOWN,m_go_down);
  hot_key(QUITTER,m_exit);

  def_fkey(RIGHT,     "[SHIFT-ARROW-RIGHT]",   "<RIGHT>");
  def_fkey(LEFT,      "[SHIFT-ARROW-LEFT]",    "<LEFT>");
  def_fkey(UP,        "[SHIFT-ARROW-UP]",      "<UP>");
  def_fkey(DOWN,      "[SHIFT-ARROW-DOWN]",    "<DOWN>");
  def_fkey(QUITTER,   "[ALT-Q]",               "<QUIT>");

  wait_key(updpointer); /* FLASH GRAPHICS CURSOR WHEN WAITING FOR KEYS */


  /* the 'root' menu defaults to horizontal */
  def_submenu('Q',"Quit","  ARE YOU SURE YOU WANT TO QUIT?  ");
    def_entry('Y',"Yes",m_exit);
    def_entry('N',"No",nofn);
  endef_submenu("Quit");

  def_submenu('B',"Block","--cell block menu--"); /* vertical */

    def_entry('B',   "Begin block",     m_beginblk);
    def_entry('U',   "Unbegin block",   m_unblock);

    def_submenu('D',"Disk","-------Blocks-------");
      def_entry('W',   "Write box to disk",        m_writedisk);
      def_entry('R',   "Read stamp from disk",     m_readdisk);
      def_entry('S',   "Stamp onto cell plane",    m_usefile);
    endef_submenu("Disk");         menu_horizontal();
   /* build this 'disk' menu horizontal -- the others default to vertical */


    def_entry('F',   "Fill block",            m_fill);
    def_entry('E',   "Empty block",           m_empty);
    def_entry('R',   "Reverse cells",         m_reverse);

  endef_submenu("Block");  

  def_entry('P',   "Pause",             m_pause);
  def_entry('S',   "Step",              m_step);
  def_entry('R',   "Run",               m_run);
  def_entry('S',   "Seed the cell plane",   m_seed);
 
  def_submenu('C',"Cell menu","---Cell manipulations---");
    def_entry('C',   "Cell value toggle", m_celltoggle);
    def_entry('R',   "Move cursor right", m_go_right);
    def_entry('L',   "Move cursor left",  m_go_left);
    def_entry('U',   "Move cursor up",    m_go_up);
    def_entry('D',   "Move cursor down",  m_go_down);
  endef_submenu("Cell menu");



};






void main()
{
  long int menukey,helpkey;


  initgr();	/* init graphics */
  menu_init();

  cleardat();
  randomize();
/*  randdata(); */

  boxtext(0,0,"Press '?' for help.");

  menukey = lkeyof('/');
  helpkey = lkeyof('?');


  while (1) {
    menu_run();
    while (bioskey(1)) {
      menu_run();
    };

/* want this any more ? */
    if (mod && (steps!=0)) {
      display();
      mod=0;
    };

    do {
      while( !keyready() ){
        mystep();

      };  
      call_for_key(rootmenu);
    } while (  (menukey != the_key)  & (helpkey != the_key)  );

  };


  m_exit();
};


