#include "graphics.h"
#include "conio.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"
#include "string.h"


unsigned size;
float z1, a1, a2, a3;
int driver = DETECT, mode;
int b, c, x, y, x1, y1, x2, y2, x3, y3, r1, r2, r3, windows[6][5];
char k, *wintext[6][11], *head[5];
void *window_1, *st_window_1, *window_2, *st_window_2;
void *window_3, *st_window_3, *window_4, *st_window_4;
void *window_5, *st_window_5, *inv_head, *inv_text, *clear_screen;


int fnpx(int c)
{
  int pixels;

  pixels = (c - 1) * 8 + 1;
  return(pixels);
}


void calculations()
{
  x1 = fnpx(windows[x][1]) - 3;
  y1 = fnpx(windows[x][2]) - 3;
  x2 = fnpx(windows[x][1] + windows[x][3] - 1) + 7;
  y2 = fnpx(windows[x][2] + windows[x][4] - 1) + 7;
  x3 = fnpx(windows[x][1] + windows[x][3] - 1) + 5;
  y3 = fnpx(windows[x][2]) + 5;
}


void initialization_routine()
{
  gotoxy(1,22);
  printf("%s","INITIALIZING...");

  head[1] = "ITEM";
  windows[1][1] = 5;
  windows[1][2] = 3;
  windows[1][3] = 11;
  windows[1][4] = 3;
  wintext[1][1] = "ORDER ENTRY";
  wintext[1][2] = "INVENTORY";
  wintext[1][3] = "PAYROLL";

  head[2] = "CASH";
  windows[2][1] = 10;
  windows[2][2] = 3;
  windows[2][3] = 19;
  windows[2][4] = 4;
  wintext[2][1] = "ACCOUNTS PAYABLE";
  wintext[2][2] = "ACCOUNTS RECEIVABLE";
  wintext[2][3] = "GENERAL LEDGER";
  wintext[2][4] = "SYSTEM SECURITY";

  head[3] = "PROG";
  windows[3][1] = 15;
  windows[3][2] = 3;
  windows[3][3] = 10;
  windows[3][4] = 10;
  wintext[3][1] = " PROGRAM 1";
  wintext[3][2] = "*PROGRAM 2";
  wintext[3][3] = " PROGRAM 3";
  wintext[3][4] = " PROGRAM 4";
  wintext[3][5] = "*PROGRAM 5";
  wintext[3][6] = "*PROGRAM 6";
  wintext[3][7] = " PROGRAM 7";
  wintext[3][8] = " PROGRAM 8";
  wintext[3][9] = " PROGRAM 9";
  wintext[3][10] = " PROGRAM A";

  head[4] = "EXIT";
  windows[4][1] = 20;
  windows[4][2] = 3;
  windows[4][3] = 4;
  windows[4][4] = 2;
  wintext[4][1] = "REDO";
  wintext[4][2] = "EXIT";

  head[5] = "PAGE";
  windows[5][1] = 25;
  windows[5][2] = 3;
  windows[5][3] = 10;
  windows[5][4] = 8;
  wintext[5][1] = "SET UP";
  wintext[5][2] = "OPEN PAGE";
  wintext[5][3] = "CLOSE PAGE";
  wintext[5][4] = "PRINT PAGE";
  wintext[5][5] = "SAVE PAGE";
  wintext[5][6] = "MOVE PAGE";
  wintext[5][7] = "EDIT PAGE";
  wintext[5][8] = "PURGE PAGE";

  for (x = 1; x <= 5; x++) {
    setgraphmode(CGAC3);
    calculations();
    setcolor(15);
    rectangle(x1, y1, x2, y2);
    for (y = 1; y <= windows[x][4]; y++) {
      gotoxy(windows[x][1], windows[x][2] + y - 1);
      printf("%s",wintext[x][y]);
      if (y == windows[x][4])
	break;
    }
    switch(x) {
      case 1 :
	size = imagesize(x1, y1, x2, y2);
	window_1 = malloc(size);
	getimage(x1, y1, x2, y2, window_1);
        break;
      case 2 :
	size = imagesize(x1, y1, x2, y2);
	window_2 = malloc(size);
	getimage(x1, y1, x2, y2, window_2);
	break;
      case 3 :
	size = imagesize(x1, y1, x2, y2);
	window_3 = malloc(size);
	getimage(x1, y1, x2, y2, window_3);
	break;
      case 4 :
	size = imagesize(x1, y1, x2, y2);
	window_4 = malloc(size);
	getimage(x1, y1, x2, y2, window_4);
	break;
      case 5 :
	size = imagesize(x1, y1, x2, y2);
	window_5 = malloc(size);
	getimage(x1, y1, x2, y2, window_5);
	break;
    }
  }
  setgraphmode(CGAC3);
  setfillstyle(SOLID_FILL, 15);
  setcolor(15);
  rectangle(100, 100, 150, 120);
  floodfill(102, 102, 15);
  size = imagesize(100, 100, 133, 107);
  inv_head = malloc(size);
  getimage(100, 100, 133, 107, inv_head);
}


void get_graphics_under_a_window()
{
  calculations();
  switch(x) {
    case 1 :
      size = imagesize(x1, y1, x2, y2);
      st_window_1 = malloc(size);
      getimage(x1, y1, x2, y2, st_window_1);
      break;
    case 2 :
      size = imagesize(x1, y1, x2, y2);
      st_window_2 = malloc(size);
      getimage(x1, y1, x2, y2, st_window_2);
      break;
    case 3 :
      size = imagesize(x1, y1, x2, y2);
      st_window_3 = malloc(size);
      getimage(x1, y1, x2, y2, st_window_3);
      break;
    case 4 :
      size = imagesize(x1, y1, x2, y2);
      st_window_4 = malloc(size);
      getimage(x1, y1, x2, y2, st_window_4);
      break;
    case 5 :
      size = imagesize(x1, y1, x2, y2);
      st_window_5 = malloc(size);
      getimage(x1, y1, x2, y2, st_window_5);
      break;
  }
  for (b = y1; b <= y3; b++) {
    setcolor(0);
    line (x1, b, x3, b);
  }
  free(inv_text);
  setcolor(15);
  setfillstyle(SOLID_FILL,15);
  rectangle(x1, y1, x2, y2);
  rectangle(x1, y1, x3, y3);
  floodfill(x1 + 2, y1 + 2, 15);
  size = imagesize(x1, y1, x3, y3);
  inv_text = malloc(size);
  getimage(x1, y1, x3, y3, inv_text);
}


void put_menu_window_on_the_screen()
{
  calculations();
  switch(x) {
    case 1 :
      putimage(x1, y1, window_1, COPY_PUT);
      break;
    case 2 :
      putimage(x1, y1, window_2, COPY_PUT);
      break;
    case 3 :
      putimage(x1, y1, window_3, COPY_PUT);
      break;
    case 4 :
      putimage(x1, y1, window_4, COPY_PUT);
      break;
    case 5 :
      putimage(x1, y1, window_5, COPY_PUT);
      break;
  }
}


void put_graphics_back_on_screen()
{
  calculations();
  switch(x) {
    case 1 :
      putimage(x1, y1, st_window_1, COPY_PUT);
      free(st_window_1);
      break;
    case 2 :
      putimage(x1, y1, st_window_2, COPY_PUT);
      free(st_window_2);
      break;
    case 3 :
      putimage(x1, y1, st_window_3, COPY_PUT);
      free(st_window_3);
      break;
    case 4 :
      putimage(x1, y1, st_window_4, COPY_PUT);
      free(st_window_4);
      break;
    case 5 :
      putimage(x1, y1, st_window_5, COPY_PUT);
      free(st_window_5);
      break;
  }
}


void inverse_character_routine_i()
{
  int icr1, icr2;

  icr1 = fnpx(windows[x][1]) - 2;
  icr2 = fnpx(windows[x][2]) - 2 + (y - 1) * 8;
  putimage (icr1, icr2, inv_text, XOR_PUT);
}


void inverse_character_routine_ii()
{
  putimage(fnpx(windows[x][1]) - 2, 0, inv_head, XOR_PUT);
}


main()
{
  initgraph(&driver, &mode, "");
  setgraphmode(CGAC3);
  setcolor(15);
  randomize();
  initialization_routine();
begin:
  randomize();
  setgraphmode(CGAC3);
  for (x = 1; x <= 5; x++) {
    gotoxy(x * 5, 1);
    printf("%s",head[x]);
  }
  r1 = random(20) + 5;
  r2 = random(20) + 5;
  r3 = random(30) + 10;
  for (z1 = 12; z1 <= 199; z1+=.5) {
    a1 = 160 + sin(z1 / r1) * 100;
    a2 = abs(cos(z1 / r3)) * 639;
    a3 = sin(z1 / r2) * 80 + 100;
    line (a1, z1, a2, a3);
  }
  x = 1;
  inverse_character_routine_ii();
  get_graphics_under_a_window();
  put_menu_window_on_the_screen();
  y = 1;
  inverse_character_routine_i();
  do {
    k = getch();
    gotoxy(1,1);
    switch(k) {
      case '\r' :
	if (x == 4)
	  if (y == 1)
	    goto begin;
	  else
	    exit(1);
      case '\x1B' :
	closegraph();
	clrscr();
	gotoxy(30,11);
	printf("%s","THAT'S ALL FOLKS....");
	exit(1);
      case '\x48' :
	inverse_character_routine_i();
	y-=1;
        if (y < 1)
	  y = windows[x][4];
	inverse_character_routine_i();
	break;
      case '\x50' :
	inverse_character_routine_i();
	y+=1;
	if (y > windows[x][4])
	  y = 1;
	inverse_character_routine_i();
	break;
      case '\x4D' :
	put_graphics_back_on_screen();
	inverse_character_routine_ii();
	x+=1;
	if (x > 5)
	  x = 1;
	inverse_character_routine_ii();
	get_graphics_under_a_window();
	put_menu_window_on_the_screen();
	y = 1;
	inverse_character_routine_i();
	break;
      case '\x4B' :
	put_graphics_back_on_screen();
	inverse_character_routine_ii();
	x-=1;
	if (x < 1)
	  x = 5;
	inverse_character_routine_ii();
	get_graphics_under_a_window();
	put_menu_window_on_the_screen();
	y = 1;
	inverse_character_routine_i();
	break;
    }
  } while (k != 'n');
  closegraph();
}