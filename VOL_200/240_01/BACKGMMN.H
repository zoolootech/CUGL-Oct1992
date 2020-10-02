/*
** backgmmn.h - includes, defines, globals, declarations
**              David C. Oshel, September 8, 1986
*/

#include <setjmp.h>
#include <string.h>
#include "ciao.h"

#define on_cursor     defcursor             /* ciao aliases */
#define off_cursor    hidecursor
#define clr_screen    clrscrn
#define clr_eol       clreol
#define blanks(N)     rptchar(' ',N)

#define TRUE 1
#define FALSE 0
#define ME 1
#define YU 2
#define YRBAR 0
#define MYBAR 25
#define YRHOME 26
#define MYHOME 27
#define ERROR 999  /* anything well out of range */
#define MYLEVEL 2


/* Global Data, in bg0.obj */

struct board {
       int stones,  /* number of stones on that point */
           owner,   /* and whose they are */
           x,y,          /* x and y coords of point base */
           lastx, lasty, /* unused */
           cx, cy;       /* coordinates for column numbers */
        };
struct dstruct { 
       int cube, 
           whosecube; 
       };
struct pstruct {
       int fr,
           to,
           flag;
       };
extern struct pstruct pending;
extern struct dstruct doubles;
extern struct board point[28], bdsave[28];

extern jmp_buf mark;

extern int list[2][28];
extern int dice[2], myscore, yrscore, player, movesleft, cantuse, myturns,
           swapped, tswap, deciding, expert, show, moremsgline, level, 
           tokenstyle, firstmove, helpdisabled, yrdice, lookforit, 
           startcubevalue;

extern char *bgversion;
extern char *logoname;
extern char *backtalk[];
extern char *token1, *token2;
extern char chatter[80], buzzard[80]; /* window[80]; (obsolete) */


/* public functions in bg1.obj */


extern unsigned int acg(void);

extern int contains(char *,char *);

extern void voice(int );
extern void beep(void);

extern char *get2tkn(void);
extern char *get1tkn(void);
extern char *get3tkn(void);
extern char *get4tkn(void);
extern char *get6tkn(void);
extern char *get5tkn(void);
extern char *get7tkn(void);
extern char *get8tkn(void);

extern void title(void);

extern char *chooseplayer(void);


/* public functions in bg2.c */


extern void msg(int ,int ,char *,...);
extern void setchat(char *);

extern int play(void );
extern int arrange(void );
extern int reverse(void );
extern int update(void );
extern int setup(void );

extern int haltgame(void );
extern int jumpjack(void);

extern void draw_board(char *);
extern int newboard(void );
extern int swaptokens(void);
extern int lurch(int ,int ,int );
extern int topstone(int );
extern void clrpend(void );
extern int bearoff(void );
extern int endgame(void );
extern int nomove(void );
extern void restoreboard(void );

extern int mytotal(void );
extern int yrtotal(void );
extern int putscore(void );
extern int hint(void );


/* public functions in bg3.obj */


extern int mymove(void );


/* eof: backgmmn.h */

