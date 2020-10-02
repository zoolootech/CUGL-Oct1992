/*
   HEADER:       ;
   TITLE:        Include File for Text Window Support Functions;
   DATE:         07/17/1990
   SYSTEM:       MS-DOS;
   FILENAME:     WGCONIO.H;
   SEE-ALSO:     wgconio.c, wgconio.lib, sample.c, sample.exe;
   AUTHORS:      Bill Giel;
*/


/* Macros for scroll_window and wg_scroll functions*/

#define UP 6
#define DOWN 7
#define SHADOW 1
#define NO_SHADOW 0


/*Colors for EGA/VGA*/

enum colors { BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHTGRAY,
	      DARKGRAY, LIGHTBLUE, LIGHTGREEN, LIGHTCYAN, LIGHTRED,
	      LIGHTMAGENTA, YELLOW, WHITE};
#define BLINK 128


/*Structure to hold textscreen info*/

struct wgtext_info{
       unsigned char winleft, wintop, winright, winbottom, attribute,
		currmode, screenheight, screenwidth, curx,
		cury, foreground, background;
};


/*Structure to hold video mode parameters used by the system*/

struct wgvid_parameters{
	unsigned char textline,textchar;
	char far *vid_mem;
};


/*Union to hold one or two byte keycode*/

union keycode{
	char ch[2];
	int i;
};


/*Structure for Current Window Parameters*/

struct wgwind{
	unsigned char foreborder,backborder,foretitle,backtitle,
		foreground,background,leftlim,toplim,rightlim,bottomlim;
};

#ifdef MAIN_MODULE
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN struct wgtext_info WGW;
EXTERN struct wgvid_parameters WGV;
EXTERN union keycode WGC;

#ifdef MAIN_MODULE
int wg_directvideo=1;
#else
extern int wg_directvideo;
#endif


/*Prototypes*/

void wgwindow		(int left,int top,int right,int bottom);
void wgtextcolor	(int color);
void wgtextbackground	(int color);
void wgtextattr		(int attribute);
void goto_xy		(int x,int y);
void wggotoxy		(int x,int y);
void wgclrscr		(void);
void wgclreol		(void);
int  wggettext		(int left,int top,int right,int bottom,char *buf);
int  wgputtext		(int left,int top,int right,int bottom,char *buf);
int  wgcputs		(char *string);
int  wgputch		(int c);
int  wgwherex		(void);
int  wgwherey		(void);
void wggettextinfo	(struct wgtext_info *inforec);
void initiallize_WGW	(void);
int  get_video_mode	(void);
void get_cursor_pos	(unsigned char *x,unsigned char *y);
int  get_keycode	(void);
int  wggetch		(void);
int  wggetche		(void);
void get_cursor_size	(unsigned char *ch,unsigned char *cl);
int  wgmovetext		(int left,int top,int right,int bottom,
				int newleft,int newtop);
void scroll_window	(unsigned char direct,unsigned char lines,
				unsigned char top,unsigned char left,
				unsigned char bottom,unsigned char right,
				unsigned char attribute);
void wg_scroll		(unsigned char direction);
void box		(int width,int height);
void box2		(int width,int height);
char *make_window	(char *title,struct wgwind w,
				struct wgtext_info *t,int shadow);
void restore_screen	(struct wgwind w, char *text_buf,
				struct wgtext_info t, int shadow);
void cursor_off		(void);
void size_cursor	(unsigned char start,unsigned char end);
int  cdecl wgcprintf	(char *format, ...);
void get_attrib		(unsigned char *attribute,unsigned char *foreground,
						unsigned char *background);
void wgdelline		(void);
void wginsline		(void);
int make_shadow 	(int left, int top, int right, int bottom);
char *make_tsr_window	(char *title,struct wgwind w,
					struct wgtext_info *t,char *buf);
void restore_tsr_screen(struct wgwind w, char *text_buf,
					struct wgtext_info t);
